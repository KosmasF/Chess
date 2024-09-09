#pragma once
#include <math.h>

float reLU(float input);

float liL(float input);

inline float Sigmoid(float input) {
	return ((1 / (1 + (exp(-input)))) * 2) - 1;
}

float FastSigmoid(float input,int smooth = 1);

float FastSigmoid(float input);

float NonNegativeLinear(float input);

float NonNegativeLimitedLinear(float input);

float None(float input);

float Smoothened(float input);

enum ActivationMethods;

ActivationMethods GetActMethodEnum(float(*ActMethod)(float));