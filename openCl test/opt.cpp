for (int layer = LayerNum - 1; layer > 0; layer--)
{
	int start_j = StartingIndexOfLayerIncludingInputLayer(layer-1);
	int start_i = StartingIndexOfLayerIncludingInputLayer(layer);
	int idx = GetNumberOfWeights();

	int LayerIndex = NeuronNum;

	if(layer == LayerNum - 1)
	{
		for (int i = LayerSize[layer] - 1; i > -1; i--)// i is destination
		{
			int neuron = (i + start_i) - LayerSize[0];
			float cachedDerivative = (activations[neuron + LayerSize[0]] - predictedOutput[neuron - NeuronNum]);
			forwardNeuronsDerivatives[neuron] = cachedDerivative;

			for (int j = LayerSize[layer - 1] - 1; j > -1; j--)//j is the source
			{	
				data[idx] = activations[j + start_j] * cachedDerivative * (-mutationRate);
				idx--;
			}
		}
	}
	else
	{
		for (int i = LayerSize[layer] - 1; i > -1; i--)// i is destination
		{
			int neuron = (i + start_i) - LayerSize[0];
			float forwardNeuronsDerivative = 0;
			for (int n = LayerIndex; n < LayerIndex + (LayerSize[layer + 1]); n++)
			{
				forwardNeuronsDerivative += neurons[n]->weights[i] * forwardNeuronsDerivatives[n];//GetWeightBetweenNeurons(neuron, n)
			}

			float cachedDerivative = forwardNeuronsDerivative;
			forwardNeuronsDerivatives[neuron] = cachedDerivative;

			for (int j = LayerSize[layer - 1] - 1; j > -1; j--)//j is the source
			{	
				data[idx] = activations[j + start_j] * cachedDerivative * (-mutationRate);
				idx--;
			}
		}
		LayerIndex -= LayerSize[layer + 1];

	}
}
