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

#ifndef ACCESS_ACCESS_H
#define ACCESS_ACCESS_H

#include "common/scummsys.h"
#include "common/system.h"
#include "common/error.h"
#include "common/random.h"
#include "common/util.h"
#include "engines/engine.h"
#include "graphics/surface.h"
#include "access/animation.h"
#include "access/bubble_box.h"
#include "access/char.h"
#include "access/data.h"
#include "access/debugger.h"
#include "access/events.h"
#include "access/files.h"
#include "access/font.h"
#include "access/inventory.h"
#include "access/player.h"
#include "access/room.h"
#include "access/screen.h"
#include "access/scripts.h"
#include "access/sound.h"
#include "access/video.h"

/**
 * This is the namespace of the Access engine.
 *
 * Status of this engine: In Development
 *
 * Games using this engine:
 * - Amazon: Guardians of Eden
 */
namespace Access {

#define DEBUG_BASIC 1
#define DEBUG_INTERMEDIATE 2
#define DEBUG_DETAILED 3

enum {
	GType_Amazon = 1,
	GType_MartianMemorandum = 2,
	GType_Noctropolis = 3
};

enum AccessDebugChannels {
	kDebugPath      = 1 << 0,
	kDebugScripts	= 1 << 1,
	kDebugGraphics	= 1 << 2
};

struct AccessGameDescription;

extern const char *const _estTable[];

class AccessEngine : public Engine {
private:
	uint32 _lastTime, _curTime;

	/**
	 * Handles basic initialisation
	 */
	void initialize();

	/**
	 * Set VGA mode
	 */
	void setVGA();

	void dummyLoop();

	void speakText(int idx);

	void doEstablish(int esatabIndex, int sub);
protected:
	const AccessGameDescription *_gameDescription;
	Common::RandomSource _randomSource;

	/**
	 * Main handler for showing game rooms
	 */
	void doRoom();

	// Engine APIs
	virtual Common::Error run();
	virtual bool hasFeature(EngineFeature f) const;
protected:
	/**
	 * Play the game
	 */
	virtual void playGame() = 0;
public:
	AnimationManager *_animation;
	BubbleBox *_bubbleBox;
	CharManager *_char;
	Debugger *_debugger;
	EventsManager *_events;
	FileManager *_files;
	InventoryManager *_inventory;
	Player *_player;
	Room *_room;
	Screen *_screen;
	Scripts *_scripts;
	SoundManager *_sound;
	VideoPlayer *_video;

	ASurface *_destIn;
	ASurface *_current;
	ASurface _buffer1;
	ASurface _buffer2;
	Common::Array<CharEntry *> _charTable;
	SpriteResource *_objectsTable[100];
	bool _establishTable[100];
	bool _establishFlag;
	int _establishMode;
	int _establishGroup;
	int _establishCtrlTblOfs;
	int _numAnimTimers;
	TimerList _timers;
	FontManager _fonts;
	Common::Array<Common::Rect> _newRects;
	Common::Array<Common::Rect> _oldRects;
	Common::Array<ExtraCell> _extraCells;
	ImageEntryList _images;
	int _pCount;
	bool _normalMouse;
	int _mouseMode;

	int _currentManOld;
	byte *_inactive;
	byte *_music;
	byte *_title;
	int _converseMode;
	int _startAboutBox;
	int _startTravelBox;
	bool _currentCharFlag;
	bool _boxSelect;
	int _scale;
	int _scaleH1, _scaleH2;
	int _scaleN1;
	int _scaleT1;
	int _scaleMaxY;
	int _scaleI;
	bool _scaleFlag;

	byte *_eseg;
	int _et;
	int _printEnd;
	int _txtPages;
	int _narateFile;
	int _sndSubFile;
	int _countTbl[6];

	// Fields that are included in savegames
	int _conversation;
	int _currentMan;
	uint32 _newTime;
	uint32 _newDate;
	int _intTim[3];
	int _timer[3];
	int _flags[256];
	byte _help1[366];
	byte _help2[366];
	byte _help3[366];
	byte *_helpTbl[3];
	int _travel;
	int _ask;
	int _rScrollRow;
	int _rScrollCol;
	int _rScrollX;
	int _rScrollY;
	int _rOldRectCount;
	int _rNewRectCount;
	int _rKeyFlag;
	int _mapOffset;
	int _screenVirtX;

	// Fields mapped into the flags array
	int &_useItem;
	int &_startup;
	int &_manScaleOff;

public:
	AccessEngine(OSystem *syst, const AccessGameDescription *gameDesc);
	virtual ~AccessEngine();

	uint32 getFeatures() const;
	bool isCD() const;
	Common::Language getLanguage() const;
	Common::Platform getPlatform() const;
	uint16 getVersion() const;
	uint32 getGameID() const;
	uint32 getGameFeatures() const;

	int getRandomNumber(int maxNumber);

	void loadCells(Common::Array<CellIdent> &cells);

	/**
	* Free the sprites list
	*/
	void freeCells();

	/**
	 * Free the inactive data
	 */
	void freeInactiveData();

	void AccessEngine::loadEstablish(int sub);

	void establish(int esatabIndex, int sub);

	void establishCenter(int esatabIndex, int sub);

	void plotList();
	void plotList1();

	void copyBlocks();

	void copyRects();

	void copyBF1BF2();

	void copyBF2Vid();

	void doLoadSave();

	void freeChar();
};

} // End of namespace Access

#endif /* ACCESS_ACCESS_H */
