#pragma once

#include <src/steamhl.h>
#include <src/steam_wrapper/data_structures/steamhlanalogactiondata.h>
#include <src/steam_wrapper/data_structures/steamhldigitalactiondata.h>

#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <steam_api.h>

#define steamHLInput_LOAD_OR_ERROR(errMsg) STEAMHL_LOAD_OR_ERROR(steamInput, SteamInput, errMsg);
#define steamHLInput_LOAD_OR_RETURN(failureValue) STEAMHL_LOAD_OR_RETURN(steamInput, SteamInput, failureValue);

VARIANT_ENUM_CAST(ESteamInputType);
VARIANT_ENUM_CAST(EInputActionOrigin);
VARIANT_ENUM_CAST(ESteamInputGlyphSize);

namespace godot {
	class SteamHLInput : public Object {
		GDCLASS(SteamHLInput, Object);

	public:
		SteamHLInput();
		~SteamHLInput();

		static SteamHLInput* get_singleton();

		// SteamInput::Init() and ::Shutdown() is handled by SteamHL when it initializes and destroys itself

		/* Steam API Methods */
		void ActivateActionSet(uint64_t inputHandle, uint64_t actionSetHandle);
		uint64_t GetActionSetHandle(const String &actionSetName);
		uint64_t GetCurrentActionSet(uint64_t inputHandle);

		SteamHLAnalogActionData* GetAnalogActionData(uint64_t inputHandle, uint64_t analogActionHandle);
		uint64_t GetAnalogActionHandle(const String &actionName);
		
		TypedArray<uint64_t> GetConnectedControllers();
		uint64_t GetControllerForGamepadIndex(int nIndex);
		int GetGamepadIndexForController(uint64_t ulControllerHandle);
		ESteamInputType GetInputTypeForHandle(uint64_t inputHandle);

		SteamHLDigitalActionData* GetDigitalActionData(uint64_t inputHandle, uint64_t digitalActionHandle);
		uint64_t GetDigitalActionHandle(const String &actionName);

		bool ShowBindingPanel(uint64_t inputHandle);

		// TODO = Investigate returning an TypedArray of enum types (currently won't compile because enums don't have get_class_static)
		// I can't even use get_underlying_type for this
		TypedArray<int> GetAnalogActionOrigins(uint64_t inputHandle, uint64_t actionSetHandle, uint64_t analogActionHandle);
		TypedArray<int> GetDigitalActionOrigins(uint64_t inputHandle, uint64_t actionSetHandle, uint64_t digitalActionHandle);
		String GetGlyphPNGForActionOrigin(EInputActionOrigin origin, ESteamInputGlyphSize glyphSize, int flags);

		void TriggerVibration(uint64_t inputHandle, uint16_t leftSpeed, uint16_t rightSpeed);

		/* GDSteamHL Methods */
		void _process(); // Actually called by SteamHL once it is initialized

		uint64_t RegisterAnalogAction(const String &analogAction);
		uint64_t RegisterDigitalAction(const String &digitalAction);
	
	protected:
		const static char* SteamHLInput::steamControllerConnectedSignal;
		const static char* SteamHLInput::steamControllerDisconnectedSignal;

		static void _bind_methods();

	private:
		static SteamHLInput* _singleton;

		// Re-implement Godot's pair hashing since it is not exposed to GDExtension
		template<class F, class S>
		_FORCE_INLINE_ static uint32_t hashPair(const F &f, const S &s) {
			uint64_t h1 = HashMapHasherDefault::hash(f);
			uint64_t h2 = HashMapHasherDefault::hash(s);
			return hash_one_uint64((h1 << 32) | h2);
		}

		InputHandle_t* __controllersCache;
		HashSet<InputHandle_t> _connectedControllers;

		Vector<InputAnalogActionHandle_t> _registeredAnalogActions;
		Vector<InputDigitalActionHandle_t> _registeredDigitalActions;
		HashMap<InputAnalogActionHandle_t, StringName> _analogHandleToNameMap;
		HashMap<InputDigitalActionHandle_t, StringName> _digitalHandleToNameMap;

		HashMap<uint32_t, InputAnalogActionData_t> _controllerActionAnalogMap;
		HashMap<uint32_t, InputDigitalActionData_t> _controllerActionDigitalMap;
	};
}