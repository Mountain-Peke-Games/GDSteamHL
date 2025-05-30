#include "inputeventsteamhlanalogaction.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

InputEventSteamHLAnalogAction::InputEventSteamHLAnalogAction() { }

InputEventSteamHLAnalogAction::InputEventSteamHLAnalogAction(uint64_t controllerHandle, uint64_t actionHandle) {
	this->controllerHandle = controllerHandle;
	this->actionHandle = actionHandle;
}

InputEventSteamHLAnalogAction::~InputEventSteamHLAnalogAction() { }

void InputEventSteamHLAnalogAction::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_controller_handle"), &InputEventSteamHLAnalogAction::godot_GetControllerHandle);
	ClassDB::bind_method(D_METHOD("set_controller_handle"), &InputEventSteamHLAnalogAction::godot_SetControllerHandle);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "controller_handle"), "set_controller_handle", "get_controller_handle");

	ClassDB::bind_method(D_METHOD("get_action_handle"), &InputEventSteamHLAnalogAction::godot_GetActionHandle);
	ClassDB::bind_method(D_METHOD("set_action_handle"), &InputEventSteamHLAnalogAction::godot_SetActionHandle);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "action_handle"), "set_action_handle", "get_action_handle");
	
	ClassDB::bind_method(D_METHOD("get_x"), &InputEventSteamHLAnalogAction::godot_GetX);
	ClassDB::bind_method(D_METHOD("set_x"), &InputEventSteamHLAnalogAction::godot_SetX);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "x"), "set_x", "get_x");

	ClassDB::bind_method(D_METHOD("get_y"), &InputEventSteamHLAnalogAction::godot_GetY);
	ClassDB::bind_method(D_METHOD("set_y"), &InputEventSteamHLAnalogAction::godot_SetY);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "y"), "set_y", "get_y");

	ClassDB::bind_method(D_METHOD("get_xy"), &InputEventSteamHLAnalogAction::godot_GetXY);
	ClassDB::bind_method(D_METHOD("set_xy"), &InputEventSteamHLAnalogAction::godot_SetXY);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "xy"), "set_xy", "get_xy");
}

uint64_t InputEventSteamHLAnalogAction::godot_GetControllerHandle() const {
	return this->controllerHandle;
}

void InputEventSteamHLAnalogAction::godot_SetControllerHandle(uint64_t handle) {
	this->controllerHandle = handle;
}

uint64_t InputEventSteamHLAnalogAction::godot_GetActionHandle() const {
	return this->actionHandle;
}

void InputEventSteamHLAnalogAction::godot_SetActionHandle(uint64_t handle) {
	this->actionHandle = handle;
}

float InputEventSteamHLAnalogAction::godot_GetX() const {
	return this->x;
}

void InputEventSteamHLAnalogAction::godot_SetX(float value) {
	this->x = value;
}

float InputEventSteamHLAnalogAction::godot_GetY() const {
	return this->y;
}

void InputEventSteamHLAnalogAction::godot_SetY(float value) {
	this->y = value;
}

Vector2 InputEventSteamHLAnalogAction::godot_GetXY() const {
	return Vector2(this->x, this->y);
}

void InputEventSteamHLAnalogAction::godot_SetXY(Vector2 value) {
	this->x = value.x;
	this->y = value.y;
}