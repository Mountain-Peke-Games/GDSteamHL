extends Node2D

const ADDRESS = "localhost"
const PORT = 4433

var messaging_name = "Dave"
var message_preload = preload("res://message.tscn")

var cursor_tracker_preload = preload("res://trackable_cursor.tscn")

func _ready():
	multiplayer.connection_failed.connect(self.on_connection_failed)
	multiplayer.connected_to_server.connect(self.on_connected_to_server)
	multiplayer.server_disconnected.connect(self.on_server_disconnected)
	multiplayer.peer_connected.connect(self.on_peer_connected)
	multiplayer.peer_disconnected.connect(self.on_peer_disconnected)
	
	SteamHL.game_lobby_join_requested.connect(self.on_steam_lobby_join_request)
	
	%Host.disabled = true
	%Connect.disabled = true
	%SessionUI.visible = false
	%MessageEntry.editable = false
	%HostConnectOptions.visible = true
	if not SteamHL.initialize():
		print("Failed to initialize Steam APIs");
		%SteamToggle.button_pressed = false
		%SteamToggle.disabled = true
	else:
		%SteamToggle.disabled = false
	%DebugValues.load_values()


func _on_host_pressed():
	if %SteamToggle.button_pressed:
		self.host_steam_session()
	else:
		self.host_enet_session()

func host_steam_session():
	var peer = SteamHLPeer.new()
	peer.create_server()
	if self._shared_host_session(peer):
		peer.lobby_changed.connect(%DebugValues.on_lobby_id_changed)

func host_enet_session():
	var peer = ENetMultiplayerPeer.new()
	peer.create_server(PORT)
	self._shared_host_session(peer)

func _shared_host_session(peer: MultiplayerPeer) -> bool:
	if peer.get_connection_status() == MultiplayerPeer.CONNECTION_DISCONNECTED:
		print("Failed to start server")
		return false
	else:
		multiplayer.multiplayer_peer = peer
		%HostConnectOptions.visible = false
		%SessionUI.visible = true
		%MessageEntry.editable = true
		self.spawn_cursor(peer.get_unique_id())
		return true


func _on_connect_pressed():
	if %SteamToggle.button_pressed:
		SteamHLFriends.activate_game_overlay(0) # 0=Friends
	else:
		self.connect_to_enet_session()

func on_steam_lobby_join_request(lobby_id: SteamHLID):
	print("Attempting to join lobby...")
	self.connect_to_steam_session(lobby_id)

func connect_to_steam_session(lobby_id: SteamHLID):
	var peer = SteamHLPeer.new()
	peer.create_client(lobby_id)
	if self._shared_connect_to_session(peer):
		peer.lobby_changed.connect(%DebugValues.on_lobby_id_changed)

func connect_to_enet_session():
	var peer = ENetMultiplayerPeer.new()
	peer.create_client(ADDRESS, PORT)
	self._shared_connect_to_session(peer)

func _shared_connect_to_session(peer: MultiplayerPeer) -> bool:
	if peer.get_connection_status() == MultiplayerPeer.CONNECTION_DISCONNECTED:
		print("Failed to connect to session")
		return false
	else:
		multiplayer.multiplayer_peer = peer
		%HostConnectOptions.visible = false
		%SessionUI.visible = true
		%MessageEntry.editable = true
		# The cursor is instantiated by the server once the connection is made
		return true


func spawn_cursor(peer_id):
	var cursor = self.cursor_tracker_preload.instantiate()
	cursor.set_name("cursor_"+str(peer_id))
	%CursorTrackers.add_child(cursor)
	cursor.rpc("change_authority", peer_id)


func _on_message_entry_text_submitted(new_text):
	%MessageEntry.clear()
	rpc("add_message", self.messaging_name, new_text)

@rpc("call_local","reliable","any_peer")
func add_message(new_name, new_text):
	var new_message : Message = message_preload.instantiate()
	new_message.set_message_name(new_name)
	new_message.set_message_value(new_text)
	%MessagesVBox.add_child(new_message)
	await get_tree().process_frame # Need to wait until next frame to focus the new node
	%ScrollContainer.call_deferred("ensure_control_visible", new_message)

func _on_name_entry_text_changed(new_text: String):
	self.messaging_name = new_text
	if new_text.is_empty():
		%Host.disabled = true
		%Connect.disabled = true
	else:
		%Host.disabled = false
		%Connect.disabled = false


func on_connected_to_server():
	print("Connected to server")

func on_server_disconnected():
	print("Server disconnected")

func on_peer_connected(id):
	print(id, " connected")
	if multiplayer.is_server():
		print("Spawning cursor")
		self.spawn_cursor(id)
	else:
		print("Skipping cursor spawn because not server")

func on_peer_disconnected(id):
	print(id, " disconnected")

func on_connection_failed():
	print("Connection failed")


func _on_invite_pressed():
	var peer = multiplayer.multiplayer_peer
	if peer is SteamHLPeer:
		SteamHLFriends.activate_game_overlay_invite_dialog(peer.get_lobby_id())
