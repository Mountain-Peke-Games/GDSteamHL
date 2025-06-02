#include "steamhlanalogactiondata.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

SteamHLAnalogActionData::SteamHLAnalogActionData() { }

SteamHLAnalogActionData::SteamHLAnalogActionData(InputAnalogActionData_t inputAnalogAction) {
	this->eMode = inputAnalogAction.eMode;
	this->x = inputAnalogAction.x;
	this->y = inputAnalogAction.y;
	this->bActive = inputAnalogAction.bActive;
}

SteamHLAnalogActionData::~SteamHLAnalogActionData() { }

void SteamHLAnalogActionData::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_mode"), &SteamHLAnalogActionData::godot_GetEMode);
	ClassDB::bind_method(D_METHOD("set_mode"), &SteamHLAnalogActionData::godot_SetEMode);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mode"), "set_mode", "get_mode");

	ClassDB::bind_method(D_METHOD("get_x"), &SteamHLAnalogActionData::godot_GetX);
	ClassDB::bind_method(D_METHOD("set_x"), &SteamHLAnalogActionData::godot_SetX);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "x"), "set_x", "get_x");

	ClassDB::bind_method(D_METHOD("get_y"), &SteamHLAnalogActionData::godot_GetY);
	ClassDB::bind_method(D_METHOD("set_y"), &SteamHLAnalogActionData::godot_SetY);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "y"), "set_y", "get_y");

	ClassDB::bind_method(D_METHOD("get_xy"), &SteamHLAnalogActionData::GetXY);
	ClassDB::bind_method(D_METHOD("set_xy"), &SteamHLAnalogActionData::SetXY);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "xy"), "set_xy", "get_xy");

	ClassDB::bind_method(D_METHOD("get_active"), &SteamHLAnalogActionData::godot_GetX);
	ClassDB::bind_method(D_METHOD("set_active"), &SteamHLAnalogActionData::godot_SetX);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "active"), "set_active", "get_active");

	BIND_ENUM_CONSTANT(k_EInputSourceMode_None);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_Dpad);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_Buttons);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_FourButtons);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_AbsoluteMouse);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_RelativeMouse);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_JoystickMove);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_JoystickMouse);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_JoystickCamera);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_ScrollWheel);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_Trigger);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_TouchMenu);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_MouseJoystick);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_MouseRegion);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_RadialMenu);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_SingleButton);
	BIND_ENUM_CONSTANT(k_EInputSourceMode_Switches);
}

EInputSourceMode SteamHLAnalogActionData::godot_GetEMode() const {
	return this->eMode;
}

void SteamHLAnalogActionData::godot_SetEMode(EInputSourceMode newMode) {
	this->eMode = newMode;
}

float SteamHLAnalogActionData::godot_GetX() const {
	return this->x;
}

void SteamHLAnalogActionData::godot_SetX(float newX) {
	this->x = newX;
}

float SteamHLAnalogActionData::godot_GetY() const {
	return this->y;
}

void SteamHLAnalogActionData::godot_SetY(float newY) {
	this->y = newY;
}

Vector2 SteamHLAnalogActionData::GetXY() const {
	return Vector2::Vector2(this->x, this->y);
}

void SteamHLAnalogActionData::SetXY(const Vector2 &newXY) {
	this->x = newXY.x;
	this->y = newXY.y;
}

bool SteamHLAnalogActionData::godot_GetBActive() const {
	return this->bActive;
}

void SteamHLAnalogActionData::godot_SetBActive(bool newActive) {
	this->bActive = newActive;
}