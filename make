gcc -o browser main.c main_frame.c bookmarks.c keys.c main_menu.c server.c -Wall `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0 gstreamer-1.0`
