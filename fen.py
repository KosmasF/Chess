import chess

board = chess.Board()

FEN = []

nextMove = ""
board.push(board.parse_san(nextMove))
FEN.append(board.fen())