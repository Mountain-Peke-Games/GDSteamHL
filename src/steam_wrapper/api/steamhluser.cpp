#include "steamhluser.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

const char* SteamHLUser::name = "SteamHLUser"; // TODO=Figure out how to make this a StringName. I can get it to compile with StringName now, but Godot will fail to load the dll (presumably a crash occurs). This happes regardless of it is const or not.


void SteamHLUser::_bind_methods() {
	ClassDB::bind_static_method(SteamHLUser::name, D_METHOD("get_steam_id"), &SteamHLUser::godot_getSteamId);
}


CSteamID SteamHLUser::getSteamId() {
	steamUser_LOAD_OR_RETURN(k_steamIDNil);
	return steamUser->GetSteamID();
}

SteamHLID* SteamHLUser::godot_getSteamId() {
	SteamHLID* steamHLId = memnew(SteamHLID); // TODO=I live in fear of memory leaks
	steamHLId->steamId = SteamHLUser::getSteamId();
	return steamHLId;
}