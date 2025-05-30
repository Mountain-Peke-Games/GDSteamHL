# Installation Instructions
GDSteamHL is Steam integration, so it naturally needs the Steamworks SDK. This is distributed by Steam to Steam Partners. As of this time of writing (2025), you must register to be a Steam Partner and be approved before downloading the API. 

<details>
<summary>Editorial: Steamworks Sign-Up</summary>
This is essentially registering your business with Steam. The process is quick, but filling out business information and tax information can be very intimidating.
 
If you're like me, most of your time will actually be spent fussing over business details (e.g., registering your DBA name in the applicable jurisdictions) rather than actually signing up and waiting for approval. The actual sign-up process was just a cheap (relatively speaking) $100 fee and waiting a few days for Steam to review-and-approve my business information, which was very simple as a sole proprietor.
 
If you plan for your game to use Steam's features (which is optional, but I assume that's why you're here), I recommend starting the registration process and then continuing work on your game during the short wait for approval. Steam integration should **not** be a blocker for your game.
 
See the [Steamworks Onboarding Documentation](https://partner.steamgames.com/doc/gettingstarted/onboarding) for the best, most accurate, up-to-date information. 
 
*Hunt (Mountain Peke Games)*
</details>

<br/>

Once you have access to the Steamworks SDK and downloaded it, place the **contents** of the `sdk/public/src` directory in `steamworks-api/public` directory and the **contents** of the `sdk/redistributable_bin` in `steamworks-api/redistributable_bin`. Alternatively, you can modify the `STEAM_HEADERS` and `STEAM_BINARIES` variables in the `scons` file to point to your system's installation location.

You're now ready to build GDSteamHL!