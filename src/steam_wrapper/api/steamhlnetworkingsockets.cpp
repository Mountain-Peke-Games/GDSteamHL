#include "steamhlnetworkingsockets.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

HSteamListenSocket SteamHLNetworkingSockets::createListenSocketIP(const SteamNetworkingIPAddr& localAddress, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
	steamNetworkingSockets_LOAD_OR_RETURN(k_HSteamListenSocket_Invalid);
	return steamNetworkingSockets->CreateListenSocketIP(localAddress, nOptions, pOptions);
}

HSteamNetConnection SteamHLNetworkingSockets::connectByIPAddress(const SteamNetworkingIPAddr& address, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
	steamNetworkingSockets_LOAD_OR_RETURN(k_HSteamNetConnection_Invalid);
	return steamNetworkingSockets->ConnectByIPAddress(address, nOptions, pOptions);
}

HSteamListenSocket SteamHLNetworkingSockets::createListenSocketP2P(int nVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
	steamNetworkingSockets_LOAD_OR_RETURN(k_HSteamListenSocket_Invalid);
	return steamNetworkingSockets->CreateListenSocketP2P(nVirtualPort, nOptions, pOptions);
}

HSteamNetConnection SteamHLNetworkingSockets::connectP2P(const SteamNetworkingIdentity& identityRemote, int nVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
	steamNetworkingSockets_LOAD_OR_RETURN(k_HSteamNetConnection_Invalid);
	return steamNetworkingSockets->ConnectP2P(identityRemote, nVirtualPort, nOptions, pOptions);
}

EResult SteamHLNetworkingSockets::acceptConnection(HSteamNetConnection hConn) {
	steamNetworkingSockets_LOAD_OR_RETURN(k_EResultInvalidState);
	return steamNetworkingSockets->AcceptConnection(hConn);
}

bool SteamHLNetworkingSockets::closeConnection(HSteamNetConnection hPeer, int nReason, const char* pszDebug, bool bEnableLinger) {
	steamNetworkingSockets_LOAD_OR_RETURN(false);
	return steamNetworkingSockets->CloseConnection(hPeer, nReason, pszDebug, bEnableLinger);
}

bool SteamHLNetworkingSockets::closeListenSocket(HSteamListenSocket hSocket) {
	steamNetworkingSockets_LOAD_OR_RETURN(false);
	return steamNetworkingSockets->CloseListenSocket(hSocket);
}

bool SteamHLNetworkingSockets::createSocketPair(HSteamNetConnection* pOutConnection1, HSteamNetConnection* pOutConnection2, bool bUseNetworkLoopback, const SteamNetworkingIdentity* pIdentity1, const SteamNetworkingIdentity* pIdentity2) {
	steamNetworkingSockets_LOAD_OR_RETURN(false);
	return steamNetworkingSockets->CreateSocketPair(pOutConnection1, pOutConnection2, bUseNetworkLoopback, pIdentity1, pIdentity2);
}

EResult SteamHLNetworkingSockets::sendMessageToConnection(HSteamNetConnection hConn, const void* pData, uint32 cbData, int nSendFlags, int64* pOutMessageNumber) {
	steamNetworkingSockets_LOAD_OR_RETURN(k_EResultInvalidState);
	return steamNetworkingSockets->SendMessageToConnection(hConn, pData, cbData, nSendFlags, pOutMessageNumber);
}

void SteamHLNetworkingSockets::sendMessages(int nMessages, SteamNetworkingMessage_t* const* pMessages, int64* pOutMessageNumberOrResult) {
	ISteamNetworkingSockets* steamNetworkingSockets = SteamNetworkingSockets();
	if (steamNetworkingSockets == NULL) {
		for (int64* msgResult = pOutMessageNumberOrResult; msgResult < (pOutMessageNumberOrResult+nMessages); msgResult++) {
			*msgResult = k_EResultInvalidState;
		}
	}
	else {
		steamNetworkingSockets->SendMessages(nMessages, pMessages, pOutMessageNumberOrResult);
	}
}

EResult SteamHLNetworkingSockets::flushMessagesOnConnection(HSteamNetConnection hConn) {
	steamNetworkingSockets_LOAD_OR_RETURN(k_EResultIgnored);
	return steamNetworkingSockets->FlushMessagesOnConnection(hConn);
}

int SteamHLNetworkingSockets::receiveMessagesOnConnection(HSteamNetConnection hConn, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages) {
	steamNetworkingSockets_LOAD_OR_RETURN(-1);
	return steamNetworkingSockets->ReceiveMessagesOnConnection(hConn, ppOutMessages, nMaxMessages);
}

HSteamNetPollGroup SteamHLNetworkingSockets::createPollGroup() {
	steamNetworkingSockets_LOAD_OR_RETURN(k_HSteamNetPollGroup_Invalid);
	return steamNetworkingSockets->CreatePollGroup();
}

bool SteamHLNetworkingSockets::destroyPollGroup(HSteamNetPollGroup hPollGroup) {
	steamNetworkingSockets_LOAD_OR_RETURN(false);
	return steamNetworkingSockets->DestroyPollGroup(hPollGroup);
}

bool SteamHLNetworkingSockets::setConnectionPollGroup(HSteamNetConnection netConn, HSteamNetPollGroup hPollGroup) {
	steamNetworkingSockets_LOAD_OR_RETURN(false);
	return steamNetworkingSockets->SetConnectionPollGroup(netConn, hPollGroup);
}

int SteamHLNetworkingSockets::receiveMessagesOnPollGroup(HSteamNetPollGroup hPollGroup, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages) {
	steamNetworkingSockets_LOAD_OR_RETURN(-1);
	return steamNetworkingSockets->ReceiveMessagesOnPollGroup(hPollGroup, ppOutMessages, nMaxMessages);
}