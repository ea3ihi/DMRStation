#include "main.h"

extern AppSettingsStruct_t settings;

extern uint32_t lastSrc;
extern uint32_t lastDst;

GtkWidget       *labelTG;
GtkWidget       *labelID;
GtkWidget       *labelTT;
GtkWidget       *buttonConnection;
GtkWidget       *window;
GtkWidget       *button4000;
GtkWidget       *buttonExit;
GtkWidget       *buttonPTT;

GtkWidget       *sliderVolume;

GtkTreeView       *treeTG;
GtkTreeView       *treeLH;
GtkNotebook 	*notebook;

void
onVolumeChanged (GtkRange *range,
               gpointer  user_data)
{
	gdouble value = gtk_range_get_value (range);

	uint32_t v = (uint32_t) (65535 * value / 100);

	setVolume(v);
}

void
onButton4000Click (GtkButton *button,
               gpointer   user_data)
{

	activateTG(settings.dmrId, 4000);
}

void
onButtonExitClick (GtkButton *button,
               gpointer   user_data)
{

	audio_deinit();
	net_deinit();
	//net_deinit();
	//gtk_main_quit();

	gtk_window_close(GTK_WINDOW(window));
}

void onButtonPTT(GtkToggleButton *togglebutton,
        gpointer         user_data)
{
	if (gtk_toggle_button_get_active (togglebutton))
	{
		gtk_widget_show(labelTT);
		audio_record_start();
	}
	else
	{
		gtk_widget_hide(labelTT);
		audio_record_stop();
	}

}


int main (int argc, char **argv)
{

	GtkBuilder      *builder;

	gtk_init(&argc, &argv);

	g_set_application_name("OpenDMR network radio");
	gtk_window_set_default_icon_name("DMR");
	g_setenv("PULSE_PROP_media.role", "phone", TRUE);

	setDefaultSettings();

	settings.currentTG = settings.initialTG;

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
	labelTT = GTK_WIDGET(gtk_builder_get_object(builder, "labelTT"));
	buttonConnection = GTK_WIDGET(gtk_builder_get_object(builder, "buttonConnection"));
	button4000 = GTK_WIDGET(gtk_builder_get_object(builder, "button4000"));
	buttonExit = GTK_WIDGET(gtk_builder_get_object(builder, "buttonExit"));
	buttonPTT = GTK_WIDGET(gtk_builder_get_object(builder, "buttonPTT"));
	sliderVolume = GTK_WIDGET(gtk_builder_get_object(builder, "sliderVolume"));
	treeTG = (GtkTreeView *) gtk_builder_get_object(builder, "treeTG");
	treeLH = (GtkTreeView *) gtk_builder_get_object(builder, "treeLH");
	notebook = (GtkNotebook *) gtk_builder_get_object(builder, "notebook1");

	gtk_notebook_set_current_page (notebook, 1); //show last heard at start

	g_signal_connect(button4000, "clicked", G_CALLBACK(onButton4000Click), NULL);
	g_signal_connect(sliderVolume, "value-changed", G_CALLBACK(onVolumeChanged), NULL);
	g_signal_connect(buttonExit, "clicked", G_CALLBACK(onButtonExitClick), NULL);

	g_signal_connect(buttonPTT, "toggled", G_CALLBACK(onButtonPTT), NULL);


	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

	ui_net_connection(WAITING_LOGIN);
	lastheard_init();

	gtk_range_set_value ( GTK_RANGE(sliderVolume),
	                     30.0);

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

	g_snprintf(str, 30, "TG %d", dst);

	gtk_label_set_text (GTK_LABEL(labelTG), (gchar *) str);

	g_snprintf(str, 30, "ID: %d", src);
	gtk_label_set_text (GTK_LABEL(labelID), (gchar *) str);

	if (src != lastSrc || dst != lastDst)
	{
		//TODO: lookup callsigns and so
		lastHeardData_t lh;
		g_snprintf((gchar *) lh.call, sizeof(lh.call), "%d", src);
		g_snprintf((gchar *) lh.name, sizeof(lh.name), " - - ");
		lastHeardAdd(&lh);

		lastSrc = src;
		lastDst = dst;
	}
}

void ui_dmr_stop(uint32_t src, uint32_t dst, uint8_t type)
{
	gchar str[30] ={0};

	g_snprintf(str, 30, "TG %d", settings.currentTG);

	gtk_label_set_text (GTK_LABEL(labelTG), (gchar *) str);

	gtk_label_set_text (GTK_LABEL(labelID), "");

}

void ui_net_connection(uint8_t status)
{
	gchar str[20] ={0};

	GtkStyleContext *context = gtk_widget_get_style_context(buttonConnection);

	switch(status)
	{
	case WAITING_LOGIN:
			g_snprintf(str, 20, "Log in...");
			gtk_style_context_remove_class(context, "greenbg");
			gtk_style_context_add_class(context, "redbg");
			break;
	case WAITING_CONNECT:
			g_snprintf(str, 20, "Reconnecting...");
			gtk_style_context_remove_class(context, "greenbg");
			gtk_style_context_add_class(context, "redbg");
			break;
	case WAITING_AUTHORISATION:
			break;
	case WAITING_CONFIG:
			break;
	case RUNNING:
		g_snprintf(str, 20, "Connected");
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

	g_print("CSS parsing error\n");
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

    g_signal_connect(provider, "parsing-error", G_CALLBACK(css_parse_error), NULL);
    gtk_css_provider_load_from_resource (provider, "/com/ea3ihi/openDMR/styles.css");

    g_object_unref (provider);
}

