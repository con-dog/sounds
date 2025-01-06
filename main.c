#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

// WAV header structure
typedef struct
{
  // RIFF chunk
  char riff_id[4]; // "RIFF"
  uint32_t size;   // Size of remaining file
  char wave_id[4]; // "WAVE"

  // Format chunk
  char fmt_id[4];           // "fmt "
  uint32_t fmt_size;        // Size of format chunk
  uint16_t format;          // Format type (1 = PCM)
  uint16_t channels;        // Number of channels
  uint32_t sample_rate;     // Sample rate
  uint32_t byte_rate;       // Bytes per second
  uint16_t block_align;     // Bytes per sample * channels
  uint16_t bits_per_sample; // Bits per sample

  // Data chunk
  char data_id[4];    // "data"
  uint32_t data_size; // Size of data chunk
} WAVHeader;

int main()
{
  // Audio parameters
  const int SAMPLE_RATE = 44100;
  const int DURATION = 2;         // seconds
  const float FREQUENCY = 440.0f; // Hz (A4 note)
  const int NUM_SAMPLES = SAMPLE_RATE * DURATION;

  // Prepare audio data (16-bit samples)
  int16_t *audio_data = (int16_t *)malloc(NUM_SAMPLES * sizeof(int16_t));
  if (!audio_data)
  {
    printf("Memory allocation failed\n");
    return 1;
  }

  // Generate square wave
  for (int i = 0; i < NUM_SAMPLES; i++)
  {
    float t = (float)i / SAMPLE_RATE;
    // Simple square wave: positive for first half of period, negative for second half
    audio_data[i] = (sin(2.0f * M_PI * FREQUENCY * t) > 0.0f) ? 32767 : -32767;
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
  FILE *file = fopen("square_wave.wav", "wb");
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

  printf("Successfully generated square_wave.wav\n");
  return 0;
}