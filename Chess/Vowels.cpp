#include "Vowels.h"

void LoadVowels()
{
    for(int i = 0; i < NUM_VOWELS; i++)
    {
        float roundness = !(i % ROUNDNESS_ACCURANCY == 0);
        float backness = i % BACKNESS_ACCURANCY;
        float height = i / HEIGHT_ACCURANCY;

        char* num = (char*)malloc(3 * sizeof(char) + 1);
        snprintf(num, 4, "%d%d%d\0", (i / 100) % 10, (i / 10) % 10, i % 10);

        size_t size = ((strlen(num) + strlen(folder) + strlen(extention) + 1) * sizeof(char));
        char* filename = (char*)malloc(size);
        memcpy(filename, folder, strlen(folder) * sizeof(char));
        memcpy(filename + (strlen(folder) * sizeof(char)), num, strlen(num) * sizeof(char));
        memcpy(filename + ((strlen(folder) + strlen(num)) * sizeof(char)), extention, strlen(extention) * sizeof(char));
        filename[size - 1] = 0;
        free(num);

        WAVE_DATA data = GetAudioDataWav(filename);
        free(filename);

        vowels[i] = {data, height, backness, roundness};        
    }
}

void FreeVowels()
{
    for(int i = 0; i < NUM_VOWELS; i++)
    {
        free(vowels[i].sound.buffer);
        free(vowels[i].sound.header);
    }
}
