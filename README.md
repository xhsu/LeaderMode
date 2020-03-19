# Leader Mod
Try [AMX Version](https://github.com/ShingekiNoRex/CS1.6-LeaderMode) before you decide to download this. The AMX version is utterly playable with almost same content.

## What is this?
A gameplay mod originally wrote in Pawn(AmxModX).<br/>
Assign roles to both CT and T, including skills.<br/>
The goal is eliminate the other team. However, due to the redeployment mechanism, having their team leader killed is a quicker way.

## How to install?
You must buy a CS:CZ copy from Valve.<br/>
Download the resource pack from AMX version. In addition, you have to download the view model pack of WeaponSight.amxx which currently not avaliable.<br/>
Compile both client.dll and mp.dll, replace the corresponding file in czero/. Make yourself a backup.

## Commands
| Command                             | Description                                     |
| :---------------------------------- | :---------------------------------------------- |
| +qtg                                | Throw grenade. It works just like R6:S. |
| melee                               | Swing the default knife for emergency attack. This won't cancel reload but pause it. |
| executeskill                        | Args:<br/> 1~4 for different character skill. This would be improve later. |
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
Thanks to the project [ReGameDLL_CS](https://github.com/s1lentq/ReGameDLL_CS) ( This mod was created on the basis of ReGameDLL_CS )

## How can I help the project?
Just install it on your game server and report problems you faced.<br />
Merge requests are also welcome :shipit:
