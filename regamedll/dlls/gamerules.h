/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

#include "game_shared/voice_gamemgr.h"
#include "cmdhandler.h"

const int MAX_RULE_BUFFER       = 1024;
const int MAX_VOTE_MAPS         = 100;
const int MAX_VIP_QUEUES        = 5;
const int MAX_MONEY_THRESHOLD   = 999999; // allowable money limit in the game that can be drawn on the HUD

const int MAX_MOTD_CHUNK        = 60;
const int MAX_MOTD_LENGTH       = 1536;	// (MAX_MOTD_CHUNK * 4)

const float ITEM_RESPAWN_TIME   = 30.0f;
const float WEAPON_RESPAWN_TIME = 20.0f;
const float AMMO_RESPAWN_TIME   = 20.0f;
const float ROUND_RESPAWN_TIME  = 20.0f;
const float ROUND_BEGIN_DELAY   = 5.0f;	// delay before beginning new round
const float ITEM_KILL_DELAY     = 300.0f;
const float RADIO_TIMEOUT       = 1.5f;

const int MAX_INTERMISSION_TIME = 120;	// longest the intermission can last, in seconds

// when we are within this close to running out of entities, items
// marked with the ITEM_FLAG_LIMITINWORLD will delay their respawn
const int ENTITY_INTOLERANCE    = 100;

enum
{
	WINSTATUS_NONE = 0,
	WINSTATUS_CTS,
	WINSTATUS_TERRORISTS,
	WINSTATUS_DRAW,
};

// Custom enum
// Used for EndRoundMessage() logged messages
enum ScenarioEventEndRound
{
	ROUND_NONE,
	ROUND_CTS_WIN,
	ROUND_TERRORISTS_WIN,
	ROUND_END_DRAW,
	ROUND_GAME_COMMENCE,
	ROUND_GAME_RESTART,
	ROUND_GAME_OVER
};

enum RewardRules
{
	RR_CTS_WIN,
	RR_TERRORISTS_WIN,
	RR_LOSER_BONUS_DEFAULT,
	RR_LOSER_BONUS_MIN,
	RR_LOSER_BONUS_MAX,
	RR_LOSER_BONUS_ADD,
	RR_LEADER_KILLED,
	RR_KILLED_ENEMY_LEADER,

	RR_END
};

// custom enum
enum RewardAccount
{
	REWARD_CTS_WIN                  = 3000,
	REWARD_TERRORISTS_WIN           = 3000,

	// loser bonus
	REWARD_LOSER_BONUS_DEFAULT      = 1400,
	REWARD_LOSER_BONUS_MIN          = 1500,
	REWARD_LOSER_BONUS_MAX          = 3000,
	REWARD_LOSER_BONUS_ADD          = 500,

	REWARD_KILLED_ENEMY             = 300,
	REWARD_KILLED_LEADER			= 3000,
	REWARD_LEADER_KILLED			= -1000,
};

// custom enum
enum PaybackForBadThing
{
	PAYBACK_FOR_KILLED_TEAMMATES    = -3300,
};

// custom enum
enum InfoMapBuyParam
{
	BUYING_EVERYONE = 0,
	BUYING_ONLY_CTS,
	BUYING_ONLY_TERRORISTS,
	BUYING_NO_ONE,
};

// weapon respawning return codes
enum
{
	GR_NONE = 0,

	GR_WEAPON_RESPAWN_YES,
	GR_WEAPON_RESPAWN_NO,

	GR_AMMO_RESPAWN_YES,
	GR_AMMO_RESPAWN_NO,

	GR_ITEM_RESPAWN_YES,
	GR_ITEM_RESPAWN_NO,

	GR_PLR_DROP_GUN_ALL,
	GR_PLR_DROP_GUN_ACTIVE,
	GR_PLR_DROP_GUN_NO,

	GR_PLR_DROP_AMMO_ALL,
	GR_PLR_DROP_AMMO_ACTIVE,
	GR_PLR_DROP_AMMO_NO,
};

// custom enum
enum
{
	SCENARIO_BLOCK_TIME_EXPRIRED      = BIT(0), // flag "a"
	SCENARIO_BLOCK_NEED_PLAYERS       = BIT(1), // flag "b"
	SCENARIO_BLOCK_TEAM_EXTERMINATION = BIT(5), // flag "f"
};

// Player relationship return codes
enum
{
	GR_NOTTEAMMATE = 0,
	GR_TEAMMATE,
	GR_ENEMY,
	GR_ALLY,
	GR_NEUTRAL,
};

class CItem;

class CGameRules
{
public:
	CGameRules();
	virtual ~CGameRules();

	virtual void RefreshSkillData();															// fill skill data struct with proper values
	virtual void Think() = 0;																	// runs every server frame, should handle any timer tasks, periodic events, etc.
	virtual BOOL IsAllowedToSpawn(CBaseEntity *pEntity) = 0;									// Can this item spawn (eg monsters don't spawn in deathmatch).

	virtual BOOL FAllowFlashlight() = 0;														// Are players allowed to switch on their flashlight?
	virtual BOOL FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon) = 0;		// should the player switch to this weapon?
	virtual BOOL GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon) = 0;	// I can't use this weapon anymore, get me the next best one.

	// Functions to verify the single/multiplayer status of a game
	virtual BOOL IsMultiplayer() = 0;												// is this a multiplayer game? (either coop or deathmatch)
	virtual BOOL IsDeathmatch() = 0;												// is this a deathmatch game?
	virtual BOOL IsTeamplay() { return FALSE; }										// is this deathmatch game being played with team rules?
	virtual BOOL IsCoOp() = 0;														// is this a coop game?
	virtual const char *GetGameDescription();										// this is the game name that gets seen in the server browser

	// Client connection/disconnection
	virtual BOOL ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char *szRejectReason) = 0;		// a client just connected to the server (player hasn't spawned yet)
	virtual void InitHUD(CBasePlayer *pl) = 0;											// the client dll is ready for updating
	virtual void ClientDisconnected(edict_t *pClient) = 0;								// a client just disconnected from the server
	virtual void UpdateGameMode(CBasePlayer *pPlayer) {};								// the client needs to be informed of the current game mode

	// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer) = 0;
	virtual BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker) { return TRUE; }		// can this player take damage from this attacker?
	virtual BOOL ShouldAutoAim(CBasePlayer *pPlayer, edict_t *target) { return TRUE; }

	// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer *pPlayer) = 0;										// called by CBasePlayer::Spawn just before releasing player into the game
	virtual void PlayerThink(CBasePlayer *pPlayer) = 0;										// called by CBasePlayer::PreThink every frame, before physics are run and after keys are accepted
	virtual BOOL FPlayerCanRespawn(CBasePlayer *pPlayer) = 0;								// is this player allowed to respawn now?
	virtual float FlPlayerSpawnTime(CBasePlayer *pPlayer) = 0;								// When in the future will this player be able to spawn?
	virtual edict_t *GetPlayerSpawnSpot(CBasePlayer *pPlayer);								// Place this player on their spawnspot and face them the proper direction.

	virtual BOOL AllowAutoTargetCrosshair() { return TRUE; }
	virtual BOOL ClientCommand_DeadOrAlive(CBasePlayer *pPlayer, const char *pcmd) { return FALSE; }
	virtual BOOL ClientCommand(CBasePlayer *pPlayer, const char *pcmd) { return FALSE; }					// handles the user commands;  returns TRUE if command handled properly
	virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer) {};							// the player has changed userinfo;  can change it now

	// Client kills/scoring
	virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled) = 0;							// how many points do I award whoever kills this player?
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor) = 0;			// Called each time a player dies
	virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor) = 0;		// Call this from within a GameRules class to report an obituary.

	// Weapon retrieval
	virtual BOOL CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pItem);							// The player is touching an CBasePlayerItem, do I give it to him?
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon) = 0;						// Called each time a player picks up a weapon from the ground

	// Weapon spawn/respawn control
	virtual int WeaponShouldRespawn(CBasePlayerItem *pWeapon) = 0;									// should this weapon respawn?
	virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon) = 0;								// when may this weapon respawn?
	virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon) = 0;									// can i respawn now,  and if not, when should i try again?
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon) = 0;								// where in the world should this weapon respawn?

	// Item retrieval
	virtual BOOL CanHaveItem(CBasePlayer *pPlayer, CItem *pItem) = 0;								// is this player allowed to take this item?
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem) = 0;								// call each time a player picks up an item (battery, healthkit, longjump)

	// Item spawn/respawn control
	virtual int ItemShouldRespawn(CItem *pItem) = 0;											// Should this item respawn?
	virtual float FlItemRespawnTime(CItem *pItem) = 0;											// when may this item respawn?
	virtual Vector VecItemRespawnSpot(CItem *pItem) = 0;										// where in the world should this item respawn?

	// Ammo retrieval
	virtual BOOL CanHaveAmmo(CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry);		// can this player take more of this ammo?
	virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, int iCount) = 0;				// called each time a player picks up some ammo in the world

	// Ammo spawn/respawn control
	virtual int AmmoShouldRespawn(CBasePlayerAmmo *pAmmo) = 0;									// should this ammo item respawn?
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo) = 0;								// when should this ammo item respawn?
	virtual Vector VecAmmoRespawnSpot(CBasePlayerAmmo *pAmmo) = 0;								// where in the world should this ammo item respawn?

	// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime() = 0;											// how long until a depleted HealthCharger recharges itself?
	virtual float FlHEVChargerRechargeTime() { return 0.0f; }									// how long until a depleted HealthCharger recharges itself?

	// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer *pPlayer) = 0;									// what do I do with a player's weapons when he's killed?

	// What happens to a dead player's ammo
	virtual int DeadPlayerAmmo(CBasePlayer *pPlayer) = 0;										// Do I drop ammo when the player dies? How much?

	// Teamplay stuff
	virtual const char *GetTeamID(CBaseEntity *pEntity) = 0;									// what team is this entity on?
	virtual int PlayerRelationship(CBasePlayer *pPlayer, CBaseEntity *pTarget) = 0;				// What is the player's relationship with this entity?
	virtual int GetTeamIndex(const char *pTeamName) { return -1; }
	virtual const char *GetIndexedTeamName(int teamIndex) { return ""; }
	virtual BOOL IsValidTeam(const char *pTeamName) { return TRUE; }
	virtual void ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib) {};
	virtual const char *SetDefaultPlayerTeam(CBasePlayer *pPlayer) { return ""; }

	// Sounds
	virtual BOOL PlayTextureSounds() { return TRUE; }

	// Monsters
	virtual BOOL FAllowMonsters() = 0;												// are monsters allowed

	// Immediately end a multiplayer game
	virtual void EndMultiplayerGame() {};

	// Stuff that is shared between client and server.
	virtual BOOL IsFreezePeriod() { return m_bFreezePeriod; }
	virtual void ServerDeactivate() {};
	virtual void CheckMapConditions() {};

	// inline function's
	inline bool IsGameOver() const { return m_bGameOver; }
	inline void SetGameOver() { m_bGameOver = true; }
	static float GetItemKillDelay();
	static float GetRadioTimeout();

public:
	BOOL m_bFreezePeriod;	// TRUE at beginning of round, set to FALSE when the period expires

	// custom
	char *m_GameDesc;
	bool m_bGameOver; // intermission or finale (deprecated name g_fGameOver)
};

#define GAMERULES_API_INTERFACE_VERSION "GAMERULES_API_INTERFACE_VERSION001"

// CHalfLifeRules - rules for the single player Half-Life game.
class CHalfLifeRules: public CGameRules
{
public:
	CHalfLifeRules();
	virtual ~CHalfLifeRules() {};

	virtual void Think();
	virtual BOOL IsAllowedToSpawn(CBaseEntity *pEntity);
	virtual BOOL FAllowFlashlight() { return TRUE; }

	virtual BOOL FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
	virtual BOOL GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon);

	// Functions to verify the single/multiplayer status of a game
	virtual BOOL IsMultiplayer();
	virtual BOOL IsDeathmatch();
	virtual BOOL IsCoOp();

	// Client connection/disconnection
	virtual BOOL ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
	virtual void InitHUD(CBasePlayer *pl);												// the client dll is ready for updating
	virtual void ClientDisconnected(edict_t *pClient);

	// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer);

	// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual void PlayerThink(CBasePlayer *pPlayer);
	virtual BOOL FPlayerCanRespawn(CBasePlayer *pPlayer);
	virtual float FlPlayerSpawnTime(CBasePlayer *pPlayer);
	virtual edict_t *GetPlayerSpawnSpot(CBasePlayer *pPlayer);

	virtual BOOL AllowAutoTargetCrosshair();

	// Client kills/scoring
	virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled);
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);
	virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);

	// Weapon retrieval
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);

	// Weapon spawn/respawn control
	virtual int WeaponShouldRespawn(CBasePlayerItem *pWeapon);
	virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon);
	virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon);
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon);

	// Item retrieval
	virtual BOOL CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem);

	// Item spawn/respawn control
	virtual int ItemShouldRespawn(CItem *pItem);
	virtual float FlItemRespawnTime(CItem *pItem);
	virtual Vector VecItemRespawnSpot(CItem *pItem);

	// Ammo retrieval
	virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, int iCount);

	// Ammo spawn/respawn control
	virtual int AmmoShouldRespawn(CBasePlayerAmmo *pAmmo);
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo);
	virtual Vector VecAmmoRespawnSpot(CBasePlayerAmmo *pAmmo);

	// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime();

	// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer *pPlayer);

	// What happens to a dead player's ammo
	virtual int DeadPlayerAmmo(CBasePlayer *pPlayer);

	// Teamplay stuff
	virtual const char *GetTeamID(CBaseEntity *pEntity) { return ""; };
	virtual int PlayerRelationship(CBasePlayer *pPlayer, CBaseEntity *pTarget);

	// Monsters
	virtual BOOL FAllowMonsters();
};

// CHalfLifeMultiplay - rules for the basic half life multiplayer competition
class CHalfLifeMultiplay: public CGameRules
{
public:
	CHalfLifeMultiplay();
	virtual ~CHalfLifeMultiplay() {};

	virtual void RefreshSkillData();
	virtual void Think();
	virtual BOOL IsAllowedToSpawn(CBaseEntity *pEntity);
	virtual BOOL FAllowFlashlight();

	virtual BOOL FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
	virtual BOOL GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon);

	virtual BOOL IsMultiplayer();
	virtual BOOL IsDeathmatch();
	virtual BOOL IsCoOp();

	// Client connection/disconnection
	// If ClientConnected returns FALSE, the connection is rejected and the user is provided the reason specified in szRejectReason
	// Only the client's name and remote address are provided to the dll for verification.
	virtual BOOL ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
	virtual void InitHUD(CBasePlayer *pl);													// the client dll is ready for updating
	virtual void ClientDisconnected(edict_t *pClient);
	virtual void UpdateGameMode(CBasePlayer *pPlayer);										// the client needs to be informed of the current game mode

	// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer);
	virtual BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker);

	// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual void PlayerThink(CBasePlayer *pPlayer);
	virtual BOOL FPlayerCanRespawn(CBasePlayer *pPlayer);
	virtual float FlPlayerSpawnTime(CBasePlayer *pPlayer);
	virtual edict_t *GetPlayerSpawnSpot(CBasePlayer *pPlayer);

	virtual BOOL AllowAutoTargetCrosshair();
	virtual BOOL ClientCommand_DeadOrAlive(CBasePlayer *pPlayer, const char *pcmd);
	virtual BOOL ClientCommand(CBasePlayer *pPlayer, const char *pcmd);
	virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer);

	// Client kills/scoring
	virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled);
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);
	virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);

	// Weapon retrieval
	virtual BOOL CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);								// The player is touching an CBasePlayerItem, do I give it to him?
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);

	// Weapon spawn/respawn control
	virtual int WeaponShouldRespawn(CBasePlayerItem *pWeapon);
	virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon);
	virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon);
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon);

	// Item retrieval
	virtual BOOL CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem);

	// Item spawn/respawn control
	virtual int ItemShouldRespawn(CItem *pItem);
	virtual float FlItemRespawnTime(CItem *pItem);
	virtual Vector VecItemRespawnSpot(CItem *pItem);

	// Ammo retrieval
	virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, int iCount);

	// Ammo spawn/respawn control
	virtual int AmmoShouldRespawn(CBasePlayerAmmo *pAmmo);
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo);
	virtual Vector VecAmmoRespawnSpot(CBasePlayerAmmo *pAmmo);

	// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime();
	virtual float FlHEVChargerRechargeTime();

	// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer *pPlayer);

	// What happens to a dead player's ammo
	virtual int DeadPlayerAmmo(CBasePlayer *pPlayer);

	// Teamplay stuff
	virtual const char *GetTeamID(CBaseEntity *pEntity) { return ""; }
	virtual int PlayerRelationship(CBasePlayer *pPlayer, CBaseEntity *pTarget);
	virtual void ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib);

	virtual BOOL PlayTextureSounds() { return FALSE; }

	// Monsters
	virtual BOOL FAllowMonsters();

	// Immediately end a multiplayer game
	virtual void EndMultiplayerGame() { GoToIntermission(); }
	virtual void ServerDeactivate();
	virtual void CheckMapConditions();

	// Recreate all the map entities from the map data (preserving their indices),
	// then remove everything else except the players.
	// Also get rid of all world decals.
	virtual void CleanUpMap();
	virtual void RestartRound();

	// check if the scenario has been won/lost
	virtual void CheckWinConditions();
	virtual void RemoveGuns();
	virtual void ChangeLevel();
	virtual void GoToIntermission();

public:
	void ServerActivate();
	void ReadMultiplayCvars();

	// Checks if it still needs players to start a round, or if it has enough players to start rounds.
	// Starts a round and returns true if there are enough players.
	bool NeededPlayersCheck();

	// Setup counts for m_iNumTerrorist, m_iNumCT, m_iNumSpawnableTerrorist, m_iNumSpawnableCT, etc.
	VFUNC void InitializePlayerCounts(int &NumAliveTerrorist, int &NumAliveCT, int &NumDeadTerrorist, int &NumDeadCT);

	// Check to see if the teams exterminated each other. Ends the round and returns true if so.
	bool TeamExterminationCheck(int NumAliveTerrorist, int NumAliveCT, int NumDeadTerrorist, int NumDeadCT);

	// for internal functions API
	void OnRoundFreezeEnd();
	bool OnRoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);

	bool RoundOver(float tmDelay);
	bool NeededPlayersCheck(float tmDelay);
	bool RestartRoundCheck(float tmDelay);

	// Team extermination
	bool Round_Cts(float tmDelay);
	bool Round_Ts(float tmDelay);
	bool Round_Draw(float tmDelay);

	// Check various conditions to end the map.
	bool CheckGameOver();
	bool CheckTimeLimit();
	bool CheckFragLimit();
	bool CheckMaxRounds();
	bool CheckWinLimit();

	void CheckFreezePeriodExpired();
	void CheckRoundTimeExpired();

	void CheckLevelInitialized();
	void CheckRestartRound();

	BOOL IsCareer();
	void QueueCareerRoundEndMenu(float tmDelay, int iWinStatus);
	void SetCareerMatchLimit(int minWins, int winDifference);
	bool IsInCareerRound();
	void CareerRestart();
	bool ShouldSkipShowMenu() const { return m_bSkipShowMenu; }
	void MarkShowMenuSkipped() { m_bSkipShowMenu = false; }
	bool ShouldSkipSpawn() const { return m_bSkipSpawn; }
	void MarkSpawnSkipped() { m_bSkipSpawn = false; }
	void PlayerJoinedTeam(CBasePlayer *pPlayer) { }
	float GetRoundRemainingTime() const { return m_iRoundTimeSecs - gpGlobals->time + m_fRoundStartTime; }
	float GetRoundRemainingTimeReal() const;
	float GetTimeLeft() const { return m_flTimeLimit - gpGlobals->time; }
	float GetRoundElapsedTime() const { return gpGlobals->time - m_fRoundStartTime; }
	float GetMapElapsedTime() const { return gpGlobals->time; }

	BOOL TeamFull(int team_id);
	BOOL TeamStacked(int newTeam_id, int curTeam_id);

	VFUNC void BalanceTeams();
	VFUNC void SwapAllPlayers();
	VFUNC void UpdateTeamScores();
	VFUNC void EndRoundMessage(const char *sentence, ScenarioEventEndRound event);
	VFUNC void SetAccountRules(RewardRules rules, int amount) { m_rgRewardAccountRules[rules] = static_cast<RewardAccount>(amount); }
	VFUNC RewardAccount GetAccountRules(RewardRules rules) const { return m_rgRewardAccountRules[rules]; }

	void DisplayMaps(CBasePlayer *pPlayer, int iVote);
	void ResetAllMapVotes();
	void ProcessMapVote(CBasePlayer *pPlayer, int iVote);

	// BOMB MAP FUNCTIONS
	VFUNC TeamName SelectDefaultTeam();

	bool IsMatchStarted() { return (m_flRestartRoundTime != 0.0f || m_fCareerRoundMenuTime != 0.0f || m_fCareerMatchMenuTime != 0.0f); }
	void SendMOTDToClient(edict_t *client);

	void TerminateRound(float tmDelay, int iWinStatus);
	float GetPlayerRespawnTime(CBasePlayer *pPlayer) const;
	float GetRoundRestartDelay() const;

	bool IsGameStarted() const { return m_bGameStarted; }

	// has a style of gameplay when aren't any teams
	bool IsFreeForAll() const;
	bool CanPlayerBuy(CBasePlayer *pPlayer) const;

	VFUNC bool HasRoundTimeExpired();

	// new functions of leader mod.
	VFUNC bool CanSkillBeUsed();
	void AssignCommander(CBasePlayer* pPlayer);
	void AssignGodfather(CBasePlayer* pPlayer);
	RoleTypes FindAvaliableRole(TeamName iTeam);
	CBasePlayer* RandomNonroleCharacter(TeamName iTeam);
	void CheckMenpower(TeamName iTeam);
	int IDamageMoney(CBasePlayer* pVictim, CBasePlayer* pAttacker, float flDamage);

private:
	void MarkLivingPlayersOnTeamAsNotReceivingMoneyNextRound(int iTeam);

public:
	static RewardAccount m_rgRewardAccountRules[RR_END];
	static RewardAccount m_rgRewardAccountRules_default[RR_END];

	CVoiceGameMgr m_VoiceGameMgr;
	float m_flRestartRoundTime;			// The global time when the round is supposed to end, if this is not 0 (deprecated name m_fTeamCount)
	float m_flCheckWinConditions;
	float m_fRoundStartTime;			// Time round has started (deprecated name m_fRoundCount)
	int m_iRoundTime;					// (From mp_roundtime) - How many seconds long this round is.
	int m_iRoundTimeSecs;
	int m_iIntroRoundTime;				// (From mp_freezetime) - How many seconds long the intro round (when players are frozen) is.
	float m_fRoundStartTimeReal;		// The global time when the intro round ends and the real one starts
										// wrote the original "m_flRoundTime" comment for this variable).
	int m_iAccountTerrorist;
	int m_iAccountCT;
	int m_iNumTerrorist;				// The number of terrorists on the team (this is generated at the end of a round)
	int m_iNumCT;						// The number of CTs on the team (this is generated at the end of a round)
	int m_iNumSpawnableTerrorist;
	int m_iNumSpawnableCT;
	int m_iSpawnPointCount_Terrorist;	// Number of Terrorist spawn points
	int m_iSpawnPointCount_CT;			// Number of CT spawn points
	int m_iRoundWinStatus;				// 1 == CT's won last round, 2 == Terrorists did, 3 == Draw, no winner

	short m_iNumCTWins;
	short m_iNumTerroristWins;

	bool m_bMapHasBuyZone;

	BOOL m_bMapHasCameras;
	int m_iLoserBonus;						// the amount of money the losing team gets. This scales up as they lose more rounds in a row
	int m_iNumConsecutiveCTLoses;			// the number of rounds the CTs have lost in a row.
	int m_iNumConsecutiveTerroristLoses;	// the number of rounds the Terrorists have lost in a row.

	float m_fMaxIdlePeriod;					// For the idle kick functionality. This is tha max amount of time that the player has to be idle before being kicked

	int m_iLimitTeams;
	bool m_bLevelInitialized;
	bool m_bRoundTerminating;
	bool m_bCompleteReset;					// Set to TRUE to have the scores reset next time round restarts
	bool m_bCTCantBuy;
	bool m_bTCantBuy;					// Who can and can't buy.
	int m_iTotalGunCount;
	int m_iTotalGrenadeCount;
	int m_iTotalArmourCount;
	int m_iUnBalancedRounds;			// keeps track of the # of consecutive rounds that have gone by where one team outnumbers the other team by more than 2
	int m_iNumEscapeRounds;				// keeps track of the # of consecutive rounds of escape played.. Teams will be swapped after 8 rounds
	int m_iMapVotes[MAX_VOTE_MAPS];
	int m_iLastPick;
	int m_iMaxMapTime;
	int m_iMaxRounds;
	int m_iTotalRoundsPlayed;
	int m_iMaxRoundsWon;
	int m_iStoredSpectValue;
	float m_flForceCameraValue;
	float m_flForceChaseCamValue;
	float m_flFadeToBlackValue;

	EntityHandle<CBasePlayer> m_rgpLeaders[4];
	hudtextparms_t m_TextParam_Notification;
	hudtextparms_t m_TextParam_Hud;
	int m_rgiMenpowers[4];
	bool m_rgbMenpowerBroadcast[4];
	

protected:
	float m_flIntermissionEndTime;
	float m_flIntermissionStartTime;
	BOOL m_iEndIntermissionButtonHit;
	float m_tmNextPeriodicThink;
	bool m_bGameStarted;				// TRUE = the game commencing when there is at least one CT and T, FALSE = scoring will not start until both teams have players (deprecated name m_bFirstConnected)
	bool m_bInCareerGame;
	float m_fCareerRoundMenuTime;
	int m_iCareerMatchWins;
	int m_iRoundWinDifference;
	float m_fCareerMatchMenuTime;
	bool m_bSkipSpawn;

	// custom
	bool m_bSkipShowMenu;
	bool m_bNeededPlayers;
	float m_flTimeLimit;
	float m_flGameStartTime;
	bool m_bTeamBalanced;
};

typedef struct mapcycle_item_s
{
	struct mapcycle_item_s *next;
	char mapname[MAX_MAPNAME_LENGHT];
	int minplayers;
	int maxplayers;
	char rulebuffer[MAX_RULE_BUFFER];

} mapcycle_item_t;

typedef struct mapcycle_s
{
	struct mapcycle_item_s *items;
	struct mapcycle_item_s *next_item;

} mapcycle_t;

class CCStrikeGameMgrHelper: public IVoiceGameMgrHelper
{
public:
	virtual bool CanPlayerHearPlayer(CBasePlayer *pListener, CBasePlayer *pSender);
	virtual void ResetCanHearPlayer(edict_t* pEdict);
	virtual void SetCanHearPlayer(CBasePlayer* pListener, CBasePlayer* pSender, bool bCanHear);
	virtual bool GetCanHearPlayer(CBasePlayer* pListener, CBasePlayer* pSender);

public:
	CBitVec<VOICE_MAX_PLAYERS> m_iCanHearMasks[VOICE_MAX_PLAYERS];
};

extern CGameRules DLLEXPORT *g_pGameRules;

CGameRules *InstallGameRules();

// Gets us at the CS game rules
inline CHalfLifeMultiplay *CSGameRules()
{
	return static_cast<CHalfLifeMultiplay *>(g_pGameRules);
}

inline void CHalfLifeMultiplay::TerminateRound(float tmDelay, int iWinStatus)
{
	m_iRoundWinStatus = iWinStatus;
	m_flRestartRoundTime = gpGlobals->time + tmDelay;
	m_bRoundTerminating = true;
}

inline float CHalfLifeMultiplay::GetPlayerRespawnTime(CBasePlayer* pPlayer) const
{
	float flLeaderHealthModifier = 1.0f;

	if (pPlayer->m_iTeam == CT && THE_COMMANDER.IsValid())
	{
		flLeaderHealthModifier = THE_COMMANDER->pev->health / THE_COMMANDER->pev->max_health;
	}
	else if (pPlayer->m_iTeam == TERRORIST && THE_GODFATHER.IsValid())
	{
		flLeaderHealthModifier = THE_GODFATHER->pev->health / THE_GODFATHER->pev->max_health;
	}

	return playerrespawn_time.value * flLeaderHealthModifier;
}

inline float CHalfLifeMultiplay::GetRoundRemainingTimeReal() const
{
	return m_iRoundTimeSecs - gpGlobals->time + m_fRoundStartTimeReal;
}

inline bool CHalfLifeMultiplay::IsFreeForAll() const
{
	if (freeforall.value != 0.0f)
		return true;

	return false;
}

inline float CHalfLifeMultiplay::GetRoundRestartDelay() const
{
	return round_restart_delay.value;
}

inline bool HasRoundInfinite(int flags = 0)
{
	if (round_infinite.string[0] == '1')
		return true;

	if (flags && (UTIL_ReadFlags(round_infinite.string) & flags))
		return true;

	return false;
}

inline float CGameRules::GetItemKillDelay()
{
	return item_staytime.value;
}

inline float CGameRules::GetRadioTimeout()
{
	return radio_timeout.value;
}

bool IsBotSpeaking();
void SV_Continue_f();
void SV_Tutor_Toggle_f();
void SV_Career_Restart_f();
void SV_Career_EndRound_f();
void SV_CareerAddTask_f();
void SV_CareerMatchLimit_f();
void Broadcast(const char *sentence);
char *GetTeam(int team);
void DestroyMapCycle(mapcycle_t *cycle);
int ReloadMapCycleFile(char *filename, mapcycle_t *cycle);
int CountPlayers();
void ExtractCommandString(char *s, char *szCommand);
int GetMapCount();

// SFX
#define SFX_GAME_START_1		"leadermode/start_game_01.wav"
#define SFX_GAME_START_2		"leadermode/start_game_02.wav"
#define SFX_MENPOWER_DEPLETED	"leadermode/unable_manpower_alert.wav"
#define SFX_TSD_GBD				"leadermode/money_in.wav"
#define SFX_TSD_SFD				"leadermode/infantry_rifle_cartridge_0%d.wav"
#define SFX_GAME_WON			"leadermode/brittania_mission_arrived.wav"
#define SFX_GAME_LOST			"leadermode/end_turn_brittania_04.wav"
#define MUSIC_GAME_WON			"sound/leadermode/Tally-ho.mp3"
#define MUSIC_GAME_LOST			"sound/leadermode/Warrior_s_Tomb.mp3"
#define SFX_VONC_PASSED			"leadermode/complete_focus_01.wav"
#define SFX_VONC_REJECTED		"leadermode/peaceconference01.wav"
#define SFX_REFUND_GUNS			"leadermode/money_out.wav"
