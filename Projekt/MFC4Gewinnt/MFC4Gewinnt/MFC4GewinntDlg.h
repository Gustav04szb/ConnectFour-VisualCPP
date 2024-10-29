#pragma once
#include "resource.h"
#include "spritelib.h"
#include "ui.h"  
#include "Windows.h"





// CMFC4GewinntDlg-Dialogfeld
class CMFC4GewinntDlg : public CDialogEx
{
	// Konstruktion
public:
	CMFC4GewinntDlg(CWnd* pParent = nullptr);	// Standardkonstruktor

	// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC4GEWINNT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


	// Implementierung
protected:
	HICON m_hIcon;
	CDIB m_buff;
	CSprite m_bkg_menu, g_bkg, m_titel, board[1], inst;
	CSprite m_buttonM[3], m_buttonS[3], g_buttonS[5];
	CSprite peace[42], b_col[7],P_text[3], col_text[4], P_over[1];
	CSprite pop_s[2], pop_b[1], pop_sb[5], pop_bb[9];
	CSpriteList m_list;
	
	friend class ui;
	friend class C4Gewinnt;
	ui ui;
	int currentColumn, m_currentSprite = 0;
	bool ActiveScene = false;
	UINT_PTR m_timerId;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
