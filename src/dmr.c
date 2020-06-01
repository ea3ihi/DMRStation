

#include "main.h"


uint8_t frameData[27];

char ambe_d[49];
char ambe_fr[4][24];
uint8_t ambeBytes[7];
char err_str[64];

dmr_control_struct_t dmr_control;
extern AppSettingsStruct_t settings;

/**
 * Return DMR status
 */
DMRStatus_t getDMRStatus()
{
	return dmr_control.dmr_status;
}

/**
 * Set DMR status
 */
void setDMRStatus(DMRStatus_t status)
{
	dmr_control.dmr_status = status;
	g_print("DMR status %d\n", status);
}

/**
 * Process the 33 bytes of DMR voice data
 */
void processDMRVoiceFrame(uint8_t * data)
{
	int errs1;
	int errs2 = 0;

	//copy first 13 bytes
	memcpy(&frameData[0], &data[0], 13);

	//copy halves
	uint8_t n1 = data[13] & 0xF0;
	uint8_t n2 = data[19] & 0x0F;
	frameData[13] = n1 | n2;

	//copy last 13
	memcpy(&frameData[14], &data[20], 13);


	// decompose the audio data in 3 AMBE chunks
	// convert to PCM and send to play buffer
	for (int idx = 0; idx<3; idx++)
	{

		prepare_framedata(&frameData[9 * idx], ambe_d, &errs1, &errs2);

		memset(ambeBytes, 0, 7);

		for (int i = 0; i<49; i++)
		{
			if (ambe_d[i] & 1)
			{
				ambeBytes[i>>3] |= 128 >> (i & 7);
			}
		}

		if (settings.codec2Enabled == 0)
		{
			ambe_send(ambeBytes, sizeof(ambeBytes));
		}
		else
		{
			codec2_send(ambeBytes, sizeof(ambeBytes));
		}

	}

}


