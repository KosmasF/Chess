#pragma once
#include "GPU.h"

#define _UNIFIED_WEIGTS_ARRAY
#ifdef _MSVC_LANG //Check if we are using c++ with visual studio
#pragma warning (disable : 4996)
#endif //_MSVC_LANG

#include "Layer.h"
#include "ActivationMethods.h"
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef float (*ActFuntionsType)(float);

#include <errno.h>

//extern int errno;

class NeuralNetwork
{
public://PRIVATE
	Layer** layers = nullptr;// IMPORTANT!!! : Input layer is ignored, BUT in layer counter it is!!!
	int* LayerSize = nullptr;
	int LayerNum = 0;
	int NeuronNum = 0;
	GPU* gpu = nullptr;
	float(**ActivationMethods)(float) = nullptr;

#ifdef _UNIFIED_WEIGTS_ARRAY
	float* weights = nullptr;
	int* weights_buffer_lookup_table = nullptr;

#endif //_UNIFIED_WEIGTS_ARRAY



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
	int GetIndexOfNeuronByIndex(int idx);

	int RetNeuronNum();


public:
	float* GetAllActivations(float* input);
	int GetNumberOfWeights() const;
	NeuralNetwork(int* layerSize, int layerNum, float (*activationMethods[])(float), bool normalizeOutput, GPU* _gpu);

	NeuralNetwork(const char* path, GPU* _gpu);
	NeuralNetwork(const NeuralNetwork& other);

	~NeuralNetwork();

	float* Generate(float* input, bool freeInput = true);

	void* Data() const;

	void Load(void* data);

	void Save(const char* path);

	void LoadFromDisk(const char* path);

	void* FromDiskData(const char* path);

	void SetNeuronNum();

	//void Mutate(float mutationRate); *DEPRECATED*

	float* BackPropagate(float* expectedOutput, float* input, float mutationRate);

	float* EmptyGradient();

	void AddToWeights(float* data);

	float GetLoss(float* output, float* predictedOutput);

	float* AverageWeightVector(float** vectors, int num);

	void* covnertFormat(void* old_data);

	const char* CheckFileType(const char* path);
};

