


#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#define PASSWORD_LENGTH 16
#define HOST_LENGTH 16
#define CALLSIGN_LENGTH 10
#define PTT_BANK_LENGTH 10

typedef struct AppSettingsStruct
{
	uint16_t 	magicNumber;
	uint32_t	dmrId;
	uint32_t	repeaterId;
	uint16_t 	remotePort;
	uint32_t 	initialTG;
	uint32_t 	currentTG;
	uint8_t		currentTGPrivate;
	uint8_t 	remoteHost[HOST_LENGTH];
	uint8_t     password[PASSWORD_LENGTH];
	uint8_t     callsign[CALLSIGN_LENGTH];
	uint8_t     ambeServerHost[HOST_LENGTH];
	uint16_t    ambeServerPort;
	uint8_t		smallUI;
	uint8_t		decorated;
	uint16_t	tot; //talk out time
	uint16_t	totReverse; //show count down timer
	uint8_t     pttBank[PTT_BANK_LENGTH];
	uint8_t		pttPort;
	uint8_t		pttEnabled;
	uint8_t		pttInvert;
} AppSettingsStruct_t;


void setDefaultSettings(void);

#endif
