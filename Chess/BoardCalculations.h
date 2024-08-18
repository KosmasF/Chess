#pragma once
#include "Piece.h"
#include "NeuralNetwork.h"
#include "ActivationMethods.h"


namespace BoardCalculations
{
	float* FindMoveProbabilities(Piece** pieces, bool* allowCastling, float* boardStatus);

	inline int MoveToIndex(int Location, int Destination);

	extern NeuralNetwork evaluator;
}