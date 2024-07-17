int StartingIndexOfLayerIncludingInputLayer(int layer, const __global int* LayerSize)
{
	int result = 0;
	for (int i = 0; i < layer; i++)
	{
		result += LayerSize[i];
	}
	return result;
}
int LayerRelativeOfNeuronIncludingInputLayer(int neuron, const __global int* LayerSize, const int LayerNum)
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
int LayerOfNeuronIncludingInputLayer(int neuron, const __global int* LayerSize, const int LayerNum)
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
int GetIndexOfWeight(int source, int destination, const __global int* LayerSize, const int LayerNum)
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


int StartingIndexOfLayer(int layer, const __global int* LayerSize)
{
	int result = 0;
	for (int i = 1; i < layer; i++)
	{
		result += LayerSize[i];
	}
	return result;
}

int LayerOfNeuron(int neuron, const __global int* LayerSize, const int LayerNum)
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

int GetIndexOfNeuron(int idx, const __global int* LayerSize, const int LayerNum)
{
	int rem = idx;
	int result = 0;
	for (int i = 1; i < LayerNum; i++)
	{
		rem -= LayerSize[i];
		if(rem <= 0)
		{
			break;
		}
		result += LayerSize[i];
	}
	return result + idx;
}

float GetWeightBetweenNeurons(int From, int To, const __global int* LayerSize, int LayerNum, const __global float* weights, const __global int* weights_buffer_lookup_table)
{
	return weights[weights_buffer_lookup_table[GetIndexOfNeuron(To, LayerSize, LayerNum)] + From - StartingIndexOfLayer(From, LayerSize)];
}

float PartialDerivativeOfErrorFunctionLastLayer(int neuron, __global float* activations, __global float* predictedOutput, __global float* forwardNeuronsDerivatives, const __global int* LayerSize, const int LayerNum, const __global float* weights, const __global int* weights_buffer_lookup_table)
{
	if (forwardNeuronsDerivatives[neuron] == 0)
		forwardNeuronsDerivatives[neuron] = (activations[neuron + LayerSize[0]] - predictedOutput[neuron - StartingIndexOfLayer(LayerNum - 1, LayerSize)]);
	return forwardNeuronsDerivatives[neuron];
}

__kernel void back_prob_last_layer(
						__global const float* activations,
                        __global const float* expectedOutput,
                        __global const int* LayerSize,
						__global const float* weights,
						__global const int* weights_buffer_lookup_table,
                        __global float* forwardNeuronsDerivatives,
                        __global float* data,
                        float mutationRate,
                        int LayerNum,
						int layer
)
{
    int j = get_global_id(0);
    int i = get_global_id(1);

    int I = i + StartingIndexOfLayerIncludingInputLayer(layer, LayerSize);
    int J = j + StartingIndexOfLayerIncludingInputLayer(layer-1, LayerSize);
    data[GetIndexOfWeight(J , I, LayerSize, LayerNum)] = activations[J] * PartialDerivativeOfErrorFunctionLastLayer(I - LayerSize[0], activations, expectedOutput, forwardNeuronsDerivatives, LayerSize, LayerNum, weights, weights_buffer_lookup_table) * (-mutationRate);

}

__kernel void back_prob_hidden_layer(
						__global const float* activations,
                        __global const float* expectedOutput,
                        __global const int* LayerSize,
						__global const float* weights,
						__global const int* weights_buffer_lookup_table,
                        __global float* forwardNeuronsDerivatives,
                        __global float* data,
                        float mutationRate,
                        int LayerNum,
						int layer					
)
{
    int j = get_global_id(0);
    int i = get_global_id(1);

    int I = i + StartingIndexOfLayerIncludingInputLayer(layer, LayerSize);
    int J = j + StartingIndexOfLayerIncludingInputLayer(layer-1, LayerSize);
    data[GetIndexOfWeight(J , I, LayerSize, LayerNum)] = activations[J] * forwardNeuronsDerivatives[I - LayerSize[0]] * (-mutationRate);

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