#include <gtk/gtk.h>
#include <stdio.h>
#include "userinput.h"

static GtkWidget *window;
static GtkWidget *grid;
static GtkWidget *urlEntry;
static GtkWidget *label;
static GtkWidget *folder_label;
static GtkWidget *downloading_label;

char* filename = NULL;
char* folder = NULL;
char* url = NULL;

static void start_update(GtkWidget *widget, gpointer data) {
  run_program("-u", "notUsed");  
}

static void start_download(GtkWidget *widget, gpointer data) {
  url = gtk_entry_get_text(GTK_ENTRY(urlEntry));

  if(url != NULL) {
    char actual_folder[200];
    strcpy(actual_folder, folder);
    strcat(actual_folder, "/");
//    printf("Folder: %s\n", folder);
    if(folder == NULL) {
      char* temporary = getDirectoryFromUrl(url);
      strcpy(actual_folder, temporary);
    } else {

    }
    char* time = "";
    char* copyUrl = strdup(url);
    callDaemonToDownload(url, actual_folder, time);
  }
  
  if(filename != NULL) {
    downloadFromFile(filename);
  }

}

static void open_dialog(GtkWidget *widget, gpointer data) {
  GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

  GtkFileFilter* filter = gtk_file_filter_new();
  gtk_file_filter_add_mime_type(filter, "text/plain");
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

  gint res = gtk_dialog_run(GTK_DIALOG(dialog));

  if(res == GTK_RESPONSE_ACCEPT) {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    filename = gtk_file_chooser_get_filename(chooser);
    //printf("File selected: %s\n", filename);
    gtk_label_set_text(GTK_LABEL(label), filename);
    gtk_widget_destroy(dialog);
  }

  if(res == GTK_RESPONSE_CANCEL) {
    gtk_widget_destroy(dialog);
  }

}

static void choose_folder(GtkWidget *widget, gpointer data) {
  GtkWidget *dialog = gtk_file_chooser_dialog_new("Choose Folder", NULL,
 GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

  gint res = gtk_dialog_run(GTK_DIALOG(dialog));

  if(res == GTK_RESPONSE_ACCEPT) {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    folder = gtk_file_chooser_get_filename(chooser);
    //printf("File selected: %s\n", filename);
    gtk_label_set_text(GTK_LABEL(folder_label), folder);
    gtk_widget_destroy(dialog);
  }

  if(res == GTK_RESPONSE_CANCEL) {
    gtk_widget_destroy(dialog);
  }

}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *button;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Download Manager");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();

  /* Pack the container in the window */
  gtk_container_add (GTK_CONTAINER (window), grid);

  button = gtk_button_new_with_label ("Choose file");
  g_signal_connect (button, "clicked", G_CALLBACK (open_dialog), NULL);

  gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

  button = gtk_button_new_with_label ("Output Destination");
  g_signal_connect (button, "clicked", G_CALLBACK (choose_folder), NULL);

  gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

  label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label), "");

  gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 2, 1);

  folder_label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(folder_label), "");
  gtk_grid_attach(GTK_GRID(grid), folder_label, 0, 2, 2, 1);

  urlEntry = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), urlEntry, 0, 3, 2, 1);

  button = gtk_button_new_with_label ("Download");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (start_download), window);

  gtk_grid_attach (GTK_GRID (grid), button, 0, 4, 2, 1);

  button = gtk_button_new_with_label("Update");
  g_signal_connect_swapped(button, "clicked", G_CALLBACK(start_update), window);
  gtk_grid_attach(GTK_GRID(grid), button, 0, 5, 2, 1);

  downloading_label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(downloading_label), "");
  gtk_grid_attach(GTK_GRID(grid), downloading_label, 0, 6, 2, 1);

  gtk_widget_show_all (window); 
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;
  
  initialize();

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
