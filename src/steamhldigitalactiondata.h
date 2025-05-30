#ifndef STEAMHLDIGITALACTIONDATA_H
#define STEAMHLDIGITALACTIONDATA_H

#include <godot_cpp/classes/ref_counted.hpp>

#include <steam_api.h>

namespace godot {
	struct SteamHLDigitalActionData : public RefCounted, InputDigitalActionData_t {
		GDCLASS(SteamHLDigitalActionData, RefCounted);

	public:
		SteamHLDigitalActionData();
		SteamHLDigitalActionData(InputDigitalActionData_t inputDigitalAction);
		~SteamHLDigitalActionData();

		bool godot_GetBState() const;
		void godot_SetBState(bool newState);

		bool godot_GetBActive() const;
		void godot_SetBActive(bool newActive);

		// TODO=Overload equality
	protected:
		static void _bind_methods();
	};
}

#endif // !STEAMHLDIGITALACTIONDATA_H