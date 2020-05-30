

#ifndef _CODEC2_CLIENT_H_
#define _CODEC2_CLIENT_H_

void codec2client_init(void);
void codec2client_deinit(void);
bool codec2_send(const uint8_t * data, unsigned int length);
gboolean codec2InCallback(GSocket *source, GIOCondition condition, gpointer data);
void convert49BitTo72BitCodec2( uint8_t *inCodec2_49 , uint8_t *outCodec2_72);

#endif
