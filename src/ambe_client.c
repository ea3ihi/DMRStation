

#include "main.h"

#define AMBE_BUFFERS		10
#define AMBE_BUFFER_SIZE	7

uint8_t ambeBuffer[AMBE_BUFFERS][AMBE_BUFFER_SIZE];
uint8_t bufferReadIndex = 0;
uint8_t bufferWriteIndex = 0;
uint8_t bufferCount = 0;

void addAmbeBuffer(uint8_t * dataIn);
void readAmbeBuffer(uint8_t * dataOut);
void processAmbeBuffer();

GSocket *ambeSocket;

uint8_t TxData [400];
uint8_t RxData [400];

extern AppSettingsStruct_t settings;

void ambeclient_init(void)
{
	GInetAddress *address = g_inet_address_new_from_string((gchar *) settings.ambeServerHost);
	GSocketAddress *socket_address = g_inet_socket_address_new(address, settings.ambeServerPort);

	GInetAddress *localAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4) ;
	GSocketAddress *localSocketAddress = g_inet_socket_address_new(localAddress, settings.ambeServerPort + 1);

	GError *error = NULL;

	ambeSocket = g_socket_new (G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, 0, &error);

	g_socket_bind (ambeSocket, localSocketAddress, TRUE, &error);

	g_socket_connect(ambeSocket, socket_address, NULL, &error);

	GSource *source = g_socket_create_source(ambeSocket, G_IO_IN, NULL);
	g_source_set_callback(source, (GSourceFunc) ambeInCallback, NULL,NULL);
	g_source_attach(source, g_main_context_default());
}

void ambeclient_deinit(void)
{
	GError *error = NULL;
	g_socket_close(ambeSocket, &error);

}

gboolean ambeInCallback(GSocket *source, GIOCondition condition, gpointer data)
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
				//ambe data
				addAmbeBuffer(RxData);
				return TRUE;
			}
			break;
		default:
			break;
	}

	return TRUE;
}


bool ambe_send(const uint8_t * data, unsigned int length)
{
	GError *error = NULL;
	g_socket_send (ambeSocket, (gchar *)data, (gsize) length, NULL, &error);

	return true;
}



void addAmbeBuffer(uint8_t * dataIn)
{
	memcpy(&ambeBuffer[bufferWriteIndex][0], dataIn, AMBE_BUFFER_SIZE);
	bufferCount++;

	bufferWriteIndex++;
	bufferWriteIndex %= AMBE_BUFFERS;

	if (bufferCount >=3) {
		processAmbeBuffer();
	}

}

void readAmbeBuffer(uint8_t * dataOut)
{
	memcpy(dataOut, &ambeBuffer[bufferReadIndex][0], AMBE_BUFFER_SIZE);
	bufferCount--;
	bufferReadIndex++;
	bufferReadIndex %= AMBE_BUFFERS;
}

void processAmbeBuffer()
{
	uint8_t ambe49[3][7];
	uint8_t ambe72[3][9];

	//get buffers
	readAmbeBuffer(&ambe49[0][0]);
	readAmbeBuffer(&ambe49[1][0]);
	readAmbeBuffer(&ambe49[2][0]);

	//ambe 49 to 72
	convert49BitTo72BitAMBE(&ambe49[0][0], &ambe72[0][0]);
	convert49BitTo72BitAMBE(&ambe49[1][0], &ambe72[1][0]);
	convert49BitTo72BitAMBE(&ambe49[2][0], &ambe72[2][0]);

	prepareVoiceFrame((uint8_t *) ambe72);
}



void convert49BitTo72BitAMBE( uint8_t *inAmbe49 , uint8_t *outAmbe72)
{
	uint8_t	ambe49bits[49];
	char ambeFrame[4][24];

	memset (ambeFrame, 0, 24*4);

	uint8_t tmp = 0;
	int pos = 0;
	for (int j = 0; j<7; j++)
	{
		for (int i = 7; i > -1 ; i--)
		{
			tmp = inAmbe49[pos >>3] & ( 1 << i );
			ambe49bits[pos] = tmp ? 1 : 0;
			pos++;
		}
	}

	convert49BitAmbeTo72BitFrames(ambe49bits, (uint8_t *) &ambeFrame);    // take raw ambe 49 + ecc and place it into C0-C3
	mbe_demodulateAmbe3600x2450Data(ambeFrame);         // demodulate C1
	interleave((uint8_t *)ambeFrame, outAmbe72);                      // Re-interleave it, returning 72 bits

}




