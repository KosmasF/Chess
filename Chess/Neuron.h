#pragma once
#include "ActivationMethods.h"
#include "random"

class Neuron
{
private:
	float* weights;
	int numWeights;
	float bias;
	float (*ActivationMethod)(float);

	void SetRandomData();

public:
	Neuron(int inputSize , float (*Activation)(float));

	float Generate(float* input);

	void* Data();

};

