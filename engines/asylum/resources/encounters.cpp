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
 * $URL$
 * $Id$
 *
 */

#include "asylum/resources/encounters.h"

#include "asylum/resources/actor.h"
#include "asylum/resources/object.h"
#include "asylum/resources/worldstats.h"

#include "asylum/views/scene.h"

#include "asylum/asylum.h"

#include "common/file.h"

namespace Asylum {

Encounter::Encounter(AsylumEngine *engine) : _vm(engine),
	_index(NULL), _keywordIndex(0), _item(NULL), _objectId1(kObjectNone), _objectId2(kObjectNone), _actorIndex(kActorInvalid),
	_flag1(false), _flag2(false) {

	_messageHandler = new MESSAGE_HANDLER(Encounter, messageHandler, this);

	load();
}

Encounter::~Encounter() {
	delete _messageHandler;

	_item = NULL;

	// Zero-out passed pointers
	_vm = NULL;
}

void Encounter::load() {
	Common::File file;

	// TODO error checks
	file.open("sntrm.dat");

	uint16 _count = file.readUint16LE();

	for (uint i = 0; i < _count; i++)
		_variables.push_back(file.readSint16LE());

	file.seek(2 + _count * 2, SEEK_SET);

	// TODO assert if true
	_anvilStyleFlag = file.readSint16LE();

	int16 _dataCount = file.readSint16LE();

	for (uint8 i = 0; i < _dataCount; i++) {
		EncounterItem item;
		memset(&item, 0, sizeof(EncounterItem));

		item.keywordIndex = file.readSint32LE();
		item.field2       = file.readSint32LE();
		item.scriptResourceId  = (ResourceId)file.readSint32LE();
		for (uint8 j = 0; j < 50; j++) {
			item.array[j] = file.readSint32LE();
		}
		item.value = file.readSint16LE();

		_items.push_back(item);
	}

	file.close();
}

void Encounter::run(int32 encounterIndex, ObjectId objectId1, ObjectId objectId2, ActorIndex actorIndex) {
	// Line: 12/15 :: 0x25 (1, 1584, 1584, 0, 0, 0, 0, 0, 0) // First Encounter
	debugC(kDebugLevelEncounter, "Running Encounter %d", encounterIndex);

	if (!_keywordIndex) {
		_item = &_items[0];
		_keywordIndex = _item->keywordIndex;
	}

	if (encounterIndex < 0)
		return;

	// Original engine saves the main event handler (to be restored later)
	_index = encounterIndex;
	_item = &_items[encounterIndex];
	_objectId1 = objectId1;
	_objectId2 = objectId2;
	_actorIndex = actorIndex;

	if (getWorld()->getObjectById(objectId2))
		getWorld()->getObjectById(objectId2)->stopSound();

	getWorld()->actors[actorIndex]->stopSound();

	setVariable(1, 0);
	setVariable(2, _item->value);

	Actor *player = getScene()->getActor();
	if (player->getStatus() == kActorStatusDisabled) {
		_flag2 = true;
	} else {
		_flag2 = false;
		player->updateStatus(kActorStatusDisabled);
	}

	_flag1 = false;

	// Setup encounter event handler
	_vm->switchMessageHandler(_messageHandler);
}



//////////////////////////////////////////////////////////////////////////
// Message handler
//////////////////////////////////////////////////////////////////////////
void Encounter::messageHandler(const AsylumEvent &evt) {

}

//////////////////////////////////////////////////////////////////////////
// Helpers functions
//////////////////////////////////////////////////////////////////////////
void Encounter::setVariable(uint32 index, int32 val) {
	if (index >= _variables.size())
		error("[Encounter::setVariable] Invalid index (was: %d, max: %d)", index, _variables.size() - 1);

	_variables[index] = val;
}

}
