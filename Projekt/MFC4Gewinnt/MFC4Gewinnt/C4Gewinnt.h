/**
 * @class C4Gewinnt
 * @brief Implements the core functionalities of a Connect Four game.
 */


#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <fstream>
#include <filesystem>
#include "spritelib.h"
#include "MFC4GewinntDlg.h"
#include <windows.h>



using namespace std;

class CMFC4GewinntDlg;

class C4Gewinnt
{
public:
    C4Gewinnt(); 
    ~C4Gewinnt();
    void printBoard(CMFC4GewinntDlg* dlg, CClientDC& dc);
    void endGame();
    bool isValidMove(int column); 
    void makeMove(int column, int player); 
    void undoMove();
    void switchPlayer(CMFC4GewinntDlg* dlg, int currentPlayer, CClientDC& dc);
    void displayLastMove(CMFC4GewinntDlg* dlg);
    bool winningMove(int player); 
    bool fileExists(const string& filename) const;
    void saveGame(const string& filename); 
    void loadGame(const string& filename);
    friend class ui;
    bool isAI = false;

private:
    static const unsigned int COL = 7; 
    static const unsigned int ROW = 6; 
    const int xy = 110;
    const int posX = 420;
    const int posY = 658;
    const int WINNING_STREAK = 4;
    stack<pair<int, int>> moveHistory; 
    vector<vector<int>> board; 
    unsigned int turns;
    bool init = true;
};

