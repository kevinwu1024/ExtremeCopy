
/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#pragma once
#include "..\Common\ptGlobal.h"

class CptWinGraphic
{
public:
	static bool GetBitmapSize(HBITMAP hBitmap,int& nWidth,int& nHeight)
	{
		bool bRet = false ;

		if(hBitmap!=NULL)
		{
			BITMAP bm ;
			::memset(&bm,0,sizeof(bm)) ;

			bRet = ::GetObject(hBitmap,sizeof(bm),&bm)>0;  

			nWidth = bm.bmWidth ;
			nHeight = bm.bmHeight ;
		}

		return bRet ;
	}

	static void TransparentBlt2(HDC hdcDest,int nXOriginDest, int nYOriginDest,
					 int nWidthDest, int nHeightDest, HDC hdcSrc,int nXOriginSrc,
					 int nYOriginSrc, int nWidthSrc,int nHeightSrc,
					 COLORREF crTransparent
					 )
	{
		HBITMAP hOldImageBMP, hImageBMP = CreateCompatibleBitmap(hdcDest, nWidthDest, nHeightDest);	// 创建兼容位图
		HBITMAP hOldMaskBMP, hMaskBMP = CreateBitmap(nWidthDest, nHeightDest, 1, 1, NULL);			// 创建单色掩码位图
		HDC		hImageDC = CreateCompatibleDC(hdcDest);
		HDC		hMaskDC = CreateCompatibleDC(hdcDest);
		hOldImageBMP = (HBITMAP)SelectObject(hImageDC, hImageBMP);
		hOldMaskBMP = (HBITMAP)SelectObject(hMaskDC, hMaskBMP);

		// 将源DC中的位图拷贝到临时DC中
		if (nWidthDest == nWidthSrc && nHeightDest == nHeightSrc)
			BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
		else
			StretchBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, 
			hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);

		// 设置透明色
		SetBkColor(hImageDC, crTransparent);

		// 生成透明区域为白色，其它区域为黑色的掩码位图
		BitBlt(hMaskDC, 0, 0, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCCOPY);

		// 生成透明区域为黑色，其它区域保持不变的位图
		SetBkColor(hImageDC, RGB(0,0,0));
		SetTextColor(hImageDC, RGB(255,255,255));
		BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);

		// 透明部分保持屏幕不变，其它部分变成黑色
		SetBkColor(hdcDest,RGB(0xff,0xff,0xff));
		SetTextColor(hdcDest,RGB(0,0,0));
		BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);

		// "或"运算,生成最终效果
		BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCPAINT);

		SelectObject(hImageDC, hOldImageBMP);
		DeleteDC(hImageDC);
		SelectObject(hMaskDC, hOldMaskBMP);
		DeleteDC(hMaskDC);
		DeleteObject(hImageBMP);
		DeleteObject(hMaskBMP);
	}

	static HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
	{
		HRGN hRgn = NULL;

		if (hBmp)
		{
			HDC hMemDC = CreateCompatibleDC(NULL);
			if (hMemDC)
			{
				BITMAP bm;
				GetObject(hBmp, sizeof(bm), &bm);

				//创建一个32位色的位图，并选进内存设备环境
				BITMAPINFOHEADER RGB32BITSBITMAPINFO = {
					sizeof(BITMAPINFOHEADER),  // biSize
					bm.bmWidth,     // biWidth;
					bm.bmHeight,    // biHeight; 
					1,       // biPlanes;
					32,       // biBitCount 
					BI_RGB,      // biCompression; 
					0,       // biSizeImage; 
					0,       // biXPelsPerMeter; 
					0,       // biYPelsPerMeter; 
					0,       // biClrUsed; 
					0       // biClrImportant; 
				};
				VOID * pbits32; 
				HBITMAP hbm32 = CreateDIBSection(hMemDC,(BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
				if (hbm32)
				{
					HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

					// Create a DC just to copy the bitmap into the memory DC
					HDC hDC = CreateCompatibleDC(hMemDC);
					if (hDC)
					{
						// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
						BITMAP bm32;
						GetObject(hbm32, sizeof(bm32), &bm32);
						while (bm32.bmWidthBytes % 4)
							bm32.bmWidthBytes++;

						// Copy the bitmap into the memory DC
						HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
						BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

						// For better performances, we will use the ExtCreateRegion() function to create the
						// region. This function take a RGNDATA structure on entry. We will add rectangles by
						// amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT 100
						DWORD maxRects = ALLOC_UNIT;
						HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
						RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
						pData->rdh.dwSize = sizeof(RGNDATAHEADER);
						pData->rdh.iType = RDH_RECTANGLES;
						pData->rdh.nCount = pData->rdh.nRgnSize = 0;
						SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

						// Keep on hand highest and lowest values for the "transparent" pixels
						BYTE lr = GetRValue(cTransparentColor);
						BYTE lg = GetGValue(cTransparentColor);
						BYTE lb = GetBValue(cTransparentColor);
						BYTE hr = min(0xff, lr + GetRValue(cTolerance));
						BYTE hg = min(0xff, lg + GetGValue(cTolerance));
						BYTE hb = min(0xff, lb + GetBValue(cTolerance));

						// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
						BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
						for (int y = 0; y < bm.bmHeight; y++)
						{
							// Scan each bitmap pixel from left to right
							for (int x = 0; x < bm.bmWidth; x++)
							{
								// Search for a continuous range of "non transparent pixels"
								int x0 = x;
								LONG *p = (LONG *)p32 + x;
								while (x < bm.bmWidth)
								{
									BYTE b = GetRValue(*p);
									if (b >= lr && b <= hr)
									{
										b = GetGValue(*p);
										if (b >= lg && b <= hg)
										{
											b = GetBValue(*p);
											if (b >= lb && b <= hb)
												// This pixel is "transparent"
												break;
										}
									}
									p++;
									x++;
								}

								if (x > x0)
								{
									// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
									if (pData->rdh.nCount >= maxRects)
									{
										GlobalUnlock(hData);
										maxRects += ALLOC_UNIT;
										hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
										pData = (RGNDATA *)GlobalLock(hData);
									}
									RECT *pr = (RECT *)&pData->Buffer;
									SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
									if (x0 < pData->rdh.rcBound.left)
										pData->rdh.rcBound.left = x0;
									if (y < pData->rdh.rcBound.top)
										pData->rdh.rcBound.top = y;
									if (x > pData->rdh.rcBound.right)
										pData->rdh.rcBound.right = x;
									if (y+1 > pData->rdh.rcBound.bottom)
										pData->rdh.rcBound.bottom = y+1;
									pData->rdh.nCount++;

									// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
									// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
									if (pData->rdh.nCount == 2000)
									{
										HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
										if (hRgn)
										{
											CombineRgn(hRgn, hRgn, h, RGN_OR);
											DeleteObject(h);
										}
										else
											hRgn = h;
										pData->rdh.nCount = 0;
										SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
									}
								}
							}

							// Go to next row (remember, the bitmap is inverted vertically)
							p32 -= bm32.bmWidthBytes;
						}

						// Create or extend the region with the remaining rectangles
						HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
						if (hRgn)
						{
							CombineRgn(hRgn, hRgn, h, RGN_OR);
							DeleteObject(h);
						}
						else
							hRgn = h;

						// Clean up
						GlobalFree(hData);
						SelectObject(hDC, holdBmp);
						DeleteDC(hDC);
					}    
					DeleteObject(SelectObject(hMemDC, holdBmp));
				}   
				DeleteDC(hMemDC);
				::DeleteObject(hbm32) ;
			}
		} 
		return hRgn; 
	}

	static int  SaveBitmapToFile2(HBITMAP   hBitmap,   LPCTSTR   lpFileName)   //hBitmap   为刚才的屏幕位图句柄   
	{           //lpFileName   为位图文件名   
		HDC           hDC;                     
		//设备描述表   
		int           iBits;               
		//当前显示分辨率下每个像素所占字节数   
		WORD         wBitCount;         
		//位图中每个像素所占字节数   
		//定义调色板大小，   位图中像素字节大小   ，     位图文件大小   ，   写入文件字节数   
		DWORD                       dwPaletteSize=0,dwBmBitsSize,dwDIBSize,   dwWritten;   
		BITMAP                     Bitmap;                   
		//位图属性结构   
		BITMAPFILEHEADER       bmfHdr;                   
		//位图文件头结构   
		BITMAPINFOHEADER       bi;                           
		//位图信息头结构     
		LPBITMAPINFOHEADER   lpbi;                       
		//指向位图信息头结构   
		HANDLE                     fh,   hDib,   hPal;   
		HPALETTE           hOldPal=NULL;   
		//定义文件，分配内存句柄，调色板句柄   

		//计算位图文件每个像素所占字节数   
		hDC   =   CreateDC(_T("DISPLAY"),NULL,NULL,NULL);   
		iBits   =   GetDeviceCaps(hDC,   BITSPIXEL) * GetDeviceCaps(hDC,PLANES); 

		DeleteDC(hDC);   
		if   (iBits   <=   1)   
			wBitCount   =   1;   
		else   if   (iBits   <=   4)   
			wBitCount   =   4;   
		else   if   (iBits   <=   8)   
			wBitCount   =   8;   
		else   if   (iBits   <=   24)   
			wBitCount   =   24;   
		else wBitCount = 32 ;
		//计算调色板大小   
		if   (wBitCount   <=   8)   
#ifdef _WIN64
			dwPaletteSize=(1i64<<wBitCount)*sizeof(RGBQUAD);   
#else
			dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);   
#endif

		//设置位图信息头结构   
		GetObject(hBitmap,   sizeof(BITMAP),   (LPSTR)&Bitmap);   
		bi.biSize                         =   sizeof(BITMAPINFOHEADER);   
		bi.biWidth                       =   Bitmap.bmWidth;   
		bi.biHeight                     =   Bitmap.bmHeight;   
		bi.biPlanes                     =   1;   
		bi.biBitCount                   =   wBitCount;   
		bi.biCompression             =   BI_RGB;   
		bi.biSizeImage                 =   0;   
		bi.biXPelsPerMeter           =   0;   
		bi.biYPelsPerMeter           =   0;   
		bi.biClrUsed                   =   0;   
		bi.biClrImportant             =   0;   

		dwBmBitsSize   =   ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;   
		//为位图内容分配内存   
		hDib     =   GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));   
		lpbi   =   (LPBITMAPINFOHEADER)GlobalLock(hDib);   
		*lpbi   =   bi;   
		//   处理调色板         
		hPal   =   GetStockObject(DEFAULT_PALETTE);   
		if   (hPal)   
		{   
			//hDC   =   GetDC(NULL);   
             hOldPal=SelectPalette(hDC,(HPALETTE)hPal,FALSE);   
            RealizePalette(hDC);   
		}   
		//   获取该调色板下新的像素值   
		GetDIBits(hDC,hBitmap,0,(UINT)Bitmap.bmHeight,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize,   (BITMAPINFO   *)lpbi,DIB_RGB_COLORS);   
		//恢复调色板         
        if   (hOldPal)   
		{   
			SelectPalette(hDC,   hOldPal,   TRUE);   
			RealizePalette(hDC);   
		//	ReleaseDC(NULL,   hDC);   
		}   
		BITMAPINFO* pp = (BITMAPINFO   *)lpbi ;

		//char szBuf[256] = {0} ;
		//sprintf(szBuf,"%s_%d.bmp",lpFileName,dwBmBitsSize) ;
		//		strcpy(szBuf,lpFileName) ;
		//		int aad = memcmp(g_Buffer,(LPSTR)lpbi+sizeof(SSectionBitmapData),dwBmBitsSize) ;

		//创建位图文件           
		fh=CreateFile(lpFileName,   GENERIC_WRITE,0,   NULL,   CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL   |   FILE_FLAG_SEQUENTIAL_SCAN,   NULL);   
		if   (fh==INVALID_HANDLE_VALUE)   
			return   FALSE;   
		//   设置位图文件头   
		bmfHdr.bfType   =   0x4D42;     //   "BM"   
		dwDIBSize=sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;       
		bmfHdr.bfSize   =   dwDIBSize;   
		bmfHdr.bfReserved1   =   0;   
		bmfHdr.bfReserved2   =   0;   
		bmfHdr.bfOffBits   =   (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize; 

		//   写入位图文件头   
		WriteFile(fh,   (LPSTR)&bmfHdr,   sizeof(BITMAPFILEHEADER),   &dwWritten,   NULL);   
		//   写入位图文件其余内容   
		WriteFile(fh,   (LPSTR)lpbi,   dwDIBSize,     
			&dwWritten,   NULL);   
		//清除         
		GlobalUnlock(hDib);   
		GlobalFree(hDib);   
		CloseHandle(fh);   
		return   TRUE;   
	}   


	//创建渐变图
	static HBITMAP CreateGradientBMP(HDC hDC, COLORREF cl1, COLORREF cl2, int nWidth, int nHeight, int nDir, int nNumColors)
	{
		if(nNumColors > 256)
			nNumColors = 256;

		COLORREF		PalVal[256];
		memset(PalVal, 0, sizeof(COLORREF)*256);

		int nIndex;
		BYTE peRed=0,peGreen=0,peBlue=0;

		int r1=GetRValue(cl1);
		int r2=GetRValue(cl2);
		int g1=GetGValue(cl1);
		int g2=GetGValue(cl2);
		int b1=GetBValue(cl1);
		int b2=GetBValue(cl2);

		for (nIndex = 0; nIndex < nNumColors; nIndex++)
		{
			peRed = (BYTE) (r1 + MulDiv((r2-r1),nIndex,nNumColors-1));
			peGreen = (BYTE) (g1 + MulDiv((g2-g1),nIndex,nNumColors-1));
			peBlue = (BYTE) (b1 + MulDiv((b2-b1),nIndex,nNumColors-1));

			PalVal[nIndex]=(peRed << 16) | (peGreen << 8) | (peBlue);
		}

		int x,y,w,h;
		w=nWidth;
		h=nHeight;

		LPDWORD			pGradBits;
		BITMAPINFO		GradBitInfo;

		pGradBits=(DWORD*) malloc(w*h*sizeof(DWORD));
		memset(&GradBitInfo,0,sizeof(BITMAPINFO));

		GradBitInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		GradBitInfo.bmiHeader.biWidth=w;
		GradBitInfo.bmiHeader.biHeight=h;
		GradBitInfo.bmiHeader.biPlanes=1;
		GradBitInfo.bmiHeader.biBitCount=32;
		GradBitInfo.bmiHeader.biCompression=BI_RGB;

		if(nDir==0) 
		{
			for(y=0;y<h;y++) 
			{
				for(x=0;x<w;x++) 
				{
					*(pGradBits+(y*w)+x)=PalVal[MulDiv(nNumColors,y,h)];
				}
			}
		}
		else if(nDir==1) 
		{
			for(y=0;y<h;y++)
			{
				int l,r;
				l=MulDiv((nNumColors/2),y,h);
				r=l+(nNumColors/2)-1;
				for(x=0;x<w;x++)
				{
					*(pGradBits+(y*w)+x)=PalVal[l+MulDiv((r-l),x,w)];
				}
			}
		}
		else if(nDir==2)
		{
			for(x=0;x<w;x++)
			{
				for(y=0;y<h;y++)
				{
					*(pGradBits+(y*w)+x)=PalVal[MulDiv(nNumColors,x,w)];
				}
			}
		}
		else if(nDir==3)
		{
			for(y=0;y<h;y++)
			{
				int l,r;
				r=MulDiv((nNumColors/2),y,h);
				l=r+(nNumColors/2)-1;
				for(x=0;x<w;x++)
				{
					*(pGradBits+(y*w)+x)=PalVal[l+MulDiv((r-l),x,w)];
				}
			}
		}

		HBITMAP hBmp = CreateDIBitmap(hDC,&GradBitInfo.bmiHeader,CBM_INIT,
			pGradBits,&GradBitInfo,DIB_RGB_COLORS);

		free(pGradBits);

		return hBmp;
	}
};

