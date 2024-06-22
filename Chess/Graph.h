#pragma once
#include "raylib.h"


class Graph
{
public:
	Graph(int maxValues);

	void Draw();

	void Add(float value);

	~Graph();

private:
	int maxNum;
	int numValues;
	float* data;

	float maxValue();
	float minValue();

	float map(float value, float leftMin, float leftMax, float rightMin, float rightMax)
	{
		return rightMin + (value - leftMin) * (rightMax - rightMin) / (leftMax - leftMin);
	}
};

