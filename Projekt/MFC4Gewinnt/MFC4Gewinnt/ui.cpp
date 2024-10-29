#include "pch.h"
#include "ui.h"
#include "Windows.h"
#undef min
#undef max
#include <algorithm>

ui::ui()
{
    game = new C4Gewinnt();
}

ui::~ui()
{
    delete game;
}

bool ui::retai() {
    return game->isAI;
}

void ui::SwitchScene(CMFC4GewinntDlg* dlg, CClientDC& dc, bool AP) {
    dlg->ActiveScene = AP;
    if (AP) {
        currentPlayer = game->turns % 2 + 1;
        dlg->m_list.Insert(&dlg->g_bkg);
        ToggleGameButton(dlg, true);
        ToggleMenuButton(dlg, false);
        TogglePeace(dlg, dc, true);
        game->printBoard(dlg, dc);
    }
    else {
        game->endGame();
        dlg->m_list.Remove(&dlg->g_bkg);
        ToggleGameButton(dlg, false);
        TogglePeace(dlg, dc, false);
        ToggleMenuButton(dlg, true);
        GameTitel(dlg, L"MENU");
        evaluate = false;
        game->isAI = false;
    }
    dlg->m_list.Update(&dc, 0, 0);
}

void ui::MakeMove(CMFC4GewinntDlg* dlg, CClientDC& dc, int col) {
    if (!menu && !submenu) {
        currentPlayer = game->turns % 2 + 1;
        if (!game->isAI) {
            if (game->isValidMove(col)) {
                if (game->turns != 0)
                    evalMove(dlg, col);
                dlg->m_list.Remove(&dlg->P_text[0]);
                game->makeMove(col, currentPlayer == 1 ? 1 : 2);
                dlg->m_list.Update(&dc, 0, 0);
                game->printBoard(dlg, dc);
                if (game->winningMove(currentPlayer)) {
                    dlg->ActiveScene = false;
                    ToggleBigGameOver(dlg, true);
                    dlg->P_over[0].SetSpriteNumber(0, currentPlayer == 2 ? 1 : 0);
                    dlg->m_list.Update(&dc, 0, 0);
                }
                if (game->turns == game->ROW * game->COL) {
                    dlg->ActiveScene = false;
                    ToggleBigGameOver(dlg, true);
                    dlg->P_over[0].SetSpriteNumber(0, 2);
                }
                game->switchPlayer(dlg, currentPlayer, dc);
            }
        }
        if (game->isAI) {
            if (currentPlayer == 1) {
                game->makeMove(col, currentPlayer);
                game->printBoard(dlg, dc);
                if (game->turns != 0)
                    evalMove(dlg, col);
                dlg->m_list.Remove(&dlg->P_text[0]);
                if (game->winningMove(currentPlayer)) {
                    dlg->ActiveScene = false;
                    dlg->P_over[0].SetSpriteNumber(0, 4);
                    ToggleBigGameOver(dlg, true);
                    dlg->m_list.Update(&dc, 0, 0);
                }
                if (game->turns == game->ROW * game->COL) {
                    dlg->ActiveScene = false;
                    ToggleBigGameOver(dlg, true);
                    dlg->P_over[0].SetSpriteNumber(0, 2);
                    dlg->m_list.Update(&dc, 0, 0);
                }
                game->switchPlayer(dlg, currentPlayer, dc);
            }

        }
    }
    
}
void ui::MoveAI(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    currentPlayer = game->turns % 2 + 1;
    int col;
        if (currentPlayer == 2) {
            submenu = true;
            col = ai.aiMove(game->board, 1 , currentPlayer, game->turns, game->ROW, game->COL, 1, 2);
            if (game->isValidMove(col)) {
                game->makeMove(col, 2);
                game->printBoard(dlg, dc);
                PlaySound(TEXT("snd/click.wav"), NULL, SND_FILENAME /*| SND_ASYNC*/);
                submenu = false;
                if (game->winningMove(currentPlayer)) {
                    dlg->ActiveScene = false;
                    dlg->P_over[0].SetSpriteNumber(0, 3);
                    Sleep(3000);
                    ToggleBigGameOver(dlg, true);
                    dlg->m_list.Update(&dc, 0, 0);
                }
                if (game->turns == game->ROW * game->COL) {
                    dlg->ActiveScene = false;
                    ToggleBigGameOver(dlg, true);
                    dlg->P_over[0].SetSpriteNumber(0, 2);
                    dlg->m_list.Update(&dc, 0, 0);
                }
                game->switchPlayer(dlg, currentPlayer, dc);
            }
        }
}
void ui::evalMove(CMFC4GewinntDlg* dlg, int col) {
    if (evaluate) {
        int bestMove = ai.aiMove(game->board, 5, currentPlayer, game->turns, game->ROW, game->COL, 1, 2);
        if (bestMove == col) {
            dlg->col_text[0].SetSpriteNumber(0, 6);
        }
        else {
            dlg->col_text[0].SetSpriteNumber(0, 8); // Neutral move
        }
    }
}


int ui::getAIDepth(CMFC4GewinntDlg* dlg) {
    srand(time(NULL));
    return (rand() % 6) + 1;
}

void ui::handleUndo(CMFC4GewinntDlg* dlg, CClientDC& dc, int& currentPlayer) {
    if (game->turns > 0) {
        if(game->isAI)
            game->undoMove();
        game->undoMove();
        game->printBoard(dlg, dc);
        currentPlayer = game->turns % 2 + 1; // Revert the player switch
        game->switchPlayer(dlg, currentPlayer, dc);
        dlg->col_text[0].SetSpriteNumber(0, 8);
    }     
}

void ui::handleSave(CMFC4GewinntDlg* dlg, CClientDC& dc, CPoint& point) {
    game->saveGame("filename");
}

void ui::handleLoad(C4Gewinnt& game) {

    game.loadGame("filename");
}

void ui::SpriteActiveIdle(CMFC4GewinntDlg* dlg, CPoint& point, CClientDC& dc, CSprite& button, int idle, int active) {
    if (button.HitTest(point, 1)) {
        /*PlaySound(TEXT("snd/hover.wav"), NULL, SND_FILENAME | SND_ASYNC);*/
        button.SetSpriteNumber(0, active);
    }
    else {
        button.SetSpriteNumber(0, idle);
    }
    dlg->m_list.Update(&dc, 0, 0);
}

void ui::clickButton(CMFC4GewinntDlg* dlg, CPoint& point, CClientDC& dc) {
    CSprite* hit = dlg->m_list.HitTest(point);
    if (hit == nullptr) return;

    if (hit == &dlg->m_buttonM[0]) {
        handleSinglePlayerButton(dlg, dc);
        depth = getAIDepth(dlg);
    }
    else if (hit == &dlg->m_buttonM[1]) {
        handleMultiplayerButton(dlg, dc);
    }
    else if (hit == &dlg->m_buttonM[2]) {
        handleInstructionsButton(dlg, dc);
    }
    else if (hit == &dlg->m_buttonS[0]) {
        handleLoadButton(dlg, dc);
    }
    else if (hit == &dlg->m_buttonS[1]) {
        handleSettingsButton(dlg, dc);
    }
    else if (hit == &dlg->m_buttonS[2]) {
        handleExitButton(dlg);
    }
    else if (hit == &dlg->g_buttonS[0]) {
        handleBackButton(dlg, dc);
    }
    else if (hit == &dlg->pop_sb[4]) {
        handleBackButtonS(dlg, dc);
    }
    else if (hit == &dlg->g_buttonS[1]) {
        handleSaveButton(dlg, dc);
    }
    else if (hit == &dlg->g_buttonS[2]) {
        handleUndoButton(dlg, dc);
    }
    else if (hit >= &dlg->pop_bb[0] && hit <= &dlg->pop_bb[2]) {
        handleLoadSaveButton(dlg, dc, hit - &dlg->pop_bb[0]);
    }
    else if (hit >= &dlg->pop_bb[3] && hit <= &dlg->pop_bb[5]) {
        handleEmptyButton(dlg, dc, hit - &dlg->pop_bb[3]);
    }
    else if (hit >= &dlg->pop_bb[6] && hit <= &dlg->pop_bb[8]) {
        handleOverrideButton(dlg, dc, hit - &dlg->pop_bb[6]);
    }
    else if (hit == &dlg->pop_sb[0]) { 
        handleJaButton(dlg, dc);
    }
    else if (hit == &dlg->pop_sb[1]) {
        handleNeinButton(dlg);
    }
    else if (hit == &dlg->pop_sb[2]) { // back
        handleBackButton(dlg, dc);
    }
    else if (hit == &dlg->pop_sb[3]) { // undo
        handleUndoButton(dlg, dc);
        dlg->ActiveScene = true;
    }
    else if (hit == &dlg->m_titel) {
        if (++ee == 3) {
            evaluate = true;
            ee = 0;
            GameTitel(dlg, L"EASTER EGG UNLOCKED !!! SEE IF AI WOULD PLACED THE SAME");
        }  
    }
    else {
        if (inst) {
            inst = false;
            ToggleInstructionsButton(dlg, false);
            ToggleMenuButton(dlg, true);
        }
        if (!inst) {
            int column = PosOnBoard(dlg, dc, point);
            if (column != -1)
                MakeMove(dlg, dc, column);
        }
    }

}

void ui::handleSinglePlayerButton(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    game->isAI = true;
    SwitchScene(dlg, dc, true);
}

void ui::handleMultiplayerButton(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    SwitchScene(dlg, dc, true);
}

void ui::handleInstructionsButton(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    ToggleMenuButton(dlg, false);
    ToggleInstructionsButton(dlg, true);
}

void ui::handleLoadButton(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    if (!dlg->ActiveScene) {
        ToggleMenuButton(dlg, false);
        ToggleBigLoad(dlg, true);
    }
    dlg->m_list.Update(&dc, 0, 0);
}

void ui::handleSettingsButton(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    ToggleMenuButton(dlg, false);
    dlg->m_list.Update(&dc, 0, 0);
    Sleep(1000);
    ToggleMenuButton(dlg, true);
    dlg->m_list.Update(&dc, 0, 0);
}

void ui::handleExitButton(CMFC4GewinntDlg* dlg) {
    dlg->PostMessage(WM_CLOSE);
}

void ui::handleBackButton(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    if (!menu && !submenu) {
        ToggleSmallBack(dlg, true);
    }
    if (menu && !dlg->ActiveScene && gameover) {
        ToggleBigGameOver(dlg, false);
        SwitchScene(dlg, dc, false);
    }
    if (menu && !dlg->ActiveScene) {
        ToggleSmallBack(dlg, true);
    }
    dlg->m_list.Update(&dc, 0, 0);
}
void ui::handleBackButtonS(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    if (!dlg->ActiveScene) {
        ToggleBigLoad(dlg, false);
        ToggleMenuButton(dlg, true);
    }
    if (dlg->ActiveScene) {
        ToggleSmallSave(dlg, false);
        ToggleBigSave(dlg, false);
    }
    dlg->m_list.Update(&dc, 0, 0);
}

void ui::handleSaveButton(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    if (!menu && !submenu) {
        ToggleBigSave(dlg, true);
    }
    dlg->m_list.Update(&dc, 0, 0);
}

void ui::handleUndoButton(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    if (!menu && !submenu && dlg->ActiveScene) {
        handleUndo(dlg, dc, currentPlayer);
        GameTitel(dlg, L" GAME | UNDO ");
    }
    if (menu && !dlg->ActiveScene && !game->isAI) {
        ToggleBigGameOver(dlg, false);
        handleUndo(dlg, dc, currentPlayer);
        GameTitel(dlg, L" GAME | UNDID WIN ");
    }
    dlg->m_list.Update(&dc, 0, 0);
}
void ui::handleLoadSaveButton(CMFC4GewinntDlg* dlg, CClientDC& dc, int index) {
    string file;
    switch (index)
    {
    case 0: file = "c4gSysSave1"; break;
    case 1: file = "c4gSysSave2"; break;
    case 2: file = "c4gSysSave3"; break;
    default: break;
    }
    if (game->fileExists(file)) {
        game->loadGame(file);
        ToggleBigLoad(dlg, false);
        SwitchScene(dlg, dc, true);
    }
    
}

void ui::handleEmptyButton(CMFC4GewinntDlg* dlg, CClientDC& dc, int index) {
    if (menu && dlg->ActiveScene && !submenu) {
        string file;
        switch (index)
        {
        case 0: file = "c4gSysSave1"; break;
        case 1: file = "c4gSysSave2"; break;
        case 2: file = "c4gSysSave3"; break;
        default: break;
        }
        game->saveGame(file);
        GameTitel(dlg, L"Game saved");
        ToggleBigSave(dlg, false);
    }
}

void ui::handleOverrideButton(CMFC4GewinntDlg* dlg, CClientDC& dc, int index) {
    if (menu && dlg->ActiveScene && !submenu) {
        ToggleSmallSave(dlg, true);
        index = saveindex;
    }
}

void ui::handleJaButton(CMFC4GewinntDlg* dlg, CClientDC& dc) {
    if (!menu) {
        ToggleSmallBack(dlg, false);
        SwitchScene(dlg, dc, false);

    }
    if (menu && submenu) {
        ToggleSmallSave(dlg, false);
        ToggleBigSave(dlg, false);

        string file;
        switch (saveindex)
        {
        case 0: file = "c4gSysSave1"; break;
        case 1: file = "c4gSysSave2"; break;
        case 2: file = "c4gSysSave3"; break;
        default: break;
        }
        if (game->fileExists(file))
            game->saveGame(file);
        GameTitel(dlg, L"Game saved");
    }
}

void ui::handleNeinButton(CMFC4GewinntDlg* dlg) {
    ToggleSmallSave(dlg, false);
    ToggleSmallBack(dlg, false);

}



void ui::loadMenuButton(CMFC4GewinntDlg* dlg)
{
    //Menu Titel
    dlg->m_titel.Load("spr/titel_1600x_900y.bmp", CSize(1600, 900));
    dlg->m_titel.SetZ(1);

    dlg->inst.Load("spr/menu_instructions.bmp", CSize(1600, 900));
    dlg->inst.SetZ(2);
    /* Buttons */
    // Big Menu Buttons
    dlg->m_buttonM[0].Load("spr/menu-bigbutton_818x_118y.bmp", CSize(818, 118));
    dlg->m_buttonM[0].SetZ(1);

    dlg->m_buttonM[0].SetPosition(386, 331); // Single Player button

    dlg->m_buttonM[2] = dlg->m_buttonM[1] = dlg->m_buttonM[0];
    dlg->m_buttonM[1].SetPosition(dlg->m_buttonM[0].GetXPos(), dlg->m_buttonM[0].GetYPos() + 120); dlg->m_buttonM[1].SetSpriteNumber(0, 1); // Multiplayer Button
    dlg->m_buttonM[2].SetPosition(dlg->m_buttonM[1].GetXPos(), dlg->m_buttonM[1].GetYPos() + 120); dlg->m_buttonM[2].SetSpriteNumber(0, 2); // Instructions Button

    // Small Menu Buttons
    dlg->m_buttonS[0].Load("spr/menu-smallbutton_222x_86y.bmp", CSize(222, 86));
    dlg->m_buttonS[0].SetZ(1);

    dlg->m_buttonS[0].SetPosition(dlg->m_buttonM[2].GetXPos(), dlg->m_buttonM[2].GetYPos() + 120); // Load Button
    dlg->m_buttonS[2] = dlg->m_buttonS[1] = dlg->m_buttonS[0];
    dlg->m_buttonS[1].SetPosition(dlg->m_buttonS[0].GetXPos() + 297, dlg->m_buttonS[0].GetYPos()); dlg->m_buttonS[1].SetSpriteNumber(0, 1); // Settings Button
    dlg->m_buttonS[2].SetPosition(dlg->m_buttonS[1].GetXPos() + 297, dlg->m_buttonS[1].GetYPos()); dlg->m_buttonS[2].SetSpriteNumber(0, 2); // Exit Butoon

}

void ui::loadGameButton(CMFC4GewinntDlg* dlg) {
    // Small Menu Buttons
    dlg->g_buttonS[2].Load("spr/menu-smallbutton_222x_86y.bmp", CSize(222, 86));
    dlg->g_buttonS[2].SetZ(4);

    dlg->g_buttonS[2].SetPosition(406, 771); dlg->g_buttonS[2].SetSpriteNumber(0, 5); // Undo Butoon
    dlg->g_buttonS[4]; dlg->g_buttonS[3]; dlg->g_buttonS[0] = dlg->g_buttonS[1] = dlg->g_buttonS[2];
    dlg->g_buttonS[1].SetPosition(dlg->g_buttonS[2].GetXPos() + 278, dlg->g_buttonS[2].GetYPos()); dlg->g_buttonS[1].SetSpriteNumber(0, 4); // Save Button
    dlg->g_buttonS[0].SetPosition(dlg->g_buttonS[1].GetXPos() + 278, dlg->g_buttonS[1].GetYPos()); dlg->g_buttonS[0].SetSpriteNumber(0, 3);// Back Button


    dlg->peace[0].Load("spr/peace_100x_100y.bmp", CSize(100, 100));
    dlg->peace[0].SetZ(5);
    dlg->peace[0].SetPosition(420, 658); // Base peace!

    dlg->board[0].Load("spr/game_board_780x_660y.bmp", CSize(780, 660)); // Board overlay
    dlg->board[0].SetZ(5);
    dlg->board[0].SetPosition(bx - 10, by - 10);

    dlg->b_col[0].Load("spr/COL_100x_660y1.bmp", CSize(100, 660)); // Hover Overlay
    dlg->b_col[0].SetZ(4);
    dlg->b_col[0].SetAlpha(0.6);
    dlg->b_col[0].SetPosition(bx, by - 10);
    for (int i = 1; i < 7; i++) {
        dlg->b_col[i] = dlg->b_col[0];
        dlg->b_col[i].SetPosition(bx + (i * 110), by - 10);
    }

}
void ui::loadGameText(CMFC4GewinntDlg* dlg) {
    dlg->P_text[0].Load("spr/text_250x_50y.bmp", CSize(250, 50));
    dlg->P_text[0].SetZ(7);

    dlg->P_text[0].SetPosition(675, 415);

    //Column - No use yet
    dlg->col_text[0].Load("spr/text_250x_50y.bmp", CSize(250, 50));
    dlg->col_text[0].SetZ(7);
    dlg->col_text[0].SetPosition(410, 45);
    dlg->col_text[0].SetSpriteNumber(0, 8);

    
    // Win Player
    dlg->P_over[0].Load("spr/game_over_1000x_250y.bmp", CSize(1000, 250));
    dlg->P_over[0].SetZ(9);
    dlg->P_over[0].SetPosition(300, 300);


}
void ui::loadGamePopupS(CMFC4GewinntDlg* dlg) {
    dlg->pop_s[0].Load("spr/game_popup_456x_228y.bmp", CSize(456, 228));
    dlg->pop_s[0].SetZ(10);

    dlg->pop_s[0].SetPosition(572, 336);
    dlg->pop_s[1] = dlg->pop_s[0];
    dlg->pop_s[1].SetSpriteNumber(0, 1);

    dlg->pop_sb[0].Load("spr/menu-smallbutton_222x_86y.bmp", CSize(222, 86));
    dlg->pop_sb[0].SetZ(11);

    dlg->pop_sb[0].SetPosition(573, 452); dlg->pop_sb[0].SetSpriteNumber(0, 6); //Ja
    dlg->pop_sb[3] = dlg->pop_sb[2] = dlg->pop_sb[1] = dlg->pop_sb[0];
    dlg->pop_sb[1].SetPosition(dlg->pop_sb[0].GetXPos() + 219, dlg->pop_sb[0].GetYPos()); dlg->pop_sb[1].SetSpriteNumber(0, 7); //nein
    dlg->pop_sb[2].SetPosition(982, 586); dlg->pop_sb[2].SetSpriteNumber(0, 3); //back
    dlg->pop_sb[3].SetPosition(386, 586); dlg->pop_sb[3].SetSpriteNumber(0, 5); //undo
}

void ui::loadGamePopupB(CMFC4GewinntDlg* dlg) {
    dlg->pop_b[0].Load("spr/game_popup_1000x_500y.bmp", CSize(1000, 500));
    dlg->pop_b[0].SetZ(8); dlg->pop_b[0].SetAlpha(0.9);
    dlg->pop_b[0].SetPosition(300, 200);

    dlg->pop_bb[0].Load("spr/menu-bigbutton_818x_118y.bmp", CSize(818, 118));
    dlg->pop_bb[0].SetZ(9);

    dlg->pop_bb[0].SetPosition(386, 223); dlg->pop_bb[0].SetSpriteNumber(0, 6); // Load Save
    for (int i = 1; i <= 8; i++) {
        dlg->pop_bb[i] = dlg->pop_bb[0];
    }
    for (int i = 0; i < 3; i++) {
        dlg->pop_bb[i + 0].SetPosition(dlg->pop_bb[0].GetXPos(), dlg->pop_bb[0].GetYPos() + i * 128); // Load Save
        dlg->pop_bb[i + 3].SetPosition(dlg->pop_bb[0].GetXPos(), dlg->pop_bb[0].GetYPos() + i * 128); // Empty
        dlg->pop_bb[i + 6].SetPosition(dlg->pop_bb[0].GetXPos(), dlg->pop_bb[0].GetYPos() + i * 128); // Override
    }

}

void ui::ToggleSmallSave(CMFC4GewinntDlg* dlg, bool state) {
    if (state) {
        submenu = true;
        dlg->m_list.Insert(&dlg->pop_s[0]);
        dlg->m_list.Insert(&dlg->pop_sb[0]); // Ja 
        dlg->m_list.Insert(&dlg->pop_sb[1]); // Nein
    }
    else {
        submenu = false;
        dlg->m_list.Remove(&dlg->pop_s[0]);
        dlg->m_list.Remove(&dlg->pop_sb[0]); // Ja 
        dlg->m_list.Remove(&dlg->pop_sb[1]); // Nein

    }
}
void ui::ToggleSmallBack(CMFC4GewinntDlg* dlg, bool state) {
    if (state) {
        submenu = true;
        dlg->m_list.Insert(&dlg->pop_s[1]);
        dlg->m_list.Insert(&dlg->pop_sb[0]); // Ja 
        dlg->m_list.Insert(&dlg->pop_sb[1]); // Nein
    }
    else {
        submenu = false;
        dlg->m_list.Remove(&dlg->pop_s[1]);
        dlg->m_list.Remove(&dlg->pop_sb[0]); // Ja 
        dlg->m_list.Remove(&dlg->pop_sb[1]); // Nein

    }
}
void ui::ToggleBigSave(CMFC4GewinntDlg* dlg, bool state) {
    if (state) {
        menu = true;
        dlg->m_list.Insert(&dlg->pop_b[0]); // Background
        if (game->fileExists("c4gSysSave1")) {
            dlg->m_list.Insert(&dlg->pop_bb[6]); // override                   
        }
        else {
            dlg->m_list.Insert(&dlg->pop_bb[3]); //Empty
        }

        if (game->fileExists("c4gSysSave2")) {
            dlg->m_list.Insert(&dlg->pop_bb[7]); // override
        }
        else {
            dlg->m_list.Insert(&dlg->pop_bb[4]); // Empty                        
        }

        if (game->fileExists("c4gSysSave3")) {
            dlg->m_list.Insert(&dlg->pop_bb[8]); // override                       
        }
        else {
            dlg->m_list.Insert(&dlg->pop_bb[5]); // empty
        }
        dlg->pop_sb[4] = dlg->pop_sb[2]; dlg->pop_sb[4].SetPosition(1024, dlg->pop_bb[0].GetYPos() + 364);
        dlg->m_list.Insert(&dlg->pop_sb[4]);
    }
    else {
        menu = false;
        dlg->m_list.Remove(&dlg->pop_b[0]);
        dlg->m_list.Remove(&dlg->pop_sb[4]);
        for (int i = 0; i < 3; i++) {
            dlg->m_list.Remove(&dlg->pop_bb[i + 3]);
            dlg->m_list.Remove(&dlg->pop_bb[i + 6]);
        }
    }
}
void ui::ToggleBigLoad(CMFC4GewinntDlg* dlg, bool state) {
    if (state) {
        menu = true;
        dlg->m_list.Insert(&dlg->pop_b[0]); // Background
        if (game->fileExists("c4gSysSave1")) {
            dlg->m_list.Insert(&dlg->pop_bb[0]); //load
        }
        else {
            dlg->m_list.Insert(&dlg->pop_bb[3]); // empty
        }

        if (game->fileExists("c4gSysSave2")) {
            dlg->m_list.Insert(&dlg->pop_bb[1]); // Load    
        }
        else {
            dlg->m_list.Insert(&dlg->pop_bb[4]); // Empty
        }

        if (game->fileExists("c4gSysSave3")) {
            dlg->m_list.Insert(&dlg->pop_bb[2]); // Load
        }
        else {
            dlg->m_list.Insert(&dlg->pop_bb[5]); // Empty
        }
        dlg->pop_sb[4] = dlg->pop_sb[2]; dlg->pop_sb[4].SetPosition(1024, dlg->pop_bb[0].GetYPos() + 364);
        dlg->m_list.Insert(&dlg->pop_sb[4]);
    }
    else {
        menu = false;
        dlg->m_list.Remove(&dlg->pop_b[0]);
        dlg->m_list.Remove(&dlg->pop_sb[4]);
        for (int i = 0; i < 3; i++) {
            dlg->m_list.Remove(&dlg->pop_bb[i]);
            dlg->m_list.Remove(&dlg->pop_bb[i + 3]);
        }
    }
}
void ui::ToggleBigGameOver(CMFC4GewinntDlg* dlg, bool state) {
    if (state) {
        menu = true;
        gameover = true;
        dlg->m_list.Insert(&dlg->pop_b[0]); // Background
        dlg->m_list.Insert(&dlg->P_over[0]); // Win Text // need to change sprite pos
        dlg->m_list.Insert(&dlg->pop_sb[2]); // back
        if(!game->isAI)
            dlg->m_list.Insert(&dlg->pop_sb[3]); // undo
    }
    else {
        menu = false;
        gameover = false;
        dlg->m_list.Remove(&dlg->pop_b[0]); // Background
        dlg->m_list.Remove(&dlg->P_over[0]); // Win Text // need to change sprite pos
        dlg->m_list.Remove(&dlg->pop_sb[2]); // back
        dlg->m_list.Remove(&dlg->pop_sb[3]); // undo
    }
}

void ui::ToggleMenuButton(CMFC4GewinntDlg* dlg, bool state) {
    //menu button big
    if (state) {
        dlg->m_list.Insert(&dlg->m_titel);
        dlg->m_list.Insert(&dlg->m_buttonM[0]);
        dlg->m_list.Insert(&dlg->m_buttonM[1]);
        dlg->m_list.Insert(&dlg->m_buttonM[2]);
        //menu button small
        dlg->m_list.Insert(&dlg->m_buttonS[0]);
        dlg->m_list.Insert(&dlg->m_buttonS[1]);
        dlg->m_list.Insert(&dlg->m_buttonS[2]);
    }
    else {
        dlg->m_list.Remove(&dlg->m_titel);
        dlg->m_list.Remove(&dlg->m_buttonM[0]);
        dlg->m_list.Remove(&dlg->m_buttonM[1]);
        dlg->m_list.Remove(&dlg->m_buttonM[2]);
        //menu button small
        dlg->m_list.Remove(&dlg->m_buttonS[0]);
        dlg->m_list.Remove(&dlg->m_buttonS[1]);
        dlg->m_list.Remove(&dlg->m_buttonS[2]);
    }

}

void ui::ToggleGameButton(CMFC4GewinntDlg* dlg, bool state) {
    // button small
    if (state) {
        dlg->m_list.Insert(&dlg->g_buttonS[0]);
        if(!game->isAI)
            dlg->m_list.Insert(&dlg->g_buttonS[1]);
        dlg->m_list.Insert(&dlg->g_buttonS[2]);
        dlg->m_list.Insert(&dlg->board[0]);
        if(evaluate)
            dlg->m_list.Insert(&dlg->col_text[0]);
    }
    else {
        dlg->m_list.Remove(&dlg->g_buttonS[0]);
        dlg->m_list.Remove(&dlg->g_buttonS[1]);
        dlg->m_list.Remove(&dlg->g_buttonS[2]);
        dlg->m_list.Remove(&dlg->board[0]);
        dlg->m_list.Remove(&dlg->col_text[0]);
        dlg->col_text[0].SetSpriteNumber(0, 8);
    }

}
void ui::TogglePeace(CMFC4GewinntDlg* dlg, CClientDC& dc, bool state) {
    if (state) {
        game->printBoard(dlg, dc);
        for (int i = 0; i < 7; i++) {
            dlg->m_list.Insert(&dlg->b_col[i]);
        }
    }
    else {
        for (int i = 0; i < 42; i++) {
            dlg->m_list.Remove(&dlg->peace[i]);
        }
        for (int i = 0; i < 7; i++) {
            dlg->m_list.Remove(&dlg->b_col[i]);
        }

    }
}
void ui::ToggleInstructionsButton(CMFC4GewinntDlg* dlg, bool state) {
    if (state) {
        dlg->m_list.Insert(&dlg->inst);
        inst = true;
    }
    else {
        dlg->m_list.Remove(&dlg->inst);
        inst = false;
    }
}

int ui::PosOnBoard(CMFC4GewinntDlg* dlg, CClientDC& dc, CPoint point) {
    CString s;
    const int columnWidth = 100; const int columnGap = 10;
    const int boardHeight = 660; const int numColumns = 7;

    // Calculate the x and y boundaries of the board
    int boardLeft = bx;
    int boardRight = bx + (columnWidth + columnGap) * numColumns - columnGap; // Total width considering gaps
    int boardTop = by;
    int boardBottom = by + boardHeight;

    if (point.x > boardLeft && point.x < boardRight && point.y > boardTop && point.y < boardBottom) {
        // Calculate the column
        int column = (point.x - boardLeft) / (columnWidth + columnGap);

        // Ensure the column index is within range
        if (column >= 0 && column < numColumns) {
            if (!game->isAI);
                s.Format(L"CONNECT FOUR | MULTIPLAYER | Column:%d", column + 1); // Column index starting from 1
            if (game->isAI)
                s.Format(L"CONNECT FOUR | SINGLE PLAYER | Column:%d", column + 1); // Column index starting from 1

            dlg->SetWindowText(s);
        }
        if (dlg->ActiveScene) return column + 1;
        return -1;
    }
    return -1;
}
void ui::GameTitel(CMFC4GewinntDlg* dlg, CString s) {
    s.Format(L"CONNECT FOUR | %s ", s); // Column index starting from 1
    dlg->SetWindowText(s);

}