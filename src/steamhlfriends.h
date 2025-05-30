#ifndef STEAMHLFRIENDS_H
#define STEAMHLFRIENDS_H
#include "steamhl.h"
#include "steamhlid.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/image_texture.hpp>

#include <steam_api.h>

#define steamFriends_LOAD_OR_ERROR(errMsg) STEAMHL_LOAD_OR_ERROR(steamFriends, SteamFriends, errMsg);
#define steamFriends_LOAD_OR_RETURN(failureValue) STEAMHL_LOAD_OR_RETURN(steamFriends, SteamFriends, failureValue);

enum EActivateOverlayOption {
	FRIENDS,
	COMMUNITY,
	PLAYERS,
	SETTINGS,
	OFFICIAL_GAME_GROUP,
	STATS,
	ACHIEVEMENTS
};
VARIANT_ENUM_CAST(EActivateOverlayOption);

VARIANT_ENUM_CAST(EOverlayToStoreFlag);
VARIANT_ENUM_CAST(EActivateGameOverlayToWebPageMode);
VARIANT_ENUM_CAST(EPersonaState);

namespace godot {
	class SteamHLFriends : public Object {
		GDCLASS(SteamHLFriends, Object);

	public:
		static void activateGameOverlay(EActivateOverlayOption overlayOption);
		static void activateGameOverlayInviteDialog(SteamHLID* steamIDLobby);
		static void activateGameOverlayToStore(uint32_t appId, EOverlayToStoreFlag flag);
		static void activateGameOverlayToWebPage(const String& url, EActivateGameOverlayToWebPageMode mode = k_EActivateGameOverlayToWebPageMode_Default);
		static int64_t getMediumFriendAvatar(CSteamID steamIDFriend);
		static int64_t getSmallFriendAvatar(CSteamID steamIDFriend);
		static EPersonaState GetPersonaState();

		static Ref<Texture> godot_getMediumFriendAvatar(SteamHLID* steamHLIDFriend, Ref<Texture> fallback);
		static Ref<Texture> godot_getSmallFriendAvatar(SteamHLID* steamHLIDFriend, Ref<Texture> fallback);
		static Ref<Texture> _godot_getFriendAvatarShared(uint64_t imageID, Ref<Texture> fallback);
		

	protected:
		const static char* name;

		static SteamHLFriends* _singleton;
		static void _bind_methods();
	};
}

#endif // !STEAMHLFRIENDS_H
