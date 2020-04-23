

#ifndef _AMBE_CLIENT_H_
#define _AMBE_CLIENT_H_

void ambeclient_init(void);
void ambeclient_deinit(void);
bool ambe_send(const uint8_t * data, unsigned int length);
gboolean ambeInCallback(GSocket *source, GIOCondition condition, gpointer data);
void convert49BitTo72BitAMBE( uint8_t *inAmbe49 , uint8_t *outAmbe72);

#endif
