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

# Path to the binary (passed as the first argument)
BINARY="$1"

# Check if the binary exists
if [ ! -f "$BINARY" ]; then
    echo -e "${RED}Error: Binary file '$BINARY' not found.${NC}"
    exit 1
fi

# Get the list of required libraries
REQUIRED_LIBS=$(ldd "$BINARY" 2>/dev/null | awk '/=>/ {print $1}' | sort -u)

if [ -z "$REQUIRED_LIBS" ]; then
    echo -e "${RED}Error: Unable to retrieve library dependencies for '$BINARY'.${NC}"
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

# Check each required library
for LIB in $REQUIRED_LIBS; do
    if check_library "$LIB"; then
        echo -e "Library ${GREEN}$LIB${NC} is ${GREEN}available${NC}."
    else
        echo -e "Library ${RED}$LIB${NC} is ${RED}NOT available${NC}."
    fi
done
