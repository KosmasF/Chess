#pragma once
#include <math.h>

float reLU(float input);

float liL(float input);

float Sigmoid(float input);

float FastSigmoid(float input,int smooth = 1);

float Linear(float input);

float NonNegativeLinear(float input);

float NonNegativeLimitedLinear(float input);