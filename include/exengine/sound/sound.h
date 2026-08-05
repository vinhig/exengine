/* sound
  The sound module, handles
  loading and playing sound files.

  Based on OpenAL (the good one),
  still requires the ability to
  stream sound sources. Currently,
  also only supports ogg sound files.

  This needs a lot of work:
  * Decode other formats
  * Add source streaming
  * Ability to change individual sources values,
    such as if it should loop, the volume and
    position, etc.
*/

#pragma once

#define EX_DEVICE_LEN 32

#include <MojoAL/AL/al.h>
#include <MojoAL/AL/alc.h>

#include <stdlib.h>

typedef enum {
  EX_SOURCE_STATIC = 0,
  EX_SOURCE_STREAMING = 1,
  EX_SOURCE_LOOPING = 1,
  EX_SOURCE_LEN
} ex_source_type_e;

typedef struct {
  ALCdevice *output, *input;
  ALCcontext *context;
} ex_sound_t;

typedef struct {
  ALuint id, buffers[3];
  ALint ready_buffers[3];
  int looping, stopped;

  // for streaming source
  void *decoder;
  int streaming, channels, rate;
  size_t sample, samples;
  short *decode_buffer;
  size_t decode_buffer_bytes, decode_buffer_shorts;
} ex_source_t;

typedef enum {
  EX_SOUND_WAV,
  EX_SOUND_OGG,
} ex_sound_e;

extern ex_sound_t ex_sound;

typedef struct {
  ALCchar names[EX_DEVICE_LEN][512];
  size_t len;
} ex_sound_devices_t;

/**
 * @brief Initialize the sound module.
 */
void ex_sound_init();

/**
 * @brief List available audio devices.
 * @param list  output device list
 * @param param ALenum parameter to query
 */
void ex_sound_list_devices(ex_sound_devices_t *list, const ALenum param);

/**
 * @brief Set the audio output device.
 * @param device device name string
 */
void ex_sound_set_output(const ALCchar *device);

/**
 * @brief Load and decode a sound source into memory.
 * @param path    the sound file to load
 * @param type    source type (EX_SOURCE_STATIC or EX_SOURCE_STREAMING)
 * @param looping 1 if the sound should loop
 * @return pointer to the new source
 */
ex_source_t *ex_sound_load(const char *path, int type, int looping);

/**
 * @brief Restart a sound source.
 * @param s sound source to restart
 */
void ex_sound_restart(ex_source_t *s);

/**
 * @brief Play a sound source.
 * @param s source to play
 */
void ex_sound_play(ex_source_t *s);

/**
 * @brief Clean up a sound source.
 * @param s the source to destroy
 */
void ex_sound_destroy(ex_source_t *s);

/**
 * @brief Clean up the sound module.
 */
void ex_sound_exit();

/**
 * @brief Set the master volume.
 * @param vol volume from 0.0 to 1.0
 */
static inline void ex_sound_master_volume(float vol) {
  alListenerf(AL_GAIN, vol);
};

/**
 * @brief Check if a source is currently playing.
 * @param s the source to check
 * @return 1 if playing, 0 otherwise
 */
static inline int ex_sound_playing(ex_source_t *s) {
  ALenum state;
  alGetSourcei(s->id, AL_SOURCE_STATE, &state);
  return (state == AL_PLAYING);
}
