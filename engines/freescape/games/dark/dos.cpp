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

#include "common/file.h"

#include "freescape/freescape.h"
#include "freescape/games/dark/dark.h"
#include "freescape/language/8bitDetokeniser.h"

namespace Freescape {

extern byte kEGADefaultPaletteData[16][3];

void DarkEngine::initDOS() {
	if (_renderMode == Common::kRenderEGA)
		_viewArea = Common::Rect(40, 24, 279, 124);
	else
		error("Invalid or unknown render mode");

	_maxEnergy = 79;
	_maxShield = 79;
}

void DarkEngine::loadAssetsDOSDemo() {
	Common::File file;
	if (_renderMode == Common::kRenderEGA) {
		file.open("SCN1E.DAT");
		if (file.isOpen()) {
			_title = load8bitBinImage(&file, 0x0);
			_title->setPalette((byte *)&kEGADefaultPaletteData, 0, 16);
		}
		file.close();
		file.open("DSIDEE.EXE");

		if (!file.isOpen())
			error("Failed to open DSIDEE.EXE");
		loadMessagesFixedSize(&file, 0x4525, 16, 27);
		loadMessagesFixedSize(&file, 0x993f - 2, 308, 5);
		loadFonts(&file, 0xa598);
		loadGlobalObjects(&file, 0x3d04, 23);
		load8bitBinary(&file, 0xa700, 16);
		_border = load8bitBinImage(&file, 0x210);
		_border->setPalette((byte *)&kEGADefaultPaletteData, 0, 16);

		for (auto &it : _areaMap) {
			addWalls(it._value);
			addECDs(it._value);
			addSkanner(it._value);
		}
	} else if (_renderMode == Common::kRenderCGA) {
		file.open("DSIDEC.EXE");

		if (!file.isOpen())
			error("Failed to open DSIDEC.EXE");
		loadFonts(&file, 0xa598);
		load8bitBinary(&file, 0x8a70, 4); // TODO
	} else
		error("Invalid or unsupported render mode %s for Dark Side", Common::getRenderModeDescription(_renderMode));

	_indicators.push_back(loadBundledImage("dark_walk_indicator"));
	_indicators.push_back(loadBundledImage("dark_jet_indicator"));

	_indicators[0]->convertToInPlace(_gfx->_texturePixelFormat, nullptr);
	_indicators[1]->convertToInPlace(_gfx->_texturePixelFormat, nullptr);
}

void DarkEngine::loadAssetsDOSFullGame() {
	Common::File file;
	if (_renderMode == Common::kRenderEGA) {
		file.open("SCN1E.DAT");
		if (file.isOpen()) {
			_title = load8bitBinImage(&file, 0x0);
			_title->setPalette((byte *)&kEGADefaultPaletteData, 0, 16);
		}
		file.close();
		file.open("DSIDEE.EXE");

		if (!file.isOpen())
			error("Failed to open DSIDEE.EXE");

		loadFonts(&file, 0xa113);
		loadMessagesFixedSize(&file, 0x4525, 16, 27);
		loadGlobalObjects(&file, 0x3d04, 23);
		load8bitBinary(&file, 0xa280, 16);
		_border = load8bitBinImage(&file, 0x210);
		_border->setPalette((byte *)&kEGADefaultPaletteData, 0, 16);

		for (auto &it : _areaMap) {
			addWalls(it._value);
			addECDs(it._value);
			addSkanner(it._value);
		}
	} else if (_renderMode == Common::kRenderCGA) {
		file.open("DSIDEC.EXE");

		if (!file.isOpen())
			error("Failed to open DSIDEC.EXE");
		load8bitBinary(&file, 0x7bb0, 4); // TODO
	} else
		error("Invalid or unsupported render mode %s for Dark Side", Common::getRenderModeDescription(_renderMode));

	_indicators.push_back(loadBundledImage("dark_walk_indicator"));
	_indicators.push_back(loadBundledImage("dark_jet_indicator"));

	_indicators[0]->convertToInPlace(_gfx->_texturePixelFormat, nullptr);
	_indicators[1]->convertToInPlace(_gfx->_texturePixelFormat, nullptr);
}

void DarkEngine::drawDOSUI(Graphics::Surface *surface) {
	uint32 color = _renderMode == Common::kRenderCGA ? 1 : 14;
	uint8 r, g, b;

	_gfx->readFromPalette(color, r, g, b);
	uint32 front = _gfx->_texturePixelFormat.ARGBToColor(0xFF, r, g, b);

	color = _currentArea->_usualBackgroundColor;
	if (_gfx->_colorRemaps && _gfx->_colorRemaps->contains(color)) {
		color = (*_gfx->_colorRemaps)[color];
	}

	_gfx->readFromPalette(color, r, g, b);
	uint32 back = _gfx->_texturePixelFormat.ARGBToColor(0xFF, r, g, b);

	int score = _gameStateVars[k8bitVariableScore];
	int ecds = _gameStateVars[kVariableActiveECDs];
	drawStringInSurface(Common::String::format("%04d", int(2 * _position.x())), 199, 137, front, back, surface);
	drawStringInSurface(Common::String::format("%04d", int(2 * _position.z())), 199, 145, front, back, surface);
	drawStringInSurface(Common::String::format("%04d", int(2 * _position.y())), 199, 153, front, back, surface);

	drawStringInSurface(Common::String::format("%02d", int(_angleRotations[_angleRotationIndex])), 71, 168, front, back, surface);
	drawStringInSurface(Common::String::format("%3d", _playerSteps[_playerStepIndex]), 71, 177, front, back, surface);
	drawStringInSurface(Common::String::format("%07d", score), 95, 8, front, back, surface);
	drawStringInSurface(Common::String::format("%3d%%", ecds), 192, 8, front, back, surface);

	int seconds, minutes, hours;
	getTimeFromCountdown(seconds, minutes, hours);
	// TODO: implement binary clock

	Common::String message;
	int deadline;
	getLatestMessages(message, deadline);
	if (deadline <= _countdown) {
		drawStringInSurface(message, 112, 177, back, front, surface);
		_temporaryMessages.push_back(message);
		_temporaryMessageDeadlines.push_back(deadline);
	} else
		drawStringInSurface(_currentArea->_name, 112, 177, front, back, surface);

	int energy = _gameStateVars[k8bitVariableEnergy]; // called fuel in this game
	int shield = _gameStateVars[k8bitVariableShield];

	_gfx->readFromPalette(9, r, g, b);
	uint32 blue = _gfx->_texturePixelFormat.ARGBToColor(0xFF, r, g, b);

	if (shield >= 0) {
		Common::Rect shieldBar;
		shieldBar = Common::Rect(72, 139, 151 - (_maxShield - shield), 146);
		surface->fillRect(shieldBar, front);

		shieldBar = Common::Rect(72, 140, 151 - (_maxShield - shield), 145);
		surface->fillRect(shieldBar, blue);
	}

	if (energy >= 0) {
		Common::Rect energyBar;
		energyBar = Common::Rect(72, 147, 151 - (_maxEnergy - energy), 154);
		surface->fillRect(energyBar, front);

		energyBar = Common::Rect(72, 148, 151 - (_maxEnergy - energy), 153);
		surface->fillRect(energyBar, blue);
	}
	uint32 clockColor = _gfx->_texturePixelFormat.ARGBToColor(0xFF, 0xFF, 0xFF, 0xFF);
	drawBinaryClock(surface, 300, 124, clockColor, back);

	if (!_flyMode)
		surface->copyRectToSurface(*_indicators[0], 160, 136, Common::Rect(_indicators[0]->w, _indicators[0]->h));
	else
		surface->copyRectToSurface(*_indicators[1], 160, 136, Common::Rect(_indicators[1]->w, _indicators[1]->h));
}

} // End of namespace Freescape