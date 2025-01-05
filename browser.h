#ifndef BROWSER_H
#define BROWSER_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// Declare global variables
extern GtkWidget *main_window;
extern GtkWidget *notebook;

// Declare callback functions
void go_button_callback(GtkWidget *widget, gpointer data);
void back_button_callback(GtkWidget *widget, gpointer data);
void forward_button_callback(GtkWidget *widget, gpointer data);
void reload_button_callback(GtkWidget *widget, gpointer data);
void download_video_callback(GtkWidget *widget, gpointer data);
void star_button_callback(GtkWidget *widget, gpointer data);

// Declare bookmark functions
void add_bookmark(const char *name, const char *url);
void remove_bookmark(const char *name);
void save_bookmarks();
void load_bookmarks();
GtkWidget *create_bookmark_toolbar(GtkNotebook *notebook);
void update_bookmark_toolbar(GtkWidget *toolbar, GtkNotebook *notebook);

// Declare bookmark button callback
void bookmark_button_callback(GtkWidget *widget, gpointer data);

// Declare main frame function
GtkWidget *create_main_frame(GtkNotebook *notebook);

// Declare server function
void start_server();

// Declare add_tab_callback function
void add_tab_callback(GtkWidget *widget, gpointer data);

// Declare create_main_menu function
GtkWidget *create_main_menu(GtkWidget *window);

// Declare key event functions
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data);
void connect_key_events(GtkWidget *window, GtkNotebook *notebook);

// Declare network stats structure and functions
typedef struct {
    guint64 requests_sent;
    guint64 responses_received;
} NetworkStats;

void update_network_stats(GtkWidget *label, NetworkStats *stats);
GtkWidget *create_network_stats_display(GtkNotebook *notebook);

#endif // BROWSER_H
