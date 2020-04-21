#include "main.h"



static const int SETTINGS_MAGIC_NUMBER 		= 0x4744;


AppSettingsStruct_t settings;


bool loadConfigFile(void)
{
	g_autoptr(GError) error = NULL;
	g_autoptr(GKeyFile) key_file = g_key_file_new ();

	const char *path;
	char *file;

	path = g_get_current_dir();
	file = g_build_filename(path, "DMRStation.ini", NULL);
	g_free((gpointer)path);

	if (!g_key_file_load_from_file (key_file, file, G_KEY_FILE_NONE, &error))
    {
		if (!g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
			g_warning ("Error loading key file: %s", error->message);
		return false;
    }

	g_autofree gchar *val;
	//user data
	val = g_key_file_get_string (key_file, "user", "callsign", &error);
	if (val != NULL)
	{
		g_snprintf((gchar *) settings.callsign, CALLSIGN_LENGTH, "%s", val);
	}

	val = g_key_file_get_string (key_file, "user", "dmrId", &error);
	if (val != NULL)
	{
		settings.dmrId = atoi(val);
	}

	val = g_key_file_get_string (key_file, "user", "repeaterId", &error);
	if (val != NULL)
	{
		settings.repeaterId = atoi(val);
	}

	val = g_key_file_get_string (key_file, "user", "initialTG", &error);
	if (val != NULL)
	{
		settings.initialTG = atoi(val);
	}


	//server data
	val = g_key_file_get_string (key_file, "server", "remoteHost", &error);
	if (val != NULL)
	{
		g_snprintf((gchar *) settings.remoteHost, HOST_LENGTH, "%s", val);
	}

	val = g_key_file_get_string (key_file, "server", "remotePort", &error);
	if (val != NULL)
	{
		settings.remotePort = atoi(val);
	}

	val = g_key_file_get_string (key_file, "server", "password", &error);
	if (val != NULL)
	{
		g_snprintf((gchar *) settings.password, HOST_LENGTH, "%s", val);
	}

	//ambe data
	val = g_key_file_get_string (key_file, "ambe", "ambeServerHost", &error);
	if (val != NULL)
	{
		g_snprintf((gchar *) settings.ambeServerHost, HOST_LENGTH, "%s", val);
	}

	val = g_key_file_get_string (key_file, "ambe", "ambeServerPort", &error);
	if (val != NULL)
	{
		settings.ambeServerPort = atoi(val);
	}

return true;
}


void setDefaultSettings(void)
{
	if(loadConfigFile())
	{
		return;
	}

	//Default config
	settings.magicNumber = SETTINGS_MAGIC_NUMBER;
	settings.dmrId = 2143814;
	settings.repeaterId = 214381466;
	settings.remotePort = 62031;
	settings.initialTG = 21460;

	//local
	strcpy((char *) settings.remoteHost, (char *) "192.168.3.254");

	//My server on the net
	//strcpy((char *) settings.remoteHost, (char *) "87.98.228.225");

	//Master spain
	//strcpy((char *) settings.remoteHost, (char *) "84.232.5.113");

	strcpy((char *) settings.password, (char *) "passw0rd");
	strcpy((char *) settings.callsign, (char *) "EA3IHI");

	strcpy((char *) settings.ambeServerHost, (char *) "127.0.0.1");
	settings.ambeServerPort = 2460;
}
