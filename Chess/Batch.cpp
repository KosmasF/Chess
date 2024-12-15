#include "Batch.h"
#include "Vowels.h"

float Batch::calcVowelBranch(NeuralNetwork *nn, float mutationRate, float **batchGenerationGradientDescent, int batch, GPU *gpu)
{
    float** vowelVectors = new float*[NUM_VOWELS];
    for(int vowel = 0; vowel < NUM_VOWELS; vowel++)
    {
        float** vectors = (float**)malloc(sizeof(float*) * (vowels[vowel].sound.header->DataSize / EVAL_DURATION) * 8);
         vowels[vowel].sound.header->DataSize = 1 * EVAL_DURATION;//REMOVE THIS LINE, ONLY FOR TESTING
        for(int i = 0; i < vowels[vowel].sound.header->DataSize; i+=EVAL_DURATION)
        {
            int idx = i / EVAL_DURATION;
            float* status = (float*)malloc(EVAL_DURATION * sizeof(float));//It is ffeed in get allctivatiopns
            for(int w = 0; w < EVAL_DURATION; w++)
                status[w] = vowels[vowel].sound.buffer[w + 2000];
            

            //float* out = nn->Generate(status, false);
            float expected[4] = {
                vowels[vowel].backness,
                vowels[vowel].height,
                vowels[vowel].roundness,
                1
            };

            float* StepGradient;
            StepGradient = gpu->BackPropagate(nn->GetAllActivations(status), expected, nn->LayerSize, nn->LayerNum, mutationRate, nn->GetNumberOfWeights(), nn->weights, nn->weights_buffer_lookup_table);
            //StepGradient = gpu->BackPropagate(nn->GetAllActivations(status), expected, nn->LayerSize, nn->LayerNum, mutationRate, nn->GetNumberOfWeights(), nn->weights, nn->weights_buffer_lookup_table);
            vectors[idx] = StepGradient;//generationStepVector
            //free(parallelVector);
        }
        float* averageVector = gpu->AvgVector(vectors, vowels[vowel].sound.header->DataSize / EVAL_DURATION, nn->GetNumberOfWeights());
        for (int i = 0; i < vowels[vowel].sound.header->DataSize / EVAL_DURATION; i++)
            free(vectors[i]);

        vowelVectors[vowel] = averageVector;
        free(vectors);
        vectors = nullptr;
        //printf("Ended calculating vowel: %d\n", vowel);
    }

    float* average = gpu->AvgVector(vowelVectors, NUM_VOWELS, nn->GetNumberOfWeights());
    for (int i = 0; i < NUM_VOWELS; i++)
        free(vowelVectors[i]);
    delete[] vowelVectors;

    batchGenerationGradientDescent[batch] = average;

    float* status = (float*)malloc(EVAL_DURATION * sizeof(float));
    for(int w = 0; w < EVAL_DURATION; w++)
    {
        status[w] = vowels[30].sound.buffer[w + 2000];
       // printf("%f\n", status[w]);
    }

    float* NNoutput = nn->Generate(status);
    float expected[4] = {
        vowels[30].backness,
        vowels[30].height,
        vowels[30].roundness,
        1
    };
    float loss = nn->GetLoss(NNoutput, expected);

    free(NNoutput);

    return loss;
}

float Batch::calcVowelBranchTransformed(NeuralNetwork *nn, float mutationRate, float **batchGenerationGradientDescent, int batch, GPU *gpu)
{
    float** vowelVectors = new float*[NUM_VOWELS];
    for(int vowel = 0; vowel < NUM_VOWELS; vowel++)
    {
        float** vectors = (float**)malloc(sizeof(float*) * (vowels[vowel].sound.header->DataSize / EVAL_DURATION) * 8);
        vowels[vowel].sound.header->DataSize = 1 * EVAL_DURATION;//REMOVE THIS LINE, ONLY FOR TESTING
        for(int i = 0; i < vowels[vowel].sound.header->DataSize; i+=EVAL_DURATION)
        {
            int idx = i / EVAL_DURATION;
            double* status = (double*)malloc(EVAL_DURATION * sizeof(double));//It is ffeed in get allctivatiopns
            for(int w = 0; w < EVAL_DURATION; w++)
                status[w] = vowels[vowel].sound.buffer[w + i];

            DFT::Wave wave = {status, vowels[vowel].sound.header->Frequence, (float)EVAL_DURATION / vowels[vowel].sound.header->Frequence};
            DFT::DFT_data data = DFT::DiscreteFourierTranform(wave, 0, 10 * KHZ, 1, false);

            free(status);
            float* dft_status = (float*)malloc(10 * KHZ * sizeof(float));
            for(int w = 0; w < 10 * KHZ; w++)
            {
                dft_status[w] = data.data[w].ampitude;
            }

            free(data.data);

            //float* out = nn->Generate(status, false);
            float expected[4] = {
                vowels[vowel].backness,
                vowels[vowel].height,
                vowels[vowel].roundness,
                1
            };

            float* StepGradient;
            StepGradient = gpu->BackPropagate(nn->GetAllActivations(dft_status), expected, nn->LayerSize, nn->LayerNum, mutationRate, nn->GetNumberOfWeights(), nn->weights, nn->weights_buffer_lookup_table);
            //StepGradient = gpu->BackPropagate(nn->GetAllActivations(status), expected, nn->LayerSize, nn->LayerNum, mutationRate, nn->GetNumberOfWeights(), nn->weights, nn->weights_buffer_lookup_table);
            vectors[idx] = StepGradient;//generationStepVector
            //free(parallelVector);
        }
        float* averageVector = gpu->AvgVector(vectors, vowels[vowel].sound.header->DataSize / EVAL_DURATION, nn->GetNumberOfWeights());
        for (int i = 0; i < vowels[vowel].sound.header->DataSize / EVAL_DURATION; i++)
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

    double* status = (double*)malloc(EVAL_DURATION * sizeof(double));
    for(int w = 0; w < EVAL_DURATION; w++)
    {
        status[w] = vowels[2].sound.buffer[w + 2000];
       // printf("%f\n", status[w]);
    }

    DFT::Wave wave = {status, vowels[2].sound.header->Frequence, (float)EVAL_DURATION / vowels[2].sound.header->Frequence};
    DFT::DFT_data data = DFT::DiscreteFourierTranform(wave, 0, 10 * KHZ, 1, false);

    free(status);
    float* dft_status = (float*)malloc(10 * KHZ * sizeof(float));
    for(int w = 0; w < 10 * KHZ; w++)
    {
        dft_status[w] = data.data[w].ampitude;
    }

    free(data.data);

    float* NNoutput = nn->Generate(dft_status);
    float expected[4] = {
        vowels[2].backness,
        vowels[2].height,
        vowels[2].roundness,
        1
    };
    float loss = nn->GetLoss(NNoutput, expected);

    free(NNoutput);

    return loss;
}
