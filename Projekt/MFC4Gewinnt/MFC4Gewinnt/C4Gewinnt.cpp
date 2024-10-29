#include "pch.h"
#include "C4Gewinnt.h"
#undef min
#undef max
#include <algorithm>


/* Initializing the board with 0 and the turn with 0 */
C4Gewinnt::C4Gewinnt() : board(ROW, vector<int>(COL, 0)), turns(0) {
    filesystem::create_directories("saves");
}

/* Destructor */
C4Gewinnt::~C4Gewinnt() {}

/* Print board to the console */
void C4Gewinnt::printBoard(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    int type{}, count = 0;
    for (unsigned int r = 0; r < ROW; r++) {
        for (unsigned int c = 0; c < COL; c++) {
            switch (board[ROW - r - 1][c]) {
            case 0: type = 3; break;
            case 1: type = 0; break;
            case 2: if (isAI) type = 2; if (!isAI) type = 1; break;
            }
            if (count == 0) {
                dlg->peace[0].SetSpriteNumber(0, type);
                if (init) {
                    dlg->m_list.Insert(&dlg->peace[0]);
                }
            }
            else {
                dlg->peace[count] = dlg->peace[0];
                dlg->peace[count].SetPosition(posX + (c * xy), posY - (r * xy));
                dlg->peace[count].SetSpriteNumber(0, type);
                if (init) {
                    dlg->m_list.Insert(&dlg->peace[count]);
                }
            }
            count++;
        }
    }
    init = false;
    dlg->m_list.Update(&dc, 0, 0);
    dlg->m_list.RedrawAll(&dc, 0, 0);
    
}

void C4Gewinnt::endGame() {
    // Reset the board
    for (auto& row : board) {
        fill(row.begin(), row.end(), 0);
    }
    // Reset the turn count
    turns = 0;
    // Clear the move history
    while (!moveHistory.empty()) {
        moveHistory.pop();
    }
    // Reinitialize the init flag if needed
    init = true;
}

/* Returns true if the move is valid (column 1..COL and the column is not full) */
bool C4Gewinnt::isValidMove(int column) {
    return column >= 1 && column <= COL && board[0][column - 1] == 0;
}

/* Goes through the column until it finds an empty field and makes the move */
void C4Gewinnt::makeMove(int column, int player) {
    for (int r = ROW - 1; r >= 0; --r) {
        if (board[r][column - 1] == 0) {
            board[r][column - 1] = player;
            moveHistory.push({ r, column - 1 });
            ++turns; // Increment the turn count
            break;
        }
    }
    if (turns != 0)
        PlaySound(TEXT("snd/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

/* Rewinds the last move */
void C4Gewinnt::undoMove() {
    if (!moveHistory.empty()) {
        auto lastMove = moveHistory.top();
        board[lastMove.first][lastMove.second] = 0;
        moveHistory.pop();
        --turns;
        PlaySound(TEXT("snd/undo.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
}
void C4Gewinnt::switchPlayer(CMFC4GewinntDlg* dlg, int currentPlayer, CClientDC& dc) {
    if (turns != 0) {
        if (currentPlayer != 1) {
            dlg->P_text[0].SetSpriteNumber(0, 0);
            dlg->m_list.Insert(&dlg->P_text[0]);
            dlg->m_list.Update(&dc, 0, 0);
            Sleep(500);
            dlg->m_list.Remove(&dlg->P_text[0]);
        }
        else {
            if (isAI) dlg->P_text[0].SetSpriteNumber(0, 2);
            if (!isAI) dlg->P_text[0].SetSpriteNumber(0, 1);
            dlg->m_list.Insert(&dlg->P_text[0]);
            dlg->m_list.Update(&dc, 0, 0);
            Sleep(500);
            dlg->m_list.Remove(&dlg->P_text[0]);
        }
    }
}
void C4Gewinnt::displayLastMove(CMFC4GewinntDlg* dlg) {
}

/* Checking if the player has won */
bool C4Gewinnt::winningMove(int player) {
    // Horizontal check
    for (unsigned int r = 0; r < ROW; ++r) {
        for (unsigned int c = 0; c < COL - WINNING_STREAK + 1; ++c) {
            if (board[r][c] == player && board[r][c + 1] == player &&
                board[r][c + 2] == player && board[r][c + 3] == player) {
                return true;
            }
        }
    }

    // Vertical check
    for (unsigned int c = 0; c < COL; ++c) {
        for (unsigned int r = 0; r < ROW - WINNING_STREAK + 1; ++r) {
            if (board[r][c] == player && board[r + 1][c] == player &&
                board[r + 2][c] == player && board[r + 3][c] == player) {
                return true;
            }
        }
    }

    // Diagonal (bottom-left to top-right) check
    for (unsigned int r = WINNING_STREAK - 1; r < ROW; ++r) {
        for (unsigned int c = 0; c < COL - WINNING_STREAK + 1; ++c) {
            if (board[r][c] == player && board[r - 1][c + 1] == player &&
                board[r - 2][c + 2] == player && board[r - 3][c + 3] == player) {
                return true;
            }
        }
    }

    // Diagonal (top-left to bottom-right) check
    for (unsigned int r = 0; r < ROW - WINNING_STREAK + 1; ++r) {
        for (unsigned int c = 0; c < COL - WINNING_STREAK + 1; ++c) {
            if (board[r][c] == player && board[r + 1][c + 1] == player &&
                board[r + 2][c + 2] == player && board[r + 3][c + 3] == player) {
                return true;
            }
        }
    }

    return false;
}

/* Saving the game to saves/file.c4g in binary format */
void C4Gewinnt::saveGame(const string& filename) {
    try {
        string fullPath = "saves/" + filename + ".c4g";
        ofstream outFile(fullPath, ios::out | ios::binary);
        if (!outFile) {
            return;
        }
        // Save the board
        for (const auto& row : board) {
            outFile.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(int));
        }
        // Save the turns
        outFile.write(reinterpret_cast<const char*>(&turns), sizeof(turns));
        outFile.close();
    }
    catch (const exception& e) {
    }
}

/* Loading the game from saves/file.c4g in binary format */
void C4Gewinnt::loadGame(const string& filename) {
    string fullPath = "saves/" + filename + ".c4g";
    ifstream inFile(fullPath, ios::in | ios::binary);
    if (!inFile) {
        return;
    }

    // Load the board
    for (auto& row : board) {
        inFile.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(int));
    }

    // Load the turns
    inFile.read(reinterpret_cast<char*>(&turns), sizeof(turns));

    inFile.close();
}
bool C4Gewinnt::fileExists(const string& filename) const {
    string fullPath = "saves/" + filename + ".c4g";
    ifstream infile(fullPath);
    return infile.good();
}