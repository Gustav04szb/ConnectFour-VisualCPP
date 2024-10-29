#pragma once
#include "framework.h"
#include "spritelib.h"


class CMFC4GewinntDlg;

class onHover
{
public:
	void menuButton(CPoint& point, CClientDC& dc, CMFC4GewinntDlg* dlg, CSprite& button, int idle, int active);
	CString buttonfunction(CPoint& point, CClientDC& dc, CMFC4GewinntDlg* dlg, CSprite& button);
};

