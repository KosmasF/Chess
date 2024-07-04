#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <string.h>
#include <iostream>
#include <string>

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
    int RunForSize();
    float* AvgVector(float** vectors, float numVectors, float vectorLength);

    float* BackPropagate();

#ifdef __OPENCL_CL_H // If we have opecl included in inside the .lib
private:
    cl_uint ret_num_platforms = 0;
    cl_uint GetPlatformIndex(cl_platform_id* platforms);

    FileData LoadFile(const char* path);//"vector.cl"
    KernelData Setup();
    cl_program Build(const char* path, KernelData kernelData);
#endif //__OPENCL_CL_H

};

