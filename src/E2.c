#include "main.h"


int main (int argc, char **argv)
{

	GtkWidget       *window;
	GtkBuilder      *builder;

	gtk_init(&argc, &argv);

	setDefaultSettings();
	net_init();

	//g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	GError * error = NULL;

	builder = gtk_builder_new();
	gtk_builder_add_from_resource (builder, "/com/ea3ihi/openDMR/OpenDMR.glade", &error);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));


	//GtkWindow *mainWindow = GTK_WINDOW(window);
	//gtk_window_fullscreen (GTK_WINDOW(window));

	g_object_unref(builder);

	gtk_widget_show(window);

	gtk_main();

	return 0;


}
