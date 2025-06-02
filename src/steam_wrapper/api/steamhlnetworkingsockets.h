#pragma once

#include <src/steamhl.h>

#include <steam_api.h>

#define steamNetworkingSockets_LOAD_OR_RETURN(failureValue) STEAMHL_LOAD_OR_RETURN(steamNetworkingSockets, SteamNetworkingSockets, failureValue);

// TODO = Consider exposing to Godot
namespace godot {
	class SteamHLNetworkingSockets {
		
	public:
		static HSteamListenSocket createListenSocketIP(const SteamNetworkingIPAddr& localAddress, int nOptions, const SteamNetworkingConfigValue_t* pOptions);
		static HSteamNetConnection connectByIPAddress(const SteamNetworkingIPAddr& address, int nOptions, const SteamNetworkingConfigValue_t* pOptions);
		static HSteamListenSocket createListenSocketP2P(int nVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions);
		static HSteamNetConnection connectP2P(const SteamNetworkingIdentity& identityRemote, int nVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions);
		static EResult acceptConnection(HSteamNetConnection hConn);
		static bool closeConnection(HSteamNetConnection hPeer, int nReason, const char* pszDebug, bool bEnableLinger);
		static bool closeListenSocket(HSteamListenSocket hSocket);
		static bool createSocketPair(HSteamNetConnection* pOutConnection1, HSteamNetConnection* pOutConnection2, bool bUseNetworkLoopback, const SteamNetworkingIdentity* pIdentity1, const SteamNetworkingIdentity* pIdentity2);

		static EResult sendMessageToConnection(HSteamNetConnection hConn, const void* pData, uint32 cbData, int nSendFlags, int64* pOutMessageNumber);
		static void sendMessages(int nMessages, SteamNetworkingMessage_t* const* pMessages, int64* pOutMessageNumberOrResult);
		static EResult flushMessagesOnConnection(HSteamNetConnection hConn);
		static int receiveMessagesOnConnection(HSteamNetConnection hConn, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages);

		static HSteamNetPollGroup createPollGroup();
		static bool destroyPollGroup(HSteamNetPollGroup hPollGroup);
		static bool setConnectionPollGroup(HSteamNetConnection netConn, HSteamNetPollGroup hPollGroup);
		static int receiveMessagesOnPollGroup(HSteamNetPollGroup hPollGroup, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages);
	};
}
