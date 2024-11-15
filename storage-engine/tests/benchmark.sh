#!/bin/bash

# Ensure the script is executable
# chmod +x benchmark.sh

strace -c -o strace_output.txt ./storage_engine_build_example

# Display the output
cat strace_output.txt