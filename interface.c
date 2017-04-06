#include <gtk/gtk.h>


static gboolean
fill (gpointer   user_data)
{
  GtkWidget *progress_bar = user_data;
  gdouble fraction;
  fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar));
  fraction += 0.001;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);
  if (fraction < 1.0) 
    return TRUE;
  return FALSE;
}



static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *progress_bar;

  gdouble fraction = 0.0;
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Download Manager");
  gtk_window_set_default_size (GTK_WINDOW (window), 720, 200);
  progress_bar = gtk_progress_bar_new ();
  gtk_container_add (GTK_CONTAINER (window), progress_bar);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);
  gtk_widget_set_margin_top(progress_bar,200);
  g_timeout_add (50, fill, GTK_PROGRESS_BAR (progress_bar));
  gtk_widget_set_size_request (progress_bar, 720,50); 
  GtkWidget * grid = gtk_grid_new ();
  gtk_grid_attach (GTK_GRID (grid), progress_bar, 0, 1, 100, 50);
  GtkWidget * child2 = gtk_label_new ("Two");
  gtk_grid_attach_next_to (GTK_GRID (grid), child2, progress_bar, GTK_POS_RIGHT, 1, 1);
  gtk_container_add (GTK_CONTAINER (window), grid);
  gtk_widget_show_all (window);
}
 



int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *myGrid;
    GtkWidget *button1, *button2, *button3, *button4, 
        *button5, *button6, *button7,*progress_bar;


    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);
    g_signal_connect(window, "destroy", 
    G_CALLBACK(gtk_main_quit), NULL);
    myGrid = gtk_grid_new();
    button1 = gtk_button_new_with_label("Download");
    progress_bar = gtk_progress_bar_new ();
    g_timeout_add (50, fill, GTK_PROGRESS_BAR (progress_bar));
    gtk_grid_attach(GTK_GRID(myGrid), button1, 0, 0, 100, 10);
    
    gtk_grid_attach(GTK_GRID(myGrid), progress_bar, 0, 2, 200, 500);

    gtk_container_add(GTK_CONTAINER (window), myGrid);
    gtk_widget_show_all (window);

    gtk_main();
    return(0);
}
