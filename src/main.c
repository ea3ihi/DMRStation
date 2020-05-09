/*
 *   Copyright (C) 2020 by EA3IHI, David
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "main.h"

extern AppSettingsStruct_t settings;


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

extern dmr_control_struct_t dmr_control;

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
	settings.currentTGPrivate = 0;
	activateTG(settings.dmrId, 4000);
	void talkgroup_select_by_index(int index);
	settings.currentTG = 0;
	talkgroup_select_by_index(0);
	ui_set_tg(settings.currentTG);
}

void
onButtonExitClick (GtkButton *button,
               gpointer   user_data)
{

	audio_deinit();
	net_deinit();
	dmrids_deinit();
	ambeclient_deinit();
	if (settings.pttEnabled == 1)
	{
		ptt_deinit();
	}
	//gtk_main_quit();

	gtk_window_close(GTK_WINDOW(window));
}

void onButtonPTT(GtkToggleButton *togglebutton,
        gpointer         user_data)
{
	if (gtk_toggle_button_get_active (togglebutton))
	{
		g_timer_start(dmr_control.timerTOT);

		gtk_widget_show(GTK_WIDGET(labelTT));

		lastHeardAddByIdAndTG(settings.dmrId, settings.currentTG);

		if (settings.tot != 0 && settings.totReverse == 1)
		{
			char buf[10];
			g_snprintf(buf, 10 , "%d", settings.tot);
			gtk_label_set_text (GTK_LABEL(labelTT), buf);
		}
		else
		{
			gtk_label_set_text (GTK_LABEL(labelTT), "0");
		}

		while(gtk_events_pending()){
					gtk_main_iteration();
				}

		dmr_start_tx();
		audio_record_start();
		g_printf("PTT ON\n");
	}
	else
	{
		audio_record_stop();
		g_timer_stop(dmr_control.timerTOT);

		gtk_label_set_text (GTK_LABEL(labelTT), "0");
		gtk_widget_hide(labelTT);

		/*
		 while(gtk_events_pending()){
			gtk_main_iteration();
		}*/

		//dmr_stop_tx();

		g_printf("PTT OFF\n");
	}

}


int main (int argc, char **argv)
{

	GtkBuilder      *builder;

	gtk_init(&argc, &argv);

	//test ambe processing
	//uint8_t ambe49[7] = {0xF0, 00, 00, 00, 00, 00, 00};//f0000000000000
	//uint8_t ambe72[9]  = {0};
	//convert49BitTo72BitAMBE(ambe49, ambe72);
	//expected result: {0xAC, 0xAA, 0x40, 0x20, 0x00, 0x44, 0x40, 0x80, 0x80)

	g_set_application_name("DMRStation network radio");
	gtk_window_set_default_icon_name("DMR");
	g_setenv("PULSE_PROP_media.role", "phone", TRUE);

	setDefaultSettings();

	dmr_control.timerTOT = g_timer_new();

	settings.currentTG = settings.initialTG;

	ambeclient_init();
	audio_init();
	dmrids_init();
	if (settings.pttEnabled == 1)
	{
		ptt_init();
	}

	GError * error = NULL;

	builder = gtk_builder_new();

	switch (settings.smallUI)
	{

		case 1:
			gtk_builder_add_from_resource (builder, "/application/ui/DMRStationMini.glade", &error);
			break;
		case 2:
			gtk_builder_add_from_resource (builder, "/application/ui/DMRStationMicro.glade", &error);
			break;

		default:
			gtk_builder_add_from_resource (builder, "/application/ui/DMRStation.glade", &error);

	}

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

	switch (settings.smallUI)
	{
		case 1:
		case 2:
			gtk_notebook_set_current_page (notebook, 0); //show main tab
			break;
		default:
			gtk_notebook_set_current_page (notebook, 1); //show last heard at start
	}

	g_signal_connect(button4000, "clicked", G_CALLBACK(onButton4000Click), NULL);
	g_signal_connect(sliderVolume, "value-changed", G_CALLBACK(onVolumeChanged), NULL);
	g_signal_connect(buttonExit, "clicked", G_CALLBACK(onButtonExitClick), NULL);

	g_signal_connect(buttonPTT, "toggled", G_CALLBACK(onButtonPTT), NULL);


	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

	ui_net_connection(WAITING_LOGIN);
	lastheard_init();
	talkgroup_init();

	ui_dmr_stop(settings.dmrId, settings.currentTG, 1);

	gtk_range_set_value ( GTK_RANGE(sliderVolume),
	                     30.0);
	init_CSS();
	net_init();

	g_object_unref(builder);

	gtk_widget_show(window);


	gtk_main();



	return 0;


}

void ui_set_tg(uint32_t tg)
{
	gchar str[30] ={0};

	if (settings.currentTGPrivate == 1)
	{
		g_snprintf(str, 30, "PC %d", tg);
	}
	else
	{
		g_snprintf(str, 30, "TG %d", tg);
	}

	if (tg == 0)
	{
		g_print("TG is 0!\n");
		gtk_label_set_text (GTK_LABEL(labelTG), " ");
		return;
	}
	gtk_label_set_text (GTK_LABEL(labelTG), (gchar *) str);
}


void ui_dmr_start(uint32_t src, uint32_t dst, uint8_t type)
{
	gchar str[30] ={0};

	ui_set_tg(dst);

	gpointer data = dmrids_lookup(src);

	if (data)
	{
		char * call = strtok(data, " ");
		g_snprintf(str, 30, "%s", call);
	}
	else
	{
		g_snprintf(str, 30, "ID: %d", src);
	}

	gtk_label_set_text (GTK_LABEL(labelID), (gchar *) str);

	if (src != dmr_control.lastSrc || dst != dmr_control.lastDst)
	{
		lastHeardAddByIdAndTG(src, dst);

		dmr_control.lastSrc = src;
		dmr_control.lastDst = dst;
	}
}

void ui_dmr_stop(uint32_t src, uint32_t dst, uint8_t type)
{
	gtk_label_set_text (GTK_LABEL(labelID), "");
	ui_set_tg(dst);
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
			gtk_widget_hide(GTK_WIDGET(buttonPTT));
			break;
	case WAITING_CONNECT:
			g_snprintf(str, 20, "Reconnecting...");
			gtk_style_context_remove_class(context, "greenbg");
			gtk_style_context_add_class(context, "redbg");
			gtk_widget_hide(GTK_WIDGET(buttonPTT));
			break;
	case WAITING_AUTHORISATION:
			break;
	case WAITING_CONFIG:
			break;
	case RUNNING:
				g_snprintf(str, 20, "Connected");
				gtk_style_context_remove_class(context,"redbg");
				gtk_style_context_add_class(context,"greenbg");

				gtk_widget_show(GTK_WIDGET(buttonPTT));
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

	g_print("CSS parse error in line %d\n", gtk_css_section_get_start_line (section));
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
    gtk_css_provider_load_from_resource (provider, "/application/ui/styles.css");

    g_object_unref (provider);
}

void tickTOT(void)
{
	gchar buf[10];

	gdouble tot = g_timer_elapsed (dmr_control.timerTOT, NULL);
	int itot = (int) tot;

	if (settings.tot != 0)
	{
		if (itot > settings.tot)
		{
			gtk_toggle_button_set_active ( (GtkToggleButton *) buttonPTT,
										  FALSE);
		}

		if (settings.totReverse == 1)
		{
			itot =settings.tot - itot;
		}
	}
	else
	{

	}

	g_snprintf(buf, 10 , "%d", itot);
	gtk_label_set_text (GTK_LABEL(labelTT), buf);

}
