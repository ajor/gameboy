#pragma once

#include "types.h"

#include "glfw/openal.h"

class Memory;

class Audio
{
public:
  Audio() = delete;
  explicit Audio(Memory &mem) : memory(mem),
                                aout(*this) { }

  u8 read_byte(uint address) const;
  void write_byte(uint address, u8 value);

  const u8 *get_channel_1() const { return &channels[0][0]; }
  const u8 *get_channel_2() const { return &channels[1][0]; }
  const u8 *get_channel_3() const { return &channels[2][0]; }

private:
  Memory &memory;
  AudioOut aout;

  void update_channel1();
  void update_channel2();
  void update_channel3();

  u8 channels[4][400];

  const u8 square_wave[4][8] =
  {
    {   0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {   0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0, 0xff, 0xff},
  };

  struct
  {
    int wave_duty;
    int snd_len;

    int sweep_time;
    int sweep_direction;
    int sweep_count;

    int envelope_volume;
    int envelope_direction;
    int envelope_count;

    int freq;
  } channel_data[4];
  bool wave_enabled = false;
};
