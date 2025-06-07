#!/bin/bash

# Define color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check if a binary path is provided as an argument
if [ -z "$1" ]; then
    echo "Usage: $0 /path/to/your/binary"
    exit 1
fi

# This script checks if the direct runtime dynamic libraries required by a given binary
# are found in common system library paths. It does not check for development packages
# needed for compilation.

# Path to the binary (passed as the first argument)
BINARY="$1"

# Check if the binary exists
if [ ! -f "$BINARY" ]; then
    echo -e "${RED}Error: Binary file '$BINARY' not found.${NC}"
    exit 1
fi

# Get the list of required libraries
# Filter out common non-file entries from ldd output like linux-vdso.so.1 or the dynamic linker itself.
# Also, ensure we only process lines that look like "name => /path/to/lib (0x...)" or "name => not found"
# The awk '/=>/ {print $1}' already handles the "name" part for found libraries.
# For "not found", ldd output is "libname.so.X => not found". $1 is still "libname.so.X".
# System dynamic linker (e.g. /lib64/ld-linux-x86-64.so.2) will also be listed by ldd.
# These are actual files, so they should be checked and found by check_library.
# The main one to filter is linux-vdso.so.1 which isn't a regular file on disk in these paths.
REQUIRED_LIBS=$(ldd "$BINARY" 2>/dev/null | grep '=>' | awk '{print $1}' | grep -v -e '^linux-vdso\.so' | sort -u)


if [ -z "$REQUIRED_LIBS" ]; then
    # This could happen if the binary is static, or ldd failed to parse it, or all libs were filtered.
    echo -e "${RED}Error: Unable to retrieve library dependencies for '$BINARY', or all were filtered (e.g., static binary or only linux-vdso).${NC}"
    # It's debatable if this should be an error. If it's a static binary, it has no dynamic libs.
    # For now, let's assume we expect dynamic libs.
    exit 1
fi

# Define library search paths for Linux and Termux
if [ -d "/data/data/com.termux/files/usr/lib" ]; then
    # Termux environment
    LIB_PATHS=("/data/data/com.termux/files/usr/lib")
else
    # Standard Linux environment
    LIB_PATHS=("/lib" "/lib64" "/usr/lib" "/usr/lib64" "/usr/local/lib" "/usr/lib/x86_64-linux-gnu" "/usr/lib/aarch64-linux-gnu")
fi

# Function to check if a library exists in the system
check_library() {
    local LIB="$1"
    for LIB_PATH in "${LIB_PATHS[@]}"; do
        # Check for exact library name (e.g., libc.so.6)
        if [ -f "$LIB_PATH/$LIB" ]; then
            return 0
        fi
        # Check for unversioned library name (e.g., libc.so)
        if [ -f "$LIB_PATH/$(echo "$LIB" | sed 's/\.so\..*/.so/')" ]; then
            return 0
        fi
    done
    return 1
}

# Overall status
ALL_LIBS_FOUND=true

# Check each required library
for LIB in $REQUIRED_LIBS; do
    if check_library "$LIB"; then
        echo -e "Library ${GREEN}$LIB${NC} is ${GREEN}available${NC}."
    else
        echo -e "Library ${RED}$LIB${NC} is ${RED}NOT available${NC}."
        ALL_LIBS_FOUND=false
    fi
done

if [ "$ALL_LIBS_FOUND" = true ]; then
    echo -e "\n${GREEN}All direct runtime libraries (excluding linux-vdso) required by $BINARY appear to be available in checked paths.${NC}"
    exit 0
else
    echo -e "\n${RED}Some direct runtime libraries required by $BINARY are NOT available in checked paths.${NC}"
    exit 1
fi
