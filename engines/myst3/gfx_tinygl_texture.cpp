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

#include "engines/myst3/gfx_tinygl_texture.h"

namespace Myst3 {

// From Bit Twiddling Hacks
static uint32 upperPowerOfTwo(uint32 v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

TinyGLTexture::TinyGLTexture(const Graphics::Surface *surface, bool nonPoTSupport) {
	width = surface->w;
	height = surface->h;
	format = surface->format;

	// Pad the textures if non power of two support is unavailable
	if (nonPoTSupport) {
		internalHeight = height;
		internalWidth = width;
	} else {
		internalHeight = upperPowerOfTwo(height);
		internalWidth = upperPowerOfTwo(width);
	}

	if (format.bytesPerPixel == 4) {
		internalFormat = TGL_RGBA;
		sourceFormat = TGL_UNSIGNED_BYTE;
	} else if (format.bytesPerPixel == 2) {
		internalFormat = TGL_RGB;
		sourceFormat = TGL_UNSIGNED_BYTE; // UNSIGNED_SHORT_5_6_5 not provided
	} else
		error("Unknown pixel format");

	tglGenTextures(1, &id);
	tglBindTexture(TGL_TEXTURE_2D, id);
	tglTexImage2D(TGL_TEXTURE_2D, 0, internalFormat, internalWidth, internalHeight, 0, internalFormat, sourceFormat, 0);
	tglTexParameteri(TGL_TEXTURE_2D, TGL_TEXTURE_MIN_FILTER, TGL_LINEAR);
	tglTexParameteri(TGL_TEXTURE_2D, TGL_TEXTURE_MAG_FILTER, TGL_LINEAR);

	// NOTE: TinyGL doesn't have issues with white lines so doesn't need use TGL_CLAMP_TO_EDGE
	tglTexParameteri(TGL_TEXTURE_2D, TGL_TEXTURE_WRAP_S, TGL_REPEAT);
	tglTexParameteri(TGL_TEXTURE_2D, TGL_TEXTURE_WRAP_T, TGL_REPEAT);

	update(surface);
}

TinyGLTexture::~TinyGLTexture() {
	tglDeleteTextures(1, &id);
	buffer.free();
}

void TinyGLTexture::update(const Graphics::Surface *surface) {
	tglBindTexture(TGL_TEXTURE_2D, id);
	tglTexImage2D(TGL_TEXTURE_2D, 0, internalFormat, internalWidth, internalHeight, 0,
			internalFormat, sourceFormat, const_cast<void *>(surface->getPixels())); // TESTME: Not sure if it works.

	buffer.free();
	buffer = Graphics::PixelBuffer(surface->format, surface->w * surface->h, DisposeAfterUse::NO);
	memcpy(buffer.getRawBuffer(), const_cast<void *>(surface->getPixels()), surface->w * surface->h * surface->format.bytesPerPixel);
}

} // End of namespace Myst3
