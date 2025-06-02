class_name Message
extends Control

func set_message_name(new_name: String):
	%Name.text = new_name+":"

func set_message_value(new_value: String):
	%Value.text = new_value
