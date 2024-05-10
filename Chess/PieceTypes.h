#pragma once
#include "Piece.h"
#include "Sprites.h"

class Pawn : public Piece
{
public:
	const char* name = "";

	const char* GetName() override;

	Color color;

	Pawn(Color inputColor);

	void Draw(int idx, Board* board , Sprites* sprites) override;

	bool IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance = true, bool ignoreColor = false) override;

	bool IsWhite() override;
};

class Bishop : public Piece
{
public:
	const char* name = "B";
	const char* GetName() override;

	Color color;

	Bishop(Color inputColor);

	void Draw(int idx, Board* board, Sprites* sprites) override;

	bool IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance = true, bool ignoreColor = false) override;

	bool IsWhite() override;
};

class Knight : public Piece
{
public:
	const char* name = "N";
	const char* GetName() override;

	Color color;

	Knight(Color inputColor);

	void Draw(int idx, Board* board, Sprites* sprites) override;

	bool IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance = true, bool ignoreColor = false) override;

	bool IsWhite() override;
};

class Rook : public Piece
{
public:
	const char* name = "R";
	const char* GetName() override;

	Color color;

	Rook(Color inputColor);

	void Draw(int idx, Board* board, Sprites* sprites) override;

	bool IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance = true, bool ignoreColor = false) override;

	bool IsWhite() override;
};

class Queen : public Piece
{
public:
	const char* name = "Q";
	const char* GetName() override;

	Bishop* bishop;
	Rook* rook;

	Color color;

	Queen(Color inputColor);

	~Queen();

	void Draw(int idx, Board* board, Sprites* sprites) override;

	bool IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance = true, bool ignoreColor = false) override;

	bool IsWhite() override;
};

class King : public Piece
{
public:
	const char* name = "K";
	const char* GetName() override;

	Color color;

	King(Color inputColor);

	void Draw(int idx, Board* board, Sprites* sprites) override;

	bool IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance = true , bool ignoreColor = false) override;

	bool IsAttacked(Piece** pieces, int ToCheck, Board* board, bool * allowCastling);

	bool IsWhite() override;
};

class EnPassantPawn : public Piece
{
public:
	Color color;
	EnPassantPawn(Color inputColor);

	bool IsWhite() override;
};