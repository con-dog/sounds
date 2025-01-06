#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

// WAV header structure (same as before)
typedef struct
{
  char riff_id[4];
  uint32_t size;
  char wave_id[4];
  char fmt_id[4];
  uint32_t fmt_size;
  uint16_t format;
  uint16_t channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
  char data_id[4];
  uint32_t data_size;
} WAVHeader;

// Note definition structure
typedef struct
{
  float frequency; // in Hz
  float duration;  // in seconds
} Note;

// Common note frequencies (A4 = 440Hz)
#define NOTE_C4 261.63f
#define NOTE_D4 293.66f
#define NOTE_E4 329.63f
#define NOTE_F4 349.23f
#define NOTE_G4 392.00f
#define NOTE_A4 440.00f
#define NOTE_B4 493.88f
#define NOTE_C5 523.25f

int main()
{
  const int SAMPLE_RATE = 44100;

  // Define a simple melody (frequencies and durations)
  Note melody[] = {
      {NOTE_C4, 0.7f}, // C4 for 0.5 seconds
      {NOTE_E4, 0.1f}, // E4 for 0.5 seconds
      {NOTE_G4, 0.3f}, // G4 for 0.5 seconds
      {NOTE_C5, 1.0f}, // C5 for 1.0 seconds
  };
  const int NUM_NOTES = sizeof(melody) / sizeof(melody[0]);

  // Calculate total duration and number of samples
  float total_duration = 0;
  for (int i = 0; i < NUM_NOTES; i++)
  {
    total_duration += melody[i].duration;
  }
  const int NUM_SAMPLES = SAMPLE_RATE * total_duration;

  // Allocate audio buffer
  int16_t *audio_data = (int16_t *)malloc(NUM_SAMPLES * sizeof(int16_t));
  if (!audio_data)
  {
    printf("Memory allocation failed\n");
    return 1;
  }

  // Generate audio data
  int current_sample = 0;
  for (int i = 0; i < NUM_NOTES; i++)
  {
    int note_samples = SAMPLE_RATE * melody[i].duration;

    for (int j = 0; j < note_samples; j++)
    {
      float t = (float)j / SAMPLE_RATE;
      // Square wave generation with simple envelope
      float envelope = 1.0f - (float)j / note_samples; // Simple linear fade-out
      audio_data[current_sample + j] =
          (sin(2.0f * M_PI * melody[i].frequency * t) > 0.0f ? 32767 : -32767) * envelope * 0.8f; // 0.8f to prevent clipping when notes overlap
    }
    current_sample += note_samples;
  }

  // Prepare WAV header
  WAVHeader header = {
      .riff_id = {'R', 'I', 'F', 'F'},
      .wave_id = {'W', 'A', 'V', 'E'},
      .fmt_id = {'f', 'm', 't', ' '},
      .data_id = {'d', 'a', 't', 'a'},
      .fmt_size = 16,
      .format = 1,
      .channels = 1,
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = 16,
      .block_align = (16 * 1) / 8,
      .byte_rate = SAMPLE_RATE * ((16 * 1) / 8),
      .data_size = NUM_SAMPLES * sizeof(int16_t),
  };
  header.size = header.data_size + sizeof(WAVHeader) - 8;

  // Write to file
  FILE *file = fopen("melody.wav", "wb");
  if (!file)
  {
    printf("Failed to open file\n");
    free(audio_data);
    return 1;
  }

  fwrite(&header, sizeof(WAVHeader), 1, file);
  fwrite(audio_data, sizeof(int16_t), NUM_SAMPLES, file);

  fclose(file);
  free(audio_data);

  printf("Successfully generated melody.wav\n");
  return 0;
}