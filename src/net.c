

#include "main.h"

#include "dmr/DMRLC.h"
#include "dmr/DMRFullLC.h"
#include "dmr/DMRShortLC.h"
#include "dmr/DMRSlotType.h"
#include "dmr/QR1676.h"

extern uint32_t lastSrc;
extern uint32_t lastDst;

GSocket *socket;

uint8_t TxData [400];
uint8_t RxData [55];

#define PING_TICKS 5
#define PONG_TICKS 25
#define RECONNECT_TICKS 15
#define INACTIVITY_TICKS 3

uint16_t timerPing;
uint16_t timeoutPong;
uint16_t timeout_reconnect;
uint16_t timeoutInactivity;

uint32_t salt;
uint32_t ticks;

volatile enum DMR_STATUS dmrnet_status = WAITING_CONNECT;

extern AppSettingsStruct_t settings;

static const uint8_t VOICE_LC_SYNC_FULL[]       = { 0x04U, 0x6DU, 0x5DU, 0x7FU, 0x77U, 0xFDU, 0x75U, 0x7EU, 0x30U };
static const uint8_t TERMINATOR_LC_SYNC_FULL[]  = { 0x04U, 0xADU, 0x5DU, 0x7FU, 0x77U, 0xFDU, 0x75U, 0x79U, 0x60U };
static const uint8_t LC_SYNC_MASK_FULL[]        = { 0x0FU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xF0U };

void net_init(void)
{
	GInetAddress *address = g_inet_address_new_from_string((gchar *) settings.remoteHost);
	GSocketAddress *socket_address = g_inet_socket_address_new(address, settings.remotePort);

	GInetAddress *localAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4) ;
	GSocketAddress *localSocketAddress = g_inet_socket_address_new(localAddress, settings.remotePort);

	GError *error = NULL;

	socket = g_socket_new (G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, 0, &error);

	g_socket_bind (socket, localSocketAddress, TRUE, &error);

	g_socket_connect(socket, socket_address, NULL, &error);

	GSource *source = g_socket_create_source(socket, G_IO_IN, NULL);
	g_source_set_callback(source, (GSourceFunc) dataInCallback, NULL,NULL);
	g_source_attach(source, g_main_context_default());

	g_timeout_add_seconds(1, (GSourceFunc) network_tick, NULL);

	//g_socket_send (socket, "RPTL123456", 10, NULL, NULL);

	writeLogin();

}

void reconnect(void)
{
	GInetAddress *address = g_inet_address_new_from_string((gchar *) settings.remoteHost);
	GSocketAddress *socket_address = g_inet_socket_address_new(address, 62031);

	GError *error = NULL;

	g_socket_connect(socket, socket_address, NULL, &error);
	writeLogin();
}

gboolean dataInCallback(GSocket *source, GIOCondition condition, gpointer data)
{
	int dataLen = 0;

	switch (condition)
	{
		case G_IO_IN:
			dataLen = g_socket_receive(source, (gchar*) RxData, sizeof(RxData), NULL, NULL);

			if (dataLen > 0)
					{
						timeoutPong = 0;

						if (memcmp(RxData, "RPTACK",  6U) == 0) {
									switch (dmrnet_status) {
										case WAITING_LOGIN:
											memcpy((uint8_t *) &salt, RxData + 6U, sizeof(uint32_t));
											writeAuthorisation();
											dmrnet_status = WAITING_AUTHORISATION;
											ui_net_connection(WAITING_AUTHORISATION);
											break;
										case WAITING_AUTHORISATION:
											writeConfig();
											dmrnet_status = WAITING_CONFIG;
											ui_net_connection(WAITING_CONFIG);
											break;
										case WAITING_CONFIG:
											dmrnet_status = RUNNING;
											ui_net_connection(RUNNING);

											activateTG(settings.dmrId, settings.currentTG);

											break;
										default:
											break;
									}
						} else if(memcmp(RxData, "MSTPONG", 7U) == 0) {
							timerPing = 0;
						} else if (memcmp(RxData, "MSTCL",   5U) == 0) {
							//master closing
							//close();
							//open();

						} else if (memcmp(RxData, "MSTNAK",  6U) == 0) {
							//NACK
							dmrnet_status = WAITING_LOGIN;
							ui_net_connection(WAITING_LOGIN);
						} else if (memcmp(RxData, "DMRD",  4U) == 0) {
							//DMR Packet
							//do we have a complete packet
							if (dataLen > 52)
							{
								timeoutInactivity = 0;

								//is it a voice packet?
								uint8_t frameType = (RxData[15] & 0x30) >> 4;
								uint8_t dataType = RxData[15] & 0x0F;
								if (frameType == VOICE)
								{
									if (getDMRStatus() == DMR_STATUS_IDLE)
									{
										setDMRStatus(DMR_STATUS_RX);
										uint32_t src = (RxData[5U] << 16) | (RxData[6U] << 8) | (RxData[7U] << 0);
										uint32_t dst = (RxData[8U] << 16) | (RxData[9U] << 8) | (RxData[10U] << 0);

										ui_dmr_start(src, dst, 1);
									}

									processDMRVoiceFrame(RxData + 20);

									return TRUE;
								}
								else
								{
									if (frameType == VOICE_SYNC)
									{
										if (getDMRStatus() == DMR_STATUS_IDLE)
										{
											setDMRStatus(DMR_STATUS_RX);
											uint32_t src = (RxData[5U] << 16) | (RxData[6U] << 8) | (RxData[7U] << 0);
											uint32_t dst = (RxData[8U] << 16) | (RxData[9U] << 8) | (RxData[10U] << 0);

											ui_dmr_start(src, dst, 1);
										}
									}
									else if (frameType == DATA_SYNC)
									{
										if (dataType == VOICE_HEADER)
										{
											//audio start
											uint32_t src = (RxData[5U] << 16) | (RxData[6U] << 8) | (RxData[7U] << 0);
											uint32_t dst = (RxData[8U] << 16) | (RxData[9U] << 8) | (RxData[10U] << 0);
											setDMRStatus(DMR_STATUS_RX);
											ui_dmr_start(src, dst, 1);

											return TRUE;
										}
										else if (dataType == VOICE_TERMINATOR)
										{
											//audio end
											audio_stop();
											setDMRStatus(DMR_STATUS_IDLE);

											uint32_t src = (RxData[5U] << 16) | (RxData[6U] << 8) | (RxData[7U] << 0);
											uint32_t dst = (RxData[8U] << 16) | (RxData[9U] << 8) | (RxData[10U] << 0);

											ui_dmr_stop(src, dst, 1);
											return TRUE;
										}
									}
								}
							}
						}
					}

				  break;
		default:
				  ;
	}


	return TRUE;
}

gboolean network_tick(void)
{

	if (dmrnet_status == RUNNING)
	{
		timeoutInactivity++;
		if (timeoutInactivity >= INACTIVITY_TICKS && lastSrc != 0)
		{
			lastSrc = 0;
			timeoutInactivity = 0;
			ui_dmr_stop(0, 0, 1);
		}

		timerPing ++;
		if (timerPing >= PING_TICKS)
		{
			timerPing = 0;

			writePing();
		}

		timeoutPong++;
		if (timeoutPong >= PONG_TICKS)
		{
			dmrnet_status = WAITING_CONNECT;
			ui_net_connection(WAITING_CONNECT);
			timeout_reconnect = 0;

		}
	}
	else
	{
		timeout_reconnect++;
		if (timeout_reconnect >= RECONNECT_TICKS)
		{
			timeout_reconnect = 0;
			reconnect();
		}
	}

	return TRUE;
}


bool network_send(uint8_t * data, unsigned int length)
{
	GError *error = NULL;
	g_socket_send (socket, (gchar *)data, (gsize) length, NULL, &error);

	return true;
}

bool writePing(void)
{
	memcpy(TxData + 0U, "RPTPING", 7U);
	uint8_t *p = (uint8_t *) &settings.repeaterId;
	TxData[7] = (uint8_t) p[3];
	TxData[8] = (uint8_t) p[2];
	TxData[9] = (uint8_t) p[1];
	TxData[10] = (uint8_t) p[0];

	return network_send(TxData, 11U);
}

bool writeConfig(void)
{
	uint8_t *buffer = TxData;

	const char software[] = "MMDVM_DMO";
	char slots = '4';

	memcpy(buffer + 0U, "RPTC", 4U);

	uint8_t *p = (uint8_t *) &settings.repeaterId;
	buffer[4] = (uint8_t) p[3];
	buffer[5] = (uint8_t) p[2];
	buffer[6] = (uint8_t) p[1];
	buffer[7] = (uint8_t) p[0];


	char latitude[] = 	" 41.4016";
	char longitude[] = 	"  -2.1830";

	unsigned int power = 10;
	int height = 0;

	sprintf( (char *) buffer + 8U, (char *) "%-8.8s%09u%09u%02u%02u%8.8s%9.9s%03d%-20.20s%-19.19s%c%-124.124s%-40.40s%-40.40s",
		settings.callsign,
		449000000, 444000000, power,
		1, latitude, longitude, height, "Earth",
		"OPENDMR", slots, "github.com/ea3ihi", "20200316", software);

	return network_send(buffer, 302U);
}

bool writeAuthorisation(void)
{
	size_t size = strlen((char *) settings.password);
	uint8_t *in = RxData;
	uint8_t *out =TxData;

	uint16_t inLength = size + sizeof(uint32_t);

	memcpy(in, (uint8_t *) &salt, sizeof(uint32_t));

	for (size_t i = 0U; i < size; i++)
		in[i + sizeof(uint32_t)] = settings.password[i];


	memcpy(out + 0U, "RPTK", 4U);
	uint8_t *p = (uint8_t *) &settings.repeaterId;
	out[4] = (uint8_t) p[3];
	out[5] = (uint8_t) p[2];
	out[6] = (uint8_t) p[1];
	out[7] = (uint8_t) p[0];

	SHA256_CTX ctx;

	SHA256_Init(&ctx);

	SHA256_Update(&ctx, in, inLength);
	SHA256_Final(out + 8U, &ctx);

	return network_send(TxData, 40U);
}

bool writeLogin(void)
{
	memcpy(TxData + 0U, "RPTL", 4U);
	uint8_t *p = (uint8_t *) &settings.repeaterId;

	TxData[4] = (uint8_t) p[3];
	TxData[5] = (uint8_t) p[2];
	TxData[6] = (uint8_t) p[1];
	TxData[7] = (uint8_t) p[0];

	dmrnet_status = WAITING_LOGIN;
	return network_send(TxData, 8U);
}



void activateTG(uint32_t src, uint32_t dst){

	uint8_t dmrData[53] = {0};

	//send VOICE HEADER
	createVoiceHeader(src, dst, dmrData);
	network_send(dmrData, 53);

	//send VOICE TERMINATOR
	createVoiceTerminator(src, dst, dmrData);
	dmrData[4] = 2; //seq
	network_send(dmrData, 53);
}

void createVoiceHeader(uint32_t src, uint32_t dst, uint8_t *dataOut)
{
	uint8_t dmrData[53] = {0};
	uint8_t *p;

	dmrData[0] = 'D';
	dmrData[1] = 'M';
	dmrData[2] = 'R';
	dmrData[3] = 'D';

	dmrData[4] = 0x01; //seq

	//src
	p = (uint8_t *) &src;
	dmrData[5]= (uint8_t) p[2];
	dmrData[6]= (uint8_t) p[1];
	dmrData[7]= (uint8_t) p[0];

	//dst
	p = (uint8_t *) &dst;
	dmrData[8]= (uint8_t) p[2];
	dmrData[9]= (uint8_t) p[1];
	dmrData[10]= (uint8_t) p[0];

	//repeaterid
	p = (uint8_t *) &settings.repeaterId;
	dmrData[11]= (uint8_t) p[3];
	dmrData[12]= (uint8_t) p[2];
	dmrData[13]= (uint8_t) p[1];
	dmrData[14]= (uint8_t) p[0];

	//bit fields
	dmrData[15]= 0xA1; //data2 frame 2 group call slot 0 voice header

	//StreamId
	uint32_t streamId = src+dst;
	dmrData[16]= (streamId >> 24) & 0xFF;
	dmrData[17]= (streamId >> 16) & 0xFF;
	dmrData[18]= (streamId >> 8) & 0xFF;
	dmrData[19]= (streamId >> 0) & 0xFF;

	//LC data
	DMRLC_T lc;

	memset(&lc, 0, sizeof(DMRLC_T));// clear automatic variable

	lc.srcId = src;
	lc.dstId = dst;
	lc.FLCO = 0;// Private or group call

	// Encode the src and dst Ids etc
	if (!DMRFullLC_encode(&lc, &dmrData[20], DT_VOICE_LC_HEADER)) // Encode the src and dst Ids etc
	{
		return;
	}

	for (uint8_t i = 0U; i < 8U; i++)
	{
		dmrData[i + 20+ 12U] = (dmrData[i + 20 + 12U] & ~LC_SYNC_MASK_FULL[i]) | VOICE_LC_SYNC_FULL[i];
	}

	memcpy(dataOut, dmrData, 53);

}

void createVoiceTerminator(uint32_t src, uint32_t dst, uint8_t *dataOut)
{
	uint8_t dmrData[53] = {0};
	uint8_t *p;

	dmrData[0] = 'D';
	dmrData[1] = 'M';
	dmrData[2] = 'R';
	dmrData[3] = 'D';

	dmrData[4] = 0x01; //seq

	//src
	p = (uint8_t *) &src;
	dmrData[5]= (uint8_t) p[2];
	dmrData[6]= (uint8_t) p[1];
	dmrData[7]= (uint8_t) p[0];

	//dst
	p = (uint8_t *) &dst;
	dmrData[8]= (uint8_t) p[2];
	dmrData[9]= (uint8_t) p[1];
	dmrData[10]= (uint8_t) p[0];

	//repeaterid
	p = (uint8_t *) &settings.repeaterId;
	dmrData[11]= (uint8_t) p[3];
	dmrData[12]= (uint8_t) p[2];
	dmrData[13]= (uint8_t) p[1];
	dmrData[14]= (uint8_t) p[0];

	//bit fields
	dmrData[15]= 0xA2; //data2 frame 2 group call slot 0 voice terminator

	//StreamId
	uint32_t streamId = src+dst;
	dmrData[16]= (streamId >> 24) & 0xFF;
	dmrData[17]= (streamId >> 16) & 0xFF;
	dmrData[18]= (streamId >> 8) & 0xFF;
	dmrData[19]= (streamId >> 0) & 0xFF;

	//LC data
	DMRLC_T lc;

	memset(&lc, 0, sizeof(DMRLC_T));// clear automatic variable

	lc.srcId = src;
	lc.dstId = dst;
	lc.FLCO = 0;// Private or group call

	// Encode the src and dst Ids etc
	if (!DMRFullLC_encode(&lc, &dmrData[20], DT_TERMINATOR_WITH_LC)) // Encode the src and dst Ids etc
	{
		return;
	}

	for (uint8_t i = 0U; i < 8U; i++)
	{
		dmrData[i + 20+ 12U] = (dmrData[i + 20 + 12U] & ~LC_SYNC_MASK_FULL[i]) | TERMINATOR_LC_SYNC_FULL[i];
	}

	memcpy(dataOut, dmrData, 53);

}


