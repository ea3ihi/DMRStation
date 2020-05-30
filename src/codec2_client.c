

#include "main.h"

#define CODEC2_BUFFERS		10
#define CODEC2_BUFFER_SIZE	7

uint8_t codec2Buffer[CODEC2_BUFFERS][CODEC2_BUFFER_SIZE];
uint8_t bufferReadIndex = 0;
uint8_t bufferWriteIndex = 0;
uint8_t bufferCount = 0;

void addCodec2Buffer(uint8_t * dataIn);
void readCodec2Buffer(uint8_t * dataOut);
void processCodec2Buffer();

GSocket *codec2Socket;

uint8_t TxData [400];
uint8_t RxData [400];

extern AppSettingsStruct_t settings;

void codec2client_init(void)
{
	GInetAddress *address = g_inet_address_new_from_string((gchar *) settings.codec2ServerHost);
	GSocketAddress *socket_address = g_inet_socket_address_new(address, settings.codec2ServerPort);

	GInetAddress *localAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4) ;
	GSocketAddress *localSocketAddress = g_inet_socket_address_new(localAddress, settings.codec2ServerPort + 1);

	GError *error = NULL;

	codec2Socket = g_socket_new (G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, 0, &error);

	g_socket_bind (codec2Socket, localSocketAddress, TRUE, &error);

	g_socket_connect(codec2Socket, socket_address, NULL, &error);

	GSource *source = g_socket_create_source(codec2Socket, G_IO_IN, NULL);
	g_source_set_callback(source, (GSourceFunc) codec2InCallback, NULL,NULL);
	g_source_attach(source, g_main_context_default());
}

void codec2client_deinit(void)
{
	GError *error = NULL;
	g_socket_close(codec2Socket, &error);

}

gboolean codec2InCallback(GSocket *source, GIOCondition condition, gpointer data)
{
	int dataLen = 0;

	switch (condition)
	{
		case G_IO_IN:
			dataLen = g_socket_receive(source, (gchar*) RxData, sizeof(RxData), NULL, NULL);

			if (dataLen == 320)
			{
				//PCM data
				audio_write_buffer(RxData, 320);
				return TRUE;
			} else if (dataLen == 7){
				//codec2 data
				addCodec2Buffer(RxData);
				return TRUE;
			}
			break;
		default:
			break;
	}

	return TRUE;
}


bool codec2_send(const uint8_t * data, unsigned int length)
{
	GError *error = NULL;
	g_socket_send (codec2Socket, (gchar *)data, (gsize) length, NULL, &error);

	return true;
}



void addCodec2Buffer(uint8_t * dataIn)
{
	memcpy(&codec2Buffer[bufferWriteIndex][0], dataIn, CODEC2_BUFFER_SIZE);
	bufferCount++;

	bufferWriteIndex++;
	bufferWriteIndex %= CODEC2_BUFFERS;

	if (bufferCount >=3) {
		processCodec2Buffer();
	}

}

void readCodec2Buffer(uint8_t * dataOut)
{
	memcpy(dataOut, &codec2Buffer[bufferReadIndex][0], CODEC2_BUFFER_SIZE);
	bufferCount--;
	bufferReadIndex++;
	bufferReadIndex %= CODEC2_BUFFERS;
}

void processCodec2Buffer()
{
	uint8_t codec2_49[3][7];
	uint8_t codec2_72[3][9];

	//get buffers
	readCodec2Buffer(&codec2_49[0][0]);
	readCodec2Buffer(&codec2_49[1][0]);
	readCodec2Buffer(&codec2_49[2][0]);

	//codec2 49 to 72
	convert49BitTo72BitCodec2(&codec2_49[0][0], &codec2_72[0][0]);
	convert49BitTo72BitCodec2(&codec2_49[1][0], &codec2_72[1][0]);
	convert49BitTo72BitCodec2(&codec2_49[2][0], &codec2_72[2][0]);

	prepareVoiceFrame((uint8_t *) codec2_72);
}



void convert49BitTo72BitCodec2( uint8_t *inCodec2_49 , uint8_t *outCodec2_72)
{
	uint8_t	codec2_49bits[49];
	char codec2Frame[4][24];

	memset (codec2Frame, 0, 24*4);

	uint8_t tmp = 0;
	int pos = 0;
	for (int j = 0; j<7; j++)
	{
		for (int i = 7; i > -1 ; i--)
		{
			tmp = inCodec2_49[pos >>3] & ( 1 << i );
			codec2_49bits[pos] = tmp ? 1 : 0;
			pos++;
		}
	}

	convert49BitAmbeTo72BitFrames(codec2_49bits, (uint8_t *) &codec2Frame);    // take raw ambe 49 + ecc and place it into C0-C3
	mbe_demodulateAmbe3600x2450Data(codec2Frame);         // demodulate C1
	interleave((uint8_t *)codec2Frame, outCodec2_72);                      // Re-interleave it, returning 72 bits

}




