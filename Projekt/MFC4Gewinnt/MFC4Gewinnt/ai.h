/**
 * @class AI
 * @brief Using the Minimax algorithm with alpha-beta pruning.
 * Theory https://medium.com/analytics-vidhya/artificial-intelligence-at-play-connect-four-minimax-algorithm-explained-3b5fc32e4a4f
 */


#pragma once
#include <vector>
#include <array>
#include <climits>
#include <limits>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

class ai
{
public:
    int aiMove(const vector<vector<int>>& board, int depth, int currentPlayer, int turns, int ROW, int COL, int PLAYER, int COMPUTER);
    array<int, 2> miniMax(const vector<vector<int>>& board, unsigned int depth, int alpha, int beta, unsigned int player, int turns, int ROW, int COL, int PLAYER, int COMPUTER);
    bool winningMove(const vector<vector<int>>& b, unsigned int p, int ROW, int COL);
    int tabScore(const vector<vector<int>>& board, unsigned int player, int ROW, int COL);
    int scoreSet(const vector<unsigned int>& set, unsigned int player, unsigned int PLAYER, unsigned int COMPUTER);
    int heurFunction(unsigned int playerCount, unsigned int opponentCount, unsigned int emptyCount);
    vector<vector<int>> copyBoard(const vector<vector<int>>& board, int ROW, int COL);
private:
    static const unsigned int COL = 7;
    static const unsigned int ROW = 6;
};
