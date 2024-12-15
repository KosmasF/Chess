#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#ifndef DFT_H
#define DFT_H

#define NULL_WAVE ((struct Wave){NULL , 0})
typedef unsigned int uint;
#define PI 3.14159
#define TAU (PI * 2)

#define KHZ 1000
#define MHZ (KHZ * 1000)
#define GHZ (MHZ * 1000)

#define MS (1.f / 1000)
#define US (MS / 1000)


struct Wave
{
    double* data;
    uint samplesPerSecond;
    float duration;//In seconds
};

struct FreqData
{
    uint freq;
    float phase;
    float ampitude;
};

struct DFT_data
{
    uint minFreq;
    uint maxFreq;
    struct FreqData* data;
    uint increment;
};

struct FreqData MultiplyByFreq(struct Wave wave, uint freq);

struct Wave SinWaveByFreq(uint freq, uint samplesPerSecond, float duration, float phase);

struct Wave CosWaveByFreq(uint freq, uint samplesPerSecond, float duration, float phase);

void PrintFreqData(struct FreqData freqData);

struct DFT_data DiscreteFourierTranform(struct Wave wave, uint minFreq, uint maxFreq, int increment, bool logProgress);

#endif
