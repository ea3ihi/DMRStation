#include "main.h"



static const int SETTINGS_MAGIC_NUMBER 		= 0x4744;


AppSettingsStruct_t settings;


void setDefaultSettings(void)
{
	settings.magicNumber = SETTINGS_MAGIC_NUMBER;
	settings.dmrId = 2143814;
	settings.repeaterId = 214381466;
	settings.remotePort = 62031;
	settings.initialTG = 214;

	strcpy((char *) settings.remoteHost, (char *) "192.168.3.254");
	//strcpy((char *) settings.remoteHost, (char *) "87.98.228.225");

	strcpy((char *) settings.password, (char *) "passw0rd");
	strcpy((char *) settings.callsign, (char *) "EA3IHI");

	strcpy((char *) settings.ambeServerHost, (char *) "127.0.0.1");
	settings.ambeServerPort = 2460;
}
