#include "steamhlapps.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

const char* SteamHLApps::name = "SteamHLApps"; // TODO=Figure out how to make this a StringName. I can get it to compile with StringName now, but Godot will fail to load the dll (presumably a crash occurs). This happes regardless of it is const or not.


void SteamHLApps::_bind_methods() {
	ClassDB::bind_static_method(SteamHLApps::name, D_METHOD("is_subscribed"), &SteamHLApps::IsSubscribed);
	ClassDB::bind_static_method(SteamHLApps::name, D_METHOD("is_subscribed_app", "app_id"), &SteamHLApps::IsSubscribedApp);
}

bool SteamHLApps::IsSubscribed() {
	steamApps_LOAD_OR_RETURN(false);
	return steamApps->BIsSubscribed();
}

bool SteamHLApps::IsSubscribedApp(uint32_t appId) {
	steamApps_LOAD_OR_RETURN(false);
	return steamApps->BIsSubscribedApp(static_cast<uint32>(appId));
}