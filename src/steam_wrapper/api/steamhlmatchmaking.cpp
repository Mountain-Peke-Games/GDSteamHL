#include "steamhlmatchmaking.h"

using namespace godot;

bool SteamHLMatchmaking::createLobby(ELobbyType eLobbyType, int cMaxMembers, SteamAPICall_t* apiCall) {
	steamHLMatchmaking_LOAD_OR_RETURN(false);
	*apiCall = steamMatchmaking->CreateLobby(eLobbyType, cMaxMembers);
	return true;
}

bool SteamHLMatchmaking::joinLobby(CSteamID steamIDLobby, SteamAPICall_t* apiCall) {
	steamHLMatchmaking_LOAD_OR_RETURN(false);
	*apiCall = steamMatchmaking->JoinLobby(steamIDLobby);
	return true;
}

void SteamHLMatchmaking::leaveLobby(CSteamID steamIDLobby) {
	steamHLMatchmaking_LOAD_OR_ERROR("Failed to load SteamMatchmaking");
	steamMatchmaking->LeaveLobby(steamIDLobby);
}

const char* SteamHLMatchmaking::getLobbyData(CSteamID steamIDLobby, const char* pchKey) {
	steamHLMatchmaking_LOAD_OR_RETURN(false);
	return steamMatchmaking->GetLobbyData(steamIDLobby, pchKey);
}

bool SteamHLMatchmaking::getLobbyDataByIndex(CSteamID steamIDLobby, int iLobbyData, char* pchKey, int cchKeyBufferSize, char* pchValue, int ccValueBufferSize) {
	steamHLMatchmaking_LOAD_OR_RETURN("");
	return steamMatchmaking->GetLobbyDataByIndex(steamIDLobby, iLobbyData, pchKey, cchKeyBufferSize, pchValue, ccValueBufferSize);
}

int SteamHLMatchmaking::getLobbyDataCount(CSteamID steamIDLobby) {
	steamHLMatchmaking_LOAD_OR_RETURN(0);
	return steamMatchmaking->GetLobbyDataCount(steamIDLobby);
}

CSteamID SteamHLMatchmaking::getLobbyMemberByIndex(CSteamID steamIDLobby, int iMember) {
	steamHLMatchmaking_LOAD_OR_RETURN(k_steamIDNil);
	return steamMatchmaking->GetLobbyMemberByIndex(steamIDLobby, iMember);
}

const char* SteamHLMatchmaking::getLobbyMemberData(CSteamID steamIDLobby, CSteamID steamIDUser, const char* pchKey) {
	steamHLMatchmaking_LOAD_OR_RETURN("");
	return steamMatchmaking->GetLobbyMemberData(steamIDLobby, steamIDUser, pchKey);
}

int SteamHLMatchmaking::getLobbyMemberLimit(CSteamID steamIDLobby) {
	steamHLMatchmaking_LOAD_OR_RETURN(0);
	return steamMatchmaking->GetLobbyMemberLimit(steamIDLobby);
}

CSteamID SteamHLMatchmaking::getLobbyOwner(CSteamID steamIDLobby) {
	steamHLMatchmaking_LOAD_OR_RETURN(k_steamIDNil);
	return steamMatchmaking->GetLobbyOwner(steamIDLobby);
}

int SteamHLMatchmaking::getNumLobbyMembers(CSteamID steamIDLobby) {
	steamHLMatchmaking_LOAD_OR_RETURN(0);
	return steamMatchmaking->GetNumLobbyMembers(steamIDLobby);
}

bool SteamHLMatchmaking::inviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvite) {
	steamHLMatchmaking_LOAD_OR_RETURN(false);
	return steamMatchmaking->InviteUserToLobby(steamIDLobby, steamIDInvite);
}