#pragma once
#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include "framework.h"
#include "spritelib.h"
#include "C4Gewinnt.h"
#include "ai.h"
#include <time.h> 
#include <windows.h>

class C4Gewinnt;
class CMFC4GewinntDlg;


class ui
{
public:
	ui::ui();
	ui::~ui();
	bool retai();
	bool menu = false, submenu = false, gameover = false, evaluate = false, inst = false;
	void SwitchScene(CMFC4GewinntDlg* dlg, CClientDC& dc, bool AP);
	void MakeMove(CMFC4GewinntDlg* dlg, CClientDC& dc, int col);
	void MoveAI(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void evalMove(CMFC4GewinntDlg* dlg, int col);
	int getAIDepth(CMFC4GewinntDlg* dlg);
	void SpriteActiveIdle(CMFC4GewinntDlg* dlg, CPoint& point, CClientDC& dc, CSprite& button, int idle, int active);
	void clickButton(CMFC4GewinntDlg* dlg, CPoint& point, CClientDC& dc);
	void loadGamePopupS(CMFC4GewinntDlg* dlg);
	void loadGamePopupB(CMFC4GewinntDlg* dlg);
	void ToggleSmallSave(CMFC4GewinntDlg* dlg, bool state);
	void ToggleSmallBack(CMFC4GewinntDlg* dlg, bool state);
	void ToggleBigSave(CMFC4GewinntDlg* dlg, bool state);
	void ToggleBigLoad(CMFC4GewinntDlg* dlg, bool state);
	void ToggleBigGameOver(CMFC4GewinntDlg* dlg, bool state);
	void ToggleMenuButton(CMFC4GewinntDlg* dlg, bool state);
	void ToggleGameButton(CMFC4GewinntDlg* dlg, bool state);
	void TogglePeace(CMFC4GewinntDlg* dlg, CClientDC& dc, bool state);
	void ToggleInstructionsButton(CMFC4GewinntDlg* dlg, bool state);
	void loadMenuButton(CMFC4GewinntDlg* dlg);
	void loadGameButton(CMFC4GewinntDlg* dlg);
	void loadGameText(CMFC4GewinntDlg* dlg);
	int PosOnBoard(CMFC4GewinntDlg* dlg, CClientDC& dc, CPoint point);
	void GameTitel(CMFC4GewinntDlg* dlg, CString s);

private:
	ai ai;
	C4Gewinnt* game;
	int bx = 420, by = 108;
	int currentPlayer = 1;
	int saveindex;
	int ee = 0;
	int depth;
	DWORD volume = 0x7FFF7FFF;
	bool sound = false;

	void handleSinglePlayerButton(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleMultiplayerButton(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleInstructionsButton(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleLoadButton(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleSettingsButton(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleExitButton(CMFC4GewinntDlg* dlg);
	void handleBackButton(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleBackButtonS(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleSaveButton(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleUndoButton(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleLoadSaveButton(CMFC4GewinntDlg* dlg, CClientDC& dc, int index);
	void handleEmptyButton(CMFC4GewinntDlg* dlg, CClientDC& dc, int index);
	void handleOverrideButton(CMFC4GewinntDlg* dlg, CClientDC& dc, int index);
	void handleJaButton(CMFC4GewinntDlg* dlg, CClientDC& dc);
	void handleNeinButton(CMFC4GewinntDlg* dlg);
	void handleUndo(CMFC4GewinntDlg* dlg, CClientDC& dc, int& currentPlayer);
	void handleSave(CMFC4GewinntDlg* dlg, CClientDC& dc, CPoint& point);
	void handleLoad(C4Gewinnt& game);
};

