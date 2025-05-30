#include "steamhlid.h"

#include <godot_cpp/core/class_db.hpp>

// TODO = Change to use CSteamID copy constructor instead of a pointer
using namespace godot;

SteamHLID::SteamHLID() {
	this->steamId = k_steamIDNil;
}

SteamHLID::SteamHLID(CSteamID cSteamID) {
	this->steamId = cSteamID;
}

SteamHLID::~SteamHLID() { }

void SteamHLID::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_from_values", "accountID", "universe", "accountType"), &SteamHLID::godot_Set); // Cannot be "set" because objects in Godot already have a set method
	ClassDB::bind_method(D_METHOD("set_from_instanced_values", "accountID", "instance", "universe", "accountType"), &SteamHLID::godot_InstancedSet); // Naming convention taken from above
	ClassDB::bind_method(D_METHOD("set_from_uint64", "steamID"), &SteamHLID::godot_SetFromUint64);
	ClassDB::bind_method(D_METHOD("get_account_id"), &SteamHLID::godot_GetAccountID);
	ClassDB::bind_method(D_METHOD("get_account_type"), &SteamHLID::godot_GetEAccountType);
	ClassDB::bind_method(D_METHOD("get_universe"), &SteamHLID::godot_GetEUniverse);
	ClassDB::bind_method(D_METHOD("convert_to_uint64"), &SteamHLID::godot_ConvertToUInt64);
	ClassDB::bind_method(D_METHOD("get_static_account_key"), &SteamHLID::godot_GetStaticAccountKey);
	ClassDB::bind_method(D_METHOD("blank_anon_account"), &SteamHLID::godot_BBlankAnonAccount);
	ClassDB::bind_method(D_METHOD("game_server_account"), &SteamHLID::godot_BGameServerAccount);
	ClassDB::bind_method(D_METHOD("persistent_game_server_account"), &SteamHLID::godot_BPersistentGameServerAccount);
	ClassDB::bind_method(D_METHOD("anon_game_server_account"), &SteamHLID::godot_BAnonGameServerAccount);
	ClassDB::bind_method(D_METHOD("content_server_account"), &SteamHLID::godot_BContentServerAccount);
	ClassDB::bind_method(D_METHOD("clan_account"), &SteamHLID::godot_BClanAccount);
	ClassDB::bind_method(D_METHOD("chat_account"), &SteamHLID::godot_BChatAccount);
	ClassDB::bind_method(D_METHOD("is_lobby"), &SteamHLID::godot_IsLobby);
	ClassDB::bind_method(D_METHOD("individual_account"), &SteamHLID::godot_BIndividualAccount);
	ClassDB::bind_method(D_METHOD("anon_account"), &SteamHLID::godot_BAnonAccount);
	ClassDB::bind_method(D_METHOD("anon_user_account"), &SteamHLID::godot_BAnonUserAccount);
	ClassDB::bind_method(D_METHOD("console_user_account"), &SteamHLID::godot_BConsoleUserAccount);
	ClassDB::bind_method(D_METHOD("is_valid"), &SteamHLID::godot_IsValid);

	BIND_ENUM_CONSTANT(k_EAccountTypeInvalid);
	BIND_ENUM_CONSTANT(k_EAccountTypeIndividual);
	BIND_ENUM_CONSTANT(k_EAccountTypeMultiseat);
	BIND_ENUM_CONSTANT(k_EAccountTypeGameServer);
	BIND_ENUM_CONSTANT(k_EAccountTypeAnonGameServer);
	BIND_ENUM_CONSTANT(k_EAccountTypePending);
	BIND_ENUM_CONSTANT(k_EAccountTypeContentServer);
	BIND_ENUM_CONSTANT(k_EAccountTypeClan);
	BIND_ENUM_CONSTANT(k_EAccountTypeChat);
	BIND_ENUM_CONSTANT(k_EAccountTypeConsoleUser);
	BIND_ENUM_CONSTANT(k_EAccountTypeAnonUser);
	BIND_ENUM_CONSTANT(k_EAccountTypeMax);

	BIND_ENUM_CONSTANT(k_EUniverseInvalid);
	BIND_ENUM_CONSTANT(k_EUniversePublic);
	BIND_ENUM_CONSTANT(k_EUniverseBeta);
	BIND_ENUM_CONSTANT(k_EUniverseInternal);
	BIND_ENUM_CONSTANT(k_EUniverseDev);
	BIND_ENUM_CONSTANT(k_EUniverseMax);

	BIND_ENUM_CONSTANT(k_EChatAccountInstanceMask);
	BIND_ENUM_CONSTANT(k_EChatInstanceFlagClan);
	BIND_ENUM_CONSTANT(k_EChatInstanceFlagLobby);
	BIND_ENUM_CONSTANT(k_EChatInstanceFlagMMSLobby);
}

void SteamHLID::godot_Set(uint32_t accountID, EUniverse universe, EAccountType accountType) {
	this->steamId.Set(static_cast<uint32>(accountID), universe, accountType);
}

void SteamHLID::godot_InstancedSet(uint32_t accountID, uint32_t instance, EUniverse universe, EAccountType accountType) {
	this->steamId.InstancedSet(static_cast<uint32>(accountID), static_cast<uint32>(instance), universe, accountType);
}

void SteamHLID::godot_SetFromUint64(uint64_t steamID) {
	this->steamId.SetFromUint64(static_cast<uint64>(steamID));
}

uint32_t SteamHLID::godot_GetAccountID() const {
	return static_cast<uint32_t>(this->steamId.GetAccountID());
}

EAccountType SteamHLID::godot_GetEAccountType() const {
	return this->steamId.GetEAccountType();
}

EUniverse SteamHLID::godot_GetEUniverse() const {
	return this->steamId.GetEUniverse();
}

uint64_t SteamHLID::godot_ConvertToUInt64() const {
	return static_cast<uint64_t>(this->steamId.ConvertToUint64());
}

uint64_t SteamHLID::godot_GetStaticAccountKey() const {
	return static_cast<uint64_t>(this->steamId.GetStaticAccountKey());
}

bool SteamHLID::godot_BBlankAnonAccount() const {
	return this->steamId.BBlankAnonAccount();
}

bool SteamHLID::godot_BGameServerAccount() const {
	return this->steamId.BGameServerAccount();
}

bool SteamHLID::godot_BPersistentGameServerAccount() const {
	return this->steamId.BPersistentGameServerAccount();
}

bool SteamHLID::godot_BAnonGameServerAccount() const {
	return this->steamId.BAnonGameServerAccount();
}

bool SteamHLID::godot_BContentServerAccount() const {
	return this->steamId.BContentServerAccount();
}

bool SteamHLID::godot_BClanAccount() const {
	return this->steamId.BClanAccount();
}

bool SteamHLID::godot_BChatAccount() const {
	return this->steamId.BChatAccount();
}

bool SteamHLID::godot_IsLobby() const {
	return this->steamId.IsLobby();
}

bool SteamHLID::godot_BIndividualAccount() const {
	return this->steamId.BIndividualAccount();
}

bool SteamHLID::godot_BAnonAccount() const {
	return this->steamId.BAnonAccount();
}

bool SteamHLID::godot_BAnonUserAccount() const {
	return this->steamId.BAnonUserAccount();
}

bool SteamHLID::godot_BConsoleUserAccount() const {
	return this->steamId.BConsoleUserAccount();
}

bool SteamHLID::godot_IsValid() const {
	return this->steamId.IsValid();
}