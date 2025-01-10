#!/bin/sh

# Define the source files and output executable name
SOURCE_FILES="main.c main_frame.c bookmarks.c keys.c main_menu.c server.c"
OUTPUT_NAME="browser"

# Function to check for available webkit2gtk version
check_webkit_version() {
    if pkg-config --exists webkit2gtk-4.1; then
        echo "webkit2gtk-4.1"
    elif pkg-config --exists webkit2gtk-4.0; then
        echo "webkit2gtk-4.0"
    else
        echo "No compatible webkit2gtk version found."
        exit 1
    fi
}

# Get the available webkit2gtk version
WEBKIT_VERSION=$(check_webkit_version)

# Run gcc with the specified options and capture all output
gcc -o "$OUTPUT_NAME" $SOURCE_FILES -Wall `pkg-config --cflags --libs gtk+-3.0 $WEBKIT_VERSION gstreamer-1.0` 2>&1

# Check if gcc succeeded
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable '$OUTPUT_NAME' created."
else
    echo "Compilation failed."
fi
