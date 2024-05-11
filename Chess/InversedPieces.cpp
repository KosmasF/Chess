#include "InversedPieces.h"

InversedPieces::InversedPieces(Piece** InputPieces, int size)
{
	pieces = InputPieces;
	Size = size;
}

InversedPieces::operator Piece**& ()
{
	return pieces;
}

InversedPieces::operator void**& ()
{
	return ((void**&)pieces);
}

Piece*& InversedPieces::operator[](unsigned int index)
{
	return pieces[index];
}

InversedPiecesIterator InversedPieces::begin()
{
	return InversedPiecesIterator(pieces);
}

InversedPiecesIterator InversedPieces::end()
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
