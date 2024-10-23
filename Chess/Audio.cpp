#include "Audio.h"

void *LoadFile(const char *filename)
{
    FILE* file = fopen(filename, "rb");

    fseek(file, 0, SEEK_END);
    int size = ftell(file);

    fseek(file, 0, SEEK_SET);

    void* data = malloc(size);

    fread(data, size, 1, file);
    fclose(file);

    return data;
}

WAVE_DATA GetAudioDataWav(const char *filename)
{
    void* data = LoadFile(filename);
    WAVE_DATA result = GetAudioDataWav(data);
    free(data);
    return result;
}

WAVE_DATA GetAudioDataWav(void *data)
{
    WAV_HEADER* wav_data = (WAV_HEADER*)data;

    void* buffer_pos = (char*)data + sizeof(WAV_HEADER);
    void* buffer = malloc(wav_data->DataSize * sizeof(int16_t));
    memcpy(buffer, buffer_pos, wav_data->DataSize * sizeof(int16_t));
    void* wave_data = malloc(sizeof(WAV_HEADER));
    memcpy(wave_data, wav_data, sizeof(WAV_HEADER));
    WAVE_DATA wave = {(int16_t*)buffer, (WAV_HEADER*)wave_data};

    return wave;
}

void PlayWave(WAVE_DATA wave)
{
    int err;
    snd_pcm_t *handle;
    snd_pcm_sframes_t frames;
    err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);

    if(err < 0)
    {
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }


    err = snd_pcm_set_params(handle,
                      SND_PCM_FORMAT_S16_LE,
                      SND_PCM_ACCESS_RW_INTERLEAVED,
                      1,
                      wave.header->Frequence,
                      0,
                      500000);
                      

                      
    if(err < 0) {   /* 0.5sec */
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }




    err = snd_pcm_writei(handle, wave.buffer, wave.header->DataSize);


    /* pass the remaining samples, otherwise they're dropped in close */
    err = snd_pcm_drain(handle);
    if (err < 0)
        printf("snd_pcm_drain failed: %s\n", snd_strerror(err));
    snd_pcm_close(handle);
}
