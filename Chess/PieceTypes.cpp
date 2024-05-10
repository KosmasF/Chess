#include "PieceTypes.h"
#include <iostream>

void Pawn::Draw(int idx, Board* board, Sprites* sprites)
{
	Position pos = Get2DCordsDrawable(idx, board->numSquares , board->SquareSize);

	if (IsWhite())
		DrawTexture(sprites->WhitePawn, pos.x, pos.y, WHITE);
	else
		DrawTexture(sprites->BlackPawn, pos.x, pos.y, WHITE);
}

bool Pawn::IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance, bool ignoreColor )
{
	bool retLegal = oppositePieceIgnorance;
	if (retLegal)
	{
		if (!IsWhite())
		{
			if (ToCheck == Original + 8)
				if (pieces[ToCheck] == nullptr || pieces[ToCheck] == board->WhiteEnPassant)
					return true;

			if (Get2DCords(Original, board->numSquares).y == 1)
				if (ToCheck == Original + 16)
					if (IsLegal(pieces, Original, ToCheck - 8, board, allowCastling))
						if (pieces[ToCheck] == nullptr || pieces[ToCheck] == board->WhiteEnPassant)
							return true;

			if (Get2DCords(ToCheck, board->numSquares).y == Get2DCords(Original, board->numSquares).y + 1)
				if (ToCheck == Original + 9 || ToCheck == Original + 7)
					if (pieces[ToCheck] != nullptr || pieces[ToCheck] == board->WhiteEnPassant)
						if (pieces[ToCheck]->IsWhite())
							return true;
		}
		else
		{
			if (ToCheck == Original - 8)
				if (pieces[ToCheck] == nullptr || pieces[ToCheck] == board->WhiteEnPassant)
					return true;

			if (Get2DCords(Original, board->numSquares).y == 6)
				if (ToCheck == Original - 16)
					if (IsLegal(pieces, Original, ToCheck + 8, board, allowCastling))
						if (pieces[ToCheck] == nullptr || pieces[ToCheck] == board->WhiteEnPassant)
							return true;

			if (Get2DCords(ToCheck, board->numSquares).y == Get2DCords(Original, board->numSquares).y - 1)
				if (ToCheck == Original - 9 || ToCheck == Original - 7)
					if (pieces[ToCheck] != nullptr)
						if (!pieces[ToCheck]->IsWhite())
							return true;
		}
	}
	else
	{
		if (!IsWhite())
		{
			if (Get2DCords(ToCheck, board->numSquares).y == Get2DCords(Original, board->numSquares).y + 1)
				if (ToCheck == Original + 9 || ToCheck == Original + 7)
					return true;
		}
		else
		{
			if (Get2DCords(ToCheck, board->numSquares).y == Get2DCords(Original, board->numSquares).y - 1)
				if (ToCheck == Original - 9 || ToCheck == Original - 7)
					return true;

		}
	}


	return false;
}

bool Pawn::IsWhite()
{
	if (color.r == 255)
		return true;
	return false;
}

Pawn::Pawn(Color inputColor)
{
	color = inputColor;
}

Bishop::Bishop(Color inputColor)
{
	color = inputColor;
}

void Bishop::Draw(int idx, Board* board, Sprites* sprites)
{
	Position pos = Get2DCordsDrawable(idx, board->numSquares, board->SquareSize);

	if (IsWhite())
		DrawTexture(sprites->WhiteBishop, pos.x, pos.y, WHITE);
	else
		DrawTexture(sprites->BlackBishop, pos.x, pos.y, WHITE);
}

bool Bishop::IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance, bool ignoreColor)
{
	if (pieces[ToCheck] == nullptr || pieces[ToCheck] == board->WhiteEnPassant || ((((pieces[ToCheck]->IsWhite() && !IsWhite()) || (!pieces[ToCheck]->IsWhite() && IsWhite())) && oppositePieceIgnorance)) || ignoreColor)
	{
		if ((ToCheck - Original) % 9 == 0 && ToCheck - Original > 0)
			if (Get2DCords(Original, board->numSquares).x < Get2DCords(ToCheck, board->numSquares).x)
				if ((ToCheck - Original) / 9 == 1)
					return true;
				else if (IsLegal(pieces, Original, ToCheck - 9, board,allowCastling ,oppositePieceIgnorance = false))
					return true;

		if ((ToCheck - Original) % 7 == 0 && ToCheck - Original > 0)
			if (Get2DCords(Original, board->numSquares).x > Get2DCords(ToCheck, board->numSquares).x)
				if ((ToCheck - Original) / 7 == 1)
					return true;
				else if (IsLegal(pieces, Original, ToCheck - 7, board,allowCastling , oppositePieceIgnorance = false))
					return true;

		if ((Original - ToCheck) % 9 == 0 && ToCheck - Original < 0)
			if (Get2DCords(Original, board->numSquares).x > Get2DCords(ToCheck, board->numSquares).x)
				if ((Original - ToCheck) / 9 == 1)
					return true;
				else if (IsLegal(pieces, Original, ToCheck + 9, board, allowCastling,oppositePieceIgnorance = false))
					return true;

		if ((Original - ToCheck) % 7 == 0 && ToCheck - Original < 0)
			if (Get2DCords(Original, board->numSquares).x < Get2DCords(ToCheck, board->numSquares).x)
				if ((Original - ToCheck) / 7 == 1)
					return true;
				else if (IsLegal(pieces, Original, ToCheck + 7, board,allowCastling ,oppositePieceIgnorance = false))
					return true;
	}
	

	return false;
}

bool Bishop::IsWhite()
{
	if (color.r == 255)
		return true;
	return false;
}

Knight::Knight(Color inputColor)
{
	color = inputColor;
}

void Knight::Draw(int idx, Board* board, Sprites* sprites)
{
	Position pos = Get2DCordsDrawable(idx, board->numSquares, board->SquareSize);

	if (IsWhite())
		DrawTexture(sprites->WhiteKnight, pos.x, pos.y, WHITE);
	else
		DrawTexture(sprites->BlackKnight, pos.x, pos.y, WHITE);
}

bool Knight::IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance, bool ignoreColor)
{
	if (pieces[ToCheck] == nullptr || pieces[ToCheck] == board->WhiteEnPassant || ((((pieces[ToCheck]->IsWhite() && !IsWhite()) || (!pieces[ToCheck]->IsWhite() && IsWhite())) && oppositePieceIgnorance)) || ignoreColor)
	{
		if (ToCheck == Original + 17)
			if (Get2DCords(ToCheck, board->numSquares).x > Get2DCords(Original, board->numSquares).x)
				return true;

		if (ToCheck == Original + 15)
			if (Get2DCords(ToCheck, board->numSquares).x < Get2DCords(Original, board->numSquares).x)
				return true;

		if (ToCheck == Original - 17)
			if (Get2DCords(ToCheck, board->numSquares).x < Get2DCords(Original, board->numSquares).x)
				return true;

		if (ToCheck == Original - 15)
			if (Get2DCords(ToCheck, board->numSquares).x > Get2DCords(Original, board->numSquares).x)
				return true;

		if (ToCheck == Original + 10)
			if (Get2DCords(ToCheck, board->numSquares).x > Get2DCords(Original, board->numSquares).x)
				return true;

		if (ToCheck == Original + 6)
			if (Get2DCords(ToCheck, board->numSquares).x < Get2DCords(Original, board->numSquares).x)
				return true;

		if (ToCheck == Original - 10)
			if (Get2DCords(ToCheck, board->numSquares).x < Get2DCords(Original, board->numSquares).x)
				return true;

		if (ToCheck == Original - 6)
			if (Get2DCords(ToCheck, board->numSquares).x > Get2DCords(Original, board->numSquares).x)
				return true;

	}

	return false;
}

bool Knight::IsWhite()
{
	if (color.r == 255)
		return true;
	return false;
}

Rook::Rook(Color inputColor)
{
	color = inputColor;
}

void Rook::Draw(int idx, Board* board, Sprites* sprites)
{
	Position pos = Get2DCordsDrawable(idx, board->numSquares, board->SquareSize);

	if (IsWhite())
		DrawTexture(sprites->WhiteRook, pos.x, pos.y, WHITE);
	else
		DrawTexture(sprites->BlackRook, pos.x, pos.y, WHITE);
}

bool Rook::IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance, bool ignoreColor)
{
	if (pieces[ToCheck] == nullptr || pieces[ToCheck] == board->WhiteEnPassant || ((((pieces[ToCheck]->IsWhite() && !IsWhite()) || (!pieces[ToCheck]->IsWhite() && IsWhite())) && oppositePieceIgnorance)) || ignoreColor)
	{
		if ((ToCheck - Original) % 8 == 0 && ToCheck - Original > 0)
			if ((ToCheck - Original) / 8 == 1)
				return true;
			else if (IsLegal(pieces, Original, ToCheck - 8, board,allowCastling, oppositePieceIgnorance = false))
				return true;

		if ((Original - ToCheck) % 8 == 0 && ToCheck - Original < 0)
			if ((Original - ToCheck) / 8 == 1)
				return true;
			else if (IsLegal(pieces, Original, ToCheck + 8, board,allowCastling, oppositePieceIgnorance = false))
				return true;

		if (Get2DCords(Original, board->numSquares).y == Get2DCords(ToCheck, board->numSquares).y)
		{
			if(ToCheck > Original)
				if ((ToCheck - Original) == 1)
					return true;
				else if (IsLegal(pieces, Original, ToCheck - 1, board,allowCastling, oppositePieceIgnorance = false))
					return true;

			if (ToCheck < Original)
				if ((Original - ToCheck) == 1)
					return true;
				else if (IsLegal(pieces, Original, ToCheck + 1, board, allowCastling, oppositePieceIgnorance = false))
					return true;
		}

	}
	return false;
}

bool Rook::IsWhite()
{
	if (color.r == 255)
		return true;
	return false;
}

bool EnPassantPawn::IsWhite()
{
	if (color.r == 255)
		return true;
	return false;
}

Queen::Queen(Color inputColor)
{
	color = inputColor;
	rook = new Rook(color);
	bishop = new Bishop(color);
}

Queen::~Queen()
{
	delete rook;
	delete bishop;
}

void Queen::Draw(int idx, Board* board, Sprites* sprites)
{
	Position pos = Get2DCordsDrawable(idx, board->numSquares, board->SquareSize);

	if (IsWhite())
		DrawTexture(sprites->WhiteQueen, pos.x, pos.y, WHITE);
	else
		DrawTexture(sprites->BlackQueen, pos.x, pos.y, WHITE);
}

bool Queen::IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance, bool ignoreColor)
{
	if (rook->IsLegal(pieces, Original, ToCheck, board, allowCastling) || bishop->IsLegal(pieces, Original, ToCheck, board , allowCastling))
		return true;
	return false;
}

bool Queen::IsWhite()
{
	if (color.r == 255)
		return true;
	return false;
}

King::King(Color inputColor)
{
	color = inputColor;
}

void King::Draw(int idx, Board* board, Sprites* sprites)
{
	Position pos = Get2DCordsDrawable(idx, board->numSquares, board->SquareSize);

	if (IsWhite())
		DrawTexture(sprites->WhiteKing, pos.x, pos.y, WHITE);
	else
		DrawTexture(sprites->BlackKing, pos.x, pos.y, WHITE);
}

bool King::IsLegal(Piece** pieces, int Original, int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance, bool ignoreColor)
{
	if (pieces[ToCheck] == nullptr || pieces[ToCheck] == board->WhiteEnPassant || ((((pieces[ToCheck]->IsWhite() && !IsWhite()) || (!pieces[ToCheck]->IsWhite() && IsWhite())) && oppositePieceIgnorance) || ignoreColor))
	{
		if (ToCheck == Original + 1 && Get2DCords(Original,board->numSquares).x < Get2DCords(ToCheck, board->numSquares).x)
		{
			return !IsAttacked(pieces, ToCheck, board, allowCastling);
		}
		if (ToCheck == Original - 7 && Get2DCords(Original, board->numSquares).x < Get2DCords(ToCheck, board->numSquares).x)
		{
			return !IsAttacked(pieces, ToCheck, board, allowCastling);
		}
		if (ToCheck == Original - 8)
		{
			return !IsAttacked(pieces, ToCheck, board, allowCastling);
		}
		if (ToCheck == Original - 9 && Get2DCords(Original, board->numSquares).x > Get2DCords(ToCheck, board->numSquares).x)
		{
			return !IsAttacked(pieces, ToCheck, board, allowCastling);
		}
		if (ToCheck == Original - 1 && Get2DCords(Original, board->numSquares).x > Get2DCords(ToCheck, board->numSquares).x)
		{
			return !IsAttacked(pieces, ToCheck, board, allowCastling);
		}
		if (ToCheck == Original + 7 && Get2DCords(Original, board->numSquares).x > Get2DCords(ToCheck, board->numSquares).x)
		{
			return !IsAttacked(pieces, ToCheck, board, allowCastling);
		}
		if (ToCheck == Original + 8)
		{
			return !IsAttacked(pieces, ToCheck, board, allowCastling);
		}
		if (ToCheck == Original + 9 && Get2DCords(Original, board->numSquares).x < Get2DCords(ToCheck, board->numSquares).x)
		{
			return !IsAttacked(pieces, ToCheck, board, allowCastling);
		}
	}
	if (IsWhite())
	{
		if (ToCheck == Original-2)
			if (allowCastling[0])
				if (pieces[57] == nullptr && pieces[58] == nullptr && pieces[59] == nullptr)
					if (!IsAttacked(pieces, 58, board, allowCastling) && !IsAttacked(pieces, 59, board, allowCastling))
						return true;
		if (ToCheck == Original + 2)
			if (allowCastling[1])
				if (pieces[61] == nullptr && pieces[62] == nullptr)
					if (!IsAttacked(pieces, 61, board, allowCastling) && !IsAttacked(pieces, 62, board, allowCastling))
						return true;
	}
	else
	{
		if(ToCheck == Original-2)
			if(allowCastling[3])
				if (pieces[1] == nullptr && pieces[2] == nullptr && pieces[3] == nullptr)
					if (!IsAttacked(pieces, 2, board, allowCastling) && !IsAttacked(pieces, 3, board, allowCastling))
						return true;
		if (ToCheck == Original + 2)
			if (allowCastling[4])
				if (pieces[5] == nullptr && pieces[6] == nullptr)
					if (!IsAttacked(pieces, 5, board, allowCastling) && !IsAttacked(pieces, 6, board, allowCastling))
						return true;
	}

	return false;
}

bool King::IsAttacked(Piece** pieces, int ToCheck, Board* board ,bool* allowCastling)
{
	for (int i = 0; i < board->totalNumSquares; i++)
	{
		if (pieces[i] != nullptr)
			if ((((pieces[i]->IsWhite() && !IsWhite()) || (!pieces[i]->IsWhite() && IsWhite()))))
				if (typeid(*pieces[i]).name() == typeid(Pawn).name())
				{ 
					if (pieces[i]->IsLegal(pieces, i, ToCheck, board ,allowCastling, false))
						return true;
				}else
				if (pieces[i]->IsLegal(pieces, i, ToCheck, board ,allowCastling, true , true))
					return true;
	}
	return false;
}

bool King::IsWhite()
{
	if (color.r == 255)
		return true;
	return false;
}

const char* King::GetName()
{
	return name;
}
const char* Pawn::GetName()
{
	return name;
}
const char* Bishop::GetName()
{
	return name;
}
const char* Queen::GetName()
{
	return name;
}
const char* Rook::GetName()
{
	return name;
}
const char* Knight::GetName()
{
	return name;
}

EnPassantPawn::EnPassantPawn(Color inputColor) : color(inputColor)
{
}
