#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <string.h>
#include <iostream>
#include <string>
#include <cmath>

#ifdef _MSVC_LANG //Check if we are using c++ with visual studio
#pragma warning (disable : 4996)
#endif //_MSVC_LANG

#ifdef __OPENCL_CL_H // If we have opecl included in inside the .lib
struct FileData {
    const char* src;
    const int size;
};

struct KernelData {
    cl_device_id device_id;
    cl_context context;
    cl_command_queue command_queue;
    cl_uint selected_platform_index;
};
#endif //__OPENCL_CL_H

class GPU
{
public:
    GPU();
    ~GPU();

    float* AvgVector(float** vectors, float numVectors, float vectorLength);
    float* BackPropagate(const float* activations, const float* expectedOutput, const int* LayerSize, const int LayerNum, const float mutationRate, const int weightsNum, const float* weights, const int* weights_buffer_lookup_table);

#ifdef __OPENCL_CL_H // If we have opecl included in inside the .lib
private:
    KernelData kernelData;

    cl_uint ret_num_platforms = 0;
    cl_uint GetPlatformIndex(cl_platform_id* platforms);

    FileData LoadFile(const char* path);//"vector.cl"
    KernelData Setup();
    cl_program BuildFromFile(const char* path);
    cl_program BuildFromString(const char* source);

    const char* avgVectorResizable(int numVectors);
#endif //__OPENCL_CL_H

};

