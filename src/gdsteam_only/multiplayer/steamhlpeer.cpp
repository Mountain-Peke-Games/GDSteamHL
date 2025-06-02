#include "steamhlpeer.h"

#include <src/steam_wrapper/api/steamhlmatchmaking.h>
#include <src/steam_wrapper/api/steamhlnetworkingsockets.h>
#include <src/steam_wrapper/api/steamhluser.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

const char* SteamHLPeer::lobbySignal = "lobby_changed";
const char* SteamHLPeer::peerConnectedSignal = "peer_connected";
const char* SteamHLPeer::peerDisconnectedSignal = "peer_disconnected";
const char* SteamHLPeer::createLobbyResult = "create_lobby_result";
const char* SteamHLPeer::joinLobbyResult = "join_lobby_result";

void SteamHLPeer::_bind_methods() {
	ADD_SIGNAL(MethodInfo(SteamHLPeer::lobbySignal, PropertyInfo(Variant::OBJECT, "new_lobby"))); // TODO=Have property hint use SteamHLID::name for class_name
	ADD_SIGNAL(MethodInfo(SteamHLPeer::createLobbyResult, PropertyInfo(Variant::INT, "result", PROPERTY_HINT_ENUM, "OK:1,Fail:2,No Connection:3,Access Denied:15,Timeout:16,Limit Exceeded:25,")));
	ADD_SIGNAL(MethodInfo(SteamHLPeer::joinLobbyResult, PropertyInfo(Variant::INT, "result", PROPERTY_HINT_ENUM, "Success:1, Doesn't Exist:2, Not Allowed:3, Full:4, Error:5, Banned:6, Limited:7, ClanDisabled:8, CommunityBan:9, BlockedYou:10, BlockedMember:11, RatelimitExceeded:15,")));

	ClassDB::bind_method(D_METHOD("create_server", "max_players"), &SteamHLPeer::createServer);
	ClassDB::bind_method(D_METHOD("create_client", "lobby_id"), &SteamHLPeer::createClient);
	ClassDB::bind_method(D_METHOD("get_lobby_id"), &SteamHLPeer::getLobbyId);
}

const int DEFAULT_BUFFER_SIZE = 100; // TODO=Research this
SteamHLPeer::SteamHLPeer() : SteamHLPeer(DEFAULT_BUFFER_SIZE) { };

SteamHLPeer::SteamHLPeer(int p_maxBufferSize) {
	this->isClosing = false;

	// TODO: if anything breaks, this new block is why
	this->isServer = false;
	this->transferChannel = 0;
	this->transferMode = TRANSFER_MODE_RELIABLE;

	this->connectionState = k_ESteamNetworkingConnectionState_None;
	this->lobbyId = k_steamIDNil;
	this->serverConnection = k_HSteamNetConnection_Invalid;
	this->serverSteamID = k_steamIDNil;
	this->listeningSocket = k_HSteamListenSocket_Invalid;
	this->remainingMsgsInBuffer = 0;
	this->refuseNewConnections = false;
	this->maxBufferSize = p_maxBufferSize;
	this->msgBuffer = memnew_arr(SteamNetworkingMessage_t*, p_maxBufferSize);
	this->msgBufferCurrentMsgIndex = 0;

	this->targetPeer = TARGET_PEER_BROADCAST; // Default for MultiplayerPeer
	this->pollGroup = SteamHLNetworkingSockets::createPollGroup();
}

SteamHLPeer::~SteamHLPeer() {
	this->_close();
	memdelete_arr(this->msgBuffer); // TODO=I live in fear of memory leaks
}

void SteamHLPeer::createClient(SteamHLID* lobbyId) {
	ERR_FAIL_COND_MSG(this->listeningSocket != k_HSteamListenSocket_Invalid, "SteamHLPeer is already configured to be a server");
	ERR_FAIL_COND_MSG(this->lobbyId != k_steamIDNil, "SteamHLPeer is already connected to a lobby");

	this->isServer = false;

	SteamAPICall_t apiCall;
	ERR_FAIL_COND_MSG(!SteamHLMatchmaking::joinLobby(lobbyId->steamId, &apiCall), "Failed to call SteamHLMatchmaking::joinLobby()");
	this->connectionState = k_ESteamNetworkingConnectionState_Connecting;
	this->joinLobbyCallResult.Set(apiCall, this, &SteamHLPeer::onJoinLobbyResult);
}


void SteamHLPeer::createServer(int maxPlayers) {
	ERR_FAIL_COND_MSG(this->listeningSocket != k_HSteamListenSocket_Invalid, "SteamHLPeer is already configured to be a server");
	ERR_FAIL_COND_MSG(this->lobbyId != k_steamIDNil, "SteamHLPeer is already connected to a lobby");

	this->isServer = true;

	HSteamListenSocket newListeningSocket = SteamHLNetworkingSockets::createListenSocketP2P(0, 0, NULL); // TODO=Make sure NULL is safe
	ERR_FAIL_COND_MSG(newListeningSocket == k_HSteamListenSocket_Invalid, "Unable to create Steam listening socket.");
	UtilityFunctions::print("Created listening socket ", newListeningSocket);
	this->listeningSocket = newListeningSocket;
	
	SteamAPICall_t apiCall;
	UtilityFunctions::print("Attempting to create lobby...");
	ERR_FAIL_COND_MSG(!SteamHLMatchmaking::createLobby(k_ELobbyTypeFriendsOnly, maxPlayers, &apiCall), "Failed to call SteamHLMatchmaking::createLobby()");
	this->connectionState = k_ESteamNetworkingConnectionState_Connecting;
	this->createLobbyCallResult.Set(apiCall, this, &SteamHLPeer::onCreateLobbyResult);
}

void SteamHLPeer::_changeLobby(CSteamID newLobby) {
	if (this->lobbyId != k_steamIDNil) {
		SteamHLMatchmaking::leaveLobby(this->lobbyId);
	}
	this->lobbyId = newLobby;
	//SteamHLID* steamHLId = memnew(SteamHLID); // TODO=I live in fear of memory leaks
	//steamHLId->steamId = newLobby;
	Ref<SteamHLID> steamHLId(memnew(SteamHLID(newLobby)));
	EMIT_SIGNAL_SAFE(SteamHLPeer::lobbySignal, steamHLId);
	//this->emit_signal(SteamHLPeer::lobbySignal, steamHLID);
}

SteamHLID* SteamHLPeer::getLobbyId() const {
	SteamHLID* steamHLId = memnew(SteamHLID); // TODO=I live in fear of memory leaks
	steamHLId->steamId = this->lobbyId;
	return steamHLId;
}

Error SteamHLPeer::_readAndBufferIfEmpty() {
	if (this->remainingMsgsInBuffer > 0) {
		return OK; // Already buffered
	}
	else {
		int totalMsgs = SteamHLNetworkingSockets::receiveMessagesOnPollGroup(this->pollGroup, this->msgBuffer, this->maxBufferSize);
		this->msgBufferCurrentMsgIndex = 0;
		this->remainingMsgsInBuffer = totalMsgs;

		if (totalMsgs >= 0) {
			return OK;
		}
		else {
			return FAILED;
		}
	}
}

// TODO=Add safety for if packet is invalid (currently will cause a SEGFAULT when Release is called on it)
Error SteamHLPeer::_get_packet(const uint8_t** r_buffer, int32_t* r_buffer_size) {
	if (this->remainingMsgsInBuffer <= 0) {
		return ERR_DOES_NOT_EXIST;
	}
	else {
		SteamNetworkingMessage_t* currentMsg = this->msgBuffer[this->msgBufferCurrentMsgIndex];
		(this->msgBufferCurrentMsgIndex)++; // Use pop and peek functions instead
		(this->remainingMsgsInBuffer)--;

		*r_buffer_size = static_cast<uint32_t>(currentMsg->GetSize());
		uint8_t* copybuffer = memnew_arr(uint8_t, *r_buffer_size);
		const uint8_t* steamDataBegin = static_cast<const uint8_t*>(currentMsg->GetData());
		const uint8_t* steamDataEnd = steamDataBegin + (*r_buffer_size);
		std::copy(steamDataBegin, steamDataEnd, copybuffer); // Copy the data so that Steam can handle the freeing using the normal message::Release() handling and Godot can do whatever it does
		*r_buffer = copybuffer; // Actually assign the copy so it will be used
		// TODO = Sanity-check that Godot frees the array (if not, I'll have to wrap it in Ref or something)

		currentMsg->Release();
		return OK;
	}
}

Error SteamHLPeer::_put_packet(const uint8_t* p_buffer, int32_t p_buffer_size) {
	Error ret;
	if (this->targetPeer == TARGET_PEER_BROADCAST) {
		ret = this->__broadcastPacket(p_buffer, p_buffer_size);
		return ret;
	}
	else if (this->targetPeer == TARGET_PEER_SERVER) {
		ret = this->__serverPacket(p_buffer, p_buffer_size); // TODO=Check that Godot allows for multiple servers
		return ret;
	}
	else {
		if (this->targetPeer >= 0) {
			ret = this->__targetPacket(this->targetPeer, p_buffer, p_buffer_size);
			return ret;
		}
		else {
			ret = this->__excludePacket(-(this->targetPeer), p_buffer, p_buffer_size);
			return ret;
		}
	}
}

Error SteamHLPeer::__broadcastPacket(const uint8_t* p_buffer, int32_t p_buffer_size, int32_t* ignoreId) {
	Error clientError = this->__clientPacket(p_buffer, p_buffer_size, ignoreId);
	Error serverError = this->__serverPacket(p_buffer, p_buffer_size, ignoreId);
	if (clientError == OK || serverError == OK) {
		return OK; // TODO=Partial success
	}
	else {
		return FAILED;
	}
}


Error SteamHLPeer::__clientPacket(const uint8_t* p_buffer, int32_t p_buffer_size, int32_t* ignoreId) {
	bool oneSuccess = false;

	HSteamNetConnection* connectionToIgnore = NULL;
	if (ignoreId != NULL && this->idConnectionMap.has(*ignoreId)) {
		connectionToIgnore = this->idConnectionMap.getptr(*ignoreId);
	}

	// TODO=Replaces with plural Messages API
	for (HashSet<HSteamNetConnection>::Iterator iter = this->clientConnections.begin(); iter != this->clientConnections.end(); ++iter) {
		HSteamNetConnection connection = *iter;
		if ((connectionToIgnore != NULL) && (connection == *connectionToIgnore)) {
			continue;
		}

		Error singleResult = this->__connectionPacket(connection, p_buffer, p_buffer_size);

		if (singleResult == OK) {
			oneSuccess = true;
		}
		else {
			// TODO = Differentiate between errors and disconnect if the connection has ended
		}
	}

	if (oneSuccess) {
		return OK; // TODO=Partial success
	}
	else {
		return FAILED;
	}
}

Error SteamHLPeer::__serverPacket(const uint8_t* p_buffer, int32_t p_buffer_size, int32_t* ignoreId) {
	if (ignoreId != NULL && *ignoreId == this->serverConnection) {
		return OK;
	}
	else {
		return this->__connectionPacket(this->serverConnection, p_buffer, p_buffer_size);
	}
}

Error SteamHLPeer::__targetPacket(int32_t peerId, const uint8_t* p_buffer, int32_t p_buffer_size) {
	int32_t id32 = peerId;
	if (this->idConnectionMap.has(id32)) {
		HSteamNetConnection connection = this->idConnectionMap.get(id32);
		return this->__connectionPacket(connection, p_buffer, p_buffer_size);
	}
	else {
		UtilityFunctions::print("Failed to find ", peerId, " in idConnectionMap@Size=", this->idConnectionMap.size());
		UtilityFunctions::print("Failed to find ", peerId, " in idConnectionMap@Size=", this->idConnectionMap.size());
		return ERR_DOES_NOT_EXIST;
	}
}

Error SteamHLPeer::__connectionPacket(HSteamNetConnection connection, const uint8_t* p_buffer, int32_t p_buffer_size) {
	uint32 bufferSize = static_cast<uint32>(p_buffer_size);
	int steamSendFlags = k_nSteamNetworkingSend_ReliableNoNagle; // TODO=Calculate based on Godot settings
	int64 assignedMessageNumber;
	EResult steamResult = SteamHLNetworkingSockets::sendMessageToConnection(connection, p_buffer, bufferSize, steamSendFlags, &assignedMessageNumber); // TODO=Maybe use message number
	if (steamResult == k_EResultOK) { // TODO=I'm not sure if OK is actually returned
		return OK;
	}
	else {
		return FAILED; // TODO = Differentiate between errors and disconnect if the connection has ended
	}
}

Error SteamHLPeer::__excludePacket(int peerId, const uint8_t* p_buffer, int32_t p_buffer_size) {
	int32_t id32 = static_cast<int32_t>(peerId);
	return this->__broadcastPacket(p_buffer, p_buffer_size, &id32);
}

int32_t SteamHLPeer::_get_available_packet_count() const {
	return static_cast<int32_t>(this->remainingMsgsInBuffer);
}

int32_t SteamHLPeer::_get_max_packet_size() const {
	return static_cast<int32_t>(k_cbMaxSteamNetworkingSocketsMessageSizeSend);
}

void SteamHLPeer::_set_transfer_channel(int32_t p_channel) {
	this->transferChannel = p_channel; // Should be inherited from MultiplayerPeer
}

int32_t SteamHLPeer::_get_transfer_channel() const {
	return this->transferChannel; // Should be inherited from MultiplayerPeer
}

void SteamHLPeer::_set_transfer_mode(MultiplayerPeer::TransferMode p_mode) {
	this->transferMode = p_mode; // Should be inherited from MultiplayerPeer
}

MultiplayerPeer::TransferMode SteamHLPeer::_get_transfer_mode() const {
	return this->transferMode; // Should be inherited from MultiplayerPeer
}

void SteamHLPeer::_set_target_peer(int32_t p_peer) {
	this->targetPeer = p_peer; // TODO=Validate and maybe automatically calculate the Steam user if this is not broadcast or server
}

// TODO=Sanity-check that "...the MultiplayerPeer who sent the next available packet" refers to the next available packet (as written)
// or if it actually means "the packet we just read
int32_t SteamHLPeer::_get_packet_channel() const {
	if (this->remainingMsgsInBuffer <= 0) {
		return 0; // TOOD=Figure out proper error channel (this is the default channel, which is valid)
	}
	else {
		SteamNetworkingMessage_t* nextMsg = this->msgBuffer[this->msgBufferCurrentMsgIndex];
		return static_cast<int32_t>(nextMsg->GetChannel());
	}
}


MultiplayerPeer::TransferMode SteamHLPeer::_get_packet_mode() const {
	if (this->remainingMsgsInBuffer <= 0) {
		return TRANSFER_MODE_UNRELIABLE; // TOOD=Figure out proper error handling
	}
	else {
		SteamNetworkingMessage_t* nextMsg = this->msgBuffer[this->msgBufferCurrentMsgIndex];
		int sendFlags = nextMsg->m_nFlags;
		if (k_nSteamNetworkingSend_Reliable & sendFlags) {
			return TRANSFER_MODE_RELIABLE;
		}
		else {
			return TRANSFER_MODE_UNRELIABLE;
		}
	}
}

// TODO=Sanity-check that "...the MultiplayerPeer who sent the next available packet" refers to the next available packet (as written)
// or if it actually means "the packet we just read
int32_t SteamHLPeer::_get_packet_peer() const {
	if (this->remainingMsgsInBuffer <= 0) {
		UtilityFunctions::print("Failed to get peer, returning 0");
		return 0; // TOOD=Figure out proper error ID
	}
	else {
		SteamNetworkingMessage_t* nextMsg = this->msgBuffer[this->msgBufferCurrentMsgIndex];
		SteamNetworkingIdentity steamIdentity = nextMsg->m_identityPeer;
		if (steamIdentity.m_eType == k_ESteamNetworkingIdentityType_SteamID) {
			CSteamID peerSteamID = steamIdentity.GetSteamID();
			if (peerSteamID == this->serverSteamID) {
				return 1; // TODO=Forced to return 1 for server ID (see is_server and get_unique_id)
			}
			else {
				return static_cast<int32_t>(peerSteamID.GetAccountID());
			}
		}
		else {
			// TODO = Add support for other types
			UtilityFunctions::print("Peer was not a SteamID, returning 0. Instead, it was ", steamIdentity.m_eType);
			return 0; // TODO = Figure out proper error ID
		}
	}
}

// TODO=This is not used as I expect it to because multiplayer.is_server() either doesn't call this or has some prior condition before it calls it.
// It is not used: multiplayer_api.h -> is_server() { return get_unique_id() == MultiplayerPeer::TARGET_PEER_SERVER; }
bool SteamHLPeer::_is_server() const {
	return this->isServer;
	//return this->listeningSocket != k_HSteamListenSocket_Invalid;
}

void SteamHLPeer::_poll() {
	this->_readAndBufferIfEmpty(); // TODO=Make sure this poll will actually buffer the data before other functions start trying to get packets from the buffer
}

void SteamHLPeer::_close() {
	if (this->isClosing) {
		return;
	}
	this->isClosing = true;

	this->serverSteamID = k_steamIDNil;
	this->connectionState = k_ESteamNetworkingConnectionState_None;

	if (this->lobbyId != k_steamIDNil) {
		/*
		We must suppress the signal otherwise we emit it after the GDScript multiplayer accessor has been unregistered. (At least that's the working theory)
		If a GDScript attempts to use the multiplayer object, the program will crash.
		This only occurs during program shutdown.

		TODO=Sanity check it is NEVER reasonable to not emit lobbyChanged during deallocation
		(e.g., do developers expect the notification when swapping multiplayer_peer for another object?)
		*/
		this->_changeLobby(k_steamIDNil);
	}

	for (HashMap<int32_t, HSteamNetConnection>::Iterator iter = this->idConnectionMap.begin(); iter != this->idConnectionMap.end(); ++iter) {
		int32_t peerId = iter->key;
		HSteamNetConnection connection = iter->value;
		this->__closeHSteamNetConnection(peerId, connection);
	}

	if (this->pollGroup != k_HSteamNetPollGroup_Invalid) {
		SteamHLNetworkingSockets::destroyPollGroup(this->pollGroup);
		this->pollGroup = k_HSteamNetPollGroup_Invalid;
	}

	for (this->remainingMsgsInBuffer; this->remainingMsgsInBuffer > 0; this->remainingMsgsInBuffer--) {
		this->msgBuffer[this->msgBufferCurrentMsgIndex]->Release();
		(this->msgBufferCurrentMsgIndex)++;
	}

	if (this->listeningSocket != k_HSteamListenSocket_Invalid) {
		if (SteamHLNetworkingSockets::closeListenSocket(this->listeningSocket)) {
			UtilityFunctions::print("Closed listening socket ", this->listeningSocket);
		}
		else {
			UtilityFunctions::print("Failed to close listening socket ", this->listeningSocket);
		}
		this->listeningSocket = k_HSteamListenSocket_Invalid;
	}
}

void SteamHLPeer::__closeHSteamNetConnection(int32_t peerId, HSteamNetConnection connection) {
	SteamHLNetworkingSockets::setConnectionPollGroup(connection, k_HSteamNetPollGroup_Invalid);
	SteamHLNetworkingSockets::closeConnection(connection, k_ESteamNetConnectionEnd_AppException_Generic, NULL, false); // TODO=Check if NULL is the right value for optional pszDebug
	UtilityFunctions::print("Closed connection ", connection);
	this->idConnectionMap.erase(peerId); // TODO=Handle or prevent bidirection connections since that doesn't work with the hashmap
	if (connection == this->serverConnection) {
		this->serverConnection = k_HSteamNetConnection_Invalid;
		EMIT_SIGNAL_SAFE(SteamHLPeer::peerDisconnectedSignal, 1);
		//this->emit_signal(SteamHLPeer::peerDisconnectedSignal, 1); // // TODO=See get_unique_id for why Godot requires this
	}
	else {
		this->clientConnections.erase(connection);
	}
}

void SteamHLPeer::_disconnect_peer(int32_t p_peer, bool p_force) { // TODO=Double-check that signals are still sent properly
	if (this->idConnectionMap.has(p_peer)) {
		HSteamNetConnection connection = this->idConnectionMap.get(p_peer);
		this->__closeHSteamNetConnection(p_peer, connection);
		EMIT_SIGNAL_SAFE(SteamHLPeer::peerDisconnectedSignal, p_peer);
		//this->emit_signal(SteamHLPeer::peerDisconnectedSignal, p_peer);
	}
	else {
		// TODO = Error handling for when peer ID was not found
	}
}

// TODO=Maybe don't want to rely on the Steam ID in the offchance there is a shared account or it cannot load
//			Currently returns null ID when it cannot load
//		However, Steam IDs are 64bit and Godot only uses 32bit... so account ID it is!
// TODO=Must be 1 if this is the server. See _is_server() for details on the Godot code that causes this.
int32_t SteamHLPeer::_get_unique_id() const {
	if (this->isServer) {
		return 1;
	}
	else {
		CSteamID steamID = SteamHLUser::getSteamId(); // TODO=Do I need to check if this is invalid?
		return static_cast<int32_t>(steamID.GetAccountID());
	}
}

void SteamHLPeer::_set_refuse_new_connections(bool p_enable) {
	this->refuseNewConnections = p_enable;
}

bool SteamHLPeer::_is_refusing_new_connections() const {
	return this->refuseNewConnections;
}

bool SteamHLPeer::_is_server_relay_supported() const {
	return false; // TODO=Wat. (probably needed for mesh)
}

MultiplayerPeer::ConnectionStatus SteamHLPeer::_get_connection_status() const {
	if (!SteamHL::get_singleton()->isInitialized()) {
		return CONNECTION_DISCONNECTED;
	}

	if (this->connectionState == k_ESteamNetworkingConnectionState_Connected) {
		return CONNECTION_CONNECTED;
	}
	else if (this->connectionState == k_ESteamNetworkingConnectionState_Connecting || k_ESteamNetworkingConnectionState_FindingRoute) {
		return CONNECTION_CONNECTING;
	}
	else {
		return CONNECTION_DISCONNECTED;
	}
}

void SteamHLPeer::onSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* callback) {
	HSteamNetConnection connection = callback->m_hConn;
	SteamNetConnectionInfo_t connectionInfo = callback->m_info;
	ESteamNetworkingConnectionState oldState = callback->m_eOldState;
	UtilityFunctions::print("Connection status changed (", connection, "): ", oldState, "->", connectionInfo.m_eState);

	// CASE 1: Connection was closed, so we need to close and clean up our side
	if (connectionInfo.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer
		|| connectionInfo.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
		UtilityFunctions::print("Connection closed: code ", connectionInfo.m_eEndReason);
		// A connection was closed
		int32_t steamId = 0; // Invalid, which is fine
		int32_t peerId = -1;
		// TODO=Ew...
		if (connection == this->serverConnection) {
			peerId = 1; // TODO=See get_unique_id for why Godot requires this
			steamId = this->serverSteamID.GetAccountID();
		}
		else {
			for (HashMap<int32_t, HSteamNetConnection>::Iterator iter = this->idConnectionMap.begin(); iter != this->idConnectionMap.end(); ++iter) {
				HSteamNetConnection candidateConnection = iter->value;
				if (candidateConnection == connection) {
					steamId = iter->key;
					peerId = steamId; // Clients just use the SteamId
					break;
				}
			}
		}

		if (connection == this->serverConnection) {
			this->connectionState = k_ESteamNetworkingConnectionState_ClosedByPeer;
		}
		EMIT_SIGNAL_SAFE(SteamHLPeer::peerDisconnectedSignal, peerId);
		//this->emit_signal(SteamHLPeer::peerDisconnectedSignal, peerId);
		this->__closeHSteamNetConnection(steamId, connection);
	}
	// CASE 2: We're the client (no listening socket since we initiated).
	else if (connectionInfo.m_hListenSocket == k_HSteamListenSocket_Invalid) {
		this->connectionState = connectionInfo.m_eState;
		if (oldState != k_ESteamNetworkingConnectionState_Connected
			&& connectionInfo.m_eState == k_ESteamNetworkingConnectionState_Connected) {
			// Server just finished connecting.
			EMIT_SIGNAL_SAFE(SteamHLPeer::peerConnectedSignal, 1);
			//this->emit_signal(SteamHLPeer::peerConnectedSignal, 1); // TODO=See get_unique_id for why Godot requires this
		}
	}
	// CASE 3: We're the server (because there is a listening socket) have to decide if we want to accept the incoming connection.
	else if (oldState == k_ESteamNetworkingConnectionState_None
		&& connectionInfo.m_eState == k_ESteamNetworkingConnectionState_Connecting) {
		// New incoming connection request
		if (this->refuseNewConnections) {
			UtilityFunctions::print("Refusing connection...");
			SteamHLNetworkingSockets::closeConnection(connection, k_ESteamNetConnectionEnd_AppException_Generic, NULL, false); // TODO=Check if NULL is the right value for optional pszDebug
		}
		else if (this->idConnectionMap.has(connectionInfo.m_identityRemote.GetSteamID().GetAccountID())) {
			ERR_PRINT("Already connected client is attempted to connect again. Rejecting and closing current connection.");
			this->disconnect_peer(connectionInfo.m_identityRemote.GetSteamID().GetAccountID(), false); // We need force to be false since we want the disconnect signal to be emitted
			SteamHLNetworkingSockets::closeConnection(connection, k_ESteamNetConnectionEnd_AppException_Generic, NULL, false); // TODO=Check if NULL is the right value for optional pszDebug
		}
		else {
			UtilityFunctions::print("Accepting connection from client...");
			if (SteamHLNetworkingSockets::acceptConnection(connection) != k_EResultOK) {
				ERR_PRINT("Failed to accept connection");
				SteamHLNetworkingSockets::closeConnection(connection, k_ESteamNetConnectionEnd_AppException_Generic, NULL, false); // TODO=Check if NULL is the right value for optional pszDebug
			}
		}
	}
	// CASE 4: We're the server (because there is a listening socket) and the peer connected.
	else if (connectionInfo.m_eState == k_ESteamNetworkingConnectionState_Connected) {
		int32_t peerId;
		if (this->listeningSocket == k_HSteamListenSocket_Invalid) {
			UtilityFunctions::print("Connection accepted by the server");
			// Our connection request was accepted by the server
			this->serverConnection = connection; // TODO=Raise error if already connected to server
			this->serverSteamID = connectionInfo.m_identityRemote.GetSteamID();
			peerId = 1; // TODO=See get_unique_id for why Godot requires this
			// TODO = Verify steamId before allowing connection
		}
		else {
			UtilityFunctions::print("Client finished connecting");
			this->clientConnections.insert(connection);
			// TODO = Verify steamId before allowing connection
			peerId = static_cast<int32_t>(connectionInfo.m_identityRemote.GetSteamID().GetAccountID());

		}
		int32_t steamId = static_cast<int32_t>(connectionInfo.m_identityRemote.GetSteamID().GetAccountID());
		SteamHLNetworkingSockets::setConnectionPollGroup(connection, this->pollGroup);
		this->idConnectionMap.insert(steamId, connection);
		EMIT_SIGNAL_SAFE(SteamHLPeer::peerConnectedSignal, peerId);
		//this->emit_signal(SteamHLPeer::peerConnectedSignal, peerId);
	}
}

void SteamHLPeer::onSteamNetAuthenticationStatus(SteamNetAuthenticationStatus_t* callback) {
	// TODO=Probably expose a signal
}

void SteamHLPeer::onLobbyDataUpdate(LobbyDataUpdate_t* callback) {
	UtilityFunctions::print("Lobby (", callback->m_ulSteamIDLobby, ") data changed for ", callback->m_ulSteamIDMember);
}

void SteamHLPeer::onLobbyChatUpdate(LobbyChatUpdate_t* callback) {
	UtilityFunctions::print("Lobby chat data changed for ", callback->m_ulSteamIDUserChanged, ": ", callback->m_rgfChatMemberStateChange);
	CSteamID steamID = SteamHLUser::getSteamId(); // TODO=Do I need to check if this is invalid?
	if (callback->m_ulSteamIDLobby == this->lobbyId.ConvertToUint64()
		&& callback->m_ulSteamIDUserChanged == SteamHLUser::getSteamId().ConvertToUint64()
		&& callback->m_rgfChatMemberStateChange & k_EChatMemberStateChangeDisconnected) {
		WARN_PRINT("Lost connection to lobby");
		this->close();
	}
}

void SteamHLPeer::onCreateLobbyResult(LobbyCreated_t* callback, bool failed) {
	if (failed || callback->m_eResult != k_EResultOK) {
		WARN_PRINT("Failed to create Steam lobby");
		EMIT_SIGNAL_SAFE(SteamHLPeer::createLobbyResult, callback->m_eResult); // Must be before close because close blocks signals (to prevent fun issues during deallocation)
		this->close();
	}
	else {
		UtilityFunctions::print("Created lobby");
		this->connectionState = k_ESteamNetworkingConnectionState_Connected;
		this->_changeLobby(CSteamID(callback->m_ulSteamIDLobby));
		EMIT_SIGNAL_SAFE(SteamHLPeer::createLobbyResult, callback->m_eResult);
	}
	//this->emit_signal(SteamHLPeer::createLobbyResult, callback->m_eResult);
}

// TODO=Close on ERR_FAIL
void SteamHLPeer::onJoinLobbyResult(LobbyEnter_t* callback, bool failed) {
	if (this->connectionState == k_ESteamNetworkingConnectionState_Connecting) {
		if (failed || callback->m_EChatRoomEnterResponse != k_EChatRoomEnterResponseSuccess) {
			WARN_PRINT(("Failed to join Steam lobby: code " + std::to_string(callback->m_EChatRoomEnterResponse)).c_str());
			EMIT_SIGNAL_SAFE(SteamHLPeer::joinLobbyResult, callback->m_EChatRoomEnterResponse); // Must be before we close since closing blocks signals
			this->close();
		}
		else {
			UtilityFunctions::print("Connected to lobby");
			this->_changeLobby(CSteamID(callback->m_ulSteamIDLobby));

			CSteamID serverId = SteamHLMatchmaking::getLobbyOwner(this->lobbyId);
			SteamNetworkingIdentity serverIdentity = SteamNetworkingIdentity();
			serverIdentity.SetSteamID(serverId);
			UtilityFunctions::print("Connecting to server...");
			HSteamNetConnection attemptedServerConnection = SteamHLNetworkingSockets::connectP2P(serverIdentity, 0, 0, NULL); // TODO=Make sure NULL is right
			ERR_FAIL_COND_MSG(attemptedServerConnection == k_HSteamNetConnection_Invalid, "Failed to connect to server / lobby owner");

			// TODO=Remove
			this->serverConnection = attemptedServerConnection;
			this->serverSteamID = serverId;
			int32_t steamId = static_cast<int32_t>(serverId.GetAccountID());
			SteamHLNetworkingSockets::setConnectionPollGroup(attemptedServerConnection, this->pollGroup);
			this->idConnectionMap.insert(serverId.GetAccountID(), attemptedServerConnection);

			// If the connection is accepted, then onSteamNetConnectionStatus changed will capture it
			EMIT_SIGNAL_SAFE(SteamHLPeer::joinLobbyResult, callback->m_EChatRoomEnterResponse);
		}
		//this->emit_signal(SteamHLPeer::joinLobbyResult, callback->m_EChatRoomEnterResponse);
	}
	// TODO = What if the signal comes after the peer is destroyed?
	else if (!failed && (callback->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess)) {
		SteamHLMatchmaking::leaveLobby(CSteamID(callback->m_ulSteamIDLobby)); // The peer was closed while connecting to the lobby. Leave the lobby.
	}
}