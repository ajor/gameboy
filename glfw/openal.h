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

  void play_channel(int channel, int freq);

  void stop_channel(int channel);

  bool muted = false;

private:
  Audio &audio;

  ALCdevice *dev;
  ALCcontext *ctx;
  ALuint source[4], buffer[4];
};
