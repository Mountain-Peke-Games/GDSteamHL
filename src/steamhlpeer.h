#ifndef STEAMHLPEER_H
#define STEAMHLPEER_H
#include <godot_cpp/classes/multiplayer_peer_extension.hpp>
#include <godot_cpp/templates/hash_set.hpp>

#include "steamhlid.h"

#define EMIT_SIGNAL_SAFE(signal, ...) if (!this->isClosing) this->emit_signal(signal, __VA_ARGS__);

namespace godot {
	class SteamHLPeer : public MultiplayerPeerExtension {
		GDCLASS(SteamHLPeer, MultiplayerPeerExtension);

		// packet_script removed because we are not implementing this via GDScript

	public:
		SteamHLPeer();
		SteamHLPeer(int p_maxBufferSize);
		~SteamHLPeer();

		/* Setup */
		void createClient(SteamHLID* lobbyId);
		void createServer(int maxPlayers);

		/* Custom Methods */
		SteamHLID* getLobbyId() const;

		/* MultiplayPeerExtension (Godot) Virtual Method Overrides */
		Error _get_packet(const uint8_t** r_buffer, int32_t* r_buffer_size) override;
		Error _put_packet(const uint8_t* p_buffer, int32_t p_buffer_size) override;
		int32_t _get_available_packet_count() const override;
		int32_t _get_max_packet_size() const override;
		void _set_transfer_channel(int32_t p_channel) override;
		int32_t _get_transfer_channel() const override;
		void _set_transfer_mode(MultiplayerPeer::TransferMode p_mode) override;
		MultiplayerPeer::TransferMode _get_transfer_mode() const override;
		void _set_target_peer(int32_t p_peer) override;
		int32_t _get_packet_channel() const override;
		MultiplayerPeer::TransferMode _get_packet_mode() const override;
		int32_t _get_packet_peer() const override;
		bool _is_server() const override;
		void _poll() override;
		void _close() override;
		void _disconnect_peer(int32_t p_peer, bool p_force) override;
		int32_t _get_unique_id() const override;
		void _set_refuse_new_connections(bool p_enable) override;
		bool _is_refusing_new_connections() const override;
		bool _is_server_relay_supported() const override;
		MultiplayerPeer::ConnectionStatus _get_connection_status() const override;

		/* Steam Callbacks */
		STEAM_CALLBACK(SteamHLPeer, onSteamNetConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);
		STEAM_CALLBACK(SteamHLPeer, onSteamNetAuthenticationStatus, SteamNetAuthenticationStatus_t);
		STEAM_CALLBACK(SteamHLPeer, onLobbyDataUpdate, LobbyDataUpdate_t);
		STEAM_CALLBACK(SteamHLPeer, onLobbyChatUpdate, LobbyChatUpdate_t);

		/* Steam CallResults */
		void onCreateLobbyResult(LobbyCreated_t* callback, bool failed);
		CCallResult< SteamHLPeer, LobbyCreated_t > createLobbyCallResult;
		void onJoinLobbyResult(LobbyEnter_t* callback, bool failed);
		CCallResult< SteamHLPeer, LobbyEnter_t > joinLobbyCallResult;

	protected:
		bool isServer;
		bool isClosing;

		const static char* peerConnectedSignal;
		const static char* peerDisconnectedSignal;
		const static char* lobbySignal;
		const static char* joinLobbyResult;
		const static char* createLobbyResult;
		static void _bind_methods();

		ESteamNetworkingConnectionState connectionState;

		int32_t transferChannel;
		MultiplayerPeer::TransferMode transferMode;
		bool refuseNewConnections;

		CSteamID lobbyId;
		HSteamListenSocket listeningSocket;
		HashMap<int32_t, HSteamNetConnection>  idConnectionMap;
		HashSet<HSteamNetConnection> clientConnections;
		CSteamID serverSteamID;
		HSteamNetConnection serverConnection;

		HSteamNetPollGroup pollGroup;
		SteamNetworkingMessage_t** msgBuffer;
		int msgBufferCurrentMsgIndex;
		int maxBufferSize;
		int remainingMsgsInBuffer;
		int targetPeer;

		Error _readAndBufferIfEmpty();
		void _changeLobby(CSteamID newLobby);

		Error __broadcastPacket(const uint8_t* p_buffer, int32_t p_buffer_size, int32_t* ignoreId = NULL);
		Error __clientPacket(const uint8_t* p_buffer, int32_t p_buffer_size, int32_t* ignoreId = NULL);
		Error __serverPacket(const uint8_t* p_buffer, int32_t p_buffer_size, int32_t* ignoreId = NULL);
		Error __targetPacket(int32_t peerId, const uint8_t* p_buffer, int32_t p_buffer_size);
		Error __connectionPacket(HSteamNetConnection connection, const uint8_t* p_buffer, int32_t p_buffer_size);
		Error __excludePacket(int peerId, const uint8_t* p_buffer, int32_t p_buffer_size);

		void __closeHSteamNetConnection(int32_t peerId, HSteamNetConnection connection);
	};
}

#endif // !STEAMHLPEER_H
