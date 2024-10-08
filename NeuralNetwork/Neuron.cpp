#include "Neuron.h"
#include <iostream>
#include <cstring>


void Neuron::SetRandomData()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

	for (int i = 0; i < numWeights; i++ )
	{
		weights[i] = 1;//dis(gen);
	}

	//bias = dis(gen);
	bias = 0;
}

Neuron::Neuron(int inputSize, float (*Activation)(float), bool normalize)
{
	weights = new float[inputSize];
	numWeights = inputSize;
	ActivationMethod = Activation;
	bias = 0;

	if (normalize)
	{
		for (int i = 0; i < inputSize; i++)
		{
			weights[i] = 1.f / inputSize;//+ 0.1f;
		}
	}
	else
		SetRandomData();
}

Neuron::Neuron(int inputSize, float(*Activation)(float), bool normalize, float* global_weights_array, int& weight_buffer)
{
	weights = global_weights_array + weight_buffer;
	numWeights = inputSize;
	weight_buffer += numWeights;
	ActivationMethod = Activation;
	bias = 0;

	if (normalize)
	{
		for (int i = 0; i < inputSize; i++)
		{
			weights[i] = 1.f / inputSize;//+ 0.1f;
		}
	}
	else
		SetRandomData();
}

Neuron::~Neuron()
{
#ifndef _UNIFIED_WEIGTS_ARRAY
	delete[] weights;
#endif
}

float Neuron::Generate(float* input)
{
	float output = 0;

	for (int i = 0; i < numWeights; i++)
	{
		output += input[i] * weights[i];
	}
	output += bias;
	output = ActivationMethod(output);

	return output;
}

void* Neuron::Data()
{	
	/*
	We need to store:
		int numWeights;  Size = 4 bytes
		float* weights;  Size = numWeights * 4 bytes
		float bias;		 Size = 4 bytes
		float (*ActivationMethod)(float); -> To char  Size = 1 byte
	*/
	int totalSize = sizeof(int) + (numWeights * sizeof(float)) + sizeof(float) + sizeof(char);

	void* output = malloc(totalSize);

	if (output == nullptr)
	{
		return nullptr;
	}

	int* numWeightsPos = (int*)output;

	float* weightsPos = (float*)((int*)output + 1);

	float* biasPos = (float*)((char*)output + sizeof(int) + (numWeights * sizeof(float)));

	char* ActivationMethodPos = (char*)output + sizeof(int) + (numWeights * sizeof(float)) + sizeof(float);

	*numWeightsPos = numWeights;
	for (int i = 0; i < numWeights; i++)
	{
		*(weightsPos + i) = weights[i];
	}
	*biasPos = bias;
	
	if (ActivationMethod == Sigmoid)
		*ActivationMethodPos = 'S';
	else if (ActivationMethod == NonNegativeLimitedLinear)
		*ActivationMethodPos = 'n';
	else
		*ActivationMethodPos = 0;


	//free(output);
	return output; //It is your responsibility now! :)
}

void Neuron::Load(void* data)
{
	int* numWeightsPos = (int*)data;
	numWeights = *numWeightsPos;

	float* weightsPos = (float*)((int*)data + 1);
	memcpy(weights, weightsPos, numWeights * sizeof(int));

	float* biasPos = (float*)((char*)data + sizeof(int) + (numWeights * sizeof(float)));
	bias = *biasPos;

	char* ActivationMethodPos = (char*)data + sizeof(int) + (numWeights * sizeof(float)) + sizeof(float);
	if (*ActivationMethodPos == 'S')
		ActivationMethod = Sigmoid;
	else if (*ActivationMethodPos == 'n')
		ActivationMethod = NonNegativeLimitedLinear;
	else
		ActivationMethod = None;
}

float Neuron::RandomFloat()
{
	return (float)(rand()) / (float)(RAND_MAX);
}

void Neuron::Mutate(float rate)
{
	for (int i = 0; i < numWeights; i++)
	{
		weights[i] += (RandomFloat() * rate);
	}
	bias += (RandomFloat() * rate);// *preferredWeightGrowth[i];
}
