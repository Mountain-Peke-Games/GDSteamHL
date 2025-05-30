#include "steamhlfriends.h"
#include "steamhlutils.h"

using namespace godot;

const char* SteamHLFriends::name = "SteamHLFriends";


void SteamHLFriends::_bind_methods() {
	ClassDB::bind_static_method(SteamHLFriends::name, D_METHOD("activate_game_overlay", "overlay_option"), &SteamHLFriends::activateGameOverlay);
	ClassDB::bind_static_method(SteamHLFriends::name, D_METHOD("activate_game_overlay_invite_dialog", "steam_id_lobby"), &SteamHLFriends::activateGameOverlayInviteDialog);
	ClassDB::bind_static_method(SteamHLFriends::name, D_METHOD("activate_game_overlay_to_store", "appId", "flag"), &SteamHLFriends::activateGameOverlayToStore);
	ClassDB::bind_static_method(SteamHLFriends::name, D_METHOD("activate_game_overlay_to_web_page", "url", "mode"), &SteamHLFriends::activateGameOverlayToWebPage, DEFVAL(k_EActivateGameOverlayToWebPageMode_Default));
	ClassDB::bind_static_method(SteamHLFriends::name, D_METHOD("get_medium_friend_avatar", "steamHLID", "fallback_image"), SteamHLFriends::godot_getMediumFriendAvatar);
	ClassDB::bind_static_method(SteamHLFriends::name, D_METHOD("get_small_friend_avatar", "steamHLID", "fallback_image"), SteamHLFriends::godot_getSmallFriendAvatar);
	ClassDB::bind_static_method(SteamHLFriends::name, D_METHOD("get_persona_state"), SteamHLFriends::GetPersonaState);

	BIND_ENUM_CONSTANT(FRIENDS);
	BIND_ENUM_CONSTANT(COMMUNITY);
	BIND_ENUM_CONSTANT(PLAYERS);
	BIND_ENUM_CONSTANT(SETTINGS);
	BIND_ENUM_CONSTANT(OFFICIAL_GAME_GROUP);
	BIND_ENUM_CONSTANT(STATS);
	BIND_ENUM_CONSTANT(ACHIEVEMENTS);

	BIND_ENUM_CONSTANT(k_EOverlayToStoreFlag_None);
	BIND_ENUM_CONSTANT(k_EOverlayToStoreFlag_AddToCart);
	BIND_ENUM_CONSTANT(k_EOverlayToStoreFlag_AddToCartAndShow);

	BIND_ENUM_CONSTANT(k_EActivateGameOverlayToWebPageMode_Default);
	BIND_ENUM_CONSTANT(k_EActivateGameOverlayToWebPageMode_Modal);

	BIND_ENUM_CONSTANT(k_EPersonaStateOffline);
	BIND_ENUM_CONSTANT(k_EPersonaStateOnline);
	BIND_ENUM_CONSTANT(k_EPersonaStateBusy);
	BIND_ENUM_CONSTANT(k_EPersonaStateAway);
	BIND_ENUM_CONSTANT(k_EPersonaStateSnooze);
	BIND_ENUM_CONSTANT(k_EPersonaStateLookingToTrade);
	BIND_ENUM_CONSTANT(k_EPersonaStateLookingToPlay);
	BIND_ENUM_CONSTANT(k_EPersonaStateInvisible);
	BIND_ENUM_CONSTANT(k_EPersonaStateMax);
}

void SteamHLFriends::activateGameOverlay(EActivateOverlayOption overlayOption) {
	char* pchDialog = "";
	switch (overlayOption) {
		case FRIENDS:
			pchDialog = "friends";
			break;
		case COMMUNITY:
			pchDialog = "community";
			break;
		case PLAYERS:
			pchDialog = "players";
			break;
		case SETTINGS:
			pchDialog = "settings";
			break;
		case OFFICIAL_GAME_GROUP:
			pchDialog = "officialgamegroup";
			break;
		case STATS:
			pchDialog = "stats";
			break;
		case ACHIEVEMENTS:
			pchDialog = "achievements";
			break;
		default:
			ERR_FAIL_COND_MSG(true, "Invalid EActivateOverlayOption value passed to SteamHLFriends.activate_game_overlay");
	}
	
	steamFriends_LOAD_OR_ERROR("Failed to load SteamFriends");
	steamFriends->ActivateGameOverlay(pchDialog);
}

void SteamHLFriends::activateGameOverlayInviteDialog(SteamHLID* steamIDLobby) {
	steamFriends_LOAD_OR_ERROR("Failed to load SteamFriends");
	steamFriends->ActivateGameOverlayInviteDialog(steamIDLobby->steamId);
}

void SteamHLFriends::activateGameOverlayToStore(uint32_t appId, EOverlayToStoreFlag flag) {
	steamFriends_LOAD_OR_ERROR("Failed to load SteamFriends");
	steamFriends->ActivateGameOverlayToStore(static_cast<AppId_t>(appId), flag);
}


void SteamHLFriends::activateGameOverlayToWebPage(const String& url, EActivateGameOverlayToWebPageMode mode) {
	steamFriends_LOAD_OR_ERROR("Failed to load SteamFriends");
	steamFriends->ActivateGameOverlayToWebPage(url.utf8().get_data(), mode);
}

int64_t SteamHLFriends::getMediumFriendAvatar(CSteamID steamIDFriend) {
	steamFriends_LOAD_OR_RETURN(0);
	return steamFriends->GetMediumFriendAvatar(steamIDFriend);
}

int64_t SteamHLFriends::getSmallFriendAvatar(CSteamID steamIDFriend) {
	steamFriends_LOAD_OR_RETURN(0);
	return steamFriends->GetSmallFriendAvatar(steamIDFriend);
}

Ref<Texture> SteamHLFriends::godot_getMediumFriendAvatar(SteamHLID* steamHLIDFriend, Ref<Texture> fallback) {
	int64_t imageID = SteamHLFriends::getMediumFriendAvatar(steamHLIDFriend->steamId);
	return SteamHLFriends::_godot_getFriendAvatarShared(imageID, fallback);
}

Ref<Texture> SteamHLFriends::godot_getSmallFriendAvatar(SteamHLID* steamHLIDFriend, Ref<Texture> fallback) {
	int64_t imageID = SteamHLFriends::getSmallFriendAvatar(steamHLIDFriend->steamId);
	return SteamHLFriends::_godot_getFriendAvatarShared(imageID, fallback);
}

Ref<Texture> SteamHLFriends::_godot_getFriendAvatarShared(uint64_t imageID, Ref<Texture> fallback) {
	if (imageID == 0) {
		return fallback;
	}
	else {
		uint32 width, height;
		if (SteamHLUtils::get_singleton()->GetImageSize(imageID, &width, &height)) {
			const int bufferSize = width * height * 4;
			PackedByteArray buffer; // TODO = I live in fear of memory leaks
			buffer.resize(bufferSize);
			uint8* imageRGBA = buffer.ptrw();
			if (SteamHLUtils::get_singleton()->GetImageRGBA(imageID, imageRGBA, bufferSize)) {
				Ref<Image> godotImage = Image::create_from_data(width, height, false, Image::FORMAT_RGBA8, buffer);
				return ImageTexture::create_from_image(godotImage);
			}
			else {
				return fallback;
			}
		}
		else {
			return fallback;
		}
	}
}

EPersonaState SteamHLFriends::GetPersonaState() {
	steamFriends_LOAD_OR_RETURN(k_EPersonaStateOffline);
	return steamFriends->GetPersonaState();
}