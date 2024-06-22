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

	float x = GetScreenWidth() / (float)numValues;

	for (int i = 0; i < numValues; i++)
	{
		float y1 = map(data[i], min, max, 0, GetScreenHeight());
		float y2 = map(data[i + 1], min, max, 0, GetScreenHeight());
		DrawLine(x * i, y1, x * (i+1), y2, RED);
	}

	EndDrawing();
}

void Graph::Add(float value)
{
	if (numValues == maxNum)
	{
		for(int i = 0; i < maxNum; i++)
		{
			data[i] = data[i + 1];
		}

		data[numValues] = value;
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
