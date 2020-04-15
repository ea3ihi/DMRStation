


#ifndef _APP_NET_H_
#define _APP_NET_H_

#include <stdint.h>
#include <stdbool.h>

gboolean dataInCallback(GSocket *source, GIOCondition condition, gpointer data);
void net_init(void);

enum DMR_STATUS {
		WAITING_CONNECT,
		WAITING_LOGIN,
		WAITING_AUTHORISATION,
		WAITING_CONFIG,
		WAITING_OPTIONS,
		RUNNING
	};

bool network_send(uint8_t * data, unsigned int length);
void reconnect(void);
bool writeLogin(void);
bool writeAuthorisation(void);
bool writeConfig(void);
bool writePing(void);
gboolean network_tick(void);
void activateTG(uint32_t src, uint32_t dst);

#endif
