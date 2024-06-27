#include "BoardCalculations.h"

NeuralNetwork BoardCalculations::evaluator = NeuralNetwork("networks/nnRe-evalInMasterGamesErrorCorrection.nn");

float* BoardCalculations::FindMoveProbabilities(Piece** pieces, bool* allowCastling, float* boardStatus)
{
	float* output = new float[64 * 64];

	for (int Location = 0; Location < 64; Location++)
	{
		for (int Destination = 0; Destination < 64; Destination++)
		{
			if (pieces[Location] != nullptr)
			{
				if (pieces[Location]->IsLegal(pieces, Location, Destination, nullptr, allowCastling))
				{
					output[MoveToIndex(Location, Destination)] = Sigmoid(evaluator.Generate(boardStatus, false)[0]);
				}
			}
			output[MoveToIndex(Location, Destination)] = -1;
		}
	}

	return output;
}

int BoardCalculations::MoveToIndex(int Location, int Destination)
{
	return Location * 64 + Destination;
}
