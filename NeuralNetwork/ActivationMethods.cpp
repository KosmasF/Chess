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


float FastSigmoid(float input,int smooth)
{
	return  input / (smooth + fabs(input));
}

float FastSigmoid(float input)
{
	return  input / (1 + fabs(input));
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

float None(float input)
{
	return input;
}

enum ActivationMethodsEnum
{
	e_reLU,
	e_liL,
	e_Sigmoid,
	e_FastSigmoid,
	e_NonNegativeLinear,
	e_NonNegativeLimitedLinear,
	e_None,
	e_Smoothened,
	e_invalid
};

ActivationMethods GetActMethodEnum(float(*ActMethod)(float))
{
	if(ActMethod == reLU)
		return e_reLU;
	else if(ActMethod == liL)
		return e_liL;
	else if(ActMethod == Sigmoid)
		return e_Sigmoid;
	else if(ActMethod == FastSigmoid)
		return e_FastSigmoid;
	else if(ActMethod == NonNegativeLinear)
		return e_NonNegativeLinear;
	else if(ActMethod == NonNegativeLimitedLinear)
		return e_NonNegativeLimitedLinear;
	else if(ActMethod == None)
		return e_None;
	else if(ActMethod == Smoothened)
		return e_Smoothened;
	else
		return e_invalid;
}
