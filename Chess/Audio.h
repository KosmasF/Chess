#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>


static const char *device = "default";   


struct WAV_HEADER
{
    int8_t  FileTypeBlocID[4]    ; //(4 bytes) : Identifier « RIFF »  (0x52, 0x49, 0x46, 0x46)
    int32_t FileSize             ; //(4 bytes) : Overall file size minus 8 bytes
    int8_t  FileFormatID[4]      ; //(4 bytes) : Format = « WAVE »  (0x57, 0x41, 0x56, 0x45)
     
    int8_t  FormatBlocID[4]      ; //(4 bytes) : Identifier « fmt␣ »  (0x66, 0x6D, 0x74, 0x20)
    int32_t BlocSize             ; //(4 bytes) : Chunk size minus 8 bytes, which is 16 bytes here  (0x10)
    int16_t AudioFormat          ; //(2 bytes) : Audio format (1: PCM integer, 3: IEEE 754 float)
    int16_t NbrChannels          ; //(2 bytes) : Number of channels
    int32_t Frequence            ; //(4 bytes) : Sample rate (in hertz)
    int32_t BytePerSec           ; //(4 bytes) : Number of bytes to read per second (Frequence * BytePerBloc).
    int16_t BytePerBloc          ; //(2 bytes) : Number of bytes per block (NbrChannels * BitsPerSample / 8).
    int16_t BitsPerSample        ; //(2 bytes) : Number of bits per sample       
     
    int8_t  DataBlocID[4]        ; //(4 bytes) : Identifier « data »  (0x64, 0x61, 0x74, 0x61)
    int32_t DataSize             ; //(4 bytes) : SampledData size
};

struct WAVE_DATA
{
    int16_t* buffer;
    WAV_HEADER* header;
};

void* LoadFile(const char* filename);

WAVE_DATA GetAudioDataWav(const char* filename);
WAVE_DATA GetAudioDataWav(void* data);

void PlayWave(WAVE_DATA wave);