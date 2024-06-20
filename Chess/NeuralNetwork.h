#pragma once
#pragma warning (disable : 4996)
#include "Neuron.h"
#include "ActivationMethods.h"


class NeuralNetwork
{
private:
	Neuron** neurons;
	int* LayerSize;
	int LayerNum;
	int NeuronNum;

	int GetNeuronDataSize(void* data);
	int GetNumberOfWeights();
	float GetWeightBetweenNeurons(int From, int To);
	int LayerOfNeuron(int neuron);
	int LayerRelativeOfNeuron(int neuron);
	int LayerRelativeOfNeuronIncludingInputLayer(int neuron);
	int LayerOfNeuronIncludingInputLayer(int neuron);
	float* GetAllActivations(float* input);
	float PartialDerivativeOfErrorFunction(int neuron, float* activations, float* predictedOutput);// Recursive
	float PartialDerivativeOfErrorFunction(int neuron, float* activations, float* predictedOutput, float* forwardNeuronsDerivatives);// Non-recursive
	float* PreCalcNeuronDerivatives();
	int StartingIndexOfLayer(int layer);
	int StartingIndexOfLayerIncludingInputLayer(int layer);
	int GetIndexOfWeight(int source, int destination);

	int RetNeuronNum();

public:
	NeuralNetwork(int* layerSize, int layerNum, float (*activationMethods[])(float), bool normalizeOutput);

	NeuralNetwork(const char* path);

	~NeuralNetwork();

	float* Generate(float* input);

	void* Data();

	void Load(void* data);

	void Save(const char* path);

	void LoadFromDisk(const char* path);

	void* FromDiskData(const char* path);

	void SetNeuronNum();

	void Mutate(float mutationRate);

	float* BackPropagate(float* expectedOutput, float* input, float mutationRate);

	void AddToWeights(float* data);

	float GetLoss(float* output, float* predictedOutput);

	float* AverageWeightVector(float** vectors, int num);
};

