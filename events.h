/*

Created Date: 08 Mar 2020

*/

#pragma once

#define DECLARE_EVENT(x) void EV_##x( event_args_s *args)
#define HOOK_EVENT(x, y) gEngfuncs.pfnHookEvent( "events/" #x ".sc", EV_##y )

enum EV_SmokeTypes
{
	EV_BLACK_SMOKE = 1,
	EV_WALL_PUFF,
	EV_PISTOL_SMOKE,
	EV_RIFLE_SMOKE,
};

void EV_EjectBrass(float* origin, float* velocity, float rotation, int model, int soundtype, float life = 2.5f);
bool EV_IsLocal(int idx);

extern int g_iRShell;
extern int g_iPShell;
extern int g_iShotgunShell;
extern int g_iBlackSmoke;

void Events_Init(void);

// weapons
DECLARE_EVENT(FireM3);
DECLARE_EVENT(FireUSP);
DECLARE_EVENT(FireCM901);
