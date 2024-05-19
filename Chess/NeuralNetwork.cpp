#include "NeuralNetwork.h"
#include <iostream>

NeuralNetwork::NeuralNetwork(int* layerSize, int layerNum)
{
	LayerSize = layerSize;
	LayerNum = layerNum;

	for (int layer = 1; layer < layerNum; layer++)
	{
		NeuronNum += layerSize[layer];
	}

	neurons = new Neuron*[NeuronNum];

	for (int layer = 1; layer < layerNum; layer++)
	{
		int buffer = 0;
		int focusLayer = layer;

		while (focusLayer > 1)
		{
			buffer += layerSize[focusLayer - 1];
			focusLayer--;
		}
		for (int i = 0; i < layerSize[layer]; i++)
		{
			neurons[i+buffer] = new Neuron(layerSize[layer - 1], layer == 1 ? Sigmoid : NonNegativeLimitedLinear);
		}
	}

	//Finished
}

NeuralNetwork::~NeuralNetwork()
{
	for (int i = 0; i < NeuronNum; i++)
	{
		delete neurons[i];
	}
	delete[] neurons;
}

float* NeuralNetwork::Generate(float* input)
{
	int inputLength = LayerSize[0];

	for (int layer = 1; layer < LayerNum; layer++)
	{
		float* layerOutput = new float[LayerSize[layer]];


		int buffer = 0;
		int focusLayer = layer;
		while (focusLayer > 1)
		{
			buffer += LayerSize[focusLayer - 1];
			focusLayer--;
		}

		for (int i = 0; i < LayerSize[layer]; i++)
		{
			layerOutput[i] = neurons[i + buffer]->Generate(input);
		}
		delete[] input;
		input = layerOutput;
	}

	return input;
}

void* NeuralNetwork::Data()
{
	/*
	We need to store:
		int LayerNum;    Size = 4 bytes
		int* LayerSize;   Size = LayerNum * 4 bytes
		Neuron** neurons;   Size = NeuronDataSize
	
	*/

	void** NeuronData = new void* [NeuronNum];

	for (int i = 0; i < NeuronNum; i++)
	{
		NeuronData[i] = neurons[i]->Data();
	}

	int NeuronDataSize = 0;
	for (int i = 0; i < NeuronNum; i++)
	{
		NeuronDataSize += GetNeuronDataSize(NeuronData[i]);
	}

	int size = sizeof(int) + (LayerNum * sizeof(int)) + NeuronDataSize;

	void* output = malloc(size+sizeof(int));

	if (output == nullptr)
	{
		return nullptr;
	}

	output = (int*)output + 1;

	int* LayerNumPos = (int*)output;

	int* LayerSizePos = (int*)output + 1;

	void* NeuronDataPos = (int*)output + 1 + LayerNum;

	*LayerNumPos = LayerNum;
	for (int i = 0; i < LayerNum; i++)
	{
		*(LayerSizePos + i) = LayerSize[i];
	}
	for (int i = 0; i < NeuronNum; i++)
	{
		int currentSize = GetNeuronDataSize(NeuronData[i]);
		memcpy(NeuronDataPos, NeuronData[i], currentSize);
		free(NeuronData[i]);
		NeuronDataPos = (char*)NeuronDataPos + currentSize;
	}

	delete[] NeuronData;
	//free(output);

	output = (int*)output - 1;
	*((int*)output) = size + sizeof(int);

	return output; //It is your responsibility now! :)
}

void NeuralNetwork::Load(void* data)
{
	/*
	To make:
		Neuron** neurons;
		int* LayerSize;
		int LayerNum;
		int NeuronNum;
	*/

	int size = ((int*)data)[0];
	data = (int*)data + 1;
	size -= 4;

	int* LayerNumPos = (int*)data;
	LayerNum = *LayerNumPos;

	int* LayerSizePos = (int*)data + 1;
	memcpy(LayerSize, LayerSizePos, sizeof(int) * LayerNum);

	void* NeuronDataPos = (int*)data + 1 + LayerNum;
	//TODO: make this


}

int NeuralNetwork::GetNeuronDataSize(void* data)
{
	return sizeof(int) + ((*(int*)data) * sizeof(float)) + sizeof(float) + sizeof(char);
}
