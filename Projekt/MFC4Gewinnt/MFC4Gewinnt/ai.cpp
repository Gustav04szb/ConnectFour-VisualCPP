#include "pch.h"
#include "ai.h"

#undef min
#undef max

int ai::aiMove(const vector<vector<int>>& board, int depth, int currentPlayer, int turns, int ROW, int COL, int PLAYER, int COMPUTER) {
    int move = miniMax(board, depth, numeric_limits<int>::min(), numeric_limits<int>::max(), COMPUTER, turns, ROW, COL, PLAYER, COMPUTER)[1];
    if (move == -1) {
        // If minimax returns -1, pick the first available column.
        for (unsigned int c = 0; c < static_cast<unsigned int>(COL); ++c) {
            if (board[0][c] == 0) { // Check the top row for an empty space
                move = c + 1;
                break;
            }
        }
    }
    return move;
}

array<int, 2> ai::miniMax(const vector<vector<int>>& board, unsigned int depth, int alpha, int beta, unsigned int player, int turns, int ROW, int COL, int PLAYER, int COMPUTER) {
    if (depth == 0 || depth >= static_cast<unsigned int>((COL * ROW) - turns)) {
        return { tabScore(board, COMPUTER, ROW, COL), -1 };
    }
    if (player == COMPUTER) { // Maximize computer score
        array<int, 2> bestMove = { numeric_limits<int>::min(), -1 };
        if (winningMove(board, PLAYER, ROW, COL)) {
            return bestMove;
        }
        for (unsigned int c = 0; c < static_cast<unsigned int>(COL); ++c) {
            if (board[0][c] == 0) { // Check the top row for an empty space
                vector<vector<int>> newBoard = copyBoard(board, ROW, COL);
                for (int r = ROW - 1; r >= 0; --r) {
                    if (newBoard[r][c] == 0) {
                        newBoard[r][c] = player;
                        break;
                    }
                }
                int score = miniMax(newBoard, depth - 1, alpha, beta, PLAYER, turns + 1, ROW, COL, PLAYER, COMPUTER)[0];
                if (score > bestMove[0]) {
                    bestMove = { score, static_cast<int>(c) + 1 };
                }
                alpha = max(alpha, bestMove[0]);
                if (alpha >= beta) break;
            }
        }
        return bestMove;
    }
    else { // Minimize player score
        array<int, 2> bestMove = { numeric_limits<int>::max(), -1 };
        if (winningMove(board, COMPUTER, ROW, COL)) {
            return bestMove;
        }
        for (unsigned int c = 0; c < static_cast<unsigned int>(COL); ++c) {
            if (board[0][c] == 0) { // Check the top row for an empty space
                vector<vector<int>> newBoard = copyBoard(board, ROW, COL);
                for (int r = ROW - 1; r >= 0; --r) {
                    if (newBoard[r][c] == 0) {
                        newBoard[r][c] = player;
                        break;
                    }
                }
                int score = miniMax(newBoard, depth - 1, alpha, beta, COMPUTER, turns + 1, ROW, COL, PLAYER, COMPUTER)[0];
                if (score < bestMove[0]) {
                    bestMove = { score, static_cast<int>(c) + 1 };
                }
                beta = min(beta, bestMove[0]);
                if (alpha >= beta) break;
            }
        }
        return bestMove;
    }
}

bool ai::winningMove(const vector<vector<int>>& b, unsigned int p, int ROW, int COL) {
    for (unsigned int r = 0; r < static_cast<unsigned int>(ROW); ++r) {
        for (unsigned int c = 0; c < static_cast<unsigned int>(COL - 3); ++c) {
            if (b[r][c] == p && b[r][c + 1] == p && b[r][c + 2] == p && b[r][c + 3] == p) {
                return true;
            }
        }
    }
    for (unsigned int c = 0; c < static_cast<unsigned int>(COL); ++c) {
        for (unsigned int r = 0; r < static_cast<unsigned int>(ROW - 3); ++r) {
            if (b[r][c] == p && b[r + 1][c] == p && b[r + 2][c] == p && b[r + 3][c] == p) {
                return true;
            }
        }
    }
    for (unsigned int r = 3; r < static_cast<unsigned int>(ROW); ++r) {
        for (unsigned int c = 0; c < static_cast<unsigned int>(COL - 3); ++c) {
            if (b[r][c] == p && b[r - 1][c + 1] == p && b[r - 2][c + 2] == p && b[r - 3][c + 3] == p) {
                return true;
            }
        }
    }
    for (unsigned int r = 0; r < static_cast<unsigned int>(ROW - 3); ++r) {
        for (unsigned int c = 0; c < static_cast<unsigned int>(COL - 3); ++c) {
            if (b[r][c] == p && b[r + 1][c + 1] == p && b[r + 2][c + 2] == p && b[r + 3][c + 3] == p) {
                return true;
            }
        }
    }
    return false;
}

int ai::tabScore(const vector<vector<int>>& board, unsigned int player, int ROW, int COL) {
    int score = 0;
    vector<unsigned int> set(4);

    // Evaluate rows
    for (unsigned int r = 0; r < static_cast<unsigned int>(ROW); ++r) {
        for (unsigned int c = 0; c <= static_cast<unsigned int>(COL - 4); ++c) {
            for (int i = 0; i < 4; ++i) {
                set[i] = board[r][c + i];
            }
            score += scoreSet(set, player, 1, 2);
        }
    }

    // Evaluate columns
    for (unsigned int c = 0; c < static_cast<unsigned int>(COL); ++c) {
        for (unsigned int r = 0; r <= static_cast<unsigned int>(ROW - 4); ++r) {
            for (int i = 0; i < 4; ++i) {
                set[i] = board[r + i][c];
            }
            score += scoreSet(set, player, 1, 2);
        }
    }

    // Evaluate diagonals (bottom-left to top-right)
    for (unsigned int r = 0; r <= static_cast<unsigned int>(ROW - 4); ++r) {
        for (unsigned int c = 0; c <= static_cast<unsigned int>(COL - 4); ++c) {
            for (int i = 0; i < 4; ++i) {
                set[i] = board[r + i][c + i];
            }
            score += scoreSet(set, player, 1, 2);
        }
    }

    // Evaluate diagonals (top-left to bottom-right)
    for (unsigned int r = 3; r < static_cast<unsigned int>(ROW); ++r) {
        for (unsigned int c = 0; c <= static_cast<unsigned int>(COL - 4); ++c) {
            for (int i = 0; i < 4; ++i) {
                set[i] = board[r - i][c + i];
            }
            score += scoreSet(set, player, 1, 2);
        }
    }

    return score;
}

int ai::scoreSet(const vector<unsigned int>& set, unsigned int player, unsigned int PLAYER, unsigned int COMPUTER) {
    unsigned int playerCount = 0;
    unsigned int opponentCount = 0;
    unsigned int emptyCount = 0;

    for (unsigned int piece : set) {
        if (piece == player) {
            ++playerCount;
        }
        else if (piece == PLAYER || piece == COMPUTER) {
            ++opponentCount;
        }
        else if (piece == 0) {
            ++emptyCount;
        }
    }

    opponentCount -= playerCount;

    return heurFunction(playerCount, opponentCount, emptyCount);
}

int ai::heurFunction(unsigned int playerCount, unsigned int opponentCount, unsigned int emptyCount) {
    if (playerCount == 4) return 500001;
    if (playerCount == 3 && emptyCount == 1) return 5000;
    if (playerCount == 2 && emptyCount == 2) return 500;
    if (opponentCount == 2 && emptyCount == 2) return -501;
    if (opponentCount == 3 && emptyCount == 1) return -5001;
    if (opponentCount == 4) return -500000;
    return 0;
}

vector<vector<int>> ai::copyBoard(const vector<vector<int>>& board, int ROW, int COL) {
    vector<vector<int>> newBoard(ROW, vector<int>(COL));
    for (unsigned int r = 0; r < static_cast<unsigned int>(ROW); ++r) {
        copy(board[r].begin(), board[r].end(), newBoard[r].begin());
    }
    return newBoard;
}