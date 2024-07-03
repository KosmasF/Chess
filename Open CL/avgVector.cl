__kernel void avg_vector(__global const float** input, __constant int num) 
{
    int idx = get_global_size(NULL);

    float result = 0;

    for(int i = 0; i < num; i++)
    {
        result += input[i][idx];
    }

    return result;
}

__kernel void back_prop(__constant int layer,__constant LayerNum, __constant int* LayerSize, __global float* forwardNeuronsDerivatives, __constant int start_j, __constant int start_i)
{
    //I have to fix idx

    const int i = get_global_id(0);
    const int j = get_global_id(1);

    if(layer == LayerNum - 1)
    {
        int neuron = (i + start_i) - LayerSize[0];
        float cachedDerivative = (activations[neuron + LayerSize[0]] - predictedOutput[neuron - NeuronNum]);
        forwardNeuronsDerivatives[neuron] = cachedDerivative;
        data[idx] = activations[j + start_j] * cachedDerivative * (-mutationRate);
        idx--;
	}
	else
	{
        int neuron = (i + start_i) - LayerSize[0];
        float forwardNeuronsDerivative = 0;
        for (int n = LayerIndex; n < LayerIndex + (LayerSize[layer + 1]); n++)
        {
            forwardNeuronsDerivative += neurons[n]->weights[i] * forwardNeuronsDerivatives[n];//GetWeightBetweenNeurons(neuron, n)
        }

        float cachedDerivative = forwardNeuronsDerivative;
        forwardNeuronsDerivatives[neuron] = cachedDerivative;
        data[idx] = activations[j + start_j] * cachedDerivative * (-mutationRate);
        idx--;
	}
}