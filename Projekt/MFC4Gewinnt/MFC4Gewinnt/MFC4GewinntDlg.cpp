
// MFC4GewinntDlg.cpp: Implementierungsdatei
//

#include "pch.h"
#include "framework.h"
#include "MFC4Gewinnt.h"
#include "MFC4GewinntDlg.h"
#include "afxdialogex.h"
#include "ui.h"
#include <filesystem>
#include <iostream>>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC4GewinntDlg-Dialogfeld



CMFC4GewinntDlg::CMFC4GewinntDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC4GEWINNT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC4GewinntDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC4GewinntDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CMFC4GewinntDlg-Meldungshandler

BOOL CMFC4GewinntDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden


	if (!std::filesystem::exists("spr")) {
		int res = AfxMessageBox(_T("The game is missing the sprites!\nPlease make sure to have the spr folder\nin the same directory as C4Gewinnt.exe"),  MB_ICONQUESTION);
		if (res == IDOK || res == IDCANCEL)
			EndDialog(IDOK);
	}

	/* Backgrounds */
	m_bkg_menu.Load("spr/bkg.bmp", CSize(1600, 900));
	m_bkg_menu.SetZ(0);
	g_bkg.Load("spr/bkg.bmp");
	g_bkg.SetZ(3);

	ui.loadMenuButton(this);
	ui.loadGameButton(this);
	ui.loadGameText(this);
	ui.loadGamePopupB(this);
	ui.loadGamePopupS(this);

	/* Inserts */
	m_list.Insert(&m_bkg_menu);
	m_buff.Load("spr/bkg.bmp");
	m_list.SetWorkspace(&m_buff);
	ui.ToggleMenuButton(this, true);
	
	m_timerId = SetTimer(1, 500, NULL);

	SetWindowPos(NULL, 0, 0,
		m_bkg_menu.DibWidth(), // Fensterbreite
		m_bkg_menu.DibHeight() + 31, // Fensterhöhe
		SWP_NOZORDER | SWP_NOMOVE);
	ui.GameTitel(this, L"MENU");
	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
//  den nachstehenden Code, um das Symbol zu zeichnen.  Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CMFC4GewinntDlg::OnPaint()
{
	CPaintDC dc(this);

	if (IsIconic())
	{
		//CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		
		m_list.RedrawAll(&dc, 0, 0);
		
		CDialogEx::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CMFC4GewinntDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFC4GewinntDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CClientDC dc(this);

	if (ActiveScene && !ui.submenu && !ui.menu) {
		for (int i = 0; i < 7; i++) {
			ui.SpriteActiveIdle(this, point, dc,b_col[i], 1, 0);
		}
		for (int i = 0; i < 3; i++) {
			ui.SpriteActiveIdle(this, point, dc,g_buttonS[i], i + 3, i + 11);
		}
		currentColumn = ui.PosOnBoard(this, dc, point);
	}

	if (!ui.submenu) {
		for (int i = 0; i < 3; i++) {
			ui.SpriteActiveIdle(this, point, dc, pop_bb[i], 6, 7);
		}
		for (int i = 0; i < 3; i++) {
			ui.SpriteActiveIdle(this, point, dc, pop_bb[i + 3], 8, 9);
		}
		for (int i = 0; i < 3; i++) {
			ui.SpriteActiveIdle(this, point, dc, pop_bb[i + 6], 10, 11);
		}
	}
	
	for (int i = 0; i < 2; i++) {
		ui.SpriteActiveIdle(this, point, dc, pop_sb[i], i + 6  , i + 14);
	}
	ui.SpriteActiveIdle(this, point, dc, pop_sb[3], 5, 13);
	ui.SpriteActiveIdle(this, point, dc, pop_sb[2], 3, 11);
	ui.SpriteActiveIdle(this, point, dc, pop_sb[4], 3, 11);


	for(int i = 0; i < 3; i++) {
		ui.SpriteActiveIdle(this, point, dc,m_buttonM[i], i, 3 + i);
	}
	for (int i = 0; i < 3; i++) {
		ui.SpriteActiveIdle(this, point, dc,m_buttonS[i], i, 8 + i);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}



void CMFC4GewinntDlg::OnTimer(UINT_PTR nIDEvent)
{
	CClientDC dc(this);
	if(ui.retai() && !ui.submenu && !ui.gameover)
		ui.MoveAI(this, dc);
}


void CMFC4GewinntDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CClientDC dc(this);

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMFC4GewinntDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CClientDC dc(this);
	ui.clickButton(this, point, dc);
	CDialogEx::OnLButtonDown(nFlags, point);
}
