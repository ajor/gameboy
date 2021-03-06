#pragma once

#if defined(__linux__) || defined(__EMSCRIPTEN__)
#include <AL/al.h>
#include <AL/alc.h>
#elif defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

class Audio;

class AudioOut
{
public:
  AudioOut() = delete;
  explicit AudioOut(Audio &audio_);
  ~AudioOut();

  void play_channel(int channel, int freq);

  void stop_channel(int channel);

  bool muted = false;
  bool debug = false;

private:
  Audio &audio;

  ALCdevice *dev;
  ALCcontext *ctx;
  ALuint source[4], buffer[4];
};
