#pragma once
#include "ActivationMethods.h"
#include "random"
#include "chrono"
#define _UNIFIED_WEIGTS_ARRAY

//#error

class Neuron // *deprecated* 
{
public:
	float* weights;
	int numWeights;
	float bias;
	float (*ActivationMethod)(float);

	void SetRandomData();

	signed char* preferredWeightGrowth;// Can be anything, char is just smaller ;0
	signed char preferredBiasGrowth;
	//Make this optimiser in the future


public:
	Neuron(int inputSize , float (*Activation)(float), bool normalize = false);
	Neuron(int inputSize, float (*Activation)(float), bool normalize, float* global_weights_array, int& weight_buffer);

	~Neuron();

	float Generate(float* input);

	void* Data();

	void Load(void* data);

	float RandomFloat();

	void Mutate(float rate);
};

