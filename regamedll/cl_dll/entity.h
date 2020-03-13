/*

Created Date: 05 Mar 2020

*/

#pragma once

// MOVEME
enum
{
	MAX_PLAYERS = 33, // including the bomb
	MAX_TEAMS = 3,
	MAX_TEAM_NAME = 16,
	MAX_HOSTAGES = 24,
};

struct extra_player_info_t
{
	short frags;
	short deaths;
	short team_id;
	bool has_c4;
	bool vip;
	Vector origin;
	float radarflash;
	float radarflashon;
	float radarflashes;
	short playerclass;
	short teamnumber;
	char teamname[16];
	bool dead;
	bool showhealth;
	int health;
	char location[32];

	// LUNA: my add.
	int m_iAccount;
};

extern int iOnTrain[MAX_PLAYERS];

// MOVEME
extern int g_iUser1;
extern int g_iUser2;
extern int g_iUser3;
extern int g_iTeamNumber;
extern int g_iPlayerClass;
extern int g_iPlayerFlags;
extern Vector g_vPlayerVelocity;
extern float g_flPlayerSpeed;
extern int g_iWeaponFlags;

extern extra_player_info_t	g_PlayerExtraInfo[MAX_PLAYERS + 1]; // additional player info sent directly to the client dll

int HUD_AddEntity2(int iType, cl_entity_s* pEntity, const char* szModelName);
void HUD_TxferLocalOverrides2(entity_state_s* pState, const clientdata_s* pClient);
void HUD_ProcessPlayerState2(entity_state_s* pDestination, const entity_state_s* pSource);
void HUD_TxferPredictionData2(entity_state_s* ps, const entity_state_s* pps, clientdata_s* pcd, const clientdata_s* ppcd, weapon_data_s* wd, const weapon_data_s* pwd);
void HUD_CreateEntities2(void);
void HUD_StudioEvent2(const mstudioevent_s* pEvent, const cl_entity_s* pEntity);
void HUD_TempEntUpdate2 (
	double flFrameTime,   // Simulation time
	double flClientTime, // Absolute time on client
	double flClientGravity,  // True gravity on client
	TEMPENTITY** ppTempEntFree,   // List of freed temporary ents
	TEMPENTITY** ppTempEntActive, // List 
	int		(*Callback_AddVisibleEntity)(cl_entity_t* pEntity),
	void	(*Callback_TempEntPlaySound)(TEMPENTITY* pTemp, float damp));
cl_entity_t* HUD_GetUserEntity2(int index);
