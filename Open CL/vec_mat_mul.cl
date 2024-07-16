__kernel void vec_mat_mul(
    __constant float* vec,
    __constant float* mat,
    __global float* out,
    int size
)
{
    int k = get_global_id(0);
    int n = get_global_size(0);

    int result = 0;
    for(int i = 0; i < size; i++)
    {
        result += vec[k] * mat[k * n + i];
    }   

    out[k] = result;
}