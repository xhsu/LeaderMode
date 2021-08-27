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

enum TacticalSchemes
{
	Scheme_UNASSIGNED = 0,	// disputation
	Doctrine_SuperiorFirepower,
	Doctrine_MassAssault,
	Doctrine_GrandBattleplan,
	Doctrine_MobileWarfare,

	SCHEMES_COUNT
};

extern std::array<const char*, SCHEMES_COUNT> g_rgszTacticalSchemeNames;
extern std::array<const char*, SCHEMES_COUNT> g_rgszTacticalSchemeDesc;

class CItem;

// CHalfLifeMultiplay - rules for the basic half life multiplayer competition
class CHalfLifeMultiplay
{
public:
	CHalfLifeMultiplay();
	virtual ~CHalfLifeMultiplay() {};

	virtual void Think();
	virtual BOOL IsAllowedToSpawn(CBaseEntity *pEntity);

	virtual BOOL FShouldSwitchWeapon(CBot* pPlayer, IWeapon* pWeapon);
	virtual BOOL GetNextBestWeapon(CBot* pPlayer, IWeapon* pCurrentWeapon);

	virtual BOOL IsMultiplayer();
	virtual BOOL IsDeathmatch();
	virtual BOOL IsCoOp();

	virtual const char* GetGameDescription();										// this is the game name that gets seen in the server browser

	// Client connection/disconnection
	// If ClientConnected returns FALSE, the connection is rejected and the user is provided the reason specified in szRejectReason
	// Only the client's name and remote address are provided to the dll for verification.
	virtual bool ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
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
	virtual bool CanHavePlayerItem(CBot* pPlayer, WeaponIdType iId, bool bPrintHint);	// The player is touching an CBasePlayerItem, do I give it to him?
	virtual void PlayerGotWeapon(CBot* pPlayer, IWeapon* pWeapon);

	// Ammo retrieval
	virtual bool CanHaveAmmo(CBot* pPlayer, AmmoIdType iId);		// can this player take more of this ammo?
	virtual void PlayerGotAmmo(CBasePlayer* pPlayer, AmmoIdType iId, int iCount) {}				// called each time a player picks up some ammo in the world

	// Item retrieval
	virtual bool CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem);

	// Item spawn/respawn control
	virtual int ItemShouldRespawn(CItem *pItem);
	virtual float FlItemRespawnTime(CItem *pItem);
	virtual Vector VecItemRespawnSpot(CItem *pItem);

	// Equipment retrieval
	virtual bool CanHaveEquipment(CBasePlayer* pPlayer, EquipmentIdType iId);
	virtual void PlayerGotEquipment(CBasePlayer* pPlayer, EquipmentIdType iId);

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
	virtual bool IsFreezePeriod() { return m_bFreezePeriod; }
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
	void InitializePlayerCounts(int &NumAliveTerrorist, int &NumAliveCT, int &NumDeadTerrorist, int &NumDeadCT);

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

	void BalanceTeams();
	void SwapAllPlayers();
	void UpdateTeamScores();
	void EndRoundMessage(const char *sentence, ScenarioEventEndRound event);
	void SetAccountRules(RewardRules rules, int amount) { m_rgRewardAccountRules[rules] = static_cast<RewardAccount>(amount); }
	RewardAccount GetAccountRules(RewardRules rules) const { return m_rgRewardAccountRules[rules]; }

	void DisplayMaps(CBasePlayer *pPlayer, int iVote);
	void ResetAllMapVotes();
	void ProcessMapVote(CBasePlayer *pPlayer, int iVote);

	// BOMB MAP FUNCTIONS
	TeamName SelectDefaultTeam();

	bool IsMatchStarted() { return !!(m_flRestartRoundTime != 0.0f); }
	void SendMOTDToClient(edict_t *client);

	void TerminateRound(float tmDelay, int iWinStatus);
	float GetPlayerRespawnTime(CBasePlayer *pPlayer) const;
	float GetRoundRestartDelay() const;

	bool IsGameStarted() const { return m_bGameStarted; }

	// has a style of gameplay when aren't any teams
	bool IsFreeForAll() const;
	bool CanPlayerBuy(CBasePlayer *pPlayer) const;

	bool HasRoundTimeExpired();

	// inline function's
	inline bool IsGameOver() const { return m_bGameOver; }
	inline void SetGameOver() { m_bGameOver = true; }

	// new functions of leader mod.
	bool CanSkillBeUsed();
	void AssignCommander(CBasePlayer* pPlayer);
	void AssignGodfather(CBasePlayer* pPlayer);
	RoleTypes FindAvaliableRole(TeamName iTeam);
	CBasePlayer* RandomNonroleCharacter(TeamName iTeam);
	void CheckMenpower(TeamName iTeam);
	int IDamageMoney(CBasePlayer* pVictim, CBasePlayer* pAttacker, float flDamage);
	bool HasRoleOccupied(RoleTypes iRole, TeamName iTeam);
	TacticalSchemes CalcTSVoteResult(TeamName iTeam);
	void GetTSVoteDetail(TeamName iTeam, int* rgiBallotBoxes);
	void GiveDefaultItems(CBasePlayer* pPlayer);
	float PlayerMaxArmour(CBasePlayer* pPlayer);
	EquipmentIdType SelectProperGrenade(CBasePlayer* pPlayer);

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

	EntityHandle<CBasePlayer> m_rgpCharacters[ROLE_COUNT];
	hudtextparms_t m_TextParam_Notification;
	hudtextparms_t m_TextParam_Hud;
	std::array<int, 4U> m_rgiManpowers;
	std::array<int, 4U> m_rgiClientKnownManpower;
	std::array<bool, 4U> m_rgbMenpowerBroadcast;
	std::array<TacticalSchemes, 4U>  m_rgTeamTacticalScheme;
	float m_flNextTSBallotBoxesOpen;

protected:
	float m_flIntermissionEndTime;
	float m_flIntermissionStartTime;
	BOOL m_iEndIntermissionButtonHit;
	float m_tmNextPeriodicThink;
	bool m_bGameStarted;				// TRUE = the game commencing when there is at least one CT and T, FALSE = scoring will not start until both teams have players (deprecated name m_bFirstConnected)
	int m_iRoundWinDifference;
	bool m_bSkipSpawn;

	// custom
	bool m_bSkipShowMenu;
	bool m_bNeededPlayers;
	float m_flTimeLimit;
	float m_flGameStartTime;
	bool m_bTeamBalanced;
	BOOL m_bFreezePeriod;	// TRUE at beginning of round, set to FALSE when the period expires
	char* m_GameDesc;
	bool m_bGameOver; // intermission or finale (deprecated name g_fGameOver)
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

extern CHalfLifeMultiplay DLLEXPORT* g_pGameRules;

CHalfLifeMultiplay* InstallGameRules();

// Gets us at the CS game rules
inline CHalfLifeMultiplay *CSGameRules()
{
	return g_pGameRules;
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

	if (m_rgTeamTacticalScheme[pPlayer->m_iTeam] == Doctrine_MassAssault)
		flLeaderHealthModifier = 0.1f;

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

bool IsBotSpeaking();
void Broadcast(const char *sentence);
const char *GetTeam(int team);
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
#define MUSIC_GAME_WON			"sound/music/Tally-ho.mp3"
#define MUSIC_GAME_LOST			"sound/music/Warrior_s_Tomb.mp3"
#define SFX_VONC_PASSED			"leadermode/complete_focus_01.wav"
#define SFX_VONC_REJECTED		"leadermode/peaceconference01.wav"
#define SFX_REFUND_GUNS			"leadermode/money_out.wav"
