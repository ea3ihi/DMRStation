

#include <pulse/context.h>


#define AUDIO_BUFFERS	0
#define AUDIO_BUFFER_SIZE	160

void audio_init(void);
void audio_deinit(void);
void audio_start(void);
void audio_stop(void);


void test_audio(void);

static void context_state_callback(pa_context *c, void *userdata);
static void audio_connect(void);
void audio_write_buffer(uint8_t *data, uint16_t size);
