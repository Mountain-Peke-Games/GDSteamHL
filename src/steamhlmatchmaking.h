#ifndef STEAMHLMATCHMAKING_H
#define STEAMHLMATCHMAKING_H
#include "steamhl.h"

#include <steam_api.h>

#define steamHLMatchmaking_LOAD_OR_ERROR(errMsg) STEAMHL_LOAD_OR_ERROR(steamMatchmaking, SteamMatchmaking, errMsg);
#define steamHLMatchmaking_LOAD_OR_RETURN(failureValue) STEAMHL_LOAD_OR_RETURN(steamMatchmaking, SteamMatchmaking, failureValue);

// TODO = Consider exposing to Godot
namespace godot {
	class SteamHLMatchmaking {

	public:
		static bool createLobby(ELobbyType eLobbyType, int cMaxMembers, SteamAPICall_t* apiCall);
		static bool joinLobby(CSteamID steamIDLobby, SteamAPICall_t* apiCall);
		static void leaveLobby(CSteamID steamIDLobby);

		static const char* getLobbyData(CSteamID steamIDLobby, const char* pchKey);
		static bool getLobbyDataByIndex(CSteamID steamIDLobby, int iLobbyData, char* pchKey, int cchKeyBufferSize, char* pchValue, int ccValueBufferSize);
		static int getLobbyDataCount(CSteamID steamIDLobby);
		static CSteamID getLobbyMemberByIndex(CSteamID steamIDLobby, int iMember);
		static const char* getLobbyMemberData(CSteamID steamIDLobby, CSteamID steamIDUser, const char* pchKey);
		static int getLobbyMemberLimit(CSteamID steamIDLobby);
		static CSteamID getLobbyOwner(CSteamID steamIDLobby);
		static int getNumLobbyMembers(CSteamID steamIDLobby);
		static bool inviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvite);
	};
}
#endif // !STEAMHLMATCHMAKING_H
