
/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BAGEL_BAGLIB_PAN_BITMAP_H
#define BAGEL_BAGLIB_PAN_BITMAP_H

#ifdef COMPRESSED
#include "bagel/boflib/gfx/compressed_bitmap.h"
#else
#include "bagel/boflib/gfx/bitmap.h"
#endif
#include "bagel/boflib/fixed.h"

namespace Bagel {

#define MAXDIVVIEW (12.8 / 3)     // Ratio for 480x380 Screen
#define DEFFOV (360 / MAXDIVVIEW) //	1TO1 Paint FOV

class CBagPanBitmap :
#ifdef COMPRESSED
	public CBofCompressedBitmap
#else
	public CBofBitmap
#endif
{
public:
	enum DIRECTION {
		NONE = 0x0,
		UP = 0x01,
		DOWN = 0x02,
		LEFT = 0x04,
		RIGHT = 0x08,
		VIEW = 0x10
	};

private:
	CBofRect m_xCurrView;      // Viewport Window size (0->Width-1,0->Heigth-1,1->Width+Width/4,1->Heigth)
	double m_xFOVAngle;        // Feild of view in radians
	CBofPoint m_xRotateRate;   // Rate of rotation on increment left, right ...
	BOOL m_bActiveScrolling;   // True when there should be screen updates
	BOOL m_bPanorama;          // True when the bitmap is a 360 panorama
	DIRECTION m_xDirection;    // Set direction for next update
	int m_nCorrWidth;          // Size of each correction band
	CBofFixed *m_pCosineTable; // Lookup table for cosine values
	int m_nNumDegrees;         // Number of lookups in the cosine table
	BOOL m_bIsValid;           // Is the bmp a valid object
	static INT AdjustConvexUp(CBofRect &, const CBofRect &, const INT);
	static INT AdjustConvexDown(CBofRect &, const CBofRect &, const INT);
	static INT AdjustPlanar(CBofRect &, const CBofRect &, const INT);

	void NormalizeViewSize();
	void GenerateCosineTable();

public:
	CBagPanBitmap(const CHAR *pszFileName, CBofPalette *pPalette, const CBofRect &xViewSize = CBofRect());
	CBagPanBitmap(INT dx, INT dy, CBofPalette *pPalette, const CBofRect &xViewSize = CBofRect());
	virtual ~CBagPanBitmap();

	BOOL IsValid() { return m_bIsValid; }
	BOOL IsPan() { return m_bPanorama; }

	ERROR_CODE Paint(CBofBitmap *pBmp, const CBofPoint xDstOffset = CBofPoint(0, 0));
	ERROR_CODE Paint(CBofWindow *pWnd, const CBofPoint xDstOffset = CBofPoint(0, 0));
	ERROR_CODE PaintUncorrected(CBofBitmap *pBmp, CBofRect &dstRect);
	ERROR_CODE PaintWarped(CBofBitmap *pBmp, const CBofRect &dstRect, const CBofRect &srcRect, const int offset = 0, CBofBitmap *pSrcBmp = nullptr, const CBofRect &preSrcRect = CBofRect());
	ERROR_CODE PaintWarped4(CBofBitmap *pBmp, const CBofRect &dstRect, const CBofRect &srcRect, const int offset = 0, CBofBitmap *pSrcBmp = nullptr, const CBofRect &preSrcRect = CBofRect());

	CBofRect GetWarpSrcRect();
	CBofPoint WarpedPoint(CBofPoint &xPoint);

	double GetFOV() { return m_xFOVAngle; }
	const CBofSize GetViewSize() { return CBofPoint(m_xCurrView.Size()); }
	const CBofRect GetCurrView() { return m_xCurrView; }

	const CBofRect GetMaxView(CBofSize s = CBofSize(640, 480));
	const CBofPoint GetRotateRate() { return m_xRotateRate; }
	DIRECTION GetDirection() { return m_xDirection; }

	void SetRotateRate(const CBofPoint &xRotRate) { m_xRotateRate = xRotRate; }
	void SetDirection(const DIRECTION xDirection) { m_xDirection = xDirection; }
	void SetCurrView(const CBofRect &xCurrView) {
		m_xCurrView = xCurrView;
		NormalizeViewSize();
	}
	void OffsetCurrView(const CBofPoint &xOffset) {
		CBofRect xCurrView = m_xCurrView;
		xCurrView.OffsetRect(xOffset);
		SetCurrView(xCurrView);
	}
	void SetFOV(double degrees, BOOL bUpdate = TRUE) {
		m_xFOVAngle = degrees / 114.5916558176;
		if (bUpdate) {
			// m_xCurrView.SetRect(0, m_xCurrView.top, Width()*degrees/360, m_xCurrView.bottom);
			GenerateCosineTable();
		}
	}
	int GetCorrWidth() { return m_nCorrWidth; }

	void SetCorrWidth(int nWidth, BOOL bUpdate = TRUE);

	void SetViewSize(const CBofSize &xViewSize, BOOL bUpdate = TRUE) {
		m_xCurrView.right = m_xCurrView.left + xViewSize.cx;
		m_xCurrView.bottom = m_xCurrView.top + xViewSize.cy;
		NormalizeViewSize();

		if (bUpdate) {
			GenerateCosineTable();
		}
	}
	CBofSize SetUnityViewSize() {
		int w = (int)(Width() * m_xFOVAngle / 3.14159);
		m_xCurrView.SetRect(0, m_xCurrView.top, w, m_xCurrView.bottom);
		GenerateCosineTable();
		return GetViewSize();
	}
	double SetUnityFOV() {
		SetFOV(360.0 * m_xCurrView.Width() / Width(), FALSE); // If FOV is set to 0 then unity FOV is assumed (faster redraws)
		GenerateCosineTable();
		return GetFOV();
	}
	void RotateRight(int nXRotRate = 0);
	void RotateLeft(int nXRotRate = 0);
	void RotateUp(int nYRotRate = 0);
	void RotateDown(int nYRotRate = 0);

	DIRECTION UpdateView();

	void ActivateScrolling(BOOL val = TRUE) { m_bActiveScrolling = val; }
	void DeActivateScrolling() { ActivateScrolling(FALSE); }
};

} // namespace Bagel

#endif
