#!/bin/sh

# Define the source files and output executable name
SOURCE_FILES="main.c main_frame.c bookmarks.c keys.c main_menu.c server.c"
OUTPUT_NAME="browser"

# Run gcc with the specified options and capture all output
gcc -o "$OUTPUT_NAME" $SOURCE_FILES -Wall `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0 gstreamer-1.0` 2>&1

# Check if gcc succeeded
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable '$OUTPUT_NAME' created."
else
    echo "Compilation failed."
fi
