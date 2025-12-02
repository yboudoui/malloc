#!/bin/bash

# Detect OS
OS="$(uname -s)"

# Get the directory where the script is located
LIB_PATH=$(pwd)

echo "Running test with custom malloc on $OS..."

if [ "$OS" = "Linux" ]; then
    # Linux configuration
    export LD_LIBRARY_PATH=$LIB_PATH:$LD_LIBRARY_PATH
    ./test_program $@
elif [ "$OS" = "Darwin" ]; then
    # macOS configuration
    export DYLD_LIBRARY_PATH=$LIB_PATH
    export DYLD_INSERT_LIBRARIES=$LIB_PATH/libft_malloc.so
    export DYLD_FORCE_FLAT_NAMESPACE=1
    ./test_program $@
else
    echo "Unknown OS"
fi