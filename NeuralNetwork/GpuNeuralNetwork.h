#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <CL/cl.h>
#include "GPU.h"
#include "ActivationMethods.h"

class GpuNeuralNetwork{
public:
    friend void generation();
    /// @brief Create a new Neural Network which is run and trained on the GPU.
    /// @param layerSize The number of neurons in each layer
    /// @param layerNum  The number of layers
    /// @param activationMethods The activation functions for each layer
    /// @param gpu The GPU to run the network on
    GpuNeuralNetwork(int* layerSize, int layerNum, ActivationMethodsEnum* activationMethods, GPU* gpu);
    /// @brief Load a Neural Network from a file, supported extension is .nn3
    /// @param path The path relative to the executable
    /// @param gpu The GPU to run the network on
    GpuNeuralNetwork(const char* path, GPU* gpu);
    GpuNeuralNetwork(GpuNeuralNetwork& other);
    ~GpuNeuralNetwork();

    cl_mem Generate(float* input, bool freeInput);
    float* GenerateAndCopyToMem(float* input, bool freeInput);
    void BackPropagate(float* input, float* output, float learningRate);
    void Save(const char* path);

    float* Data();
    void Load(char* data);

    void AddToWeights(float* data);
private:
    GPU* gpu;

    /// @brief The number of layers in the network, input layer excluded
    int LayerNum;
    /// @brief The number of neurons in each layer, input layer included
    int* LayerSize;

    cl_mem weights;
    cl_mem biases;

    cl_mem* weightSubbuffers;
    cl_mem* biasSubbuffers;

    ActivationMethodsEnum* ActivationMethods;

    int numberOfWeights;
    int numberOfNeurons;
};