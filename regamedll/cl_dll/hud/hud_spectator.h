/*

Created Date: 10 Mar 2020

*/

#pragma once

#define INSET_OFF 0
#define INSET_CHASE_FREE 1
#define INSET_IN_EYE 2
#define INSET_MAP_FREE 3
#define INSET_MAP_CHASE 4

#define MAX_SPEC_HUD_MESSAGES 8
#define MAX_OVERVIEW_ENTITIES 128
#define MAX_CAM_WAYPOINTS 32

#define OVERVIEW_TILE_SIZE 128
#define OVERVIEW_MAX_LAYERS 1

typedef struct cameraWayPoint_s
{
	float time;
	Vector position;
	Vector angle;
	float fov;
	int flags;
}
cameraWayPoint_t;

typedef struct overviewInfo_s
{
	char map[64];
	vec3_t origin;
	float zoom;
	int layers;
	float layersHeights[OVERVIEW_MAX_LAYERS];
	char layersImages[OVERVIEW_MAX_LAYERS][255];
	qboolean rotated;

	int insetWindowX;
	int insetWindowY;
	int insetWindowHeight;
	int insetWindowWidth;
}
overviewInfo_t;

typedef struct overviewEntity_s
{
	hSprite hSprite;
	struct cl_entity_s* entity;
	double killTime;
}
overviewEntity_t;

class CHudSpectator : public CBaseHUDElement
{
public:
	void Reset(void);
	int ToggleInset(bool allowOff);
	void CheckSettings(void);
	void InitHUDData(void);
	bool AddOverviewEntityToList(hSprite sprite, cl_entity_t* ent, double killTime);
	void DeathMessage(int victim);
	bool AddOverviewEntity(int type, cl_entity_s* ent, const char* modelname);
	void CheckOverviewEntities(void);
	void DrawOverview(void);
	void DrawOverviewEntities(void);
	void GetMapPosition(float* returnvec);
	void DrawOverviewLayer(void);
	void LoadMapSprites(void);
	bool ParseOverviewFile(void);
	bool IsActivePlayer(cl_entity_t* ent);
	void SetModes(int iMainMode, int iInsetMode);
	void HandleButtonsDown(int ButtonPressed);
	void HandleButtonsUp(int ButtonPressed);
	void FindNextPlayer(bool bReverse);
	void FindPlayer(const char* name);
	bool DirectorMessage(int iSize, void* pbuf);
	void SetSpectatorStartPosition(void);
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);

	void AddWaypoint(float time, Vector pos, Vector angle, float fov, int flags);
	void SetCameraView(Vector pos, Vector angle, float fov);
	float GetFOV(void);
	bool GetDirectorCamera(Vector& position, Vector& angle);
	void SetWayInterpolation(cameraWayPoint_t* prev, cameraWayPoint_t* start, cameraWayPoint_t* end, cameraWayPoint_t* next);

public:
	int m_iDrawCycle;
	client_textmessage_t m_HUDMessages[MAX_SPEC_HUD_MESSAGES];
	char m_HUDMessageText[MAX_SPEC_HUD_MESSAGES][128];
	int m_lastHudMessage;
	overviewInfo_t m_OverviewData;
	overviewEntity_t m_OverviewEntities[MAX_OVERVIEW_ENTITIES];
	int m_iObserverFlags;
	int m_iSpectatorNumber;

	float m_mapZoom;
	Vector m_mapOrigin;
	cvar_t* m_drawnames;
	cvar_t* m_drawcone;
	cvar_t* m_drawstatus;
	cvar_t* m_autoDirector;
	cvar_t* m_pip;
	cvar_t* m_mode;
	qboolean m_chatEnabled;
	qboolean m_IsInterpolating;
	int m_ChaseEntity;
	int m_WayPoint;
	int m_NumWayPoints;
	Vector m_cameraOrigin;
	Vector m_cameraAngles;
	CInterpolation m_WayInterpolation;

private:
	Vector m_vPlayerPos[MAX_PLAYERS];
	hSprite m_hsprPlayerBlue;
	hSprite m_hsprPlayerRed;
	hSprite m_hsprPlayer;
	hSprite m_hsprPlayerVIP;
	hSprite m_hsprPlayerC4;
	hSprite m_hsprCamera;
	hSprite m_hsprPlayerDead;
	hSprite m_hsprViewcone;
	hSprite m_hsprUnkownMap;
	hSprite m_hsprBeam;
	hSprite m_hsprBomb;
	hSprite m_hsprHostage;
	hSprite m_hsprBackpack;

	model_s* m_MapSprite;
	float m_flNextObserverInput;
	float m_FOV;
	float m_zoomDelta;
	float m_moveDelta;
	int m_lastPrimaryObject;
	int m_lastSecondaryObject;
	int m_lastAutoDirector;
	cameraWayPoint_t m_CamPath[MAX_CAM_WAYPOINTS];
};
