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


Tested on Linux , Termux with vncserver in Android

screen from firefox
https://github.com/jelspace/browser.c/blob/main/Screenshot%20from%202025-01-05%2009-01-29.png

type in terminal:
git clone https://github.com/jelspace/browser.c 

enter dir:
cd browser.c

change permisions of 'make' file and 'compile.sh' file to make them executable:
 chmod +x make  
 chmod +x compile.sh

compile with:
 ./compile.sh
 or
 ./make
 ("./make" is different than "make")

then create video dir:
mkdir videos

and run 'browser':
./browser

You can install missing libraries using your package manager (e.g., apt on Debian/Ubuntu, dnf on Fedora). For example:

If any of these libraries are missing or incompatible, the browser may fail to run. 

To compile and run the browser application from scratch on a newly installed Linux system, you need to install several dependencies. Below is a list of all the requirements and the steps to install them.
1. Update the System

Before installing any dependencies, ensure your system is up to date:
bash
Copy

sudo apt update
sudo apt upgrade -y

2. Install Build Tools

You need essential build tools like gcc, make, and pkg-config to compile the application:
bash
Copy

sudo apt install build-essential

3. Install GTK and WebKitGTK

The application uses GTK for the graphical interface and WebKitGTK for rendering web pages. Install the necessary libraries:
bash
Copy

sudo apt install libgtk-3-dev libwebkit2gtk-4.0-dev

4. Install GStreamer

GStreamer is used for multimedia support. Install the GStreamer development libraries:
bash
Copy

sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev

5. Install yt-dlp

yt-dlp is required for downloading YouTube videos. Install it using pip:
bash
Copy

sudo apt install python3-pip
pip3 install yt-dlp

6. Install pthread

The application uses POSIX threads (pthread) for the server. Install the development library:
bash
Copy

sudo apt install libpthread-stubs0-dev

7. Install libsoup

libsoup is used for HTTP client/server functionality. Install the development library:
bash
Copy

sudo apt install libsoup2.4-dev

8. Install json-glib

The application uses json-glib for JSON parsing. Install the development library:
bash
Copy

sudo apt install libjson-glib-dev

9. Install libssl-dev

If the application uses SSL/TLS for secure communication, install the OpenSSL development library:
bash
Copy

sudo apt install libssl-dev

10. Install libcurl

libcurl is used for making HTTP requests. Install the development library:
bash
Copy

sudo apt install libcurl4-openssl-dev

11. Install libgirepository1.0-dev

This library is required for GObject introspection, which is used by GTK and WebKitGTK:
bash
Copy

sudo apt install libgirepository1.0-dev

12. Install libnotify-dev

If the application uses desktop notifications, install the libnotify development library:
bash
Copy

sudo apt install libnotify-dev

13. Install libappindicator3-dev

For system tray integration, install the libappindicator development library:
bash
Copy

sudo apt install libappindicator3-dev

14. Install libsqlite3-dev

If the application uses SQLite for local storage, install the development library:
bash
Copy

sudo apt install libsqlite3-dev

15. Install libxml2-dev

libxml2 is used for XML parsing. Install the development library:
bash
Copy

sudo apt install libxml2-dev

16. Install libjpeg-dev

If the application handles JPEG images, install the development library:
bash
Copy

sudo apt install libjpeg-dev

17. Install libpng-dev

For PNG image support, install the development library:
bash
Copy

sudo apt install libpng-dev

18. Install libavcodec-dev and libavformat-dev

For multimedia support, install the FFmpeg development libraries:
bash
Copy

sudo apt install libavcodec-dev libavformat-dev

19. Install libpulse-dev

If the application uses PulseAudio for audio, install the development library:
bash
Copy

sudo apt install libpulse-dev

20. Install libgstreamer-plugins-bad1.0-dev

For additional GStreamer plugins, install the development library:
bash
Copy

sudo apt install libgstreamer-plugins-bad1.0-dev

21. Install libsoup2.4-dev

For HTTP client/server functionality, install the development library:
bash
Copy

sudo apt install libsoup2.4-dev

22. Install libsecret-1-dev

If the application uses secure storage for credentials, install the development library:
bash
Copy

sudo apt install libsecret-1-dev

23. Install libatk-bridge2.0-dev

For accessibility support, install the development library:
bash
Copy

sudo apt install libatk-bridge2.0-dev

24. Install libepoxy-dev

For OpenGL support, install the development library:
bash
Copy

sudo apt install libepoxy-dev

25. Install libwayland-dev

For Wayland support, install the development library:
bash
Copy

sudo apt install libwayland-dev

26. Install libxkbcommon-dev

For keyboard handling, install the development library:
bash
Copy

sudo apt install libxkbcommon-dev

27. Install libegl1-mesa-dev

For EGL support, install the development library:
bash
Copy

sudo apt install libegl1-mesa-dev

28. Install libgles2-mesa-dev

For OpenGL ES support, install the development library:
bash
Copy

sudo apt install libgles2-mesa-dev

29. Install libgl1-mesa-dev

For OpenGL support, install the development library:
bash
Copy

sudo apt install libgl1-mesa-dev

30. Install libgdk-pixbuf2.0-dev

For image loading and manipulation, install the development library:
bash
Copy

sudo apt install libgdk-pixbuf2.0-dev

31. Install libcairo2-dev

For 2D graphics rendering, install the development library:
bash
Copy

sudo apt install libcairo2-dev

32. Install libpango1.0-dev

For text rendering, install the development library:
bash
Copy

sudo apt install libpango1.0-dev

33. Install libharfbuzz-dev

For text shaping, install the development library:
bash
Copy

sudo apt install libharfbuzz-dev

34. Install libfreetype6-dev

For font rendering, install the development library:
bash
Copy

sudo apt install libfreetype6-dev

35. Install libicu-dev

For Unicode support, install the development library:
bash
Copy

sudo apt install libicu-dev

36. Install libenchant-2-dev

For spell checking, install the development library:
bash
Copy

sudo apt install libenchant-2-dev

37. Install libsecret-1-dev

For secure storage of credentials, install the development library:
bash
Copy

sudo apt install libsecret-1-dev

38. Install libsoup2.4-dev

For HTTP client/server functionality, install the development library:
bash
Copy

sudo apt install libsoup2.4-dev

39. Install libsqlite3-dev

For SQLite database support, install the development library:
bash
Copy

sudo apt install libsqlite3-dev

40. Install libxml2-dev

For XML parsing, install the development library:
bash
Copy

sudo apt install libxml2-dev

41. Install libjpeg-dev

For JPEG image support, install the development library:
bash
Copy

sudo apt install libjpeg-dev

42. Install libpng-dev

For PNG image support, install the development library:
bash
Copy

sudo apt install libpng-dev

43. Install libavcodec-dev and libavformat-dev

For multimedia support, install the FFmpeg development libraries:
bash
Copy

sudo apt install libavcodec-dev libavformat-dev

44. Install libpulse-dev

For PulseAudio support, install the development library:
bash
Copy

sudo apt install libpulse-dev

45. Install libgstreamer-plugins-bad1.0-dev

For additional GStreamer plugins, install the development library:
bash
Copy

sudo apt install libgstreamer-plugins-bad1.0-dev

46. Install libsoup2.4-dev

For HTTP client/server functionality, install the development library:
bash
Copy

sudo apt install libsoup2.4-dev

47. Install libsecret-1-dev

For secure storage of credentials, install the development library:
bash
Copy

sudo apt install libsecret-1-dev

48. Install libatk-bridge2.0-dev

For accessibility support, install the development library:
bash
Copy

sudo apt install libatk-bridge2.0-dev

49. Install libepoxy-dev

For OpenGL support, install the development library:
bash
Copy

sudo apt install libepoxy-dev

50. Install libwayland-dev

For Wayland support, install the development library:
bash
Copy

sudo apt install libwayland-dev

51. Install libxkbcommon-dev

For keyboard handling, install the development library:
bash
Copy

sudo apt install libxkbcommon-dev

52. Install libegl1-mesa-dev

For EGL support, install the development library:
bash
Copy

sudo apt install libegl1-mesa-dev

53. Install libgles2-mesa-dev

For OpenGL ES support, install the development library:
bash
Copy

sudo apt install libgles2-mesa-dev

54. Install libgl1-mesa-dev

For OpenGL support, install the development library:
bash
Copy

sudo apt install libgl1-mesa-dev

55. Install libgdk-pixbuf2.0-dev

For image loading and manipulation, install the development library:
bash
Copy

sudo apt install libgdk-pixbuf2.0-dev

56. Install libcairo2-dev

For 2D graphics rendering, install the development library:
bash
Copy

sudo apt install libcairo2-dev

57. Install libpango1.0-dev

For text rendering, install the development library:
bash
Copy

sudo apt install libpango1.0-dev

58. Install libharfbuzz-dev

For text shaping, install the development library:
bash
Copy

sudo apt install libharfbuzz-dev

59. Install libfreetype6-dev

For font rendering, install the development library:
bash
Copy

sudo apt install libfreetype6-dev

60. Install libicu-dev

For Unicode support, install the development library:
bash
Copy

sudo apt install libicu-dev

61. Install libenchant-2-dev

For spell checking, install the development library:
bash
Copy

sudo apt install libenchant-2-dev

62. Install libsecret-1-dev

For secure storage of credentials, install the development library:
bash
Copy

sudo apt install libsecret-1-dev

63. Install libsoup2.4-dev

For HTTP client/server functionality, install the development library:
bash
Copy

sudo apt install libsoup2.4-dev

64. Install libsqlite3-dev

For SQLite database support, install the development library:
bash
Copy

sudo apt install libsqlite3-dev

65. Install libxml2-dev

For XML parsing, install the development library:
bash
Copy

sudo apt install libxml2-dev

66. Install libjpeg-dev

For JPEG image support, install the development library:
bash
Copy

sudo apt install libjpeg-dev

67. Install libpng-dev

For PNG image support, install the development library:
bash
Copy

sudo apt install libpng-dev

68. Install libavcodec-dev and libavformat-dev

For multimedia support, install the FFmpeg development libraries:
bash
Copy

sudo apt install libavcodec-dev libavformat-dev

69. Install libpulse-dev

For PulseAudio support, install the development library:
bash
Copy

sudo apt install libpulse-dev

70. Install libgstreamer-plugins-bad1.0-dev

For additional GStreamer plugins, install the development library:
bash
Copy

sudo apt install libgstreamer-plugins-bad1.0-dev

71. Install libsoup2.4-dev

For HTTP client/server functionality, install the development library:
bash
Copy

sudo apt install libsoup2.4-dev

72. Install libsecret-1-dev

For secure storage of credentials, install the development library:
bash
Copy

sudo apt install libsecret-1-dev

73. Install libatk-bridge2.0-dev

For accessibility support, install the development library:
bash
Copy

sudo apt install libatk-bridge2.0-dev

74. Install libepoxy-dev

For OpenGL support, install the development library:
bash
Copy

sudo apt install libepoxy-dev

75. Install libwayland-dev

For Wayland support, install the development library:
bash
Copy

sudo apt install libwayland-dev

76. Install libxkbcommon-dev

For keyboard handling, install the development library:
bash
Copy

sudo apt install libxkbcommon-dev

77. Install libegl1-mesa-dev

For EGL support, install the development library:
bash
Copy

sudo apt install libegl1-mesa-dev

78. Install libgles2-mesa-dev

For OpenGL ES support, install the development library:
bash
Copy

sudo apt install libgles2-mesa-dev

79. Install libgl1-mesa-dev

For OpenGL support, install the development library:
bash
Copy

sudo apt install libgl1-mesa-dev

80. Install libgdk-pixbuf2.0-dev

For image loading and manipulation, install the development library:
bash
Copy

sudo apt install libgdk-pixbuf2.0-dev

81. Install libcairo2-dev

For 2D graphics rendering, install the development library:
bash
Copy

sudo apt install libcairo2-dev

82. Install libpango1.0-dev

For text rendering, install the development library:
bash
Copy

sudo apt install libpango1.0-dev

83. Install libharfbuzz-dev

For text shaping, install the development library:
bash
Copy

sudo apt install libharfbuzz-dev

84. Install libfreetype6-dev

For font rendering, install the development library:
bash
Copy

sudo apt install libfreetype6-dev

85. Install libicu-dev

For Unicode support, install the development library:
bash
Copy

sudo apt install libicu-dev

86. Install libenchant-2-dev

For spell checking, install the development library:
bash
Copy

sudo apt install libenchant-2-dev

87. Install libsecret-1-dev

For secure storage of credentials, install the development library:
bash
Copy

sudo apt install libsecret-1-dev

88. Install libsoup2.4-dev

For HTTP client/server functionality, install the development library:
bash
Copy

sudo apt install libsoup2.4-dev

89. Install libsqlite3-dev

For SQLite database support, install the development library:
bash
Copy

sudo apt install libsqlite3-dev

90. Install libxml2-dev

For XML parsing, install the development library:
bash
Copy

sudo apt install libxml2-dev

91. Install libjpeg-dev

For JPEG image support, install the development library:
bash
Copy

sudo apt install libjpeg-dev

92. Install libpng-dev

For PNG image support, install the development library:
bash
Copy

sudo apt install libpng-dev

93. Install libavcodec-dev and libavformat-dev

For multimedia support, install the FFmpeg development libraries:
bash
Copy

sudo apt install libavcodec-dev libavformat-dev

94. Install libpulse-dev

For PulseAudio support, install the development library:
bash
Copy

sudo apt install libpulse-dev

95. Install libgstreamer-plugins-bad1.0-dev

For additional GStreamer plugins, install the development library:
bash
Copy

sudo apt install libgstreamer-plugins-bad1.0-dev

96. Install libsoup2.4-dev

For HTTP client/server functionality, install the development library:
bash
Copy

sudo apt install libsoup2.4-dev

97. Install libsecret-1-dev

For secure storage of credentials, install the development library:
bash
Copy

sudo apt install libsecret-1-dev

98. Install libatk-bridge2.0-dev

For accessibility support, install the development library:
bash
Copy

sudo apt install libatk-bridge2.0-dev

99. Install libepoxy-dev

For OpenGL support, install the development library:
bash
Copy

sudo apt install libepoxy-dev

100. Install libwayland-dev

For Wayland support, install the development library:
bash
Copy

sudo apt install libwayland-dev

101. Install libxkbcommon-dev

For keyboard handling



