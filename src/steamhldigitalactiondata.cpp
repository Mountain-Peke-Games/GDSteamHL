#include "steamhldigitalactiondata.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

SteamHLDigitalActionData::SteamHLDigitalActionData() { }

SteamHLDigitalActionData::SteamHLDigitalActionData(InputDigitalActionData_t inputDigitalAction) {
	this->bActive = inputDigitalAction.bActive;
	this->bState = inputDigitalAction.bState;
}

SteamHLDigitalActionData::~SteamHLDigitalActionData() { }

void SteamHLDigitalActionData::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_state"), &SteamHLDigitalActionData::godot_GetBState);
	ClassDB::bind_method(D_METHOD("set_state"), &SteamHLDigitalActionData::godot_SetBState);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "state"), "set_state", "get_state");

	ClassDB::bind_method(D_METHOD("get_active"), &SteamHLDigitalActionData::godot_GetBActive);
	ClassDB::bind_method(D_METHOD("set_active"), &SteamHLDigitalActionData::godot_SetBActive);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "active"), "set_active", "get_active");
}

bool SteamHLDigitalActionData::godot_GetBState() const {
	return this->bState;
}

void SteamHLDigitalActionData::godot_SetBState(bool newState) {
	this->bState = newState;
}

bool SteamHLDigitalActionData::godot_GetBActive() const {
	return this->bActive;
}

void SteamHLDigitalActionData::godot_SetBActive(bool newActive) {
	this->bActive = newActive;
}