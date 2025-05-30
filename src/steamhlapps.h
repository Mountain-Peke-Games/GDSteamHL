#ifndef STEAMHLAPPS_H
#define STEAMHLAPPS_H
#include "steamhl.h"

#include <godot_cpp/core/object.hpp>

#define steamApps_LOAD_OR_ERROR(errMsg) STEAMHL_LOAD_OR_ERROR(steamApps, SteamApps, errMsg);
#define steamApps_LOAD_OR_RETURN(failureValue) STEAMHL_LOAD_OR_RETURN(steamApps, SteamApps, failureValue);

namespace godot {
	class SteamHLApps : public Object {
		GDCLASS(SteamHLApps, Object);

	public:
		static bool IsSubscribed();
		static bool IsSubscribedApp(uint32_t appId);

	protected:
		const static char* name;
		static void _bind_methods();
	};
}

#endif // !STEAMHLAPPS_H
