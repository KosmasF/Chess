#include "Batch.h"
#include "Vowels.h"

float Batch::calcVowelBranch(NeuralNetwork *nn, float mutationRate, float **batchGenerationGradientDescent, int batch, GPU *gpu)
{
    float** vowelVectors = new float*[NUM_VOWELS];
    for(int vowel = 0; vowel < NUM_VOWELS; vowel++)
    {
        float** vectors = (float**)malloc(sizeof(float*) * (vowels[vowel].sound.header->DataSize / 4410));
        for(int i = 0; i < vowels[vowel].sound.header->DataSize; i+=4410)
        {
            int idx = i / 4410;
            float* status = (float*)malloc(4410 * sizeof(float));//It is ffeed in get allctivatiopns
            for(int w = 0; w < 4410; w++)
                status[w] = vowels[vowel].sound.buffer[w + i];
            

            //float* out = nn->Generate(status, false);
            float expected[4] = {
                vowels[vowel].backness,
                vowels[vowel].height,
                vowels[vowel].roundness,
                1
            };

            float* StepGradient = gpu->BackPropagate(nn->GetAllActivations(status), expected, nn->LayerSize, nn->LayerNum, mutationRate, nn->GetNumberOfWeights(), nn->weights, nn->weights_buffer_lookup_table);

            vectors[idx] = StepGradient;//generationStepVector
            //free(parallelVector);
        }
        float* averageVector = gpu->AvgVector(vectors, vowels[vowel].sound.header->DataSize / 4410, nn->GetNumberOfWeights());
        for (int i = 0; i < vowels[vowel].sound.header->DataSize / 4410; i++)
            free(vectors[i]);

        vowelVectors[vowel] = averageVector;
        free(vectors);
        vectors = nullptr;
        printf("Ended calculating vowel: %d\n", vowel);
    }

    float* average = gpu->AvgVector(vowelVectors, NUM_VOWELS, nn->GetNumberOfWeights());
    for (int i = 0; i < NUM_VOWELS; i++)
        free(vowelVectors[i]);
    delete[] vowelVectors;

    batchGenerationGradientDescent[batch] = average;

    float* status = (float*)malloc(4410 * sizeof(float));
    for(int w = 0; w < 4410; w++)
        status[w] = vowels[0].sound.buffer[w + 0];

    float* NNoutput = nn->Generate(status);
    float expected[4] = {
        vowels[0].backness,
        vowels[0].height,
        vowels[0].roundness,
        1
    };
    float loss = nn->GetLoss(NNoutput, expected);

    free(NNoutput);

    return loss;
}
