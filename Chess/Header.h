#pragma once

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <algorithm>

// Function to generate a random piece
char randomPiece() {
    const std::string pieces = "rnbqkpRNBQKP";
    return pieces[rand() % pieces.size()];
}

// Function to generate a random position on the board ensuring at least one king per side
std::string randomPosition() {
    std::vector<std::string> board(8, "");
    bool whiteKingPlaced = false;
    bool blackKingPlaced = false;

    for (int i = 0; i < 8; ++i) {
        int emptySquares = 0;
        for (int j = 0; j < 8; ++j) {
            if (rand() % 2 == 0 && (i != 7 || j != 7)) { // 50% chance of empty square
                ++emptySquares;
            }
            else {
                if (emptySquares > 0) {
                    board[i] += std::to_string(emptySquares);
                    emptySquares = 0;
                }
                char piece = randomPiece();
                if (!whiteKingPlaced && piece == 'K') {
                    whiteKingPlaced = true;
                }
                else if (!blackKingPlaced && piece == 'k') {
                    blackKingPlaced = true;
                }
                board[i] += piece;
            }
        }
        if (emptySquares > 0) {
            board[i] += std::to_string(emptySquares);
        }
    }

    // Ensure white king is placed
    if (!whiteKingPlaced) {
        int row = rand() % 8;
        int col = rand() % 8;
        while (board[row][col] != '1') {
            row = rand() % 8;
            col = rand() % 8;
        }
        board[row][col] = 'K';
    }

    // Ensure black king is placed
    if (!blackKingPlaced) {
        int row = rand() % 8;
        int col = rand() % 8;
        while (board[row][col] != '1') {
            row = rand() % 8;
            col = rand() % 8;
        }
        board[row][col] = 'k';
    }

    // Construct the FEN position part
    std::string position;
    for (int i = 0; i < 8; ++i) {
        if (i != 0) position += '/';
        position += board[i];
    }
    return position;
}

// Function to generate a random castling availability string
std::string randomCastling() {
    const std::string options = "KQkq";
    std::string castling = "";
    for (char c : options) {
        if (rand() % 2 == 0) {
            castling += c;
        }
    }
    return castling.empty() ? "-" : castling;
}

// Function to generate a random en passant target square
std::string randomEnPassant() {
    const std::string files = "abcdefgh";
    int rank = rand() % 2 == 0 ? 3 : 6;
    return std::string(1, files[rand() % 8]) + std::to_string(rank);
}

// Main function to generate a random valid FEN string
std::string generateRandomFEN() {
    std::string position = randomPosition();
    char activeColor = rand() % 2 == 0 ? 'w' : 'b';
    std::string castling = randomCastling();
    std::string enPassant = randomEnPassant();
    int halfmoveClock = rand() % 100;
    int fullmoveNumber = rand() % 100 + 1;

    return position + " " + activeColor + " " + castling + " " + enPassant + " " + std::to_string(halfmoveClock) + " " + std::to_string(fullmoveNumber);
}
