#include "Board.h"
#include "Piece.h"
#include "PieceTypes.h"

Board::Board(int size)
{
	width = size;
	height = size;
	if (width == height)
	{
		SquareSize = width / numSquares;
	}
}

void Board::CheckInput(void** Ipieces , void* WhiteDefaultPromotionPiece, void* BlackDefaultPromotionPiece)
{
	Piece** pieces = (Piece**)Ipieces;

	if (IsMouseButtonPressed(0))
	{
		Vector2 pos = GetMousePosition();

		if (pos.x < width && pos.y < height)
		{
			Vector2 cords = Get2DCords(pos);
			int idx = cords.x + (cords.y * numSquares);


			if (CollectedPiece != -1)
			{
				if (pieces[CollectedPiece]->IsLegal(pieces, CollectedPiece, idx, this))
				{
					std::cout << "MOVEMENT LOG: ";
					printf(MovementNotation((void**)pieces, idx, CollectedPiece));

					pieces[idx] = pieces[CollectedPiece];
					pieces[CollectedPiece] = nullptr;
					CollectedPiece = -1;

					for (int i = 0; i < totalNumSquares; i++)
					{
						if (pieces[i] != nullptr)
							if (typeid(*pieces[i]).name() == typeid(King).name())
							{
								if (((King*)(pieces[i]))->IsAttacked(pieces, i, this))
									std::cout << "+";
							}
					}

					std::cout << std::endl;

					if (typeid(*pieces[idx]).name() == typeid(Pawn).name())
						if (pieces[idx]->IsWhite() && (int)(idx / 8) == 0)
							pieces[idx] = (Piece*)WhiteDefaultPromotionPiece;
						if (!pieces[idx]->IsWhite() && (int)(idx / 8) == 7)
							pieces[idx] = (Piece*)BlackDefaultPromotionPiece;

					std::cout << "Piece: " << CollectedPiece << " moved to: " << idx << std::endl;

					return;
				}
			}

			if (pieces[idx] != nullptr)
			{
				if (idx != CollectedPiece)
					CollectedPiece = idx;
				else
					CollectedPiece = -1;
			}

			std::cout << "Collected Piece: " << CollectedPiece << std::endl;



		}
		
		



	}
}


void Board::Draw()
{
	for (int x = 0; x < numSquares; x++)
	{
		for (int y = 0; y < numSquares; y++)
		{
			if (!((y + x) % 2))
			{
				DrawRectangle(x * SquareSize, y * SquareSize, SquareSize, SquareSize, lightColor);
			}
			else
			{
				DrawRectangle(x * SquareSize, y * SquareSize, SquareSize, SquareSize, darkColor);
			}
		}
	}
}

char* Board::MovementNotation(void** Ipieces, int Destination, int Location)
{
	Piece** pieces = (Piece**)Ipieces;
	char notation[6];//Max length + null termination
	int buffer = 0;

	const char* name = pieces[Location]->GetName();
	int nameBuffer = strlen(name);

	for (int i = 0; i < nameBuffer; i++)
	{
		notation[buffer] = name[i];
		buffer++;
	}

	Piece piece;

	if (pieces[Destination] != nullptr)
	{
		if (typeid(*pieces[Location]).name() == typeid(Pawn).name())
		{
			Position pos = piece.Get2DCords(Location, numSquares);

			notation[buffer] = (char)(pos.x + 97);
			buffer++;
			notation[buffer] = (char)(pos.y + 49);
			buffer++;
		}
		notation[buffer] = 'x';
		buffer++;
	}

	Position pos = piece.Get2DCords(Destination, numSquares);

	notation[buffer] = (char)(pos.x + 97);
	buffer++;	
	notation[buffer] = (char)(pos.y + 49);
	buffer++;

	notation[buffer] = 0;

	return notation;
}

Vector2 Board::Get2DCords(Vector2 pos)
{
	int x = pos.x / SquareSize;
	int y = pos.y / SquareSize;

	return { (float)x,(float)y };
}
