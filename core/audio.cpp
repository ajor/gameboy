#include "audio.h"
#include "memory.h"

#include <stdlib.h>

void Audio::update_channel1()
{
  for (int i=0; i<8; i++)
  {
    channels[0][i] = square_wave[channel_data[0].wave_duty][i];
  }

  int real_freq = 131072 / (2048 - channel_data[0].freq);
  aout.update_channel1(real_freq);
}

void Audio::update_channel2()
{
  for (int i=0; i<8; i++)
  {
    channels[1][i] = square_wave[channel_data[1].wave_duty][i];
  }

  int real_freq = 131072 / (2048 - channel_data[1].freq);
  aout.update_channel2(real_freq);
}

void Audio::update_channel3()
{
  for (int i=0; i<=0xf; i++)
  {
//    channels[3][i] = memory.get8(Memory::IO::WAVE + i);
  }

  aout.update_channel3();
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
      return 0;
    case Memory::IO::NR14:
      // TODO counter/consecutive selection
      return 0;

    case Memory::IO::NR21:
      return channel_data[1].wave_duty << 6; // Low bits are write-only
    case Memory::IO::NR22:
      return (channel_data[1].envelope_volume << 4) |
             (channel_data[1].envelope_direction << 3) |
             (channel_data[1].envelope_count);
    case Memory::IO::NR23:
      return 0;
    case Memory::IO::NR24:
      // TODO counter/consecutive selection
      return 0;
    default:
      break;
//      abort();
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
        update_channel1();

        // TODO Initial / restart sound
        //      counter selection
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
        update_channel2();

        // TODO Initial / restart sound
        //      counter selection
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
        update_channel3();

        // TODO Initial / restart sound
        //      counter selection
        break;
      }

    default:
      break;
//      abort();
  }
}
