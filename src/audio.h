




#define AUDIO_BUFFERS	0
#define AUDIO_BUFFER_SIZE	160	//in samples, not bytes

#include <pulse/context.h>

void audio_init(void);
void audio_deinit(void);
void audio_start(void);
void audio_stop(void);
void audio_record_start(void);
void audio_record_stop(void);
void audioBufferFlush(void);

void test_audio(void);

void audioBufferWrite(const uint8_t *data, uint32_t size);
uint32_t audioBufferRead(uint8_t *data, uint32_t size);

void context_state_callback(pa_context *c, void *userdata);
void audio_write_buffer(const uint8_t *data, uint16_t size);
void setVolume(uint32_t volume);
