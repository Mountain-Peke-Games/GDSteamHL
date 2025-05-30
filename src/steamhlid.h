#ifndef STEAMHLID_H
#define STEAMHLID_H

#include <steam_api.h>
#include <godot_cpp/classes/ref_counted.hpp>

VARIANT_ENUM_CAST(EAccountType);
VARIANT_ENUM_CAST(EUniverse);
VARIANT_ENUM_CAST(EChatSteamIDInstanceFlags);

namespace godot {
	struct SteamHLID : public RefCounted, CSteamID {
		GDCLASS(SteamHLID, RefCounted);

	public:
		SteamHLID();
		SteamHLID(CSteamID cSteamID);
		~SteamHLID();

		CSteamID steamId;

		void godot_Set(uint32_t accountID, EUniverse universe, EAccountType accountType);
		void godot_InstancedSet(uint32_t accountID, uint32_t instance, EUniverse universe, EAccountType accountType);
		void godot_SetFromUint64(uint64_t steamID);

		uint32_t godot_GetAccountID() const;
		EAccountType godot_GetEAccountType() const;
		EUniverse godot_GetEUniverse() const;
		uint64_t godot_ConvertToUInt64() const;
		uint64_t godot_GetStaticAccountKey() const;
		bool godot_BBlankAnonAccount() const;
		bool godot_BGameServerAccount() const;
		bool godot_BPersistentGameServerAccount() const;
		bool godot_BAnonGameServerAccount() const;
		bool godot_BContentServerAccount() const;
		bool godot_BClanAccount() const;
		bool godot_BChatAccount() const;
		bool godot_IsLobby() const;
		bool godot_BIndividualAccount() const;
		bool godot_BAnonAccount() const;
		bool godot_BAnonUserAccount() const;
		bool godot_BConsoleUserAccount() const;
		bool godot_IsValid() const;

		// TODO=Overload equality
	protected:
		static void _bind_methods();
	};
}

#endif // !STEAMHLID_H