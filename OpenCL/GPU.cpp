#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define _CRT_SECURE_NO_WARNINGS
#include <CL/cl.h>

#include "GPU.h"
#include "raylib.h"


GPU::GPU()
{
    //--------------------------------Setup---------------------------------------------
    kernelData = Setup();
	printf("GPU SETTING UP!!!\n");



}

GPU::~GPU()
{
    cl_uint ret;
    ret = clReleaseCommandQueue(kernelData.command_queue);
    ret = clReleaseContext(kernelData.context);
}

cl_uint GPU::GetPlatformIndex(cl_platform_id* platforms) {

    char* required_platform_subname = (char*)malloc(strlen("Graphics") + 1);
    cl_uint selected_platform_index = 3; //Start at max

    strcpy(required_platform_subname, "Graphics"); //Names as per CapsBasic

    //std::cout << "Reqd name = " << required_platform_subname << std::endl;
    for (cl_uint i = 0; i < ret_num_platforms; ++i)
    {
    cl_int err;

    // Determine platform name length
    size_t platform_name_length = 0;
    err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, nullptr, &platform_name_length);
    if (err != CL_SUCCESS) {
        // Handle error
        std::cerr << "Error getting platform name length: " << err << std::endl;
        return -1;
    }

        // Get the name itself for the i-th platform
        char* platform_name = new char[platform_name_length];
        clGetPlatformInfo(
            platforms[i],
            CL_PLATFORM_NAME,
            platform_name_length,
            platform_name,
            0
        );

        // decide if this i-th platform is what we are looking for
        // we select the first one matched skipping the next one if any
        if (
            strstr(platform_name, required_platform_subname)
            //&&            selected_platform_index == num_of_platforms // have not selected yet
            )
        {
           // std::cout << " [Selected] " << i << std::endl;
            selected_platform_index = i;
            delete[] platform_name;
            //free(required_platform_subname);
            return selected_platform_index;
            // return the first match
        }

        //        cout << endl;
        //        delete [] platform_name;
    }
    free(required_platform_subname);
    return -1;
}

FileData GPU::LoadFile(const char* path)
{
    FILE* fp;
    char* source_str;

    fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load file.\n");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    source_str = (char*)malloc(size + 1);
    int sz = fread(source_str,sizeof(char), size, fp);
    fclose(fp);

    source_str[size - (size - sz)] = 0;

    return { source_str, size - (size - sz) };
}

KernelData GPU::Setup()
{
    cl_device_type platformType = CL_DEVICE_TYPE_GPU;

    std::cout << "Platform " << platformType << std::endl;//" Matrix size " << SIZE << "x" << SIZE << " Tile size " << TILE_SIZE << std::endl;

    //Init variables
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    
    //Get number of platforms
    cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);


    cl_platform_id* platform_id = new cl_platform_id[ret_num_platforms]; //List of platforms

    std::cout << "clGetPlatformIDs " << ret_num_platforms << std::endl;

    // Get platform and device information
    ret = clGetPlatformIDs(ret_num_platforms, platform_id, 0); //Returns the list of platforms found. Minimum of arg1 and arg3.

    std::cout << "clGetPlatformIDs List Ret = " << ret << std::endl;

    cl_uint selected_platform_index = 0;//GetPlatformIndex(platform_id);

    std::cout << "getPlatformIndex " << selected_platform_index << std::endl;

    cl_platform_id platformCPU = platform_id[selected_platform_index];

    ret = clGetDeviceIDs(platformCPU, platformType, 1, &device_id, &ret_num_devices); //Returns the devices found
    std::cout << "clGetDeviceIDs " << ret << std::endl;
    // Create an OpenCL context
    //An OpenCL context is created with one or more devices. Contexts are used by the OpenCL runtime for managing objects such as command-queues, memory, program and kernel objects and for executing kernels on one or more devices specified in the context.
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue with the capability of performance profiling for target device
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);

    delete[] platform_id;

    return {
        device_id,
        context,
        command_queue,
        selected_platform_index
    };
}

cl_program GPU::BuildFromFile(const char* path, const char* args)
{
    cl_int ret;
    FileData file = LoadFile(path);
    size_t size = file.size;

    cl_program program = clCreateProgramWithSource(kernelData.context, 1, &(file.src), & (size), &ret);

    // Build and compile the OpenCL kernel program
    //std::string build_option = "-DTILE_SIZE=" + std::to_string(TILE_SIZE);
    ret = clBuildProgram(program, 1, &kernelData.device_id, args, NULL, NULL);
    if (ret == CL_BUILD_PROGRAM_FAILURE) { // If compile failed, print the error message
        // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(program, kernelData.device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = (char*)malloc(log_size);

        // Get the log and print it
        clGetProgramBuildInfo(program, kernelData.device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("%s\n", log);
    }

    free((void*)file.src);
    return program;
}

cl_program GPU::BuildFromString(const char* source)
{
    cl_int ret;

    size_t size = strlen(source);
    cl_program program = clCreateProgramWithSource(kernelData.context, 1, &(source), &(size), &ret);

    // Build and compile the OpenCL kernel program
    //std::string build_option = "-DTILE_SIZE=" + std::to_string(TILE_SIZE);
    ret = clBuildProgram(program, 1, &kernelData.device_id, "", NULL, NULL);
    if (ret == CL_BUILD_PROGRAM_FAILURE) { // If compile failed, print the error message
        // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(program, kernelData.device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = (char*)malloc(log_size);

        // Get the log and print it
        clGetProgramBuildInfo(program, kernelData.device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("%s\n", log);
    }

    return program;
}

size_t GPU::GetMaxLocalWorkSize()
{
    cl_int ret;

    cl_uint maxDimensions;
    ret = clGetDeviceInfo(kernelData.device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &maxDimensions, NULL);
    size_t* max_work_sizes = (size_t*)malloc(sizeof(size_t) * maxDimensions);
    ret = clGetDeviceInfo(kernelData.device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * maxDimensions, max_work_sizes, NULL);
    size_t max_work_size = max_work_sizes[0];
    free(max_work_sizes);


    return max_work_size; 
}

const char* GPU::avgVectorResizable(int numVectors)
{
    const int maxSize = 1000;
    char* result = (char*)malloc(maxSize * sizeof(char));
    int buffer = 0;

    const char* start = "__kernel void avg_vector(";
    memcpy(result + buffer, start, strlen(start));
    buffer += strlen(start);

    for (int i = 0; i < numVectors; i++)
    {
        const char* string = "__global const float* vec1,";
        char* vectorArguments = (char*)malloc(strlen(string));
        memcpy(vectorArguments, string, strlen(string));
        vectorArguments[25] = i + 65;
        memcpy(result + buffer, vectorArguments, strlen(string));
        buffer += strlen(string);
        free(vectorArguments);
    }

    const char* middle = "__global float* output){ int idx = get_global_id(0); int vectorSize = get_global_size(0);float result =  ";
    memcpy(result + buffer, middle, strlen(middle));
    buffer += strlen(middle);

    for (int i = 0; i < numVectors; i++)
    {
        const char* string = "vec1[idx]";
        char* vectorArguments = (char*)malloc(strlen(string));
        memcpy(vectorArguments, string, strlen(string));
        vectorArguments[3] = i + 65;
        memcpy(result + buffer, vectorArguments, strlen(string));
        buffer += strlen(string);
        free(vectorArguments);
        if (i != numVectors - 1)
        {
            const char* plus = "+";
            memcpy(result + buffer, plus, strlen(plus));
            buffer += strlen(plus);
        }
        else
        {
            const char* plus = "/";
            memcpy(result + buffer, plus, strlen(plus));
            buffer += strlen(plus);
        }
    }

    char str[20];
    sprintf(str, "%d", numVectors);
    memcpy(result + buffer, str, strlen(str));
    buffer += strlen(str);

    const char* end = ";output[idx] = result;}";
    memcpy(result + buffer, end, strlen(end));
    buffer += strlen(end);

    result[buffer] = 0;
    buffer++;

    return result;
}

float* GPU::AvgVector(float** vectors,const float numVectors, float vectorLength)
{
    cl_int inputSize = vectorLength  * sizeof(float);
    cl_int outputSize = vectorLength * sizeof(float);

    float* output = (float*)malloc(outputSize);
    memset(output, 0, vectorLength * sizeof(float));

    /*float* vectorsUnited = (float*)malloc(inputSize);
    for (int i = 0; i < numVectors; i++)
    {
        int buffer = i * vectorLength;
        memcpy(vectorsUnited + buffer, vectors[i], vectorLength * sizeof(float));
    }*/
    //avgVectorResizable(10);

    //---------------------------------Build Code------------------------------------------------------------

    const char* code = avgVectorResizable(numVectors);
    cl_program program = BuildFromString(code);
    free((void*)code);

    //---------------------------------Memory buffers------------------------------------------------------
    cl_int ret;

    cl_mem *vector_buffers = (cl_mem*)malloc(numVectors * sizeof(cl_mem));

    for (int i = 0; i < numVectors; i++)
    {
        vector_buffers[i] = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, inputSize, NULL, &ret);
    }
    //cl_mem size_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, sizeof(int), NULL, &ret);
    cl_mem output_buffer = clCreateBuffer(kernelData.context, CL_MEM_WRITE_ONLY, outputSize, NULL, &ret);
    //cl_mem vector_size_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, sizeof(int), NULL, &ret);

    // Copy the matrix A, B and C to each device memory counterpart

    for (int i = 0; i < numVectors; i++)
    {
        ret = clEnqueueWriteBuffer(kernelData.command_queue, vector_buffers[i], CL_TRUE, 0, inputSize, vectors[i], 0, NULL, NULL);
    }
    //ret = clEnqueueWriteBuffer(kernelData.command_queue, size_buffer, CL_TRUE, 0, sizeof(int), &numVectors, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(kernelData.command_queue, output_buffer, CL_TRUE, 0, outputSize, output, 0, NULL, NULL);
    //ret = clEnqueueWriteBuffer(kernelData.command_queue, vector_size_buffer, CL_TRUE, 0, sizeof(int), &vectorLength, 0, NULL, NULL);

    //------------------------------------Execute------------------------------------------------------------
    //double st = GetTime();
    // Create the OpenCL kernel
    cl_kernel kernel;
    kernel = clCreateKernel(program, "avg_vector", &ret);

    // Set the arguments of the kernel

    for (int i = 0; i < numVectors; i++)
    {
        ret = clSetKernelArg(kernel, i, sizeof(cl_mem), (void*)&vector_buffers[i]);
    }
    ret = clSetKernelArg(kernel, numVectors, sizeof(cl_mem), (void*)&output_buffer);// __global float* output
    // = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&vector_size_buffer);// __constant int* vectorSize

    cl_uint maxDimensions;
    ret = clGetDeviceInfo(kernelData.device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &maxDimensions, NULL);
    size_t* max_work_sizes = (size_t*)malloc(sizeof(size_t)*maxDimensions);
    ret = clGetDeviceInfo(kernelData.device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * maxDimensions, max_work_sizes, NULL);
    size_t max_work_size = max_work_sizes[0];
    cl_int dimensions = 1;
    size_t local_item_size[] = { max_work_size };
    size_t global_item_size[] = {vectorLength};
    free(max_work_sizes);


    cl_event perf_event;

    // Execute the OpenCL kernel
    //ret = clEnqueueNDRangeKernel(kernelData.command_queue, kernel, dimensions, NULL, global_item_size, local_item_size, 0, NULL, &perf_event);
    //ret = clEnqueueTask(kernelData.command_queue, kernel, NULL, NULL, &perf_event);
    ret = clEnqueueNDRangeKernel(kernelData.command_queue, kernel, dimensions, NULL, global_item_size, local_item_size, NULL, nullptr, &perf_event);
    //CL_INVALID_KERNEL
    // Capture performance event from target device. In this case the event is to retrive the execution time.
    ret = clWaitForEvents(1, &perf_event);
    //ret = clGetEventProfilingInfo(perf_event, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
    //ret = clGetEventProfilingInfo(perf_event, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);
    //std::cout << "OpenCL matrix multiplication: " << (float)(end - start) / 1000000000 << " sec" << std::endl;

    // Read the memory buffer C from the device into the local variable C

    // Make sure all the command in the command queue has been executed
    ret = clFinish(kernelData.command_queue);

    ret = clEnqueueReadBuffer(kernelData.command_queue, output_buffer, CL_TRUE, 0, outputSize, output, NULL, nullptr, NULL);

    //------------------------------------------CleanUp------------------------------------------------------------
    for (int i = 0; i < numVectors; i++)
    {
        ret = clReleaseMemObject(vector_buffers[i]);
    }
    ret = clReleaseMemObject(output_buffer);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);

    //free(vectorsUnited);
    free(vector_buffers);
    return output;
}

float* GPU::BackPropagate(const float* activations, const float* expectedOutput, const int* LayerSize, const int LayerNum, const float mutationRate, const int weightsNum, const float* weights, const int* weights_buffer_lookup_table)
{
    float* data = (float*)malloc(sizeof(float) * weightsNum);
    //data[0] = 100.f;
    if (data == nullptr)
        return nullptr;


    //float zeroValuesCounter = 0;
    //float zeroedActivations = 0;

    int NeuronNum = 0;
    for (int i = 1; i < LayerNum; i++)
    {
        NeuronNum += LayerSize[i];
    }


    float* forwardNeuronsDerivatives = (float*)malloc((NeuronNum) * sizeof(float));
    memset(forwardNeuronsDerivatives, 0, (NeuronNum) * sizeof(float));




    cl_program program = BuildFromFile("../OpenCL/back_prop.cl", "-cl-std=CL2.0");

    cl_int ret;

    //-------------------------------------------------------BUFFERS-----------------------------------------------------------------------

    //Declare

    cl_mem activations_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, (NeuronNum + LayerSize[0]) * sizeof(float), NULL, &ret);
    cl_mem expected_output_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, LayerSize[LayerNum - 1] * sizeof(float), NULL, &ret);
    cl_mem layer_size_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, LayerNum * sizeof(int), NULL, &ret);
    cl_mem weights_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, weightsNum * sizeof(float), NULL, &ret);
    cl_mem weights_buffer_lookup_table_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, NeuronNum * sizeof(int), NULL, &ret);

    cl_mem forward_neuron_derivatives_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_WRITE, NeuronNum * sizeof(float), NULL, &ret);

    cl_mem data_buffer = clCreateBuffer(kernelData.context, CL_MEM_WRITE_ONLY, weightsNum * sizeof(float), NULL, &ret);

    //Init

    ret = clEnqueueWriteBuffer(kernelData.command_queue, activations_buffer, CL_TRUE, 0, (NeuronNum + LayerSize[0]) * sizeof(float), activations, 0, nullptr, nullptr);
    ret = clEnqueueWriteBuffer(kernelData.command_queue, expected_output_buffer, CL_TRUE, 0, LayerSize[LayerNum - 1] * sizeof(float), expectedOutput, 0, nullptr, nullptr);
    ret = clEnqueueWriteBuffer(kernelData.command_queue, layer_size_buffer, CL_TRUE, 0, LayerNum * sizeof(int), LayerSize, 0, nullptr, nullptr);
    ret = clEnqueueWriteBuffer(kernelData.command_queue, weights_buffer, CL_TRUE, 0, weightsNum * sizeof(float), weights, 0, nullptr, nullptr);
    ret = clEnqueueWriteBuffer(kernelData.command_queue, weights_buffer_lookup_table_buffer, CL_TRUE, 0, NeuronNum * sizeof(int), weights_buffer_lookup_table, 0, nullptr, nullptr);

    ret = clEnqueueWriteBuffer(kernelData.command_queue, forward_neuron_derivatives_buffer, CL_TRUE, 0, NeuronNum * sizeof(float), forwardNeuronsDerivatives, 0, nullptr, nullptr);


    for (int layer = LayerNum - 1; layer > 0; layer--)
    {
        bool isLastLayer = layer == LayerNum - 1;
        cl_kernel kernel;

        if (isLastLayer)
        {
            //Create kernel

            kernel = clCreateKernel(program, "back_prob_last_layer", &ret);

            //Set args

            ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &activations_buffer);
            ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), &expected_output_buffer);
            ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), &layer_size_buffer);
            ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), &weights_buffer);
            ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), &weights_buffer_lookup_table_buffer);
            ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), &forward_neuron_derivatives_buffer);
            ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), &data_buffer);
            ret = clSetKernelArg(kernel, 7, sizeof(float), &mutationRate);
            ret = clSetKernelArg(kernel, 8, sizeof(int), &LayerNum);
            ret = clSetKernelArg(kernel, 9, sizeof(int), &layer);
        }
        else
        {
            kernel = clCreateKernel(program, "back_prob_hidden_layer", &ret);


            ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &activations_buffer);
            ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), &expected_output_buffer);
            ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), &layer_size_buffer);
            ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), &weights_buffer);
            ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), &weights_buffer_lookup_table_buffer);
            ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), &forward_neuron_derivatives_buffer);
            ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), &data_buffer);
            ret = clSetKernelArg(kernel, 7, sizeof(float), &mutationRate);
            ret = clSetKernelArg(kernel, 8, sizeof(int), &LayerNum);
            ret = clSetKernelArg(kernel, 9, sizeof(int), &layer);

            ret = clEnqueueReadBuffer(kernelData.command_queue, forward_neuron_derivatives_buffer, CL_TRUE, 0, NeuronNum * sizeof(float), forwardNeuronsDerivatives, 0, nullptr, nullptr);

            SetHiddenLayerForwardNeuronDerivative(forwardNeuronsDerivatives, LayerSize, weights, weights_buffer_lookup_table, layer);

            ret = clEnqueueWriteBuffer(kernelData.command_queue, forward_neuron_derivatives_buffer, CL_TRUE, 0, NeuronNum * sizeof(float), forwardNeuronsDerivatives, 0, nullptr, nullptr);
        }

        //Set dim + work size values

        cl_int dimensions = 2;
        size_t global_item_size[] = { LayerSize[layer - 1], LayerSize[layer] };
        size_t local_item_size[] = { 16,16 };

        //ND range kernel

        cl_event perf_event;
        ret = clEnqueueNDRangeKernel(kernelData.command_queue, kernel, dimensions, NULL, global_item_size, local_item_size, NULL, nullptr, &perf_event);

        //----------------------------------------------------------Execute---------------------------------------------------------

        ret = clFinish(kernelData.command_queue);
        ret = clReleaseKernel(kernel);
    }


    ret = clEnqueueReadBuffer(kernelData.command_queue, data_buffer, CL_TRUE, 0, weightsNum * sizeof(float), data, NULL, nullptr, NULL);

    //----------------------------------------------------CLEANUP

    
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(activations_buffer);
    ret = clReleaseMemObject(expected_output_buffer);
    ret = clReleaseMemObject(layer_size_buffer);
    ret = clReleaseMemObject(weights_buffer);
    ret = clReleaseMemObject(weights_buffer_lookup_table_buffer);
    ret = clReleaseMemObject(forward_neuron_derivatives_buffer);
    ret = clReleaseMemObject(data_buffer);

    free(forwardNeuronsDerivatives);
    free((void*)activations);
    return data;
}

float* GPU::vector_matrix_multiplication(const float* vector, const float* matrix, const int vec_width, const int matrix_width)
{
    float* output = (float*)malloc(matrix_width * sizeof(float));

    cl_program program = BuildFromFile("../OpenCL/vec_mat_mul.cl", "");

    cl_int ret;

    cl_mem vector_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, vec_width * sizeof(float), nullptr, &ret);
    cl_mem matrix_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, vec_width * matrix_width * sizeof(float), nullptr, &ret);
    cl_mem output_buffer = clCreateBuffer(kernelData.context, CL_MEM_WRITE_ONLY, matrix_width * sizeof(float), nullptr, &ret);

    ret = clEnqueueWriteBuffer(kernelData.command_queue, vector_buffer, CL_TRUE, 0, vec_width * sizeof(float), vector, NULL, nullptr, nullptr);
    ret = clEnqueueWriteBuffer(kernelData.command_queue, matrix_buffer, CL_TRUE, 0, vec_width * matrix_width * sizeof(float), matrix, NULL, nullptr, nullptr);

    cl_kernel kernel = clCreateKernel(program, "vec_mat_mul", &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &vector_buffer);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), &matrix_buffer);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), &output_buffer);
    ret = clSetKernelArg(kernel, 3, sizeof(int), &vec_width);

    int dimensions = 1;
    size_t global_work_size[] = { matrix_width };
    size_t max_local_work_size = GetMaxLocalWorkSize();
    size_t local_work_size[] = { max_local_work_size > global_work_size[0] ? matrix_width : max_local_work_size};

    ret = clEnqueueNDRangeKernel(kernelData.command_queue, kernel, dimensions, 0, global_work_size, local_work_size, 0, nullptr, nullptr);

    ret = clFinish(kernelData.command_queue);

    ret = clEnqueueReadBuffer(kernelData.command_queue, output_buffer, CL_TRUE, 0, matrix_width * sizeof(float), output, 0, nullptr, nullptr);

    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(vector_buffer);
    ret = clReleaseMemObject(matrix_buffer);
    ret = clReleaseMemObject(output_buffer);

    return output;
}

float* GPU::GetHiddenLayerForwardNeuronDerivative(const float* forwardNeuronDerivatives, const int* LayerSize, const float* weights, const int* weights_buffer_lookup_table, int layer)
{
    int fd_idx = 0;
    for (int i = 1; i < layer + 1; i++)
    {
        fd_idx += LayerSize[i];
    }

    const float* vector = forwardNeuronDerivatives + fd_idx;

    int wt_idx = 0;
    for (int i = 1; i < layer; i++)
    {
        wt_idx += LayerSize[i];
    }

    int w_idx = weights_buffer_lookup_table[wt_idx];

    const float* matrix = weights + w_idx;
    
    float* output = vector_matrix_multiplication(vector, matrix, LayerSize[layer + 1], LayerSize[layer]);

    return output;
}

void GPU::SetHiddenLayerForwardNeuronDerivative(float* forwardNeuronDerivatives, const int* LayerSize, const float* weights, const int* weights_buffer_lookup_table, int layer)
{
    float* output = GetHiddenLayerForwardNeuronDerivative(forwardNeuronDerivatives, LayerSize, weights, weights_buffer_lookup_table, layer);

    int idx = 0;
    for (int i = 1; i < layer; i++)
    {
        idx += LayerSize[i];
    }

    memcpy(forwardNeuronDerivatives + idx, output, LayerSize[layer] * sizeof(float));

    free(output);
}

void GPU::VectorIncrement(float* A, const float* B, const int size)
{
    cl_int ret;
    cl_program program = BuildFromFile("../OpenCL/vec_add.cl", "");

    cl_mem A_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, size * sizeof(float), nullptr, &ret);
    cl_mem B_buffer = clCreateBuffer(kernelData.context, CL_MEM_READ_ONLY, size * sizeof(float), nullptr, &ret);
    cl_mem C_buffer = clCreateBuffer(kernelData.context, CL_MEM_WRITE_ONLY, size * sizeof(float), nullptr, &ret);

    ret = clEnqueueWriteBuffer(kernelData.command_queue, A_buffer, CL_TRUE, 0, size * sizeof(float), A, 0, nullptr, nullptr);
    ret = clEnqueueWriteBuffer(kernelData.command_queue, B_buffer, CL_TRUE, 0, size * sizeof(float), B, 0, nullptr, nullptr);

    cl_kernel kernel = clCreateKernel(program, "vec_add", &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &A_buffer);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), &B_buffer);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), &C_buffer);

    const int dimensions = 1;
    size_t global_work_size[] = { size };
    size_t local_work_size[] = { GetMaxLocalWorkSize() };

    ret = clEnqueueNDRangeKernel(kernelData.command_queue, kernel, dimensions, 0, global_work_size, local_work_size, 0, nullptr, nullptr);

    ret = clFinish(kernelData.command_queue);

    ret = clEnqueueReadBuffer(kernelData.command_queue, C_buffer, CL_TRUE, 0, size * sizeof(float), A, 0, nullptr, nullptr);
 
    ret = clReleaseKernel(kernel);
    ret = clRetainProgram(program);
    ret = clReleaseMemObject(A_buffer);
    ret = clReleaseMemObject(B_buffer);
    ret = clReleaseMemObject(C_buffer);
}
