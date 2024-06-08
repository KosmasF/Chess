#include "NeuralNetwork.h"
#include <iostream>

NeuralNetwork::NeuralNetwork(int* layerSize, int layerNum)
{
	LayerSize = layerSize;
	LayerNum = layerNum;

	SetNeuronNum();

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
			neurons[i+buffer] = new Neuron(layerSize[layer - 1], layer != LayerNum - 1 ? Sigmoid : None);
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

	SetNeuronNum();

	for(int i = 0; i < NeuronNum; i++)
	{
		int neuronSize = GetNeuronDataSize(NeuronDataPos);
		neurons[i]->Load(NeuronDataPos);
		NeuronDataPos = (char*)NeuronDataPos + neuronSize;
	}
}

int NeuralNetwork::GetNeuronDataSize(void* data)
{
	return sizeof(int) + ((*(int*)data) * sizeof(float)) + sizeof(float) + sizeof(char);
}

int NeuralNetwork::GetNumberOfWeights()
{
	int result = 0;
	for (int i = 1; i < NeuronNum; i++)
	{
		result += LayerSize[i];
	}
	return result;
}

float NeuralNetwork::GetWeightBetweenNeurons(int From, int To)
{
	return neurons[To]->weights[LayerRelativeOfNeuron(From)];
}

int NeuralNetwork::LayerOfNeuron(int neuron)
{
	int buffer = 0;
	for (int i = 0; i < LayerNum; i++)
	{
		buffer += LayerSize[LayerNum];
		if (neuron < buffer)
			return i;
	}
	return -1;
}

int NeuralNetwork::LayerRelativeOfNeuron(int neuron)
{
	int buffer = 0;
	for (int i = 0; i < LayerNum; i++)
	{
		buffer += LayerSize[LayerNum];
		if (neuron < buffer)
			return buffer - neuron;
	}
	return -1;
}

float* NeuralNetwork::GetAllActivations(float* input)
{
	float* result = (float*)malloc(sizeof(float) * NeuronNum);
	if (result == nullptr) return nullptr;

	int inputLength = LayerSize[0];
	for (int i = 0; i < inputLength; i++)
	{
		result[i] = input[i];
	}
	int resultBuffer = inputLength;

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
			result[resultBuffer] = layerOutput[i];
			resultBuffer++;
		}
		delete[] input;
		input = layerOutput;
	}

	return result;
}

float NeuralNetwork::PartialDerivativeOfErrorFunction(int neuron, float* activations, float* predictedOutput)
{
	if (LayerOfNeuron(neuron) == LayerNum - 1)
	{
		if (neurons[neuron]->ActivationMethod == None)
		{
			return (activations[neuron] - predictedOutput[neuron]);
		}
		else if (neurons[neuron]->ActivationMethod == Sigmoid)
		{
			return (activations[neuron] - predictedOutput[neuron]) * activations[neuron] * (1 - activations[neuron]);
		}
		else
		{
			return 0.0f;
		}
	}
	else
	{
		if (neurons[neuron]->ActivationMethod == Sigmoid)
		{
			int forwardNeuronsDerivative = 0;
			for (int i = StartingIndexOfLayer(LayerOfNeuron(neuron) + 1); i < StartingIndexOfLayer(LayerOfNeuron(neuron) + 1) + (LayerSize[LayerOfNeuron(neuron) + 1]); i++)
			{
				forwardNeuronsDerivative += GetWeightBetweenNeurons(neuron, i) * PartialDerivativeOfErrorFunction(i,activations,predictedOutput);
			}

			return forwardNeuronsDerivative * activations[neuron] * (1 - activations[neuron]);
		}
		else
		{
			return 0.0f;
		}
	}
}

int NeuralNetwork::StartingIndexOfLayer(int layer)
{
	int result = 0;
	for (int i = 0; i < layer; i++)
	{
		result += LayerSize[i];
	}
	return result;
}


void NeuralNetwork::Save(const char* path)
{
	void* data = Data();
	int dataSize = ((int*)data)[0];
	FILE* file = fopen(path, "wb");
	fwrite(data, dataSize, 1, file);
	std::cout << "Saved file as: " << path << std::endl;
	fclose(file);
	free(data);
}

void NeuralNetwork::LoadFromDisk(const char* path)
{
	FILE* file = fopen(path, "rb");
	int* size = new int;
	fread(size, sizeof(int), 1, file);
	std::cout << "Loaded file: "<< path << " with size of: " << *size << " bytes" << std::endl;
	void* data = malloc(*size);

	if (data == nullptr)
		return;

	fclose(file);
	file = fopen(path, "rb");
	fread(data, *size, 1, file);
	Load(data);
	fclose(file);
	free(data);
	delete size;
}

void NeuralNetwork::SetNeuronNum()
{
	NeuronNum = 0;

	for (int layer = 1; layer < LayerNum; layer++)
	{
		NeuronNum += LayerSize[layer];
	}
}

void NeuralNetwork::Mutate(float mutationRate)
{
	srand(rand());

	for (int i = 0; i < NeuronNum; i++)
	{
		neurons[i]->Mutate(mutationRate);
	}
}

float* NeuralNetwork::BackPropagate(float* expectedOutput, float* input, float mutationRate)
{
	float* data = (float*)malloc(sizeof(float) * GetNumberOfWeights());
	if (data == nullptr)
		return nullptr;

	float* activations = GetAllActivations(input);

	int buffer = 0;
	for (int i = 0; i < NeuronNum; i++)
	{
		for (int j = 0; j < LayerOfNeuron(i) + 1; j++)
		{
			data[buffer] = activations[i]*PartialDerivativeOfErrorFunction(j, activations, expectedOutput)*(-mutationRate);
			buffer++;
		}
	}

	return data;
}

void NeuralNetwork::AddToWeights(float* data)
{
}
