#include "BoardCalculations.h"
#include "GPU.h"

extern GPU gpu;

inline NeuralNetwork BoardCalculations::evaluator = NeuralNetwork("../networks/testedNonRandom3LayersBIG.nn", &gpu);

float* BoardCalculations::FindMoveProbabilities(Piece** pieces, bool* allowCastling, float* boardStatus)
{
	float* output = (float*)malloc(64*64*sizeof(float));

	for (int Location = 0; Location < 64; Location++)
	{
		for (int Destination = 0; Destination < 64; Destination++)
		{
			if (pieces[Location] != nullptr)
			{
				if (pieces[Location]->IsLegal(pieces, Location, Destination, nullptr, allowCastling))
				{
					float* eval = evaluator.Generate(boardStatus, false);
					output[MoveToIndex(Location, Destination)] = Sigmoid(eval[0]);
					delete[] eval;
					continue;
				}
			}
			output[MoveToIndex(Location, Destination)] = -1.f;
		}
	}

	return output;
}

int BoardCalculations::MoveToIndex(int Location, int Destination)
{
	return Location * 64 + Destination;
}
