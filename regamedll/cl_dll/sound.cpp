/*

Remastered Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

//#define FMOD_VECTOR Vector
#include "../external/FMOD/fmod.hpp"

#define FMOD_DEFAULT_IN_GOLDSRC		(FMOD_LOOP_OFF | FMOD_3D | FMOD_3D_WORLDRELATIVE | FMOD_3D_LINEARROLLOFF)
#define FMOD_MAX_CHANNEL_GOLDSRC	256

FMOD::System* gFModSystem = nullptr;

static double g_flClientTime = 0;
static double g_flClientTimeDelta = 0;
static Vector g_vecPlayerLastPos = g_vecZero;

static FMOD::Sound* sound1;
static FMOD::Channel* g_phLocal2DChannel;

static constexpr FMOD_VECTOR	g_fmodvecZero = { 0.0f, 0.0f, 0.0f };

static std::unordered_map<std::string, FMOD::Sound*> g_mapSoundPrecache;

namespace gFMODChannelManager
{
	struct
	{
		FMOD::Channel* m_pChannel;
		double m_flNextAvailable;

	} m_Channels[FMOD_MAX_CHANNEL_GOLDSRC];

	FMOD::Channel** Allocate()
	{
		for (short i = 0; i < FMOD_MAX_CHANNEL_GOLDSRC; i++)
		{
			if (m_Channels[i].m_flNextAvailable > g_flClientTime)
				continue;

			m_Channels[i].m_flNextAvailable = g_flClientTime + 3.5f;	// since only gun fire SFX supported by FMOD... it quite enough.
			return &m_Channels[i].m_pChannel;
		}

		return nullptr;
	}
}

inline FMOD_VECTOR VecConverts(const Vector& v, bool bScale = false)
{
	if (bScale)
		return FMOD_VECTOR({ v.x * SND_DISTANCEFACTOR, v.z * SND_DISTANCEFACTOR, v.y * SND_DISTANCEFACTOR });

	return FMOD_VECTOR({ v.x, v.z, v.y });
}

void Sound_Init()
{
	/*
		Create a System object and initialize.
	*/
	FMOD::System_Create(&gFModSystem);
	gFModSystem->init(FMOD_MAX_CHANNEL_GOLDSRC, FMOD_INIT_NORMAL, nullptr);
	Q_memset(&gFMODChannelManager::m_Channels, NULL, sizeof(gFMODChannelManager::m_Channels));

	/*
		Set the distance units. (meters/feet etc).
	*/
	gFModSystem->set3DSettings(1.0, SND_DISTANCEFACTOR, 1.0f);

	gFModSystem->createSound("drumloop.wav", FMOD_LOOP_NORMAL | FMOD_3D | FMOD_3D_WORLDRELATIVE | FMOD_3D_LINEARSQUAREROLLOFF, 0, &sound1);
	sound1->set3DMinMaxDistance(0.1f * SND_DISTANCEFACTOR, 200.0f * SND_DISTANCEFACTOR);
	//gFModSystem->playSound(sound1, 0, false, &g_phLocal2DChannel);
}

void TestFMOD()
{
	bool paused;
	g_phLocal2DChannel->getPaused(&paused);

	gFModSystem->playSound(sound1, 0, true, &g_phLocal2DChannel);
	g_phLocal2DChannel->set3DAttributes(&g_fmodvecZero, &g_fmodvecZero);
	g_phLocal2DChannel->setPaused(!paused);
}

void PlaySound(const char* szSound)
{
	auto strKey = gEngfuncs.pfnGetGameDirectory() + std::string("/sound/") + std::string(szSound);

	if (g_mapSoundPrecache.find(strKey) == g_mapSoundPrecache.end())
	{
		// precache no found, let's do it.
		gFModSystem->createSound(strKey.c_str(), FMOD_2D | FMOD_LOOP_OFF, 0, &g_mapSoundPrecache[strKey]);
	}

	gFModSystem->playSound(g_mapSoundPrecache[strKey], nullptr, false, &g_phLocal2DChannel);
}

void Play3DSound(const char* szSound, float flMinDist, float flMaxDist, const Vector& vecOrigin)
{
	auto strKey = gEngfuncs.pfnGetGameDirectory() + std::string("/sound/") + std::string(szSound);

	if (g_mapSoundPrecache.find(strKey) == g_mapSoundPrecache.end())
	{
		// precache no found, let's do it.
		gFModSystem->createSound(strKey.c_str(), FMOD_DEFAULT_IN_GOLDSRC, 0, &g_mapSoundPrecache[strKey]);
	}

	g_mapSoundPrecache[strKey]->set3DMinMaxDistance(flMinDist * SND_DISTANCEFACTOR, flMaxDist * SND_DISTANCEFACTOR);

	auto ppChannel = gFMODChannelManager::Allocate();
	FMOD_VECTOR	pos = VecConverts(vecOrigin);
	(*ppChannel)->set3DAttributes(&pos, &g_fmodvecZero);
	gFModSystem->playSound(g_mapSoundPrecache[strKey], nullptr, false, ppChannel);

	// since this function is only used for gun sound playing, let's just randomize it here.
	// original formula: 94 + gEngfuncs.pfnRandomLong(0, 0xf)
	(*ppChannel)->setPitch(0.94f + RANDOM_FLOAT(0.0f, 0.16f));
}

void Sound_Think(double flTime)
{
	g_flClientTimeDelta = flTime - g_flClientTime;
	g_flClientTime = flTime;

	Vector vecFwd, vecRight, vecUp;
	gEngfuncs.pfnAngleVectors(gPseudoPlayer.pev->v_angle, vecFwd, vecRight, vecUp);

	FMOD_VECTOR pos = VecConverts(gPseudoPlayer.GetGunPosition(), true);
	FMOD_VECTOR forward = VecConverts(vecFwd);
	FMOD_VECTOR up = VecConverts(vecUp);

	// this velocity stuff is only for making Doppler effect.
	// [reference] https://en.wikipedia.org/wiki/Doppler_effect
	gFModSystem->set3DListenerAttributes(0, &pos, nullptr, &forward, &up);
	gFModSystem->update();
}

void Sound_Exit()
{
	gFModSystem->close();
	gFModSystem->release();
}
