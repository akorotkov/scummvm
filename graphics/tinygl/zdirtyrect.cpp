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

/*
 * This file is based on, or a modified version of code from TinyGL (C) 1997-1998 Fabrice Bellard,
 * which is licensed under the zlib-license (see LICENSE).
 * It also has modifications by the ResidualVM-team, which are covered under the GPLv2 (or later).
 */

#include "graphics/tinygl/zdirtyrect.h"
#include "graphics/tinygl/zgl.h"
#include "graphics/tinygl/gl.h"
#include "common/debug.h"
#include "common/math.h"

namespace TinyGL {

void GLContext::issueDrawCall(DrawCall *drawCall) {
	if (_enableDirtyRectangles && drawCall->getDirtyRegion().isEmpty())
		return;
	_drawCallsQueue.push_back(drawCall);
}

#if TGL_DIRTY_RECT_SHOW
static void DebugDrawRectangle(Common::Rect rect, int r, int g, int b) {
	GLContext *c = gl_get_context();

	if (rect.left < 0)
		rect.left = 0;
	if (rect.right >= c->fb->xsize)
		rect.right = c->fb->xsize - 1;
	if (rect.top < 0)
		rect.top = 0;
	if (rect.bottom >= c->fb->ysize)
		rect.bottom = c->fb->ysize - 1;

	for(int x = rect.left; x < rect.right; x++) {
		c->fb->writePixel(rect.top * c->fb->xsize + x, 255, r, g, b);
		c->fb->writePixel((rect.bottom - 1) * c->fb->xsize + x, 255, r, g, b);
	}
	for(int y = rect.top; y < rect.bottom; y++) {
		c->fb->writePixel(y * c->fb->xsize + rect.left, 255, r, g, b);
		c->fb->writePixel(y * c->fb->xsize + rect.right - 1, 255, r, g, b);
	}
}
#endif

struct DirtyRectangle {
	Common::Rect rectangle;
	int r, g, b;

	DirtyRectangle() { }
	DirtyRectangle(Common::Rect rect, int red, int green, int blue) {
		rectangle = rect;
		r = red;
		g = green;
		b = blue;
	}
};

void GLContext::disposeResources() {
	// Dispose textures and resources.
	bool allDisposed = true;
	do {
		allDisposed = true;
		for (int i = 0; i < TEXTURE_HASH_TABLE_SIZE; i++) {
			GLTexture *t = shared_state.texture_hash_table[i];
			while (t) {
				if (t->disposed) {
					free_texture(t);
					allDisposed = false;
					break;
				}
				t = t->next;
			}
		}

	} while (allDisposed == false);

	TinyGL::Internal::tglCleanupImages();
}

void GLContext::disposeDrawCallLists() {
	typedef Common::List<DrawCall *>::const_iterator DrawCallIterator;
	for (DrawCallIterator it = _previousFrameDrawCallsQueue.begin(); it != _previousFrameDrawCallsQueue.end(); ++it) {
		delete *it;
	}
	_previousFrameDrawCallsQueue.clear();
	for (DrawCallIterator it = _drawCallsQueue.begin(); it != _drawCallsQueue.end(); ++it) {
		delete *it;
	}
	_drawCallsQueue.clear();
}

static inline void _appendDirtyRectangle(const DrawCall &call, Common::List<DirtyRectangle> &rectangles, int r, int g, int b) {
	Common::Rect dirty_region = call.getDirtyRegion();
	if (rectangles.empty() || dirty_region != rectangles.back().rectangle)
		rectangles.push_back(DirtyRectangle(dirty_region, r, g, b));
}

void GLContext::presentBufferDirtyRects() {
	typedef Common::List<DrawCall *>::const_iterator DrawCallIterator;
	typedef Common::List<DirtyRectangle>::iterator RectangleIterator;

	Common::List<DirtyRectangle> rectangles;

	DrawCallIterator itFrame = _drawCallsQueue.begin();
	DrawCallIterator endFrame = _drawCallsQueue.end();
	DrawCallIterator itPrevFrame = _previousFrameDrawCallsQueue.begin();
	DrawCallIterator endPrevFrame = _previousFrameDrawCallsQueue.end();

	// Compare draw calls.
	for ( ; itPrevFrame != endPrevFrame && itFrame != endFrame;
		++itPrevFrame, ++itFrame) {
			const DrawCall &currentCall = **itFrame;
			const DrawCall &previousCall = **itPrevFrame;

			if (previousCall != currentCall) {
				_appendDirtyRectangle(previousCall, rectangles, 255, 255, 255);
				_appendDirtyRectangle(currentCall, rectangles, 255, 0, 0);
			}
	}

	for ( ; itPrevFrame != endPrevFrame; ++itPrevFrame) {
		_appendDirtyRectangle(**itPrevFrame, rectangles, 255, 255, 255);
	}

	for ( ; itFrame != endFrame; ++itFrame) {
		_appendDirtyRectangle(**itFrame, rectangles, 255, 0, 0);
	}

	// This loop increases outer rectangle coordinates to favor merging of adjacent rectangles.
	for (RectangleIterator it = rectangles.begin(); it != rectangles.end(); ++it) {
		(*it).rectangle.right++;
		(*it).rectangle.bottom++;
	}

	// Merge coalesce dirty rects.
	bool restartMerge;
	do {
		restartMerge = false;
		for (RectangleIterator it1 = rectangles.begin(); it1 != rectangles.end(); ++it1) {
			for (RectangleIterator it2 = rectangles.begin(); it2 != rectangles.end();) {
				if (it1 != it2) {
					if ((*it1).rectangle.intersects((*it2).rectangle)) {
						(*it1).rectangle.extend((*it2).rectangle);
						it2 = rectangles.erase(it2);
						restartMerge = true;
					} else {
						++it2;
					}
				} else {
					++it2;
				}
			}
		}
	} while(restartMerge);

	for (RectangleIterator it1 = rectangles.begin(); it1 != rectangles.end(); ++it1) {
		RectangleIterator it2 = it1;
		it2++;
		while (it2 != rectangles.end()) {
			if ((*it1).rectangle.contains((*it2).rectangle)) {
				it2 = rectangles.erase(it2);
			} else {
				++it2;
			}
		}
	}

	for (RectangleIterator it1 = rectangles.begin(); it1 != rectangles.end(); ++it1) {
		(*it1).rectangle.clip(renderRect);
	}

	if (!rectangles.empty()) {
		// Execute draw calls.
		for (DrawCallIterator it = _drawCallsQueue.begin(); it != _drawCallsQueue.end(); ++it) {
			Common::Rect drawCallRegion = (*it)->getDirtyRegion();
			for (RectangleIterator itRect = rectangles.begin(); itRect != rectangles.end(); ++itRect) {
				Common::Rect dirtyRegion = (*itRect).rectangle;
				if (dirtyRegion.intersects(drawCallRegion)) {
					(*it)->execute(dirtyRegion, true);
				}
			}
		}
#if TGL_DIRTY_RECT_SHOW
		// Draw debug rectangles.
		// Note: white rectangles are rectangle that contained other rectangles
		// blue rectangles are rectangle merged from other rectangles
		// red rectangles are original dirty rects

		bool blendingEnabled = fb->isBlendingEnabled();
		bool alphaTestEnabled = fb->isAlphaTestEnabled();
		fb->enableBlending(false);
		fb->enableAlphaTest(false);

		for (RectangleIterator it = rectangles.begin(); it != rectangles.end(); ++it) {
			DebugDrawRectangle((*it).rectangle, (*it).r, (*it).g, (*it).b);
		}

		fb->enableBlending(blendingEnabled);
		fb->enableAlphaTest(alphaTestEnabled);
#endif
	}

	// Dispose not necessary draw calls.
	for (DrawCallIterator it = _previousFrameDrawCallsQueue.begin(); it !=  _previousFrameDrawCallsQueue.end(); ++it) {
		delete *it;
	}

	_previousFrameDrawCallsQueue = _drawCallsQueue;
	_drawCallsQueue.clear();


	disposeResources();

	_currentAllocatorIndex = (_currentAllocatorIndex + 1) & 0x1;
	_drawCallAllocator[_currentAllocatorIndex].reset();
}

void GLContext::presentBufferSimple() {
	typedef Common::List<DrawCall *>::const_iterator DrawCallIterator;

	for (DrawCallIterator it = _drawCallsQueue.begin(); it != _drawCallsQueue.end(); ++it) {
		(*it)->execute(true);
		delete *it;
	}

	_drawCallsQueue.clear();

	disposeResources();

	_drawCallAllocator[_currentAllocatorIndex].reset();
}

void presentBuffer() {
	GLContext *c = gl_get_context();
	if (c->_enableDirtyRectangles) {
		c->presentBufferDirtyRects();
	} else {
		c->presentBufferSimple();
	}
}

bool DrawCall::operator==(const DrawCall &other) const {
	if (_type == other._type) {
		switch (_type) {
		case DrawCall_Rasterization:
			return *(const RasterizationDrawCall *)this == (const RasterizationDrawCall &)other;
			break;
		case DrawCall_Blitting:
			return *(const BlittingDrawCall *)this == (const BlittingDrawCall &)other;
			break;
		case DrawCall_Clear:
			return *(const ClearBufferDrawCall *)this == (const ClearBufferDrawCall &)other;
			break;
		default:
			return false;
		}
	} else {
		return false;
	}
}


RasterizationDrawCall::RasterizationDrawCall() : DrawCall(DrawCall_Rasterization) {
	GLContext *c = gl_get_context();
	_vertexCount = c->vertex_cnt;
	_vertex = (GLVertex *) Internal::allocateFrame(_vertexCount * sizeof(GLVertex));
	_drawTriangleFront = c->draw_triangle_front;
	_drawTriangleBack = c->draw_triangle_back;
	memcpy(_vertex, c->vertex, sizeof(TinyGL::GLVertex) * _vertexCount);
	_state = captureState();
	if (c->_enableDirtyRectangles) {
		computeDirtyRegion();
	}
}

void RasterizationDrawCall::computeDirtyRegion() {
	int clip_code = 0xf;

	for (int i = 0; i < _vertexCount; i++) {
		clip_code &= _vertex[i].clip_code;
	}

	if (!clip_code) {
		GLContext *c = gl_get_context();
		int xmax = c->fb->getPixelBufferWidth() - 1;
		int ymax = c->fb->getPixelBufferHeight() - 1;
		int left = xmax, right = 0, top = ymax, bottom = 0;
		for (int i = 0; i < _vertexCount; i++) {
			TinyGL::GLVertex *v = &_vertex[i];
			if (v->clip_code)
				c->gl_transform_to_viewport(v);
			left =   MIN(left,   v->clip_code & 0x1 ?    0 : v->zp.x);
			right =  MAX(right,  v->clip_code & 0x2 ? xmax : v->zp.x);
			bottom = MAX(bottom, v->clip_code & 0x4 ? ymax : v->zp.y);
			top =    MIN(top,    v->clip_code & 0x8 ?    0 : v->zp.y);
		}
		// Note: clipping outside of Rect is required despite above clip_code checks,
		// as vertices far on the Z axis will overflow X and/or Y coordinates.
		// This happens in EMI intro, for example.
		_dirtyRegion = Common::Rect(
			MAX(0, left),
			MAX(0, top),
			MIN(right, xmax) + 1,
			MIN(bottom, ymax) + 1
		);
	}
}

void RasterizationDrawCall::execute(bool restoreState) const {
	GLContext *c = gl_get_context();

	RasterizationDrawCall::RasterizationState backupState;
	if (restoreState) {
		backupState = captureState();
	}
	applyState(_state);

	GLVertex *prevVertex = c->vertex;
	int prevVertexCount = c->vertex_cnt;

	c->vertex = _vertex;
	c->vertex_cnt = _vertexCount;
	c->draw_triangle_front = (gl_draw_triangle_func)_drawTriangleFront;
	c->draw_triangle_back = (gl_draw_triangle_func)_drawTriangleBack;

	int n = c->vertex_n;
	int cnt = c->vertex_cnt;

	switch (c->begin_type) {
	case TGL_POINTS:
		for(int i = 0; i < cnt; i++) {
			c->gl_draw_point(&c->vertex[i]);
		}
		break;
	case TGL_LINES:
		for(int i = 0; i < cnt / 2; i++) {
			c->gl_draw_line(&c->vertex[i * 2], &c->vertex[i * 2 + 1]);
		}
		break;
	case TGL_LINE_LOOP:
		c->gl_draw_line(&c->vertex[cnt - 1], &c->vertex[0]);
		// Fall through...
	case TGL_LINE_STRIP:
		for(int i = 0; i < cnt - 1; i++) {
			c->gl_draw_line(&c->vertex[i], &c->vertex[i + 1]);
		}
		break;
	case TGL_TRIANGLES:
		for(int i = 0; i < cnt; i += 3) {
			c->gl_draw_triangle(&c->vertex[i], &c->vertex[i + 1], &c->vertex[i + 2]);
		}
		break;
	case TGL_TRIANGLE_STRIP:
		while (cnt >= 3) {
			// needed to respect triangle orientation
			switch (cnt & 1) {
			case 0:
				c->gl_draw_triangle(&c->vertex[2], &c->vertex[1], &c->vertex[0]);
				break;
			case 1:
				c->gl_draw_triangle(&c->vertex[0], &c->vertex[1], &c->vertex[2]);
				break;
			}
			cnt--;
			c->vertex++;
		}
		break;
	case TGL_TRIANGLE_FAN:
		for(int i = 1; i < cnt; i += 2) {
			c->gl_draw_triangle(&c->vertex[0], &c->vertex[i], &c->vertex[i + 1]);
		}
		break;
	case TGL_QUADS:
		for(int i = 0; i < cnt; i += 4) {
			c->vertex[i + 2].edge_flag = 0;
			c->gl_draw_triangle(&c->vertex[i], &c->vertex[i + 1], &c->vertex[i + 2]);
			c->vertex[i + 2].edge_flag = 1;
			c->vertex[i + 0].edge_flag = 0;
			c->gl_draw_triangle(&c->vertex[i], &c->vertex[i + 2], &c->vertex[i + 3]);
		}
		break;
	case TGL_QUAD_STRIP:
		for( ; n >= 4; n -= 2) {
			c->gl_draw_triangle(&c->vertex[0], &c->vertex[1], &c->vertex[2]);
			c->gl_draw_triangle(&c->vertex[1], &c->vertex[3], &c->vertex[2]);
			for (int i = 0; i < 2; i++) {
				c->vertex[i] = c->vertex[i + 2];
			}
		}
		break;
	case TGL_POLYGON: {
		for (int i = c->vertex_cnt; i >= 3; i--) {
			c->gl_draw_triangle(&c->vertex[i - 1], &c->vertex[0], &c->vertex[i - 2]);
		}
		break;
	}
	default:
		error("glBegin: type %x not handled", c->begin_type);
	}

	c->vertex = prevVertex;
	c->vertex_cnt = prevVertexCount;

	if (restoreState) {
		applyState(backupState);
	}
}

RasterizationDrawCall::RasterizationState RasterizationDrawCall::captureState() const {
	RasterizationState state;
	GLContext *c = gl_get_context();
	state.alphaTest = c->alpha_test_enabled;
	state.sfactor = c->source_blending_factor;
	state.dfactor = c->destination_blending_factor;
	state.enableBlending = c->blending_enabled;
	state.alphaFunc = c->alpha_test_func;
	state.alphaRefValue = c->alpha_test_ref_val;
	state.cullFaceEnabled = c->cull_face_enabled;
	state.beginType = c->begin_type;
	state.colorMask = c->color_mask;
	state.currentFrontFace = c->current_front_face;
	state.currentShadeModel = c->current_shade_model;
	state.depthTest = c->depth_test;
	state.offsetStates = c->offset_states;
	state.offsetFactor = c->offset_factor;
	state.offsetUnits = c->offset_units;
	state.polygonModeBack = c->polygon_mode_back;
	state.polygonModeFront = c->polygon_mode_front;
	state.shadowMode = c->shadow_mode;
	state.texture2DEnabled = c->texture_2d_enabled;
	state.texture = c->current_texture;
	state.wrapS = c->texture_wrap_s;
	state.wrapT = c->texture_wrap_t;
	state.shadowMaskBuf = c->shadow_mask_buf;
	state.shadowColorR = c->shadow_color_r;
	state.shadowColorG = c->shadow_color_g;
	state.shadowColorB = c->shadow_color_b;
	state.depthFunction = c->depth_func;
	state.depthWrite = c->depth_write;
	state.lightingEnabled = c->lighting_enabled;
	state.depthTestEnabled = c->depth_test;
	if (c->current_texture != nullptr)
		state.textureVersion = c->current_texture->versionNumber;

	memcpy(state.viewportScaling, c->viewport.scale._v, sizeof(c->viewport.scale._v));
	memcpy(state.viewportTranslation, c->viewport.trans._v, sizeof(c->viewport.trans._v));

	return state;
}

void RasterizationDrawCall::applyState(const RasterizationDrawCall::RasterizationState &state) const {
	GLContext *c = gl_get_context();
	c->fb->enableBlending(state.enableBlending);
	c->fb->setBlendingFactors(state.sfactor, state.dfactor);
	c->fb->enableAlphaTest(state.alphaTest);
	c->fb->setAlphaTestFunc(state.alphaFunc, state.alphaRefValue);
	c->fb->setDepthFunc(state.depthFunction);
	c->fb->enableDepthWrite(state.depthWrite);
	c->fb->enableDepthTest(state.depthTestEnabled);
	c->fb->setOffsetStates(state.offsetStates);
	c->fb->setOffsetFactor(state.offsetFactor);
	c->fb->setOffsetUnits(state.offsetUnits);
	c->fb->setShadowMaskBuf(state.shadowMaskBuf);
	c->fb->setShadowRGB(state.shadowColorB, state.shadowColorG, state.shadowColorB);

	c->blending_enabled = state.enableBlending;
	c->source_blending_factor = state.sfactor;
	c->destination_blending_factor = state.dfactor;
	c->alpha_test_enabled = state.alphaTest;
	c->alpha_test_func = state.alphaFunc;
	c->alpha_test_ref_val = state.alphaRefValue;
	c->depth_test = state.depthTest;
	c->depth_func = state.depthFunction;
	c->depth_write = state.depthWrite;
	c->offset_states = state.offsetStates;
	c->offset_factor = state.offsetFactor;
	c->offset_units = state.offsetUnits;
	c->shadow_mask_buf = state.shadowMaskBuf;
	c->shadow_color_r = state.shadowColorR;
	c->shadow_color_g = state.shadowColorG;
	c->shadow_color_b = state.shadowColorB;

	c->lighting_enabled = state.lightingEnabled;
	c->cull_face_enabled = state.cullFaceEnabled;
	c->begin_type = state.beginType;
	c->color_mask = state.colorMask;
	c->current_front_face = state.currentFrontFace;
	c->current_shade_model = state.currentShadeModel;
	c->polygon_mode_back = state.polygonModeBack;
	c->polygon_mode_front = state.polygonModeFront;
	c->shadow_mode = state.shadowMode;
	c->texture_2d_enabled = state.texture2DEnabled;
	c->current_texture = state.texture;
	c->texture_wrap_s = state.wrapS;
	c->texture_wrap_t = state.wrapT;

	memcpy(c->viewport.scale._v, state.viewportScaling, sizeof(c->viewport.scale._v));
	memcpy(c->viewport.trans._v, state.viewportTranslation, sizeof(c->viewport.trans._v));
}

void RasterizationDrawCall::execute(const Common::Rect &clippingRectangle, bool restoreState) const {
	TinyGL::GLContext *c = gl_get_context();
	c->fb->setScissorRectangle(clippingRectangle);
	execute(restoreState);
	c->fb->resetScissorRectangle();
}

bool RasterizationDrawCall::operator==(const RasterizationDrawCall &other) const {
	if (_vertexCount == other._vertexCount &&
		_drawTriangleFront == other._drawTriangleFront &&
		_drawTriangleBack == other._drawTriangleBack &&
		_state == other._state) {
		for (int i = 0; i < _vertexCount; i++) {
			if ((_vertex[i] != other._vertex[i])) {
				return false;
			}
		}
		return true;
	}
	return false;
}


BlittingDrawCall::BlittingDrawCall(BlitImage *image, const BlitTransform &transform, BlittingMode blittingMode) : DrawCall(DrawCall_Blitting), _transform(transform), _mode(blittingMode), _image(image) {
	tglIncBlitImageRef(image);
	_blitState = captureState();
	_imageVersion = tglGetBlitImageVersion(image);
	if (gl_get_context()->_enableDirtyRectangles) {
		computeDirtyRegion();
	}
}

BlittingDrawCall::~BlittingDrawCall() {
	tglDeleteBlitImage(_image);
}

void BlittingDrawCall::execute(bool restoreState) const {
	BlittingState backupState;
	if (restoreState) {
		backupState = captureState();
	}
	applyState(_blitState);

	switch (_mode) {
	case BlittingDrawCall::BlitMode_Regular:
		Internal::tglBlit(_image, _transform);
		break;
	case BlittingDrawCall::BlitMode_NoBlend:
		Internal::tglBlitNoBlend(_image, _transform);
		break;
	case BlittingDrawCall::BlitMode_Fast:
		Internal::tglBlitFast(_image, _transform._destinationRectangle.left, _transform._destinationRectangle.top);
		break;
	case BlittingDrawCall::BlitMode_ZBuffer:
		Internal::tglBlitZBuffer(_image, _transform._destinationRectangle.left, _transform._destinationRectangle.top);
		break;
	default:
		break;
	}
	if (restoreState) {
		applyState(backupState);
	}
}

void BlittingDrawCall::execute(const Common::Rect &clippingRectangle, bool restoreState) const {
	Internal::tglBlitSetScissorRect(clippingRectangle);
	execute(restoreState);
	Internal::tglBlitResetScissorRect();
}

BlittingDrawCall::BlittingState BlittingDrawCall::captureState() const {
	BlittingState state;
	TinyGL::GLContext *c = gl_get_context();
	state.enableBlending = c->blending_enabled;
	state.sfactor = c->source_blending_factor;
	state.dfactor = c->destination_blending_factor;
	state.alphaTest = c->alpha_test_enabled;
	state.alphaFunc = c->alpha_test_func;
	state.alphaRefValue = c->alpha_test_ref_val;
	state.depthTestEnabled = c->depth_test;
	return state;
}

void BlittingDrawCall::applyState(const BlittingState &state) const {
	TinyGL::GLContext *c = gl_get_context();
	c->fb->enableBlending(state.enableBlending);
	c->fb->setBlendingFactors(state.sfactor, state.dfactor);
	c->fb->enableAlphaTest(state.alphaTest);
	c->fb->setAlphaTestFunc(state.alphaFunc, state.alphaRefValue);
	c->fb->enableDepthTest(state.depthTestEnabled);

	c->blending_enabled = state.enableBlending;
	c->source_blending_factor = state.sfactor;
	c->destination_blending_factor = state.dfactor;
	c->alpha_test_enabled = state.alphaTest;
	c->alpha_test_func = state.alphaFunc;
	c->alpha_test_ref_val = state.alphaRefValue;
	c->depth_test = state.depthTestEnabled;
}

void BlittingDrawCall::computeDirtyRegion() {
	int blitWidth = _transform._destinationRectangle.width();
	int blitHeight = _transform._destinationRectangle.height();
	if (blitWidth == 0) {
		if (_transform._sourceRectangle.width() != 0) {
			blitWidth = _transform._sourceRectangle.width();
		} else {
			tglGetBlitImageSize(_image, blitWidth, blitHeight);
		}
	}
	if (blitHeight == 0) {
		if (_transform._sourceRectangle.height() != 0) {
			blitHeight = _transform._sourceRectangle.height();
		} else {
			tglGetBlitImageSize(_image, blitWidth, blitHeight);
		}
	}
	if (blitWidth == 0 || blitHeight == 0) {
		_dirtyRegion = Common::Rect();
	} else {
		_dirtyRegion = Common::Rect(
			_transform._destinationRectangle.left,
			_transform._destinationRectangle.top,
			_transform._destinationRectangle.left + blitWidth + 1,
			_transform._destinationRectangle.top + blitHeight + 1
		);
		_dirtyRegion.clip(gl_get_context()->renderRect);
	}
}

bool BlittingDrawCall::operator==(const BlittingDrawCall &other) const {
	return	_mode == other._mode &&
			_image == other._image &&
			_transform == other._transform &&
			_blitState == other._blitState &&
			_imageVersion == tglGetBlitImageVersion(other._image);
}


ClearBufferDrawCall::ClearBufferDrawCall(bool clearZBuffer, int zValue, bool clearColorBuffer, int rValue, int gValue, int bValue)
	: _clearZBuffer(clearZBuffer), _clearColorBuffer(clearColorBuffer), _zValue(zValue), _rValue(rValue), _gValue(gValue), _bValue(bValue), DrawCall(DrawCall_Clear) {
	TinyGL::GLContext *c = gl_get_context();
	if (c->_enableDirtyRectangles) {
		_dirtyRegion = c->renderRect;
	}
}

void ClearBufferDrawCall::execute(bool restoreState) const {
	TinyGL::GLContext *c = gl_get_context();
	c->fb->clear(_clearZBuffer, _zValue, _clearColorBuffer, _rValue, _gValue, _bValue);
}

void ClearBufferDrawCall::execute(const Common::Rect &clippingRectangle, bool restoreState) const {
	TinyGL::GLContext *c = gl_get_context();
	Common::Rect clearRect = clippingRectangle.findIntersectingRect(getDirtyRegion());
	c->fb->clearRegion(clearRect.left, clearRect.top, clearRect.width(), clearRect.height(), _clearZBuffer, _zValue, _clearColorBuffer, _rValue, _gValue, _bValue);
}

bool ClearBufferDrawCall::operator==(const ClearBufferDrawCall &other) const {
	return	_clearZBuffer == other._clearZBuffer &&
			_clearColorBuffer == other._clearColorBuffer &&
			_rValue == other._rValue &&
			_gValue == other._gValue &&
			_bValue == other._bValue &&
			_zValue == other._zValue;
}


bool RasterizationDrawCall::RasterizationState::operator==(const RasterizationState &other) const {
	return	beginType == other.beginType &&
			currentFrontFace == other.currentFrontFace &&
			cullFaceEnabled == other.cullFaceEnabled &&
			colorMask == other.colorMask &&
			depthTest == other.depthTest &&
			offsetStates == other.offsetStates &&
			offsetFactor == other.offsetFactor &&
			offsetUnits == other.offsetUnits &&
			depthFunction == other.depthFunction &&
			depthWrite == other.depthWrite &&
			shadowMode == other.shadowMode &&
			texture2DEnabled == other.texture2DEnabled &&
			currentShadeModel == other.currentShadeModel &&
			polygonModeBack == other.polygonModeBack &&
			polygonModeFront == other.polygonModeFront &&
			lightingEnabled == other.lightingEnabled &&
			enableBlending == other.enableBlending &&
			sfactor == other.sfactor &&
			dfactor == other.dfactor &&
			alphaTest == other.alphaTest &&
			alphaFunc == other.alphaFunc &&
			alphaRefValue == other.alphaRefValue &&
			depthTestEnabled == other.depthTestEnabled &&
			texture == other.texture &&
			textureVersion == texture->versionNumber &&
			shadowMaskBuf == other.shadowMaskBuf &&
			viewportTranslation[0] == other.viewportTranslation[0] &&
			viewportTranslation[1] == other.viewportTranslation[1] &&
			viewportTranslation[2] == other.viewportTranslation[2] &&
			viewportScaling[0] == other.viewportScaling[0] &&
			viewportScaling[1] == other.viewportScaling[1] &&
			viewportScaling[2] == other.viewportScaling[2];
}

void *Internal::allocateFrame(int size) {
	GLContext *c = gl_get_context();
	return c->_drawCallAllocator[c->_currentAllocatorIndex].allocate(size);
}

} // end of namespace TinyGL
