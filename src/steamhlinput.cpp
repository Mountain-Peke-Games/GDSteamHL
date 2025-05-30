#include "inputeventsteamhlanalogaction.h"
#include "inputeventsteamhldigitalaction.h"
#include "steamhlinput.h"

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <steam_api.h>

using namespace godot;

const char* SteamHLInput::steamControllerConnectedSignal = "steam_controller_connected";
const char* SteamHLInput::steamControllerDisconnectedSignal = "steam_controller_disconnected";

SteamHLInput* SteamHLInput::_singleton = nullptr;

SteamHLInput::SteamHLInput() {
	if (SteamHLInput::_singleton != nullptr) {
		// TODO=Doesn't actually print anything to the console
		UtilityFunctions::print("Warning: Someone is creating SteamHLInput instead of using the singleton");
	}

	this->__controllersCache = memnew_arr(InputHandle_t, STEAM_INPUT_MAX_COUNT);
	SteamHLInput::_singleton = this;
}

SteamHLInput::~SteamHLInput() {
	memdelete_arr(this->__controllersCache); // TODO=Ilive in fear of memory leaks
	SteamHLInput::_singleton = nullptr;
}

void SteamHLInput::_bind_methods() {
	/* Steam API Methods */
	BIND_CONSTANT(STEAM_INPUT_HANDLE_ALL_CONTROLLERS);
	ClassDB::bind_method(D_METHOD("activate_action_set", "input_handle", "action_set_handle"), &SteamHLInput::ActivateActionSet);
	ClassDB::bind_method(D_METHOD("get_action_set_handle", "action_set_name"), &SteamHLInput::GetActionSetHandle);
	ClassDB::bind_method(D_METHOD("get_current_action_set", "input_handle"), &SteamHLInput::GetCurrentActionSet);
	ClassDB::bind_method(D_METHOD("get_analog_action_data", "input_handle", "action_handle"), &SteamHLInput::GetAnalogActionData);
	ClassDB::bind_method(D_METHOD("get_analog_action_handle", "action_name"), &SteamHLInput::GetAnalogActionHandle);
	ClassDB::bind_method(D_METHOD("get_connected_controllers"), &SteamHLInput::GetConnectedControllers);
	ClassDB::bind_method(D_METHOD("get_controller_for_gamepad_index", "index"), &SteamHLInput::GetControllerForGamepadIndex);
	ClassDB::bind_method(D_METHOD("get_gamepad_index_for_controller", "controller_handle"), &SteamHLInput::GetGamepadIndexForController);
	ClassDB::bind_method(D_METHOD("get_input_type_for_handle", "input_handle"), &SteamHLInput::GetInputTypeForHandle);
	ClassDB::bind_method(D_METHOD("get_digital_action_data", "input_handle", "action_handle"), &SteamHLInput::GetDigitalActionData);
	ClassDB::bind_method(D_METHOD("get_digital_action_handle", "action_name"), &SteamHLInput::GetDigitalActionHandle);
	ClassDB::bind_method(D_METHOD("register_analog_action", "action_name"), &SteamHLInput::RegisterAnalogAction);
	ClassDB::bind_method(D_METHOD("register_digital_action", "action_name"), &SteamHLInput::RegisterDigitalAction);
	ClassDB::bind_method(D_METHOD("show_binding_panel", "input_handle"), &SteamHLInput::ShowBindingPanel);
	ClassDB::bind_method(D_METHOD("get_analog_action_origins", "input_handle", "action_set_handle", "analog_action_handle"), &SteamHLInput::GetAnalogActionOrigins);
	ClassDB::bind_method(D_METHOD("get_digital_action_origins", "input_handle", "action_set_handle", "digital_action_handle"), &SteamHLInput::GetDigitalActionOrigins);
	ClassDB::bind_method(D_METHOD("get_glyph_png_for_action_origin", "origin", "glyph_size", "flags"), &SteamHLInput::GetGlyphPNGForActionOrigin);
	ClassDB::bind_method(D_METHOD("trigger_vibration", "input_handle", "left_speed", "right_speed"), &SteamHLInput::TriggerVibration);

	#pragma region ESteamInputType Bindings
	BIND_ENUM_CONSTANT(k_ESteamInputType_Unknown);
	BIND_ENUM_CONSTANT(k_ESteamInputType_SteamController);
	BIND_ENUM_CONSTANT(k_ESteamInputType_XBox360Controller);
	BIND_ENUM_CONSTANT(k_ESteamInputType_XBoxOneController);
	BIND_ENUM_CONSTANT(k_ESteamInputType_GenericGamepad);
	BIND_ENUM_CONSTANT(k_ESteamInputType_PS4Controller);
	BIND_ENUM_CONSTANT(k_ESteamInputType_AppleMFiController);
	BIND_ENUM_CONSTANT(k_ESteamInputType_AndroidController);
	BIND_ENUM_CONSTANT(k_ESteamInputType_SwitchJoyConPair);
	BIND_ENUM_CONSTANT(k_ESteamInputType_SwitchJoyConSingle);
	BIND_ENUM_CONSTANT(k_ESteamInputType_SwitchProController);
	BIND_ENUM_CONSTANT(k_ESteamInputType_MobileTouch);
	BIND_ENUM_CONSTANT(k_ESteamInputType_PS3Controller);
	BIND_ENUM_CONSTANT(k_ESteamInputType_PS5Controller);
	BIND_ENUM_CONSTANT(k_ESteamInputType_SteamDeckController);
	BIND_ENUM_CONSTANT(k_ESteamInputType_Count);
	BIND_ENUM_CONSTANT(k_ESteamInputType_MaximumPossibleValue);
	#pragma endregion ESteamInputType Bindings

	#pragma region EInputActionOrigin Bindings
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_None);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_A);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_B);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_X);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Y);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftGrip);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightGrip);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Start);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Back);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_RightTrigger_Click);
	/*BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_LeftSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Gyro_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Gyro_Pitch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Gyro_Yaw);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Gyro_Roll);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved0);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved1);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved2);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved3);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved4);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved5);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved6);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved7);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved8);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved9);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamController_Reserved10);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_X);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Circle);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Triangle);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Square);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Options);	//Start
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Share);		//Back
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_CenterPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_CenterPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_CenterPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_CenterPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_CenterPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_CenterPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_CenterPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_LeftSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_RightSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_DPad_North);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_DPad_South);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_DPad_West);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_DPad_East);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Gyro_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Gyro_Pitch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Gyro_Yaw);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Gyro_Roll);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_DPad_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved1);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved2);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved3);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved4);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved5);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved6);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved7);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved8);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved9);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS4_Reserved10);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_A);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_B);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_X);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_Y);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_Menu);  //Start
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_View);  //Back
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_DPad_North);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_DPad_South);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_DPad_West);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_DPad_East);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_DPad_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftGrip_Lower);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_LeftGrip_Upper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightGrip_Lower);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_RightGrip_Upper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_Share); // Xbox Series X controllers only
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_Reserved6);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_Reserved7);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_Reserved8);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_Reserved9);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBoxOne_Reserved10);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_A);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_B);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_X);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Y);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_LeftBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_RightBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Start);		//Start
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Back);		//Back
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_LeftTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_LeftTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_RightTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_RightTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_LeftSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_LeftSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_LeftSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_LeftSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_LeftSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_LeftSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_RightSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_RightSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_RightSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_RightSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_RightSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_RightSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_DPad_North);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_DPad_South);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_DPad_West);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_DPad_East);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_DPad_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved1);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved2);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved3);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved4);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved5);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved6);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved7);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved8);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved9);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_XBox360_Reserved10);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_A);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_B);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_X);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Y);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Plus);	//Start
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Minus);	//Back
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Capture);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_DPad_North);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_DPad_South);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_DPad_West);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_DPad_East);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_ProGyro_Move);  // Primary Gyro in Pro Controller); or Right JoyCon
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_ProGyro_Pitch);  // Primary Gyro in Pro Controller); or Right JoyCon
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_ProGyro_Yaw);  // Primary Gyro in Pro Controller); or Right JoyCon
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_ProGyro_Roll);  // Primary Gyro in Pro Controller); or Right JoyCon
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_DPad_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved1);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved2);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved3);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved4);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved5);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved6);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved7);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved8);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved9);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved10);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightGyro_Move);  // Right JoyCon Gyro generally should correspond to Pro's single gyro
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightGyro_Pitch);  // Right JoyCon Gyro generally should correspond to Pro's single gyro
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightGyro_Yaw);  // Right JoyCon Gyro generally should correspond to Pro's single gyro
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightGyro_Roll);  // Right JoyCon Gyro generally should correspond to Pro's single gyro
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftGyro_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftGyro_Pitch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftGyro_Yaw);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftGyro_Roll);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftGrip_Lower); // Left JoyCon SR Button
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_LeftGrip_Upper); // Left JoyCon SL Button
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightGrip_Lower);  // Right JoyCon SL Button
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_RightGrip_Upper);  // Right JoyCon SR Button
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_JoyConButton_N); // With a Horizontal JoyCon this will be Y or what would be Dpad Right when vertical
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_JoyConButton_E); // X
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_JoyConButton_S); // A
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_JoyConButton_W); // B
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved15);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved16);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved17);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved18);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved19);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Switch_Reserved20);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_X);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Circle);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Triangle);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Square);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightBumper);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Option);	//Start
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Create);		//Back
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Mute);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_CenterPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_CenterPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_CenterPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_CenterPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_CenterPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_CenterPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_CenterPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightTrigger_Pull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightTrigger_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightSticBIND_ENUM_CONSTANT(k_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_DPad_North);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_DPad_South);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_DPad_West);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_DPad_East);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Gyro_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Gyro_Pitch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Gyro_Yaw);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Gyro_Roll);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_DPad_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftGrip);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightGrip);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_LeftFn);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_RightFn);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved5);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved6);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved7);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved8);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved9);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved10);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved11);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved12);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved13);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved14);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved15);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved16);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved17);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved18);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved19);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_PS5_Reserved20);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_A);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_B);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_X);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Y);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_L1);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_R1);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Menu);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_View);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightPad_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightPad_Swipe);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightPad_Click);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightPad_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightPad_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightPad_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightPad_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_L2_SoftPull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_L2);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_R2_SoftPull);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_R2);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_L3);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_LeftSticBIND_ENUM_CONSTANT(k_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightSticBIND_ENUM_CONSTANT(k_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_R3);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightSticBIND_ENUM_CONSTANT(k_DPadNorth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightSticBIND_ENUM_CONSTANT(k_DPadSouth);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightSticBIND_ENUM_CONSTANT(k_DPadWest);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightSticBIND_ENUM_CONSTANT(k_DPadEast);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_RightSticBIND_ENUM_CONSTANT(k_Touch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_L4);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_R4);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_L5);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_R5);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_DPad_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_DPad_North);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_DPad_South);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_DPad_West);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_DPad_East);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Gyro_Move);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Gyro_Pitch);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Gyro_Yaw);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Gyro_Roll);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved1);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved2);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved3);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved4);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved5);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved6);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved7);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved8);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved9);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved10);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved11);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved12);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved13);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved14);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved15);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved16);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved17);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved18);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved19);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_SteamDecBIND_ENUM_CONSTANT(k_Reserved20);
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_Count); // If Steam has added support for new controllers origins will go here.
	BIND_ENUM_CONSTANT(k_EInputActionOrigin_MaximumPossibleValue = 32767); // Origins are currently a maximum of 16 bits.*/
#pragma endregion EInputActionOrigin Bindings

	#pragma region ESteamInputGlyphSize Bindings
	BIND_ENUM_CONSTANT(k_ESteamInputGlyphSize_Small);
	BIND_ENUM_CONSTANT(k_ESteamInputGlyphSize_Medium);
	BIND_ENUM_CONSTANT(k_ESteamInputGlyphSize_Large);
	BIND_ENUM_CONSTANT(k_ESteamInputGlyphSize_Count);
	#pragma endregion ESteamInputGlyphSize Bindings

	/* GDSteamHL */
	ADD_SIGNAL(MethodInfo(SteamHLInput::steamControllerConnectedSignal, PropertyInfo(Variant::INT, "controller_id")));
	ADD_SIGNAL(MethodInfo(SteamHLInput::steamControllerDisconnectedSignal, PropertyInfo(Variant::INT, "controller_id")));
}

SteamHLInput* SteamHLInput::get_singleton() {
	if (SteamHLInput::_singleton == nullptr) {
		SteamHLInput::_singleton = memnew(SteamHLInput);
	}
	return SteamHLInput::_singleton;
}

void SteamHLInput::_process() { // TODO = Change to use NOTIFICATION process
	steamHLInput_LOAD_OR_ERROR("Failed to load SteamInput");
	steamInput->RunFrame();
	
	#pragma region Controller Dis/Connects
	// Find any new controllers and erase current so only disconnected controllers are left.
	int totalConnectedControllers = steamInput->GetConnectedControllers(this->__controllersCache);
	for (int i = 0; i < totalConnectedControllers; i++) {
		InputHandle_t controller = *(this->__controllersCache + i);
		if (!this->_connectedControllers.has(controller)) {
			this->emit_signal(SteamHLInput::steamControllerConnectedSignal, static_cast<uint64_t>(controller)); // TODO = Should this be a StringName?
		}
		else {
			this->_connectedControllers.erase(controller);
		}
	}

	// Find leftover controllers, which will be the disconnected ones.
	for (HashSet<InputHandle_t>::Iterator controllerIter = this->_connectedControllers.begin(); controllerIter != this->_connectedControllers.end(); ++controllerIter) {
		this->_connectedControllers.erase(*controllerIter);
		this->emit_signal(SteamHLInput::steamControllerDisconnectedSignal, static_cast<uint64_t>(*controllerIter));
		
		// Remove controllers from input tracking maps
		for (Vector<InputAnalogActionHandle_t>::Iterator analogIter = this->_registeredAnalogActions.begin(); analogIter != this->_registeredAnalogActions.end(); ++analogIter) {
			this->_controllerActionAnalogMap.erase(hashPair(*controllerIter, *analogIter));
		}
		for (Vector<InputAnalogActionHandle_t>::Iterator digitalIter = this->_registeredDigitalActions.begin(); digitalIter != this->_registeredDigitalActions.end(); ++digitalIter) {
			this->_controllerActionDigitalMap.erase(hashPair(*controllerIter, *digitalIter));
		}
	}

	// Finally add all the current controllers (readd old and add new)
	for (int i = 0; i < totalConnectedControllers; i++) {
		InputHandle_t controller = *(this->__controllersCache + i);
		this->_connectedControllers.insert(controller);
	}
	#pragma endregion

	#pragma region InputEvents
	for (int i = 0; i < totalConnectedControllers; i++) {
		InputHandle_t controller = *(this->__controllersCache + i);
		for (Vector<InputAnalogActionHandle_t>::Iterator analogIter = this->_registeredAnalogActions.begin(); analogIter != this->_registeredAnalogActions.end(); ++analogIter) {
			uint32_t hash = hashPair(controller, *analogIter);
			InputAnalogActionData_t currentData = steamInput->GetAnalogActionData(controller, *analogIter);
			if (this->_controllerActionAnalogMap.has(hash) && (this->_controllerActionAnalogMap[hash].x != currentData.x || this->_controllerActionAnalogMap[hash].y != currentData.y)) {
				Ref<InputEventSteamHLAnalogAction> ref;
				ref.instantiate();
				ref->controllerHandle = static_cast<uint64_t>(controller);
				ref->actionHandle = static_cast<uint64_t>(*analogIter);
				if (this->_analogHandleToNameMap.has(*analogIter)) {
					ref->set_action(this->_analogHandleToNameMap[*analogIter]);
				}
				else {
					ref->set_action("Unregistered Steam Action");
				}
				ref->x = currentData.x;
				ref->y = currentData.y;
				ref->set_pressed(currentData.x != 0.0 || currentData.y != 0.0);
				ref->set_strength(Math::sqrt(currentData.x*currentData.x + currentData.y*currentData.y));

				Input::get_singleton()->parse_input_event(ref);
			}
			// We don't emit a signal the first time the input is added (because it was just registered and we cannot guarantee a change)
			this->_controllerActionAnalogMap[hash] = currentData;
		}

		for (Vector<InputAnalogActionHandle_t>::Iterator digitalIter = this->_registeredDigitalActions.begin(); digitalIter != this->_registeredDigitalActions.end(); ++digitalIter) {
			uint32_t hash = hashPair(controller, *digitalIter);
			InputDigitalActionData_t currentData = steamInput->GetDigitalActionData(controller, *digitalIter);
			if (this->_controllerActionDigitalMap.has(hash) && this->_controllerActionDigitalMap[hash].bState != currentData.bState) {
				Ref<InputEventSteamHLDigitalAction> ref;
				ref.instantiate();	
				ref->controllerHandle = static_cast<uint64_t>(controller);
				ref->actionHandle = static_cast<uint64_t>(*digitalIter);
				if (this->_digitalHandleToNameMap.has(*digitalIter)) {
					ref->set_action(this->_digitalHandleToNameMap[*digitalIter]);
				}
				else {
					ref->set_action("Unregistered Steam Action");
				}
				ref->set_pressed(currentData.bState);
				ref->set_strength(currentData.bState ? 1.0 : 0.0);

				Input::get_singleton()->parse_input_event(ref);
			}
			// We don't emit a signal the first time the input is added (because it was just registered and we cannot guarantee a change)
			this->_controllerActionDigitalMap[hash] = currentData;
		}
	}
	#pragma endregion
}

uint64_t SteamHLInput::RegisterAnalogAction(const String& analogAction) {
	steamHLInput_LOAD_OR_RETURN(0);
	InputAnalogActionHandle_t handle = steamInput->GetAnalogActionHandle(analogAction.utf8().get_data());
	if (static_cast<uint64_t>(handle) != 0 && !this->_registeredAnalogActions.has(handle)) {
		this->_registeredAnalogActions.push_back(handle);
		this->_analogHandleToNameMap[handle] = StringName(analogAction);
	}
	return static_cast<uint64_t>(handle);
}

uint64_t SteamHLInput::RegisterDigitalAction(const String& digitalAction) {
	steamHLInput_LOAD_OR_RETURN(0);
	InputDigitalActionHandle_t handle = steamInput->GetDigitalActionHandle(digitalAction.utf8().get_data());
	if (static_cast<uint64_t>(handle) != 0 && !this->_registeredDigitalActions.has(handle)) {
		this->_registeredDigitalActions.push_back(handle);
		this->_digitalHandleToNameMap[handle] = StringName(digitalAction);
	}
	return static_cast<uint64_t>(handle);
}

void SteamHLInput::ActivateActionSet(uint64_t inputHandle, uint64_t actionSetHandle) {
	steamHLInput_LOAD_OR_ERROR("Failed to load SteamInput");
	steamInput->ActivateActionSet(static_cast<InputHandle_t>(inputHandle), static_cast<InputActionSetHandle_t>(actionSetHandle));
}

uint64_t SteamHLInput::GetActionSetHandle(const String &actionSetName) {
	steamHLInput_LOAD_OR_RETURN(0);
	return static_cast<uint64_t>(steamInput->GetActionSetHandle(actionSetName.utf8().get_data()));
}

uint64_t SteamHLInput::GetCurrentActionSet(uint64_t inputHandle) {
	steamHLInput_LOAD_OR_RETURN(0);
	return static_cast<uint64_t>(steamInput->GetCurrentActionSet(static_cast<InputHandle_t>(inputHandle)));
}

// TODO=mouse_mode is borked, likely because it returns the delta since RunFrame was last called, which causes issues when we're polling for a registered action
// Except that is not it because it is completely borked if GetAnalogActionData is called twice because
// apparently that destroys the mouse data?!?!?!
SteamHLAnalogActionData* SteamHLInput::GetAnalogActionData(uint64_t inputHandle, uint64_t analogActionHandle) {
	steamHLInput_LOAD_OR_RETURN(memnew(SteamHLAnalogActionData)); // TODO=I live in fear of memory leaks...
	InputAnalogActionData_t inputAnalogActionData = steamInput->GetAnalogActionData(static_cast<InputHandle_t>(inputHandle), static_cast<InputAnalogActionHandle_t>(analogActionHandle));
	SteamHLAnalogActionData* steamHLAnalogActionData = memnew(SteamHLAnalogActionData(inputAnalogActionData)); // TODO=I live in fear of memory leaks
	return steamHLAnalogActionData;
}

uint64_t SteamHLInput::GetAnalogActionHandle(const String& actionName) {
	steamHLInput_LOAD_OR_RETURN(0);
	return static_cast<uint64_t>(steamInput->GetAnalogActionHandle(actionName.utf8().get_data()));
}

TypedArray<uint64_t> SteamHLInput::GetConnectedControllers() {
	TypedArray<uint64_t> ret; // I live in fear of memory leaks
	steamHLInput_LOAD_OR_RETURN(ret);
	int totalControllers = steamInput->GetConnectedControllers(this->__controllersCache);
	ret.resize(totalControllers);
	for (int i = 0; i < totalControllers; i++) {
		ret[i] = static_cast<uint64_t>(*(this->__controllersCache + i));
	}
	
	return ret;
}

uint64_t SteamHLInput::GetControllerForGamepadIndex(int nIndex) {
	steamHLInput_LOAD_OR_RETURN(0);
	return static_cast<uint64_t>(steamInput->GetControllerForGamepadIndex(nIndex));
}

int SteamHLInput::GetGamepadIndexForController(uint64_t ulControllerHandle) {
	steamHLInput_LOAD_OR_RETURN(-1);
	return steamInput->GetGamepadIndexForController(static_cast<InputHandle_t>(ulControllerHandle));
}

ESteamInputType SteamHLInput::GetInputTypeForHandle(uint64_t inputHandle) {
	steamHLInput_LOAD_OR_RETURN(k_ESteamInputType_Unknown);
	return steamInput->GetInputTypeForHandle(static_cast<InputHandle_t>(inputHandle));
}

SteamHLDigitalActionData* SteamHLInput::GetDigitalActionData(uint64_t inputHandle, uint64_t digitalActionHandle) {
	steamHLInput_LOAD_OR_RETURN(memnew(SteamHLDigitalActionData)); // TODO=I live in fear of memory leaks...
	InputDigitalActionData_t inputDigitalActionData = steamInput->GetDigitalActionData(static_cast<uint64_t>(inputHandle), static_cast<uint64_t>(digitalActionHandle));
	SteamHLDigitalActionData* steamHLDigitalActionData = memnew(SteamHLDigitalActionData(inputDigitalActionData));
	return steamHLDigitalActionData;
}

uint64_t SteamHLInput::GetDigitalActionHandle(const String& actionName) {
	steamHLInput_LOAD_OR_RETURN(0);
	return static_cast<uint64_t>(steamInput->GetDigitalActionHandle(actionName.utf8().get_data()));
}

bool SteamHLInput::ShowBindingPanel(uint64_t inputHandle) {
	steamHLInput_LOAD_OR_RETURN(false);
	return steamInput->ShowBindingPanel(static_cast<InputHandle_t>(inputHandle));
}

TypedArray<int> SteamHLInput::GetAnalogActionOrigins(uint64_t inputHandle, uint64_t actionSetHandle, uint64_t analogActionHandle) {
	TypedArray<int> ret; // I live in fear of memory leaks
	steamHLInput_LOAD_OR_RETURN(ret);
	EInputActionOrigin* originsBuffer = memnew_arr(EInputActionOrigin, STEAM_INPUT_MAX_ORIGINS);
	int totalOrigins = steamInput->GetAnalogActionOrigins(inputHandle, actionSetHandle, analogActionHandle, originsBuffer);
	ret.resize(totalOrigins);
	for (int i = 0; i < totalOrigins; i++) {
		ret[i] = *(originsBuffer + i);
	}
	memdelete_arr(originsBuffer);

	return ret;
}

TypedArray<int> SteamHLInput::GetDigitalActionOrigins(uint64_t inputHandle, uint64_t actionSetHandle, uint64_t digitalActionHandle) {
	TypedArray<int> ret; // I live in fear of memory leaks
	steamHLInput_LOAD_OR_RETURN(ret);
	EInputActionOrigin* originsBuffer = memnew_arr(EInputActionOrigin, STEAM_INPUT_MAX_ORIGINS);
	int totalOrigins = steamInput->GetDigitalActionOrigins(inputHandle, actionSetHandle, digitalActionHandle, originsBuffer);
	ret.resize(totalOrigins);
	for (int i = 0; i < totalOrigins; i++) {
		ret[i] = *(originsBuffer + i);
	}
	memdelete_arr(originsBuffer);

	return ret;
}

String SteamHLInput::GetGlyphPNGForActionOrigin(EInputActionOrigin origin, ESteamInputGlyphSize glyphSize, int flags) {
	steamHLInput_LOAD_OR_RETURN("");
	return steamInput->GetGlyphPNGForActionOrigin(origin, glyphSize, flags);
}

void SteamHLInput::TriggerVibration(uint64_t inputHandle, uint16_t leftSpeed, uint16_t rightSpeed) {
	steamHLInput_LOAD_OR_ERROR("Failed to load SteamInput");
	steamInput->TriggerVibration(static_cast<InputHandle_t>(inputHandle), static_cast<unsigned short>(leftSpeed), static_cast<unsigned short>(rightSpeed));
}