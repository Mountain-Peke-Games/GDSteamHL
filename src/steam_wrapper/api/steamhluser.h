#pragma once

#include <src/steamhl.h>
#include <src/steam_wrapper/data_structures/steamhlid.h>

#include <godot_cpp/core/class_db.hpp>

#include <steam_api.h>

#define steamUser_LOAD_OR_RETURN(failureValue) STEAMHL_LOAD_OR_RETURN(steamUser, SteamUser, failureValue);

namespace godot {
	class SteamHLUser : public Object {
		GDCLASS(SteamHLUser, Object);

	public:
		static CSteamID getSteamId();
		static SteamHLID* godot_getSteamId();

	protected:
		const static char* name;
		static void _bind_methods();
	};
}