#!/bin/bash

# Detect OS
OS="$(uname -s)"
LIB_PATH=$(pwd)
LIB_NAME="libft_malloc_$(uname -m)_$(uname -s).so" # Matches your Makefile name
SYMLINK="libft_malloc.so"

# Ensure the library exists
if [ ! -f "$SYMLINK" ]; then
    echo "Error: $SYMLINK not found. Did you run 'make'?"
    exit 1
fi

if [ "$OS" = "Linux" ]; then
    # --- LINUX ---
    # LD_PRELOAD forces the dynamic linker to load your lib BEFORE libc.
    export LD_PRELOAD=$LIB_PATH/$SYMLINK
    
    # Run the command passed as argument
    exec "$@"

elif [ "$OS" = "Darwin" ]; then
    # --- MACOS ---
    # DYLD_INSERT_LIBRARIES is the macOS equivalent of LD_PRELOAD
    export DYLD_INSERT_LIBRARIES=$LIB_PATH/$SYMLINK
    export DYLD_FORCE_FLAT_NAMESPACE=1
    
    # Run the command passed as argument
    exec "$@"

else
    echo "Unknown OS"
    exit 1
fi