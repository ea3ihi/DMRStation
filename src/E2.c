#include "main.h"



GtkWidget       *labelTG;
GtkWidget       *labelID;
GtkWidget       *buttonConnection;
GtkWidget       *window;



int main (int argc, char **argv)
{


	GtkBuilder      *builder;

	gtk_init(&argc, &argv);

	g_set_application_name("OpenDMR network radio");
	gtk_window_set_default_icon_name("DMR");
	g_setenv("PULSE_PROP_media.role", "phone", TRUE);


	setDefaultSettings();

	ambeclient_init();
	audio_init();

	//activateTG(2143827, 21463);

	//g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	GError * error = NULL;

	builder = gtk_builder_new();
	gtk_builder_add_from_resource (builder, "/com/ea3ihi/openDMR/OpenDMR.glade", &error);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));

	labelTG = GTK_WIDGET(gtk_builder_get_object(builder, "labelTG"));
	labelID = GTK_WIDGET(gtk_builder_get_object(builder, "labelID"));
	buttonConnection = GTK_WIDGET(gtk_builder_get_object(builder, "buttonConnection"));
	ui_net_connection(WAITING_LOGIN);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	init_CSS();


	net_init();

	//GtkWindow *mainWindow = GTK_WINDOW(window);
	//gtk_window_fullscreen (GTK_WINDOW(window));

	g_object_unref(builder);

	gtk_widget_show(window);

	gtk_main();

	audio_deinit();

	return 0;


}


void ui_dmr_start(uint32_t src, uint32_t dst, uint8_t type)
{
	gchar str[30] ={0};

	g_sprintf(str, "TG %d", dst);

	gtk_label_set_text (GTK_LABEL(labelTG), (gchar *) str);

	g_sprintf(str, "ID: %d", src);
	gtk_label_set_text (GTK_LABEL(labelID), (gchar *) str);

}

void ui_dmr_stop(uint32_t src, uint32_t dst, uint8_t type)
{
	//gchar str[30] ={0};

	//g_sprintf(str, "TG: %d", dst);

	//gtk_label_set_text (GTK_LABEL(labelTG), (gchar *) str);

	gtk_label_set_text (GTK_LABEL(labelID), "");

}

void ui_net_connection(uint8_t status)
{
	gchar str[20] ={0};

	GtkStyleContext *context = gtk_widget_get_style_context(buttonConnection);

	switch(status)
	{
	case WAITING_LOGIN:
			g_sprintf(str, "Log in...");
			gtk_style_context_remove_class(context, "greenbg");
			gtk_style_context_add_class(context, "redbg");
			break;
	case WAITING_CONNECT:
			g_sprintf(str, "Reconnecting...");
			gtk_style_context_remove_class(context, "greenbg");
			gtk_style_context_add_class(context, "redbg");
			break;
	case WAITING_AUTHORISATION:
			break;
	case WAITING_CONFIG:
			break;
	case RUNNING:
		g_sprintf(str, "Connected");
		gtk_style_context_remove_class(context,"redbg");
		gtk_style_context_add_class(context,"greenbg");
			break;
	default:
			return;
			break;
	}
	gtk_button_set_label(GTK_BUTTON(buttonConnection), (gchar *) str);
}

void
css_parse_error (GtkCssProvider *provider,
               GtkCssSection  *section,
               GError         *error,
               gpointer        user_data)
{

	g_print("Parsing error");
}
void init_CSS(void)
{
	GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);



    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


    //gtk_style_context_add_provider(gtk_widget_get_style_context(window), GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    /*
	GError *error = 0;
    gchar data[] = "#mainWindow {background-color:black;} .blackbg {background-color: black;} .greenbg {background-color: green;} .redbg {background-color: red;}";

    gtk_css_provider_load_from_data (provider,
                                     data,
                                     -1,
                                     &error);*/

    g_signal_connect(provider, "parse_error", G_CALLBACK(css_parse_error), NULL);
    gtk_css_provider_load_from_resource (provider, "/com/ea3ihi/openDMR/styles.css");

    g_object_unref (provider);
}

