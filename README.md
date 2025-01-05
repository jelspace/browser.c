# browser.c
coder:deepseek v3

www browser/webkit/ and local video server

access in any other web browser at:
localhost:8000

many things not work bookmarks,download yt video from html
but yt video download from 'browser' and then press button [download video] works

keys:
[ctrl]+t new tab
[ctrl]+w close tab
[ctrl]+r reload page


Tested on Linux 

screen from firefox
https://github.com/jelspace/browser.c/blob/main/Screenshot%20from%202025-01-05%2009-01-29.png

compile with:
gcc -o browser main.c main_frame.c bookmarks.c keys.c main_menu.c server.c -Wall `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0 gstreamer-1.0`





