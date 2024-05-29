#include "InversedPieces.h"
#include "Piece.h"


PiecesArray::PiecesArray(Piece** InputPieces, int size)
{
	pieces = InputPieces;
	Size = size;
}

PiecesArray::~PiecesArray()
{
}

PiecesArray::operator Piece**& ()
{
	return pieces;
}

PiecesArray::operator void**& ()
{
	return ((void**&)pieces);
}

Piece*& PiecesArray::operator[](unsigned int index)
{
	return pieces[index];
}

char* PiecesArray::fen()
{
	char* value = new char[maxFenSize];
	int buffer = 0;

	int emptySpotsPassed = 0;
	for (int i = 0; i < Size; i++)
	{
		if (pieces[i] != nullptr)
		{
			char name = (pieces[i]->GetName())[0];
			name = name + ((!pieces[i]->IsWhite())*32);
			if (name == 0)
			{
				if (emptySpotsPassed > 0)
				{
					value[buffer] = emptySpotsPassed + 48;
					emptySpotsPassed = 0;
					buffer++;
				}
				value[buffer] = 'P';
				buffer++;
			}
			else if (name == 32)
			{
				if (emptySpotsPassed > 0)
				{
					value[buffer] = emptySpotsPassed + 48;
					emptySpotsPassed = 0;
					buffer++;
				}
				value[buffer] = 'p';
				buffer++;
			}
			else if (name == 'I' || name == 'i')
			{
				emptySpotsPassed++;
			}
			else
			{
				if (emptySpotsPassed > 0)
				{
					value[buffer] = emptySpotsPassed + 48;
					emptySpotsPassed = 0;
					buffer++;
				}
				value[buffer] = name;
				buffer++;
			}
		}
		else
		{
			emptySpotsPassed++;
		}


		if ((i + 1) % 8 == 0)
		{
			if (emptySpotsPassed > 0)
			{
				value[buffer] = emptySpotsPassed + 48;
				emptySpotsPassed = 0;
				buffer++;
			}
			if (i != 63)
			{
				value[buffer] = '/';
				buffer++;
			}
		}
	}

	value[buffer] = 0;

	return value;
}





/*
InversedPiecesIterator PiecesArray::begin()
{
	return InversedPiecesIterator(pieces);
}

InversedPiecesIterator PiecesArray::end()
{
	return InversedPiecesIterator(pieces+Size);
}

InversedPiecesIterator::InversedPiecesIterator(ValueType* ptr)
{
	pointer = ptr;
}

InversedPiecesIterator& InversedPiecesIterator::operator++()
{
	pointer++;
	return *this;
}

InversedPiecesIterator InversedPiecesIterator::operator++(int)
{
	InversedPiecesIterator iterator = *this;
	++(*this);
	return iterator;
}

InversedPiecesIterator& InversedPiecesIterator::operator--()
{
	pointer--;
	return *this;
}

InversedPiecesIterator InversedPiecesIterator::operator--(int)
{
	InversedPiecesIterator iterator = *this;
	--(*this);
	return iterator;
}

InversedPiecesIterator::ValueType& InversedPiecesIterator::operator[](int index)
{
	return *(pointer + index);
}

InversedPiecesIterator::ValueType* InversedPiecesIterator::operator->()
{
	return pointer;
}

InversedPiecesIterator::ValueType& InversedPiecesIterator::operator*()
{
	return *pointer;
}

bool InversedPiecesIterator::operator==(const InversedPiecesIterator& other)
{
	return pointer == other.pointer;
}

bool InversedPiecesIterator::operator!=(const InversedPiecesIterator& other)
{
	return !(*this == other);
}
*/