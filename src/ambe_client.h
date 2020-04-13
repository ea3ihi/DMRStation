

#ifndef _AMBE_CLIENT_H_
#define _AMBE_CLIENT_H_

void ambeclient_init(void);
bool ambe_send(uint8_t * data, unsigned int length);
gboolean ambeInCallback(GSocket *source, GIOCondition condition, gpointer data);

#endif
