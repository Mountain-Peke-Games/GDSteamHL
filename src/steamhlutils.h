#ifndef STEAMHLUTILS_H
#define STEAMHLUTILS_H
#include "steamhl.h"

#include <godot_cpp/core/object.hpp>

#define steamUtils_LOAD_OR_ERROR(errMsg) STEAMHL_LOAD_OR_ERROR(steamUtils, SteamUtils, errMsg);
#define steamUtils_LOAD_OR_RETURN(failureValue) STEAMHL_LOAD_OR_RETURN(steamUtils, SteamUtils, failureValue);

VARIANT_ENUM_CAST(EFloatingGamepadTextInputMode);
VARIANT_ENUM_CAST(EGamepadTextInputMode);
VARIANT_ENUM_CAST(EGamepadTextInputLineMode);

namespace godot {
	class SteamHLUtils : public Object {
		GDCLASS(SteamHLUtils, Object);

	public:
		SteamHLUtils();
		~SteamHLUtils();
		static SteamHLUtils* get_singleton();

		uint32_t GetAppId();
		bool IsOverlayEnabled();
		bool IsSteamInBigPictureMode();
		bool IsSteamRunningInVR();
		bool IsSteamRunningOnSteamDeck();
		bool ShowFloatingGamepadTextInput(EFloatingGamepadTextInputMode keyboardMode, const Rect2& fieldRect);
		bool ShowGamepadTextInput(EGamepadTextInputMode inputMode, EGamepadTextInputLineMode lineInputMode, const String& description, uint32_t charMax, const String& existingText);
		// GetEnteredGamepadText* must be called within the callback, so we are not exposing it to Godot
		bool GetImageSize(int64_t imageID, uint32* width, uint32* height); // Not exposed to Godot
		bool GetImageRGBA(int64_t imageID, uint8* pubDest, int destBufferSize); // Not exposed to Godot
		
		// TODO = Don't do callback until Steam API is initialized (probably slam them in a separate object that isn't created until Steam is initialized)
		/* Steam Callbacks */
		STEAM_CALLBACK(SteamHLUtils, onFloatingGamepadTextInputDismissed, FloatingGamepadTextInputDismissed_t);
		STEAM_CALLBACK(SteamHLUtils, onGamepadTextInputDismissed, GamepadTextInputDismissed_t);

	protected:
		const static char* floatingGamepadTextInputDismissedSignal;
		const static char* gamepadTextInputDismissedSignal;

		static SteamHLUtils* _singleton;
		static void _bind_methods();
	};
}

#endif // !STEAMHLUTILS_H
