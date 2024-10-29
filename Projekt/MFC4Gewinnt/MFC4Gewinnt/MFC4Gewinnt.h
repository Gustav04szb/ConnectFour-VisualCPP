
// MFC4Gewinnt.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'pch.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CMFC4GewinntApp:
// Siehe MFC4Gewinnt.cpp für die Implementierung dieser Klasse
//

class CMFC4GewinntApp : public CWinApp
{
public:
	CMFC4GewinntApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CMFC4GewinntApp theApp;
