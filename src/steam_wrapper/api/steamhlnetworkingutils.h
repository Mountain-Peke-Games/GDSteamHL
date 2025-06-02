#pragma once

#include <src/steamhl.h>

#include <godot_cpp/core/object.hpp>

#define steamNetworkingUtils_LOAD_OR_ERROR(errMsg) STEAMHL_LOAD_OR_ERROR(steamNetworkingUtils, SteamNetworkingUtils, errMsg);
#define steamNetworkingUtils_LOAD_OR_RETURN(failureValue) STEAMHL_LOAD_OR_RETURN(steamNetworkingUtils, SteamNetworkingUtils, failureValue);

VARIANT_ENUM_CAST(ESteamNetworkingAvailability);

namespace godot {
	class SteamHLNetworkingUtils : public Object {
		GDCLASS(SteamHLNetworkingUtils, Object);

	public:
		static ESteamNetworkingAvailability GetRelayNetworkStatus();
		static void InitRelayNetworkAccess();
		
	protected:
		const static char* name;

		static void _bind_methods();
	};
}
