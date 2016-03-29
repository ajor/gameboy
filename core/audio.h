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

  void update(uint cycles);

  u8 read_byte(uint address) const;
  void write_byte(uint address, u8 value);

  const u8 *get_channel_1() const { return &channels[0][0]; }
  const u8 *get_channel_2() const { return &channels[1][0]; }
  const u8 *get_channel_3() const { return &channels[2][0]; }
  const u8 *get_channel_4() const { return &channels[3][0]; }

private:
  Memory &memory;
  AudioOut aout;

  static int freq_to_hz(int freq);
  static int length_to_cycles(int cnt);

  void update_channel1();
  void update_channel2();
  void update_channel3();
  void update_channel4();

  u8 channels[4][400];

  const u8 square_wave[4][8] =
  {
    {   0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {   0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0, 0xff, 0xff},
  };

  int volume[2];
  u8 terminal_selection = 0;
  bool sound_enabled = true;

  u8 wave_data[16];

  struct
  {
    int wave_duty;
    int snd_len;
    bool counter_enabled;

    int sweep_time;
    int sweep_direction;
    int sweep_count;

    int envelope_volume;
    int envelope_direction;
    int envelope_count;

    int freq;
  } channel_data[4];
  bool wave_enabled = false;

  struct
  {
    int counter = 0;
  } channel_state[4];
};
