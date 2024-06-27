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

	float average(int start, int end);

	float map(float value, float min, float max, float mapMin, float mapMax);
};

