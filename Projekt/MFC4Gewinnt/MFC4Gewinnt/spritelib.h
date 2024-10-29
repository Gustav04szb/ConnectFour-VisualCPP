

/////////////////////////////////////////////////////////////////////////////
//
// SpriteLib: 	declaration file
// classes:		CDIB, CSprite, CSpriteLib
// Last update: 02.03.2020
// author: 		K. Bruns
// Version: 	1.5 (Unicode)
//
/////////////////////////////////////////////////////////////////////////////


#pragma once
#pragma comment(lib,"winmm")
#pragma warning (disable: 4996)
#pragma warning (disable: 28183)

#include <mmsystem.h>

class CDIB : public CObject {
public:
    CDIB();
    CDIB(CDIB&);
    virtual ~CDIB();

    BITMAPINFO* GetBitmapInfoAddress()
        {return m_pBMI;}                         // Pointer to bitmap info
    void* GetBitsAddress()
        {return m_pBits;}                        // Pointer to the bits
    BYTE* GetPixelAddress(int x, int y);
    virtual BOOL Load(CFile* fp);                // Load from file
    virtual BOOL Load(char* pszFileName = NULL); // Load DIB from disk file
#ifndef ZIPSPRITES
    virtual BOOL Load(char* zipFileName, char* pszFileName); // Load DIB from zip file
#endif
    virtual BOOL Load(CDC* pDC, CSize size);     // Load DIB from a Storage DCs Bitmap
    virtual BOOL Load(BYTE* pmem);               // Load DIB from a memory file
    virtual BOOL LoadBlank(CDC* pDC, CSize size, COLORREF color=RGB(0,0,0));
                                                 // Loads a black DIB compatible to pDC
    virtual BOOL Save(CFile* fp);                // Save to file
    virtual BOOL Save(char* pszFileName = NULL); // Save DIB to disk file
    virtual void Draw(CDC* pDC, int x, int y);
    virtual void DrawRect(CDC* pDC, CRect, int x, int y);
    virtual void CopyBits(CDIB* pDIB,            // SpriteAdd to pDIB
              int xd, int yd,
              int w,  int h,
              int xs, int ys, float a,
              BOOL clrTrans = FALSE);
    virtual void SpriteAdd(CDIB* pDIB,           // add pDIB to Sprite
              int xd, int yd,
              int w,  int h,
              int xs, int ys, float a,
              BOOL clrTrans = FALSE);
    virtual int DibWidth()
        {return m_pBMI ? m_pBMI->bmiHeader.biWidth : 0;}
    virtual int DibHeight()
        {return m_pBMI ? m_pBMI->bmiHeader.biHeight : 0;}
    virtual int DibColorDepth()
        {return m_pBMI ? m_pBMI->bmiHeader.biBitCount : 0;}
    virtual void GetRect(CRect* pRect);
    virtual void Copy(CDIB* pDib);          // Copy a DIB from an other one
    virtual void Copy(CDC* pDC,CSize size); // Copy a DIB from a Storage DC's Bitmap into the current one
    virtual CDIB& operator=(CDIB &b);

protected:
    int *m_pCopy;                           // number of copies
    CDC *m_pDC;                             // Pointer to DIB sections CDC

private:
    BITMAPFILEHEADER *m_pBMFH;  // Pointer to BITMAPFILEHEADER
    BITMAPINFO* m_pBMI;         // Pointer to BITMAPINFO struct
    BYTE* m_pBits;              // Pointer to Pixelbits
    HBITMAP m_hBitmap;          // Handle to DIB sections Bitmap
    HGDIOBJ m_hOld;             // Handle to old (initial) Bitmap

    int StorageWidth() {
        return (m_pBMI ? ((m_pBMI->bmiHeader.biWidth*
                 (m_pBMI->bmiHeader.biBitCount/8)+ 3) & ~3):0);}

    static void memcpyT(BYTE* dest, BYTE* source, int n, float a)
    {
        for (int i=0; i<n; i++, dest++, source++)
            *dest += (BYTE) ((*source-*dest)*a);
    }
};


class CSprite : public CDIB {
public:
    CSprite();      // Erzeugung eines Sprites mit der Größe 0,0
    CSprite(CSprite& b);
    ~CSprite();     // Löschen des Sprites

    // Spriteinhalt ändern
    BOOL    Load(CFile* fp, CSize size = CSize(0,0), BOOL transparent = TRUE);
    BOOL    Load(char* pszFileName, CSize size = CSize(0,0), BOOL transparent = TRUE);
#ifndef ZIPSPRITES
    BOOL    Load(char* zipFileName, char* pszFileName, CSize size = CSize(0,0), BOOL transparent = TRUE);
#endif
    BOOL    Load(CDC* pDC, CSize PictSize, CSize SpriteSize, BOOL Transparent = TRUE);
    BOOL    Load(BYTE* pmem, CSize size = CSize(0,0), BOOL transparent = TRUE);
    BOOL    LoadBlank(CDC* pDC, CSize size,COLORREF color=RGB(0,0,0), BOOL transparent=TRUE);
    void    Copy(CSprite* pSprite);     //Benutzt die selben Bilddaten, wie eine anderes Sprite

    //Spriteinhalt speichern, darstellen
    void    Render(CDIB* pDIB, CRect* pClipRect = NULL);

    //Spriteparameter in Bezug auf die Szene ändern
    void    SetZ(int z);    // Z-Position des Sprites in der Szene (0..Hintergrund) setzen
    int     GetZ();         // Z-Position des Sprites in der Szene ermitteln
    void    SetPosition(int x, int y);  // X,Y-Position des Sprites in der Szene setzen (0,0)=linke obere Ecke
    CPoint  GetPos();       // liefert die X,Y Position des Sprites in der Szene
    void    GetRect(CRect* pRect); // liefert die Position und die Ausdehnung des Sprites in der Szene
    void    SetAlpha(float a); // setzt den Alphakanal 0.0 ... 1.0
    float   GetAlpha();     // liefert den aktuellen Wert für die Spritetransparenz

    //Spriteparamerter Höhe,Breite
    int     GetXW();        // liefert die Breite eines Sprites in einem Mehrfachsprite
    int     GetYW();        // liefert die Höhe eines Sprites in einem Mehrfachsprite

    //Spritewahl bei Mehrfachsprites
    void    SetSpriteNumber(int xsn, int ysn);  // setzt aktuelles Sprite in einem Mehrfachsprite
    // (0..Spriteanzahl_in_X_Richtung-1,
    //  0..Spriteanzahl_in_Y_Richtung-1)
    void    GetSpriteNumber(int *xsn, int *ysn);// ermittelt die aktuelle Spriteform
    int     GetXPos();      // liefert Position des aktuellen Sprites in einem
    // Mehrfachsprite in X-Richung (0..Spriteanzahl_in_X_Richtung-1)
    int     GetYPos();      // liefert Position des aktuellen Sprites in einem
    // Mehrfachsprite in Y-Richung (0..Spriteanzahl_in_Y_Richtung-1)

    //Erweiterte - Funktionen
    BOOL    HitTest(CPoint point,BOOL ignoretransparenz=FALSE);
    BOOL    IsPixelTransparent(CPoint);
    BOOL    CrashTest(CSprite* pS1);

    //Dirtyregion-Funktionen
    void    GetDirtyRect(CRect* pRect);
    void    SetDirtyRect(CRect* pRect);
    void    ResetDirtyRect();       //setzt die Dirtyregion auf 0

protected:
    int m_x;        // x Coordinate of top-left corner
    int m_y;        // y Coordinate of top-left corner
    int m_xw;       // widht of a sprite inside the image
    int m_yw;       // height of a sprite inside the image
    int m_xsn;      // actual sprite of the image (x direction)
    int m_ysn;      // actual sprite of the image (y direction)
    int m_z;        // z-order for sprite
    float m_alpha;  // alpha channel 0.0f ... 1.0f
    BOOL m_bTrans;  // Transparency color flag
    CRect m_dirty;  // dirty region (for sprite updates)
};


class CSpriteList : private CObList {
public:
    CSpriteList();
    CSpriteList(CSpriteList& b);
    ~CSpriteList();
    void RemoveAll();
    BOOL Insert(CSprite* pSprite);
    void Reorder(CSprite* pSprite);
    CSprite *Remove(CSprite* pSprite);
    CSprite *GetNext(POSITION &pos)
        {return (CSprite*) CObList::GetNext(pos);}
    CSprite *GetPrev(POSITION &pos)
        {return (CSprite*) CObList::GetPrev(pos);}
    POSITION GetTailPosition() const
        {return CObList::GetTailPosition();}
    POSITION GetHeadPosition() const
        {return CObList::GetHeadPosition();}
    CSprite* HitTest(CPoint point,BOOL ignoretransparenz=FALSE);
    CSprite* CrashTest(CSprite* pS1);
    BOOL IsEmpty()
        {return CObList::IsEmpty();}
    void SetWorkspace(CDIB *w);
    void Render(CRect* pClipRect=NULL);
    void Update(CDC* pDC, int x, int y);
    void RedrawAll(CDC* pDC,int x,int y);
    CSpriteList& operator=(CSpriteList&);
    void fpsCounter(CWnd* w);
    float fpsWriter(bool write=true);

    CDIB*   m_workspace;
    CWnd*   m_fpswnd;
    int     m_fpscount;
    DWORD   m_fpstime;
};
