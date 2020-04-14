

#include "main.h"


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
				uint16_t * buffer = audio_get_buffer();

				memcpy(buffer, (gchar*) RxData, dataLen);
				audio_advance_buffer();
				return TRUE;
			} else if (dataLen == 7){
				//ambe data
				return TRUE;
			}
			break;
		default:
			break;
	}

	return TRUE;
}


bool ambe_send(uint8_t * data, unsigned int length)
{
	GError *error = NULL;
	g_socket_send (ambeSocket, (gchar *)data, (gsize) length, NULL, &error);

	return true;
}
