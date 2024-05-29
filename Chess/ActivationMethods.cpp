#include "ActivationMethods.h"

float reLU(float input)
{
	if (input < 0)
		return 0;
	return input;
}

float liL(float input)
{
	if (input > 1)
		return 1;
	if (input < -1)
		return -1;
	return input;
}

float Sigmoid(float input)
{
	return ((1 / (1 + (exp(-input)))) * 2) - 1;
}

float FastSigmoid(float input,int smooth)
{
	return  input / (smooth + fabs(input));
}

float Linear(float input)
{
	return input;
}

float NonNegativeLinear(float input)
{
	if (input < 0)
		return input * -1;
	return input;
}

float NonNegativeLimitedLinear(float input)
{
	return NonNegativeLinear(liL(input));
}
