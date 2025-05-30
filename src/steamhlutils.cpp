#include "steamhlutils.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <steam_api.h>

using namespace godot;

const char* SteamHLUtils::floatingGamepadTextInputDismissedSignal = "floating_gamepad_text_input_dismissed";
const char* SteamHLUtils::gamepadTextInputDismissedSignal = "gamepad_text_input_dismissed";

SteamHLUtils* SteamHLUtils::_singleton = nullptr;

SteamHLUtils::SteamHLUtils() {
	if (SteamHLUtils::_singleton != nullptr) {
		// TODO=Doesn't actually print anything to the console
		UtilityFunctions::print("Warning: Someone is creating SteamHL instead of using the singleton");
	}

	SteamHLUtils::_singleton = this;
}

SteamHLUtils::~SteamHLUtils() {
	SteamHLUtils::_singleton = nullptr;
}

void SteamHLUtils::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_app_id"), &SteamHLUtils::GetAppId);
	ClassDB::bind_method(D_METHOD("is_overlay_enabled"), &SteamHLUtils::IsOverlayEnabled);
	ClassDB::bind_method(D_METHOD("is_steam_in_big_picture_mode"), &SteamHLUtils::IsSteamInBigPictureMode);
	ClassDB::bind_method(D_METHOD("is_steam_running_in_vr"), &SteamHLUtils::IsSteamRunningInVR);
	ClassDB::bind_method(D_METHOD("is_steam_running_on_steam_deck"), &SteamHLUtils::IsSteamRunningOnSteamDeck);
	ClassDB::bind_method(D_METHOD("show_floating_gamepad_text_input", "keyboard_mode", "field_rect"), &SteamHLUtils::ShowFloatingGamepadTextInput);
	ClassDB::bind_method(D_METHOD("show_gamepad_text_input", "input_mode", "line_input_mode", "description", "char_max", "existing_text"), &SteamHLUtils::ShowGamepadTextInput);
	
	ADD_SIGNAL(MethodInfo(SteamHLUtils::floatingGamepadTextInputDismissedSignal));
	ADD_SIGNAL(MethodInfo(SteamHLUtils::gamepadTextInputDismissedSignal, PropertyInfo(Variant::BOOL, "submitted"), PropertyInfo(Variant::STRING, "submitted_text")));

	BIND_ENUM_CONSTANT(k_EFloatingGamepadTextInputModeModeSingleLine);
	BIND_ENUM_CONSTANT(k_EFloatingGamepadTextInputModeModeMultipleLines);
	BIND_ENUM_CONSTANT(k_EFloatingGamepadTextInputModeModeEmail);
	BIND_ENUM_CONSTANT(k_EFloatingGamepadTextInputModeModeNumeric);

	BIND_ENUM_CONSTANT(k_EGamepadTextInputModeNormal);
	BIND_ENUM_CONSTANT(k_EGamepadTextInputModePassword);

	BIND_ENUM_CONSTANT(k_EGamepadTextInputLineModeSingleLine);
	BIND_ENUM_CONSTANT(k_EGamepadTextInputLineModeMultipleLines);
}

SteamHLUtils* SteamHLUtils::get_singleton() {
	if (SteamHLUtils::_singleton == nullptr) {
		SteamHLUtils::_singleton = memnew(SteamHLUtils);
	}
	return SteamHLUtils::_singleton;
}

uint32_t SteamHLUtils::GetAppId() {
	steamUtils_LOAD_OR_RETURN(0);
	return static_cast<uint32_t>(steamUtils->GetAppID());
}

bool SteamHLUtils::IsOverlayEnabled() {
	steamUtils_LOAD_OR_RETURN(false);
	return steamUtils->IsOverlayEnabled();
}

bool SteamHLUtils::IsSteamInBigPictureMode() {
	steamUtils_LOAD_OR_RETURN(false);
	return steamUtils->IsSteamInBigPictureMode();
}

bool SteamHLUtils::IsSteamRunningInVR() {
	steamUtils_LOAD_OR_RETURN(false);
	return steamUtils->IsSteamRunningInVR();
}

bool SteamHLUtils::IsSteamRunningOnSteamDeck() {
	steamUtils_LOAD_OR_RETURN(false);
	return steamUtils->IsSteamRunningOnSteamDeck();
}

bool SteamHLUtils::ShowFloatingGamepadTextInput(EFloatingGamepadTextInputMode keyboardMode, const Rect2& fieldRect) {
	steamUtils_LOAD_OR_RETURN(false);
	return steamUtils->ShowFloatingGamepadTextInput(keyboardMode, fieldRect.position.x, fieldRect.position.y, fieldRect.size.x, fieldRect.size.y);
}

bool SteamHLUtils::ShowGamepadTextInput(EGamepadTextInputMode inputMode, EGamepadTextInputLineMode lineInputMode, const String& description, uint32_t charMax, const String& existingText) {
	steamUtils_LOAD_OR_RETURN(false);
	return steamUtils->ShowGamepadTextInput(inputMode, lineInputMode, description.utf8().get_data(), charMax, existingText.utf8().get_data());
}

void SteamHLUtils::onFloatingGamepadTextInputDismissed(FloatingGamepadTextInputDismissed_t* callback) {
	this->emit_signal(SteamHLUtils::floatingGamepadTextInputDismissedSignal);
}

void SteamHLUtils::onGamepadTextInputDismissed(GamepadTextInputDismissed_t* callback) {
	steamUtils_LOAD_OR_ERROR("Failed to load SteamUtils");
	if (callback->m_bSubmitted) {
		const uint32_t length = steamUtils->GetEnteredGamepadTextLength();
		char* buffer = memnew_arr(char, length+1); // +1 because it is unclear if length will include the terminating character
		steamUtils->GetEnteredGamepadTextInput(buffer, length);
		this->emit_signal(SteamHLUtils::gamepadTextInputDismissedSignal, callback->m_bSubmitted, String(buffer));
		memdelete_arr(buffer);
	}
	else {
		this->emit_signal(SteamHLUtils::gamepadTextInputDismissedSignal, callback->m_bSubmitted, String(""));
	}
}

bool SteamHLUtils::GetImageSize(int64_t imageID, uint32* width, uint32* height) {
	steamUtils_LOAD_OR_RETURN(false);
	return steamUtils->GetImageSize(imageID, width, height);
}


bool SteamHLUtils::GetImageRGBA(int64_t imageID, uint8* pubDest, int destBufferSize) {
	steamUtils_LOAD_OR_RETURN(false);
	return steamUtils->GetImageRGBA(imageID, pubDest, destBufferSize);
}