#include "Graph.h"


Graph::Graph(int maxValues)
{
	InitWindow(1920 / 2, 1080 / 2, "Graph");

	maxNum = maxValues;
	data = new float[maxValues];
	numValues = 0;
}

void Graph::Draw()
{
	BeginDrawing();

	ClearBackground(RAYWHITE);

	float max = maxValue();
	float min = minValue();

	float x = GetScreenWidth() / ((float)numValues - 1);

	for (int i = 0; i < numValues - 1; i++)
	{
		float y1 = map(data[i], min, max, GetScreenHeight(), 0);
		float y2 = map(data[i + 1], min, max, GetScreenHeight(), 0);
		DrawLine(x * i, y1, x * (i+1), y2, RED);
	}

	float averageStart = average(0, numValues / 2);
	float averageEnd = average(numValues / 2, numValues);

	DrawLine(0, map(averageStart, min, max, GetScreenHeight(), 0), GetScreenWidth(), map(averageEnd, min, max, GetScreenHeight(), 0), GREEN);


	EndDrawing();
}

void Graph::Add(float value)
{
	if (numValues == maxNum)
	{
		for(int i = 0; i < maxNum - 1; i++)
		{
			data[i] = data[i + 1];
		}

		data[numValues - 1] = value;
	}
	else {

		data[numValues] = value;
		numValues++;
	}
}

Graph::~Graph()
{
	delete[] data;
	CloseWindow();
}

float Graph::maxValue()
{
	float result = 0;
	for (int i = 0; i < numValues; i++)
		if (data[i] > result)
			result = data[i];
	return result;
}

float Graph::minValue()
{
	float result = ((float)(1e+300 * 1e+300));//INF
	for (int i = 0; i < numValues; i++)
		if (data[i] < result)
			result = data[i];
	return result;
}

float Graph::average(int start, int end)
{
	int num = end - start;
	float result = 0;
	for (int i = start; i < end; i++)
		result += data[i];
	return result / num;
}

float Graph::map(float value, float leftMin, float leftMax, float rightMin, float rightMax)
{
	return rightMin + (value - leftMin) * (rightMax - rightMin) / (leftMax - leftMin);
}
