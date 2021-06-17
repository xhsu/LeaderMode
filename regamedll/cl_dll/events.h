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

extern void EV_MuzzleFlash(void);
extern void EV_HLDM_CreateSmoke(float* origin, float* dir, int speed, float scale, int r, int g, int b, EV_SmokeTypes iSmokeType, float* base_velocity, bool bWind, int framerate);
extern void EV_EjectBrass(float* origin, float* velocity, float rotation, int model, int soundtype, float flAngularVel = 4.0f, float life = 15.0f);
extern bool EV_IsLocal(int idx);
extern void EV_HLDM_FireBullets(int idx, Vector& forward, Vector& right, Vector& up, int cShots, Vector& vecSrc, Vector& vecDirShooting, const Vector2D& vecSpread, float flDistance, AmmoIdType iBulletType, int iPenetration);
extern void EV_HLDM_FireBullets(int idx, Vector& forward, Vector& right, Vector& up, int cShots, Vector& vecSrc, Vector& vecDirShooting, const Vector2D& vecSpread, float flDistance, AmmoIdType iBulletType, int iPenetration, int shared_rand);
extern void EV_GetDefaultShellInfo(int idx, bool ducking, const Vector& origin, const Vector& velocity, Vector& ShellVelocity, Vector& ShellOrigin, const Vector& forward, const Vector& right, const Vector& up, float forwardScale, float upScale, float rightScale);
extern Vector EV_GetGunPosition(int idx, bool ducking, Vector origin);
extern void EV_PlayGunFire(const Vector& src, const char* sample, float attn, float vol, int iPitch);
extern void EV_PlayGunFire2(const Vector& src, const char* sample, float rad, float vol, int iPitch);
extern void EV_PlayGunFire2(const Vector& src, const char* sample, float rad, float vol = 1.0f);

extern int g_iRShell;
extern int g_iPShell;
extern int g_iShotgunShell;
extern int g_iBlackSmoke;

void Events_Init(void);

// weapons
DECLARE_EVENT(FireAK47);
DECLARE_EVENT(FireAnaconda);
DECLARE_EVENT(FireAWP);
DECLARE_EVENT(FireDEagle);
DECLARE_EVENT(Fire57);
DECLARE_EVENT(Fireglock18);
DECLARE_EVENT(FireKSG12);
DECLARE_EVENT(FireM1014);
DECLARE_EVENT(FireM4A1);
DECLARE_EVENT(FireM45A1);
DECLARE_EVENT(FireMK46);
DECLARE_EVENT(FireMP7A1);
DECLARE_EVENT(FirePSG1);
DECLARE_EVENT(FireSCARH);
DECLARE_EVENT(FireSVD);
DECLARE_EVENT(FireUMP45);
//DECLARE_EVENT(FireUSP);
DECLARE_EVENT(FireXM8);
