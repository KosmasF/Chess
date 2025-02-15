#include "GpuNeuralNetwork.h"

GpuNeuralNetwork::GpuNeuralNetwork(int *layerSize, int layerNum, ActivationMethodsEnum *activationMethods, GPU *gpu) 
    : gpu(gpu), LayerNum(layerNum)
{
    cl_int ret;

    // Copy the LayerSize array
    LayerSize = (int*)malloc(layerNum * sizeof(int));
    memcpy(LayerSize, layerSize, layerNum * sizeof(int));

    int numberOfWeights = 0;
    for(int layer = 1; layer < layerNum; layer++)
    {
        numberOfWeights += layerSize[layer] * layerSize[layer - 1];
    }
    int numberOfNeurons = 0;
    for(int i = 1; i < layerNum; i++)// i = 1 because the input layer is excluded
    {
        numberOfNeurons += layerSize[i];
    }

    // Allocate memory for the weights and biases
    weights = clCreateBuffer(gpu->kernelData.context, CL_MEM_READ_WRITE, numberOfWeights * sizeof(float), nullptr, nullptr);
    biases = clCreateBuffer(gpu->kernelData.context, CL_MEM_READ_WRITE, numberOfNeurons * sizeof(float), nullptr, nullptr);

    float weightsNum = 1.f;
    float biasesNum = 0.f;
    ret = clEnqueueFillBuffer(gpu->kernelData.command_queue, weights, &weightsNum, sizeof(float), 0, numberOfWeights * sizeof(float), 0, nullptr, nullptr);
    ret = clEnqueueFillBuffer(gpu->kernelData.command_queue, biases, &biasesNum, sizeof(float), 0, numberOfNeurons * sizeof(float), 0, nullptr, nullptr);

    // Copy the activation methods
    ActivationMethods = (ActivationMethodsEnum*)malloc((layerNum - 1) * sizeof(ActivationMethodsEnum));
    memcpy(ActivationMethods, activationMethods, (layerNum - 1) * sizeof(ActivationMethodsEnum));

    weightSubbuffers = (cl_mem*)malloc((layerNum - 1) * sizeof(cl_mem));
    biasSubbuffers = (cl_mem*)malloc((layerNum - 1) * sizeof(cl_mem));

    size_t buffer = 0;
    for(int i = 1; i < layerNum; i++){
        size_t size = layerSize[i] * layerSize[i - 1];
        cl_buffer_region region = {
            .origin = buffer * sizeof(float),
            .size = size * sizeof(float)
        };
        weightSubbuffers[i - 1] = clCreateSubBuffer(weights, CL_MEM_READ_WRITE, CL_BUFFER_CREATE_TYPE_REGION, &region, nullptr);
        buffer += size;
    }

    buffer = 0;
    for(int i = 1; i < layerNum; i++){
        size_t size = layerSize[i];
        cl_buffer_region region = {
            .origin = buffer * sizeof(float),
            .size = size * sizeof(float)
        };
        biasSubbuffers[i - 1] = clCreateSubBuffer(biases, CL_MEM_READ_WRITE, CL_BUFFER_CREATE_TYPE_REGION, &region, nullptr);
        buffer += size;
    }
}

GpuNeuralNetwork::GpuNeuralNetwork(const char *path, GPU *gpu)
: gpu(gpu)
{
	int seperatorPos;
	for(int i = strlen(path); i > 0; i--)
		if(path[i] == '.')
		{
			seperatorPos = i;
			break;
		}

	const char* type = path + seperatorPos;
    if(strcmp(type, ".nn3") != 0){
        printf("Invalid file type\n");
    }
    else{
        FILE* file = fopen(path, "rb");
        fread(&LayerNum, sizeof(int), 1, file);
        LayerSize = (int*)malloc(LayerNum * sizeof(int));
        fread(LayerSize, sizeof(int), LayerNum, file);
        ActivationMethods = (ActivationMethodsEnum*)malloc((LayerNum - 1) * sizeof(ActivationMethodsEnum));
        fread(ActivationMethods, sizeof(ActivationMethodsEnum), LayerNum - 1, file);
        int numberOfWeights = 0;
        for(int layer = 1; layer < LayerNum; layer++)
        {
            numberOfWeights += LayerSize[layer] * LayerSize[layer - 1];
        }
        float *weightsData = (float*)malloc(numberOfWeights * sizeof(float));
        fread(weightsData, sizeof(float), numberOfWeights, file);
        int layerNum = 0;
        for(int i = 1; i < LayerNum; i++)
        {
            layerNum += LayerSize[i];
        }
        float *biasesData = (float*)malloc(layerNum * sizeof(float));
        fread(biasesData, sizeof(float), layerNum, file);
        fclose(file);
        weights = clCreateBuffer(gpu->kernelData.context, CL_MEM_READ_WRITE, numberOfWeights * sizeof(float), nullptr, nullptr);
        biases = clCreateBuffer(gpu->kernelData.context, CL_MEM_READ_WRITE, layerNum * sizeof(float), nullptr, nullptr);
        clEnqueueWriteBuffer(gpu->kernelData.command_queue, weights, CL_TRUE, 0, numberOfWeights * sizeof(float), weightsData, 0, nullptr, nullptr);
        clEnqueueWriteBuffer(gpu->kernelData.command_queue, biases, CL_TRUE, 0, layerNum * sizeof(float), biasesData, 0, nullptr, nullptr);
        free(weightsData);
        free(biasesData);

        weightSubbuffers = (cl_mem*)malloc((layerNum - 1) * sizeof(cl_mem));
        biasSubbuffers = (cl_mem*)malloc((layerNum - 1) * sizeof(cl_mem));

        size_t buffer = 0;
        for(int i = 1; i < layerNum; i++){
            size_t size = LayerSize[i] * LayerSize[i - 1];
            cl_buffer_region region = {
                .origin = buffer * sizeof(float),
                .size = size * sizeof(float)
            };
            weightSubbuffers[i - 1] = clCreateSubBuffer(weights, CL_MEM_READ_WRITE, CL_BUFFER_CREATE_TYPE_REGION, &region, nullptr);
            buffer += size;
        }

        buffer = 0;
        for(int i = 1; i < layerNum; i++){
            size_t size = LayerSize[i];
            cl_buffer_region region = {
                .origin = buffer * sizeof(float),
                .size = size * sizeof(float)
            };
            biasSubbuffers[i - 1] = clCreateSubBuffer(biases, CL_MEM_READ_WRITE, CL_BUFFER_CREATE_TYPE_REGION, &region, nullptr);
            buffer += size;
        }
    }
}

GpuNeuralNetwork::~GpuNeuralNetwork()
{
    free(LayerSize);
    free(ActivationMethods);
    free(weightSubbuffers);
    free(biasSubbuffers);
    clReleaseMemObject(weights);
    clReleaseMemObject(biases);
}

cl_mem GpuNeuralNetwork::Generate(float *input, bool freeInput)
{
    cl_mem inp = gpu->MatrixTimesColumnVector(input, weights, LayerSize[1], LayerSize[0]);
    gpu->VectorIncrement(inp, biases, LayerSize[1]);
    if(freeInput) {
        free(input);
    }
    for(int layer = 2; layer < LayerNum; layer++)
    {
        cl_mem layerOutput = gpu->MatrixTimesColumnVector(inp, weightSubbuffers[layer - 1], LayerSize[layer], LayerSize[layer - 1]);
        gpu->VectorIncrement(layerOutput, biasSubbuffers[layer - 1], LayerSize[layer]);
        clReleaseMemObject(inp);
        inp = layerOutput;
    }
    return inp;
}

float* GpuNeuralNetwork::GenerateAndCopyToMem(float* input, bool freeInput)
{
    cl_int ret;
    cl_mem out = Generate(input, freeInput);
    float* res = (float*)malloc(LayerSize[LayerNum - 1] * sizeof(float));
    ret = clEnqueueReadBuffer(gpu->kernelData.command_queue, out, CL_TRUE, 0, LayerSize[LayerNum - 1] * sizeof(float), res, 0, nullptr, nullptr);
    ret = clReleaseMemObject(out);
    return res;
}

void GpuNeuralNetwork::Save(const char *path)
{
    FILE *file = fopen(path, "wb");
    fwrite(&LayerNum, sizeof(int), 1, file);
    fwrite(LayerSize, sizeof(int), LayerNum, file);
    fwrite(ActivationMethods, sizeof(ActivationMethodsEnum), LayerNum - 1, file);
    int numberOfWeights = 0;
    for(int layer = 1; layer < LayerNum; layer++)
    {
        numberOfWeights += LayerSize[layer] * LayerSize[layer - 1];
    }
    float *weightsData = (float*)malloc(numberOfWeights * sizeof(float));
    clEnqueueReadBuffer(gpu->kernelData.command_queue, weights, CL_TRUE, 0, numberOfWeights * sizeof(float), weightsData, 0, nullptr, nullptr);
    fwrite(weightsData, sizeof(float), numberOfWeights, file);
    free(weightsData);
    int layerNum = 0;
    for(int i = 1; i < LayerNum; i++)
    {
        layerNum += LayerSize[i];
    }
    float *biasesData = (float*)malloc(layerNum * sizeof(float));
    clEnqueueReadBuffer(gpu->kernelData.command_queue, biases, CL_TRUE, 0, layerNum * sizeof(float), biasesData, 0, nullptr, nullptr);
    fwrite(biasesData, sizeof(float), layerNum, file);
    free(biasesData);
    fclose(file);
    return;
}

void GpuNeuralNetwork::BackPropagate(float *input, float *output, float learningRate)
{
    gpu->BackPropagate(input, output, LayerSize, LayerNum, learningRate, weightSubbuffers, biasSubbuffers, ActivationMethods);
}