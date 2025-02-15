#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "ActivationMethods.h"

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

// struct Vector_matrix_multiplication_data
// {
//     cl_kernel kernel;
//     cl_program program;
// };
// Vector_matrix_multiplication_data vector_matrix_multiplication_data;
#else
struct KernelData {
    long long device_id;
    long long context;
    long long command_queue;
    unsigned int selected_platform_index;
};

#endif //__OPENCL_CL_H

//  #include <CL/cl.h>
class GPU
{
    friend class GpuNeuralNetwork;
public:
    GPU();
    ~GPU();

    float* AvgVector(float** vectors, float numVectors, float vectorLength);
    //Note: activation is freed automatically
    float* BackPropagate(const float* activations, const float* expectedOutput, const int* LayerSize, const int LayerNum, const float mutationRate, const int weightsNum, const float* weights, const int* weights_buffer_lookup_table);
    //In this function the matrix gets flipped to suit my needs, ne careful!
    float* vector_matrix_multiplication(const float* vector, const float* matrix, const int vec_width, const int matrix_width);
    // void Setup_vector_matrix_multiplication();
    // void Destroy_vector_matrix_multiplication();

    float* GetHiddenLayerForwardNeuronDerivative(const float* forwardNeuronDerivatives, const int* LayerSize, const float* weights, const int* weights_buffer_lookup_table, int layer);
    void SetHiddenLayerForwardNeuronDerivative(float* forwardNeuronDerivatives, const int* LayerSize, const float* weights, const int* weights_buffer_lookup_table, int layer);
    void VectorIncrement(float* A, const float* B, const int size);
    void ApplyActivationMethod(float* input, int length, ActivationMethodsEnum activationMethod);

#ifdef __OPENCL_CL_H
    cl_mem MatrixTimesColumnVector(const float* vector, const cl_mem matrix, const int vec_width, const int matrix_width);
    cl_mem MatrixTimesColumnVector(const cl_mem vector, const cl_mem matrix, const int vec_width, const int matrix_width);
    cl_mem TransposedMatrixTimesColumnVector(const cl_mem vector, const cl_mem matrix, const int vec_width, const int matrix_width);
    cl_mem ColumnVectorTimesRowVector(const cl_mem A, const cl_mem B, const int A_size, const int B_size);
    void VectorIncrement(cl_mem A, const cl_mem B, const int size);

    cl_mem HadamardProduct(const cl_mem A, const cl_mem B, const int size);
    cl_mem SquareErrorGradient(const cl_mem output, const cl_mem expected, const int size);

    cl_mem ScalarVectorMultiplication(const cl_mem vec, const float scalar, const int size);
    void ScaleVector(cl_mem vec, const float scalar, const int size);

    void BackPropagate(const float* input, const float* expected_output, const int* LayerSize, const int LayerNum, const float learningRate, cl_mem* weightSubbuffers, cl_mem* biasSubbuffers, ActivationMethodsEnum* activationMethods); 
    void BackPropagate(const cl_mem input, const cl_mem expected_output, const int* LayerSize, const int LayerNum, const float learningRate, cl_mem* weightSubbuffers, cl_mem* biasSubbuffers, ActivationMethodsEnum* activationMethods);
#endif
private:
    pthread_mutex_t mutex;
#ifdef __OPENCL_CL_H // If we have opecl included in inside the .lib

    KernelData kernelData;

    cl_uint GetPlatformIndex(cl_platform_id* platforms, cl_uint ret_num_platforms);

    FileData LoadFile(const char* path);//"vector.cl"
    KernelData Setup();
    cl_program BuildFromFile(const char* path, const char* args);
    cl_program BuildFromString(const char* source);

    size_t GetMaxLocalWorkSize();

    const char* avgVectorResizable(int numVectors);

#else
private:// *DISABLED*
    KernelData kernelDataTEMP;
    unsigned int ret_num_platforms = 0;

#endif //__OPENCL_CL_H

};

