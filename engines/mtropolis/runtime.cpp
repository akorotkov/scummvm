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

#include "mtropolis/runtime.h"
#include "mtropolis/data.h"
#include "mtropolis/vthread.h"
#include "mtropolis/asset_factory.h"
#include "mtropolis/element_factory.h"
#include "mtropolis/modifier_factory.h"
#include "mtropolis/modifiers.h"

#include "common/debug.h"
#include "common/file.h"
#include "common/substream.h"


namespace MTropolis {

class ModifierChildMaterializer : public IStructuralReferenceVisitor {
public:
	ModifierChildMaterializer(Runtime *runtime, ObjectLinkingScope *outerScope);

	void visitChildStructuralRef(Common::SharedPtr<Structural> &structural) override;
	void visitChildModifierRef(Common::SharedPtr<Modifier> &modifier) override;
	void visitWeakStructuralRef(Common::SharedPtr<Structural> &structural) override;
	void visitWeakModifierRef(Common::SharedPtr<Modifier> &modifier) override;

private:
	Runtime *_runtime;
	ObjectLinkingScope *_outerScope;
};

ModifierChildMaterializer::ModifierChildMaterializer(Runtime *runtime, ObjectLinkingScope *outerScope)
	: _runtime(runtime), _outerScope(outerScope) {
}

void ModifierChildMaterializer::visitChildStructuralRef(Common::SharedPtr<Structural> &structural) {
	assert(false);
}

void ModifierChildMaterializer::visitChildModifierRef(Common::SharedPtr<Modifier> &modifier) {
	modifier->materialize(_runtime, _outerScope);
}

void ModifierChildMaterializer::visitWeakStructuralRef(Common::SharedPtr<Structural> &structural) {
	assert(false);
}

void ModifierChildMaterializer::visitWeakModifierRef(Common::SharedPtr<Modifier> &modifier) {
	// Do nothing
}

bool Point16::load(const Data::Point &point) {
	x = point.x;
	y = point.y;

	return true;
}

bool Rect16::load(const Data::Rect &rect) {
	top = rect.top;
	left = rect.left;
	bottom = rect.bottom;
	right = rect.right;

	return true;
}

bool IntRange::load(const Data::IntRange &range) {
	max = range.max;
	min = range.min;

	return true;
}

bool Label::load(const Data::Label &label) {
	id = label.labelID;
	superGroupID = label.superGroupID;

	return true;
}

bool ColorRGB8::load(const Data::ColorRGB16 &color) {
	this->r = (color.red * 510 + 1) / 131070;
	this->g = (color.green * 510 + 1) / 131070;
	this->b = (color.blue * 510 + 1) / 131070;

	return true;
}

MessageFlags::MessageFlags() : relay(true), cascade(true), immediate(true) {
}

DynamicListContainerBase::~DynamicListContainerBase() {
}

void DynamicListDefaultSetter::defaultSet(int32 &value) {
	value = 0;
}

void DynamicListDefaultSetter::defaultSet(double &value) {
	value = 0.0;
}

void DynamicListDefaultSetter::defaultSet(Point16 &value) {
	value.x = 0;
	value.y = 0;
}

void DynamicListDefaultSetter::defaultSet(IntRange &value) {
	value.min = 0;
	value.max = 0;
}

void DynamicListDefaultSetter::defaultSet(bool &value) {
	value = false;
}

void DynamicListDefaultSetter::defaultSet(AngleMagVector &value) {
	value.angleRadians = 0.0;
	value.magnitude = 0.0;
}

void DynamicListDefaultSetter::defaultSet(Label &value) {
	value.id = 0;
	value.superGroupID = 0;
}

void DynamicListDefaultSetter::defaultSet(Event &value) {
	value.eventType = EventIDs::EventID::kNothing;
	value.eventInfo = 0;
}

void DynamicListDefaultSetter::defaultSet(Common::String &value) {
}

void DynamicListDefaultSetter::defaultSet(DynamicList &value) {
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const int32 *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kInteger)
		return false;
	outPtr = &dynValue.getInt();
	return true;
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const double *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kFloat)
		return false;
	outPtr = &dynValue.getFloat();
	return true;
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const Point16 *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kPoint)
		return false;
	outPtr = &dynValue.getPoint();
	return true;
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const IntRange *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kIntegerRange)
		return false;
	outPtr = &dynValue.getIntRange();
	return true;
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const bool *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kBoolean)
		return false;
	outPtr = &dynValue.getBool();
	return true;
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const AngleMagVector *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kVector)
		return false;
	outPtr = &dynValue.getVector();
	return true;
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const Label *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kLabel)
		return false;
	outPtr = &dynValue.getLabel();
	return true;
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const Event *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kEvent)
		return false;
	outPtr = &dynValue.getEvent();
	return true;
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const Common::String *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kString)
		return false;
	outPtr = &dynValue.getString();
	return true;
}

bool DynamicListValueImporter::importValue(const DynamicValue &dynValue, const DynamicList *&outPtr) {
	if (dynValue.getType() != DynamicValueTypes::kBoolean)
		return false;
	outPtr = &dynValue.getList();
	return true;
}

DynamicListContainer<void>::DynamicListContainer() : _size(0) {
}

bool DynamicListContainer<void>::setAtIndex(size_t index, const DynamicValue &dynValue) {
	return true;
}

void DynamicListContainer<void>::setFrom(const DynamicListContainerBase &other) {
	_size = other.getSize(); // ... the only thing we have anyway...
}

const void *DynamicListContainer<void>::getConstArrayPtr() const {
	return nullptr;
}

size_t DynamicListContainer<void>::getSize() const {
	return _size;
}

bool DynamicListContainer<void>::compareEqual(const DynamicListContainerBase &other) const {
	return true;
}

bool DynamicListContainer<VarReference>::setAtIndex(size_t index, const DynamicValue &dynValue) {
	if (dynValue.getType() != DynamicValueTypes::kVariableReference)
		return false;

	size_t requiredSize = index + 1;

	if (_array.size() < requiredSize) {
		size_t prevSize = _array.size();
		_array.resize(requiredSize);
		_strings.resize(requiredSize);

		for (size_t i = prevSize; i < index; prevSize++) {
			_array[i].guid = 0;
		}

		const VarReference &varRef = dynValue.getVarReference();
		_array[index].guid = varRef.guid;
		_strings[index] = *varRef.source;

		rebuildStringPointers();
	} else {
		const VarReference &varRef = dynValue.getVarReference();
		_array[index].guid = varRef.guid;
		_strings[index] = *varRef.source;
	}

	return true;
}

void DynamicListContainer<VarReference>::setFrom(const DynamicListContainerBase &other) {
	const DynamicListContainer<VarReference> &otherTyped = static_cast<const DynamicListContainer<VarReference> &>(other);

	_array = otherTyped._array;
	_strings = otherTyped._strings;
	rebuildStringPointers();
}

const void *DynamicListContainer<VarReference>::getConstArrayPtr() const {
	return &_array;
}

size_t DynamicListContainer<VarReference>::getSize() const {
	return _array.size();
}

bool DynamicListContainer<VarReference>::compareEqual(const DynamicListContainerBase &other) const {
	const DynamicListContainer<VarReference> &otherTyped = static_cast<const DynamicListContainer<VarReference> &>(other);
	return _array == otherTyped._array;
}

void DynamicListContainer<VarReference>::rebuildStringPointers() {
	assert(_strings.size() == _array.size());

	size_t numStrings = _array.size();
	for (size_t i = 0; i < numStrings; i++) {
		_array[i].source = &_strings[i];
	}
}

DynamicList::DynamicList() : _type(DynamicValueTypes::kEmpty), _container(nullptr) {
}

DynamicList::DynamicList(const DynamicList &other) : _type(DynamicValueTypes::kEmpty), _container(nullptr) {
	initFromOther(other);
}

DynamicList::~DynamicList() {
	clear();
}

DynamicValueTypes::DynamicValueType DynamicList::getType() const {
	return _type;
}

const Common::Array<int32> &DynamicList::getInt() const {
	assert(_type == DynamicValueTypes::kInteger);
	return *static_cast<const Common::Array<int32> *>(_container->getConstArrayPtr());
}

const Common::Array<double> &DynamicList::getFloat() const {
	assert(_type == DynamicValueTypes::kFloat);
	return *static_cast<const Common::Array<double> *>(_container->getConstArrayPtr());
}

const Common::Array<Point16> &DynamicList::getPoint() const {
	assert(_type == DynamicValueTypes::kPoint);
	return *static_cast<const Common::Array<Point16> *>(_container->getConstArrayPtr());
}

const Common::Array<IntRange> &DynamicList::getIntRange() const {
	assert(_type == DynamicValueTypes::kIntegerRange);
	return *static_cast<const Common::Array<IntRange> *>(_container->getConstArrayPtr());
}

const Common::Array<AngleMagVector> &DynamicList::getVector() const {
	assert(_type == DynamicValueTypes::kVector);
	return *static_cast<const Common::Array<AngleMagVector> *>(_container->getConstArrayPtr());
}

const Common::Array<Label> &DynamicList::getLabel() const {
	assert(_type == DynamicValueTypes::kLabel);
	return *static_cast<const Common::Array<Label> *>(_container->getConstArrayPtr());
}

const Common::Array<Event> &DynamicList::getEvent() const {
	assert(_type == DynamicValueTypes::kEvent);
	return *static_cast<const Common::Array<Event> *>(_container->getConstArrayPtr());
}

const Common::Array<VarReference> &DynamicList::getVarReference() const {
	assert(_type == DynamicValueTypes::kVariableReference);
	return *static_cast<const Common::Array<VarReference> *>(_container->getConstArrayPtr());
}

const Common::Array<Common::String> &DynamicList::getString() const {
	assert(_type == DynamicValueTypes::kString);
	return *static_cast<const Common::Array<Common::String> *>(_container->getConstArrayPtr());
}

const Common::Array<bool> &DynamicList::getBool() const {
	assert(_type == DynamicValueTypes::kBoolean);
	return *static_cast<const Common::Array<bool> *>(_container->getConstArrayPtr());
}

bool DynamicList::setAtIndex(size_t index, const DynamicValue &value) {
	if (_type != value.getType()) {
		if (_container != nullptr && _container->getSize() != 0)
			return false;
		else {
			clear();
			changeToType(value.getType());
			return _container->setAtIndex(index, value);
		}
	} else {
		return _container->setAtIndex(index, value);
	}
}

DynamicList &DynamicList::operator=(const DynamicList &other) {
	if (this != &other) {
		if (_type == DynamicValueTypes::kList && other._type == DynamicValueTypes::kList) {
			// In this case, one operand may be inside of the other operand, so we need to copy instead of clear
			DynamicList listClone(*this);
			swap(listClone);
		} else {
			clear();
			initFromOther(other);
		}
	}

	return *this;
}

bool DynamicList::operator==(const DynamicList &other) const {
	if (this == &other)
		return true;

	if (_type != other._type)
		return false;

	if (_container == nullptr)
		return other._container == nullptr;

	if (other._container == nullptr)
		return false; // (_container == nullptr)

	return _container->compareEqual(*other._container);
}

void DynamicList::swap(DynamicList &other) {
	if (this == &other)
		return;

	DynamicValueTypes::DynamicValueType tempType = _type;
	_type = other._type;
	other._type = tempType;

	DynamicListContainerBase *tempContainer = _container;
	_container = other._container;
	other._container = tempContainer;
}

bool DynamicList::changeToType(DynamicValueTypes::DynamicValueType type) {
	switch (type) {
	case DynamicValueTypes::kNull:
		_container = new DynamicListContainer<void>();
		break;
	case DynamicValueTypes::kInteger:
		_container = new DynamicListContainer<int32>();
		break;
	case DynamicValueTypes::kFloat:
		_container = new DynamicListContainer<double>();
		break;
	case DynamicValueTypes::kPoint:
		_container = new DynamicListContainer<Point16>();
		break;
	case DynamicValueTypes::kIntegerRange:
		_container = new DynamicListContainer<IntRange>();
		break;
	case DynamicValueTypes::kBoolean:
		_container = new DynamicListContainer<bool>();
		break;
	case DynamicValueTypes::kVector:
		_container = new DynamicListContainer<AngleMagVector>();
		break;
	case DynamicValueTypes::kLabel:
		_container = new DynamicListContainer<Label>();
		break;
	case DynamicValueTypes::kEvent:
		_container = new DynamicListContainer<Event>();
		break;
	case DynamicValueTypes::kVariableReference:
		_container = new DynamicListContainer<VarReference>();
		break;
	case DynamicValueTypes::kIncomingData:
		_container = new DynamicListContainer<void>();
		break;
	case DynamicValueTypes::kString:
		_container = new DynamicListContainer<Common::String>();
		break;
	case DynamicValueTypes::kList:
		_container = new DynamicListContainer<DynamicList>();
		break;
	}

	_type = type;

	return true;
}

void DynamicList::clear() {
	_type = DynamicValueTypes::kEmpty;
	if (_container)
		delete _container;
	_container = nullptr;
}

void DynamicList::initFromOther(const DynamicList &other) {
	assert(_container == nullptr);
	assert(_type == DynamicValueTypes::kEmpty);

	if (other._type != DynamicValueTypes::kEmpty) {
		changeToType(other._type);
		_container->setFrom(*other._container);
	}
}

DynamicValue::DynamicValue() : _type(DynamicValueTypes::kNull) {
}

DynamicValue::DynamicValue(const DynamicValue &other) : _type(DynamicValueTypes::kNull) {
	initFromOther(other);
}

DynamicValue::~DynamicValue() {
	clear();
}

bool DynamicValue::load(const Data::InternalTypeTaggedValue &data, const Common::String &varSource, const Common::String &varString) {
	switch (data.type) {
	case Data::InternalTypeTaggedValue::kNull:
		_type = DynamicValueTypes::kNull;
		break;
	case Data::InternalTypeTaggedValue::kIncomingData:
		_type = DynamicValueTypes::kIncomingData;
		break;
	case Data::InternalTypeTaggedValue::kInteger:
		_type = DynamicValueTypes::kInteger;
		_value.asInt = data.value.asInteger;
		break;
	case Data::InternalTypeTaggedValue::kString:
		_type = DynamicValueTypes::kString;
		_str = varString;
		break;
	case Data::InternalTypeTaggedValue::kPoint:
		_type = DynamicValueTypes::kPoint;
		if (!_value.asPoint.load(data.value.asPoint))
			return false;
		break;
	case Data::InternalTypeTaggedValue::kIntegerRange:
		_type = DynamicValueTypes::kIntegerRange;
		if (!_value.asIntRange.load(data.value.asIntegerRange))
			return false;
		break;
	case Data::InternalTypeTaggedValue::kFloat:
		_type = DynamicValueTypes::kFloat;
		_value.asFloat = data.value.asFloat.toDouble();
		break;
	case Data::InternalTypeTaggedValue::kBool:
		_type = DynamicValueTypes::kBoolean;
		_value.asBool = (data.value.asBool != 0);
		break;
	case Data::InternalTypeTaggedValue::kVariableReference:
		_type = DynamicValueTypes::kVariableReference;
		_value.asVarReference.guid = data.value.asVariableReference.guid;
		_value.asVarReference.source = &_str;
		_str = varSource;
		break;
	case Data::InternalTypeTaggedValue::kLabel:
		_type = DynamicValueTypes::kLabel;
		if (!_value.asLabel.load(data.value.asLabel))
			return false;
		break;
	default:
		assert(false);
		return false;
	}

	return true;
}

bool DynamicValue::load(const Data::PlugInTypeTaggedValue &data) {
	switch (data.type) {
	case Data::PlugInTypeTaggedValue::kNull:
		_type = DynamicValueTypes::kNull;
		break;
	case Data::PlugInTypeTaggedValue::kIncomingData:
		_type = DynamicValueTypes::kIncomingData;
		break;
	case Data::PlugInTypeTaggedValue::kInteger:
		_type = DynamicValueTypes::kInteger;
		_value.asInt = data.value.asInt;
		break;
	case Data::PlugInTypeTaggedValue::kIntegerRange:
		_type = DynamicValueTypes::kIntegerRange;
		if (!_value.asIntRange.load(data.value.asIntRange))
			return false;
		break;
	case Data::PlugInTypeTaggedValue::kFloat:
		_type = DynamicValueTypes::kFloat;
		_value.asFloat = data.value.asFloat.toDouble();
		break;
	case Data::PlugInTypeTaggedValue::kBoolean:
		_type = DynamicValueTypes::kBoolean;
		_value.asBool = (data.value.asBoolean != 0);
		break;
	case Data::PlugInTypeTaggedValue::kEvent:
		_type = DynamicValueTypes::kEvent;
		if (!_value.asEvent.load(data.value.asEvent))
			return false;
		break;
	case Data::PlugInTypeTaggedValue::kLabel:
		_type = DynamicValueTypes::kLabel;
		if (!_value.asLabel.load(data.value.asLabel))
			return false;
		break;
	case Data::PlugInTypeTaggedValue::kString:
		_type = DynamicValueTypes::kString;
		_str = data.str;
		break;
	case Data::PlugInTypeTaggedValue::kVariableReference:
		_type = DynamicValueTypes::kVariableReference;
		_value.asVarReference.guid = data.value.asVarRefGUID;
		_value.asVarReference.source = &_str;
		_str.clear(); // Extra data doesn't seem to correlate to this
		break;
	case Data::PlugInTypeTaggedValue::kPoint:
		_type = DynamicValueTypes::kPoint;
		if (!_value.asPoint.load(data.value.asPoint))
			return false;
		break;
	default:
		assert(false);
		return false;
	}

	return true;
}

DynamicValueTypes::DynamicValueType DynamicValue::getType() const {
	return _type;
}

const int32 &DynamicValue::getInt() const {
	assert(_type == DynamicValueTypes::kInteger);
	return _value.asInt;
}

const double &DynamicValue::getFloat() const {
	assert(_type == DynamicValueTypes::kFloat);
	return _value.asFloat;
}

const Point16 &DynamicValue::getPoint() const {
	assert(_type == DynamicValueTypes::kPoint);
	return _value.asPoint;
}

const IntRange &DynamicValue::getIntRange() const {
	assert(_type == DynamicValueTypes::kIntegerRange);
	return _value.asIntRange;
}

const AngleMagVector &DynamicValue::getVector() const {
	assert(_type == DynamicValueTypes::kVector);
	return _value.asVector;
}

const Label &DynamicValue::getLabel() const {
	assert(_type == DynamicValueTypes::kLabel);
	return _value.asLabel;
}

const Event &DynamicValue::getEvent() const {
	assert(_type == DynamicValueTypes::kEvent);
	return _value.asEvent;
}

const VarReference &DynamicValue::getVarReference() const {
	assert(_type == DynamicValueTypes::kVariableReference);
	return _value.asVarReference;
}

const Common::String &DynamicValue::getString() const {
	assert(_type == DynamicValueTypes::kString);
	return _str;
}

const bool &DynamicValue::getBool() const {
	assert(_type == DynamicValueTypes::kBoolean);
	return _value.asBool;
}

const DynamicList &DynamicValue::getList() const {
	assert(_type == DynamicValueTypes::kList);
	return *_value.asList;
}

void DynamicValue::swap(DynamicValue &other) {
	DynamicValueTypes::DynamicValueType tempType = _type;
	_type = other._type;
	other._type = tempType;

	Common::String tempStr = _str;
	_str = other._str;
	other._str = tempStr;

	ValueUnion tempValue;
	memcpy(&tempValue, &_value, sizeof(ValueUnion));
	memcpy(&_value, &other._value, sizeof(ValueUnion));
	memcpy(&other._value, &tempValue, sizeof(ValueUnion));
}

DynamicValue &DynamicValue::operator=(const DynamicValue &other) {
	if (this != &other) {
		DynamicValue temp(other);
		swap(temp);
	}

	return *this;
}

bool DynamicValue::operator==(const DynamicValue &other) const {
	if (this == &other)
		return true;

	if (_type != other._type)
		return false;

	switch (_type) {
	case DynamicValueTypes::kNull:
		return true;
	case DynamicValueTypes::kInteger:
		return _value.asInt == other._value.asInt;
	case DynamicValueTypes::kFloat:
		return _value.asFloat == other._value.asFloat;
	case DynamicValueTypes::kPoint:
		return _value.asPoint == other._value.asPoint;
	case DynamicValueTypes::kIntegerRange:
		return _value.asIntRange == other._value.asIntRange;
	case DynamicValueTypes::kVector:
		return _value.asVector == other._value.asVector;
	case DynamicValueTypes::kLabel:
		return _value.asLabel == other._value.asLabel;
	case DynamicValueTypes::kEvent:
		return _value.asEvent == other._value.asEvent;
	case DynamicValueTypes::kVariableReference:
		return _value.asVarReference == other._value.asVarReference;
	case DynamicValueTypes::kIncomingData:
		return true;
	case DynamicValueTypes::kString:
		return _str == other._str;
	case DynamicValueTypes::kBoolean:
		return _value.asBool == other._value.asBool;
	case DynamicValueTypes::kList:
		return (*_value.asList) == (*other._value.asList);
	default:
		break;
	}

	assert(false);
	return false;
}

void DynamicValue::clear() {
	if (_type == DynamicValueTypes::kList)
		delete _value.asList;

	_str.clear();
	_type = DynamicValueTypes::kNull;
}

void DynamicValue::initFromOther(const DynamicValue &other) {
	assert(_type == DynamicValueTypes::kNull);

	switch (_type) {
	case DynamicValueTypes::kNull:
	case DynamicValueTypes::kIncomingData:
		break;
	case DynamicValueTypes::kInteger:
		_value.asInt = other._value.asInt;
		break;
	case DynamicValueTypes::kFloat:
		_value.asFloat = other._value.asFloat;
		break;
	case DynamicValueTypes::kPoint:
		_value.asPoint = other._value.asPoint;
		break;
	case DynamicValueTypes::kIntegerRange:
		_value.asIntRange = other._value.asIntRange;
		break;
	case DynamicValueTypes::kVector:
		_value.asVector = other._value.asVector;
		break;
	case DynamicValueTypes::kLabel:
		_value.asLabel = other._value.asLabel;
		break;
	case DynamicValueTypes::kEvent:
		_value.asEvent = other._value.asEvent;
		break;
	case DynamicValueTypes::kVariableReference:
		_value.asVarReference = other._value.asVarReference;
		_str = other._str;
		_value.asVarReference.source = &_str;
		break;
	case DynamicValueTypes::kString:
		_str = other._str;
		break;
	case DynamicValueTypes::kBoolean:
		_value.asBool = other._value.asBool;
		break;
	case DynamicValueTypes::kList:
		_value.asList = new DynamicList(*other._value.asList);
		break;
	default:
		assert(false);
		break;
	}

	_type = other._type;
}

MessengerSendSpec::MessengerSendSpec() : destination(0) {
}

bool MessengerSendSpec::load(const Data::Event &dataEvent, uint32 dataMessageFlags, const Data::InternalTypeTaggedValue &dataLocator, const Common::String &dataWithSource, const Common::String &dataWithString, uint32 dataDestination) {
	messageFlags.relay = ((dataMessageFlags & 0x20000000) == 0);
	messageFlags.cascade = ((dataMessageFlags & 0x40000000) == 0);
	messageFlags.immediate = ((dataMessageFlags & 0x80000000) == 0);

	if (!this->send.load(dataEvent))
		return false;

	if (!this->with.load(dataLocator, dataWithSource, dataWithString))
		return false;

	this->destination = dataDestination;

	return true;
}

bool MessengerSendSpec::load(const Data::PlugInTypeTaggedValue &dataEvent, const MessageFlags &dataMessageFlags, const Data::PlugInTypeTaggedValue &dataWith, uint32 dataDestination) {
	if (dataEvent.type != Data::PlugInTypeTaggedValue::kEvent)
		return false;

	if (!this->send.load(dataEvent.value.asEvent))
		return false;

	if (!this->with.load(dataWith))
		return false;

	this->destination = dataDestination;

	return true;
}

Event Event::create() {
	Event evt;
	evt.eventInfo = 0;
	evt.eventType = EventIDs::kNothing;

	return evt;
}

Event Event::create(EventIDs::EventID eventType, uint32 eventInfo) {
	Event evt;
	evt.eventType = eventType;
	evt.eventInfo = eventInfo;

	return evt;
}

bool Event::load(const Data::Event &data) {
	eventType = static_cast<EventIDs::EventID>(data.eventID);
	eventInfo = data.eventInfo;

	return true;
}

void IPlugInModifierRegistrar::registerPlugInModifier(const char *name, const IPlugInModifierFactoryAndDataFactory *loaderFactory) {
	return this->registerPlugInModifier(name, loaderFactory, loaderFactory);
}

PlugIn::~PlugIn() {
}

ProjectResources::~ProjectResources() {
}

ProjectDescription::ProjectDescription() {
}

ProjectDescription::~ProjectDescription() {
}

void ProjectDescription::addSegment(int volumeID, const char *filePath) {
	SegmentDescription desc;
	desc.volumeID = volumeID;
	desc.filePath = filePath;
	desc.stream = nullptr;

	_segments.push_back(desc);
}

void ProjectDescription::addSegment(int volumeID, Common::SeekableReadStream *stream) {
	SegmentDescription desc;
	desc.volumeID = volumeID;
	desc.stream = stream;

	_segments.push_back(desc);
}

const Common::Array<SegmentDescription> &ProjectDescription::getSegments() const {
	return _segments;
}

void ProjectDescription::addPlugIn(const Common::SharedPtr<PlugIn>& plugIn) {
	_plugIns.push_back(plugIn);
}

const Common::Array<Common::SharedPtr<PlugIn> >& ProjectDescription::getPlugIns() const {
	return _plugIns;
}

void ProjectDescription::setResources(const Common::SharedPtr<ProjectResources> &resources) {
	_resources = resources;
}

const Common::SharedPtr<ProjectResources> &ProjectDescription::getResources() const {
	return _resources;
}


const Common::Array<Common::SharedPtr<Modifier> >& SimpleModifierContainer::getModifiers() const {
	return _modifiers;
}

void SimpleModifierContainer::appendModifier(const Common::SharedPtr<Modifier> &modifier) {
	_modifiers.push_back(modifier);
}


RuntimeObject::RuntimeObject() : _guid(0), _runtimeGUID(0) {
}

RuntimeObject::~RuntimeObject() {
}

uint32 RuntimeObject::getStaticGUID() const {
	return _guid;
}

uint32 RuntimeObject::getRuntimeGUID() const {
	return _runtimeGUID;
}

void RuntimeObject::setRuntimeGUID(uint32 runtimeGUID) {
	_runtimeGUID = runtimeGUID;
}


MessageProperties::MessageProperties(const Event &evt, const DynamicValue &value, const Common::WeakPtr<RuntimeObject> &source)
	: _evt(evt), _value(value), _source(source) {
}

const Event &MessageProperties::getEvent() const {
	return _evt;
}

const DynamicValue& MessageProperties::getValue() const {
	return _value;
}

const Common::WeakPtr<RuntimeObject>& MessageProperties::getSource() const {
	return _source;
}

Structural::Structural() : _parent(nullptr) {
}

Structural::~Structural() {
}

ProjectPresentationSettings::ProjectPresentationSettings() : width(640), height(480), bitsPerPixel(8) {
}

const Common::Array<Common::SharedPtr<Structural> > &Structural::getChildren() const {
	return _children;
}

void Structural::addChild(const Common::SharedPtr<Structural>& child) {
	_children.push_back(child);
	child->setParent(this);
}

void Structural::removeAllChildren() {
	_children.clear();
}

void Structural::removeAllModifiers() {
	_modifiers.clear();
}

void Structural::removeChild(Structural* child) {
	for (size_t i = 0; i < _children.size(); i++) {
		if (_children[i].get() == child) {
			_children.remove_at(i);
			return;
		}
	}
}

Structural* Structural::getParent() const {
	return _parent;
}

void Structural::setParent(Structural *parent) {
	_parent = parent;
}

const Common::String &Structural::getName() const {
	return _name;
}

const Common::Array<Common::SharedPtr<Modifier> > &Structural::getModifiers() const {
	return _modifiers;
}

void Structural::appendModifier(const Common::SharedPtr<Modifier> &modifier) {
	_modifiers.push_back(modifier);
}

bool Structural::respondsToEvent(const Event &evt) const {
	return false;
}

VThreadState Structural::consumeMessage(Runtime *runtime, const Common::SharedPtr<MessageProperties> &msg) const {
	assert(false);
	return kVThreadError;
}

void Structural::materializeSelfAndDescendents(Runtime *runtime, ObjectLinkingScope *outerScope) {
	linkInternalReferences(outerScope);
	setRuntimeGUID(runtime->allocateRuntimeGUID());

	materializeDescendents(runtime, outerScope);
}

void Structural::materializeDescendents(Runtime *runtime, ObjectLinkingScope *outerScope) {
	// Materialization is the step after objects are fully constructed and filled with data.
	// It does three things, recursively:
	// - Assigns all objects a new runtime GUID
	// - Clones any non-variable aliased modifiers
	// - Links any static GUIDs to in-scope visible objects
	// Objects are only ever materialized once
	ObjectLinkingScope tempModifierScope;
	ObjectLinkingScope tempStructuralScope;
	ObjectLinkingScope *modifierScope = this->getPersistentModifierScope();
	ObjectLinkingScope *structuralScope = this->getPersistentStructuralScope();

	if (!modifierScope)
		modifierScope = &tempModifierScope;
	if (!structuralScope)
		structuralScope = &tempStructuralScope;

	modifierScope->setParent(outerScope);

	for (Common::Array<Common::SharedPtr<Modifier> >::iterator it = _modifiers.begin(), itEnd = _modifiers.end(); it != itEnd; ++it) {
		Modifier *modifier = it->get();
		uint32 modifierGUID = modifier->getStaticGUID();
		if (modifier->isAlias() && !modifier->isVariable()) {
			Common::SharedPtr<Modifier> templateModifier = runtime->getProject()->resolveAlias(static_cast<AliasModifier *>(modifier)->getAliasID());
			if (!templateModifier) {
				error("Failed to resolve alias");
			}

			Common::SharedPtr<Modifier> clonedModifier = templateModifier->shallowClone();
			clonedModifier->setName(modifier->getName());

			(*it) = clonedModifier;
			modifier = clonedModifier.get();
		}
		modifierScope->addObject(modifierGUID, *it);
	}

	for (Common::Array<Common::SharedPtr<Modifier> >::const_iterator it = _modifiers.begin(), itEnd = _modifiers.end(); it != itEnd; ++it) {
		Modifier *modifier = it->get();
		modifier->materialize(runtime, modifierScope);
	}

	structuralScope->setParent(modifierScope);

	const Common::Array<Common::SharedPtr<Structural> > &children = this->getChildren();
	for (Common::Array<Common::SharedPtr<Structural> >::const_iterator it = children.begin(), itEnd = children.end(); it != itEnd; ++it) {
		Structural *child = it->get();
		structuralScope->addObject(child->getStaticGUID(), *it);
	}

	for (Common::Array<Common::SharedPtr<Structural> >::const_iterator it = children.begin(), itEnd = children.end(); it != itEnd; ++it) {
		Structural *child = it->get();
		child->materializeSelfAndDescendents(runtime, structuralScope);
	}
}

void Structural::linkInternalReferences(ObjectLinkingScope *scope) {
}

ObjectLinkingScope *Structural::getPersistentStructuralScope() {
	return nullptr;
}

ObjectLinkingScope* Structural::getPersistentModifierScope() {
	return nullptr;
}

ObjectLinkingScope::ObjectLinkingScope() : _parent(nullptr) {
}

ObjectLinkingScope::~ObjectLinkingScope() {
}

void ObjectLinkingScope::setParent(ObjectLinkingScope *parent) {
	_parent = parent;
}

void ObjectLinkingScope::addObject(uint32 guid, const Common::WeakPtr<RuntimeObject> &object) {
	_guidToObject[guid] = object;
}

void ObjectLinkingScope::reset() {
	_parent = nullptr;
	_guidToObject.clear(true);
}

LowLevelSceneStateTransitionAction::LowLevelSceneStateTransitionAction(const Common::SharedPtr<MessageDispatch> &msg)
	: _actionType(LowLevelSceneStateTransitionAction::kSendMessage), _msg(msg) {
}

LowLevelSceneStateTransitionAction::LowLevelSceneStateTransitionAction(const LowLevelSceneStateTransitionAction &other)
	: _actionType(other._actionType), _msg(other._msg), _scene(other._scene) {
}

LowLevelSceneStateTransitionAction::LowLevelSceneStateTransitionAction(const Common::SharedPtr<Structural> &scene, ActionType actionType)
	: _scene(scene), _actionType(actionType) {
}

LowLevelSceneStateTransitionAction::ActionType LowLevelSceneStateTransitionAction::getActionType() const {
	return _actionType;
}

const Common::SharedPtr<Structural>& LowLevelSceneStateTransitionAction::getScene() const {
	return _scene;
}

const Common::SharedPtr<MessageDispatch>& LowLevelSceneStateTransitionAction::getMessage() const {
	return _msg;
}

LowLevelSceneStateTransitionAction &LowLevelSceneStateTransitionAction::operator=(const LowLevelSceneStateTransitionAction &other) {
	_scene = other._scene;
	_msg = other._msg;
	_actionType = other._actionType;

	return *this;
}


HighLevelSceneTransition::HighLevelSceneTransition(const Common::SharedPtr<Structural> &scene, Type type, bool addToDestinationScene, bool addToReturnList)
	: scene(scene), type(type), addToDestinationScene(addToDestinationScene), addToReturnList(addToReturnList) {
}

MessageDispatch::MessageDispatch(const Event &evt, Structural *root, bool cascade, bool relay)
	: _cascade(cascade), _relay(relay), _terminated(false) {
	PropagationStack topEntry;
	topEntry.index = 0;
	topEntry.propagationStage = PropagationStack::kStageSendToStructuralSelf;
	topEntry.ptr.structural = root;

	_propagationStack.push_back(topEntry);
}

MessageDispatch::MessageDispatch(const Event &evt, Modifier *root, bool cascade, bool relay)
	: _cascade(cascade), _relay(relay), _terminated(false) {
	_msg.reset(new MessageProperties(evt, DynamicValue(), Common::WeakPtr<RuntimeObject>()));

	PropagationStack topEntry;
	topEntry.index = 0;
	topEntry.propagationStage = PropagationStack::kStageSendToModifier;
	topEntry.ptr.modifier = root;

	_propagationStack.push_back(topEntry);
}

bool MessageDispatch::isTerminated() const {
	return _terminated;
}

VThreadState MessageDispatch::continuePropagating(Runtime *runtime) {
	// By the point this function is called, continuePropagating has been re-posted to the VThread,
	// so any propagation state changed in this function will be handled after any VThread tasks
	// posted here.
	while (_propagationStack.size() > 0) {
		PropagationStack &stackTop = _propagationStack.back();

		switch (stackTop.propagationStage)
		{
		case PropagationStack::kStageSendToModifier: {
				Modifier *modifier = stackTop.ptr.modifier;
				_propagationStack.pop_back();

				// Handle the action in the VThread
				bool responds = modifier->respondsToEvent(_msg->getEvent());

				// Queue propagation to children, if any, when the VThread task is done
				if (responds && !_relay) {
					_terminated = true;
				} else {
					IModifierContainer *childContainer = modifier->getMessagePropagationContainer();
					if (childContainer && childContainer->getModifiers().size() > 0) {
						PropagationStack childPropagation;
						childPropagation.propagationStage = PropagationStack::kStageSendToModifierContainer;
						childPropagation.index = 0;
						childPropagation.ptr.modifierContainer = childContainer;
						_propagationStack.push_back(childPropagation);
					}
				}

				// Post to the message action itself to VThread
				runtime->postConsumeMessageTask(modifier, _msg);
				return kVThreadReturn;
			} break;
		case PropagationStack::kStageSendToModifierContainer: {
				IModifierContainer *container = stackTop.ptr.modifierContainer;
				const Common::Array<Common::SharedPtr<Modifier> > &children = container->getModifiers();
				if (stackTop.index >= children.size()) {
					_propagationStack.pop_back();
				} else {
					Common::SharedPtr<Modifier> target = children[stackTop.index++];

					PropagationStack modifierPropagation;
					modifierPropagation.propagationStage = PropagationStack::kStageSendToModifier;
					modifierPropagation.index = 0;
					modifierPropagation.ptr.modifier = target.get();
					_propagationStack.push_back(modifierPropagation);
				}
			} break;
		case PropagationStack::kStageSendToStructuralChildren: {
				Structural *structural = stackTop.ptr.structural;
				const Common::Array<Common::SharedPtr<Structural> > &children = structural->getChildren();
				if (stackTop.index >= children.size()) {
					_propagationStack.pop_back();
				} else {
					PropagationStack childPropagation;
					childPropagation.propagationStage = PropagationStack::kStageSendToStructuralSelf;
					childPropagation.index = 0;
					childPropagation.ptr.structural = structural;
					_propagationStack.push_back(childPropagation);
				}
			} break;
		case PropagationStack::kStageSendToStructuralSelf: {
				Structural *structural = stackTop.ptr.structural;
				stackTop.propagationStage = PropagationStack::kStageSendToStructuralModifiers;
				stackTop.index = 0;
				stackTop.ptr.structural = structural;

				bool responds = structural->respondsToEvent(_msg->getEvent());

				if (responds && !_relay) {
					_terminated = true;
				}

				runtime->postConsumeMessageTask(structural, _msg);
				return kVThreadReturn;
			} break;
		case PropagationStack::kStageSendToStructuralModifiers: {
				Structural *structural = stackTop.ptr.structural;

				// Once done with modifiers, propagate to children if set to cascade
				if (_cascade) {
					stackTop.propagationStage = PropagationStack::kStageSendToStructuralChildren;
					stackTop.index = 0;
					stackTop.ptr.structural = structural;
				} else {
					_propagationStack.pop_back();
				}

				if (structural->getModifiers().size() > 0) {
					PropagationStack modifierContainerPropagation;
					modifierContainerPropagation.propagationStage = PropagationStack::kStageSendToModifierContainer;
					modifierContainerPropagation.index = 0;
					modifierContainerPropagation.ptr.modifierContainer = structural;
					_propagationStack.push_back(modifierContainerPropagation);
				}
			} break;
		default:
			return kVThreadError;
		}
	}

	_terminated = true;

	return kVThreadReturn;
}

Runtime::SceneStackEntry::SceneStackEntry() {
}

Runtime::Runtime() : _nextRuntimeGUID(1) {
	_vthread.reset(new VThread());
}

void Runtime::runFrame() {
	for (;;) {
		VThreadState state = _vthread->step();
		if (state != kVThreadReturn) {
			// Still doing blocking tasks
			return;
		}

		if (_queuedProjectDesc) {
			Common::SharedPtr<ProjectDescription> desc = _queuedProjectDesc;
			_queuedProjectDesc.reset();

			unloadProject();

			_project.reset(new Project());

			_project->loadFromDescription(*desc);

			_rootLinkingScope.addObject(_project->getStaticGUID(), _project);

			// We have to materialize global variables because they are not cloned from aliases.
			debug(1, "Materializing global variables...");
			_project->materializeGlobalVariables(this, &_rootLinkingScope);

			debug(1, "Materializing project...");
			_project->materializeSelfAndDescendents(this, &_rootLinkingScope);

			debug(1, "Project is fully loaded!  Starting up...");

			if (_project->getChildren().size() == 0) {
				error("Project has no sections");
			}

			Structural *firstSection = _project->getChildren()[0].get();
			if (firstSection->getChildren().size() == 0) {
				error("Project has no subsections");
			}

			Structural *firstSubsection = firstSection->getChildren()[0].get();
			if (firstSubsection->getChildren().size() < 2) {
				error("Project has no subsections");
			}

			_pendingSceneTransitions.push_back(HighLevelSceneTransition(firstSubsection->getChildren()[1], HighLevelSceneTransition::kTypeChangeToScene, false, false));
			continue;
		}

		if (_messageQueue.size() > 0) {
			Common::SharedPtr<MessageDispatch> msg = _messageQueue[0];
			_messageQueue.remove_at(0);

			sendMessageOnVThread(msg);
			continue;
		}

		// Teardowns must only occur during idle conditions where there are no queued message and no VThread tasks
		if (_pendingTeardowns.size() > 0) {
			for (Common::Array<Teardown>::const_iterator it = _pendingTeardowns.begin(), itEnd = _pendingTeardowns.end(); it != itEnd; ++it) {
				executeTeardown(*it);
			}
			_pendingTeardowns.clear();
			continue;
		}

		if (_pendingLowLevelTransitions.size() > 0) {
			LowLevelSceneStateTransitionAction transition = _pendingLowLevelTransitions[0];
			_pendingLowLevelTransitions.remove_at(0);

			executeLowLevelSceneStateTransition(transition);
			continue;
		}

		if (_pendingSceneTransitions.size() > 0) {
			HighLevelSceneTransition transition = _pendingSceneTransitions[0];
			_pendingSceneTransitions.remove_at(0);

			executeHighLevelSceneTransition(transition);
			continue;
		}
	}
}

Common::SharedPtr<Structural> Runtime::findDefaultSharedSceneForScene(Structural *scene) {
	Structural *subsection = scene->getParent();
	const Common::Array<Common::SharedPtr<Structural> > &children = subsection->getChildren();
	if (children.size() == 0 || children[0].get() == scene)
		return Common::SharedPtr<Structural>();

	return children[0];
}

void Runtime::executeTeardown(const Teardown &teardown) {
	Common::SharedPtr<Structural> structural = teardown.structural.lock();
	if (!structural)
		return;	// Already gone

	if (teardown.onlyRemoveChildren) {
		structural->removeAllChildren();
		structural->removeAllModifiers();
	} else {
		Structural *parent = structural->getParent();

		// Nothing should be holding strong references to structural objects after they're removed from the project
		assert(parent != nullptr);

		if (!parent) {
			return; // Already unlinked but still alive somehow
		}

		parent->removeChild(structural.get());

		structural->setParent(nullptr);
	}
}

void Runtime::executeLowLevelSceneStateTransition(const LowLevelSceneStateTransitionAction &action) {
	switch (action.getActionType())
	{
	case LowLevelSceneStateTransitionAction::kSendMessage:
		sendMessageOnVThread(action.getMessage());
		break;
	case LowLevelSceneStateTransitionAction::kLoad:
		loadScene(action.getScene());
		break;
	case LowLevelSceneStateTransitionAction::kUnload: {
			Teardown teardown;
			teardown.onlyRemoveChildren = true;
			teardown.structural = action.getScene();

			_pendingTeardowns.push_back(teardown);
		} break;
	default:
		assert(false);
		break;
	}
}

void Runtime::executeCompleteTransitionToScene(const Common::SharedPtr<Structural> &targetScene) {
	if (targetScene == _activeMainScene)
		return;

	if (_sceneStack.size() == 0)
		_sceneStack.resize(1);	// Reserve shared scene slot

	Common::SharedPtr<Structural> targetSharedScene = findDefaultSharedSceneForScene(targetScene.get());

	if (targetScene == targetSharedScene)
		error("Transitioned into a default shared scene, this is not supported");

	if (_activeMainScene == targetSharedScene)
		error("Transitioned into scene currently being used as a target scene, this is not supported");

	bool sceneAlreadyInStack = false;
	for (size_t i = _sceneStack.size() - 1; i > 0; i--) {
		Common::SharedPtr<Structural> stackedScene = _sceneStack[i].scene;
		if (stackedScene == targetScene) {
			sceneAlreadyInStack = true;
		} else {
			_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneEnded, 0), _activeMainScene.get(), false, true))));
			_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kParentDisabled, 0), _activeMainScene.get(), true, true))));
			_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(_activeMainScene, LowLevelSceneStateTransitionAction::kUnload));

			if (stackedScene == targetSharedScene)
				error("Transitioned to a shared scene which was already on the stack as a normal scene.  This is not supported.");

			_sceneStack.remove_at(i);
		}
	}

	if (targetSharedScene != _activeSharedScene) {
		if (_activeSharedScene) {
			_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneEnded, 0), _activeSharedScene.get(), false, true))));
			_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kParentDisabled, 0), _activeSharedScene.get(), true, true))));
			_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(_activeSharedScene, LowLevelSceneStateTransitionAction::kUnload));
		}

		_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(targetSharedScene, LowLevelSceneStateTransitionAction::kLoad));
		_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kParentEnabled, 0), targetSharedScene.get(), true, true))));
		_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneStarted, 0), targetSharedScene.get(), false, true))));

		SceneStackEntry sharedSceneEntry;
		sharedSceneEntry.scene = targetScene;

		_sceneStack[0] = sharedSceneEntry;
	}

	if (!sceneAlreadyInStack) {
		_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(targetScene, LowLevelSceneStateTransitionAction::kLoad));
		_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kParentEnabled, 0), targetScene.get(), true, true))));
		_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneStarted, 0), targetScene.get(), false, true))));

		SceneStackEntry sceneEntry;
		sceneEntry.scene = targetScene;

		_sceneStack.push_back(sceneEntry);
	}

	_activeMainScene = targetScene;
	_activeSharedScene = targetSharedScene;

	executeSharedScenePostSceneChangeActions();
}

void Runtime::executeHighLevelSceneTransition(const HighLevelSceneTransition &transition) {
	if (_sceneStack.size() == 0)
		_sceneStack.resize(1); // Reserve shared scene slot

	// This replicates a bunch of quirks/bugs of mTropolis's scene transition logic,
	// see the accompanying notes file.  There are probably some missing cases related to
	// shared scene, calling return/scene transitions during scene deactivation, or other
	// edge cases that hopefully no games actually do!
	switch (transition.type) {
	case HighLevelSceneTransition::kTypeReturn: {
			if (_sceneReturnList.size() == 0) {
				warning("A scene return was requested, but no scenes are in the scene return list");
				return;
			}

			const SceneReturnListEntry &sceneReturn = _sceneReturnList.back();

			if (sceneReturn.scene == _activeSharedScene)
				error("Transitioned into the active shared scene as the main scene, this is not supported");

			if (sceneReturn.scene != _activeMainScene) {
				assert(_activeMainScene.get() != nullptr); // There should always be an active main scene after the first transition

				if (sceneReturn.isAddToDestinationSceneTransition) {
					// In this case we unload the active main scene and reactivate the old main
					_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneEnded, 0), _activeMainScene.get(), false, true))));
					_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kParentDisabled, 0), _activeMainScene.get(), true, true))));
					_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(_activeMainScene, LowLevelSceneStateTransitionAction::kUnload));

					_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneReactivated, 0), _activeMainScene.get(), false, true))));

					_activeMainScene = sceneReturn.scene;

					executeSharedScenePostSceneChangeActions();
				} else {
					executeCompleteTransitionToScene(sceneReturn.scene);
				}
			}
		} break;
	case HighLevelSceneTransition::kTypeChangeToScene: {
			const Common::SharedPtr<Structural> targetScene = transition.scene;

			if (transition.addToDestinationScene || !transition.addToReturnList) {
				SceneReturnListEntry returnListEntry;
				returnListEntry.isAddToDestinationSceneTransition = transition.addToDestinationScene;
				returnListEntry.scene = _activeMainScene;
				_sceneReturnList.push_back(returnListEntry);
			}

			if (transition.addToDestinationScene) {
				if (targetScene != _activeMainScene) {
					Common::SharedPtr<Structural> targetSharedScene = findDefaultSharedSceneForScene(targetScene.get());

					if (targetScene == targetSharedScene)
						error("Transitioned into a default shared scene, this is not supported");

					if (_activeMainScene == targetSharedScene)
						error("Transitioned into scene currently being used as a target scene, this is not supported");

					_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneDeactivated, 0), _activeMainScene.get(), false, true))));

					if (targetSharedScene != _activeSharedScene) {
						if (_activeSharedScene) {
							_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneEnded, 0), _activeSharedScene.get(), false, true))));
							_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kParentDisabled, 0), _activeSharedScene.get(), true, true))));
							_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(_activeSharedScene, LowLevelSceneStateTransitionAction::kUnload));
						}

						_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(targetSharedScene, LowLevelSceneStateTransitionAction::kLoad));
						_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kParentEnabled, 0), targetSharedScene.get(), true, true))));
						_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneStarted, 0), targetSharedScene.get(), false, true))));

						SceneStackEntry sharedSceneEntry;
						sharedSceneEntry.scene = targetScene;

						_sceneStack[0] = sharedSceneEntry;
					}

						
					bool sceneAlreadyInStack = false;
					for (size_t i = _sceneStack.size() - 1; i > 0; i--) {
						Common::SharedPtr<Structural> stackedScene = _sceneStack[i].scene;
						if (stackedScene == targetScene) {
							sceneAlreadyInStack = true;
							break;
						}
					}

					// This is probably wrong if it's already in the stack, but transitioning to already-in-stack scenes is extremely buggy in mTropolis Player anyway
					if (!sceneAlreadyInStack) {
						_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(targetScene, LowLevelSceneStateTransitionAction::kLoad));
						_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kParentEnabled, 0), targetScene.get(), true, true))));
						_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSceneStarted, 0), targetScene.get(), false, true))));

						SceneStackEntry sceneEntry;
						sceneEntry.scene = targetScene;

						_sceneStack.push_back(sceneEntry);
					}

					_activeMainScene = targetScene;
					_activeSharedScene = targetSharedScene;

					executeSharedScenePostSceneChangeActions();
				}
			} else {
				executeCompleteTransitionToScene(targetScene);
			}
		} break;
	default:
		break;
	}
}

void Runtime::executeSharedScenePostSceneChangeActions() {
	Structural *subsection = _activeMainScene->getParent();
	const Common::Array<Common::SharedPtr<Structural> > &subsectionScenes = subsection->getChildren();

	_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSharedSceneSceneChanged, 0), _activeSharedScene.get(), false, true))));
	if (subsectionScenes.size() > 1) {
		if (_activeMainScene == subsectionScenes[subsectionScenes.size() - 1])
			_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSharedSceneNoNextScene, 0), _activeSharedScene.get(), false, true))));
		if (_activeMainScene == subsectionScenes[1])
			_pendingLowLevelTransitions.push_back(LowLevelSceneStateTransitionAction(Common::SharedPtr<MessageDispatch>(new MessageDispatch(Event::create(EventIDs::kSharedSceneNoPrevScene, 0), _activeSharedScene.get(), false, true))));
	}
}

void Runtime::loadScene(const Common::SharedPtr<Structural>& scene) {
	Element *element = static_cast<Element *>(scene.get());
	uint32 streamIndex = element->getStreamLocator() & 0xffff;	// Not actually sure how many bits are legal here

	error("TODO: Load scene");
}

void Runtime::sendMessageOnVThread(const Common::SharedPtr<MessageDispatch> &dispatch) {
	DispatchMethodTaskData *taskData = _vthread->pushTask(this, &Runtime::dispatchMessageTask);
	taskData->dispatch = dispatch;
}

VThreadState Runtime::dispatchMessageTask(const DispatchMethodTaskData &data) {
	Common::SharedPtr<MessageDispatch> dispatchPtr = data.dispatch;
	MessageDispatch &dispatch = *dispatchPtr.get();

	if (dispatch.isTerminated())
		return kVThreadReturn;
	else {
		// Requeue propagation after whatever happens with this propagation step
		DispatchMethodTaskData *requeueData = _vthread->pushTask(this, &Runtime::dispatchMessageTask);
		requeueData->dispatch = dispatchPtr;

		return dispatch.continuePropagating(this);
	}
}

VThreadState Runtime::consumeMessageTask(const ConsumeMessageTaskData &data) {
	IMessageConsumer *consumer = data.consumer;
	assert(consumer->respondsToEvent(data.message->getEvent()));
	return consumer->consumeMessage(this, data.message);
}

void Runtime::queueMessage(const Common::SharedPtr<MessageDispatch>& dispatch) {
	_messageQueue.push_back(dispatch);
}

void Runtime::unloadProject() {
	_activeMainScene.reset();
	_activeSharedScene.reset();
	_sceneStack.clear();
	_sceneReturnList.clear();
	_pendingLowLevelTransitions.clear();
	_pendingSceneTransitions.clear();
	_pendingTeardowns.clear();
	_messageQueue.clear();
	_vthread.reset(new VThread());

	// These should be last
	_project.reset();
	_rootLinkingScope.reset();
}

void Runtime::queueProject(const Common::SharedPtr<ProjectDescription> &desc) {
	_queuedProjectDesc = desc;
}

void Runtime::addVolume(int volumeID, const char *name, bool isMounted) {
	VolumeState volume;
	volume.name = name;
	volume.isMounted = isMounted;
	volume.volumeID = volumeID;

	_volumes.push_back(volume);
}

void Runtime::addSceneStateTransition(const HighLevelSceneTransition &transition) {
	_pendingSceneTransitions.push_back(transition);
}

Project *Runtime::getProject() const {
	return _project.get();
}

void Runtime::postConsumeMessageTask(IMessageConsumer *consumer, const Common::SharedPtr<MessageProperties> &msg) {
	ConsumeMessageTaskData *params = _vthread->pushTask(this, &Runtime::consumeMessageTask);
	params->consumer = consumer;
	params->message = msg;
}

uint32 Runtime::allocateRuntimeGUID() {
	return _nextRuntimeGUID++;
}

const Common::Array<Common::SharedPtr<Modifier> > &IModifierContainer::getModifiers() const {
	return const_cast<IModifierContainer &>(*this).getModifiers();
};

ChildLoaderContext::ChildLoaderContext() : remainingCount(0), type(kTypeUnknown) {
}

ProjectPlugInRegistry::ProjectPlugInRegistry() {
}

void ProjectPlugInRegistry::registerPlugInModifier(const char *name, const Data::IPlugInModifierDataFactory *loader, const IPlugInModifierFactory *factory) {
	_dataLoaderRegistry.registerLoader(name, loader);
	_factoryRegistry[name] = factory;
}

const Data::PlugInModifierRegistry& ProjectPlugInRegistry::getDataLoaderRegistry() const {
	return _dataLoaderRegistry;
}

const IPlugInModifierFactory *ProjectPlugInRegistry::findPlugInModifierFactory(const char *name) const {
	Common::HashMap<Common::String, const IPlugInModifierFactory *>::const_iterator it = _factoryRegistry.find(name);
	if (it == _factoryRegistry.end())
		return nullptr;
	return it->_value;
}

Project::Project()
	: _projectFormat(Data::kProjectFormatUnknown), _isBigEndian(false), _haveGlobalObjectInfo(false), _haveProjectStructuralDef(false) {
}

Project::~Project() {
}

void Project::loadFromDescription(const ProjectDescription& desc) {
	_resources = desc.getResources();

	debug(1, "Loading new project...");

	const Common::Array<Common::SharedPtr<PlugIn> > &plugIns = desc.getPlugIns();

	for (Common::Array<Common::SharedPtr<PlugIn> >::const_iterator it = plugIns.begin(), itEnd = plugIns.end(); it != itEnd; ++it) {
		Common::SharedPtr<PlugIn> plugIn = (*it);

		_plugIns.push_back(plugIn);

		plugIn->registerModifiers(&_plugInRegistry);
	}

	const Data::PlugInModifierRegistry &plugInDataLoaderRegistry = _plugInRegistry.getDataLoaderRegistry();

	size_t numSegments = desc.getSegments().size();
	_segments.resize(numSegments);

	for (size_t i = 0; i < numSegments; i++) {
		_segments[i].desc = desc.getSegments()[i];
	}

	// Try to open the first segment
	openSegmentStream(0);

	Common::SeekableReadStream *baseStream = _segments[0].weakStream;
	uint16_t startValue = baseStream->readUint16LE();

	if (startValue == 1) {
		// Windows format
		_isBigEndian = false;
		_projectFormat = Data::kProjectFormatWindows;
	} else if (startValue == 0) {
		// Mac format
		_isBigEndian = true;
		_projectFormat = Data::kProjectFormatMacintosh;
	} else {
		error("Unrecognized project segment header");
	}

	Common::SeekableSubReadStreamEndian stream(baseStream, 2, baseStream->size(), _isBigEndian);
	if (stream.readUint32() != 0xaa55a5a5 || stream.readUint32() != 0 || stream.readUint32() != 14) {
		error("Unrecognized project segment header");
	}

	Data::DataReader reader(stream, _projectFormat);

	Common::SharedPtr<Data::DataObject> dataObject;
	Data::loadDataObject(_plugInRegistry.getDataLoaderRegistry(), reader, dataObject);

	if (!dataObject || dataObject->getType() != Data::DataObjectTypes::kProjectHeader) {
		error("Expected project header but found something else");
	}

	Data::loadDataObject(plugInDataLoaderRegistry, reader, dataObject);
	if (!dataObject || dataObject->getType() != Data::DataObjectTypes::kProjectCatalog) {
		error("Expected project catalog but found something else");
	}

	Data::ProjectCatalog *catalog = static_cast<Data::ProjectCatalog *>(dataObject.get());

	if (catalog->segments.size() != desc.getSegments().size()) {
		error("Project declared a different number of segments than the project description provided");
	}

	debug(1, "Catalog loaded OK, identified %i streams", static_cast<int>(catalog->streams.size()));

	_streams.resize(catalog->streams.size());
	for (size_t i = 0; i < _streams.size(); i++) {
		StreamDesc &streamDesc = _streams[i];
		const Data::ProjectCatalog::StreamDesc &srcStream = catalog->streams[i];

		if (!strcmp(srcStream.streamType, "assetStream"))
			streamDesc.streamType = kStreamTypeAsset;
		else if (!strcmp(srcStream.streamType, "bootStream"))
			streamDesc.streamType = kStreamTypeBoot;
		else if (!strcmp(srcStream.streamType, "sceneStream"))
			streamDesc.streamType = kStreamTypeScene;
		else
			streamDesc.streamType = kStreamTypeUnknown;

		streamDesc.segmentIndex = srcStream.segmentIndexPlusOne - 1;
		streamDesc.size = srcStream.size;
		streamDesc.pos = srcStream.pos;
	}

	// Locate the boot stream
	size_t bootStreamIndex = 0;
	bool foundBootStream = false;
	for (size_t i = 0; i < _streams.size(); i++) {
		if (_streams[i].streamType == kStreamTypeBoot) {
			bootStreamIndex = i;
			foundBootStream = true;
			break;
		}
	}
	if (!foundBootStream) {
		error("Failed to find boot stream");
	}

	debug(1, "Loading boot stream");

	loadBootStream(bootStreamIndex);

	debug(1, "Boot stream loaded successfully");
}

Common::SharedPtr<Modifier> Project::resolveAlias(uint32 aliasID) const {
	if (aliasID == 0 || aliasID > _globalModifiers.getModifiers().size())
		return Common::SharedPtr<Modifier>();

	return _globalModifiers.getModifiers()[aliasID - 1];
}

void Project::materializeGlobalVariables(Runtime *runtime, ObjectLinkingScope *outerScope) {
	for (Common::Array<Common::SharedPtr<Modifier> >::const_iterator it = _globalModifiers.getModifiers().begin(), itEnd = _globalModifiers.getModifiers().end(); it != itEnd; ++it) {
		Modifier *modifier = it->get();
		if (!modifier)
			continue;

		if (modifier->isVariable())
			modifier->materialize(runtime, outerScope);
	}
}

void Project::openSegmentStream(int segmentIndex) {
	if (segmentIndex < 0 || static_cast<size_t>(segmentIndex) > _segments.size()) {
		error("Invalid segment index %i", segmentIndex);
	}

	Segment &segment = _segments[segmentIndex];

	if (segment.weakStream)
		return;

	if (segment.desc.stream) {
		segment.rcStream.reset();
		segment.weakStream = segment.desc.stream;
	} else {
		Common::File *f = new Common::File();
		segment.rcStream.reset(f);
		segment.weakStream = f;

		if (!f->open(segment.desc.filePath)) {
			error("Failed to open segment file %s", segment.desc.filePath.c_str());
		}
	}
}

void Project::loadBootStream(size_t streamIndex) {
	const StreamDesc &streamDesc = _streams[streamIndex];

	size_t segmentIndex = streamDesc.segmentIndex;
	openSegmentStream(segmentIndex);

	Common::SeekableSubReadStreamEndian stream(_segments[segmentIndex].weakStream, streamDesc.pos, streamDesc.pos + streamDesc.size, _isBigEndian);
	Data::DataReader reader(stream, _projectFormat);

	ChildLoaderStack loaderStack;
	AssetDefLoaderContext assetDefLoader;

	const Data::PlugInModifierRegistry &plugInDataLoaderRegistry = _plugInRegistry.getDataLoaderRegistry();

	size_t numObjectsLoaded = 0;
	while (stream.pos() != streamDesc.size) {
		Common::SharedPtr<Data::DataObject> dataObject;
		Data::loadDataObject(plugInDataLoaderRegistry, reader, dataObject);

		if (!dataObject) {
			error("Failed to load project boot data");
		}

		Data::DataObjectTypes::DataObjectType dataObjectType = dataObject->getType();

		if (Data::DataObjectTypes::isAsset(dataObjectType)) {
			// Asset defs can appear anywhere
			loadAssetDef(assetDefLoader, *dataObject.get());
		} else if (dataObjectType == Data::DataObjectTypes::kAssetDataChunk) {
			// Ignore
			continue;
		} else if (loaderStack.contexts.size() > 0) {
			loadContextualObject(loaderStack, *dataObject.get());
		} else {
			// Root-level objects
			switch (dataObject->getType()) {
			case Data::DataObjectTypes::kPresentationSettings:
				loadPresentationSettings(*static_cast<const Data::PresentationSettings *>(dataObject.get()));
				break;
			case Data::DataObjectTypes::kAssetCatalog:
				loadAssetCatalog(*static_cast<const Data::AssetCatalog *>(dataObject.get()));
				break;
			case Data::DataObjectTypes::kGlobalObjectInfo:
				loadGlobalObjectInfo(loaderStack, *static_cast<const Data::GlobalObjectInfo *>(dataObject.get()));
				break;
			case Data::DataObjectTypes::kProjectLabelMap:
				loadLabelMap(*static_cast<const Data::ProjectLabelMap *>(dataObject.get()));
				break;
			case Data::DataObjectTypes::kProjectStructuralDef: {
					if (_haveProjectStructuralDef)
						error("Multiple project structural defs");

					_haveProjectStructuralDef = true;

					ChildLoaderContext loaderContext;
					loaderContext.containerUnion.structural = this;
					loaderContext.remainingCount = 0;
					loaderContext.type = ChildLoaderContext::kTypeProject;

					loaderStack.contexts.push_back(loaderContext);
				} break;
			case Data::DataObjectTypes::kStreamHeader:
			case Data::DataObjectTypes::kUnknown19:
				// Ignore
				break;
			default:
				error("Unexpected object type in boot stream");
			}
		}

		numObjectsLoaded++;
	}

	if (loaderStack.contexts.size() != 1 || loaderStack.contexts[0].type != ChildLoaderContext::kTypeProject) {
		error("Boot stream loader finished in an expected state, something didn't finish loading");
	}
}

void Project::loadPresentationSettings(const Data::PresentationSettings &presentationSettings) {
	_presentationSettings.bitsPerPixel = presentationSettings.bitsPerPixel;
	if (_presentationSettings.bitsPerPixel != 16) {
		error("Unsupported bit depth");
	}
	_presentationSettings.width = presentationSettings.dimensions.x;
	_presentationSettings.height = presentationSettings.dimensions.y;
}

void Project::loadAssetCatalog(const Data::AssetCatalog &assetCatalog) {
	_assetsByID.clear();
	_realAssets.clear();
	_assetNameToID.clear();

	size_t numRealAssets = 0;
	for (size_t i = 0; i < assetCatalog.assets.size(); i++) {
		const Data::AssetCatalog::AssetInfo &assetInfo = assetCatalog.assets[i];
		if ((assetInfo.flags1 & Data::AssetCatalog::kFlag1Deleted) == 0)
			numRealAssets++;
	}

	_realAssets.resize(numRealAssets);
	_assetsByID.resize(assetCatalog.assets.size() + 1);

	_assetsByID[0] = nullptr;

	numRealAssets = 0;
	for (size_t i = 0; i < assetCatalog.assets.size(); i++) {
		const Data::AssetCatalog::AssetInfo &assetInfo = assetCatalog.assets[i];
		if (assetInfo.flags1 & Data::AssetCatalog::kFlag1Deleted) {
			_assetsByID[i + 1] = nullptr;
		} else {
			AssetDesc &assetDesc = _realAssets[numRealAssets++];

			assetDesc.id = i + 1;
			assetDesc.name = assetInfo.name;
			assetDesc.typeCode = assetInfo.assetType;

			_assetsByID[assetDesc.id] = &assetDesc;
			if (!assetDesc.name.empty())
				_assetNameToID[assetDesc.name] = assetDesc.id;
		}
	}
}

void Project::loadGlobalObjectInfo(ChildLoaderStack& loaderStack, const Data::GlobalObjectInfo& globalObjectInfo) {
	if (_haveGlobalObjectInfo)
		error("Multiple global object infos");

	_haveGlobalObjectInfo = true;

	if (globalObjectInfo.numGlobalModifiers > 0) {
		ChildLoaderContext loaderContext;
		loaderContext.containerUnion.modifierContainer = &_globalModifiers;
		loaderContext.remainingCount = globalObjectInfo.numGlobalModifiers;
		loaderContext.type = ChildLoaderContext::kTypeModifierList;

		loaderStack.contexts.push_back(loaderContext);
	}
}

Common::SharedPtr<Modifier> Project::loadModifierObject(ModifierLoaderContext &loaderContext, const Data::DataObject &dataObject) {
	// Special case for debris
	if (dataObject.getType() == Data::DataObjectTypes::kDebris)
		return nullptr;

	Common::SharedPtr<Modifier> modifier;

	// Special case for plug-ins
	if (dataObject.getType() == Data::DataObjectTypes::kPlugInModifier) {
		const Data::PlugInModifier &plugInData = static_cast<const Data::PlugInModifier &>(dataObject);
		const IPlugInModifierFactory *factory = _plugInRegistry.findPlugInModifierFactory(plugInData.modifierName);
		if (!factory)
			error("Unknown or unsupported plug-in modifier type");

		modifier = factory->createModifier(loaderContext, plugInData);
	} else {
		IModifierFactory *factory = getModifierFactoryForDataObjectType(dataObject.getType());

		if (!factory)
			error("Unknown or unsupported modifier type, or non-modifier encountered where a modifier was expected");

		modifier = factory->createModifier(loaderContext, dataObject);
	}
	if (!modifier)
		error("Modifier object failed to load");

	return modifier;
}

void Project::loadLabelMap(const Data::ProjectLabelMap &projectLabelMap) {
	debug(1, "Loading label map...");

	_labelSuperGroups.resize(projectLabelMap.numSuperGroups);

	size_t totalLabels = 0;
	for (size_t i = 0; i < projectLabelMap.numSuperGroups; i++) {
		_labelSuperGroups[i].numTotalNodes = 0;
		for (size_t j = 0; j < projectLabelMap.superGroups[i].numChildren; j++)
			totalLabels += recursiveCountLabels(projectLabelMap.superGroups[i].tree[j]);
	}

	Common::Array<const Data::ProjectLabelMap::LabelTree *> treeQueue;
	treeQueue.resize(totalLabels);
	_labelTree.resize(totalLabels);

	// Expand label tree into a breadth-first tree but cluster all super-groups
	size_t insertionOffset = 0;
	size_t dequeueOffset = 0;
	for (size_t i = 0; i < projectLabelMap.numSuperGroups; i++) {
		const Data::ProjectLabelMap::SuperGroup &dataSG = projectLabelMap.superGroups[i];
		LabelSuperGroup &sg = _labelSuperGroups[i];

		sg.name = dataSG.name;
		sg.superGroupID = dataSG.id;

		sg.firstRootNodeIndex = insertionOffset;

		for (size_t j = 0; j < dataSG.numChildren; j++)
			treeQueue[insertionOffset++] = &dataSG.tree[j];

		while (dequeueOffset < insertionOffset) {
			const Data::ProjectLabelMap::LabelTree &dataTree = *treeQueue[dequeueOffset];
			LabelTree &labelTree = _labelTree[dequeueOffset];

			labelTree.id = dataTree.id;
			labelTree.name = dataTree.name;
			dequeueOffset++;

			labelTree.firstChildIndex = insertionOffset;
			labelTree.numChildren = dataTree.numChildren;
			for (size_t j = 0; j < dataTree.numChildren; j++)
				treeQueue[insertionOffset++] = &dataTree.children[j];
		}

		sg.numTotalNodes = insertionOffset - sg.firstRootNodeIndex;
	}

	debug(1, "Loaded %i labels and %i supergroups", static_cast<int>(_labelTree.size()), static_cast<int>(_labelSuperGroups.size()));
}

size_t Project::recursiveCountLabels(const Data::ProjectLabelMap::LabelTree& tree) {
	size_t numLabels = 1;	// For the node itself
	for (size_t i = 0; i < tree.numChildren; i++)
		numLabels += recursiveCountLabels(tree.children[i]);
	return numLabels;
}

ObjectLinkingScope *Project::getPersistentStructuralScope() {
	return &_structuralScope;
}

ObjectLinkingScope *Project::getPersistentModifierScope() {
	return &_modifierScope;
}

void Project::loadContextualObject(ChildLoaderStack &stack, const Data::DataObject &dataObject) {
	ChildLoaderContext &topContext = stack.contexts.back();
	const Data::DataObjectTypes::DataObjectType dataObjectType = dataObject.getType();

	// The stack entry must always be popped before loading the object because the load process may descend into more children,
	// such as when behaviors are nested.
	switch (topContext.type) {
	case ChildLoaderContext::kTypeModifierList: {
			IModifierContainer *container = topContext.containerUnion.modifierContainer;

			if ((--topContext.remainingCount) == 0)
				stack.contexts.pop_back();

			ModifierLoaderContext loaderContext(&stack);

			container->appendModifier(loadModifierObject(loaderContext, dataObject));
		} break;
	case ChildLoaderContext::kTypeProject: {
			Structural *project = topContext.containerUnion.structural;

			if (dataObjectType == Data::DataObjectTypes::kSectionStructuralDef) {

				const Data::SectionStructuralDef &sectionObject = static_cast<const Data::SectionStructuralDef &>(dataObject);

				Common::SharedPtr<Structural> section(new Section());
				if (!static_cast<Section *>(section.get())->load(sectionObject))
					error("Failed to load section");

				project->addChild(section);
				section->setParent(project);

				// For some reason all section objects have the "no more siblings" structural flag.
				// There doesn't appear to be any indication of how many section objects there will
				// be either.
				//if (sectionObject.structuralFlags & Data::StructuralFlags::kNoMoreSiblings)
				//	stack.contexts.pop_back();

				if (sectionObject.structuralFlags & Data::StructuralFlags::kHasChildren) {
					ChildLoaderContext loaderContext;
					loaderContext.containerUnion.structural = section.get();
					loaderContext.remainingCount = 0;
					loaderContext.type = ChildLoaderContext::kTypeSection;

					stack.contexts.push_back(loaderContext);
				}
			} else if (Data::DataObjectTypes::isModifier(dataObjectType)) {
				ModifierLoaderContext loaderContext(&stack);
				project->appendModifier(loadModifierObject(loaderContext, dataObject));
			} else {
				error("Unexpected object type in this context");
			}
		} break;
	case ChildLoaderContext::kTypeSection: {
			Structural *section = topContext.containerUnion.structural;

			if (dataObject.getType() == Data::DataObjectTypes::kSubsectionStructuralDef) {
				const Data::SubsectionStructuralDef &subsectionObject = static_cast<const Data::SubsectionStructuralDef &>(dataObject);

				Common::SharedPtr<Structural> subsection(new Subsection());
				if (!static_cast<Subsection *>(subsection.get())->load(subsectionObject))
					error("Failed to load subsection");

				section->addChild(subsection);

				if (subsectionObject.structuralFlags & Data::StructuralFlags::kNoMoreSiblings)
					stack.contexts.pop_back();

				if (subsectionObject.structuralFlags & Data::StructuralFlags::kHasChildren) {
					ChildLoaderContext loaderContext;
					loaderContext.containerUnion.filteredElements.structural = subsection.get();
					loaderContext.containerUnion.filteredElements.filterFunc = Data::DataObjectTypes::isValidSceneRootElement;
					loaderContext.remainingCount = 0;
					loaderContext.type = ChildLoaderContext::kTypeFilteredElements;

					stack.contexts.push_back(loaderContext);
				}
			} else if (Data::DataObjectTypes::isModifier(dataObjectType)) {
				ModifierLoaderContext loaderContext(&stack);
				section->appendModifier(loadModifierObject(loaderContext, dataObject));
			} else {
				error("Unexpected object type in this context");
			}
		} break;
	case ChildLoaderContext::kTypeFilteredElements: {
			Structural *container = topContext.containerUnion.filteredElements.structural;

			if (topContext.containerUnion.filteredElements.filterFunc(dataObjectType)) {
				const Data::StructuralDef &structuralDef = static_cast<const Data::StructuralDef &>(dataObject);

				IElementFactory *elementFactory = getElementFactoryForDataObjectType(dataObjectType);
				if (!elementFactory) {
					error("No element factory defined for structural object");
				}

				ElementLoaderContext elementLoaderContext;
				Common::SharedPtr<Element> element = elementFactory->createElement(elementLoaderContext, dataObject);

				container->addChild(element);

				if (structuralDef.structuralFlags & Data::StructuralFlags::kNoMoreSiblings)
					stack.contexts.pop_back();

				if (structuralDef.structuralFlags & Data::StructuralFlags::kHasChildren) {
					ChildLoaderContext loaderContext;
					// Visual elements can contain non-visual element children, but non-visual elements
					// can only contain non-visual element children
					loaderContext.containerUnion.filteredElements.filterFunc = element->isVisual() ? Data::DataObjectTypes::isElement : Data::DataObjectTypes::isNonVisualElement;
					loaderContext.containerUnion.filteredElements.structural = container;
					loaderContext.remainingCount = 0;
					loaderContext.type = ChildLoaderContext::kTypeFilteredElements;

					stack.contexts.push_back(loaderContext);
				}
			} else if (Data::DataObjectTypes::isModifier(dataObjectType)) {
				ModifierLoaderContext loaderContext(&stack);
				container->appendModifier(loadModifierObject(loaderContext, dataObject));
			} else {
				error("Unexpected object type in this context");
			}
		} break;
	default:
		error("Tried to load a contextual object outside of a context");
		break;
	}
}

void Project::loadAssetDef(AssetDefLoaderContext& context, const Data::DataObject& dataObject) {
	assert(Data::DataObjectTypes::isAsset(dataObject.getType()));

	IAssetFactory *factory = getAssetFactoryForDataObjectType(dataObject.getType());
	if (!factory) {
		error("Unimplemented asset type");
		return;
	}

	AssetLoaderContext loaderContext;
	context.assets.push_back(factory->createAsset(loaderContext, dataObject));
}

bool Section::load(const Data::SectionStructuralDef &data) {
	_name = data.name;
	_guid = data.guid;

	return true;
}

ObjectLinkingScope *Section::getPersistentStructuralScope() {
	return &_structuralScope;
}

ObjectLinkingScope *Section::getPersistentModifierScope() {
	return &_modifierScope;
}

bool Subsection::load(const Data::SubsectionStructuralDef &data) {
	_name = data.name;
	_guid = data.guid;

	return true;
}

ObjectLinkingScope *Subsection::getPersistentStructuralScope() {
	return &_structuralScope;
}

ObjectLinkingScope *Subsection::getPersistentModifierScope() {
	return &_modifierScope;
}

uint32 Element::getStreamLocator() const {
	return _streamLocator;
}

bool VisualElement::isVisual() const {
	return true;
}

bool VisualElement::loadCommon(const Common::String &name, uint32 guid, const Data::Rect &rect, uint32 elementFlags, uint16 layer, uint32 streamLocator, uint16 sectionID) {
	if (!_rect.load(rect))
		return false;

	_name = name;
	_guid = guid;
	_isHidden = ((elementFlags & Data::ElementFlags::kHidden) != 0);
	_streamLocator = streamLocator;
	_sectionID = sectionID;

	return true;
}

bool NonVisualElement::isVisual() const {
	return false;
}


ModifierFlags::ModifierFlags() : isLastModifier(false) {
}

bool ModifierFlags::load(const uint32 dataModifierFlags) {
	isLastModifier = ((dataModifierFlags & 0x2) != 0);
	return true;
}

Modifier::Modifier() {
}

Modifier::~Modifier() {
}

void Modifier::materialize(Runtime *runtime, ObjectLinkingScope *outerScope) {
	ModifierChildMaterializer childMaterializer(runtime, outerScope);
	this->visitInternalReferences(&childMaterializer);

	linkInternalReferences();
	setRuntimeGUID(runtime->allocateRuntimeGUID());
}

bool Modifier::isAlias() const {
	return false;
}

bool Modifier::isVariable() const {
	return false;
}

IModifierContainer *Modifier::getMessagePropagationContainer() const {
	return nullptr;
}

bool Modifier::respondsToEvent(const Event &evt) const {
	return false;
}

VThreadState Modifier::consumeMessage(Runtime *runtime, const Common::SharedPtr<MessageProperties> &msg) const {
	// If you're here, a message type was reported as responsive by respondsToEvent but consumeMessage wasn't overrided
	assert(false);
	return kVThreadError;
}

void Modifier::setName(const Common::String& name) {
	_name = name;
}

const Common::String& Modifier::getName() const {
	return _name;
}

void Modifier::visitInternalReferences(IStructuralReferenceVisitor *visitor) {
}

bool VariableModifier::isVariable() const {
	return true;
}

bool Modifier::loadTypicalHeader(const Data::TypicalModifierHeader &typicalHeader) {
	if (!_modifierFlags.load(typicalHeader.modifierFlags))
		return false;
	_guid = typicalHeader.guid;
	_name = typicalHeader.name;

	return true;
}

void Modifier::linkInternalReferences() {
}

} // End of namespace MTropolis
