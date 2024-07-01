__kernel void avg_vector(__global const float** input, __constant int num) {
{
    int idx = get_global_size(NULL);

    float result = 0;

    for(int i = 0; i < num; i++)
    {
        result += input[i][idx];
    }

    return result;
}

__kernel void 