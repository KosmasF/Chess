#include "GPU.h"
#include "NeuralNetwork.h"
#include <stdio.h>


GPU gpu;

int main()
{
    const char* path = "validation_test.nn2";
    float input[64];
    for (size_t i = 0; i < sizeof(input) / sizeof(float);++i) {
        input[i] = 0.5f;
    }

    float* output_old;
    float* output_new;

    {
        int networkSizes[] = { 64, 256, 128};
        float (*activationMethods[])(float) = {None,None};

        NeuralNetwork nn = NeuralNetwork(networkSizes, sizeof(networkSizes) / sizeof(int) , activationMethods, true, &gpu);
        
        output_old = nn.Generate(input, false);

        nn.Save(path);
    }
    {
        NeuralNetwork nn = NeuralNetwork(path, &gpu);

        output_new = nn.Generate(input, false);
    }

    for(int i = 0; i < 128; i++)
    {
        if(output_new[i] != output_old[i])
        {
            printf("Test failed, found mismatch!\n");
            goto SHUT;
        }

    }
    printf("Test was completed successfully!!!\n");

    SHUT:
    free(output_old);
    free(output_new);
    return 0;


}