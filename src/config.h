


#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#define PASSWORD_LENGTH 16
#define HOST_LENGTH 16
#define CALLSIGN_LENGTH 10

typedef struct AppSettingsStruct
{
	uint16_t 	magicNumber;
	uint32_t	dmrId;
	uint32_t	repeaterId;
	uint16_t 	remotePort;
	uint8_t 	remoteHost[HOST_LENGTH];
	uint8_t     password[PASSWORD_LENGTH];
	uint8_t     callsign[CALLSIGN_LENGTH];
	uint8_t     ambeServerHost[HOST_LENGTH];
	uint16_t    ambeServerPort;

} AppSettingsStruct_t;


void setDefaultSettings(void);

#endif
