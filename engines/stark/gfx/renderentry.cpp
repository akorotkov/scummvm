/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
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

#include "engines/stark/gfx/renderentry.h"

#include "engines/stark/visual/actor.h"
#include "engines/stark/visual/image.h"
#include "engines/stark/visual/smacker.h"
#include "engines/stark/visual/visual.h"

namespace Stark {

RenderEntry::RenderEntry(Resource *owner, const Common::String &name) :
	_visual(nullptr),
	_name(name),
	_owner(owner) {
}

void RenderEntry::update(uint32 delta) {
	if (!_visual) {
		// warning("No visual for render entry '%s'", _name.c_str());
		return;
	}

	VisualActor *actor = _visual->get<VisualActor>();
	if (actor) {
		actor->update(delta);
	}
}

void RenderEntry::render(GfxDriver *gfx) {
	if (!_visual) {
		// warning("No visual for render entry '%s'", _name.c_str());
		return;
	}

	VisualImageXMG *imageXMG = _visual->get<VisualImageXMG>();
	if (imageXMG) {
		imageXMG->render(gfx, _position);
	}

	VisualActor *actor = _visual->get<VisualActor>();
	if (actor) {
		actor->render(gfx);
	}

	VisualSmacker *smacker = _visual->get<VisualSmacker>();
	if (smacker) {
		smacker->render(gfx, _position);
	}
}

void RenderEntry::setVisual(Visual *visual) {
	_visual = visual;
}

void RenderEntry::setPosition(const Common::Point &position) {
	_position = position;
}

} // End of namespace Stark
