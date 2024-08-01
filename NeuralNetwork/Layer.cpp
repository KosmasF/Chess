#include "Layer.h"
#include <vcruntime_string.h>
#include <stdlib.h>


void Layer::SetWeights(bool normalizeOutput)
{
	size_t num = neurons * inputNeurons;
	if (normalizeOutput)
	{
		float n = 1;
		memset(weightsMatrix, n, sizeof(float) * num);
	}
	else
	{
		float n = 1.f / inputNeurons;
		memset(weightsMatrix, n, sizeof(float) * num);
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
