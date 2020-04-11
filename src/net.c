

#include "main.h"


GSocket *socket;
GIOChannel *ioc;


void net_test(void)
{


	GInetAddress *address = g_inet_address_new_from_string("192.168.3.254");
	GSocketAddress *socket_address = g_inet_socket_address_new(address, 62031);

	GInetAddress *localAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4) ;
	GSocketAddress *localSocketAddress = g_inet_socket_address_new(localAddress, 62031);

	GError *error = NULL;

	socket = g_socket_new (G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, 0, &error);

	g_socket_bind (socket, localSocketAddress, TRUE, &error);

	g_socket_connect(socket, socket_address, NULL, &error);

	GSource *source = g_socket_create_source(socket, G_IO_IN, NULL);

	g_source_set_callback(source, (GSourceFunc) dataInCallback, NULL,NULL);

	g_source_attach(source, g_main_context_default());


	g_socket_send (socket, "RPTL123456", 10, NULL, NULL);

}


gboolean dataInCallback(GSocket *source, GIOCondition condition, gpointer data)
{
	int Input_Length = 0;
	gchar Receive_Buffer[600];

	switch (condition)
	{
	case G_IO_IN:
	          Input_Length = g_socket_receive(source, (gchar*) Receive_Buffer, 600, NULL, NULL);

	          break;
	default:
	          ;
	}


	return TRUE;
}


