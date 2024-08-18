__kernel void avg_vector(__global const float* vec1,__global const float* vec2,  __global float* output) 
{
int idx = get_global_id(0);
int vectorSize = get_global_size(0);

float result = vec1[idx] + vec2[idx] / 2;

output[idx] = result;
}