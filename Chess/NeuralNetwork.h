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
	float* GetAllActivations(float* input);
	float PartialDerivativeOfErrorFunction(int neuron, float* activations, float* predictedOutput);
	int StartingIndexOfLayer(int layer);

public:
	NeuralNetwork(int* layerSize , int layerNum);

	~NeuralNetwork();

	float* Generate(float* input);

	void* Data();

	void Load(void* data);

	void Save(const char* path);

	void LoadFromDisk(const char* path);

	void SetNeuronNum();

	void Mutate(float mutationRate);

	float* BackPropagate(float* expectedOutput, float* input, float mutationRate);

	void AddToWeights(float* data);

	float GetLoss(float* output, float* predictedOutput);

	float* AverageWeightVector(float** vectors, int num);
};

