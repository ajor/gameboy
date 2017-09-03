#include "audio.h"
#include "memory.h"

#include <stdlib.h>
#include <stdio.h>

int Audio::freq_to_hz(int freq)
{
  return 131072 / (2048 - freq);
}

int Audio::snd_len_to_cycles(int len)
{
  return (64 - len) * (4194304 / 256);
}

int Audio::envelope_cycles_per_step(int len)
{
  return len * (4194304 / 64);
}

void Audio::update(uint cycles)
{
  for (int i=0; i<4; i++)
  {
    channel_state[i].counter += cycles;
    channel_state[i].envelope_counter += cycles;

    if (channel_data[i].envelope_steps > 0 && channel_state[i].envelope_step > 0)
    {
      int env_step_length = envelope_cycles_per_step(channel_data[i].envelope_steps);
      if (channel_state[i].envelope_counter > env_step_length)
      {
        channel_state[i].envelope_counter -= env_step_length;
        channel_state[i].envelope_step -= 1;
        if (channel_state[i].envelope_step < 0)
          channel_state[i].envelope_step = 0;
        if (debug)
          printf("step %d\n", channel_state[i].envelope_step);

        // TODO only update correct channel here
        update_channel1();
        update_channel2();
      }
    }
  }
}

void Audio::reset()
{
  write_byte(Memory::IO::NR10, 0);
  write_byte(Memory::IO::NR11, 0);
  write_byte(Memory::IO::NR12, 0);
  write_byte(Memory::IO::NR13, 0);
  write_byte(Memory::IO::NR14, 0);

  write_byte(Memory::IO::NR21, 0);
  write_byte(Memory::IO::NR22, 0);
  write_byte(Memory::IO::NR23, 0);
  write_byte(Memory::IO::NR24, 0);

  write_byte(Memory::IO::NR30, 0);
  write_byte(Memory::IO::NR31, 0);
  write_byte(Memory::IO::NR32, 0);
  write_byte(Memory::IO::NR33, 0);
  write_byte(Memory::IO::NR34, 0);

  write_byte(Memory::IO::NR41, 0);
  write_byte(Memory::IO::NR42, 0);
  write_byte(Memory::IO::NR43, 0);
  write_byte(Memory::IO::NR44, 0);

  write_byte(Memory::IO::NR50, 0);
  write_byte(Memory::IO::NR51, 0);
  write_byte(Memory::IO::NR52, 0);
}

void Audio::restart_channel(int channel)
{
  channel_state[channel].counter = 0;

  channel_data[channel].on = true;
}

void Audio::restart_channel_envelope(int channel)
{
  channel_state[channel].envelope_counter = 0;
  channel_state[channel].envelope_step = channel_data[channel].envelope_steps;
}

void Audio::update_channel1()
{
  if (!sound_enabled ||
      (channel_data[0].counter_enabled &&
       snd_len_to_cycles(channel_data[0].snd_len) < channel_state[0].counter))
  {
    // don't play sound
    aout.stop_channel(0);
    channel_data[0].on = false;
    return;
  }

  int volume = channel_data[0].envelope_volume;
  int steps = channel_data[0].envelope_steps;
  int step = channel_state[0].envelope_step;
  if (steps > 0)
    volume = volume * step/steps;

  if (channel_data[0].envelope_direction == 1)
  {
    // Reverse envelope direction (increase volume with steps)
    volume = channel_data[0].envelope_volume - volume;
  }
  if (debug)
    printf("vol = %d  ", volume);

  for (int i=0; i<8; i++)
  {
    channels[0][i] = square_wave[channel_data[0].wave_duty][i] * volume / 0xf;
    if (debug)
      printf("%d, ", channels[0][i]);
  }
  if (debug)
    puts("\n");

  aout.play_channel(0, freq_to_hz(channel_data[0].freq));
}

void Audio::update_channel2()
{
  if (!sound_enabled ||
      (channel_data[1].counter_enabled &&
       snd_len_to_cycles(channel_data[1].snd_len) < channel_state[1].counter))
  {
    // don't play sound
    aout.stop_channel(1);
    channel_data[1].on = false;
    return;
  }

  int volume = channel_data[1].envelope_volume;
  int steps = channel_data[1].envelope_steps;
  int step = channel_state[1].envelope_step;
  if (steps > 0)
    volume = volume * step/steps;
  if (debug)
    printf("vol = %d  ", volume);
  for (int i=0; i<8; i++)
  {
    channels[1][i] = square_wave[channel_data[1].wave_duty][i] * volume / 0xf;
    if (debug)
      printf("%d, ", channels[1][i]);
  }
  if (debug)
    puts("\n");

  aout.play_channel(1, freq_to_hz(channel_data[1].freq));
}

void Audio::update_channel3()
{
//  for (int i=0; i<=0xf; i++)
//  {
//    channels[3][i] = memory.get8(Memory::IO::WAVE + i);
//  }
//
//  aout.play_channel3();
}

void Audio::update_channel4()
{
}

u8 Audio::read_byte(uint address) const
{
  switch (address)
  {
    case Memory::IO::NR10:
      return (channel_data[0].sweep_time << 4) |
             (channel_data[0].sweep_direction << 3) |
             (channel_data[0].sweep_count);
    case Memory::IO::NR11:
      return channel_data[0].wave_duty << 6; // Low bits are write-only
    case Memory::IO::NR12:
      return (channel_data[0].envelope_volume << 4) |
             (channel_data[0].envelope_direction << 3) |
             (channel_data[0].envelope_steps);
    case Memory::IO::NR13:
      return 0; // Write only
    case Memory::IO::NR14:
      return channel_data[0].counter_enabled << 6;

    case Memory::IO::NR21:
      return channel_data[1].wave_duty << 6; // Low bits are write-only
    case Memory::IO::NR22:
      return (channel_data[1].envelope_volume << 4) |
             (channel_data[1].envelope_direction << 3) |
             (channel_data[1].envelope_steps);
    case Memory::IO::NR23:
      return 0; // Write only
    case Memory::IO::NR24:
      return channel_data[1].counter_enabled << 6;

    case Memory::IO::NR30:
      return wave_enabled << 7;
    case Memory::IO::NR31:
      return channel_data[2].snd_len;
    case Memory::IO::NR32:
      return channel_data[2].envelope_volume << 5;
    case Memory::IO::NR33:
      return 0; // Write only
    case Memory::IO::NR34:
      return channel_data[2].counter_enabled << 6;

    case Memory::IO::NR41:
      return channel_data[3].snd_len;
    case Memory::IO::NR42:
      return (channel_data[3].envelope_volume << 4) |
             (channel_data[3].envelope_direction << 3) |
             (channel_data[3].envelope_steps);
    case Memory::IO::NR43:
      return (channel_data[3].freq << 4) |
             (channel_data[3].sweep_direction << 3) |
             (channel_data[3].sweep_count);
    case Memory::IO::NR44:
      return channel_data[3].counter_enabled << 6;

    case Memory::IO::NR50:
      return (volume[1] << 4) | volume[0];
    case Memory::IO::NR51:
      return terminal_selection;
    case Memory::IO::NR52:
      return (sound_enabled << 7) |
             (channel_data[3].on << 3) |
             (channel_data[2].on << 2) |
             (channel_data[1].on << 1) |
             (channel_data[0].on << 0);

    default:
      if (address >= Memory::IO::WAVE && address <= Memory::IO::WAVE + 0xf)
      {
        return wave_data[address - Memory::IO::WAVE];
      }
      else
      {
        abort();
      }
  }
}

void Audio::write_byte(uint address, u8 value)
{
  switch (address)
  {
    case Memory::IO::NR10:
      channel_data[0].sweep_time      = (value >> 4) & 0x7; // Bits 6-4
      channel_data[0].sweep_direction = (value >> 3) & 0x1; // Bit 3
      channel_data[0].sweep_count     = (value >> 0) & 0x7; // Bits 2-0
      update_channel1();
      break;
    case Memory::IO::NR11:
      channel_data[0].wave_duty = (value >> 6) & 0x3;  // Bits 7-6
      channel_data[0].snd_len   = (value >> 0) & 0x3f; // Bits 5-0
      update_channel1();
      break;
    case Memory::IO::NR12:
      channel_data[0].envelope_volume    = (value >> 4) & 0xf; // Bits 7-4
      channel_data[0].envelope_direction = (value >> 3) & 0x1; // Bit 3
      channel_data[0].envelope_steps     = (value >> 0) & 0x7; // Bits 2-0
      restart_channel_envelope(0);
      update_channel1();
      break;
    case Memory::IO::NR13:
      {
        int freq_high = channel_data[0].freq & 0x700; // Bits 10-8
        channel_data[0].freq = freq_high | value;
        update_channel1();
        break;
      }
    case Memory::IO::NR14:
      {
        int freq_low = channel_data[0].freq & 0xff;             // Bits 7-0
        channel_data[0].freq = ((value & 0x7) << 8) | freq_low; // Bits 2-0

        channel_data[0].counter_enabled = (value >> 6) & 0x1;   // Bit 6

        if ((value >> 7) & 0x1) // Bit 7
        {
          restart_channel(0);
        }

        update_channel1();
        break;
      }

    case Memory::IO::NR21:
      channel_data[1].wave_duty = (value >> 6) & 0x3;  // Bits 7-6
      channel_data[1].snd_len   = (value >> 0) & 0x3f; // Bits 5-0
      update_channel2();
      break;
    case Memory::IO::NR22:
      channel_data[1].envelope_volume    = (value >> 4) & 0xf; // Bits 7-4
      channel_data[1].envelope_direction = (value >> 3) & 0x1; // Bit 3
      channel_data[1].envelope_steps     = (value >> 0) & 0x7; // Bits 2-0
      restart_channel_envelope(1);
      update_channel2();
      break;
    case Memory::IO::NR23:
      {
        int freq_high = channel_data[1].freq & 0x700; // Bits 10-8
        channel_data[1].freq = freq_high | value;
        update_channel2();
        break;
      }
    case Memory::IO::NR24:
      {
        int freq_low = channel_data[1].freq & 0xff;             // Bits 7-0
        channel_data[1].freq = ((value & 0x7) << 8) | freq_low; // Bits 2-0

        channel_data[1].counter_enabled = (value >> 6) & 0x1;   // Bit 6

        if ((value >> 7) & 0x1) // Bit 7
        {
          restart_channel(1);
        }

        update_channel2();
        break;
      }

    case Memory::IO::NR30:
      wave_enabled = value & 0x80; // Bit 7
      update_channel3();
      break;
    case Memory::IO::NR31:
      channel_data[2].snd_len = value; // Bits 7-0
      update_channel3();
      break;
    case Memory::IO::NR32:
      channel_data[2].envelope_volume = (value >> 5) & 0x3; // Bits 6-5
      restart_channel_envelope(2);
      update_channel3();
      break;
    case Memory::IO::NR33:
      {
        int freq_high = channel_data[2].freq & 0x700; // Bits 10-8
        channel_data[2].freq = freq_high | value;
        update_channel3();
        break;
      }
    case Memory::IO::NR34:
      {
        int freq_low = channel_data[2].freq & 0xff;             // Bits 7-0
        channel_data[2].freq = ((value & 0x7) << 8) | freq_low; // Bits 2-0

        channel_data[2].counter_enabled = (value >> 6) & 0x1;   // Bit 6

        if ((value >> 7) & 0x1) // Bit 7
        {
          restart_channel(2);
        }

        update_channel3();
        break;
      }

    case Memory::IO::NR41:
      channel_data[3].snd_len = value & 0x3f; // Bits 5-0
      update_channel4();
      break;
    case Memory::IO::NR42:
      channel_data[3].envelope_volume    = (value >> 4) & 0xf; // Bits 7-4
      channel_data[3].envelope_direction = (value >> 3) & 0x1; // Bit 3
      channel_data[3].envelope_steps     = (value >> 0) & 0x7; // Bits 2-0
      restart_channel_envelope(3);
      update_channel4();
      break;
    case Memory::IO::NR43:
      // TODO store to correctly named variables?
      channel_data[3].freq            = (value >> 4) & 0xf; // Bits 7-4
      channel_data[3].sweep_direction = (value >> 3) & 0x1; // Bit 3
      channel_data[3].sweep_count     = (value >> 0) & 0x7; // Bits 2-0
      update_channel4();
      break;
    case Memory::IO::NR44:
      channel_data[3].counter_enabled = (value >> 6) & 0x1; // Bit 6

      if ((value >> 7) & 0x1) // Bit 7
      {
        restart_channel(3);
      }

      update_channel4();
      break;

    case Memory::IO::NR50:
      volume[1] = (value >> 4) & 0x7; // Bits 6-4
      volume[0] = (value >> 0) & 0x7; // Bits 2-0
      update_channel1();
      update_channel2();
      update_channel3();
      update_channel4();
      break;
    case Memory::IO::NR51:
      terminal_selection = value;
      update_channel1();
      update_channel2();
      update_channel3();
      update_channel4();
      break;
    case Memory::IO::NR52:
      sound_enabled = (value >> 7) & 0x1; // Bit 7
      if (!sound_enabled)
      {
        reset();
      }
      update_channel1();
      update_channel2();
      update_channel3();
      update_channel4();
      break;

    default:
      if (address >= Memory::IO::WAVE && address <= Memory::IO::WAVE + 0xf)
      {
        wave_data[address - Memory::IO::WAVE] = value;
        update_channel3();
      }
      else if (address == 0xff15 || address == 0xff1f)
      {
        // Ignore - unused registers, likely originally intended as
        // sweep registers for channels 2 and 4
      }
      else
      {
        abort();
      }
  }
}

void Audio::set_muted(bool muted)
{
  aout.muted = muted;
}

void Audio::set_debug(bool debug_)
{
  debug = debug_;
  aout.debug = debug_;
}
