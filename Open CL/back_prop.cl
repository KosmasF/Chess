int StartingIndexOfLayerIncludingInputLayer(int layer, const int* LayerSize)
{
	int result = 0;
	for (int i = 0; i < layer; i++)
	{
		result += LayerSize[i];
	}
	return result;
}
int LayerRelativeOfNeuronIncludingInputLayer(int neuron, const int* LayerSize, const int LayerNum)
{
	int buffer = 0;
	for (int i = 0; i < LayerNum; i++)
	{
		buffer += LayerSize[i];
		if (neuron < buffer)
			return neuron - StartingIndexOfLayerIncludingInputLayer(i, LayerSize);
	}
	return -1;
}
int LayerOfNeuronIncludingInputLayer(int neuron, const int* LayerSize, const int LayerNum)
{
	int buffer = 0;
	for (int i = 0; i < LayerNum; i++)
	{
		buffer += LayerSize[i];
		if (neuron < buffer)
			return i;
	}
	return -1;
}
int GetIndexOfWeight(int source, int destination, const int* LayerSize, const int LayerNum)
{
	int result = 0;

	int layer = LayerOfNeuronIncludingInputLayer(source, LayerSize, LayerNum);
	while (layer > 0)
	{
		result += LayerSize[layer] * LayerSize[layer - 1];
		layer--;
	}

	result += LayerRelativeOfNeuronIncludingInputLayer(source, LayerSize,LayerNum) * LayerSize[LayerOfNeuronIncludingInputLayer(destination,LayerSize, LayerNum)];

	result += LayerRelativeOfNeuronIncludingInputLayer(destination,LayerSize, LayerNum);

	return result;
}


int StartingIndexOfLayer(int layer, const int* LayerSize)
{
	int result = 0;
	for (int i = 1; i < layer; i++)
	{
		result += LayerSize[i];
	}
	return result;
}

int LayerOfNeuron(int neuron, const int* LayerSize, const int LayerNum)
{
	int buffer = 0;
	for (int i = 1; i < LayerNum; i++)
	{
		buffer += LayerSize[i];
		if (neuron < buffer)
			return i;
	}
	return -1;
}

float GetWeightBetweenNeurons(int From, int To)
{
	return weights[];
}

float PartialDerivativeOfErrorFunction(int neuron, float* activations, float* predictedOutput, float* forwardNeuronsDerivatives, const int* LayerSize, const int LayerNum)
{
	if (LayerOfNeuron(neuron, LayerSize, LayerNum) == LayerNum - 1)
	{
        if (forwardNeuronsDerivatives[neuron] == 0)
            forwardNeuronsDerivatives[neuron] = (activations[neuron + LayerSize[0]] - predictedOutput[neuron - StartingIndexOfLayer(LayerNum - 1, LayerSize)]);
        return forwardNeuronsDerivatives[neuron];
	}
	else
	{	
        if (forwardNeuronsDerivatives[neuron] == 0)
        {
            float forwardNeuronsDerivative = 0;
            for (int i = StartingIndexOfLayer(LayerOfNeuron(neuron, LayerSize, LayerNum) + 1, LayerSize); i < StartingIndexOfLayer(LayerOfNeuron(neuron, LayerSize, LayerNum) + 1, LayerSize) + (LayerSize[LayerOfNeuron(neuron, LayerSize, LayerNum) + 1]); i++)
            {
                forwardNeuronsDerivative += GetWeightBetweenNeurons(neuron, i) * forwardNeuronsDerivatives[i];
            }

            if (forwardNeuronsDerivative == 0)
            {
                forwardNeuronsDerivative += 0.01f;
            }
            forwardNeuronsDerivatives[neuron] = forwardNeuronsDerivative;
        }
        return forwardNeuronsDerivatives[neuron];
    }
}



__kernel void back_prob(__global const float* activations,
                        __global const float* expectedOutput,
                        __global const int* LayerSize,
                        __global float* forwardNeuronsDerivatives,
                        __global float* data,
                        int layer,
                        float mutationRate,
                        int LayerNum
)
{
    int j = get_global_id(0);
    int i = get_global_id(1);

    int I = i + StartingIndexOfLayerIncludingInputLayer(layer, LayerSize);
    int J = j + StartingIndexOfLayerIncludingInputLayer(layer-1, LayerSize);
    data[GetIndexOfWeight(J , I, LayerSize, LayerNum)] = activations[J] * PartialDerivativeOfErrorFunction(I - LayerSize[0], activations, expectedOutput, forwardNeuronsDerivatives, LayerSize, LayerNum) * (-mutationRate);

}

/*
    for (int j = 0; j < LayerSize[layer - 1]; j++)
    {
        for (int i = 0; i < LayerSize[layer]; i++)
        {
            int I = i + StartingIndexOfLayerIncludingInputLayer(layer);
            int J = j + StartingIndexOfLayerIncludingInputLayer(layer-1);
            data[GetIndexOfWeight(J , I)] = activations[J] * PartialDerivativeOfErrorFunction(I - LayerSize[0], activations, expectedOutput, forwardNeuronsDerivatives) * (-mutationRate);
        }
    }
*/