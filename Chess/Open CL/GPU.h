#pragma once

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable : 4996)

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <string.h>
#include <iostream>
#include <string>

//Pre def
typedef unsigned int cl_uint;
typedef struct _cl_platform_id* cl_platform_id;

struct FileData {
    const char* src;
    const int size;
};

class GPU
{
    GPU();

    cl_uint ret_num_platforms = 0;

    cl_uint GetPlatformIndex(cl_platform_id* platforms);

    int RunForSize();
    FileData LoadFile(const char* path);//"vector.cl"
};

