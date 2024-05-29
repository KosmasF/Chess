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


public:
	NeuralNetwork(int* layerSize , int layerNum);

	~NeuralNetwork();

	float* Generate(float* input);

	void* Data();

	void Load(void* data);

	int GetNeuronDataSize(void* data);

	void Save(const char* path);

	void LoadFromDisk(const char* path);

	void SetNeuronNum();

	void Mutate(float mutationRate);

};

