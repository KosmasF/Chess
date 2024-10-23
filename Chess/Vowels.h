#pragma once
#include "Audio.h"
#include <stdlib.h>
#include <string.h>

#define NUM_VOWELS 70
#define BACKNESS_ACCURANCY 10
#define HEIGHT_ACCURANCY 7
#define ROUNDNESS_ACCURANCY 2

static const char* folder = "../Vowels/";
static const char* extention = ".wav";
//Exemple: ../Vowels/019.wav

struct VowelProperties
{
    WAVE_DATA sound;
    float height;
    float backness;
    float roundness;
};

inline VowelProperties vowels[NUM_VOWELS];

void LoadVowels();
void FreeVowels();