/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This code is based on original Soltys source code
 * Copyright (c) 1994-1995 Janus B. Wisniewski and L.K. Avalon
 */

#ifndef __VOL__
#define __VOL__

#include "cge/btfile.h"
#include "cge/cfile.h"

namespace CGE {

#define CAT_NAME    "VOL.CAT"
#define DAT_NAME    "VOL.DAT"

#ifndef CRP
#define CRP     XCrypt
#endif

#define XMASK       0xA5

#ifdef  VOL_UPD
#define VOLBASE     IOHAND
#else
#define VOLBASE     CFile
#endif


class DAT {
	friend class VFILE;
	VOLBASE _File;
public:
	DAT();

	bool append(uint8 *buf, uint16 len);
	bool write(CFile &f);
	bool read(long org, uint16 len, uint8 *buf);
};


class   VFILE : public IoBuf {
private:
	static DAT *_dat;
	static BtFile *_cat;
	static VFILE *_recent;

	long _begMark;
	long _endMark;

	void readBuff(void);
	void writeBuff(void) { }
	void make(const char *fspec);
public:
	VFILE(const char *name, IOMODE mode = REA);
	~VFILE(void);
	static void init();
	static void deinit();

	static bool exist(const char *name);
	static const char *next(void);
	long mark(void) {
		return (_bufMark + _ptr) - _begMark;
	}
	long size(void) {
		return _endMark - _begMark;
	}
	long seek(long pos) {
		_recent = NULL;
		_lim = 0;
		return (_bufMark = _begMark + pos);
	}
};


} // End of namespace CGE

#endif
