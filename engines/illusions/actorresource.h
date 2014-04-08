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

#ifndef ILLUSIONS_ACTORRESOURCE_H
#define ILLUSIONS_ACTORRESOURCE_H

#include "illusions/graphics.h"
#include "illusions/resourcesystem.h"
#include "graphics/surface.h"

namespace Illusions {

class IllusionsEngine;

class ActorResourceLoader : public BaseResourceLoader {
public:
	ActorResourceLoader(IllusionsEngine *vm) : _vm(vm) {}
	virtual ~ActorResourceLoader() {}
	virtual void load(Resource *resource);
	virtual void unload(Resource *resource);
	virtual void buildFilename(Resource *resource);
	virtual bool isFlag(int flag);
protected:
	IllusionsEngine *_vm;
};

struct Frame {
	uint16 _flags;
	byte *_pointsConfig;
	SurfInfo _surfInfo;
	byte *_compressedPixels;
	void load(byte *dataStart, Common::SeekableReadStream &stream);
};

struct Sequence {
	uint32 _sequenceId;
	uint32 _unk4;
	byte *_sequenceCode;
	void load(byte *dataStart, Common::SeekableReadStream &stream);
};

struct ActorType {
	uint32 _actorTypeId;
	SurfInfo _surfInfo;
	byte *_pointsConfig;
	NamedPoints _namedPoints;
	RGB _color;
	byte _scale;
	byte _priority;
	int16 _value1E;
	uint16 _pathWalkPointsIndex;
	uint16 _scaleLayerIndex;
	uint16 _pathWalkRectIndex;
	uint16 _priorityLayerIndex;
	uint16 _regionLayerIndex;
	uint16 _flags;
	void load(byte *dataStart, Common::SeekableReadStream &stream);
};

class FramesList : public Common::Array<Frame> {
};

class ActorResource {
public:
	ActorResource();
	~ActorResource();
	void load(Resource *resource);
	bool containsSequence(Sequence *sequence);
	bool findNamedPoint(uint32 namedPointId, Common::Point &pt);
public:
	uint32 _totalSize;
	Common::Array<ActorType> _actorTypes;
	Common::Array<Sequence> _sequences;
	FramesList _frames;
	NamedPoints _namedPoints;
};

class ActorItem {
public:
	ActorItem(IllusionsEngine *vm);              
	~ActorItem();
	void pause();
	void unpause();
public:
	IllusionsEngine *_vm;
	uint32 _tag;
	int _pauseCtr;
	ActorResource *_actRes;
};

class ActorItems {
public:
	ActorItems(IllusionsEngine *vm);
	~ActorItems();
	ActorItem *allocActorItem();
	void freeActorItem(ActorItem *actorItem);
	void pauseByTag(uint32 tag);
	void unpauseByTag(uint32 tag);
	FramesList *findSequenceFrames(Sequence *sequence);
	ActorItem *findActorByResource(ActorResource *actorResource);
	bool findNamedPoint(uint32 namedPointId, Common::Point &pt);
protected:
	typedef Common::List<ActorItem*> Items;
	typedef Items::iterator ItemsIterator;
	IllusionsEngine *_vm;
	Items _items;
};

} // End of namespace Illusions

#endif // ILLUSIONS_ACTORRESOURCE_H
