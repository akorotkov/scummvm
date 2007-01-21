/* ScummVM - Scumm Interpreter
 * Copyright (C) 2006 The ScummVM project
 *
 * Copyright (C) 1999-2003 Sarien Team
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
 * $URL$
 * $Id$
 *
 */

#include "common/stdafx.h"

#include "agi/agi.h"
#include "agi/opcodes.h"

namespace Agi {

int AgiEngine::v2IdGame() {
	int ver;

	ver = getVersion();
	_game.ver = ver;
	debugC(2, kDebugLevelMain, "game.ver = 0x%x", _game.ver);
	agiSetRelease(ver);

	return setupV2Game(ver, 0);
}

/*
 * Currently, there is no known difference between v3.002.098 -> v3.002.149
 * So version emulated;
 *
 * 0x0086,
 * 0x0149
 */

int AgiEngine::v3IdGame() {
	int ver;

	ver = getVersion();
	_game.ver = ver;
	debugC(2, kDebugLevelMain, "game.ver = 0x%x", _game.ver);
	agiSetRelease(ver);

	return setupV3Game(ver, 0);
}

/**
 *
 */
int AgiEngine::setupV2Game(int ver, uint32 crc) {
	int ec = errOK;

	if (ver == 0) {
		report("Unknown v2 Sierra game: %08x\n\n", crc);
		agiSetRelease(ver = 0x2917);
	}

	/* setup the differences in the opcodes and other bits in the
	 * AGI v2 specs
	 */
	if (_opt.emuversion)
		agiSetRelease(ver = _opt.emuversion);

	// Should this go above the previous lines, so we can force emulation versions
	// even for AGDS games? -- dsymonds
	if (_opt.agdsMode)
		agiSetRelease(ver = 0x2440);	/* ALL AGDS games built for 2.440 */

	report("Seting up for version 0x%04X\n", ver);

	// 'quit' takes 0 args for 2.089
	if (ver == 0x2089)
		logicNamesCmd[0x86].numArgs = 0;

	// 'print.at' and 'print.at.v' take 3 args before 2.272
	// This is documented in the specs as only < 2.440, but it seems
	// that KQ3 (2.272) needs a 'print.at' taking 4 args.
	if (ver < 0x2272) {
		logicNamesCmd[0x97].numArgs = 3;
		logicNamesCmd[0x98].numArgs = 3;
	}

	return ec;
}

/**
 *
 */
int AgiEngine::setupV3Game(int ver, uint32 crc) {
	int ec = errOK;

	if (ver == 0) {
		report("Unknown v3 Sierra game: %08x\n\n", crc);
		agiSetRelease(ver = 0x3149);
	}

	if (_opt.emuversion)
		agiSetRelease(ver = _opt.emuversion);

	report("Seting up for version 0x%04X\n", ver);

	// 'unknown176' takes 1 arg for 3.002.086, not 0 args.
	// 'unknown173' also takes 1 arg for 3.002.068, not 0 args.
	// Is this actually used anywhere? -- dsymonds
	if (ver == 0x3086) {
		logicNamesCmd[0xb0].numArgs = 1;
		logicNamesCmd[0xad].numArgs = 1;
	}

	return ec;
}

} // End of namespace Agi
