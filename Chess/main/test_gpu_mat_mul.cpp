#include "GPU.h"
#include <stdio.h>


GPU gpu;


//This test was completed, but the function was ultimately changed!

int main()
{
    float vector[100];
    float matrix[100 * 100];
    for (int i = 0; i < 100; i++)
    {
        vector[i] = i;
    }
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 100; j++)
            matrix[i * 100 + j] = i;
    }

    float* output_gpu = gpu.vector_matrix_multiplication(vector, matrix, 100 ,100);

    float out[100];
    for (int  i = 0; i < 100; i++ )
    {
        out[i] = 0;
        for (int j = 0; j < 100; j++ )
            out[i] += matrix[(i * 100) + j] * vector[j];
    }

    for(int i = 0; i < 100; i++)
    {
        if(output_gpu[i] != out[i])
        {
            printf("Test failed, found mismatch!\n");
            goto SHUT;
        }

    }
    printf("Test was completed successfully!!!\n");

    SHUT:

    return 0;
}