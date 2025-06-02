#pragma once

#include <godot_cpp/classes/input_event_action.hpp>

#include <steam_api.h>

namespace godot {
	struct InputEventSteamHLAnalogAction : public InputEventAction {
		GDCLASS(InputEventSteamHLAnalogAction, InputEventAction);

	public:
		InputEventSteamHLAnalogAction();
		InputEventSteamHLAnalogAction(uint64_t controllerHandle, uint64_t actionHandle);
		~InputEventSteamHLAnalogAction();

		// InputEvent has device, but it is int32 instead of int64
		uint64_t controllerHandle;
		uint64_t godot_GetControllerHandle() const;
		void godot_SetControllerHandle(uint64_t handle);

		uint64_t actionHandle;
		uint64_t godot_GetActionHandle() const;
		void godot_SetActionHandle(uint64_t handle);

		float x;
		float godot_GetX() const;
		void godot_SetX(float value);

		float y;
		float godot_GetY() const;
		void godot_SetY(float value);

		Vector2 godot_GetXY() const;
		void godot_SetXY(Vector2 value);

	protected:
		static void _bind_methods();
	};
}
