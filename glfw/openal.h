#pragma once

#include <AL/al.h>
#include <AL/alc.h>

class Audio;

class AudioOut
{
public:
  AudioOut() = delete;
  explicit AudioOut(Audio &audio_);
  ~AudioOut();

  void update_channel1(int freq);
  void update_channel2(int freq);
  void update_channel3();

private:
  Audio &audio;

  ALCdevice *dev;
  ALCcontext *ctx;
  ALuint source[4], buffer[4];
};
