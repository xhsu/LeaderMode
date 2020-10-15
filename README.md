# Leader Mod
Try [AMX Version](https://github.com/ShingekiNoRex/CS1.6-LeaderMode) before you decide to download this. The AMX version is utterly playable with almost same contents.

## What is this?
A gameplay mod originally wrote in Pawn(AmxModX).<br/>
Exclusive roles will be assigned to each player in both teams. The roles mean unique skills, weapon limitations, and playstyles.<br/>
The ultimate goal is to eliminate the other team. Use whatever strategies you want. Tips: kill their leader will stop their redeployment.

## How to install?
1. You have to own copies of all these three games: [Half-Life](https://store.steampowered.com/app/70/HalfLife/), [Condition Zero](https://store.steampowered.com/app/80/CounterStrike_Condition_Zero/) and [Counter-Strike](https://store.steampowered.com/app/10/CounterStrike/).
2. Rename the RESOURCE/ folder to "leadermode" or something like that, than place it along with GoldSrc engine like other mods..
3. Install fonts located in RESOURCE/ folder into Windows.
4. Download the released version or build the game DLLs by yourself. Place the game DLLs in their corresponding locations.
5. Build [metahook](https://github.com/nagist/metahook). Rename the .exe file to "leadermode" as well, and place it along with the GoldSrc engine.
6. Move the fmod.dll in RESOURCE/ to where leadermode.exe currently located.
7. Start leadermode.exe and enjoy the game.

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

### Credits
* [ReGameDLL_CS](https://github.com/s1lentq/ReGameDLL_CS) (This mod was created on the basis of ReGameDLL_CS)
* [Metahook](https://github.com/nagist/metahook) (A total game changer. Literally and metaphorically.)
* [FMOD] (https://www.fmod.com/) (Via whom we get rid of the original obsolete sound system of GoldSrc.)

## How can I help the project?
Just try it and contact me if you have anything to say.
