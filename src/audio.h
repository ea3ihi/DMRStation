
#define AUDIO_BUFFERS	32
#define AUDIO_BUFFER_SIZE	320

void audio_init(void);
void audio_tick(void);
void audio_start(void);
void audio_next(void);
void audio_stop(void);
uint16_t* audio_get_buffer(void);
void audio_advance_buffer(void);

void test_audio(void);
