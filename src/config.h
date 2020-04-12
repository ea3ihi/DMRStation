
#define PASSWORD_LENGTH 16
#define HOST_LENGTH 16
#define CALLSIGN_LENGTH 10

typedef struct settingsStruct
{
	uint16_t 	magicNumber;
	uint32_t	dmrId;
	uint32_t 	remotePort;
	uint8_t 	remoteHost[HOST_LENGTH];
	uint8_t     password[PASSWORD_LENGTH];
	uint8_t     callsign[CALLSIGN_LENGTH];

} settingsStruct_t;


void setDefaultSettings(void);
