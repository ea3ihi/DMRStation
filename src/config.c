#include "main.h"



static const int SETTINGS_MAGIC_NUMBER 		= 0x4744;


settingsStruct_t settings;


void setDefaultSettings(void)
{
	settings.magicNumber = SETTINGS_MAGIC_NUMBER;
	settings.dmrId = 2143827;
	settings.remotePort = 62031;

	strcpy((char *) settings.remoteHost, (char *) "192.168.3.254");
	strcpy((char *) settings.password, (char *) "passw0rd");
	strcpy((char *) settings.callsign, (char *) "EA3IHI");
}
