#include "NeuralNetwork.h"
#include <iostream>
#include <intrin.h>

NeuralNetwork::NeuralNetwork(int* layerSize, int layerNum, float (*activationMethods[])(float), bool normalizeOutput)
{
	LayerSize = (int*)malloc(layerNum*sizeof(int));
	memcpy(LayerSize, layerSize, layerNum * sizeof(int));
	LayerNum = layerNum;

	SetNeuronNum();

	neurons = new Neuron*[NeuronNum];

	for (int layer = 1; layer < layerNum; layer++)
	{
		int buffer = 0;
		int focusLayer = layer;

		while (focusLayer > 1)
		{
			buffer += layerSize[focusLayer - 1];
			focusLayer--;
		}
		for (int i = 0; i < layerSize[layer]; i++)
		{
			neurons[i+buffer] = new Neuron(layerSize[layer - 1], activationMethods[layer - 1], normalizeOutput);
		}
	}

	//Finished
}

NeuralNetwork::NeuralNetwork(const char* path)
{
	LayerNum = 0;
	NeuronNum = 0;

	neurons = nullptr;
	LayerSize = nullptr;
	void* data = FromDiskData(path);
	void* LoadData = data;

	int size = ((int*)data)[0];
	data = (int*)data + 1;
	size -= 4;

	int* LayerNumPos = (int*)data;
	LayerNum = *LayerNumPos;
	LayerSize = (int*)malloc(*LayerNumPos*sizeof(int));

	int* LayerSizePos = (int*)data + 1;
	memcpy(LayerSize, LayerSizePos, sizeof(int) * LayerNum);

	NeuronNum = RetNeuronNum();
	neurons = new Neuron * [NeuronNum];

	for (int layer = 1; layer < LayerNum; layer++)
	{
		int buffer = 0;
		int focusLayer = layer;

		while (focusLayer > 1)
		{
			buffer += LayerSize[focusLayer - 1];
			focusLayer--;
		}
		for (int i = 0; i < LayerSize[layer]; i++)
		{
			neurons[i + buffer] = new Neuron(LayerSize[layer - 1], layer != LayerNum - 1 ? None : None);
		}
	}

	Load(LoadData);
	free(LoadData);
}

NeuralNetwork::~NeuralNetwork()
{
	for (int i = 0; i < NeuronNum; i++)
	{
		delete neurons[i];
	}
	delete[] neurons;

	free(LayerSize);
}

float* NeuralNetwork::Generate(float* input, bool freeInput)
{
	//int inputLength = LayerSize[0];

	for (int layer = 1; layer < LayerNum; layer++)
	{
		float* layerOutput = new float[LayerSize[layer]];


		int buffer = 0;
		int focusLayer = layer;
		while (focusLayer > 1)
		{
			buffer += LayerSize[focusLayer - 1];
			focusLayer--;
		}

		for (int i = 0; i < LayerSize[layer]; i++)
		{
			layerOutput[i] = neurons[i + buffer]->Generate(input);
		}
		if (!freeInput && layer == 1)
			;
		else
			delete[] input;
		input = layerOutput;
	}

	return input;
}

void* NeuralNetwork::Data()
{
	/*
	We need to store:
		int LayerNum;    Size = 4 bytes
		int* LayerSize;   Size = LayerNum * 4 bytes
		Neuron** neurons;   Size = NeuronDataSize
	
	*/

	void** NeuronData = new void* [NeuronNum];

	for (int i = 0; i < NeuronNum; i++)
	{
		NeuronData[i] = neurons[i]->Data();
	}

	int NeuronDataSize = 0;
	for (int i = 0; i < NeuronNum; i++)
	{
		NeuronDataSize += GetNeuronDataSize(NeuronData[i]);
	}

	int size = sizeof(int) + (LayerNum * sizeof(int)) + NeuronDataSize;

	void* output = malloc(size+sizeof(int));

	if (output == nullptr)
	{
		return nullptr;
	}

	output = (int*)output + 1;

	int* LayerNumPos = (int*)output;

	int* LayerSizePos = (int*)output + 1;

	void* NeuronDataPos = (int*)output + 1 + LayerNum;

	*LayerNumPos = LayerNum;
	for (int i = 0; i < LayerNum; i++)
	{
		*(LayerSizePos + i) = LayerSize[i];
	}
	for (int i = 0; i < NeuronNum; i++)
	{
		int currentSize = GetNeuronDataSize(NeuronData[i]);
		memcpy(NeuronDataPos, NeuronData[i], currentSize);
		free(NeuronData[i]);
		NeuronDataPos = (char*)NeuronDataPos + currentSize;
	}

	delete[] NeuronData;
	//free(output);

	output = (int*)output - 1;
	*((int*)output) = size + sizeof(int);

	return output; //It is your responsibility now! :)
}

void NeuralNetwork::Load(void* data)
{
	/*
	To make:
		Neuron** neurons;
		int* LayerSize;
		int LayerNum;
		int NeuronNum;
	*/
	int size = ((int*)data)[0];
	data = (int*)data + 1;
	size -= 4;

	int* LayerNumPos = (int*)data;
	LayerNum = *LayerNumPos;

	int* LayerSizePos = (int*)data + 1;
	memcpy(LayerSize, LayerSizePos, sizeof(int) * LayerNum);

	void* NeuronDataPos = (int*)data + 1 + LayerNum;
	//TODO: make this

	SetNeuronNum();

	for(int i = 0; i < NeuronNum; i++)
	{
		int neuronSize = GetNeuronDataSize(NeuronDataPos);
		neurons[i]->Load(NeuronDataPos);
		NeuronDataPos = (char*)NeuronDataPos + neuronSize;
	}
}

int NeuralNetwork::GetNeuronDataSize(void* data)
{
	return sizeof(int) + ((*(int*)data) * sizeof(float)) + sizeof(float) + sizeof(char);
}

int NeuralNetwork::GetNumberOfWeights()
{
	int result = 0;
	for (int layer = 1; layer < LayerNum; layer++)
	{
		result += LayerSize[layer]*LayerSize[layer-1];
	}
	return result;
}

float NeuralNetwork::GetWeightBetweenNeurons(int From, int To)
{
	return neurons[To]->weights[LayerRelativeOfNeuron(From)];
}

int NeuralNetwork::LayerOfNeuron(int neuron)
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

int NeuralNetwork::LayerRelativeOfNeuron(int neuron)
{
	int buffer = 0;
	for (int i = 1; i < LayerNum; i++)
	{
		buffer += LayerSize[i];
		if (neuron < buffer)
			return neuron - StartingIndexOfLayer(i);
	}
	return -1;
}

int NeuralNetwork::LayerRelativeOfNeuronIncludingInputLayer(int neuron)
{
	int buffer = 0;
	for (int i = 0; i < LayerNum; i++)
	{
		buffer += LayerSize[i];
		if (neuron < buffer)
			return neuron - StartingIndexOfLayerIncludingInputLayer(i);
	}
	return -1;
}

int NeuralNetwork::LayerOfNeuronIncludingInputLayer(int neuron)
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

float* NeuralNetwork::GetAllActivations(float* input)
{
	float* result = (float*)malloc(sizeof(float) * (NeuronNum+LayerSize[0]));
	if (result == nullptr) return nullptr;

	int inputLength = LayerSize[0];
	for (int i = 0; i < inputLength; i++)
	{
		result[i] = input[i];
	}
	int resultBuffer = inputLength;

	for (int layer = 1; layer < LayerNum; layer++)
	{
		float* layerOutput = new float[LayerSize[layer]];


		int buffer = 0;
		int focusLayer = layer;
		while (focusLayer > 1)
		{
			buffer += LayerSize[focusLayer - 1];
			focusLayer--;
		}

		for (int i = 0; i < LayerSize[layer]; i++)
		{
			layerOutput[i] = neurons[i + buffer]->Generate(input);
			result[resultBuffer] = layerOutput[i];
			resultBuffer++;
		}
		delete[] input;
		input = layerOutput;
	}

	delete[] input;// MEM0RY LEAK BABYYYYYYY!!!!!!!!!!!!!

	return result;
}

float NeuralNetwork::PartialDerivativeOfErrorFunction(int neuron, float* activations, float* predictedOutput)
{
	if (LayerOfNeuron(neuron) == LayerNum - 1)
	{
		if (neurons[neuron]->ActivationMethod == None)
		{
			return (activations[neuron+LayerSize[0]] - predictedOutput[neuron-StartingIndexOfLayer(LayerNum-1)]);
		}
		else if (neurons[neuron]->ActivationMethod == Sigmoid)
		{
			return (activations[neuron + LayerSize[0]] - predictedOutput[neuron - StartingIndexOfLayer(LayerNum - 1)]) * 2 * activations[neuron + LayerSize[0]] * (1 - activations[neuron + LayerSize[0]]);
		}
		else
		{
			return 0.0f;
		}
	}
	else
	{
		//return 0.0f;//TEMP
		if (neurons[neuron]->ActivationMethod == Sigmoid)
		{
			float forwardNeuronsDerivative = 0;
			for (int i = StartingIndexOfLayer(LayerOfNeuron(neuron) + 1); i < StartingIndexOfLayer(LayerOfNeuron(neuron) + 1) + (LayerSize[LayerOfNeuron(neuron) + 1]); i++)
			{
				forwardNeuronsDerivative += GetWeightBetweenNeurons(neuron, i) * PartialDerivativeOfErrorFunction(i, activations, predictedOutput);
			}

			return forwardNeuronsDerivative * 2 * activations[neuron+LayerSize[0]] * (1 - activations[neuron+LayerSize[0]]);
		}
		else if (neurons[neuron]->ActivationMethod == None)
		{
			float forwardNeuronsDerivative = 0;
			for (int i = StartingIndexOfLayer(LayerOfNeuron(neuron) + 1); i < StartingIndexOfLayer(LayerOfNeuron(neuron) + 1) + (LayerSize[LayerOfNeuron(neuron) + 1]); i++)
			{
				forwardNeuronsDerivative += GetWeightBetweenNeurons(neuron, i) * PartialDerivativeOfErrorFunction(i, activations, predictedOutput);
			}

			return forwardNeuronsDerivative;
		}
		else
		{
			return 0.0f;
		}
	}
}

float NeuralNetwork::PartialDerivativeOfErrorFunction(int neuron, float* activations, float* predictedOutput, float* forwardNeuronsDerivatives)
{
	//if (forwardNeuronsDerivatives[neuron] == 0)
	//{
		//Hasn't been calculated
	//}
	if (LayerOfNeuron(neuron) == LayerNum - 1)
	{
		if (neurons[neuron]->ActivationMethod == None)
		{
			if (forwardNeuronsDerivatives[neuron] == 0)
				forwardNeuronsDerivatives[neuron] = (activations[neuron + LayerSize[0]] - predictedOutput[neuron - StartingIndexOfLayer(LayerNum - 1)]);
			return forwardNeuronsDerivatives[neuron];
		}
		else if (neurons[neuron]->ActivationMethod == Sigmoid)
		{
			if (forwardNeuronsDerivatives[neuron] == 0)
				forwardNeuronsDerivatives[neuron] = (activations[neuron + LayerSize[0]] - predictedOutput[neuron - StartingIndexOfLayer(LayerNum - 1)]) * 2 * activations[neuron + LayerSize[0]] * (1 - activations[neuron + LayerSize[0]]);
			return forwardNeuronsDerivatives[neuron];
		}
		else
		{
			if (forwardNeuronsDerivatives[neuron] == 0)
				forwardNeuronsDerivatives[neuron] = 1;
			return forwardNeuronsDerivatives[neuron];
		}
	}
	else
	{
		//return 0.0f;//TEMP
		if (neurons[neuron]->ActivationMethod == Sigmoid)
		{
			if (forwardNeuronsDerivatives[neuron] == 0)
			{
				float forwardNeuronsDerivative = 0;
				for (int i = StartingIndexOfLayer(LayerOfNeuron(neuron) + 1); i < StartingIndexOfLayer(LayerOfNeuron(neuron) + 1) + (LayerSize[LayerOfNeuron(neuron) + 1]); i++)
				{
					forwardNeuronsDerivative += GetWeightBetweenNeurons(neuron, i) * forwardNeuronsDerivatives[i - LayerSize[0]];
				}
				forwardNeuronsDerivatives[neuron] = forwardNeuronsDerivative * 2 * activations[neuron + LayerSize[0]] * (1 - activations[neuron + LayerSize[0]]);
			}

			return forwardNeuronsDerivatives[neuron];
		}
		else if (neurons[neuron]->ActivationMethod == None)
		{
			if (forwardNeuronsDerivatives[neuron] == 0)
			{
				float forwardNeuronsDerivative = 0;
				for (int i = StartingIndexOfLayer(LayerOfNeuron(neuron) + 1); i < StartingIndexOfLayer(LayerOfNeuron(neuron) + 1) + (LayerSize[LayerOfNeuron(neuron) + 1]); i++)
				{
					forwardNeuronsDerivative += GetWeightBetweenNeurons(neuron, i) * forwardNeuronsDerivatives[i];
				}

				if (forwardNeuronsDerivative == 0)
				{
					forwardNeuronsDerivative += std::numeric_limits<float>::min();
				}
				forwardNeuronsDerivatives[neuron] = forwardNeuronsDerivative;
			}
			return forwardNeuronsDerivatives[neuron];
		}
		else
		{
			if (forwardNeuronsDerivatives[neuron] == 0)
				forwardNeuronsDerivatives[neuron] = 1;
			return forwardNeuronsDerivatives[neuron];
		}
	}
}

/*
float* NeuralNetwork::PreCalcNeuronDerivatives()
{
	int NonInputNeuronNum = NeuronNum - LayerSize[0];

	float* result = (float*)malloc(sizeof(float) * (NonInputNeuronNum));

	for (int i = NonInputNeuronNum; i > 0; i--)
	{
		result[i];
	}


	return result;
}*/

int NeuralNetwork::StartingIndexOfLayer(int layer)
{
	int result = 0;
	for (int i = 1; i < layer; i++)
	{
		result += LayerSize[i];
	}
	return result;
}

int NeuralNetwork::StartingIndexOfLayerIncludingInputLayer(int layer)
{
	int result = 0;
	for (int i = 0; i < layer; i++)
	{
		result += LayerSize[i];
	}
	return result;
}

int NeuralNetwork::GetIndexOfWeight(int source, int destination)
{
	//The data includes input layer
	if (destination < source)
		__debugbreak();

	int result = 0;

	int layer = LayerOfNeuronIncludingInputLayer(source);
	while (layer > 0)
	{
		result += LayerSize[layer] * LayerSize[layer - 1];
		layer--;
	}

	result += LayerRelativeOfNeuronIncludingInputLayer(source) * LayerSize[LayerOfNeuronIncludingInputLayer(destination)];

	result += LayerRelativeOfNeuronIncludingInputLayer(destination);

	return result;
}


int NeuralNetwork::RetNeuronNum()
{
	int result = 0;

	for (int layer = 1; layer < LayerNum; layer++)
	{
		result += LayerSize[layer];
	}

	return result;
}


void NeuralNetwork::Save(const char* path)
{
	void* data = Data();
	int dataSize = ((int*)data)[0];
	FILE* file = fopen(path, "wb");
	fwrite(data, dataSize, 1, file);
	std::cout << "Saved file as: " << path << std::endl;
	fclose(file);
	free(data);
}

void NeuralNetwork::LoadFromDisk(const char* path)
{
	FILE* file = fopen(path, "rb");
	int* size = new int;
	fread(size, sizeof(int), 1, file);
	std::cout << "Loaded file: "<< path << " with size of: " << *size << " bytes" << std::endl;
	void* data = malloc(*size);

	if (data == nullptr)
		return;

	fclose(file);
	file = fopen(path, "rb");
	fread(data, *size, 1, file);
	Load(data);
	fclose(file);
	free(data);
	delete size;
}

void* NeuralNetwork::FromDiskData(const char* path)
{
	FILE* file = fopen(path, "rb");
	int* size = new int;
	fread(size, sizeof(int), 1, file);
	std::cout << "Loaded file: " << path << " with size of: " << *size << " bytes" << std::endl;
	void* data = malloc(*size);

	if (data == nullptr)
		return nullptr;

	fclose(file);
	file = fopen(path, "rb");
	fread(data, *size, 1, file);
	fclose(file);
	delete size;
	return data;
}

void NeuralNetwork::SetNeuronNum()
{
	this->NeuronNum = 0;

	for (int layer = 1; layer < LayerNum; layer++)
	{
		this->NeuronNum += LayerSize[layer];
	}
}

void NeuralNetwork::Mutate(float mutationRate)
{
	srand(rand());

	for (int i = 0; i < NeuronNum; i++)
	{
		neurons[i]->Mutate(mutationRate);
	}
}

float* NeuralNetwork::BackPropagate(float* expectedOutput, float* input, float mutationRate)
{
	float* data = (float*)malloc(sizeof(float) * GetNumberOfWeights());
	if (data == nullptr)
		return nullptr;

	//float zeroValuesCounter = 0;
	//float zeroedActivations = 0;

	float* activations = GetAllActivations(input);
	float* forwardNeuronsDerivatives = (float*)malloc((NeuronNum) * sizeof(float));
	memset(forwardNeuronsDerivatives, 0, (NeuronNum) * sizeof(float));


	for (int layer = LayerNum - 1; layer > 0; layer--)
	{
		for (int j = 0; j < LayerSize[layer - 1]; j++)
		{
			for (int i = 0; i < LayerSize[layer]; i++)
			{
				//In index LayerSize[0] is added!
				int I = i + StartingIndexOfLayerIncludingInputLayer(layer);// I is destination
				int J = j + StartingIndexOfLayerIncludingInputLayer(layer-1);// J is the source
				// Considering PartialDerivativeOfErrorFunction() input layer is inexistent
				data[GetIndexOfWeight(J , I)] = activations[J] * PartialDerivativeOfErrorFunction(I - LayerSize[0], activations, expectedOutput, forwardNeuronsDerivatives) * (-mutationRate);
				/*if (data[GetIndexOfWeight(J, I)] == 0)
				{
					//__debugbreak();
					zeroValuesCounter++;
					if (activations[J] == 0)
					{
						zeroedActivations++;
					}
					activations[J] * PartialDerivativeOfErrorFunction(I - LayerSize[0], activations, expectedOutput, forwardNeuronsDerivatives)* (-mutationRate);
				}*/
			}
		}
	}

	//float percentage = (zeroValuesCounter / GetNumberOfWeights()) * 100;
	//printf("Zeroes: %i and with total percentage: %f %c , Activation's fault: %f %c \n", (int)zeroValuesCounter , percentage, '%', zeroedActivations / zeroValuesCounter * 100 , '%');

	free(activations); //MEM0RY LEAK!!!// _>
	free(forwardNeuronsDerivatives);
	return data;
}

float* NeuralNetwork::EmptyGradient()
{
	float* data = (float*)malloc(sizeof(float) * GetNumberOfWeights());
	memset(data, 0, sizeof(float) * GetNumberOfWeights());

	return data;
}

void NeuralNetwork::AddToWeights(float* data)
{
	//int buffer = 0;
	for (int layer = 1; layer < LayerNum; layer++)
	{
		for (int neuron = 0; neuron < LayerSize[layer]; neuron++)
		{
			for (int i = 0; i < LayerSize[layer - 1]; i++)
			{
				int index = GetIndexOfWeight(StartingIndexOfLayerIncludingInputLayer(layer - 1) + i, StartingIndexOfLayerIncludingInputLayer(layer) + neuron);
				neurons[StartingIndexOfLayer(layer)+neuron]->weights[i] += data[index];
				//if (g != buffer)
				//{
					//__debugbreak();
				//}
				//g = GetIndexOfWeight(StartingIndexOfLayerIncludingInputLayer(layer - 1) + i, StartingIndexOfLayerIncludingInputLayer(layer) + neuron);
				//buffer++;
			}
		}
	}
}

float NeuralNetwork::GetLoss(float* output, float* predictedOutput)
{
	float loss = 0;
	float total = 0;
	for (int i = 0; i < LayerSize[LayerNum - 1]; i++)
	{
		float diff = (output[i]) - (predictedOutput[i]);
		loss += diff*diff;
		total++;
	}
	return loss;
}

float* NeuralNetwork::AverageWeightVector(float** vectors, int num)
{
	float* result = new float[GetNumberOfWeights()];

	for (int i = 0; i < GetNumberOfWeights(); i++)
	{
		float average = 0;
		for (int n = 0; n < num; n++)
		{
			average += vectors[n][i];
		}
		average /= num;
		result[i] = average;
	}

	return result;
}
