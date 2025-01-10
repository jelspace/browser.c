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

# Check each required library
for LIB in $REQUIRED_LIBS; do
    # Check if the library is available
    if ldconfig -p | grep -i "$LIB" > /dev/null; then
        echo -e "Library ${GREEN}$LIB${NC} is ${GREEN}available${NC}."
    else
        echo -e "Library ${RED}$LIB${NC} is ${RED}NOT available${NC}."
    fi
done
