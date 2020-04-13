#include "main.h"


static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
  g_print ("Hello World\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *button_box;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add (GTK_CONTAINER (window), button_box);

  button = gtk_button_new_with_label ("Hello World");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  gtk_container_add (GTK_CONTAINER (button_box), button);

  gtk_widget_show_all (window);
}

int
main (int    argc,
      char **argv)
{
	int status;
	GtkApplication *app;
	GtkWidget       *window;
	GtkBuilder      *builder;


	gtk_init(&argc, &argv);


	setDefaultSettings();
	net_init();


	//app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	//g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	GError * error = NULL;

	builder = gtk_builder_new();
	gtk_builder_add_from_resource (builder, "/com/ea3ihi/openDMR/OpenDMR.glade", &error);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));


	//GtkWindow *mainWindow = GTK_WINDOW(window);
	//gtk_window_fullscreen (GTK_WINDOW(window));

	//status = g_application_run (G_APPLICATION (app), argc, argv);
	//g_object_unref (app);

	g_object_unref(builder);

	gtk_widget_show(window);

	gtk_main();

	return 0;


}
