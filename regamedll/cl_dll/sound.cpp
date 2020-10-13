/*

Remastered Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

//#define FMOD_VECTOR Vector
#include "../lib/FMOD/inc/fmod.hpp"

FMOD::System* gFModSystem = nullptr;

static double g_flClientTime = 0;
static double g_flClientTimeDelta = 0;

static FMOD::Sound* sound1;
static FMOD::Channel* channel3;

static std::unordered_map<std::string, FMOD::Sound*> g_mapSoundPrecache;

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

	gFModSystem->createSound("drumloop.wav", FMOD_2D|FMOD_LOOP_OFF, 0, &sound1);
	//sound1->set3DMinMaxDistance(0.5f * SND_DISTANCEFACTOR, 5000.0f * SND_DISTANCEFACTOR);
	//sound1->setMode(FMOD_LOOP_NORMAL);
	gFModSystem->playSound(sound1, 0, false, &channel3);
}

void TestFMOD()
{
	gFModSystem->playSound(sound1, 0, false, &channel3);
}

void PlaySound(const char* szSound)
{
	auto strKey = gEngfuncs.pfnGetGameDirectory() + std::string("/") + std::string(szSound);

	if (g_mapSoundPrecache.find(strKey) == g_mapSoundPrecache.end())
	{
		// precache no found, let's do it.
		gFModSystem->createSound(strKey.c_str(), FMOD_2D | FMOD_LOOP_OFF, 0, &g_mapSoundPrecache[strKey]);
	}

	gFModSystem->playSound(g_mapSoundPrecache[strKey], 0, false, &channel3);
}

void Sound_Think(double flTime)
{
	g_flClientTimeDelta = flTime - g_flClientTime;
	g_flClientTime = flTime;

	static FMOD_VECTOR lastpos = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR vel;

	// vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
	//vel.x = (listenerpos.x - lastpos.x) * (1000 / INTERFACE_UPDATETIME);
	//vel.y = (listenerpos.y - lastpos.y) * (1000 / INTERFACE_UPDATETIME);
	//vel.z = (listenerpos.z - lastpos.z) * (1000 / INTERFACE_UPDATETIME);

	// store pos for next time
	//lastpos = listenerpos;

	//gFModSystem->set3DListenerAttributes(0, &g_vecZero, g_vecZero, &forward, &up);
	gFModSystem->update();
}

void Sound_Exit()
{
	gFModSystem->close();
	gFModSystem->release();
}
