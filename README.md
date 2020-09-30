# Leader Mod
Try [AMX Version](https://github.com/ShingekiNoRex/CS1.6-LeaderMode) before you decide to download this. The AMX version is utterly playable with almost same content.

## What is this?
A gameplay mod originally wrote in Pawn(AmxModX).<br/>
Assign roles to both CT and T, including skills.<br/>
The goal is eliminate the other team. However, due to the redeployment mechanism, having their team leader killed is a quicker way.

## How to install?
1. You have to own copies of all these three games: [Half-Life](https://store.steampowered.com/app/70/HalfLife/), [Condition Zero](https://store.steampowered.com/app/80/CounterStrike_Condition_Zero/) and [Counter-Strike](https://store.steampowered.com/app/10/CounterStrike/).
2. Create a folder in the GoldSrc engine folder.
3. Rename the RESOURCE/ folder to "leadermode" or something like that.
4. Install fonts located in RESOURCE/ folder into Windows.
5. Build [metahook](https://github.com/nagist/metahook). Rename the .exe file to "leadermode" as well, and place it along with the GoldSrc engine.
6. Start leadermode.exe and enjoy the game.

## Known Issues
* Players have to press and hold CTRL to keep their speed while ducking.
* Running into walls will cause a sudden stop of Dash_Out anim.
* Models cannot emit sounds during weapon swapping, if the sound EVENT is assigned on the first frame.
* WeaponsList HUD could flashback and forth during weapon swapping.

## Commands
| Command                             | Description                                     |
| :---------------------------------- | :---------------------------------------------- |
| +qtg                                | Throw grenade. It works just like R6:S. (no cooking support... yet.) |
| lastinv                             | Cancel grenade throw. It also works as original quick weapon switch. |
| eqpnext                             | Choose next equipment for +qtg usage from your inventory. |
| eqpprev                             | Choose previous equipment for +qtg usage from your inventory. |
| melee                               | Swing the default knife for emergency attack. This won't cancel reload but pause it. |
| executeskill                        | Execute skills designed for your role. |
| declarerole                         | Open a menu with which you may choose your role. |
| votescheme                          | Vote the tactical scheme for your team. |

## Configuration (cvars)
<details>
<summary>Click to expand</summary>

| CVar                               | Default | Min | Max          | Description                                    |
| :--------------------------------- | :-----: | :-: | :----------: | :--------------------------------------------- |
| UNDER CONSTRUCTION                 | :-----: | :-: | :----------: | :--------------------------------------------- |
</details>

## How to install zBot for CS 1.6?
* Extract all the files from an [archive](regamedll/extra/zBot/bot_profiles.zip?raw=true)
* Enter `-bots` option at the command line HLDS

### Credits
* [ReGameDLL_CS](https://github.com/s1lentq/ReGameDLL_CS) (This mod was created on the basis of ReGameDLL_CS)
* [Metahook](https://github.com/nagist/metahook) (A total game changer. Literally and metaphorically.)

## How can I help the project?
Just install it on your game server and report problems you faced.<br />
Merge requests are also welcome :shipit:
