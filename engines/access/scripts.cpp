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

#include "common/scummsys.h"
#include "access/access.h"
#include "access/scripts.h"

#define SCRIPT_START_BYTE 0xE0

namespace Access {

Scripts::Scripts(AccessEngine *vm) : _vm(vm) {
	_rawData = nullptr;
	_data = nullptr;
	_sequence = 0;
	_endFlag = false;
	_returnCode = 0;
	_choice = 0;
	_choiceStart = 0;
}

Scripts::~Scripts() {
	freeScriptData();
}

void Scripts::setScript(const byte *data, int size) {
	_rawData = data;
	_data = new Common::MemoryReadStream(data, size, DisposeAfterUse::NO);
}

void Scripts::freeScriptData() {
	delete[] _rawData;
	delete _data;
	_data = nullptr;
	_rawData = nullptr;
}

void Scripts::searchForSequence() {
	assert(_data);

	_data->seek(0);
	int sequenceId;
	do {
		while (_data->readByte() != SCRIPT_START_BYTE) ;
		sequenceId = _data->readUint16LE();
	} while (sequenceId != _sequence);
}

int Scripts::executeScript() {
	assert(_data);
	_endFlag = false;
	_returnCode = 0;

	do {
		// Get next command, skipping over script start start if it's being pointed to
		while ((_scriptCommand = _data->readByte()) == SCRIPT_START_BYTE)
			_data->skip(2);

		assert(_scriptCommand >= 0x80);
		executeCommand(_scriptCommand - 0x80);
	} while (!_endFlag);

	return _returnCode;
}

void Scripts::executeCommand(int commandIndex) {
	static const ScriptMethodPtr COMMAND_LIST[] = {
		&Scripts::CMDOBJECT, &Scripts::CMDENDOBJECT, &Scripts::cmdJumpLook, 
		&Scripts::cmdJumpHelp, &Scripts::cmdJumpGet, &Scripts::cmdJumpMove,
		&Scripts::cmdJumpUse, &Scripts::cmdJumpTalk, &Scripts::cmdNull, 
		&Scripts::cmdPrint, &Scripts::cmdRetPos, &Scripts::cmdAnim,
		&Scripts::cmdSetFlag, &Scripts::cmdCheckFlag, &Scripts::cmdGoto, 
		&Scripts::cmdSetInventory, &Scripts::cmdSetInventory, &Scripts::cmdCheckInventory, 
		&Scripts::CMDSETTEX, &Scripts::CMDNEWROOM, &Scripts::CMDCONVERSE, 
		&Scripts::cmdCheckFrame, &Scripts::cmdCheckAnim, &Scripts::cmdSnd, 
		&Scripts::cmdRetNeg, &Scripts::cmdRetPos, &Scripts::cmdCheckLoc, 
		&Scripts::cmdSetAnim, &Scripts::cmdDispInv, &Scripts::CMDSETTIMER, 
		&Scripts::CMDSETTIMER, &Scripts::CMDCHECKTIMER, &Scripts::cmdSetTravel,
		&Scripts::cmdSetTravel, &Scripts::CMDSETVID, &Scripts::CMDPLAYVID, 
		&Scripts::cmdPlotImage, &Scripts::cmdSetDisplay, &Scripts::cmdSetBuffer, 
		&Scripts::cmdSetScroll, &Scripts::CMDSAVERECT, &Scripts::CMDSAVERECT, 
		&Scripts::CMDSETBUFVID, &Scripts::CMDPLAYBUFVID, &Scripts::cmdRemoveLast, 
		&Scripts::CMDSPECIAL, &Scripts::CMDSPECIAL, &Scripts::CMDSPECIAL,
		&Scripts::CMDSETCYCLE, &Scripts::CMDCYCLE, &Scripts::CMDCHARSPEAK, 
		&Scripts::CMDTEXSPEAK, &Scripts::CMDTEXCHOICE, &Scripts::CMDWAIT, 
		&Scripts::CMDSETCONPOS, &Scripts::CMDCHECKVFRAME, &Scripts::cmdJumpChoice, 
		&Scripts::cmdReturnChoice, &Scripts::cmdClearBlock, &Scripts::CMDLOADSOUND, 
		&Scripts::CMDFREESOUND, &Scripts::CMDSETVIDSND, &Scripts::CMDPLAYVIDSND,
		&Scripts::CMDPUSHLOCATION, &Scripts::CMDPUSHLOCATION, &Scripts::CMDPUSHLOCATION, 
		&Scripts::CMDPUSHLOCATION, &Scripts::CMDPUSHLOCATION, &Scripts::cmdPlayerOff, 
		&Scripts::cmdPlayerOn, &Scripts::CMDDEAD, &Scripts::cmdFadeOut,
		&Scripts::CMDENDVID, &Scripts::CMDHELP, &Scripts::CMDCYCLEBACK, 
		&Scripts::CMDCHAPTER, &Scripts::cmdSetHelp, &Scripts::cmdCenterPanel,
		&Scripts::cmdMainPanel, &Scripts::CMDRETFLASH
	};

	(this->*COMMAND_LIST[commandIndex])();
}

void Scripts::CMDOBJECT() { 
	_vm->_room->_bubbleBox.load(_data);
}

void Scripts::CMDENDOBJECT() { error("TODO ENDOBJECT"); }

void Scripts::cmdJumpLook() {
	if (_vm->_room->_selectCommand == 0)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdJumpHelp() { 
	if (_vm->_room->_selectCommand == 8)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdJumpGet() { 
	if (_vm->_room->_selectCommand == 3)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdJumpMove() { 
	if (_vm->_room->_selectCommand == 2)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdJumpUse() { 
	if (_vm->_room->_selectCommand == 4)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdJumpTalk() { 
	if (_vm->_room->_selectCommand == 6)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdNull() {
}

#define PRINT_TIMER 25

void Scripts::cmdPrint() { 
	_vm->_fonts._printOrg = Common::Point(20, 42);
	_vm->_fonts._printStart = Common::Point(20, 42);
	_vm->_timers[PRINT_TIMER]._timer = 50;
	_vm->_timers[PRINT_TIMER]._initTm = 50;
	_vm->_timers[PRINT_TIMER]._flag = true;

	_vm->_room->placeBubble();
	_vm->_events->waitKeyMouse();

	while (_vm->_timers[PRINT_TIMER]._flag) {
		_vm->_events->pollEvents();
	}

	_vm->_screen->restoreBlock();
}

void Scripts::cmdRetPos() {
	_endFlag = true;
	_returnCode = 0;
}

void Scripts::cmdAnim() { 
	int animId = _data->readByte();
	_vm->_animation->animate(animId);
}

void Scripts::cmdSetFlag() { 
	int flagNum = _data->readByte();
	byte flagVal = _data->readByte();
	assert(flagNum < 256);

	_vm->_flags[flagNum] = flagVal;
}

void Scripts::cmdCheckFlag() { 
	int flagNum = _data->readUint16LE();
	int flagVal = _data->readUint16LE();
	assert(flagNum < 256);

	if (_vm->_flags[flagNum] == flagVal)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdGoto() { 
	_sequence = _data->readUint16LE();
	searchForSequence();
}

void Scripts::cmdSetInventory() { 
	int itemId = _data->readByte();
	int itemVal = _data->readByte();

	(*_vm->_inventory)[itemId] = itemVal;
	_vm->_inventory->_startInvItem = 0;
	_vm->_inventory->_startInvBox = 0;
	_vm->_inventory->_invChangeFlag = true;
}

void Scripts::cmdCheckInventory() { 
	int itemId = _data->readUint16LE();
	int itemVal = _data->readUint16LE();

	if ((*_vm->_inventory)[itemId] == itemVal)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::CMDSETTEX() { error("TODO CMDSETTEX"); }
void Scripts::CMDNEWROOM() { error("TODO CMDNEWROOM"); }
void Scripts::CMDCONVERSE() { error("TODO CMDCONVERSE"); }

void Scripts::cmdCheckFrame() {
	int id = _data->readUint16LE();
	Animation *anim = _vm->_animation->findAnimation(id);

	int frame = _data->readUint16LE();
	if (anim->_frameNumber == frame)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdCheckAnim() {
	int id = _data->readUint16LE();
	Animation *anim = _vm->_animation->findAnimation(id);

	if (anim->_currentLoopCount == -1)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdSnd() {
	int id = _data->readByte();
	_vm->_sound->playSound(id);
}

void Scripts::cmdRetNeg() {
	_endFlag = true;
	_returnCode = -1;
}

void Scripts::cmdCheckLoc() {
	int minX = _data->readUint16LE();
	int minY = _data->readUint16LE();
	int maxX = _data->readUint16LE();
	int maxY = _data->readUint16LE();

	int curX = _vm->_player->_rawPlayer.x + _vm->_player->_playerOffset.x;
	int curY = _vm->_player->_rawPlayer.y;

	if ((curX >= minX) && (curX <= maxX) && (curY >= minY) && (curY <= maxY))
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::cmdSetAnim() { 
	int animId = _data->readByte();
	Animation *anim = _vm->_animation->setAnimation(animId);
	_vm->_animation->setAnimTimer(anim);
}

void Scripts::cmdDispInv() {
	_vm->_inventory->newDisplayInv();
}

void Scripts::CMDSETTIMER() { error("TODO CMDSETTIMER"); }
void Scripts::CMDCHECKTIMER() { error("TODO CMDCHECKTIMER"); }

void Scripts::cmdSetTravel() {
	if (_vm->_room->_selectCommand == 5)
		cmdGoto();
	else
		_data->skip(2);
}

void Scripts::CMDSETVID() { error("TODO CMDSETVID"); }
void Scripts::CMDPLAYVID() { error("TODO CMDPLAYVID"); }

void Scripts::cmdPlotImage() {
	_vm->_destIn = _vm->_current;

	int destX = _data->readUint16LE();
	int destY = _data->readUint16LE();
	int objId = _data->readUint16LE();
	int imgId = _data->readUint16LE();

	_vm->_screen->plotImage(_vm->_objectsTable[objId], imgId, Common::Point(destX, destY));
}

void Scripts::cmdSetDisplay() {
	_vm->_screen->setDisplayScan();
	_vm->_current = _vm->_screen;
}

void Scripts::cmdSetBuffer() {
	_vm->_current = &_vm->_buffer2;
}

void Scripts::cmdSetScroll() {
	_vm->_screen->_scrollCol = _data->readUint16LE();
	_vm->_screen->_scrollRow = _data->readUint16LE();
	_vm->_screen->_scrollX = 0;
	_vm->_screen->_scrollY = 0;
}

void Scripts::CMDSAVERECT() { error("TODO CMDSAVERECT"); }
void Scripts::CMDSETBUFVID() { error("TODO CMDSETBUFVID"); }
void Scripts::CMDPLAYBUFVID() { error("TODO CMDPLAYBUFVID"); }

void Scripts::cmdRemoveLast() {
	--_vm->_numAnimTimers;
}

void Scripts::CMDSPECIAL() { 
	_specialFunction = _data->readUint16LE();
	int p1 = _data->readUint16LE();
	int p2 = _data->readUint16LE();

	if (_specialFunction == 1) {
		if (_vm->_establishTable[p2] == 1)
			return;

		_vm->_screen->savePalette();
	}

	executeSpecial(_specialFunction, p1, p2);

	if (_specialFunction == 1) {
		_vm->_screen->restorePalette();
		_vm->_room->_function = 3;
	}
}

void Scripts::CMDSETCYCLE() { error("TODO CMDSETCYCLE"); }
void Scripts::CMDCYCLE() { error("TODO CMDCYCLE"); }
void Scripts::CMDCHARSPEAK() { error("TODO CMDCHARSPEAK"); }
void Scripts::CMDTEXSPEAK() { error("TODO CMDTEXSPEAK"); }
void Scripts::CMDTEXCHOICE() { error("TODO CMDTEXCHOICE"); } // _choiceStart = _data->pos() - 1;
void Scripts::CMDWAIT() { error("TODO CMDWAIT"); }
void Scripts::CMDSETCONPOS() { error("TODO CMDSETCONPOS"); }
void Scripts::CMDCHECKVFRAME() { error("TODO CMDCHECKVFRAME"); }

void Scripts::cmdJumpChoice() {
	int val = (_data->readUint16LE() && 0xFF);
	
	if (val == _choice) {
		_sequence = _data->readUint16LE();
		searchForSequence();
	} else
		_data->skip(2);
}

void Scripts::cmdReturnChoice() {
	_data->seek(_choiceStart);
}

void Scripts::cmdClearBlock() {
	_vm->_screen->restoreBlock();
}

void Scripts::CMDLOADSOUND() { error("TODO CMDLOADSOUND"); }
void Scripts::CMDFREESOUND() { error("TODO CMDFREESOUND"); }
void Scripts::CMDSETVIDSND() { error("TODO CMDSETVIDSND"); }
void Scripts::CMDPLAYVIDSND() { error("TODO CMDPLAYVIDSND"); }
void Scripts::CMDPUSHLOCATION() { error("TODO CMDPUSHLOCATION"); }

void Scripts::cmdPlayerOff() {
	_vm->_player->_playerOff = true;
}

void Scripts::cmdPlayerOn() {
	_vm->_player->_playerOff = false;
}

void Scripts::CMDDEAD() { error("TODO CMDDEAD"); }

void Scripts::cmdFadeOut() {
	_vm->_screen->forceFadeOut();
}

void Scripts::CMDENDVID() { error("TODO CMDENDVID"); }
void Scripts::CMDHELP() { error("TODO CMDHELP"); }
void Scripts::CMDCYCLEBACK() { error("TODO CMDCYCLEBACK"); }
void Scripts::CMDCHAPTER() { error("TODO CMDCHAPTER"); }

void Scripts::cmdSetHelp() {
	int arrayId = (_data->readUint16LE() && 0xFF) - 1;
	int helpId = _data->readUint16LE() && 0xFF;

	byte *help = _vm->_helpTbl[arrayId];
	help[helpId] = 1;

	if (_vm->_useItem == 0) {
		_sequence = 11000;
		searchForSequence();
	}
}

void Scripts::cmdCenterPanel() {
	if (_vm->_screen->_vesaMode) {
		_vm->_screen->clearScreen();
		_vm->_screen->setPanel(3);
	}
}

void Scripts::cmdMainPanel() { 
	if (_vm->_screen->_vesaMode) {
		_vm->_room->init4Quads();
		_vm->_screen->setPanel(0);
	}
}

void Scripts::CMDRETFLASH() { error("TODO CMDRETFLASH"); }


} // End of namespace Access
