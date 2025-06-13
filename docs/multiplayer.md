# SteamHLPeer

## Usage
The `MultiplayerPeer` used by GDSteamHL, `SteamHLPeer`, is intended as a *almost*-drop-in replacement for Godot's `ENetMultiplayerPeer`. It's only differences (listed below) are its constructor arguments and an additional signal, `lobby_changed`, for dis/connecting from a Steam lobby. Additionally, you should connect to the `SteamHL.game_lobby_join_requested` signal so that players can join a multiplayer game from their friends list or from an invite. 

Once `SteamHLPeer` has been created and connected to a lobby, you can use it all the normal RPC, `MultiplayerSynchonizer`, `MultiplayerSpawner` and other Godot features just as you would with `ENetMultiplayerPeer` or any other `MultiplayerPeer`.

`SteamHLPeer` is peer-to-peer with a single player acting as the "server" for all other "client" players. All traffic is routed through the [Steam Datagram Relay](https://partner.steamgames.com/doc/features/multiplayer/steamdatagramrelay).

<details>
<summary>Editorial: A Note on Steam Outages</summary>

If Steam is down, then the Datagram Relay is down and your multiplayer will not work. Steam is pretty reliable, so this is rarely an issue. However, as of the time of writing this (2025), Steam has weekly maintenace around 5:00pm CST almost every Tuesday (and occasionally on Wednesday). It usually lasts a little under an hour. A good site to monitor Steam is the [Unofficial Steam Status](https://steamstat.us/).

I mention the weekly maintenace in particular because, for some reason, that is always when I try to do my multiplayer testing.

*Hunt (Mountain Peke Games)*
</details>

## Setup
### Server
1. Create the peer with `SteamHLPeer.new()`.
2. Initialize the peer as a server with `create_server(LOBBY_SIZE)`, where `LOBBY_SIZE` is the maximum number of players than can connect to the lobby (including this host). This function will return `true` is 
3. *(Optional)* Await or connect to the `lobby_changed` be notified when the peer finishes connecting to the Steam lobby. Alternatively, you can connect or await `create_lobby_result`. The `lobby_changed` signal is emitted with the SteamID of the lobby, and `create_lobby_result` signal is emitted with an [`EChatRoomEnterResponse`](https://partner.steamgames.com/doc/api/steam_api#EChatRoomEnterResponse) integer, with 1 signifying success and all others some sort of error.

### Client
1. Create the peer with `SteamHLPeer.new()`.
2. Initialize the peer as a client with `create_client(LOBBY_ID)`, where `LOBBY_ID` is the ID of the lobby to join. See [Handling Lobby IDs](#handling-lobby-ids) below.
3. *(Optional)* Await or connect to the `lobby_changed` be notified when the peer finishes connecting to the Steam lobby. Alternatively, you can connect or await `join_lobby_result`. The `lobby_changed` signal is emitted with the SteamID of the lobby, and `join_lobby_result` signal is emitted with an [`EChatRoomEnterResponse`](https://partner.steamgames.com/doc/api/steam_api#EChatRoomEnterResponse) integer, with 1 signifying success and all others some sort of error.
4. *(Recommended, Optional)* Connect the `SteamHL.game_lobby_join_requested` signal to your logic for creating clients so that players can join the game from invites and their friends list. `game_lobby_join_requested` is emitted with the lobby' SteamID.

## Handling Lobby IDs
Instead of clients connecting to a specific player, they connect to a lobby by its SteamID. SteamHL thinly wraps Steamworks SteamID object in a `SteamHLID` object.

### Built-In Steam UI
Once the host peer has connected to a Steam lobby, their friends can join though the Steam's UI by selecting the "Join Game" option under their friend in the friends list. Similarly, the host can send invites through their friends lists.

If you've performed step 4 of the [client setup](#client), then players will automatically join when joining a game or accepting an invite through Steam's UI.

### Adding In-Game UI
#### Option 1: Display the SteamUI
The simplest way to add invite/join UI to your game is to just raise the appropriate Steam Overlay.

You can show a player's friends list with `SteamHLFriends.activate_game_overlay(SteamHLFriends.FRIENDS)`.

You can open the UI to invite friends by calling `SteamHLFriends.activate_game_overlay_invite_dialog(LOBBY_ID)`, where `LOBBY_ID` is the ID of the lobby the player is connected to. You can get the current lobby ID by calling `get_lobby_id()` on your peer, typically through `multiplayer.multiplayer_peer.get_lobby_id()`.

You **MUST** complete step 4 of the [client setup](#client) for join requests and invites to work via the Steam UI.

#### Option 2: Join Code and Field
A more natural way to have invite/join UI is to use Godot's UI system. A simple way to do this would be to display a join code on the server and add a field for clients to input it.

You can get the current lobby ID by calling `get_lobby_id()` on your peer, typically through `multiplayer.multiplayer_peer.get_lobby_id()`. Alternatively, you can track it through the `lobby_changed` signal. Regardless, once you know the lobby ID as a `SteamHLID`, you will need to translate it to a human-readable join code to display. You can it into a displayable number with `get_account_id()`. This is still a large number, so it is recommended that you convert it to a smaller, more readable join code with some sort of string encoding, such as base64. Once you have your join code, displaying it is as simple as updating your Godot UI.

When a client joins using the code, you will need to translate it back into a `SteamHLID`:
1. Reverse your string encoding to get the Steam account ID.
2. Create an ID object with `SteamHLID.new()`.
3. Set its values with `set_from_instanced_values(account_id, FLAGS, SteamHLID.k_EUniversePublic, SteamHLID.k_EAccountTypeChat)`, where `FLAGS` is `SteamHLID.k_EChatInstanceFlagLobby | SteamHLID.k_EChatInstanceFlagMMSLobby`. This creates a ID of the lobby ID type with the account ID retreived from the join code.