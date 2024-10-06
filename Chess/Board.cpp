#include "Board.h"
#include "Piece.h"
#include "PieceTypes.h"

Piece* Board::WhiteEnPassant = new EnPassantPawn(WHITE);
Piece* Board::BlackEnPassant = new EnPassantPawn(BLACK);

Board::Board(int size, int xOFFSET, int yOFFSET, bool inversed)
{
	width = size;
	height = size;
	offsetX = xOFFSET;
	offsetY = yOFFSET;
	SquareSize = width / numSquares;

	Inversed = inversed;
}

Board::~Board()
{
	//Static variables shall be destroyed once and thus, elsewhere.
}

void Board::DrawMove(int From, int To)
{
	Position startPos = ((Piece*)WhiteEnPassant)->Get2DCordsDrawable(From, this);
	startPos.x += SquareSize / 2;
	startPos.y += SquareSize / 2;
	Position endPos = ((Piece*)WhiteEnPassant)->Get2DCordsDrawable(To, this);
	endPos.x += SquareSize / 2;
	endPos.y += SquareSize / 2;

	//DrawLine(startPos.x, startPos.y, endPos.x, endPos.y, ORANGE);

	float angle = atan2((startPos.y - endPos.y) , (endPos.x - startPos.x));

	float size = sqrt(((startPos.x - endPos.x) * (startPos.x - endPos.x)) + ((startPos.y - endPos.y) * (startPos.y - endPos.y)));

	int offset = 20;

	DrawLineEx({ (float)startPos.x, (float)startPos.y }, { 
		(float)((cos(angle) * (size - offset)) + startPos.x) ,
		(float)(-sin(angle) * (size - offset) + startPos.y) 
		}, 10, ORANGE);

	float angleOffset = asin(offset / size);
	angleOffset = abs(angleOffset);

	//int ang = angle * (180 / PI); // From radians to degress

	Vector2 UpperPoint = { 
		((float)cos((double)(angle + angleOffset)) * (size - offset)) + startPos.x,
		((float)-sin(angle + angleOffset) * (size - offset)) + startPos.y
	};

	Vector2 LowerPoint = {
		((float)cos(((double)angle - angleOffset))) * (size - offset)+startPos.x,
		((float)-sin(((double)angle - angleOffset))) * (size - offset)+startPos.y
	};

	DrawTriangle({ (float)endPos.x,(float)endPos.y }, UpperPoint, LowerPoint, ORANGE);

	DrawTriangle({ (float)endPos.x,(float)endPos.y }, LowerPoint, UpperPoint, ORANGE);
}

bool Board::MakeMove(int From, int To, PiecesArray pieces, bool* allowCastling, void* WhiteDefaultPromotionPiece, void* BlackDefaultPromotionPiece, MovementLog* movementLog, bool disableLogging, int* GlobalCollectedPiece, Piece* WhiteEnPassant, Piece* BlackEnPassant)
{
	int idx = To;
	int CollectedPiece = From;

	//printf("%i, %i",idx,CollectedPiece);

	if (pieces[CollectedPiece]->IsLegal(pieces, CollectedPiece, idx, nullptr, allowCastling))
	{
		if (!disableLogging)
			std::cout << "MOVEMENT LOG: ";
		char* notation = MovementNotation(pieces, idx, CollectedPiece, allowCastling);
		if (!(pieces[CollectedPiece]->GetName()[0] == 'K' && abs(idx - CollectedPiece) == 2))
		{
			if (!disableLogging)
				std::cout << (notation);
		}


		if (pieces[idx] != nullptr)
		{
			if (strcmp(pieces[idx]->GetName(), "Invalid!") == 0 && pieces[idx]->IsWhite() && pieces[CollectedPiece]->GetName()[0] == '\0')
			{
				pieces[idx - 8] = nullptr;
			}
			if (strcmp(pieces[idx]->GetName(), "Invalid!") == 0 && !(pieces[idx]->IsWhite()) && pieces[CollectedPiece]->GetName()[0] == '\0')
			{
				pieces[idx + 8] = nullptr;
			}
		}

		for (int i = 0; i < totalNumSquares; i++)
		{
			if (pieces[i] != nullptr)
			{
				if (strcmp(pieces[i]->GetName(), "Invalid!") == 0 && !(pieces[CollectedPiece]->IsWhite() ^ pieces[i]->IsWhite()))
				{
					pieces[i] = nullptr;
				}
			}
		}

		pieces[idx] = pieces[CollectedPiece];
		pieces[CollectedPiece] = nullptr;

		bool castle = false;
		//Castling
		if (pieces[idx]->GetName()[0] == 'K')
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
				if (!disableLogging)
					std::cout << "O-O";
				if (movementLog != nullptr)
				{
					char* notation = (char*)malloc(4);
					notation[0] = 'O';
					notation[1] = '-';
					notation[2] = 'O';
					notation[3] = 0;

					movementLog->AddMove(notation);
				}
				castle = true;

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
				if (!disableLogging)
					std::cout << "O-O-O";
				if (movementLog != nullptr)
				{
					char* notation = (char*)malloc(6);
					notation[0] = 'O';
					notation[1] = '-';
					notation[2] = 'O';
					notation[3] = '-';
					notation[4] = 'O';
					notation[5] = 0;
					movementLog->AddMove(notation);
				}
				castle = true;
			}
		}

		//Is king attacked!
		for (int i = 0; i < totalNumSquares; i++)
		{
			if (pieces[i] != nullptr)
				if (pieces[i]->GetName()[0] == 'K')
				{
					if (((King*)(pieces[i]))->IsAttacked(pieces, i, nullptr, allowCastling))
					{
						if (!disableLogging)
							std::cout << "+";
						size_t notationSize = strlen(notation);
						notation[notationSize] = '+';
						notation[notationSize + 1] = 0;
					}
				}
		}
		
		if (movementLog != nullptr && !castle)
			movementLog->AddMove(notation);
		else if (movementLog == nullptr)
			delete[] notation;

		if (!disableLogging)
			std::cout << std::endl;

		if (pieces[idx]->GetName()[0] == '\0')
		{
			if (pieces[idx]->IsWhite() && (int)(idx / 8) == 0)
				pieces[idx] = (Piece*)WhiteDefaultPromotionPiece;
			if (!pieces[idx]->IsWhite() && (int)(idx / 8) == 7)
				pieces[idx] = (Piece*)BlackDefaultPromotionPiece;
			if (abs(CollectedPiece - idx) == 16)
			{
				if (pieces[idx]->IsWhite())
					pieces[idx + 8] = WhiteEnPassant;
				else
					pieces[idx - 8] = BlackEnPassant;
			}
		}

		if (!disableLogging)
			std::cout << "Piece: " << CollectedPiece << " moved to: " << idx << std::endl;


		//Check for allowCastling!
		if (pieces[idx]->GetName()[0] == 'K')
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
		if (pieces[idx]->GetName()[0] == 'R')
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
		if (!disableLogging)
		{
			std::cout << "Allow castling :  ";
			for (int i = 0; i < 4; i++)
				std::cout << allowCastling[i] << ", ";
			std::cout << std::endl;
		}

		if(GlobalCollectedPiece != nullptr)
			*GlobalCollectedPiece = -1;

		return true;
	}
	return false;
}

Position Board::TranslateMove(const char* move, Piece** pieces, bool whitePlays)
{
	if (!strcmp(move, "O-O-O"))
	{
		for (int i = 0; i < 64; i++)
		{
			if (pieces[i] != nullptr)
			{
				if (pieces[i]->GetName()[0] == 'K' && pieces[i]->IsWhite() == whitePlays)
				{
					return { i , i - 2 };
				}
			}
		}
		return { -1,-1 };
	}

	if (!strcmp(move, "O-O"))
	{
		for (int i = 0; i < 64; i++)
		{
			if (pieces[i] != nullptr)
			{
				if (pieces[i]->GetName()[0] == 'K' && pieces[i]->IsWhite() == whitePlays)
				{
					return { i , i + 2 };
				}
			}
		}
		return { -1,-1 };
	}

	char columnSpecifier = 0;
	char rowSpecifier = 0;
	int buffer = 0;
	char piece;

	if (!(move[0] > 96 && move[0] < 123))//a-z
	{
		piece = move[0];
		buffer++;
	}
	else
	{
		piece = 0;
		if (move[2] == ' ' || move[2] == '+' || move[2] == '#' || move[2] == '\0' || move[2] == '=')
		{
			goto LocationPieceSearch;
		}
		else if (move[2] == 'x')
		{
			columnSpecifier = move[0];
			rowSpecifier = move[1];
			buffer += 3;
			goto LocationPieceSearch;
		}
		else if (move[1] == 'x')
		{
			columnSpecifier = move[0];
			buffer += 2;
			goto LocationPieceSearch;
		}
		else
		{
			return { -1,-1 };
		}
	}

	if (move[1] > 47 && move[1] < 58)//0-9
	{
		rowSpecifier = move[1];
		buffer++;
	}
	else if (move[1] > 96 && move[1] < 123 && move[1] != 'x')//a-z
	{
		if (move[2] > 47 && move[2] < 58)//0-9
		{
			if (!(move[3] != ' ' || move[3] != '+' || move[3] != '#' || move[3] != '\0'))
			{
				columnSpecifier = move[1];
				rowSpecifier = move[2];
				buffer += 2;
			}
		}
		else
		{
			columnSpecifier = move[1];
			buffer++;
		}
	}

	if (move[buffer] == 'x')
	{
		buffer++;
	}



LocationPieceSearch:

	int Destination = TranslateCords(&(move[buffer]));

	bool castling[] = {true,true,true,true};

	for (int i = 0; i < 64; i++)
	{
		if (pieces[i] != nullptr)
		{
			if (pieces[i]->GetName()[0] == piece)
			{
				if (pieces[i]->IsLegal(pieces, i, Destination, nullptr, castling))
				{
					if (columnSpecifier != 0)
						if (Piece::Get2DCords(i, 8).x != columnSpecifier - 97)
							continue;
					if (rowSpecifier != 0)
						if (Piece::Get2DCords(i, 8).y != 7 - (rowSpecifier - '1'))
							continue;
					if (pieces[i]->IsWhite() != whitePlays)
						continue;


					return { i,Destination };
				}
			}
		}
	}


	return { -1,-1 };
}

int Board::TranslateCords(const char* pos)
{
	int x = pos[0] - 97;
	int y = pos[1] - '1';
	y = 7 - y;

	return (y * 8) + x;
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
				if (MakeMove(CollectedPiece, idx, pieces, allowCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, movementLog, false, &CollectedPiece, WhiteEnPassant, BlackEnPassant))
					return;
			}
			if (pieces[idx] != nullptr)
			{
				if (idx != CollectedPiece)
				{
					if (!(pieces[idx]->IsWhite() ^ !Inversed))
					{
						if (strcmp(pieces[idx]->GetName(), "Invalid!") != 0)
						{
							CollectedPiece = idx;
							std::cout << "Collected Piece: " << CollectedPiece << std::endl;
							return;
						}
					}
				}

			}


			CollectedPiece = -1;




		}
		
		



	}
}


void Board::Draw()
{
	for (int x = 0; x < numSquares; x++)
	{
		for (int y = 0; y < numSquares; y++)
		{
			if (!((y + x) % 2) ^ Inversed)
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
				if (pieces[i]->IsLegal(pieces, i, Destination, nullptr, allowCastling))
				{
					Position pos = piece.Get2DCords(Location, numSquares);

					notation[buffer] = (char)(pos.x + 97);
					buffer++;
					notation[buffer] = (char)((7-pos.y) + 49);
					buffer++;
				}
			}

	}

	if (pieces[Destination] != nullptr && (!(strcmp(pieces[Destination]->GetName() , "Invalid!") == 0)  || pieces[Location]->GetName()[0] == '\0'))
	{
		if (typeid(*pieces[Location]).name() == typeid(Pawn).name())
		{
			Position pos = piece.Get2DCords(Location, numSquares);

			notation[buffer] = (char)(pos.x + 97);
			buffer++;
			notation[buffer] = (char)((7-pos.y) + 49);
			buffer++;
		}
		notation[buffer] = 'x';
		buffer++;
	}

	Position pos = piece.Get2DCords(Destination, numSquares);

	notation[buffer] = (char)(pos.x + 97);
	buffer++;	
	notation[buffer] = (char)((7-pos.y) + 49);
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
