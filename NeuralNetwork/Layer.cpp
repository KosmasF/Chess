#include "Layer.h"
#include <vcruntime_string.h>

void Layer::setWeights(bool normalizeOutput)
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

Layer::Layer(int numNeurons, int numInputNeurons, float* weightsPTR)
{
	inputNeurons = numInputNeurons;
	neurons = numNeurons;
	weightsMatrix = weightsPTR;
}
