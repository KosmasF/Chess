#pragma once
#include "GPU.h"
#include "ActivationMethods.h"

class Layer
{
public:
	int neurons;
	int inputNeurons;

	float* weightsMatrix;

	float (*ActivationMethod)(float);

	float* Generate(float* input, GPU* gpu);
	void SetWeights(bool normalizeOutput);
	Layer(int numNeurons, int numInputNeurons, float* weightsPTR, float (*ActMethod)(float));
};

