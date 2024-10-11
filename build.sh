#!/bin/bash
if [ ! -d "build" ]; then
  echo "Creating build directory..."
  mkdir "build"
fi

cd "build" || exit

cmake ..

cmake --build .

echo "Build complete!"