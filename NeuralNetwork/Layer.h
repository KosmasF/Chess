#pragma once


class Layer
{
public:
	int neurons;
	int inputNeurons;

	float* weightsMatrix;

	float* generate(float* input);
	void setWeights(bool normalizeOutput);
	Layer(int numNeurons, int numInputNeurons, float* weightsPTR);
};

