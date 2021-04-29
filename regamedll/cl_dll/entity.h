/*

Created Date: 05 Mar 2020

*/

#pragma once

// MOVEME
enum
{
	MAX_PLAYERS = 32, // including the bomb
	MAX_TEAMS = 3,
	MAX_TEAM_NAME = 16,
	MAX_HOSTAGES = 24,
};

struct extra_player_info_t
{
	short		m_iKills;
	short		m_iDeaths;
	short		m_iTeam;
	Vector		m_vecOrigin;
	RoleTypes	m_iRoleType;
	int			m_iHealth;
	int			m_iAccount;

	float	m_flTimeNextRadarFlash;
	bool	m_bRadarFlashing;	// this flashing is the big yellowish block when the corresponding player is talking.
	float	m_iRadarFlashRemains;
	bool	m_bBeingTracking;	// assassin's skill & commander's skill.

	bool	m_bIsDead;
	bool	m_bIsGodfather;
	bool	m_bIsCommander;
	
	char	m_szLocationText[32];
};

extern int iOnTrain[MAX_PLAYERS];

extern extra_player_info_t	g_PlayerExtraInfo[MAX_PLAYERS]; // additional player info sent directly to the client.dll

extern double g_flEntUpdateAbsoluteTime;
extern double g_flEntUpdateFrameTime;

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

// for events.cpp
void EV_CS16Client_KillEveryRound(TEMPENTITY* te, float frametime, float current_time);
void EV_Smoke_FadeOut(struct tempent_s* te, float frametime, float currenttime);
void EV_FlameDeath(struct tempent_s* ent, float frametime, float currenttime);
