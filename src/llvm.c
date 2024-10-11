#include "llvm.h"
#include "log.h"
#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Transforms/PassBuilder.h>
#include <llvm-c/BitWriter.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PLATFORM_WINDOWS
#include <stdarg.h>
#include <direct.h>
#define MKDIR(path) _mkdir(path)
int vasprintf(char **strp, const char *fmt, va_list ap)
{
    int size = _vscprintf(fmt, ap);
    if (size == -1)
        return -1;
    char *str = (char *)malloc((size_t)size + 1);
    if (!str)
        return -1;
    int result = vsprintf(str, fmt, ap);
    if (result == -1)
    {
        free(str);
        return -1;
    }
    *strp = str;
    return result;
}

int asprintf(char **strp, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int size = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);

    if (size <= 0)
    {
        return -1;
    }
    *strp = (char *)malloc(size);
    if (*strp == NULL)
    {
        return -1;
    }

    va_start(args, fmt);
    int result = vsnprintf(*strp, size, fmt, args);
    va_end(args);

    return result;
}
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

LLVMModuleRef module;
LLVMBuilderRef builder;
LLVMContextRef context;

void initialize_llvm_target()
{
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    LLVMInitializeNativeDisassembler();
}

void ensure_build_directory_exists()
{
    const char *build_dir = "build";
    if (MKDIR(build_dir) == 0)
    {
        log_message(LOG_LEVEL_INFO, "Created directory: %s", build_dir);
    }
    else
    {
        log_message(LOG_LEVEL_TRACE, "Directory %s already exists or could not be created.", build_dir);
    }
}

void emit_executable(const char *output_name)
{
    ensure_build_directory_exists();
    LLVMTargetRef target;
    char *error = NULL;

    if (LLVMGetTargetFromTriple(LLVMGetDefaultTargetTriple(), &target, &error))
    {
        log_message(LOG_LEVEL_ERROR, "Failed to get target: %s", error);
        LLVMDisposeMessage(error);
        return;
    }
    LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(
        target,
        LLVMGetDefaultTargetTriple(),
        LLVMGetHostCPUName(),
        LLVMGetHostCPUFeatures(),
        LLVMCodeGenLevelDefault,
        LLVMRelocDefault,
        LLVMCodeModelDefault);
    char *object_output;
    asprintf(&object_output, "build/%s.o", output_name);

    if (LLVMTargetMachineEmitToFile(target_machine, module, object_output, LLVMObjectFile, &error))
    {
        log_message(LOG_LEVEL_ERROR, "Failed to emit object file: %s", error);
        LLVMDisposeMessage(error);
        free(object_output);
        return;
    }

    log_message(LOG_LEVEL_TRACE, "Object file generated: %s", object_output);

    char *link_command;
    asprintf(&link_command, "gcc build/%s.o -o build/%s", output_name, output_name);
    log_message(LOG_LEVEL_TRACE, "Linking with command: %s", link_command);

    int result = system(link_command);
    if (result != 0)
    {
        log_message(LOG_LEVEL_ERROR, "Error during linking. Command exited with code %d.", result);
    }
    else
    {
        log_message(LOG_LEVEL_INFO, "Executable generated: %s", output_name);
    }

    free(link_command);
    free(object_output);
    LLVMDisposeTargetMachine(target_machine);
}

LLVMValueRef codegen_return_statement(LLVMBuilderRef builder, ReturnASTNode *ret)
{
    LiteralASTNode *literal = (LiteralASTNode *)ret->value;
    log_message(LOG_LEVEL_TRACE, "Generating return statement with value: %d", literal->value);

    return LLVMConstInt(LLVMInt8TypeInContext(context), literal->value, 0);
}

LLVMValueRef codegen_function(FunctionASTNode *func)
{
    log_message(LOG_LEVEL_TRACE, "Generating function: %s", func->name);

    LLVMTypeRef return_type = LLVMInt8TypeInContext(context);
    LLVMTypeRef func_type = LLVMFunctionType(return_type, NULL, 0, 0);
    LLVMValueRef llvm_function = LLVMAddFunction(module, func->name, func_type);
    LLVMBasicBlockRef block = LLVMAppendBasicBlockInContext(context, llvm_function, "entry");
    LLVMPositionBuilderAtEnd(builder, block);

    LLVMValueRef return_value = codegen_return_statement(builder, (ReturnASTNode *)func->body);
    LLVMBuildRet(builder, return_value);
    LLVMVerifyFunction(llvm_function, LLVMAbortProcessAction);

    log_message(LOG_LEVEL_INFO, "Finished generating function: %s", func->name);
    return llvm_function;
}

void generate_code_from_ast(ASTNode *root_node, const char *output_name)
{
    log_message(LOG_LEVEL_TRACE, "Starting LLVM code generation...");

    context = LLVMContextCreate();
    module = LLVMModuleCreateWithNameInContext("jpp_module", context);
    builder = LLVMCreateBuilderInContext(context);

    initialize_llvm_target();
    if (root_node->type == AST_FUNCTION)
    {
        codegen_function((FunctionASTNode *)root_node);
    }
    LLVMDumpModule(module);

    emit_executable(output_name);

    log_message(LOG_LEVEL_INFO, "Successfully wrote to file: %s\n", output_name);

    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    LLVMContextDispose(context);
}
