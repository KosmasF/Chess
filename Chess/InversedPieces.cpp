#include "InversedPieces.h"
#include "Piece.h"

PiecesArray::PiecesArray(Piece** InputPieces, int size)
{
	pieces = InputPieces;
	Size = size;
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
	bool inversed = false;
	if (inversed)
		return pieces[index];
	else
		return pieces[index];
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