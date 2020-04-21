


#ifndef _APP_NET_H_
#define _APP_NET_H_

#include <stdint.h>
#include <stdbool.h>

enum DMR_STATUS {
		WAITING_CONNECT,
		WAITING_LOGIN,
		WAITING_AUTHORISATION,
		WAITING_CONFIG,
		WAITING_OPTIONS,
		RUNNING
	};

//Structure to work with tx parameters
typedef struct dmr_control_struct {
	uint8_t DMRSequence;
	uint8_t voiceSequence;
	uint32_t streamId;
	uint32_t destination;
	DMRStatus_t dmr_status;
	uint32_t lastSrc;
	uint32_t lastDst;

	GTimer			*timerTOT;
} dmr_control_struct_t;

gboolean dataInCallback(GSocket *source, GIOCondition condition, gpointer data);
void net_init(void);
void net_deinit(void);



bool network_send(uint8_t * data, unsigned int length);
void reconnect(void);
bool writeLogin(void);
bool writeAuthorisation(void);
bool writeConfig(void);
bool writePing(void);
gboolean network_tick(void);
void activateTG(uint32_t src, uint32_t dst);
void createVoiceHeader(uint32_t src, uint32_t dst, uint8_t *dataOut, uint8_t seq, uint32_t streamId);
void createVoiceTerminator(uint32_t src, uint32_t dst, uint8_t *dataOut, uint8_t seq, uint32_t streamId);
void createSilenceFrame(uint32_t src, uint32_t dst, uint8_t *dataOut, uint8_t seq, uint32_t streamId, uint8_t voiceSeq);
void createVoiceFrame(uint32_t src, uint32_t dst, uint8_t *dataOut, uint8_t seq, uint32_t streamId, uint8_t voiceSeq, uint8_t * ambe72Data);
void prepareVoiceFrame( uint8_t * ambe72Data);
void dmr_start_tx(void);
void dmr_stop_tx(void);


void tick_DMRQueue(void);
void readDMRQueue(uint8_t *dataOut);
void writeDMRQueue(uint8_t *data);



#endif
