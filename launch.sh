#!/bin/bash

# --- EAE Firmware Launcher ---

# 'set -e' tells the script to exit immediately if any command fails
set -e

# 1. Configuration
BUILD_DIR="build"
DEFAULT_SETPOINT=55
SETPOINT=${1:-$DEFAULT_SETPOINT}

echo "-----------------------------------------------"
echo "EAE Cooling System: Initializing Build..."
echo "Target Setpoint: $SETPOINT C"
echo "-----------------------------------------------"

# 2. Environment Check
if [[ "$MSYSTEM" != "MINGW64" ]]; then
  echo "[WARNING] You are not in the MinGW 64-bit terminal."
  echo "Build might fail. Please use MinGW 64-bit for best results."
fi

# 3. Create build directory
if [ ! -d "$BUILD_DIR" ]; then
  mkdir $BUILD_DIR
fi

# 4. Navigate to build folder
cd $BUILD_DIR

# 5. Run CMake
# If this fails, 'set -e' will stop the script here.
echo "Running CMake..."
cmake -G "Unix Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..

# 6. Compile
echo "Compiling..."
make

# 7. Launch Firmware
echo "-----------------------------------------------"
echo "Build Successful. Launching Firmware..."
echo "-----------------------------------------------"

# Use .exe extension for Windows/MSYS2 compatibility
#./firmware_app.exe $SETPOINT


echo ""
echo "Running Unit Tests"
echo ""
./run_tests.exe