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

  const s8 *get_channel(int channel) const { return &channels[channel][0]; }

private:
  Memory &memory;
  AudioOut aout;

  static int freq_to_hz(int freq);
  static int snd_len_to_cycles(int len);
  static int envelope_cycles_per_step(int len);

  void reset();

  void restart_channel(int channel);
  void restart_channel_envelope(int channel);

  void update_channel1();
  void update_channel2();
  void update_channel3();
  void update_channel4();

  s8 channels[4][400];

  const s8 square_wave[4][8] =
  {
    {-0x7f,  0x7f,  0x7f,  0x7f,  0x7f,  0x7f,  0x7f,  0x7f},
    {-0x7f, -0x7f,  0x7f,  0x7f,  0x7f,  0x7f,  0x7f,  0x7f},
    {-0x7f, -0x7f, -0x7f, -0x7f,  0x7f,  0x7f,  0x7f,  0x7f},
    {-0x7f, -0x7f, -0x7f, -0x7f, -0x7f, -0x7f,  0x7f,  0x7f},
  };

  int volume[2];
  u8 terminal_selection = 0;
  bool sound_enabled = false;

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
    int envelope_steps;

    //              ___
    //            ^ |  |__
    //  initial   | |     |__
    //  envelope  | |        |__
    //  volume    | |           |__
    //            | |              |__
    //            v |                 |_____
    //               <--------------->
    //                  number of     n/64 seconds
    //                  steps (n)      long each

    int freq;

    bool on;
  } channel_data[4];
  bool wave_enabled = false;

  struct
  {
    int counter;
    int envelope_counter;
    int envelope_step;
  } channel_state[4];
};
