#include "openal.h"
#include "core/gameboy.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <AL/al.h>
#include <AL/alc.h>

AudioOut::AudioOut(Audio &audio_) : audio(audio_)
{
  dev = alcOpenDevice(NULL);
  if (!dev)
  {
    fprintf(stderr, "no device\n");
    return;
  }

  ctx = alcCreateContext(dev, NULL);
  alcMakeContextCurrent(ctx);
  if (!ctx)
  {
    fprintf(stderr, "no context\n");
    return;
  }

  alGenBuffers(4, buffer);
  alGenSources(4, source);
  if (alGetError() != AL_NO_ERROR)
  {
    fprintf(stderr, "error generating buffers\n");
    return;
  }
}

AudioOut::~AudioOut()
{
  alDeleteSources(4, source);
  alDeleteBuffers(4, buffer);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(ctx);
  alcCloseDevice(dev);
}

void AudioOut::play_channel(int channel, int freq)
{
  printf("playing %d at %dhz\n", channel, freq);

  stop_channel(channel);
  alGenBuffers(1, &buffer[channel]);
  alGenSources(1, &source[channel]);

  alBufferData(buffer[channel], AL_FORMAT_MONO8, audio.get_channel(channel), 8, freq*8);

  alSourcei(source[channel], AL_BUFFER, buffer[channel]);
  alSourcei(source[channel], AL_LOOPING, AL_TRUE);

  alSourcePlay(source[channel]);
}

void AudioOut::stop_channel(int channel)
{
  printf("stopppped %d\n", channel);
  alSourceStop(source[channel]);
  alDeleteSources(1, &source[channel]);
  alDeleteBuffers(1, &buffer[channel]);
}
