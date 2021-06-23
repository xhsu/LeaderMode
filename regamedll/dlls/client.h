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

#include "menus.h"

// custom enum
enum ChooseTeamMenuSlot
{
	MENU_SLOT_TEAM_UNDEFINED = -1,

	MENU_SLOT_TEAM_TERRORIST = 1,
	MENU_SLOT_TEAM_CT,

	MENU_SLOT_TEAM_RANDOM = 5,
	MENU_SLOT_TEAM_SPECT
};

#define CS_NUM_SKIN				4
#define CZ_NUM_SKIN				5

#define FIELD_ORIGIN0			0
#define FIELD_ORIGIN1			1
#define FIELD_ORIGIN2			2

#define FIELD_ANGLES0			3
#define FIELD_ANGLES1			4
#define FIELD_ANGLES2			5

#define CUSTOMFIELD_ORIGIN0		0
#define CUSTOMFIELD_ORIGIN1		1
#define CUSTOMFIELD_ORIGIN2		2

#define CUSTOMFIELD_ANGLES0		3
#define CUSTOMFIELD_ANGLES1		4
#define CUSTOMFIELD_ANGLES2		5

#define CUSTOMFIELD_SKIN		6
#define CUSTOMFIELD_SEQUENCE	7
#define CUSTOMFIELD_ANIMTIME	8

typedef struct
{
	float m_fTimeEnteredPVS;

} ENTITYPVSSTATUS;

const int MAX_ENTITIES = 1380;
struct PLAYERPVSSTATUS
{
	ENTITYPVSSTATUS m_Status[MAX_ENTITIES];
	int headnode;
	int num_leafs;
	short int leafnums[MAX_ENT_LEAFS];
};

struct entity_field_alias_t
{
	char name[32];
	int field;
};

// struct info (filled by engine)
typedef struct
{
	const char* name;
	const int		offset;
	const int		size;
} delta_field_t;

// one field
typedef struct delta_s
{
	const char* name;
	int		offset;		// in bytes
	int		size;		// used for bounds checking in DT_STRING
	int		flags;		// DT_INTEGER, DT_FLOAT etc
	float		multiplier;
	float		post_multiplier;	// for DEFINE_DELTA_POST
	int		bits;		// how many bits we send\receive
	qboolean		bInactive;	// unsetted by user request
} delta_t;

typedef void (*pfnDeltaEncode)(delta_t* pFields, const byte* from, const byte* to);

typedef struct
{
	const char* pName;
	const delta_field_t* pInfo;
	const int		maxFields;	// maximum number of fields in struct
	int		numFields;	// may be merged during initialization
	delta_t* pFields;

	// added these for custom entity encode
	int		customEncode;
	char		funcName[32];
	pfnDeltaEncode	userCallback;
	qboolean		bInitialized;
} delta_info_t;

C_DLLEXPORT int CountTeams();
C_DLLEXPORT int CountTeamPlayers(int iTeam);

extern bool g_bServerActive;
extern bool g_bClientPrintEnable;

extern unsigned short m_usResetDecals;
extern unsigned short g_iShadowSprite;

void LinkUserMessages();
void WriteSigonMessages();

int CMD_ARGC_();
const char *CMD_ARGV_(int i);
void set_suicide_frame(entvars_t *pev);
void BlinkAccount(CBasePlayer *pPlayer, int numBlinks = 2);
BOOL ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char *szRejectReason);
void ClientDisconnect(edict_t *pEntity);
void respawn(entvars_t *pev, BOOL fCopyCorpse = FALSE);
void ClientKill(edict_t *pEntity);
void ShowMenu(CBasePlayer* pPlayer, int bitsValidSlots, int nDisplayTime, const std::string& szText);
void ShowVGUIMenu(CBasePlayer* pPlayer, VGUIMenu MenuType);
void ListPlayers(CBasePlayer *current);
void ProcessKickVote(CBasePlayer *pVotingPlayer, CBasePlayer *pKickPlayer);
void CheckStartMoney();
void ClientPutInServer(edict_t *pEntity);
void Host_Say(edict_t *pEntity, BOOL teamonly);
CBaseWeapon *BuyWeapon(CBasePlayer *pPlayer, WeaponIdType weaponID);
void BuyEquipment(CBasePlayer *pPlayer, EquipmentIdType iSlot);
void HandleMenu_ChooseAppearance(CBasePlayer *pPlayer, int slot);
BOOL HandleMenu_ChooseTeam(CBasePlayer *pPlayer, int slot);
void Radio1(CBasePlayer *pPlayer, int slot);
void Radio2(CBasePlayer *pPlayer, int slot);
void Radio3(CBasePlayer *pPlayer, int slot);
bool BuyGunAmmo(CBasePlayer *pPlayer, CBaseWeapon *weapon, bool bBlinkMoney = true);
bool BuyAmmo(CBasePlayer *pPlayer, int nSlot, bool bBlinkMoney = true);
CBaseEntity *EntityFromUserID(int userID);
int CountPlayersInServer();
BOOL HandleRadioAliasCommands(CBasePlayer *pPlayer, const char *pszCommand);
void ClientCommand_(edict_t *pEntity);
void ClientUserInfoChanged(edict_t *pEntity, char *infobuffer);
void ServerDeactivate();
void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
void PlayerPreThink(edict_t *pEntity);
void PlayerPostThink(edict_t *pEntity);
void ParmsNewLevel();
void ParmsChangeLevel();
void StartFrame();
void EndFrame();
void ClientPrecache();
const char *GetGameDescription();
void SysEngine_Error(const char *error_string);
void PlayerCustomization(edict_t *pEntity, customization_t *pCust);
void SpectatorConnect(edict_t *pEntity);
void SpectatorDisconnect(edict_t *pEntity);
void SpectatorThink(edict_t *pEntity);
void SetupVisibility(edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas);
void ResetPlayerPVS(edict_t *client, int clientnum);
bool CheckPlayerPVSLeafChanged(edict_t *client, int clientnum);
void MarkEntityInPVS(int clientnum, int entitynum, float time, bool inpvs);
bool CheckEntityRecentlyInPVS(int clientnum, int entitynum, float currenttime);
BOOL AddToFullPack(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, BOOL player, unsigned char *pSet);
void CreateBaseline(int player, int eindex, struct entity_state_s *baseline, edict_t *entity, int playermodelindex, Vector player_mins, Vector player_maxs);
void Entity_FieldInit(struct delta_s *pFields);
void Entity_Encode(struct delta_s *pFields, const unsigned char *from, const unsigned char *to);
void Player_FieldInit(struct delta_s *pFields);
void Player_Encode(struct delta_s *pFields, const unsigned char *from, const unsigned char *to);
void Custom_Entity_FieldInit(delta_s *pFields);
void Custom_Encode(struct delta_s *pFields, const unsigned char *from, const unsigned char *to);
void RegisterEncoders();
int GetWeaponData(edict_t *pEdict, struct weapon_data_s *info);
void UpdateClientData(const edict_t *ent, int sendweapons, struct clientdata_s *cd);
void CmdStart(const edict_t *pEdict, const struct usercmd_s *cmd, unsigned int random_seed);
void CmdEnd(const edict_t *pEdict);
int ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
BOOL GetHullBounds(int hullnumber, float *mins, float *maxs);
void CreateInstancedBaselines();
int InconsistentFile(const edict_t *pEdict, const char *filename, char *disconnect_message);
int AllowLagCompensation();

inline const char *GetTeamName(int team)
{
	switch (team)
	{
	case CT:        return "CT";
	case TERRORIST: return "TERRORIST";
	case SPECTATOR: return "SPECTATOR";
	default:        return "UNASSIGNED";
	}
}

extern float g_flTrueServerFrameRate;

inline int gmsgWeapPickup = 0;
inline int gmsgHudText = 0;
inline int gmsgHudTextPro = 0;
inline int gmsgHudTextArgs = 0;
inline int gmsgShake = 0;
inline int gmsgFade = 0;
inline int gmsgFlashlight = 0;
inline int gmsgFlashBattery = 0;
inline int gmsgResetHUD = 0;
inline int gmsgInitHUD = 0;
inline int gmsgViewMode = 0;
inline int gmsgShowGameTitle = 0;
inline int gmsgHealth = 0;
inline int gmsgDamage = 0;
inline int gmsgBattery = 0;
inline int gmsgTrain = 0;
inline int gmsgLogo = 0;
inline int gmsgAmmoX = 0;
inline int gmsgDeathMsg = 0;
inline int gmsgScoreAttrib = 0;
inline int gmsgScoreInfo = 0;
inline int gmsgTeamInfo = 0;
inline int gmsgTeamScore = 0;
inline int gmsgGameMode = 0;
inline int gmsgMOTD = 0;
inline int gmsgServerName = 0;
inline int gmsgAmmoPickup = 0;
inline int gmsgItemPickup = 0;
inline int gmsgHideWeapon = 0;
inline int gmsgSayText = 0;
inline int gmsgTextMsg = 0;
inline int gmsgSetFOV = 0;
inline int gmsgShowMenu = 0;
inline int gmsgSendAudio = 0;
inline int gmsgRoundTime = 0;
inline int gmsgMoney = 0;
inline int gmsgBlinkAcct = 0;
inline int gmsgArmorType = 0;
inline int gmsgStatusValue = 0;
inline int gmsgStatusText = 0;
inline int gmsgStatusIcon = 0;
inline int gmsgBarTime = 0;
inline int gmsgReloadSound = 0;
inline int gmsgCrosshair = 0;
inline int gmsgNVGToggle = 0;
inline int gmsgRadar = 0;
inline int gmsgSpectator = 0;
inline int gmsgVGUIMenu = 0;
inline int gmsgShadowIdx = 0;
inline int gmsgAllowSpec = 0;
inline int gmsgGeigerRange = 0;
inline int gmsgSendCorpse = 0;
inline int gmsgHLTV = 0;
inline int gmsgSpecHealth = 0;
inline int gmsgForceCam = 0;
inline int gmsgReceiveW = 0;
inline int gmsgBotVoice = 0;
inline int gmsgBuyClose = 0;
inline int gmsgItemStatus = 0;
inline int gmsgLocation = 0;
inline int gmsgSpecHealth2 = 0;
inline int gmsgBarTime2 = 0;
inline int gmsgBotProgress = 0;
inline int gmsgBrass = 0;
inline int gmsgFog = 0;
inline int gmsgShowTimer = 0;
inline int gmsgRole = 0;
inline int gmsgRadarPoint = 0;
inline int gmsgRadarRP = 0;
inline int gmsgSetSlot = 0;
inline int gmsgShoot = 0;
inline int gmsgSteelSight = 0;
inline int gmsgEqpSelect = 0;
inline int gmsgSkillTimer = 0;
inline int gmsgSound = 0;
inline int gmsgSecVMDL = 0;
inline int gmsgEquipment = 0;
inline int gmsgManpower = 0;
inline int gmsgScheme = 0;
inline int gmsgNewRound = 0;
inline int gmsgGiveWpn = 0;
