


#ifndef _APP_DMR_H_
#define _APP_DMR_H_

typedef enum {
    DMR_STATUS_IDLE,
	DMR_STATUS_RX,
	DMR_STATUS_TX,
} DMRStatus_t;

enum DMRCallType {
    GROUP_CALL,
    PRIVATE_CALL
};

enum DMRFrameType {
    VOICE,
    VOICE_SYNC,
    DATA_SYNC,
    UNUSED
};

enum DMRDataType {
    PRIVACY_INDICATOR,
    VOICE_HEADER,
    VOICE_TERMINATOR,
    CSBK,
    MULTIBLOCK,
    MULTIBLOCK1,
    DATA,
    RATE12DATA,
    RATE34DATA,
    IDLE,
    BURST_A,
    BURST_B,
    BURST_C,
    BURST_D,
    BURST_E,
    BURST_F
};



void processDMRVoiceFrame(uint8_t * data);
DMRStatus_t getDMRStatus();
void setDMRStatus(DMRStatus_t status);

#endif
