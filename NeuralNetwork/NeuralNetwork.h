#pragma once

#define _UNIFIED_WEIGTS_ARRAY
#ifdef _MSVC_LANG //Check if we are using c++ with visual studio
#pragma warning (disable : 4996)
#endif //_MSVC_LANG

#include "Neuron.h"
#include "ActivationMethods.h"
#include <limits>


class NeuralNetwork
{
public://PRIVATE
	Neuron** neurons;
	int* LayerSize;
	int LayerNum;
	int NeuronNum;

#ifdef _UNIFIED_WEIGTS_ARRAY
	float* weights;
	int* weights_buffer_lookup_table;

#endif _UNIFIED_WEIGTS_ARRAY



	int GetNeuronDataSize(void* data);
	float GetWeightBetweenNeurons(int From, int To);
	int LayerOfNeuron(int neuron);
	int LayerRelativeOfNeuron(int neuron);
	int LayerRelativeOfNeuronIncludingInputLayer(int neuron);
	int LayerOfNeuronIncludingInputLayer(int neuron);
	float PartialDerivativeOfErrorFunction(int neuron, float* activations, float* predictedOutput);// Recursive
	float PartialDerivativeOfErrorFunction(int neuron, float* activations, float* predictedOutput, float* forwardNeuronsDerivatives);// Non-recursive
	//float* PreCalcNeuronDerivatives();
	int StartingIndexOfLayer(int layer);
	int StartingIndexOfLayerIncludingInputLayer(int layer);
	int GetIndexOfWeight(int source, int destination);
	int GetIndexOfNeuron(int layer, int idx);

	int RetNeuronNum();


public:
	float* GetAllActivations(float* input);
	int GetNumberOfWeights();
	NeuralNetwork(int* layerSize, int layerNum, float (*activationMethods[])(float), bool normalizeOutput);

	NeuralNetwork(const char* path);

	~NeuralNetwork();

	float* Generate(float* input, bool freeInput = true);

	void* Data();

	void Load(void* data);

	void Save(const char* path);

	void LoadFromDisk(const char* path);

	void* FromDiskData(const char* path);

	void SetNeuronNum();

	void Mutate(float mutationRate);

	float* BackPropagate(float* expectedOutput, float* input, float mutationRate);

	float* EmptyGradient();

	void AddToWeights(float* data);

	float GetLoss(float* output, float* predictedOutput);

	float* AverageWeightVector(float** vectors, int num);
};

