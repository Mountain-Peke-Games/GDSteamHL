#ifndef STEAMHLUSERSTATS_H
#define STEAMHLUSERSTATS_H
#include "steamhl.h"

#include <steam_api.h>

#define steamHLUserStats_LOAD_OR_ERROR(errMsg) STEAMHL_LOAD_OR_ERROR(steamUserStats, SteamUserStats, errMsg);
#define steamHLUserStats_LOAD_OR_RETURN(failureValue) STEAMHL_LOAD_OR_RETURN(steamUserStats, SteamUserStats, failureValue);

namespace godot {
	class SteamHLUserStats : public Object {
		GDCLASS(SteamHLUserStats, Object);

	public:
		SteamHLUserStats();
		~SteamHLUserStats();

		static SteamHLUserStats* get_singleton();

		// SteamHL will attempt to initialize this automatically
		static void Initialize();

		// TODO = Better handling of aynchronous APIs
		bool RequestCurrentStats();
		bool SetAchievement(const String& name);
		bool GetAchievement(const String& name);

		/* Steam Callbacks */
		STEAM_CALLBACK(SteamHLUserStats, onUserStatsReceived, UserStatsReceived_t);
		
	protected:
		static bool is_initialized;

		static void _bind_methods();

	private:
		static SteamHLUserStats* _singleton;
	};
}
#endif // !STEAMHLUSERSTATS_H
