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

#include "neverhood/resource.h"
#include "neverhood/resourceman.h"

namespace Neverhood {

// SpriteResource

SpriteResource::SpriteResource(NeverhoodEngine *vm)
	 : _vm(vm), _resourceHandle(-1), _pixels(NULL) {
}

SpriteResource::~SpriteResource() {
	unload();
}

void SpriteResource::draw(byte *dest, int destPitch, bool flipX, bool flipY) {
	if (_pixels) {
		if (_rle) {
			unpackSpriteRle(_pixels, _dimensions.width, _dimensions.height, dest, destPitch, flipX, flipY);
		} else {
			unpackSpriteNormal(_pixels, _dimensions.width, _dimensions.height, dest, destPitch, flipX, flipY);
		}
	}
}

bool SpriteResource::load(uint32 fileHash) {
	debug("SpriteResource::load(%08X)", fileHash);
	// TODO: Later merge with load2 and make the mode a parameter
	unload();
	_resourceHandle = _vm->_res->useResource(fileHash);
	debug("SpriteResource::load(0x%08X) _resourceHandle = %d", fileHash, _resourceHandle);
	if (_resourceHandle != -1) {
		if (_vm->_res->getResourceType(_resourceHandle) == 2) {
			byte *spriteData = _vm->_res->loadResource(_resourceHandle, true);
			parseBitmapResource(spriteData, &_rle, &_dimensions, NULL, NULL, &_pixels);
		} else {
			_vm->_res->unuseResource(_resourceHandle);
			_resourceHandle = -1;
		}
	} 
	return _pixels != NULL;
}

bool SpriteResource::load2(uint32 fileHash) {
	unload();
	_resourceHandle = _vm->_res->useResource(fileHash);
	if (_resourceHandle != -1) {
		if (_vm->_res->getResourceType(_resourceHandle) == 2) {
			byte *spriteData = _vm->_res->loadResource(_resourceHandle, true);
			parseBitmapResource(spriteData, &_rle, &_dimensions, &_position, NULL, &_pixels);
		} else {
			_vm->_res->unuseResource(_resourceHandle);
			_resourceHandle = -1;
		}
	} 
	return _pixels != NULL;
}

void SpriteResource::unload() {
	if (_resourceHandle != -1) {
		_vm->_res->unloadResource(_resourceHandle);
		_vm->_res->unuseResource(_resourceHandle);
		_resourceHandle = -1;
	} else {
		delete[] _pixels;
	}
	_pixels = NULL;
	_rle = false;
}

// PaletteResource

PaletteResource::PaletteResource(NeverhoodEngine *vm)
	: _vm(vm), _resourceHandle(-1), _palette(NULL) {
}

PaletteResource::~PaletteResource() {
	unload();
}

bool PaletteResource::load(uint32 fileHash) {
	debug("PaletteResource::load(%08X)", fileHash);
	unload();
	_resourceHandle = _vm->_res->useResource(fileHash);
	if (_resourceHandle != -1) {
		_palette = _vm->_res->loadResource(_resourceHandle, true);
		switch (_vm->_res->getResourceType(_resourceHandle)) {
		case 2:
			// Palette is stored in a bitmap
			parseBitmapResource(_palette, NULL, NULL, NULL, &_palette, NULL);
			break;
		case 3:
			// _palette already points to the correct data
			break;
		default:						
			_vm->_res->unuseResource(_resourceHandle);
			_resourceHandle = -1;
			break;
		}
	} 
	return _palette != NULL;
}

void PaletteResource::unload() {
	if (_resourceHandle != -1) {
		_vm->_res->unloadResource(_resourceHandle);
		_vm->_res->unuseResource(_resourceHandle);
		_resourceHandle = -1;
		_palette = NULL;
	}
}

void PaletteResource::copyPalette(byte *destPalette) {
	if (_palette) {
		memcpy(destPalette, _palette, 1024);
	} else {
		// TODO?: buildDefaultPalette(palette);
	}
}

// AnimResource

AnimResource::AnimResource(NeverhoodEngine *vm)
	: _vm(vm), _width(0), _height(0), _currSpriteData(NULL) {
	
	clear();
	clear2();
}

AnimResource::~AnimResource() {
	unloadInternal();
}

bool AnimResource::load(uint32 fileHash) {
	debug("AnimResource::load(%08X)", fileHash);

	if (fileHash == _fileHash)
		return true;

	unload();
	_resourceHandle = _vm->_res->useResource(fileHash);
	if (_resourceHandle == -1)
		return false;
	
	byte *resourceData, *animList, *frameList;
	uint16 animInfoStartOfs, animListIndex, animListCount;
	uint16 frameListStartOfs, frameCount;
	uint32 spriteDataOfs, paletteDataOfs;

	if (_vm->_res->getResourceType(_resourceHandle) != 4) {
		_vm->_res->unuseResource(_resourceHandle);
		_resourceHandle = -1;
		return false;
	}
	
	resourceData = _vm->_res->loadResource(_resourceHandle);
	if (!resourceData) {
		_vm->_res->unuseResource(_resourceHandle);
		_resourceHandle = -1;
		return false;
	}

	animListCount = READ_LE_UINT16(resourceData);
	animInfoStartOfs = READ_LE_UINT16(resourceData + 2);
	spriteDataOfs = READ_LE_UINT32(resourceData + 4);
	paletteDataOfs = READ_LE_UINT32(resourceData + 8);
	
	animList = resourceData + 12;
	for (animListIndex = 0; animListIndex < animListCount; animListIndex++) {
		debug("hash: %08X", READ_LE_UINT32(animList));
		if (READ_LE_UINT32(animList) == fileHash)
			break;
		animList += 8;				
	}
	
	if (animListIndex >= animListCount) {
		_vm->_res->unloadResource(_resourceHandle);
		_vm->_res->unuseResource(_resourceHandle);
		_resourceHandle = -1;
		return false;
	}

	_spriteData = resourceData + spriteDataOfs;
	if (paletteDataOfs > 0)
		_paletteData = resourceData + paletteDataOfs;
	
	frameCount = READ_LE_UINT16(animList + 4);
	frameListStartOfs = READ_LE_UINT16(animList + 6);
	
	debug("frameCount = %d; frameListStartOfs = %04X; animInfoStartOfs = %04X", frameCount, frameListStartOfs, animInfoStartOfs);
	
	frameList = resourceData + animInfoStartOfs + frameListStartOfs;

	_frames.clear();
	_frames.reserve(frameCount);

	for (uint16 frameIndex = 0; frameIndex < frameCount; frameIndex++) {
		AnimFrameInfo frameInfo;
		frameInfo.frameHash = READ_LE_UINT32(frameList);
		frameInfo.counter = READ_LE_UINT16(frameList + 4);
		frameInfo.rect.x1 = READ_LE_UINT16(frameList + 6);
		frameInfo.rect.y1 = READ_LE_UINT16(frameList + 8);
		frameInfo.rect.x2 = READ_LE_UINT16(frameList + 10);
		frameInfo.rect.y2 = READ_LE_UINT16(frameList + 12);
		frameInfo.deltaX = READ_LE_UINT16(frameList + 14);
		frameInfo.deltaY = READ_LE_UINT16(frameList + 16);
		frameInfo.deltaRect.x = READ_LE_UINT16(frameList + 18); 
		frameInfo.deltaRect.y = READ_LE_UINT16(frameList + 20); 
		frameInfo.deltaRect.width = READ_LE_UINT16(frameList + 22); 
		frameInfo.deltaRect.height = READ_LE_UINT16(frameList + 24);
		frameInfo.field_1A = READ_LE_UINT16(frameList + 26); 
		frameInfo.spriteDataOffs = READ_LE_UINT32(frameList + 28);
		debug("frameHash = %08X; counter = %d; rect = (%d,%d,%d,%d); deltaX = %d; deltaY = %d; deltaRect = (%d,%d,%d,%d); field_1A = %04X; spriteDataOffs = %08X", 
			frameInfo.frameHash, frameInfo.counter, 
			frameInfo.rect.x1, frameInfo.rect.y1, frameInfo.rect.x2, frameInfo.rect.y2,
			frameInfo.deltaX, frameInfo.deltaY,
			frameInfo.deltaRect.x, frameInfo.deltaRect.y, frameInfo.deltaRect.width, frameInfo.deltaRect.height,
			frameInfo.field_1A, frameInfo.spriteDataOffs);
		frameList += 32;
		_frames.push_back(frameInfo);
	}
	
	_fileHash = fileHash;
	
	return true;
	
}

void AnimResource::unload() {
	if (_resourceHandle != -1) {
		_vm->_res->unloadResource(_resourceHandle);
		_vm->_res->unuseResource(_resourceHandle);
		clear();
	}
}

void AnimResource::clear() {
	_resourceHandle = -1;
	// _count = 0;
	// _infoStructs = NULL;
	_currSpriteData = NULL;
	_fileHash = 0;
	_paletteData = NULL;
	_spriteData = NULL;
}

void AnimResource::clear2() {
	clear();
	_replEnabled = true;
	_replOldByte = 0;
	_replNewByte = 0;
}

bool AnimResource::loadInternal(uint32 fileHash) {
	unloadInternal();
	return load(fileHash);
}

void AnimResource::unloadInternal() {
	unload();
	clear2();
}

int16 AnimResource::getFrameIndex(uint32 frameHash) {
	for (uint i = 0; i < _frames.size(); i++)
		if (_frames[i].frameHash == frameHash)
			return (int16)i;
	return -1;			
}

} // End of namespace Neverhood
