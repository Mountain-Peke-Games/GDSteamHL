#ifndef INPUTEVENTSTEAMHLDIGITALACTION_H
#define INPUTEVENTSTEAMHLDIGITALACTION_H

#include <godot_cpp/classes/input_event_action.hpp>

#include <steam_api.h>

namespace godot {
	struct InputEventSteamHLDigitalAction : public InputEventAction {
		GDCLASS(InputEventSteamHLDigitalAction, InputEventAction);

	public:
		InputEventSteamHLDigitalAction();
		~InputEventSteamHLDigitalAction();

		uint64_t controllerHandle;
		uint64_t godot_GetControllerHandle() const;
		void godot_SetControllerHandle(uint64_t handle);

		uint64_t actionHandle;
		uint64_t godot_GetActionHandle() const;
		void godot_SetActionHandle(uint64_t handle);

	protected:
		static void _bind_methods();
	};
}
#endif // !INPUTEVENTSTEAMHLDIGITALACTION_H
