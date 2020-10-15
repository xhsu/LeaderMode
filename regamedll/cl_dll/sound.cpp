/*

Remastered Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

//#define FMOD_VECTOR Vector
#include "../external/FMOD/fmod.hpp"

FMOD::System* gFModSystem = nullptr;

static double g_flClientTime = 0;
static double g_flClientTimeDelta = 0;
static Vector g_vecPlayerLastPos = g_vecZero;

static FMOD::Sound* sound1;
static FMOD::Channel* channel1;

static std::unordered_map<std::string, FMOD::Sound*> g_mapSoundPrecache;

inline FMOD_VECTOR VecConverts(const Vector& v)
{
	return FMOD_VECTOR({ v.x * SND_DISTANCEFACTOR, v.z * SND_DISTANCEFACTOR, v.y * SND_DISTANCEFACTOR });
}

void Sound_Init()
{
	/*
		Create a System object and initialize.
	*/
	FMOD::System_Create(&gFModSystem);
	gFModSystem->init(100, FMOD_INIT_NORMAL, nullptr);

	/*
		Set the distance units. (meters/feet etc).
	*/
	gFModSystem->set3DSettings(1.0, SND_DISTANCEFACTOR, 1.0f);

	gFModSystem->createSound("drumloop.wav", FMOD_3D, 0, &sound1);
	sound1->set3DMinMaxDistance(0.5f * SND_DISTANCEFACTOR, 300.0f * SND_DISTANCEFACTOR);
	sound1->setMode(FMOD_LOOP_NORMAL);
	//gFModSystem->playSound(sound1, 0, false, &channel1);
}

void TestFMOD()
{
	FMOD_VECTOR zero = { 0, 0, 0 };

	bool paused;
	channel1->getPaused(&paused);

	gFModSystem->playSound(sound1, 0, true, &channel1);
	channel1->set3DAttributes(&zero, &zero);
	channel1->setPaused(!paused);
}

void PlaySound(const char* szSound)
{
	auto strKey = gEngfuncs.pfnGetGameDirectory() + std::string("/") + std::string(szSound);

	if (g_mapSoundPrecache.find(strKey) == g_mapSoundPrecache.end())
	{
		// precache no found, let's do it.
		gFModSystem->createSound(strKey.c_str(), FMOD_2D | FMOD_LOOP_OFF, 0, &g_mapSoundPrecache[strKey]);
	}

	gFModSystem->playSound(g_mapSoundPrecache[strKey], 0, false, &channel1);
}

void Sound_Think(double flTime)
{
	g_flClientTimeDelta = flTime - g_flClientTime;
	g_flClientTime = flTime;

	Vector vecFwd, vecRight, vecUp;
	gEngfuncs.pfnAngleVectors(gPseudoPlayer.pev->v_angle, vecFwd, vecRight, vecUp);

	// dv = ds / dt
	Vector vecVel = (gPseudoPlayer.pev->origin - g_vecPlayerLastPos) / g_flClientTimeDelta;
	g_vecPlayerLastPos = gPseudoPlayer.pev->origin;

	FMOD_VECTOR pos = VecConverts(gPseudoPlayer.pev->origin);
	FMOD_VECTOR forward = VecConverts(vecFwd);
	FMOD_VECTOR up = VecConverts(vecUp);
	FMOD_VECTOR vel = VecConverts(vecVel);

	gFModSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
	gFModSystem->update();
}

void Sound_Exit()
{
	gFModSystem->close();
	gFModSystem->release();
}
