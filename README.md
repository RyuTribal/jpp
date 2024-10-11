# JPP Compiler

## Table of Contents

- [Overview](#overview)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
  - [Windows](#windows)
  - [Linux](#linux)
- [Build Scripts](#build-scripts)

## Overview

JPP Compiler is a lightweight compiler for a custom language, leveraging the LLVM infrastructure. This repository contains all the necessary files to build and run the compiler on both Windows and Linux platforms.

## Requirements

### Windows

- [Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/vs/community/)
- CMake (can be installed with Visual Studio components)
- LLVM submodule (see instructions below)

### Linux

- CMake
- Clang
- GNU Make

## Getting Started

### Windows

1. **Clone the Repository**

   ```bash
   git clone https://github.com/RyuTribal/jpp_compiler.git
   ```

2. **Initialize the LLVM Submodule**
   After cloning the repository, you need to initialize and pull the `vendor/llvm` submodule:

   ```bash
   git submodule update --init --recursive vendor/llvm
   ```

3. **Run the Build Script**
   Use the provided `build.bat` script to generate and compile the project with Visual Studio:

   ```bash
   build.bat
   ```

   This script will create a `build` directory, configure the project using CMake, and compile it with Visual Studio's MSBuild.

### Linux

1. **Clone the Repository**

   ```bash
   git clone https://github.com/RyuTribal/jpp_compiler.git
   ```

2. **Initialize the LLVM Submodule**
   After cloning, initialize and pull the `vendor/llvm` submodule:

   ```bash
   git submodule update --init --recursive vendor/llvm
   ```

3. **Install Required Dependencies**
   Ensure you have `cmake`, `clang`, and `make` installed on your system. For Debian-based systems, run:

   ```bash
   sudo apt-get install cmake clang build-essential
   ```

4. **Run the Build Script**
   Use the provided `build.sh` script to configure and compile the project:

   ```bash
   ./build.sh
   ```

   The script will create a `build` directory, run `cmake`, and compile the project with `make`.

## Troubleshooting

### Windows

If you encounter issues during compilation, ensure you have the required dependencies installed, and Visual Studio is properly set up. Verify that CMake is installed and added to your system's PATH.

### Linux

If you encounter any issues while building, ensure that Clang and CMake are installed and up-to-date. Use the following command to verify the installation:

```bash
clang --version
cmake --version
```

## Using the Compiler

Once the jpp_compiler is compiled, you can use the jpp_compiler executable to compile any of the test programs in the **test_programs** folder.

Navigate to the **test_programs** folder Make sure you are inside the test_programs directory:

```
cd test_programs
```

Run the Compiler Use the following command to compile a .jpp file into an executable:

```
<path_to_jpp_compiler> <path_to_jpp_file> <name_of_resulting_executable>
```

For example:

```
../build/jpp_compiler hello_world.jpp hello_world
```

This command will create a **build/** directory, compile the **hello_world.jpp** file and generate an executable named **hello_world**.
