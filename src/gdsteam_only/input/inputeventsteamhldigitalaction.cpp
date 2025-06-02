#include "inputeventsteamhldigitalaction.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

InputEventSteamHLDigitalAction::InputEventSteamHLDigitalAction() { }

InputEventSteamHLDigitalAction::~InputEventSteamHLDigitalAction() { }

void InputEventSteamHLDigitalAction::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_controller_handle"), &InputEventSteamHLDigitalAction::godot_GetControllerHandle);
	ClassDB::bind_method(D_METHOD("set_controller_handle"), &InputEventSteamHLDigitalAction::godot_SetControllerHandle);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "controller_handle"), "set_controller_handle", "get_controller_handle");

	ClassDB::bind_method(D_METHOD("get_action_handle"), &InputEventSteamHLDigitalAction::godot_GetActionHandle);
	ClassDB::bind_method(D_METHOD("set_action_handle"), &InputEventSteamHLDigitalAction::godot_SetActionHandle);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "action_handle"), "set_action_handle", "get_action_handle");
}

uint64_t InputEventSteamHLDigitalAction::godot_GetControllerHandle() const {
	return this->controllerHandle;
}

void InputEventSteamHLDigitalAction::godot_SetControllerHandle(uint64_t handle) {
	this->controllerHandle = handle;
}

uint64_t InputEventSteamHLDigitalAction::godot_GetActionHandle() const {
	return this->actionHandle;
}

void InputEventSteamHLDigitalAction::godot_SetActionHandle(uint64_t handle) {
	this->actionHandle = handle;
}