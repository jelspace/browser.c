#include "browser.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <gst/gst.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Callback for the "Back" button
void back_button_callback(GtkWidget *widget, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data);
    GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));

    webkit_web_view_go_back(web_view);
}

// Callback for the "Forward" button
void forward_button_callback(GtkWidget *widget, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data);
    GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));

    webkit_web_view_go_forward(web_view);
}

// Callback for the "Reload" button
void reload_button_callback(GtkWidget *widget, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data);
    GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));

    webkit_web_view_reload(web_view);
}

// Callback for the "Download Video" button
void download_video_callback(GtkWidget *widget, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data);
    GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));

    const char *url = webkit_web_view_get_uri(web_view);

    if (strstr(url, "youtube.com") == NULL && strstr(url, "youtu.be") == NULL) {
        printf("This feature is only available for YouTube videos.\n");
        return;
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Save Video",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Save", GTK_RESPONSE_ACCEPT,
        NULL
    );

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "video.mp4");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char command[1024];
        snprintf(command, sizeof(command), "yt-dlp -o \"%s\" \"%s\"", filename, url);
        int result = system(command);
        if (result == 0) {
            printf("Video downloaded successfully: %s\n", filename);
        } else {
            printf("Failed to download video.\n");
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Callback for the "Star" button (add bookmark)
void star_button_callback(GtkWidget *widget, gpointer data) {
    GtkEntry *entry = GTK_ENTRY(data);
    const char *url = gtk_entry_get_text(entry);
    const char *name = "Bookmark"; // You can modify this to allow custom names

    add_bookmark(name, url);
    printf("Bookmark added: %s -> %s\n", name, url);
}

// Callback for the "Go" button
void go_button_callback(GtkWidget *widget, gpointer data) {
    GtkEntry *entry = GTK_ENTRY(data);
    const char *url = gtk_entry_get_text(entry);

    char full_url[256];
    if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
        snprintf(full_url, sizeof(full_url), "http://%s", url);
    } else {
        strncpy(full_url, url, sizeof(full_url));
    }

    GtkNotebook *notebook = GTK_NOTEBOOK(g_object_get_data(G_OBJECT(widget), "notebook"));
    GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));

    if (web_view && WEBKIT_IS_WEB_VIEW(web_view)) {
        printf("Loading URL: %s\n", full_url); // Debug print
        webkit_web_view_load_uri(web_view, full_url);
    } else {
        g_warning("Failed to retrieve WebKitWebView from the current page.");
    }
}

// Function to update the network statistics display
void update_network_stats(GtkWidget *label, NetworkStats *stats) {
    char text[256];
    snprintf(text, sizeof(text), "Requests Sent: %lu | Responses Received: %lu",
             stats->requests_sent, stats->responses_received);
    gtk_label_set_text(GTK_LABEL(label), text);
}

// Callback to monitor network requests
static void network_request_started_callback(WebKitWebView *web_view, WebKitWebResource *resource, WebKitURIRequest *request, gpointer data) {
    NetworkStats *stats = (NetworkStats *)data;
    stats->requests_sent++;
}

// Callback to monitor network responses
static void network_response_received_callback(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer data) {
    NetworkStats *stats = (NetworkStats *)data;
    stats->responses_received++;
}

// Function to create the network statistics display
GtkWidget *create_network_stats_display(GtkNotebook *notebook) {
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    GtkWidget *label = gtk_label_new("Requests Sent: 0 | Responses Received: 0");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);

    NetworkStats *stats = g_new0(NetworkStats, 1);

    // Connect to the WebKitWebView signals
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook)));
    g_signal_connect(web_view, "resource-load-started", G_CALLBACK(network_request_started_callback), stats);
    g_signal_connect(web_view, "load-changed", G_CALLBACK(network_response_received_callback), stats);

    g_timeout_add_seconds(1, (GSourceFunc)update_network_stats, label);

    return hbox;
}

// Function to create the main frame without volume indicators
GtkWidget *create_main_frame(GtkNotebook *notebook) {
    // Create a vertical box to hold the buttons, entry, and network stats
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Create the horizontal box for buttons and entry
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    // Create the "Back" button
    GtkWidget *back_button = gtk_button_new_with_label("Back");
    g_signal_connect(back_button, "clicked", G_CALLBACK(back_button_callback), notebook);
    gtk_box_pack_start(GTK_BOX(hbox), back_button, FALSE, FALSE, 0);

    // Create the "Forward" button
    GtkWidget *forward_button = gtk_button_new_with_label("Forward");
    g_signal_connect(forward_button, "clicked", G_CALLBACK(forward_button_callback), notebook);
    gtk_box_pack_start(GTK_BOX(hbox), forward_button, FALSE, FALSE, 0);

    // Create the "Reload" button
    GtkWidget *reload_button = gtk_button_new_with_label("Reload");
    g_signal_connect(reload_button, "clicked", G_CALLBACK(reload_button_callback), notebook);
    gtk_box_pack_start(GTK_BOX(hbox), reload_button, FALSE, FALSE, 0);

    // Create the "Download Video" button
    GtkWidget *download_button = gtk_button_new_with_label("Download Video");
    g_signal_connect(download_button, "clicked", G_CALLBACK(download_video_callback), notebook);
    gtk_box_pack_start(GTK_BOX(hbox), download_button, FALSE, FALSE, 0);

    // Create the "Star" button (for bookmarks)
    GtkWidget *star_button = gtk_button_new_with_label("Star");
    g_signal_connect(star_button, "clicked", G_CALLBACK(star_button_callback), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), star_button, FALSE, FALSE, 0);

    // Create the URL entry
    GtkWidget *url_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(url_entry), "Enter URL...");
    gtk_box_pack_start(GTK_BOX(hbox), url_entry, TRUE, TRUE, 0);

    // Create the "Go" button
    GtkWidget *go_button = gtk_button_new_with_label("Go");
    g_object_set_data(G_OBJECT(go_button), "notebook", notebook); // Pass the notebook to the button's data
    g_signal_connect(go_button, "clicked", G_CALLBACK(go_button_callback), url_entry);
    gtk_box_pack_start(GTK_BOX(hbox), go_button, FALSE, FALSE, 0);

    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    // Create and add the network statistics display
    GtkWidget *network_stats_display = create_network_stats_display(notebook);
    gtk_box_pack_start(GTK_BOX(vbox), network_stats_display, FALSE, FALSE, 0);

    return vbox;
}
