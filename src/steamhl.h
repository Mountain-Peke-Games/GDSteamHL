#pragma once

#include <steam_api.h>

#include <godot_cpp/classes/node.hpp>

#define STEAMHL_LOAD_OR_ERROR(varName,className,errMsg) I##className* varName = className(); ERR_FAIL_COND_MSG(varName == NULL, errMsg);
#define STEAMHL_LOAD_OR_RETURN(varName,className,failureValue) I##className* varName = className(); if (varName == NULL) return failureValue;

VARIANT_ENUM_CAST(EResult);

namespace godot {
	class SteamHL : public Object {
		GDCLASS(SteamHL, Object);

	public:
		SteamHL();
		~SteamHL();

		static SteamHL* get_singleton();

		void _process(); // Connected in SteamHL::initialize()

		bool initialize();
		bool isInitialized();
		void shutdown();

		bool isSteamRunning();

		// TODO = Don't do callback until Steam API is initialized (probably slam them in a separate object that isn't created until Steam is initialized)
		/* Steam Callbacks */
		STEAM_CALLBACK(SteamHL, onGameLobbyJoinRequested, GameLobbyJoinRequested_t);

	protected:
		const static char* gameLobbyJoinRequestedSignal;
		static void _bind_methods();

	private:
		static SteamHL* _singleton;
		static bool _steamApiInitialized;
	};
}