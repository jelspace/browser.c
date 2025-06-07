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
REQUIRED_LIBS=$(ldd "$BINARY" | awk '/=>/ {print $1}' | sort -u)

# Overall status
ALL_LIBS_FOUND=true

# Check each required library
for LIB in $REQUIRED_LIBS; do
    # Check if the library is available
    # Using grep -q for quieter operation, and ensuring whole word match for the lib name.
    # Example: if LIB is "libc.so.6", we want to match "libc.so.6" and not "libc.so.66"
    # The default output of ldd gives specific names, so direct grep is often okay.
    # ldconfig -p output format: libname.so.version (arch,abi) => /path/to/libname.so.version
    if ldconfig -p | grep -E "^\s*$LIB\s+\(" > /dev/null; then
        echo -e "Library ${GREEN}$LIB${NC} is ${GREEN}available${NC}."
    else
        echo -e "Library ${RED}$LIB${NC} is ${RED}NOT available${NC}."
        ALL_LIBS_FOUND=false
    fi
done

if [ "$ALL_LIBS_FOUND" = true ]; then
    echo -e "\n${GREEN}All direct runtime libraries required by $BINARY appear to be available.${NC}"
    exit 0
else
    echo -e "\n${RED}Some direct runtime libraries required by $BINARY are NOT available.${NC}"
    exit 1
fi
