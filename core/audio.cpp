#include "audio.h"
#include "memory.h"

#include <stdlib.h>

int Audio::freq_to_hz(int freq)
{
  return 131072 / (2048 - freq);
}

int Audio::length_to_cycles(int cnt)
{
  return (64 - cnt) * (4194304 / 256);
}

void Audio::update(uint cycles)
{
  for (int i=0; i<4; i++)
  {
    if (channel_data[i].counter_enabled)
    {
      channel_state[i].counter += cycles;
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

void Audio::update_channel1()
{
  if (!sound_enabled ||
      (channel_data[0].counter_enabled &&
       length_to_cycles(channel_data[0].snd_len) < channel_state[0].counter))
  {
    // don't play sound
    aout.stop_channel1();
    return;
  }

  for (int i=0; i<8; i++)
  {
    channels[0][i] = square_wave[channel_data[0].wave_duty][i];
  }

  aout.play_channel1(freq_to_hz(channel_data[0].freq));
}

void Audio::update_channel2()
{
  if (!sound_enabled ||
      (channel_data[1].counter_enabled &&
       length_to_cycles(channel_data[1].snd_len) < channel_state[1].counter))
  {
    // don't play sound
    aout.stop_channel2();
    return;
  }

  for (int i=0; i<8; i++)
  {
    channels[1][i] = square_wave[channel_data[1].wave_duty][i];
  }

  aout.play_channel2(freq_to_hz(channel_data[1].freq));
}

void Audio::update_channel3()
{
  for (int i=0; i<=0xf; i++)
  {
//    channels[3][i] = memory.get8(Memory::IO::WAVE + i);
  }

  aout.play_channel3();
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
             (channel_data[0].envelope_count);
    case Memory::IO::NR13:
      return 0; // Write only
    case Memory::IO::NR14:
      return channel_data[0].counter_enabled << 6;

    case Memory::IO::NR21:
      return channel_data[1].wave_duty << 6; // Low bits are write-only
    case Memory::IO::NR22:
      return (channel_data[1].envelope_volume << 4) |
             (channel_data[1].envelope_direction << 3) |
             (channel_data[1].envelope_count);
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
             (channel_data[3].envelope_count);
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
      return (sound_enabled << 7); // TODO sound 1,2,3,4 ON flags

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
      channel_data[0].envelope_count     = (value >> 0) & 0x7; // Bits 2-0
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
        update_channel1();

        // TODO Initial / restart sound
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
      channel_data[1].envelope_count     = (value >> 0) & 0x7; // Bits 2-0
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
        update_channel2();

        // TODO Initial / restart sound
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
        update_channel3();

        // TODO Initial / restart sound
        break;
      }

    case Memory::IO::NR41:
      channel_data[3].snd_len = value & 0x3f; // Bits 5-0
      update_channel4();
      break;
    case Memory::IO::NR42:
      channel_data[3].envelope_volume    = (value >> 4) & 0xf; // Bits 7-4
      channel_data[3].envelope_direction = (value >> 3) & 0x1; // Bit 3
      channel_data[3].envelope_count     = (value >> 0) & 0x7; // Bits 2-0
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
      // TODO initial
      channel_data[3].counter_enabled = (value >> 6) & 0x1; // Bit 6
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
      else
      {
        abort();
      }
  }
}
