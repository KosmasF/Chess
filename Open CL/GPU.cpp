#include "GPU.h"

#include <CL/cl.h>

cl_uint GPU::GetPlatformIndex(cl_platform_id* platforms) {

    char* required_platform_subname = (char*)malloc(5);
    cl_uint selected_platform_index = 3; //Start at max

    strcpy(required_platform_subname, "Graphics"); //Names as per CapsBasic
    
    std::cout << "Reqd name = " << required_platform_subname << std::endl;
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
            std::cout << " [Selected] " << i << std::endl;
            selected_platform_index = i;
            delete[] platform_name;
            return selected_platform_index;
            // return the first match
        }

        //        cout << endl;
        //        delete [] platform_name;
    }
    return -1;
}

FileData GPU::LoadFile(const char* path)
{
    FILE* fp;
    char* source_str;

    fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    source_str = (char*)malloc(size + 1);
    fread(source_str, size, 1, fp);
    fclose(fp);

    source_str[size] = 0;

    return { source_str, size };
}


int GPU::RunForSize() {

    cl_device_type platformType = CL_DEVICE_TYPE_CPU;

    std::cout << "Platform " << platformType << std::endl;//" Matrix size " << SIZE << "x" << SIZE << " Tile size " << TILE_SIZE << std::endl;

Setup:

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

    cl_uint selected_platform_index = GetPlatformIndex(platform_id);

    std::cout << "getPlatformIndex " << selected_platform_index << std::endl;

    cl_platform_id platformCPU = platform_id[selected_platform_index];

    ret = clGetDeviceIDs(platformCPU, platformType, 1, &device_id, &ret_num_devices); //Returns the devices found
    std::cout << "clGetDeviceIDs " << ret << std::endl;
    // Create an OpenCL context
    //An OpenCL context is created with one or more devices. Contexts are used by the OpenCL runtime for managing objects such as command-queues, memory, program and kernel objects and for executing kernels on one or more devices specified in the context.
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue with the capability of performance profiling for target device
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);

MemorySetup:

    /*
    // Create memory buffers on the device for each matrix
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE * SIZE * sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE * SIZE * sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE * SIZE * sizeof(int), NULL, &ret);

    // Copy the matrix A, B and C to each device memory counterpart
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, SIZE * SIZE * sizeof(int), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, SIZE * SIZE * sizeof(int), B, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, c_mem_obj, CL_TRUE, 0, SIZE * SIZE * sizeof(int), C, 0, NULL, NULL);*/
    
Build:

    // Create a program from the kernel source
    FileData file = LoadFile("vector.cl");

    cl_program program = clCreateProgramWithSource(context, 1, &(file.src), (size_t*) & (file.size), &ret);

    // Build and compile the OpenCL kernel program
    //std::string build_option = "-DTILE_SIZE=" + std::to_string(TILE_SIZE);
    ret = clBuildProgram(program, 1, &device_id, "", NULL, NULL);
    if (ret == CL_BUILD_PROGRAM_FAILURE) { // If compile failed, print the error message
        // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = (char*)malloc(log_size);

        // Get the log and print it
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("%s\n", log);
    }

Execution:
    /*
    // Create the OpenCL kernel
    cl_kernel kernel;
    kernel = clCreateKernel(program, "matrix_mul", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&c_mem_obj);

    int dimention = 2; // In this example, We will use 2 dimention index
    size_t global_item_size[] = { SIZE, SIZE, 1 };
    size_t local_item_size[] = { TILE_SIZE, TILE_SIZE, 1 };

    cl_event perf_event;
    cl_ulong start, end;

    // Execute the OpenCL kernel
    ret = clEnqueueNDRangeKernel(command_queue, kernel, dimention, NULL, global_item_size, local_item_size, 0, NULL, &perf_event);
    // Capture performance event from target device. In this case the event is to retrive the execution time.
    ret = clWaitForEvents(1, &perf_event);
    ret = clGetEventProfilingInfo(perf_event, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
    ret = clGetEventProfilingInfo(perf_event, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);
    std::cout << "OpenCL matrix multiplication: " << (float)(end - start) / 1000000000 << " sec" << std::endl;

    // Read the memory buffer C from the device into the local variable C
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, SIZE * SIZE * sizeof(int), C, 0, NULL, NULL);

    // Make sure all the command in the command queue has been executed
    ret = clFinish(command_queue);*/

CleanUp:

    // Clean up
    //ret = clReleaseKernel(kernel); it is correct but the boy is commented
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free((void*)file.src);

    std::cout << "Press Enter to finish..." << std::endl;
    //getchar();
    return 0;
}