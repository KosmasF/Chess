#include "GPU.h"
#include "NeuralNetwork.h"
#include "Graph.h"
#include "Batch.h"
#include "Audio.h"
#include "Vowels.h"

GPU gpu;

int main()
{
    LoadVowels();

    Graph graph = Graph(1000);
    time_t startTime = time(NULL);
    srand(startTime);

    printf("Setup\n");

    const int batchSize = 1;//Max is 99 rn
    const int batches = 10000;

    int networkSizes[] = {EVAL_DURATION , 256 ,4};
    float (*activationMethods[])(float) = {None, None, None, None, None};

    NeuralNetwork nn = NeuralNetwork(networkSizes, sizeof(networkSizes) / sizeof(int) , activationMethods, true, &gpu);

    long int sum = 1;
    for(int i = 1; i < sizeof(networkSizes) / sizeof(int); i++)
        sum *= networkSizes[i];
    float mutationRate = 0.5f * (1.f / sum);// * ( 1.f / pow(10, (sizeof(networkSizes)) / sizeof(int))); //* (1.f / (sizeof(networkSizes)) / sizeof(int));
    //mutationRate= abs(mutationRate);
    printf("Dynamic mutation Rate: %.40f %ld\n", mutationRate, sum);

    printf("Training...\n");

    float loss = 0;
    int iterations = 0;
    while (iterations < batches)
    {
        float* output = nullptr;
        //float eval = 0; I dont want it because of functionalization of calcBatch
        float** batchGenerationGradientDescent = new float*[batchSize];

        for (int batch = 0; batch < batchSize; batch++)
        {

            loss = Batch::calcVowelBranch(&nn, mutationRate, batchGenerationGradientDescent, batch, &gpu);
        }
        
        float* batchGradientDescent = gpu.AvgVector(batchGenerationGradientDescent, batchSize, nn.GetNumberOfWeights());


        nn.AddToWeights(batchGradientDescent);
        for (int i = 0; i < batchSize; i++)
            free(batchGenerationGradientDescent[i]);
        free(batchGradientDescent);
        delete[] batchGenerationGradientDescent;

        if (WindowShouldClose())//(loss == ((float)(1e+300 * 1e+300)))
        {
            goto Shutdown;
        }
 
        graph.Add(loss);
        graph.Draw();
        printf("Iteration %d , loss: %f \n", iterations, loss);

        delete[] output;

        iterations++;
    }
    
    Shutdown:

    printf("Closing...\n");
    nn.Save("../networks/vowe3.nn2");
    printf("Training started in %i and ended, duration: %f\n",(int)startTime, (float)(time(NULL) - startTime));

    FreeVowels();
    return 0;
}


//1972 moves in chess !!!!!! :) :) :) :)
//I'll use 4096
