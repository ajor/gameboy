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

void AudioOut::play_channel1(int freq)
{
  printf("Hello1 %d\n", freq);

  alSourceStop(source[0]);
  alDeleteSources(1, &source[0]);
  alDeleteBuffers(1, &buffer[0]);
  alGenBuffers(1, &buffer[0]);
  alGenSources(1, &source[0]);

  alBufferData(buffer[0], AL_FORMAT_MONO8, audio.get_channel_1(), 8, freq*8);

  alSourcei(source[0], AL_BUFFER, buffer[0]);
  alSourcei(source[0], AL_LOOPING, AL_TRUE);

  alSourcePlay(source[0]);
}

void AudioOut::play_channel2(int freq)
{
  printf("Hello2 %d\n", freq);

  alSourceStop(source[1]);
  alDeleteSources(1, &source[1]);
  alDeleteBuffers(1, &buffer[1]);
  alGenBuffers(1, &buffer[1]);
  alGenSources(1, &source[1]);

  alBufferData(buffer[1], AL_FORMAT_MONO8, audio.get_channel_2(), 8, freq*8);

  alSourcei(source[1], AL_BUFFER, buffer[1]);
  alSourcei(source[1], AL_LOOPING, AL_TRUE);

  alSourcePlay(source[1]);
}

void AudioOut::play_channel3()
{
//  printf("Hello3\n");
//  alBufferData(buffer[2], AL_FORMAT_MONO8, audio.get_channel_3(), 400, 44100);
//
//  alSourcei(source[2], AL_BUFFER, buffer[2]);
//  alSourcei(source[2], AL_LOOPING, AL_TRUE);
//
//  alSourcePlay(source[2]);
}

void AudioOut::stop_channel1()
{
  printf("stopppped 1\n");
  alSourceStop(source[0]);
  alDeleteSources(1, &source[0]);
  alDeleteBuffers(1, &buffer[0]);
}

void AudioOut::stop_channel2()
{
  printf("stopppped 2\n");
  alSourceStop(source[1]);
  alDeleteSources(1, &source[1]);
  alDeleteBuffers(1, &buffer[1]);
}

void AudioOut::stop_channel3()
{
  alSourceStop(source[2]);
  alDeleteSources(1, &source[2]);
  alDeleteBuffers(1, &buffer[2]);
}
