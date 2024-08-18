
__kernel void vec_mat_mul(
    __global const float* vec,
    __global const float* mat,
    __global float* out,
    int size
)
{
    int k = get_global_id(0);
    int n = get_global_size(0);


    float result = 0;
    for(int i = 0; i < size; i++)
    {
        result += vec[k] * mat[(i * n) + k];
    }   

    out[k] = result;
}