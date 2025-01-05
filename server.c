#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>
#include <signal.h>

#define PORT 8000
#define BUFFER_SIZE 1024
#define VIDEO_DIR "videos"
#define FILE_PATH_SIZE 512 // Increase buffer size for file paths

// Global variable to store the server socket
static int server_socket = -1;

// Function to kill any process using the specified port
void kill_existing_server() {
    char command[256];
    snprintf(command, sizeof(command), "fuser -k -n tcp %d", PORT);
    int result = system(command);
    if (result != 0) {
        printf("No existing server found on port %d\n", PORT);
    } else {
        printf("Killed existing server on port %d\n", PORT);
    }
}

// Function to send an HTTP response
void send_response(int client_socket, const char *status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    int length = snprintf(response, BUFFER_SIZE,
                          "HTTP/1.1 %s\r\n"
                          "Content-Type: %s\r\n"
                          "Content-Length: %zu\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "%s",
                          status, content_type, strlen(body), body);
    send(client_socket, response, length, 0);
}

// Function to send a file
void send_file(int client_socket, const char *file_path, const char *content_type) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        send_response(client_socket, "404 Not Found", "text/plain", "File not found");
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Send HTTP headers
    char headers[BUFFER_SIZE];
    int headers_length = snprintf(headers, BUFFER_SIZE,
                                  "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: %s\r\n"
                                  "Content-Length: %ld\r\n"
                                  "Connection: close\r\n"
                                  "\r\n",
                                  content_type, file_size);
    send(client_socket, headers, headers_length, 0);

    // Send file content
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

// Function to handle HTTP requests
void handle_request(int client_socket, const char *request) {
    char method[16], path[256];
    sscanf(request, "%s %s", method, path);

    printf("Received request: %s %s\n", method, path); // Debug log

    if (strcmp(method, "GET") != 0 && strcmp(method, "POST") != 0) {
        send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method not allowed");
        return;
    }

    if (strcmp(path, "/") == 0) {
        // Serve the index.html file
        send_file(client_socket, "index.html", "text/html");
    } else if (strncmp(path, "/videos/", 8) == 0) {
        // Serve video files
        char file_path[FILE_PATH_SIZE]; // Use the larger buffer size
        snprintf(file_path, sizeof(file_path), "%s/%s", VIDEO_DIR, path + 8);
        printf("Serving video file: %s\n", file_path); // Debug log
        send_file(client_socket, file_path, "video/mp4");
    } else if (strcmp(path, "/videos") == 0) {
        // List video files in JSON format
        DIR *dir = opendir(VIDEO_DIR);
        if (!dir) {
            send_response(client_socket, "500 Internal Server Error", "text/plain", "Failed to open video directory");
            return;
        }

        char json[BUFFER_SIZE] = "[";
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            // Check if the entry is a regular file (fallback to stat if DT_REG is not available)
            struct stat file_stat;
            char file_path[FILE_PATH_SIZE]; // Use the larger buffer size
            snprintf(file_path, sizeof(file_path), "%s/%s", VIDEO_DIR, entry->d_name);
            if (stat(file_path, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
                if (strstr(entry->d_name, ".mp4") || strstr(entry->d_name, ".webm") || strstr(entry->d_name, ".ogg")) {
                    if (strlen(json) > 1) {
                        strcat(json, ",");
                    }
                    snprintf(json + strlen(json), BUFFER_SIZE - strlen(json),
                             "{\"name\":\"%s\",\"url\":\"/videos/%s\"}", entry->d_name, entry->d_name);
                }
            }
        }
        closedir(dir);
        strcat(json, "]");

        printf("Sending video list: %s\n", json); // Debug log
        send_response(client_socket, "200 OK", "application/json", json);
    } else if (strcmp(path, "/download") == 0 && strcmp(method, "POST") == 0) {
        // Handle video download request
        char body[BUFFER_SIZE];
        int bytes_read = recv(client_socket, body, BUFFER_SIZE - 1, 0);
        if (bytes_read < 0) {
            send_response(client_socket, "400 Bad Request", "text/plain", "Failed to read request body");
            return;
        }
        body[bytes_read] = '\0';

        printf("Received download request body: %s\n", body); // Debug log

        // Parse the JSON body to get the video URL
        char video_url[256];
        if (sscanf(body, "{\"url\":\"%255[^\"]\"}", video_url) != 1) {
            send_response(client_socket, "400 Bad Request", "text/plain", "Invalid request body");
            return;
        }

        printf("Downloading video from URL: %s\n", video_url); // Debug log

        // Use yt-dlp to download the video with the original name
        char command[512];
        snprintf(command, sizeof(command), "yt-dlp --restrict-filenames -o '%s/%%(title)s.%%(ext)s' \"%s\"", VIDEO_DIR, video_url);

        printf("Executing command: %s\n", command); // Debug log

        int result = system(command);
        if (result == 0) {
            printf("Video downloaded successfully: %s\n", video_url); // Debug log
            send_response(client_socket, "200 OK", "application/json", "{\"success\": true}");
        } else {
            printf("Failed to download video: %s\n", video_url); // Debug log
            send_response(client_socket, "500 Internal Server Error", "application/json", "{\"success\": false, \"message\": \"Failed to download video\"}");
        }
    } else {
        send_response(client_socket, "404 Not Found", "text/plain", "Resource not found");
    }
}

// Function to run the server in a separate thread
void *server_thread(void *arg) {
    // Create the video directory if it doesn't exist
    mkdir(VIDEO_DIR, 0755);

    // Kill any existing server process using the same port
    kill_existing_server();

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return NULL;
    }

    // Bind the socket to the port
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        return NULL;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        return NULL;
    }

    printf("Server is running on http://localhost:%d\n", PORT);

    while (1) {
        // Accept a client connection
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        // Read the HTTP request
        char request[BUFFER_SIZE];
        int bytes_received = recv(client_socket, request, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            perror("Receive failed");
            close(client_socket);
            continue;
        }
        request[bytes_received] = '\0';

        // Handle the request
        handle_request(client_socket, request);

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);
    return NULL;
}

// Function to start the server
void start_server() {
    pthread_t thread;
    if (pthread_create(&thread, NULL, server_thread, NULL) != 0) {
        perror("Failed to create server thread");
        return;
    }
    pthread_detach(thread); // Detach the thread to run independently
}
