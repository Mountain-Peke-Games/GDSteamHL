extends Control

func _process(delta):
	if self.is_multiplayer_authority():
		self.global_position = get_viewport().get_mouse_position()

@rpc("any_peer", "call_local", "reliable")
func change_authority(new_authority):
	print("Authority ", self.get_multiplayer_authority(), "->", new_authority)
	self.set_multiplayer_authority(new_authority)
	#$MultiplayerSynchronizer.set_multiplayer_authority(authority)
