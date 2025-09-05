#!/bin/bash

# --- Check for gcc ---
if ! command -v gcc &> /dev/null; then
    echo "Error: gcc is not installed or not in PATH."
    exit 1
fi

echo "gcc found: $(gcc --version | head -n 1)"

# --- Compile sources ---
gcc -g -O0 \
    main_service.c \
    task.c \
    uart.c \
    tools.c \
    -static \
    -o KEIRO250803

# --- Check build result ---
if [ $? -eq 0 ]; then
    echo "Build successful: ./KEIRO250803"
else
    echo "Build failed."
    exit 1
fi