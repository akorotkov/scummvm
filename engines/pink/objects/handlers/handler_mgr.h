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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef PINK_HANDLER_MGR_H
#define PINK_HANDLER_MGR_H

#include <engines/pink/objects/object.h>
#include <common/array.h>

namespace Pink {

class InventoryItem;
class InventoryMgr;

class Handler;
class HandlerLeftClick;
class HandlerUseClick;
class HandlerTimer;

class Actor;

class HandlerMgr : public Object {
public:
    virtual ~HandlerMgr();
    virtual void deserialize(Archive &archive);

    virtual void toConsole();

    bool isLeftClickHandler(Actor *actor);

    void onTimerMessage(Actor *actor);
    bool onLeftClickMessage(Actor *actor);
    bool onUseClickMessage(Actor *actor, InventoryItem *item, InventoryMgr *mgr);

private:
    Handler *findSuitableHandlerTimer(Actor *actor);
    Handler *findSuitableHandlerLeftClick(Actor *actor);
    Handler *findSuitableHandlerUseClick(Actor *actor);

    Common::Array<HandlerLeftClick*> _leftClickHandlers;
    Common::Array<HandlerUseClick*> _useClickHandlers;
    Common::Array<HandlerTimer*> _timerHandlers;
};

}

#endif
