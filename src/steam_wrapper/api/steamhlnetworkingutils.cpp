#include "steamhlnetworkingutils.h"

using namespace godot;

const char* SteamHLNetworkingUtils::name = "SteamHLNetworkingUtils";

void SteamHLNetworkingUtils::_bind_methods() {
	ClassDB::bind_static_method(SteamHLNetworkingUtils::name, D_METHOD("get_relay_network_status"), SteamHLNetworkingUtils::GetRelayNetworkStatus);
	ClassDB::bind_static_method(SteamHLNetworkingUtils::name, D_METHOD("init_relay_network_access"), SteamHLNetworkingUtils::InitRelayNetworkAccess);

	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability_CannotTry);
	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability_Failed);
	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability_Previously);
	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability_Retrying);
	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability_NeverTried);
	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability_Waiting);
	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability_Attempting);
	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability_Current);
	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability_Unknown);
	BIND_ENUM_CONSTANT(k_ESteamNetworkingAvailability__Force32bit);
}

ESteamNetworkingAvailability SteamHLNetworkingUtils::GetRelayNetworkStatus() {
	steamNetworkingUtils_LOAD_OR_RETURN(k_ESteamNetworkingAvailability_Unknown);
	return steamNetworkingUtils->GetRelayNetworkStatus(NULL); // TODO=Make sure NULL is safe to use here
}

void SteamHLNetworkingUtils::InitRelayNetworkAccess() {
	steamNetworkingUtils_LOAD_OR_ERROR("Failed to load SteamNetworkingUtils");
	steamNetworkingUtils->InitRelayNetworkAccess();
}