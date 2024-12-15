#pragma once
#include <math.h>

float reLU(float input);

float liL(float input);

inline float SigmoidSym(float input) {
	return ((1 / (1 + (exp(-input)))) * 2) - 1;
}

inline float Sigmoid(float input) {
	return (1 / (1 + (exp(-input))));
}

float FastSigmoid(float input,int smooth = 1);

float FastSigmoid(float input);

float NonNegativeLinear(float input);

float NonNegativeLimitedLinear(float input);

float None(float input);

float Smoothened(float input);

enum ActivationMethodsEnum
{
	e_reLU,
	e_liL,
	e_Sigmoid_sym,
	e_FastSigmoid,
	e_NonNegativeLinear,
	e_NonNegativeLimitedLinear,
	e_None,
	e_Smoothened,
	e_invalid,
	e_Sigmoid
};

ActivationMethodsEnum GetActMethodEnum(float(*ActMethod)(float));