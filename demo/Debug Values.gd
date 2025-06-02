extends Control

func load_values():
	$APIStatus.text = "Initialized" if SteamHL.is_initialized() else "API Down"
	$AppId.text = str(SteamHLUtils.get_app_id())
	$Running.text = "Running" if SteamHL.is_steam_running() else "Offline"
	$LobbyId.text = "(Not in a lobby)"

func on_lobby_id_changed(new_id: SteamHLID):
	var id_as_num = new_id.convert_to_uint64()
	$LobbyId.text = "Lobby ID: "+str(id_as_num)
