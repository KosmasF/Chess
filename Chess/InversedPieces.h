#pragma once

class Piece;

/*
class InversedPiecesIterator
{
	using ValueType = typename Piece*;
public:
	InversedPiecesIterator(ValueType* ptr);

	InversedPiecesIterator& operator++();

	InversedPiecesIterator operator++(int);

	InversedPiecesIterator& operator--();

	InversedPiecesIterator operator--(int);

	ValueType& operator[](int index);

	ValueType* operator->();

	ValueType& operator*();

	bool operator==(const InversedPiecesIterator& other);

	bool operator!=(const InversedPiecesIterator& other);
private:
	ValueType* pointer;
};
*/
class PiecesArray
{
	//public:
	//using ValueType = typename Piece*;
	//using Iterator = InversedPiecesIterator;
private:
	Piece** pieces;
	int Size;

public:

	PiecesArray(Piece** InputPieces , int size);

	operator Piece**& ();

	operator void**& ();

	Piece*& operator[](unsigned int index);

	//InversedPiecesIterator begin();
	//InversedPiecesIterator end();
};


