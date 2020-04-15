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

#include "ultima/ultima8/audio/midi_player.h"
#include "ultima/ultima8/ultima8.h"
#include "audio/midiparser.h"

namespace Ultima {
namespace Ultima8 {

byte MidiPlayer::_callbackData[2];

MidiPlayer::MidiPlayer() {
	MidiPlayer::createDriver();
	MidiDriver::DeviceHandle dev = MidiDriver::detectDevice(MDT_MIDI | MDT_ADLIB | MDT_PREFER_GM);
	_isFMSynth = MidiDriver::getMusicType(dev) == MT_ADLIB;

	if (_driver) {
		int retValue = _driver->open();
		if (retValue == 0) {
			if (_nativeMT32)
				_driver->sendMT32Reset();
			else
				_driver->sendGMReset();

			_driver->setTimerCallback(this, &timerCallback);
		}
	}
}

MidiPlayer::~MidiPlayer() {
	_driver->close();
}

void MidiPlayer::play(byte *data, size_t size, int seqNo) {
	if (!_driver)
		return;

	assert(seqNo == 0 || seqNo == 1);

	stop();

	if (size < 4)
		error("play() wrong music resource size");

	if (READ_BE_UINT32(data) != MKTAG('F', 'O', 'R', 'M')) {
		warning("play() Unexpected signature");
		_isPlaying = false;
	} else {
		_parser = MidiParser::createParser_XMIDI(xmidiCallback, _callbackData + seqNo);

		if (!_parser->loadMusic(data, size))
			error("play() wrong music resource");

		_parser->setTrack(0);
		_parser->setMidiDriver(this);
		_parser->setTimerRate(_driver->getBaseTempo());
		_parser->property(MidiParser::mpCenterPitchWheelOnUnload, 1);
		_parser->property(MidiParser::mpSendSustainOffOnNotesOff, 1);

		int volume = g_engine->_mixer->getVolumeForSoundType(Audio::Mixer::kMusicSoundType);
		setVolume(volume);
		_isPlaying = true;
	}
}

void MidiPlayer::setLooping(bool loop) {
	_parser->property(MidiParser::mpAutoLoop, loop);
}

void MidiPlayer::xmidiCallback(byte eventData, void *data) {
	if (data == nullptr)
		return;

	*static_cast<byte*>(data) = eventData;
}


} // End of namespace Ultima8
} // End of namespace Ultima
