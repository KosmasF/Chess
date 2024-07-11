#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define _CRT_SECURE_NO_WARNINGS
#include <CL/cl.h>

#include "GPU.h"
#include "raylib.h"


GPU::GPU()
{
    //--------------------------------Setup---------------------------------------------
    kernelData = Setup();




}

GPU::~GPU()
{
    cl_uint ret;
    ret = clReleaseCommandQueue(kernelData.command_queue);
    ret = clReleaseContext(kernelData.context);
}

cl_uint GPU::GetPlatformIndex(cl_platform_id* platforms) {

    char* required_platform_subname = (char*)malloc(5);
    cl_uint selected_platform_index = 3; //Start at max

    strcpy(required_platform_subname, "Graphics"); //Names as per CapsBasic

    //std::cout << "Reqd name = " << required_platform_subname << std::endl;
    for (cl_uint i = 0; i < ret_num_platforms; ++i)
    {
        // Get the length for the i-th platform name
        size_t platform_name_length = 0;
        clGetPlatformInfo(
            platforms[i],
            CL_PLATFORM_NAME,
            0,
            0,
            &platform_name_length
        );

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

    //std::cout << "Platform " << platformType << std::endl;//" Matrix size " << SIZE << "x" << SIZE << " Tile size " << TILE_SIZE << std::endl;

    //Init variables
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;

    //Get number of platforms
    cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);


    cl_platform_id* platform_id = new cl_platform_id[ret_num_platforms]; //List of platforms

    //std::cout << "clGetPlatformIDs " << ret_num_platforms << std::endl;

    // Get platform and device information
    ret = clGetPlatformIDs(ret_num_platforms, platform_id, 0); //Returns the list of platforms found. Minimum of arg1 and arg3.

    //std::cout << "clGetPlatformIDs List Ret = " << ret << std::endl;

    cl_uint selected_platform_index = GetPlatformIndex(platform_id);

    //std::cout << "getPlatformIndex " << selected_platform_index << std::endl;

    cl_platform_id platformCPU = platform_id[selected_platform_index];

    ret = clGetDeviceIDs(platformCPU, platformType, 1, &device_id, &ret_num_devices); //Returns the devices found
    //std::cout << "clGetDeviceIDs " << ret << std::endl;
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

cl_program GPU::BuildFromFile(const char* path)
{
    cl_int ret;
    FileData file = LoadFile(path);
    size_t size = file.size;

    cl_program program = clCreateProgramWithSource(kernelData.context, 1, &(file.src), & (size), &ret);

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
    ret = clEnqueueReadBuffer(kernelData.command_queue, output_buffer, CL_TRUE, 0, outputSize, output, NULL, nullptr, NULL);

    // Make sure all the command in the command queue has been executed
    ret = clFinish(kernelData.command_queue);

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

void GPU::AvgVectorSetup(float numVectors)
{
}
