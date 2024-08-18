#include "Layer.h"
#include <stdlib.h>
#include <algorithm>


void Layer::SetWeights(bool normalizeOutput)
{
	size_t num = neurons * inputNeurons;
	if (normalizeOutput)
	{
		float n = 1;
		std::fill(weightsMatrix, weightsMatrix + num, n);
	}
	else
	{
		float n = 1.f / inputNeurons;
		std::fill(weightsMatrix, weightsMatrix + num, n);
	}

}

Layer::Layer(int numNeurons, int numInputNeurons, float* weightsPTR, float (*ActMethod)(float))
{
	inputNeurons = numInputNeurons;
	neurons = numNeurons;
	weightsMatrix = weightsPTR;
	ActivationMethod = ActMethod;
}

float* Layer::Generate(float* input, GPU* gpu)
{
	float* output = gpu->vector_matrix_multiplication(input, weightsMatrix, inputNeurons, neurons);

	return output;
}
