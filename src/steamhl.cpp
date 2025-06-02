#include "steamhl.h"
#include "steamhlid.h"
#include "steamhlinput.h"
#include "steamhluserstats.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/main_loop.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

const char* SteamHL::gameLobbyJoinRequestedSignal = "game_lobby_join_requested";

SteamHL* SteamHL::_singleton = nullptr;
bool SteamHL::_steamApiInitialized = false;

SteamHL::SteamHL() {
	if (SteamHL::_singleton != nullptr) {
		// TODO=Doesn't actually print anything to the console
		UtilityFunctions::print("Warning: Someone is creating SteamHL instead of using the singleton");
	}

	SteamHL::_singleton = this;
}

SteamHL::~SteamHL() {
	this->shutdown();
	SteamHL::_singleton = nullptr;
}

void SteamHL::shutdown() {
	if (SteamHL::_steamApiInitialized) {
		SteamInput()->Shutdown(); //TODO=What if SteamInput returns null?
		SteamAPI_Shutdown();
		SteamHL::_steamApiInitialized = false;
	}
}

void SteamHL::_process() {
	if (SteamHL::isInitialized()) {
		SteamAPI_RunCallbacks();
		SteamHLInput::get_singleton()->_process();
	}
}

void SteamHL::_bind_methods() {
	ADD_SIGNAL(MethodInfo(SteamHL::gameLobbyJoinRequestedSignal, PropertyInfo(Variant::OBJECT, "lobby_id"))); // TODO=Have property hint use SteamHLID::name for class_name
	ClassDB::bind_method(D_METHOD("_process"), &SteamHL::_process); // Needed for signal registration
	ClassDB::bind_method(D_METHOD("initialize"), &SteamHL::initialize);
	ClassDB::bind_method(D_METHOD("is_initialized"), &SteamHL::isInitialized);
	ClassDB::bind_method(D_METHOD("is_steam_running"), &SteamHL::isSteamRunning);
	ClassDB::bind_method(D_METHOD("shutdown"), &SteamHL::shutdown);

	BIND_CONSTANT(k_EResultNone);							// no result
	BIND_CONSTANT(k_EResultOK);
	BIND_CONSTANT(k_EResultFail);
	BIND_CONSTANT(k_EResultNoConnection);
	BIND_CONSTANT(k_EResultInvalidPassword);
	BIND_CONSTANT(k_EResultLoggedInElsewhere);
	BIND_CONSTANT(k_EResultInvalidProtocolVer);
	BIND_CONSTANT(k_EResultInvalidParam);
	BIND_CONSTANT(k_EResultFileNotFound);
	BIND_CONSTANT(k_EResultBusy);
	BIND_CONSTANT(k_EResultInvalidState);
	BIND_CONSTANT(k_EResultInvalidName);
	BIND_CONSTANT(k_EResultInvalidEmail);
	BIND_CONSTANT(k_EResultDuplicateName);
	BIND_CONSTANT(k_EResultAccessDenied);
	BIND_CONSTANT(k_EResultTimeout);
	BIND_CONSTANT(k_EResultBanned);
	BIND_CONSTANT(k_EResultAccountNotFound);
	BIND_CONSTANT(k_EResultInvalidSteamID);
	BIND_CONSTANT(k_EResultServiceUnavailable);
	BIND_CONSTANT(k_EResultNotLoggedOn);
	BIND_CONSTANT(k_EResultPending);
	BIND_CONSTANT(k_EResultEncryptionFailure);
	BIND_CONSTANT(k_EResultInsufficientPrivilege);
	BIND_CONSTANT(k_EResultLimitExceeded);
	BIND_CONSTANT(k_EResultRevoked);
	BIND_CONSTANT(k_EResultExpired);
	BIND_CONSTANT(k_EResultAlreadyRedeemed);
	BIND_CONSTANT(k_EResultDuplicateRequest);
	BIND_CONSTANT(k_EResultAlreadyOwned);
	BIND_CONSTANT(k_EResultIPNotFound);
	BIND_CONSTANT(k_EResultPersistFailed);
	BIND_CONSTANT(k_EResultLockingFailed);
	BIND_CONSTANT(k_EResultLogonSessionReplaced);
	BIND_CONSTANT(k_EResultConnectFailed);
	BIND_CONSTANT(k_EResultHandshakeFailed);
	BIND_CONSTANT(k_EResultIOFailure);
	BIND_CONSTANT(k_EResultRemoteDisconnect);
	BIND_CONSTANT(k_EResultShoppingCartNotFound);
	BIND_CONSTANT(k_EResultBlocked);
	BIND_CONSTANT(k_EResultIgnored);
	BIND_CONSTANT(k_EResultNoMatch);
	BIND_CONSTANT(k_EResultAccountDisabled);
	BIND_CONSTANT(k_EResultServiceReadOnly);
	BIND_CONSTANT(k_EResultAccountNotFeatured);
	BIND_CONSTANT(k_EResultAdministratorOK);
	BIND_CONSTANT(k_EResultContentVersion);
	BIND_CONSTANT(k_EResultTryAnotherCM);
	BIND_CONSTANT(k_EResultPasswordRequiredToKickSession);
	BIND_CONSTANT(k_EResultAlreadyLoggedInElsewhere);
	BIND_CONSTANT(k_EResultSuspended);
	BIND_CONSTANT(k_EResultCancelled);
	BIND_CONSTANT(k_EResultDataCorruption);
	BIND_CONSTANT(k_EResultDiskFull);
	BIND_CONSTANT(k_EResultRemoteCallFailed);
	BIND_CONSTANT(k_EResultPasswordUnset);
	BIND_CONSTANT(k_EResultExternalAccountUnlinked);
	BIND_CONSTANT(k_EResultPSNTicketInvalid);
	BIND_CONSTANT(k_EResultExternalAccountAlreadyLinked);
	BIND_CONSTANT(k_EResultRemoteFileConflict);
	BIND_CONSTANT(k_EResultIllegalPassword);
	BIND_CONSTANT(k_EResultSameAsPreviousValue);
	BIND_CONSTANT(k_EResultAccountLogonDenied);
	BIND_CONSTANT(k_EResultCannotUseOldPassword);
	BIND_CONSTANT(k_EResultInvalidLoginAuthCode);
	BIND_CONSTANT(k_EResultAccountLogonDeniedNoMail);
	BIND_CONSTANT(k_EResultHardwareNotCapableOfIPT);
	BIND_CONSTANT(k_EResultIPTInitError);
	BIND_CONSTANT(k_EResultParentalControlRestricted);
	BIND_CONSTANT(k_EResultFacebookQueryError);
	BIND_CONSTANT(k_EResultExpiredLoginAuthCode);
	BIND_CONSTANT(k_EResultIPLoginRestrictionFailed);
	BIND_CONSTANT(k_EResultAccountLockedDown);
	BIND_CONSTANT(k_EResultAccountLogonDeniedVerifiedEmailRequired);
	BIND_CONSTANT(k_EResultNoMatchingURL);
	BIND_CONSTANT(k_EResultBadResponse);
	BIND_CONSTANT(k_EResultRequirePasswordReEntry);
	BIND_CONSTANT(k_EResultValueOutOfRange);
	BIND_CONSTANT(k_EResultUnexpectedError);
	BIND_CONSTANT(k_EResultDisabled);
	BIND_CONSTANT(k_EResultInvalidCEGSubmission);
	BIND_CONSTANT(k_EResultRestrictedDevice);
	BIND_CONSTANT(k_EResultRegionLocked);
	BIND_CONSTANT(k_EResultRateLimitExceeded);
	BIND_CONSTANT(k_EResultAccountLoginDeniedNeedTwoFactor);
	BIND_CONSTANT(k_EResultItemDeleted);
	BIND_CONSTANT(k_EResultAccountLoginDeniedThrottle);
	BIND_CONSTANT(k_EResultTwoFactorCodeMismatch);
	BIND_CONSTANT(k_EResultTwoFactorActivationCodeMismatch);
	BIND_CONSTANT(k_EResultAccountAssociatedToMultiplePartners);
	BIND_CONSTANT(k_EResultNotModified);
	BIND_CONSTANT(k_EResultNoMobileDevice);
	BIND_CONSTANT(k_EResultTimeNotSynced);
	BIND_CONSTANT(k_EResultSmsCodeFailed);
	BIND_CONSTANT(k_EResultAccountLimitExceeded);
	BIND_CONSTANT(k_EResultAccountActivityLimitExceeded);
	BIND_CONSTANT(k_EResultPhoneActivityLimitExceeded);
	BIND_CONSTANT(k_EResultRefundToWallet);
	BIND_CONSTANT(k_EResultEmailSendFailure);
	BIND_CONSTANT(k_EResultNotSettled);
	BIND_CONSTANT(k_EResultNeedCaptcha);
	BIND_CONSTANT(k_EResultGSLTDenied);
	BIND_CONSTANT(k_EResultGSOwnerDenied);
	BIND_CONSTANT(k_EResultInvalidItemType);
	BIND_CONSTANT(k_EResultIPBanned);
	BIND_CONSTANT(k_EResultGSLTExpired);
	BIND_CONSTANT(k_EResultInsufficientFunds);
	BIND_CONSTANT(k_EResultTooManyPending);
	BIND_CONSTANT(k_EResultNoSiteLicensesFound);
	BIND_CONSTANT(k_EResultWGNetworkSendExceeded);
	BIND_CONSTANT(k_EResultAccountNotFriends);
	BIND_CONSTANT(k_EResultLimitedUserAccount);
	BIND_CONSTANT(k_EResultCantRemoveItem);
	BIND_CONSTANT(k_EResultAccountDeleted);
	BIND_CONSTANT(k_EResultExistingUserCancelledLicense);
	BIND_CONSTANT(k_EResultCommunityCooldown);
	BIND_CONSTANT(k_EResultNoLauncherSpecified);
	BIND_CONSTANT(k_EResultMustAgreeToSSA);
	BIND_CONSTANT(k_EResultLauncherMigrated);
	BIND_CONSTANT(k_EResultSteamRealmMismatch);
	BIND_CONSTANT(k_EResultInvalidSignature);
	BIND_CONSTANT(k_EResultParseFailure);
	BIND_CONSTANT(k_EResultNoVerifiedPhone);
	BIND_CONSTANT(k_EResultInsufficientBattery);
	BIND_CONSTANT(k_EResultChargerRequired);
	BIND_CONSTANT(k_EResultCachedCredentialInvalid);
	BIND_CONSTANT(K_EResultPhoneNumberIsVOIP);
}

SteamHL* SteamHL::get_singleton() {
	if (SteamHL::_singleton == nullptr) {
		SteamHL::_singleton = memnew(SteamHL);
	}
	return SteamHL::_singleton;
}

bool SteamHL::isInitialized() {
	return SteamHL::_steamApiInitialized;
}

bool SteamHL::initialize() {
	if (SteamAPI_Init()) {
		SteamHL::_steamApiInitialized = true;
		if (SteamInput()->Init(true)) { // TODO=What to do if null?
			UtilityFunctions::print("Successfully started SteamInput");
		}
		else {
			UtilityFunctions::print("Failed to load SteamInput");
		}
		// Since this isn't a Node, we can't listen for NOTIFICATION_PROCESS like a good little node
		MainLoop* mainLoop = Engine::get_singleton()->get_main_loop();
		SceneTree* sceneTree = Object::cast_to<SceneTree>(mainLoop);
		Callable processCallable = Callable(this, "_process");
		sceneTree->connect("process_frame", processCallable);
		
		return true;
	}
	else {
		SteamHL::_steamApiInitialized = false;
		return false;
	}
}

bool SteamHL::isSteamRunning() {
	return SteamAPI_IsSteamRunning();
}

void SteamHL::onGameLobbyJoinRequested(GameLobbyJoinRequested_t* callback) {
	//SteamHLID* lobbySteamHLId = memnew(SteamHLID(callback->m_steamIDLobby));
	Ref<SteamHLID> lobbySteamHLId(memnew(SteamHLID(callback->m_steamIDLobby)));
	this->emit_signal(SteamHL::gameLobbyJoinRequestedSignal, lobbySteamHLId);
}