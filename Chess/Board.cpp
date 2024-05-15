#include "Board.h"
#include "Piece.h"
#include "PieceTypes.h"

Board::Board(int size, int xOFFSET, int yOFFSET, bool inversed)
{
	width = size;
	height = size;
	offsetX = xOFFSET;
	offsetY = yOFFSET;
	SquareSize = width / numSquares;

	Inversed = inversed;

	WhiteEnPassant = new EnPassantPawn(WHITE);
	BlackEnPassant = new EnPassantPawn(BLACK);
}

Board::~Board()
{
	delete WhiteEnPassant;
	delete BlackEnPassant;
}

void Board::CheckInput(PiecesArray pieces , void* WhiteDefaultPromotionPiece, void* BlackDefaultPromotionPiece, bool* allowCastling , MovementLog* movementLog)
{
	if (IsMouseButtonPressed(0))
	{
		Vector2 pos = GetMousePosition();

		if (pos.x - offsetX < width && pos.x > offsetX && pos.y - offsetY < height && pos.y > offsetY)
		{
			Vector2 cords = Get2DCords(pos);
			int idx = cords.x + (cords.y * numSquares);


			if (CollectedPiece != -1)
			{
				if (pieces[CollectedPiece]->IsLegal(pieces, CollectedPiece, idx, this , allowCastling))
				{
					std::cout << "MOVEMENT LOG: ";
					char* notation = MovementNotation(pieces, idx, CollectedPiece, allowCastling);
					if (!(pieces[CollectedPiece]->GetName() == "K" && abs(idx - CollectedPiece) == 2))
					{
						std::cout << (notation);
					}


					if (pieces[idx] == WhiteEnPassant && pieces[CollectedPiece]->GetName() == "")
					{
						pieces[idx - 8] = nullptr;
					}
					if (pieces[idx] == BlackEnPassant && pieces[CollectedPiece]->GetName() == "")
					{
						pieces[idx + 8] = nullptr;
					}

					for (int i = 0; i < totalNumSquares; i++)
					{
						if (pieces[i] != nullptr)
						{
							if (pieces[i]->GetName() == "Invalid!" && pieces[CollectedPiece]->IsWhite() && pieces[i]->IsWhite())
								pieces[i] = nullptr;
							else if (pieces[i]->GetName() == "Invalid!" && !pieces[CollectedPiece]->IsWhite() && !pieces[i]->IsWhite())
								pieces[i] = nullptr;
						}
					}

					pieces[idx] = pieces[CollectedPiece];
					pieces[CollectedPiece] = nullptr;

					//Castling
					if (pieces[idx]->GetName() == "K")
					{
						if (idx - CollectedPiece == 2)
						{
							if (pieces[idx]->IsWhite())
							{
								pieces[61] = pieces[63];
								pieces[63] = nullptr;
							}
							else
							{
								pieces[5] = pieces[7];
								pieces[7] = nullptr;
							}
							std::cout << "O-O";
							movementLog->AddMove("O-O");

						}
						if (idx - CollectedPiece == -2)
						{
							if (pieces[idx]->IsWhite())
							{
								pieces[59] = pieces[56];
								pieces[56] = nullptr;
							}
							else
							{
								pieces[3] = pieces[0];
								pieces[0] = nullptr;

							}
							std::cout << "O-O-O";
							movementLog->AddMove("O-O-O");
						}
					}

					//Is king attacked!
					for (int i = 0; i < totalNumSquares; i++)
					{
						if (pieces[i] != nullptr)
							if (pieces[i]->GetName() == "K")
							{
								if (((King*)(pieces[i]))->IsAttacked(pieces, i, this, allowCastling))
								{
									std::cout << "+";
									size_t notationSize = strlen(notation);
									notation[notationSize] = '+';
									notation[notationSize + 1] = 0;
								}
							}
					}

					movementLog->AddMove(notation);

					std::cout << std::endl;

					if (pieces[idx]->GetName() == "")
					{
						if (pieces[idx]->IsWhite() && (int)(idx / 8) == 0)
							pieces[idx] = (Piece*)WhiteDefaultPromotionPiece;
						if (!pieces[idx]->IsWhite() && (int)(idx / 8) == 7)
							pieces[idx] = (Piece*)BlackDefaultPromotionPiece;
						if (abs(CollectedPiece - idx) == 16)
						{
							if (pieces[idx]->IsWhite())
								pieces[idx + 8] = (Piece*)WhiteEnPassant;
							else
								pieces[idx - 8] = (Piece*)BlackEnPassant;
						}
					}

					std::cout << "Piece: " << CollectedPiece << " moved to: " << idx << std::endl;


					//Check for allowCastling!
					if (pieces[idx]->GetName() == "K")
					{
						if (pieces[idx]->IsWhite())
						{
							allowCastling[0] = false;
							allowCastling[1] = false;
						}
						if (!(pieces[idx]->IsWhite()))
						{
							allowCastling[2] = false;
							allowCastling[3] = false;
						}
					}
					if (pieces[idx]->GetName() == "R")
					{
						if (CollectedPiece == 56)
							allowCastling[0] = false;
						if (CollectedPiece == 63)
							allowCastling[1] = false;
						if (CollectedPiece == 0)
							allowCastling[4] = false;
						if (CollectedPiece == 7)
							allowCastling[3] = false;
					}
					std::cout << "Allow castling :  ";
					for (int i = 0; i < 4; i++)
						std::cout << allowCastling[i] << ", ";
					std::cout << std::endl;
					CollectedPiece = -1;
					return;
				}
			}

			if (pieces[idx] != nullptr)
			{
				if (idx != CollectedPiece)
					if(!(pieces[idx]->IsWhite() ^ !Inversed))
						CollectedPiece = idx;
					else
						CollectedPiece = -1;
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
				DrawRectangle(x * SquareSize + offsetX, y * SquareSize + offsetY, SquareSize, SquareSize, lightColor);
			}
			else
			{
				DrawRectangle(x * SquareSize + offsetX, y * SquareSize + offsetY, SquareSize, SquareSize, darkColor);
			}
		}
	}
}

char* Board::MovementNotation(PiecesArray pieces, int Destination, int Location , bool* allowCastling)
{
	char* notation = new char[10];//Max length + null termination
	int buffer = 0;

#ifdef _DEBUG
	//buffer++;
#endif

	const char* name = pieces[Location]->GetName();
	int nameBuffer = strlen(name);

	for (int i = 0; i < nameBuffer; i++)
	{
		notation[buffer] = name[i];
		buffer++;
	}

	Piece piece;

	for (int i = 0; i < totalNumSquares; i++)
	{
		if (pieces[i] != nullptr && i != Location)
			if (pieces[i]->GetName() == pieces[Location]->GetName())
			{
				if (!(pieces[i]->IsWhite() ^ pieces[Location]->IsWhite()))
				if (pieces[i]->IsLegal(pieces, i, Destination, this, allowCastling))
				{
					Position pos = piece.Get2DCords(Location, numSquares);

					notation[buffer] = (char)(pos.x + 97);
					buffer++;
					notation[buffer] = (char)(pos.y + 49);
					buffer++;
				}
			}

	}

	if (pieces[Destination] != nullptr && (!(pieces[Destination]->GetName() == "Invalid!")  || pieces[Location]->GetName() == ""))
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
	int x = (pos.x - offsetX)/ SquareSize;
	int y = (pos.y - offsetY)/ SquareSize;

	if (Inversed)
		y = 7 - y;

	return { (float)x,(float)y };
}
