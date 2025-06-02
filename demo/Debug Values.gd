extends VBoxContainer

func load_values():
	$Init.text = "API Init: "+str(SteamHL.is_initialized())
	$AppId.text = "App ID: "+str(SteamHLUtils.get_app_id())
	$Running.text = "Steam Running: "+str(SteamHL.is_steam_running())
	$Deck.text = "On Steamdeck: "+str(SteamHLUtils.is_steam_running_on_steam_deck())
	$Lobby.text = "Lobby ID: ??"

func on_lobby_id_changed(new_id: SteamHLID):
	var id_as_num = new_id.convert_to_uint64()
	$Lobby.text = "Lobby ID: "+str(id_as_num)
