#include "GPU.h"
#include "NeuralNetwork.h"
#include "Graph.h"
#include "NonGraphicalBoard.h"
#include <thread>
#include "Batch.h"

GPU gpu;

int main()
{
    Graph graph = Graph(1000);

    time_t startTime = time(NULL);
    srand(startTime);

    printf("Setup\n");

    const int batchSize = 1;//Max is 26 rn
    const int batches = 10000;

    int networkSizes[] = { 64, 64 * 64 * 64, 64 * 64 ,64 * 64};
    float (*activationMethods[])(float) = {None,None,None};

    //NeuralNetwork nn = NeuralNetwork("networks/testedNonRandom3LayersBIG.nn");
    NeuralNetwork nn = NeuralNetwork(networkSizes, sizeof(networkSizes) / sizeof(int) , activationMethods, true, &gpu);
    NonGraphicalBoard board;


    //const char* path = "networks/testEvaluatorNonRandomWeights.nn";
    //nn.LoadFromDisk(path);

    long int sum = 1;
    for(int i = 1; i < sizeof(networkSizes) / sizeof(int); i++)
        sum *= networkSizes[i];
    //sum *= sizeof(networkSizes) / sizeof(int);
    const float mutationRate = 0.01f * (1.f / sum) * ( 1.f / pow(10, sizeof(networkSizes) / sizeof(int)));
    printf("Dynamic mutation Rate: %.20f %ld\n", mutationRate, sum);

    std::thread batchThreads[batchSize];

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

            loss = Batch::calcBatch( &board, &nn, mutationRate, batchGenerationGradientDescent, batch, &gpu);
        }
        for (int batch = 0; batch < batchSize; batch++)
        {
            //batchThreads[batch].join();
            //Server can't handle instant attempts
        }
        //output = nn.Generate(board.Status(board.whitePlays));
        //output = nn.Generate(input);

        //double st = GetTime();
        //float* batchGradientDescent = nn.AverageWeightVector(batchGenerationGradientDescent,batchSize);
        
        float* batchGradientDescent = gpu.AvgVector(batchGenerationGradientDescent, batchSize, nn.GetNumberOfWeights());
        //double ed = GetTime();
        //printf("%f\n", ed-st);


        nn.AddToWeights(batchGradientDescent);
        for (int i = 0; i < batchSize; i++)
            free(batchGenerationGradientDescent[i]);
        //delete[] batchGradientDescent;
        free(batchGradientDescent);
        delete[] batchGenerationGradientDescent;

        //float loss = nn.GetLoss(output, &eval);

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
    nn.Save("../networks/testn1.nn2");
    printf("Training started in %i and ended, duration: %f\n",(int)startTime, (float)(time(NULL) - startTime));
        
    return 0;
}


//1972 moves in chess !!!!!! :) :) :) :)
//I'll use 4096
