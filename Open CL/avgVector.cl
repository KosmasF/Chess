__kernel void avg_vector(__global const float* input, __constant int* num, __global float* output, __constant int* vectorSize) 
{
int idx = get_global_size(0);

float result = 0;

for(int i = 0; i < *num; i++)
{
    result += input[(i * (*vectorSize)) + idx];
}

output[idx] = result;
}