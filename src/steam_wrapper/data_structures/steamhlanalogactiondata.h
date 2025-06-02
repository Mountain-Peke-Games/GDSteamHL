#pragma once

#include <steam_api.h>
#include <godot_cpp/classes/ref_counted.hpp>

VARIANT_ENUM_CAST(EInputSourceMode);

namespace godot {
	struct SteamHLAnalogActionData : public RefCounted, InputAnalogActionData_t {
		GDCLASS(SteamHLAnalogActionData, RefCounted);

	public:
		SteamHLAnalogActionData();
		SteamHLAnalogActionData(InputAnalogActionData_t inputAnalogAction);
		~SteamHLAnalogActionData();

		EInputSourceMode godot_GetEMode() const;
		void godot_SetEMode(EInputSourceMode newMode);

		float godot_GetX() const;
		void godot_SetX(float newX);
		
		float godot_GetY() const;
		void godot_SetY(float newY);
		
		Vector2 GetXY() const;
		void SetXY(const Vector2 &newXY);
		
		bool godot_GetBActive() const;
		void godot_SetBActive(bool newActive);

		// TODO=Overload equality
	protected:
		static void _bind_methods();
	};
}