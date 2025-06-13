# GDSteamHL

GDSteamHL (GDExtension + Steam + High-Level) wraps a *subset* of the Steamworks API for use in GDScript. Rather and being 1-to-1 with the Steamworks API, GDSteamHL focuses on "high-level" integration that abstracts away what would be implementation details in GDScript (e.g., packet handling) in order to simplify integration and GDScript logic.

## Features
1. A `MultiplayerPeer` that uses the Steam Datagram Relay and Steam Lobbies for P2P multiplayer.
2. `InputEvents` for SteamInput controls.
3. Scattered access to other Steamworks APIs.

## Limitations
1. Windows only.
2. Only a subset of Steamworks APIs are exposed.

## Documentation
- Installation ([here](#installation))
- Alternatives to GDSteamHL ([here](#alternatives))
- [Supported Steamworks APIs](docs/steamworks_support.md)
- [Multiplayer Peer](docs/multiplayer.md)
- [SteamInput](docs/input.md)

## Installation
1. Build from source ([see below](#building-from-source)).
2. Copy the files from the demo project's `res://bin/gdsteamhl` directory into a `res://bin/gdsteamhl` directory in your project. This should contain:
    1. `gdsteamhl.gdextension`, which tells Godot how to load the extension and its dependencies (the Steamworks library). If you want to install GDSteamHL in a different location, then you will need to modify the paths in this file.
    2. Your `libgdsteamhl.{platform}.{debug/release}.{architecture}` binaries. For Windows, this is `libgdsteamhl.windows.template_debug.x86_64.dll` for the Editor and `libgdsteamhl.windows.template_release.x86_64.dll` for non-debug builds.
    3. The relevant steam API binary. For Windows, this is `steam_api64.dll` from `steamworks-api/redistributable_bin/win64`.
3. Add `steam_appid.txt` to `res://` and set it's contents to your Steam App ID.
4. Launch your Steam Client.

## Building from Source
1. Install the requirements:
    1. git
    2. The Scons buildtool ([documentation](https://scons.org/doc/production/HTML/scons-user/ch01s02.html))
    3. A C++17 or greater compiler (e.g., MSVC for [VSCode](https://code.visualstudio.com/docs/cpp/config-msvc) or [Visual Studio](https://learn.microsoft.com/en-us/cpp/build/vscpp-step-0-installation))
2. Clone the repo.
3. Initialize the godot-cpp submodule with `git submodule update --init --recursive` in the root directory.
4. Install the Steamworks API
    1. From the Steamworks API source, copy the **contents** of `sdk/public/src` into `steamworks-api/public` under the GDSteamHL root directory.
    2. From the Steamworks API source, copy the **contents** of `sdk/redistributable_bin` into `steamworks-api/redistributable_bin` under the GDSteamHL root directory.
    3. For more details, see the steamworks-api [README](steamworks-api/README.md).
5. Build with the `scons` command:
    1. Debug/Editor: `scons platform=windows`
    2. Release: `scons platform=windows target=template_release`
    3. The binaries will be in `demo/bin/gdsteamhl`.

## Alternatives
Your main alternative implementation will be Expressobits' Steam Multiplayer Peer Extension ([link](https://github.com/expressobits/steam-multiplayer-peer)) plus [GodotSteam](https://godotsteam.com/). Expressobits' peer allows for P2P connections by users' SteamIDs, but GodotSteam is needed to actually retrieve the SteamIDs. Both are great projects with clean codebases and good resources, so I recommend checking them out!

GodotSteam actually has a multiplayer peer ([link](https://github.com/GodotSteam/MultiplayerPeer)) that is similar to Expressobits', but it is currently a module and *not* a GDExtension.

<details>
<summary>Editorial: Modules vs. Extensions</summary>
Godot Extensions (aka GDExtensions) are standalone libraries that can be added to a Godot project without much fuss. Godot Modules require recompiling the engine from source.

Under the hood, both share a very similar API and thus their code is mostly-but-not-completely interchangeable. Modules access the APIs directly as part of the engine. Extensions access the GDExtension interface. This interface consists of function points that Godot will incrementally initialize during its startup process.

Choosing one over the other is a choice between UNLIMITED POWER (modules) or PORTABILITY (extensions). Unless you are performing some low level integration or need to access singletons earlier than they are exposed to GDExtension (e.g., Engine in pre-4.4 Godot before Vulkan creates a window), then you probably will not need UNLIMITED POWER, and I'd personally recommend creating an extension so you can leverage its PORTABILITY and more easily take new engine versions.

Additionally, if you prefer to work with a language other than C++, there are community bindings for GDExtensions in other languages (e.g., Rust via godot-rust).

*Hunt (Mountain Peke Games)*
</details>

### Differences
Expressobits' client connects by **user** SteamID. GDSteamHL clients connect by **lobby** SteamID. The lobby system is what allows you to press "Join Game" and send invites in Steam, which is why GDSteamHL uses it. You can achieve the same behavior with Expressobits' peer by directly using the `ISteamMatchmaking` APIs exposed by GodotSteam.

GodotSteam wraps **all** Steamworks APIs.

Neither expose SteamInput events as a Godot `InputEvent`, but that's possible to implement yourself in GodotSteam.

<details>
<summary>Editorial: Why Use GDSteamHL?</summary>
Through GDSteamHL, the Steam Overlay (and SteamInput) work in the Godot Editor even when you use the Forward+ renderer, so you save time and energy not having to build and run from the Steam client. Now that Godot Engine PR#98862 has been merged, I suspect GodotSteam will address or already has addressed this.

Regardless, I would actually recommend using GodotSteam if you don't need a peer. It is established, actively maintained, and has a community. If you do need a peer, then seriously consider Expressobits'. His code is clean and even has a reference implementation for using lobbies.

This repo largely exists as a historical artifact and reference implementation. Work on GDSteamHL stared around June 2023, predating the GitHub release of Expressobits' peer by a few months. GodotSteam's GDExtension implementation existed at that point, but its multiplayer peer was still a module. That (and admittedly some desire to learn it myself) is why GDSteamHL exists today. History has made it redundant though, in my eyes.

*Hunt (Mountain Peke Games)*
</details>
