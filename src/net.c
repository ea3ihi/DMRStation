

#include "main.h"


GSocket *socket;

uint8_t TxData [400];
uint8_t RxData [55];

#define PING_TICKS 5
#define PONG_TICKS 15
#define RECONNECT_TICKS 5

uint16_t timerPing;
uint16_t timeoutPong;
uint16_t timeout_reconnect;

uint32_t salt;
uint32_t ticks;

enum DMR_STATUS dmr_status = WAITING_CONNECT;

extern AppSettingsStruct_t settings;

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
									switch (dmr_status) {
										case WAITING_LOGIN:
											memcpy((uint8_t *) &salt, RxData + 6U, sizeof(uint32_t));
											writeAuthorisation();
											dmr_status = WAITING_AUTHORISATION;
											break;
										case WAITING_AUTHORISATION:
											writeConfig();
											dmr_status = WAITING_CONFIG;
											break;
										case WAITING_CONFIG:
											dmr_status = RUNNING;
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
							dmr_status = WAITING_LOGIN;
						} else if (memcmp(RxData, "DMRD",  4U) == 0) {
							//DMR Packet
							//do we have a complete packet
							if (dataLen > 52)
							{
								//is it a voice packet?
								uint8_t frameType = (RxData[15] & 0x30) >> 4;
								uint8_t dataType = RxData[15] & 0x0F;
								if (frameType == VOICE)
								{

									processDMRVoiceFrame(RxData + 20);

									return TRUE;
								}
								else
								{
									if (frameType == DATA_SYNC)
									{
										if (dataType == VOICE_HEADER)
										{
											//audio start
											return TRUE;
										} else if (dataType == VOICE_TERMINATOR)
										{
											//audio end
											audio_stop();
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

	if (dmr_status == RUNNING)
	{
		timerPing ++;
		if (timerPing >= PING_TICKS)
		{
			timerPing = 0;

			writePing();
		}

		timeoutPong++;
		if (timeoutPong >= PONG_TICKS)
		{
			dmr_status = WAITING_CONNECT;
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
	uint8_t *p = (uint8_t *) &settings.dmrId;
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

	uint8_t *p = (uint8_t *) &settings.dmrId;
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
		"stm32 X", slots, "github.com/ea3ihi", "20200316", software);

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
	uint8_t *p = (uint8_t *) &settings.dmrId;
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
	uint8_t *p = (uint8_t *) &settings.dmrId;

	TxData[4] = (uint8_t) p[3];
	TxData[5] = (uint8_t) p[2];
	TxData[6] = (uint8_t) p[1];
	TxData[7] = (uint8_t) p[0];

	dmr_status = WAITING_LOGIN;
	return network_send(TxData, 8U);
}

