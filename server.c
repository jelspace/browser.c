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
#include <ctype.h> // For isprint
#include <errno.h> // For EADDRINUSE

#define PORT 8000
#define BUFFER_SIZE 1024
#define VIDEO_DIR "videos"
#define FILE_PATH_SIZE 512 // Increase buffer size for file paths

// Global variable to store the server socket
static int server_socket = -1;

// Basic URL validation. Returns 1 if potentially safe, 0 otherwise.
// This function remains as it's used by download functionality.
static int is_safe_url_for_system_call(const char *url) {
    if (url == NULL || url[0] == '\0') {
        fprintf(stderr, "is_safe_url_for_system_call: Validation failed: URL is null or empty.\n");
        return 0;
    }

    if (strlen(url) > 2048) { // Max reasonable length for a URL
        fprintf(stderr, "is_safe_url_for_system_call: Validation failed: URL too long (max 2048).\n");
        return 0;
    }

    if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
        fprintf(stderr, "is_safe_url_for_system_call: Validation failed: URL scheme not http:// or https://.\n");
        return 0;
    }

    const char *ptr = url;
    while (*ptr) {
        // Disallow most control characters. Allow printable.
        // Explicitly disallow backtick, pipe, semicolon as extra precaution,
        // though quoting in system() is primary defense.
        if (!isprint((unsigned char)*ptr) || *ptr == '`' || *ptr == '|' || *ptr == ';') {
            fprintf(stderr, "is_safe_url_for_system_call: Validation failed: URL contains non-printable or disallowed character (ASCII %d '%c').\n", (int)*ptr, *ptr);
            return 0;
        }
        ptr++;
    }
    return 1; // Seems okay
}


// Function to send an HTTP response
void send_response(int client_socket, const char *status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    // Ensure body is not excessively long to prevent header corruption if body itself is huge.
    // A more robust solution would be multi-part send or dynamic buffer for response.
    size_t body_len = strlen(body);
    if (body_len > BUFFER_SIZE / 2) { // Heuristic to leave space for headers
        // This is a simplification. Real fix is more complex.
        fprintf(stderr, "send_response: body too large, potential truncation issues.\n");
    }

    int length = snprintf(response, BUFFER_SIZE,
                          "HTTP/1.1 %s\r\n"
                          "Content-Type: %s\r\n"
                          "Content-Length: %zu\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "%s",
                          status, content_type, body_len, body);
    if (length < 0 || length >= BUFFER_SIZE) {
        fprintf(stderr, "send_response: snprintf error or truncation occurred.\n");
        // Avoid sending potentially corrupt response
        return;
    }
    if (send(client_socket, response, length, 0) == -1) {
        perror("send_response: send failed");
    }
}

// Function to send a file
void send_file(int client_socket, const char *file_path, const char *content_type) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("send_file: fopen failed");
        send_response(client_socket, "404 Not Found", "text/plain", "File not found");
        return;
    }

    // Get file size
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("send_file: fseek SEEK_END failed");
        fclose(file);
        send_response(client_socket, "500 Internal Server Error", "text/plain", "Error accessing file");
        return;
    }
    long file_size = ftell(file);
    if (file_size == -1L) {
        perror("send_file: ftell failed");
        fclose(file);
        send_response(client_socket, "500 Internal Server Error", "text/plain", "Error accessing file size");
        return;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("send_file: fseek SEEK_SET failed");
        fclose(file);
        send_response(client_socket, "500 Internal Server Error", "text/plain", "Error reading file");
        return;
    }

    // Send HTTP headers
    char headers[BUFFER_SIZE];
    int headers_length = snprintf(headers, BUFFER_SIZE,
                                  "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: %s\r\n"
                                  "Content-Length: %ld\r\n"
                                  "Connection: close\r\n"
                                  "\r\n",
                                  content_type, file_size);
    if (headers_length < 0 || headers_length >= BUFFER_SIZE) {
        fprintf(stderr, "send_file: headers snprintf error or truncation.\n");
        fclose(file);
        // Avoid sending potentially corrupt response, client will likely time out or get partial response.
        return;
    }
    if (send(client_socket, headers, headers_length, 0) == -1) {
        perror("send_file: send headers failed");
        fclose(file);
        return;
    }

    // Send file content
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(client_socket, buffer, bytes_read, 0) == -1) {
            perror("send_file: send file content failed");
            break; // Stop sending if error occurs
        }
    }
    if (ferror(file)) {
        perror("send_file: fread failed");
    }

    if (fclose(file) == EOF) {
        perror("send_file: fclose failed");
    }
}

// Function to handle HTTP requests
void handle_request(int client_socket, const char *request) {
    char method[16], path[256], protocol[16];
    // Try to parse three components: method, path, and protocol
    // Increased path size for safety, but still fixed.
    // A more robust parser would be better.
    if (sscanf(request, "%15s %255s %15s", method, path, protocol) < 2) {
        fprintf(stderr, "handle_request: Could not parse request line: %s\n", request);
        send_response(client_socket, "400 Bad Request", "text/plain", "Malformed HTTP request");
        return;
    }
    // Check if path or method were too long and got truncated by sscanf.
    // This is a basic check. If request contains no spaces, sscanf can still overflow.
    if (strlen(method) == 15 || strlen(path) == 255) {
         fprintf(stderr, "handle_request: Method or Path too long in request: %s\n", request);
         send_response(client_socket, "414 URI Too Long", "text/plain", "Method or URI too long");
         return;
    }


    fprintf(stderr, "Received request: %s %s\n", method, path); // Debug log to stderr

    if (strcmp(method, "GET") != 0 && strcmp(method, "POST") != 0) {
        fprintf(stderr, "handle_request: Unsupported method: %s\n", method);
        send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method not allowed");
        return;
    }

    if (strcmp(path, "/") == 0) {
        // Serve the index.html file
        send_file(client_socket, "index.html", "text/html");
    } else if (strncmp(path, "/videos/", 8) == 0) {
        // Serve video files
        char file_path[FILE_PATH_SIZE];
        snprintf(file_path, sizeof(file_path), "%s/%s", VIDEO_DIR, path + 8);
        fprintf(stderr, "Serving video file: %s\n", file_path); // Debug log to stderr
        send_file(client_socket, file_path, "video/mp4");
    } else if (strcmp(path, "/videos") == 0) {
        // List video files in JSON format
        DIR *dir = opendir(VIDEO_DIR);
        if (!dir) {
            perror("handle_request: opendir failed for VIDEO_DIR");
            send_response(client_socket, "500 Internal Server Error", "text/plain", "Failed to open video directory");
            return;
        }

        char json[BUFFER_SIZE * 2] = "["; // Increased buffer for JSON, but still risky
        size_t json_len = strlen(json);
        struct dirent *entry;
        int first_entry = 1;

        while ((entry = readdir(dir)) != NULL) {
            struct stat file_stat;
            char file_path_listing[FILE_PATH_SIZE];
            snprintf(file_path_listing, sizeof(file_path_listing), "%s/%s", VIDEO_DIR, entry->d_name);

            // Check if the entry is a regular file and a video file
            if (stat(file_path_listing, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
                if (strstr(entry->d_name, ".mp4") || strstr(entry->d_name, ".webm") || strstr(entry->d_name, ".ogg")) {
                    if (!first_entry) {
                        if (json_len + 1 >= sizeof(json)) {
                            fprintf(stderr, "handle_request: JSON buffer overflow risk for comma.\n");
                            break;
                        }
                        strcat(json, ",");
                        json_len++;
                    }
                    first_entry = 0;

                    // Estimate remaining buffer before snprintf
                    // {"name":"","url":"/videos/"} is 28 chars + 2*strlen(d_name)
                    if (json_len + 30 + 2 * strlen(entry->d_name) >= sizeof(json)) {
                         fprintf(stderr, "handle_request: JSON buffer overflow risk for entry %s.\n", entry->d_name);
                         break; // Too risky to continue
                    }
                    int written = snprintf(json + json_len, sizeof(json) - json_len,
                                           "{\"name\":\"%s\",\"url\":\"/videos/%s\"}", entry->d_name, entry->d_name);
                    if (written < 0 || json_len + written >= sizeof(json)) {
                        fprintf(stderr, "handle_request: snprintf error or JSON buffer overflow for entry %s.\n", entry->d_name);
                        break;
                    }
                    json_len += written;
                }
            }
        }
        closedir(dir);
        if (json_len + 1 >= sizeof(json)) {
             fprintf(stderr, "handle_request: JSON buffer overflow risk for closing bracket.\n");
             // Don't append ']', send what we have, possibly truncated/malformed
        } else {
            strcat(json, "]");
        }


        fprintf(stderr, "Sending video list: %s\n", json); // Debug log to stderr
        send_response(client_socket, "200 OK", "application/json", json);
    } else if (strcmp(path, "/download") == 0 && strcmp(method, "POST") == 0) {
        // Handle video download request
        char body[BUFFER_SIZE];
        int bytes_read = recv(client_socket, body, BUFFER_SIZE - 1, 0);
        if (bytes_read < 0) {
            perror("handle_request: recv failed for POST body");
            send_response(client_socket, "500 Internal Server Error", "text/plain", "Failed to read request body");
            return;
        } else if (bytes_read == 0) {
            fprintf(stderr, "handle_request: client closed connection before sending POST body.\n");
            // No response can be sent if client closed connection
            return;
        }
        body[bytes_read] = '\0';

        fprintf(stderr, "Received download request body: %s\n", body); // Debug log to stderr

        // Parse the JSON body to get the video URL
        char video_url[256];
        if (sscanf(body, "{\"url\":\"%255[^\"]\"}", video_url) != 1) {
            fprintf(stderr, "handle_request: Invalid JSON in POST body: %s\n", body);
            send_response(client_socket, "400 Bad Request", "text/plain", "Invalid request body format");
            return;
        }

        fprintf(stderr, "Downloading video from URL: %s\n", video_url); // Debug log to stderr

        if (!is_safe_url_for_system_call(video_url)) {
            fprintf(stderr, "handle_request: Invalid URL for download: %s\n", video_url);
            send_response(client_socket, "400 Bad Request", "text/plain", "Invalid URL format or content for download.");
            return;
        }

        // Use yt-dlp to download the video with the original name
        char command[512];
        // The format string for snprintf ALREADY quotes the video_url with \"%s\". This is crucial.
        snprintf(command, sizeof(command), "yt-dlp --restrict-filenames -o '%s/%%(title)s.%%(ext)s' \"%s\"", VIDEO_DIR, video_url);

        fprintf(stderr, "Executing command: %s\n", command); // Debug log to stderr

        int result = system(command);
        if (result == -1) {
            perror("handle_request: system call for yt-dlp failed");
            send_response(client_socket, "500 Internal Server Error", "application/json", "{\"success\": false, \"message\": \"Error executing download command\"}");
        } else if (WIFEXITED(result) && WEXITSTATUS(result) == 0) {
            fprintf(stderr, "Video downloaded successfully: %s\n", video_url); // Debug log to stderr
            send_response(client_socket, "200 OK", "application/json", "{\"success\": true}");
        } else {
            fprintf(stderr, "Failed to download video (yt-dlp exit code %d): %s\n", WEXITSTATUS(result), video_url); // Debug log to stderr
            send_response(client_socket, "500 Internal Server Error", "application/json", "{\"success\": false, \"message\": \"Failed to download video\"}");
        }
    } else {
        fprintf(stderr, "handle_request: Resource not found: %s\n", path);
        send_response(client_socket, "404 Not Found", "text/plain", "Resource not found");
    }
}

// Function to run the server in a separate thread
void *server_thread(void *arg) {
    // Create the video directory if it doesn't exist
    if (mkdir(VIDEO_DIR, 0755) == -1 && errno != EEXIST) {
        perror("server_thread: Failed to create video directory");
        // Decide if this is fatal. For now, we'll let it continue.
    }

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("server_thread: Socket creation failed");
        return NULL;
    }

    // Allow address reuse
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("server_thread: setsockopt SO_REUSEADDR failed");
        close(server_socket);
        return NULL;
    }

    // Bind the socket to the port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); // Clear struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        if (errno == EADDRINUSE) {
            fprintf(stderr, "server_thread: Port %d is already in use. Server cannot start.\n", PORT);
        } else {
            perror("server_thread: Bind failed");
        }
        close(server_socket);
        server_socket = -1; // Mark as closed
        return NULL;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        perror("server_thread: Listen failed");
        close(server_socket);
        server_socket = -1; // Mark as closed
        return NULL;
    }

    fprintf(stderr, "Server is running on http://localhost:%d\n", PORT); // To stderr

    // Setup signal handler for graceful shutdown
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint; // Define this function
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);


    while (1) {
        // Accept a client connection
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            if (errno == EINTR && server_socket == -1) { // Interrupted by our signal handler
                 fprintf(stderr, "Server shutting down due to signal.\n");
                 break;
            }
            perror("server_thread: Accept failed");
            continue;
        }

        // Read the HTTP request
        char request[BUFFER_SIZE];
        int bytes_received = recv(client_socket, request, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            perror("server_thread: Receive failed");
            close(client_socket);
            continue;
        } else if (bytes_received == 0) {
            fprintf(stderr, "server_thread: Client closed connection before sending request.\n");
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
    if (server_socket != -1) {
        fprintf(stderr, "Closing server socket.\n");
        close(server_socket);
        server_socket = -1;
    }
    return NULL;
}

// Signal handler for SIGINT and SIGTERM
void handle_sigint(int sig) {
    fprintf(stderr, "\nCaught signal %d. Shutting down server...\n", sig);
    if (server_socket != -1) {
        // This makes accept() return -1 with errno EINTR in the main loop
        // Or, if already closed by another thread/path, it does nothing.
        // Consider shutdown() for more graceful client connection termination if needed.
        close(server_socket);
        server_socket = -1; // Mark as closed to stop the accept loop
    }
    // exit(0); // Or let server_thread return naturally
}


// Function to start the server
void start_server() {
    pthread_t thread;
    if (pthread_create(&thread, NULL, server_thread, NULL) != 0) {
        perror("start_server: Failed to create server thread");
        return;
    }
    pthread_detach(thread); // Detach the thread to run independently
}
