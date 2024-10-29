
/////////////////////////////////////////////////////////////////////////////
//
// SpriteLib: 	implementation file
// classes:		CDIB, CSprite, CSpriteLib
// Last update: 02.03.2020
// author: 		K. Bruns
// Version: 	1.5c (Unicode, VS2019)
//
/////////////////////////////////////////////////////////////////////////////


#include "pch.h"
#include <stdlib.h>

#include "spritelib.h"
#ifndef ZIPSPRITES
  #include "xunzip.h"
#endif


// Create a empty DIB
CDIB::CDIB() {
    m_pBMFH = NULL;
    m_pBMI = NULL;
    m_pBits = NULL;
    m_pCopy = new int;
    *m_pCopy = 0;
    m_hBitmap = 0;
    m_hOld = 0;
    m_pDC = new CDC;
}

CDIB::CDIB(CDIB& b) {
    AfxMessageBox(L"ein Kopier-Konstruktor wird durch die CDIB-Klasse nicht unterstützt");
    exit(1);
}

CDIB::~CDIB() {
    if (*m_pCopy == 0) {      // no copy remains
       free(m_pBMFH);         // free the memory
       free(m_pBMI);
       SelectObject(m_pDC->GetSafeHdc(),m_hOld);
       DeleteObject(m_hBitmap); // free bitmap
       delete m_pDC;
       delete m_pCopy;
    } else
     (*m_pCopy)--;
}

// Load a DIB from an open file.
BOOL CDIB::Load(CFile* pFile) {
    // file assumed to be open
    if (m_pBMFH) {                // DIB is not empty
        if (*m_pCopy > 0) {       // Copies are remaining
            (*m_pCopy)--;         // it's not longer a copy
            m_pCopy = new int;
            m_pDC = new CDC;
            *m_pCopy = 0;
        } else {
            free(m_pBMFH);       // free the memory
            free(m_pBMI);
            SelectObject(m_pDC->GetSafeHdc(),m_hOld);
            DeleteObject(m_hBitmap); // free bitmap
            m_pDC->DeleteDC();
        }
    }

    if ((m_pBMFH = (BITMAPFILEHEADER*) malloc(sizeof(BITMAPFILEHEADER)))==NULL) {
          AfxMessageBox(L"Unable to allocate BITMAPFILEHEADER-Memory");
          return FALSE;
    }
    pFile->SeekToBegin();            // seek to file begin
    if (pFile->Read(m_pBMFH, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER)) {
          AfxMessageBox(L"Read error");
          return FALSE;
    }
    if ((m_pBMFH->bfType != 0x4d42) && (m_pBMFH->bfType != 0x1910)) {
          AfxMessageBox(L"Invalid bitmap file");
          return FALSE;
    }
    if ((m_pBMI = (BITMAPINFO*) malloc(m_pBMFH->bfOffBits - sizeof(BITMAPFILEHEADER)))==NULL) {
          AfxMessageBox(L"Unable to allocate BITMAPINFO-Memory");
          return FALSE;
    }
    if (pFile->Read(m_pBMI, m_pBMFH->bfOffBits - sizeof(BITMAPFILEHEADER)) !=
            m_pBMFH->bfOffBits - sizeof(BITMAPFILEHEADER)) {
          AfxMessageBox(L"Read error");
          return FALSE;
    }

    m_pDC->CreateCompatibleDC(NULL);
    m_hBitmap = ::CreateDIBSection(m_pDC->GetSafeHdc(),
        (LPBITMAPINFO)m_pBMI, DIB_RGB_COLORS, (LPVOID*)&m_pBits, NULL, 0);
    if (m_hBitmap == 0) {
        AfxMessageBox(L"Unable to create DIB-Section");
        return FALSE;
    }
    if (m_pBits == NULL) {
        AfxMessageBox(L"Unable to create DIB-Section");
        return FALSE;
    }

    if ((m_hOld=SelectObject(m_pDC->GetSafeHdc(),m_hBitmap))==NULL) {
        AfxMessageBox(L"Bitmap not selected");
        return FALSE;
    }

    if (pFile->Read(m_pBits, StorageWidth()*DibHeight()) !=
            (unsigned) StorageWidth()*DibHeight()) {
        AfxMessageBox(L"Read error");
        return FALSE;
    }

    return TRUE;
}

#ifndef ZIPSPRITES
// Load DIB from zip file
BOOL CDIB::Load(char* zipFileName, char* pszFileName){
    using namespace XUnZip;
    HZIP hz;
	CString fnzip(zipFileName);
    if (!(hz = OpenZip(fnzip.AllocSysString(),0,ZIP_FILENAME))) {
        AfxMessageBox(L"Unable to open zip-Archiv");
        return FALSE;
    }
    ZIPENTRYW ze; int i;
	CString fn(pszFileName);
    if (FindZipItem(hz, fn.AllocSysString(), TRUE, &i,&ze)!=ZR_OK) {
        AfxMessageBox(L"Unable to find Bitmap in zip-Archiv");
        return FALSE;
    }
    BYTE *ibuf = new BYTE[ze.unc_size];
    UnzipItem(hz, i, ibuf, ze.unc_size,ZIP_MEMORY);
    BOOL r = Load(ibuf);
    delete[] ibuf; CloseZip(hz);
    return r;
}
#endif

// Load DIB from a memory file
BOOL CDIB::Load(BYTE* pmem)
{
    // file assumed to be open
    if (m_pBMFH) {                // DIB is not empty
        if (*m_pCopy > 0) {       // Copies are remaining
            (*m_pCopy)--;         // it's not longer a copy
            m_pCopy = new int;
            m_pDC = new CDC;
            *m_pCopy = 0;
        } else {
            free(m_pBMFH);       // free the memory
            free(m_pBMI);
            SelectObject(m_pDC->GetSafeHdc(),m_hOld);
            DeleteObject(m_hBitmap); // free bitmap
            m_pDC->DeleteDC();
        }
    }

    if ((m_pBMFH = (BITMAPFILEHEADER*) malloc(sizeof(BITMAPFILEHEADER)))==NULL) {
          AfxMessageBox(L"Unable to allocate BITMAPFILEHEADER-Memory");
          return FALSE;
    }
    memcpy(m_pBMFH, pmem, sizeof(BITMAPFILEHEADER));

    if ((m_pBMFH->bfType != 0x4d42) && (m_pBMFH->bfType != 0x1910)) {
          AfxMessageBox(L"Invalid bitmap file");
          return FALSE;
    }
    if ((m_pBMI = (BITMAPINFO*) malloc(m_pBMFH->bfOffBits - sizeof(BITMAPFILEHEADER)))==NULL) {
          AfxMessageBox(L"Unable to allocate BITMAPINFO-Memory");
          return FALSE;
    }
    memcpy(m_pBMI, pmem+sizeof(BITMAPFILEHEADER), m_pBMFH->bfOffBits - sizeof(BITMAPFILEHEADER));

    m_pDC->CreateCompatibleDC(NULL);
    m_hBitmap = ::CreateDIBSection(m_pDC->GetSafeHdc(),
        (LPBITMAPINFO) m_pBMI,DIB_RGB_COLORS, (LPVOID*) &m_pBits, NULL, 0);
    if (m_pBits == NULL) {
        AfxMessageBox(L"Unable to create DIB-Section");
        return FALSE;
    }

    if ((m_hOld=SelectObject(m_pDC->GetSafeHdc(),m_hBitmap))==NULL) {
        AfxMessageBox(L"Bitmap not selected");
        return FALSE;
    }
    memcpy(m_pBits, pmem+m_pBMFH->bfOffBits, StorageWidth()*DibHeight());

    return TRUE;
}

// Load a DIB from a disk file. If no file name is given, show
// an Open File dialog to get one.
BOOL CDIB::Load(LPSTR pszFileName) {
    CString strFile;

    if ((pszFileName == NULL) || (strlen(pszFileName) == 0)) {
         // Show an Open File dialog to get the name.
         CFileDialog dlg(TRUE,   // Open
              NULL,  // No default extension
              NULL,  // No initial file name
              OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
              L"Image files (*.DIB, *.BMP)|*.DIB;*.BMP|All files (*.*)|*.*||");
         if (dlg.DoModal() == IDOK)
              strFile = dlg.GetPathName();
         else
              return FALSE;
    } else {
         // Copy the supplied file path.
         strFile = pszFileName;
    }

    // Try to open the file for read access.
    CFile file;
    if (! file.Open(strFile,
            CFile::modeRead | CFile::shareDenyWrite)) {
        TRACE("Failed to open file");
        return FALSE;
    }

    BOOL bResult = Load(&file);
    file.Close();
    return bResult;
}

// Copy a DIB from an other one
// Bitmap pixels are not dublicated !!!
// you only have two references to one pixelmap !!!
void CDIB::Copy(CDIB* pDib)
{
    // free old Memory allocation
    if (m_pBMFH)        // DIB is not empty
        if (*m_pCopy == 0) {
           free(m_pBMFH);   // DIB must be empty
           free(m_pBMI);
           SelectObject(m_pDC->GetSafeHdc(),m_hOld);
           DeleteObject(m_hBitmap); // free bitmap
           delete m_pDC;
           delete m_pCopy;  // free copy counter
        } else
           (*m_pCopy)--;    // decrease number of copies
    else {
        delete m_pCopy;     // free copy counter
        delete m_pDC;       // free storage DC
    }

    // copy member values
    m_pBits = pDib->m_pBits;
    m_pBMFH = pDib->m_pBMFH;
    m_pBMI = pDib->m_pBMI;
    m_pCopy = pDib->m_pCopy;
    m_hBitmap = pDib->m_hBitmap;
    m_pDC = pDib->m_pDC;
    (*m_pCopy)++;         // notify the new copy
}

CDIB& CDIB::operator=(CDIB &b) {
    if (&b!=this)
        Copy(&b);
    return *this;
}



// Copy a DIB from a Storage DC's Bitmap into the current one
void CDIB::Copy(CDC* pDC, CSize size)
{
    m_pDC->BitBlt(0,0,size.cx,size.cy,pDC,0,0,SRCCOPY);
}

// Load a DIB from a storage DCs Bitmap    !!! unbedingt anpassen !!!
BOOL CDIB::Load(CDC* pDC, CSize size)
{
    if ((m_pBMFH) && (m_pBMI))  // exists already a DIB with resources
        if ((m_pBMI->bmiHeader.biWidth == size.cx) &&
            (m_pBMI->bmiHeader.biHeight == size.cy)) { // and the old DIB has the same size
            Copy(pDC, size);    // just copy the new pixel
            return TRUE;        // and return
        } else {                // we have to free old ressorces
            if (*m_pCopy > 0) { // Copies are remaining
               (*m_pCopy)--;    // it's not longer a copy
               m_pCopy = new int;
               *m_pCopy = 0;
            } else {
               free(m_pBMFH);             // free the memory
               free(m_pBMI);
               SelectObject(m_pDC->GetSafeHdc(),m_hOld);
               DeleteObject(m_hBitmap);   // free bitmap
               m_pDC->DeleteDC();         // delete stored dc
            }
        }

    if ((m_pBMFH = (BITMAPFILEHEADER*) malloc(sizeof(BITMAPFILEHEADER)))==NULL) {
        AfxMessageBox(L"Unable to allocate BITMAPFILEHEADER-Memory");
        return FALSE;
    }

    /* Initialize BITMAPFILEHEADER */
    m_pBMFH->bfType = 0x4d42;
    m_pBMFH->bfReserved1 = m_pBMFH->bfReserved2 = 0;
    m_pBMFH->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    if ((m_pBMI = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER)))==NULL) {
        AfxMessageBox(L"Unable to allocate BITMAPINFO-Memory");
        return FALSE;
    }

    /* Initialize BITMAPINFOHEADER */
    m_pBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_pBMI->bmiHeader.biWidth = size.cx;
    m_pBMI->bmiHeader.biHeight = size.cy;
    m_pBMI->bmiHeader.biPlanes = 1;
    m_pBMI->bmiHeader.biBitCount = 24;       // 3*8  (RGB)
    m_pBMI->bmiHeader.biCompression = BI_RGB;
    m_pBMI->bmiHeader.biSizeImage =
        m_pBMI->bmiHeader.biXPelsPerMeter =
        m_pBMI->bmiHeader.biYPelsPerMeter =
        m_pBMI->bmiHeader.biClrUsed =
        m_pBMI->bmiHeader.biClrImportant = 0;

    m_pBMFH->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
        StorageWidth()*size.cy;

    m_pDC->CreateCompatibleDC(NULL);
    m_hBitmap = ::CreateDIBSection(m_pDC->GetSafeHdc(),
        (LPBITMAPINFO) m_pBMI, DIB_RGB_COLORS, (LPVOID*) &m_pBits, NULL, 0);

    if (m_pBits == NULL) {
        AfxMessageBox(L"Unable to create DIB-Section");
        return FALSE;
    }

    if ((m_hOld=SelectObject(m_pDC->GetSafeHdc(),m_hBitmap))==NULL)
        AfxMessageBox(L"Bitmap not selected");

    Copy(pDC, size);
    return TRUE;
}

// Loads a white DIB compatible to pDC
BOOL CDIB::LoadBlank(CDC* pDC, CSize size,COLORREF color)
{
    CDC memdc;
    CBitmap bm, *pb;
    BOOL exitstatus;

    //create bitmap
    memdc.CreateCompatibleDC(pDC);
    bm.CreateCompatibleBitmap(pDC,size.cx,size.cy);
    pb=memdc.SelectObject(&bm);

    memdc.FillSolidRect(0,0,size.cx,size.cy,color);
    exitstatus=Load(&memdc,size);

    //delete bitmap
    memdc.SelectObject(pb);
    bm.DeleteObject();

    return exitstatus;
}

// Draw the DIB to a given DC.
void CDIB::Draw(CDC* pDC, int x, int y)
{
      pDC->StretchBlt(
            x,                // Destination x
            y,                // Destination y
            DibWidth(),       // Destination width
            DibHeight(),      // Destination height
            m_pDC,
            0,                // Source x
            0,                // Source y
            DibWidth(),       // Source width
            DibHeight(),      // Source height
            SRCCOPY);
}

// Draw a rectangle of the DIB to a given DC.
void CDIB::DrawRect(CDC* pDC, CRect rect, int x, int y)
{
     pDC->StretchBlt(
            x+rect.left,          // Destination x
            y+rect.top,           // Destination y
            rect.Width(),         // Destination width
            rect.Height(),        // Destination height
            m_pDC,
            rect.left,            // Source x
            rect.top,             // Source y
            rect.Width(),         // Source width
            rect.Height(),        // Source height
            SRCCOPY);
}

// Get a pointer to a pixel.
// NOTE: DIB scan lines are DWORD aligned. The scan line
// storage width may be wider than the scan line image width
// so calc the storage width by rounding the image width
// to the next highest DWORD value.
BYTE* CDIB::GetPixelAddress(int x, int y)
{
    int iWidth;
    if ((x >= DibWidth()) || (y >= DibHeight())) {
        TRACE("Attempt to get out of range pixel address");
        return NULL;
    }

    // Calculate the scan line storage width.
    iWidth = StorageWidth();
    if (m_pBMI->bmiHeader.biBitCount == 24)
        return m_pBits + (DibHeight()-y-1) * iWidth +
                x*(m_pBMI->bmiHeader.biBitCount/8);
        return m_pBits + (DibHeight()-y-1) * iWidth + x;
}

// Get the bounding rectangle.
void CDIB::GetRect(CRect* pRect)
{
    pRect->top = 0;
    pRect->left = 0;
    pRect->bottom = DibHeight();
    pRect->right = DibWidth();
}

// Copy a rectangle of the DIB to pdibDest
void CDIB::CopyBits(CDIB* pdibDest,
            int xd, int yd, // destination position
            int w,  int h,  // width and height
            int xs, int ys, // source position
            float a,        // alpha channel
            BOOL clrTrans)  // a transparent color ?
{
    // normalize the parameters so that only pixels inside
    // the DIB are affected
    if (xd<0) {
        w+=xd; xs-=xd; xd=0;
    }
    if (yd<0) {
        h+=yd; ys-=yd; yd=0;
    }
    CRect rd;
    rd.IntersectRect(&CRect(xd,yd,xd+w,yd+h),
                     &CRect(0,0,pdibDest->DibWidth(),pdibDest->DibHeight()));
    xd = rd.left;
    yd = rd.top;
    w  = rd.right-rd.left;
    h  = rd.bottom-rd.top;

    // Test for silly cases.
    if (w == 0 || h == 0) return;
    if ((xs<0)||(ys<0)||(xs+w>DibWidth())||(ys+h>DibHeight())) {
        AfxMessageBox(L"der Bildbereich für die Spriteaddition ist ungültig");
        return;
    }

    // First pixel has the color for transparent pixels
    BYTE *refpixel = GetPixelAddress(0,0);
    BYTE *pixeld, *pixels;
    for(int l=0, i=0, n=0; l<h; l++)          // für alle Zeilen
		if (!clrTrans) {                      // ohne Transparenz
            if ((a>0.01f)&&(a<0.99f))         // mit Alpha-Kanal
                   memcpyT(pdibDest->GetPixelAddress(xd, yd+l),
                           GetPixelAddress(xs, ys+l), w*3, a);
                else if (a>=0.99f)
                   memcpy(pdibDest->GetPixelAddress(xd, yd+l),
                          GetPixelAddress(xs, ys+l),w*3);
		} else {                                // mit Transparenz
            pixeld = pdibDest->GetPixelAddress(xd, yd+l);
            pixels = GetPixelAddress(xs, ys+l);
            for (i=0, n=0; i<w; i++, pixeld+=3, pixels+=3) {
                // für alle Pixel der Zeile
                if ((pixels[0]!=refpixel[0])||
                    (pixels[1]!=refpixel[1])||
                    (pixels[2]!=refpixel[2]))
                        n+=3;
                else if (n) {
                        if ((a>0.01f)&&(a<0.99f))
                            memcpyT(pixeld-n, pixels-n, n, a);
                        else if (a>=0.99f)
                            memcpy(pixeld-n, pixels-n, n);
                        n=0;
                }
            }
            if (n) {
                if ((a>0.01f)&&(a<0.99f))
                    memcpyT(pixeld-n, pixels-n, n, a);
                else if (a>=0.99f)
                    memcpy(pixeld-n, pixels-n, n);
            }
        }
}

// add Sprite pDIB to this
void CDIB::SpriteAdd(CDIB* pDIB,
              int xd, int yd,
              int w,  int h,
              int xs, int ys, float a, BOOL clrTrans) {
    pDIB->CopyBits(this,xd,yd,w,h,xs,ys,a,clrTrans);
}

// Save a DIB to a disk file.
BOOL CDIB::Save(CFile* fp)
{
    TRY {
        // Write BMP image
        fp->Write(m_pBMFH, sizeof(BITMAPFILEHEADER));
        fp->Write(m_pBMI,  m_pBMFH->bfOffBits - sizeof(BITMAPFILEHEADER));
        fp->Write(m_pBits, m_pBMFH->bfSize - m_pBMFH->bfOffBits);
    }
    CATCH (CException, e) {
        AfxMessageBox(L"Write error--possible disk full condition");
        return FALSE;
    }
    END_CATCH
    return TRUE;
}

// Save a DIB to a disk file. If no file name is given, show
// a File Save dialog to get one.
BOOL CDIB::Save(LPSTR pszFileName)
{
    CString strFile;

    if ((pszFileName == NULL) || (strlen(pszFileName) == 0)) {
        // Show a File Save dialog to get the name.
        CFileDialog dlg(FALSE,   // Save
                NULL,    // No default extension
                NULL,    // No initial file name
                OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
                L"Image files (*.DIB, *.BMP)|*.DIB;*.BMP|All files (*.*)|*.*||");
        if (dlg.DoModal() == IDOK)
            strFile = dlg.GetPathName();
        else
            return FALSE;
    } else {
        // Copy the supplied file path.
        strFile = pszFileName;
    }

    // Try to open the file for write access.
    CFile file;
    if (!file.Open(strFile, CFile::modeReadWrite |
                   CFile::modeCreate | CFile::shareExclusive)) {
        AfxMessageBox(L"Failed to open file");
        return FALSE;
    }

    BOOL bResult = Save(&file);
    file.Close();
    if (!bResult)
        AfxMessageBox(L"Failed to save file");
    return bResult;
}



/////////////////////////////////////////////////////////////////////////////
//
// CSprite class
//
/////////////////////////////////////////////////////////////////////////////

CSprite::CSprite()
{
    m_x   = 0;
    m_y   = 0;
    m_xsn = 0;
    m_ysn = 0;
    m_z   = 50;
    m_xw  = m_yw = 0;
    m_alpha  = 1.f;
    m_bTrans = TRUE;
    m_dirty  = CRect(0,0,0,0);
}

CSprite::CSprite(CSprite& b) {
    AfxMessageBox(L"ein Kopier-Konstruktor wird durch die CSprite-Klasse nicht unterstützt");
    exit(1);
}

CSprite::~CSprite()
{
}

// Render a sprite to a DIB.
void CSprite::Render(CDIB* pDIB, CRect* pClipRect)
{
    // Get the sprite rectangle.
    CRect rcDraw;
    GetRect(&rcDraw);

    // If a clip rectangle was supplied, see if the sprite
    // is visible inside the rectangle.
    if (pClipRect) {
        if (!rcDraw.IntersectRect(pClipRect, &rcDraw)) {
            return; // not visible
        }
    }
    // Copy the image of the sprite.
    CopyBits(pDIB,                         // Dest DIB
             rcDraw.left,                  // Dest x
             rcDraw.top,                   // Dest y
             rcDraw.right - rcDraw.left,   // Width
             rcDraw.bottom - rcDraw.top,   // Height
             (rcDraw.left - m_x) + (m_xsn * m_xw),  // Source x
             (rcDraw.top - m_y) + (m_ysn * m_yw),   // Source y
             m_alpha,                      // Alpha
             m_bTrans);                    // Transparent flag
}

// Load a sprite image from a disk file (via filename)
BOOL CSprite::Load(char* pszFileName, CSize SpriteSize, BOOL transparent)
{
    CRect rcOld; GetRect(&rcOld);
    if (!CDIB::Load(pszFileName)) {
        return FALSE;
    }
    if (SpriteSize != CSize(0,0)) {
        m_xw = SpriteSize.cx;  // Size of each Sprite in the DIB
        m_yw = SpriteSize.cy;
    } else {
        m_xw = DibWidth();     // use the whole DIB as one sprite
        m_yw = DibHeight();
    }
    CRect rcNew; GetRect(&rcNew);
    m_dirty.UnionRect(&rcNew,&rcOld); // set dirty-rectangle
    if (m_z > 0)
        m_bTrans=transparent;
    if (DibColorDepth()!=24)
        AfxMessageBox(L"Die Spritebibliothek unterstützt nur Bitmaps mit 24-Bit Farbtiefe");
    return TRUE;
}

#ifndef ZIPSPRITES
// Load a sprite image from a zip file
BOOL CSprite::Load(char* zipFileName, char* pszFileName, CSize size, BOOL transparent){
    CRect rcOld; GetRect(&rcOld);
    if (!CDIB::Load(zipFileName, pszFileName)) {
        return FALSE;
    }
    if (size != CSize(0,0)) {
        m_xw = size.cx;  // Size of each Sprite in the DIB
        m_yw = size.cy;
    } else {
        m_xw = DibWidth();     // use the whole DIB as one sprite
        m_yw = DibHeight();
    }
    CRect rcNew; GetRect(&rcNew);
    m_dirty.UnionRect(&rcNew,&rcOld); // set dirty-rectangle
    if (m_z > 0)
        m_bTrans=transparent;
    if (DibColorDepth()!=24)
        AfxMessageBox(L"Die Spritebibliothek unterstützt nur Bitmaps mit 24-Bit Farbtiefe");
    return TRUE;
}
#endif

// Load a sprite image from a disk file (via filepointer)
BOOL CSprite::Load(CFile* fp, CSize size, BOOL transparent)
{
    CRect rcOld; GetRect(&rcOld);
    if (!CDIB::Load(fp)) {
        return FALSE;
    }
    if (size != CSize(0,0)) {
        m_xw = size.cx;  // Size of each Sprite in the DIB
        m_yw = size.cy;
    } else {
        m_xw = DibWidth();     // use the whole DIB as one sprite
        m_yw = DibHeight();
    }
    CRect rcNew; GetRect(&rcNew);
    m_dirty.UnionRect(&rcNew,&rcOld); // set dirty-rectangle
    if (m_z > 0)
        m_bTrans=transparent;
    if (DibColorDepth()!=24)
        AfxMessageBox(L"Die Spritebibliothek unterstützt nur Bitmaps mit 24-Bit Farbtiefe");
    return TRUE;
}

// Load a sprite image from a memory file
BOOL CSprite::Load(BYTE* pmem, CSize SpriteSize, BOOL Transparent)
{
    CRect rcOld; GetRect(&rcOld);
    if (!CDIB::Load(pmem)) {
        return FALSE;
    }
    if (SpriteSize != CSize(0,0)) {
        m_xw = SpriteSize.cx;  // Size of each Sprite in the DIB
        m_yw = SpriteSize.cy;
    } else {
        m_xw = DibWidth();     // use the whole DIB as one sprite
        m_yw = DibHeight();
    }
    CRect rcNew; GetRect(&rcNew);
    m_dirty.UnionRect(&rcNew,&rcOld); // set dirty-rectangle
    m_bTrans=Transparent;
    if (DibColorDepth()!=24)
        AfxMessageBox(L"Die Spritebibliothek unterstützt nur Bitmaps mit 24-Bit Farbtiefe");
    return TRUE;
}

// Load a sprite image from a Storage-DC's Bitmap
BOOL CSprite::Load(CDC* pDC, CSize PictSize, CSize SpriteSize, BOOL Transparent)
{
    CRect rcOld; GetRect(&rcOld);
    if (!CDIB::Load(pDC, PictSize)) {
        return FALSE;
    }
    m_xw = SpriteSize.cx;
    m_yw = SpriteSize.cy;

    CRect rcNew; GetRect(&rcNew);
    m_dirty.UnionRect(&rcNew,&rcOld); // set dirty-rectangle
    m_bTrans=Transparent;
    return TRUE;
}

BOOL CSprite::LoadBlank(CDC* pDC, CSize size,COLORREF color, BOOL Transparent)    // Loads a white Csprite compatible to pDC
{
    CRect rcOld; GetRect(&rcOld);
    if (!CDIB::LoadBlank(pDC,size,color)) {
        return FALSE;
    }
    m_xw = size.cx;  // use the whole DIB as one sprite
    m_yw = size.cy;

    CRect rcNew; GetRect(&rcNew);
    m_dirty.UnionRect(&rcNew,&rcOld); // set dirty-rectangle
    m_bTrans=Transparent;
    return TRUE;
}


/*// Load a sprite image from a COpenGlView
BOOL CSprite::Load(COpenGlView* pOpenGlView)
{
    return Load(pOpenGlView->GetpDC(), pOpenGlView->GetSize());
}*/

// Get the bounding rectangle.
void CSprite::GetRect(CRect* pRect)
{
    pRect->left = m_x;
    pRect->top = m_y;
    pRect->right = m_x + m_xw;
    pRect->bottom = m_y + m_yw;
}

// Get the dirty rectangle.
void CSprite::GetDirtyRect(CRect* pRect)
{
    *pRect = m_dirty;
}

// Set the dirty rectangle.
void CSprite::SetDirtyRect(CRect* pRect)
{
    m_dirty |= *pRect;  // union of two rects
}

// Test for a hit in a non-transparent area of the sprite
BOOL CSprite::HitTest(CPoint point,BOOL ignoretransparenz)
{
    // Test if the point is inside the sprite rectangle
    // and the Image is not the background image
    if ((point.x > m_x) && (point.x < m_x + m_xw)
        && (point.y > m_y ) && (point.y < m_y + m_yw)) {

        if (ignoretransparenz==TRUE) return TRUE;

        // See if this point is transparent by testing whether
        // the pixel value is the same as the top left
        // corner value.
        COLORREF *p = (COLORREF *) GetPixelAddress(
            point.x - m_x + (m_xsn * m_xw),
            point.y - m_y + (m_ysn * m_yw));
        if (memcmp(p,GetPixelAddress(0,0),DibColorDepth()/8) ||
            (m_bTrans == FALSE))
            return TRUE; // hit
    }
    return FALSE;
}

// Set a new x,y position to the sprite
void CSprite::SetPosition(int x, int y)
{
    CRect r,dr;

    // Save the current position.
    GetRect(&r);
    GetDirtyRect(&dr);
    if (r!=dr)
        SetDirtyRect(&r);

    // Move to new position.
    m_x = x; m_y = y;

    // Notify that we have moved from our old position to
    // our new position.
    GetRect(&r);
    GetDirtyRect(&dr);
    if (r!=dr)
        SetDirtyRect(&r);
}

// Gets the current  x,y-psoition of the sprite
CPoint CSprite::GetPos() {
    return CPoint(m_x,m_y);
}

// Gets the current  x-position of the sprite
int CSprite::GetXPos() {
    return m_x;
}

// Gets the current  y-position of the sprite
int CSprite::GetYPos() {
    return m_y;
}

// Set a new z-Position
void CSprite::SetZ(int z)
{
    if (z < 0) z = 0;     // do not accept a negative z position
    if ((m_z = z) == 0)
        m_bTrans = FALSE; // its the background image
    // notify the dirty region
    if (m_dirty == CRect(0,0,0,0))
        GetRect(&m_dirty);
}

// Get current z-Position
int CSprite::GetZ()
{
    return m_z;
}

// setzt den alpha-Kanal für das Sprite
void CSprite::SetAlpha(float a) {
    if (a > 1.0f) a = 1.0f;
    if (a < 0.0f) a = 0.0f;
    m_alpha = a;

    CRect dr; GetRect(&dr);
    SetDirtyRect(&dr);   // notify the dirty region
}

float CSprite::GetAlpha() {
    return m_alpha;
}

// Get the width of a sprite
int CSprite::GetXW()
{
    return m_xw;
}

// Get the height of a sprite
int CSprite::GetYW()
{
    return m_yw;
}

// Choose a new Sprite from the DIB-Image
void CSprite::SetSpriteNumber(int xsn, int ysn)
{
    if ((m_xsn == xsn) && (m_ysn == ysn))
        return;           // nothing new
    m_xsn = xsn; m_ysn = ysn;

    CRect dr; GetRect(&dr);
    SetDirtyRect(&dr);   // notify the dirty region
}

// Ask for the current Sprite of the DIB-Image
void CSprite::GetSpriteNumber(int *xsn, int *ysn) {
    if (xsn!=NULL) *xsn = m_xsn;
    if (ysn!=NULL) *ysn = m_ysn;
}


// Make a duplication from a sprite image
void CSprite::Copy(CSprite* pSprite)
{
    CDIB::Copy(pSprite);  // copy base class members
    // copy member values
    m_x = pSprite->m_x; m_y = pSprite->m_y; m_z = pSprite->m_z;
    m_xsn = pSprite->m_xsn; m_ysn = pSprite->m_ysn;
    m_xw = pSprite->m_xw; m_yw = pSprite->m_yw;
    m_bTrans = pSprite->m_bTrans;
    m_dirty = pSprite->m_dirty;
}

// Reset dirty region entry
void CSprite::ResetDirtyRect()
{
    m_dirty = CRect(0,0,0,0);
}

// Check if a given pixel is transparent or not
BOOL CSprite::IsPixelTransparent(CPoint point)
{
    return !HitTest(point);
}

// CrashTest for two different sprites in their non-transparent area
BOOL CSprite::CrashTest(CSprite* pS1)
{
    CPoint      pt;
    CPoint      p1LT, p1RB, p2LT, p2RB; // 2 points of sprite 1+2
                                        // (LT is the left upper corner)
    CRect       rec1, rec2, intrec;
    CSize       d1, d2;

    if (GetZ() != pS1->GetZ())  // Sprites don't use the same z-position
        return( FALSE );

    p1LT = p1RB = GetPos();
    p1RB.Offset( GetXW(), GetYW() );
    p2LT = p2RB = pS1->GetPos();
    p2RB.Offset( pS1->GetXW(), pS1->GetYW() );

    rec1.left   = p1LT.x;   rec1.top    = p1LT.y;
    rec1.right  = p1RB.x;   rec1.bottom = p1RB.y;
    rec2.left   = p2LT.x;   rec2.top    = p2LT.y;
    rec2.right  = p2RB.x;   rec2.bottom = p2RB.y;

    if (intrec.IntersectRect( &rec1, &rec2 ) == NULL)
        return( FALSE );                // sprites don't use the same area

    // check the intersected area of both sprites for non-transparent points
    for (int i=intrec.left; i<intrec.right; i++)
        for (int j=intrec.top; j<intrec.bottom; j++)
        {
            pt.x = i;
            pt.y = j;
            if ( !IsPixelTransparent(pt) )
                if ( !(pS1->IsPixelTransparent(pt)) )
                    return( TRUE );
        }

    return( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
//
// CSpriteList
//
/////////////////////////////////////////////////////////////////////////////

CSpriteList::CSpriteList()
{
    m_workspace = 0;
    m_fpswnd    = 0;
    m_fpscount  = 0;
}

CSpriteList::CSpriteList(CSpriteList& b) {
    AfxMessageBox(L"ein Kopier-Konstruktor wird durch die CSpriteList-Klasse nicht unterstützt");
    exit(1);
}

CSpriteList::~CSpriteList()
{
}

// load workspace for render method
void CSpriteList::SetWorkspace(CDIB *w)
{
    m_workspace=w;
}

// Remove everything from the list
void CSpriteList::RemoveAll()
{
    // Walk down the list deleting objects as we go.
    // We need to do this here because the base class
    // simply deletes the pointers.
/*    POSITION pos;
    CSprite* pSprite;
    for (pos = GetHeadPosition(); pos != NULL;) {
        pSprite = GetNext(pos); // Increment position.
        if (pSprite) {
            delete pSprite;
        }
    }
*/    // Now call the base class to remove the pointers.
    CObList::RemoveAll();
}

// Add a sprite to the list, placing it according to its z-order value.
BOOL CSpriteList::Insert(CSprite* pNewSprite)
{
    // Walk down the list until we either get to the end
    // or we find a sprite with the same or higher z-order
    // in which case we insert just before that one.

    CRect rect;
    CSprite* pSprite;
    pNewSprite->GetRect(&rect);
    POSITION pos;
    BOOL ok=FALSE;

    if (Find(pNewSprite)) {
        AfxMessageBox(L"Ein Sprite sollte nicht mehrmals in die Liste aufgenommen werden");
        return FALSE;
    }
    for( pos = GetHeadPosition(); pos != NULL; GetNext(pos)) {
        pSprite = (CSprite*) GetAt(pos);
        if (pSprite->GetZ() >= pNewSprite->GetZ()) {
            InsertBefore(pos, pNewSprite);
            ok = TRUE; break;             // insert is done
        }
    }

    // Nothing with the same or a higher z-order,
    // so add the sprite to the end.
    if (!ok)
        AddTail(pNewSprite);
    Render(&rect);
    pos = GetHeadPosition();
    CSprite* pBackground = GetNext(pos);  // get background sprite
    pBackground->SetDirtyRect(&rect);     // set background dirty
    return TRUE;
}

// Remove a sprite from the list, but do not delete it
CSprite* CSpriteList::Remove(CSprite* pSprite)
{
    POSITION pos = Find(pSprite);

    if (pos == NULL) {
        return NULL;
    }
    CRect rect;
    RemoveAt(pos);
    pSprite->GetRect(&rect);
    Render(&rect);
    pos = GetHeadPosition();
    if (pos != NULL) {
        CSprite* pBackground = GetNext(pos);  // get background sprite
        pBackground->SetDirtyRect(&rect);     // set background dirty
    }
    return pSprite;
}

// Move a sprite to its correct z-order position.
void CSpriteList::Reorder(CSprite* pSprite)
{
    // Remove the sprite from the list.
    if (!Remove(pSprite)) {
        TRACE("Unable to find sprite");
        return; // Not there.
    }
    // Now insert it again in the right place.
    Insert(pSprite);
}

// Test for a mouse hit on any sprite in the list.
CSprite* CSpriteList::HitTest(CPoint point,BOOL ignoretransparenz)
{
    // Walk the list buttom up
    POSITION pos = GetTailPosition();
    if (pos == NULL)
        return NULL;
    CSprite* pSprite = (CSprite *) GetTail();
    for (;; pSprite = GetPrev(pos)) {
        if (pSprite->HitTest(point,ignoretransparenz))
            return pSprite;
        if (pos == NULL)
            break;
    }
    return NULL;
}

// CrashTest for two different sprites in their non-transparent area
CSprite* CSpriteList::CrashTest(CSprite* pS1)
{
    // Walk the list top down.
    POSITION pos;
    CSprite* pSprite;
    for (pos = GetHeadPosition(); pos != NULL;)
    {
        pSprite = GetNext(pos); // Increment position.
        if (pSprite != pS1)
            if (pSprite->CrashTest(pS1))
                return pSprite;
    }
    return NULL;
}

// Draw the Sprites in the right order inside the ClipRect
void CSpriteList::Render(CRect* pClipRect)
{
    // Walk the list top down.
    if (m_workspace==NULL)
        return;            // nothing to render
    POSITION pos;
    CSprite* pSprite;
    for (pos = GetHeadPosition(); pos != NULL;) {
        pSprite = GetNext(pos); // Increment position.
        pSprite->Render(m_workspace, pClipRect);
    }
    fpsWriter();
}

// Update sprite rendering
void CSpriteList::Update(CDC* pDC, int x, int y)
{
    // Walk the list top down.
    POSITION pos;
    CSprite* pSprite;
    int zpos=0;
    CRect drect, sdrect=CRect(0,0,0,0);
    if (m_workspace==NULL)
        return;                 // nothing to render
    for (pos = GetHeadPosition(); pos != NULL;) {
        pSprite = GetNext(pos); // Increment position.
        pSprite->GetDirtyRect(&drect);
        if (drect != CRect(0,0,0,0)) {
            sdrect.UnionRect(&drect,&sdrect);
            pSprite->ResetDirtyRect();
        }
		if (zpos > pSprite->GetZ()) {
            Reorder(pSprite);
			pos = GetHeadPosition(); 
			zpos = 0;
		}else{
			zpos = pSprite->GetZ();
		}
    }
    Render(&sdrect);
    if (pDC)
        m_workspace->DrawRect(pDC,sdrect,x,y);
}

// Redraw the workspace of the given sprite list
void CSpriteList::RedrawAll(CDC* pDC,int x,int y)
{
    if (m_workspace) {
        Render(&CRect(0,0,m_workspace->DibWidth(),m_workspace->DibHeight()));
        m_workspace->Draw(pDC,x,y);
    }

    //for all Sprites in the List reset dirty rect
    POSITION pos = GetTailPosition();
    if (pos == NULL) return;
    CSprite* pSprite = (CSprite *) GetTail();
    for (;; pSprite = GetPrev(pos)) {
      pSprite->ResetDirtyRect();
      if (pos == NULL) break;
    }

}

CSpriteList& CSpriteList::operator=(CSpriteList&)
{
    AfxMessageBox(L"der Zuweisungsoperator wird durch die CSpriteList-Klasse nicht unterstützt!");
    return *this;
}

void CSpriteList::fpsCounter(CWnd* w)
{
    m_fpswnd   = w;
    m_fpscount = 0;
    m_fpstime  = timeGetTime();
}

float CSpriteList::fpsWriter(bool write)
{
    static float fps=0.0f;
    if (!m_fpswnd) return 0.0f;

    DWORD t2 = timeGetTime();
    m_fpscount++;

    if ((t2-m_fpstime)>1000) {
        CString s;
        fps = (m_fpscount/((t2-m_fpstime)/1000.f));
        if (write) {
            s.Format(L"%.2f fps", fps );
            m_fpswnd->SetWindowText(s);
        }
        m_fpscount = 0; m_fpstime = t2;
    }
    return fps;
}
