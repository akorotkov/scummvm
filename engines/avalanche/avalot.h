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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This code is based on the original source code of Lord Avalot d'Argent version 1.3.
 * Copyright (c) 1994-1995 Mike, Mark and Thomas Thurman.
 */

/* AVALOT		The kernel of the program. */

#ifndef AVALANCHE_AVALOT_H
#define AVALANCHE_AVALOT_H

#include "avalanche/animation.h"

#include "common/events.h"
#include "common/system.h"
#include "common/str.h"
#include "common/scummsys.h"
#include "common/file.h"
#include "graphics/surface.h"

namespace Avalanche {
class AvalancheEngine;

class Clock {
public:
	Clock(AvalancheEngine *vm);

	void update();
	
private:
	static const int kCenterX = 510;
	static const int kCenterY = 183;

	AvalancheEngine *_vm;

	uint16 _hour, _minute, _second, _hourAngle, _oldHour, _oldMinute, _oldHourAngle;
	Common::Point _clockHandHour, _clockHandMinute;

	Common::Point calcHand(uint16 angle, uint16 length, Color color);
	void drawHand(const Common::Point &endPoint, Color color);
	void plotHands();
	void chime();
};

static const byte kObjectNum = 18; // always preface with a #
static const int16 kCarryLimit = 12;  // carry limit

static const int16 kNumlockCode = 32;  // Code for Num Lock
static const int16 kMouseSize = 134;

struct PedType {
	int16 _x, _y;
	Direction _direction;
};

struct MagicType {
	byte _operation; // one of the operations
	uint16 _data; // data for them
};

struct FieldType {
	int16 _x1, _y1, _x2, _y2;
};

struct LineType : public FieldType {
	Color _color;
};

typedef int8 TuneType[31];

struct QuasipedType {
	byte   _whichPed;
	Color  _textColor;
	Room   _room;
	Color  _backgroundColor;
	People _who;
};

#if 0
struct Sundry { // Things which must be saved over a backtobootstrap, outside DNA.
	Common::String _qEnidFilename;
	bool _qSoundFx;
	byte _qThinks;
	bool _qThinkThing;
};
#endif

} // End of namespace Avalanche

#endif // AVALANCHE_AVALOT_H
