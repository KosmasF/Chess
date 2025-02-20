#pragma once

#include "GpuNeuralNetwork.h"

#define MAX_MUTATION_RATE 2
#define MAX_LAYER_NUM 4
#define MAX_LAYER_SIZE 128

#define NUM_INPUT_LAYERS 5

#define INPUT_WIDTH 50
#define INPUT_HEIGHT 50

#define NUM_SIMULATIONS_PER_AGENT 3

class Q_Learning
{
public:
    Q_Learning();

    void Generation();


private:
    void* starting_state;

    void** states;
    int num_actions;

    int num_agents;    
    GpuNeuralNetwork* agents;

    float*(*Observe)(void* state);
    float(*Act)(float* action, void* state);
    void(**Actions)(float strength, void* state);

    int BATCH_SIZE = 15;
    int SURVIVOR_NUM = 5;

    GPU* gpu;
};