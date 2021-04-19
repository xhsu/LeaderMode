/*

Created Date: Mar 27 2020
Remastered Date: Oct 15 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Audio engineer - Qian Ge

*/

#include "precompiled.h"

//#define FMOD_VECTOR Vector
#include "../external/FMOD/fmod.hpp"

#define FMOD_DEFAULT_IN_GOLDSRC		(FMOD_LOOP_OFF | FMOD_3D | FMOD_3D_WORLDRELATIVE | FMOD_3D_LINEARROLLOFF)
#define FMOD_MAX_CHANNEL_GOLDSRC	4092	// just... max it out, according to user manual. Reserve an additional channel for all other 2D first-personal SFX.

FMOD::System* gFModSystem = nullptr;

double g_flClientTime = 0;
double g_flClientTimeDelta = 0;

static FMOD::Channel* g_phLocal2DChannel;

static constexpr FMOD_VECTOR	g_fmodvecZero = { 0.0f, 0.0f, 0.0f };

static std::unordered_map<std::string, FMOD::Sound*> g_mapSoundPrecache;

namespace gFMODChannelManager
{
	struct channel_info_s
	{
		FMOD::Channel* m_pChannel;
		double m_flNextAvailable;
	};
	std::array<channel_info_s, FMOD_MAX_CHANNEL_GOLDSRC> m_Channels;

	FMOD::Channel** Allocate(float flCooldown)
	{
		size_t iMin = 0;
		double flMinTime = 99999999.0;

		for (size_t i = 0; i < FMOD_MAX_CHANNEL_GOLDSRC; i++)
		{
			if (m_Channels[i].m_flNextAvailable < g_flClientTime)
			{
				iMin = i;
				break;
			}

			// record the time by the way.
			if (m_Channels[i].m_flNextAvailable < flMinTime)
			{
				iMin = i;
				flMinTime = m_Channels[i].m_flNextAvailable;
			}
		}

		// just occuiped a furthest unused channel.
		m_Channels[iMin].m_flNextAvailable = g_flClientTime + flCooldown;
		return &m_Channels[iMin].m_pChannel;
	}

	FMOD::Channel** PermanentAllocate(size_t* piIndex)
	{
		size_t iMin = 0, i = 0;
		double flMinTime = 99999999.0;

		for (i = 0; i < FMOD_MAX_CHANNEL_GOLDSRC; i++)
		{
			if (m_Channels[i].m_flNextAvailable < g_flClientTime)
			{
				iMin = i;
				break;
			}

			// record the time by the way.
			if (m_Channels[i].m_flNextAvailable < flMinTime)
			{
				iMin = i;
				flMinTime = m_Channels[i].m_flNextAvailable;
			}
		}

		// just occuiped a furthest unused channel.
		m_Channels[iMin].m_flNextAvailable = g_flClientTime + 31622400.0;

		if (piIndex)
			*piIndex = iMin;

		return &m_Channels[iMin].m_pChannel;
	}

	void Free(size_t index)
	{
		m_Channels[index].m_flNextAvailable = g_flClientTime;
		m_Channels[index].m_pChannel->stop();
	}
}

inline FMOD_VECTOR VecConverts(const Vector& v, bool bScale = false)
{
	// this function has some problem.
	// you cannot place a calculating formula in the first parameter.

	if (bScale)
		return FMOD_VECTOR({ v.x / SND_DISTANCEFACTOR, v.z / SND_DISTANCEFACTOR, v.y / SND_DISTANCEFACTOR });	// FMOD has a inversed Y/Z axis.

	return FMOD_VECTOR({ v.x, v.z, v.y });
}

#ifdef _DEBUG
void Test_FMOD(void)
{
	PlaySound("weapons/SCARH/mk17_shoot-s.wav");
}

void CountChannels(void)
{
	int iResult = 0;

	for (auto& info : gFMODChannelManager::m_Channels)
	{
		if (info.m_flNextAvailable < g_flClientTime)
		{
			iResult++;
		}
	}

	std::string text = "Avaliable channels: " + std::to_string(iResult) + "\n";
	gEngfuncs.pfnConsolePrint(text.c_str());
}
#endif

void Sound_Init()
{
#ifdef _DEBUG
	gEngfuncs.pfnAddCommand("testfmod", &Test_FMOD);
	gEngfuncs.pfnAddCommand("countchannels", &CountChannels);
#endif

	/*
		Create a System object and initialize.
	*/
	FMOD::System_Create(&gFModSystem);
	gFModSystem->init(FMOD_MAX_CHANNEL_GOLDSRC + 1, FMOD_INIT_NORMAL, nullptr);	// plus an additional 2D first-personal SFX channel.
	Q_memset(&gFMODChannelManager::m_Channels, NULL, sizeof(gFMODChannelManager::m_Channels));

	/*
		Set the distance units. (meters/feet etc).
	*/
	gFModSystem->set3DSettings(1.0, SND_DISTANCEFACTOR, 1.0f);
}

FMOD::Sound* PrecacheSound(std::string szSample, FMOD_MODE iMode)
{
	static char szFullPath[256];

	// Add the "sound/" at the start.
	if (strncmp(szSample.c_str(), "sound/", 6U))
	{
		std::string szLocalPath = "sound/" + szSample;
		FILE_SYSTEM->GetLocalPath(szLocalPath.c_str(), szFullPath, charsmax(szFullPath));
	}
	else
		FILE_SYSTEM->GetLocalPath(szSample.c_str(), szFullPath, charsmax(szFullPath));

	if (g_mapSoundPrecache.find(szFullPath) == g_mapSoundPrecache.end())
	{
		// precache no found, let's do it.
		gFModSystem->createSound(szFullPath, iMode, 0, &g_mapSoundPrecache[szFullPath]);
	}
	else
	{
		// Switch to current mode. This is because that it could be played under a different mode.
		g_mapSoundPrecache[szFullPath]->setMode(iMode);
	}

	return g_mapSoundPrecache[szFullPath];
}

void PlaySound(const char* szSound, float flVolume, int iPitch)
{
	FMOD_TIMEUNIT iLength = 3500;
	auto pSound = PrecacheSound(szSound, FMOD_2D | FMOD_LOOP_OFF);
	pSound->getLength(&iLength, FMOD_TIMEUNIT_MS);

	auto ppChannel = gFMODChannelManager::Allocate(float(iLength) / 1000.0f);
	gFModSystem->playSound(pSound, nullptr, false, ppChannel);
	(*ppChannel)->setVolume(flVolume);
	(*ppChannel)->setVolumeRamp(false);
	(*ppChannel)->setPitch(float(iPitch) / 100.0f);
	(*ppChannel)->setPaused(false);
}

void Play3DSound(const char* szSound, float flMinDist, float flMaxDist, const Vector& vecOrigin, float flVolume, int iPitch)
{
	FMOD_TIMEUNIT iLength = 3500;
	FMOD_VECTOR	pos = VecConverts(vecOrigin, true);
	auto pSound = PrecacheSound(szSound, FMOD_DEFAULT_IN_GOLDSRC);

	pSound->set3DMinMaxDistance(flMinDist / SND_DISTANCEFACTOR, flMaxDist / SND_DISTANCEFACTOR);
	pSound->getLength(&iLength, FMOD_TIMEUNIT_MS);

	auto ppChannel = gFMODChannelManager::Allocate(float(iLength) / 1000.0f);
	gFModSystem->playSound(pSound, nullptr, true, ppChannel);	// Have to activate the channel first. Otherwise it will be a nullptr.
	(*ppChannel)->set3DAttributes(&pos, &g_fmodvecZero);
	(*ppChannel)->setVolume(flVolume);
	(*ppChannel)->setVolumeRamp(false);
	(*ppChannel)->setPitch(float(iPitch) / 100.0f);	// original formula for most CS weapons: 94 + gEngfuncs.pfnRandomLong(0, 0xf)
	(*ppChannel)->setPaused(false);
}

void Sound_Think(double flDeltaTime)
{
	Vector vecFwd, vecRight, vecUp;
	gEngfuncs.pfnAngleVectors(gPseudoPlayer.pev->v_angle, vecFwd, vecRight, vecUp);

	FMOD_VECTOR pos = VecConverts(gPseudoPlayer.GetGunPosition(), true);
	FMOD_VECTOR vel = VecConverts(g_vPlayerVelocity, true);
	FMOD_VECTOR forward = VecConverts(vecFwd);
	FMOD_VECTOR up = VecConverts(vecUp);

	// this velocity stuff is only for making Doppler effect.
	// [reference] https://en.wikipedia.org/wiki/Doppler_effect
	gFModSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
	gFModSystem->update();
}

void Sound_Exit()
{
	// only sound precache needs to release. channels don't.
	for (auto& elem : g_mapSoundPrecache)
	{
		elem.second->release();
	}

	g_mapSoundPrecache.clear();

	gFModSystem->close();
	gFModSystem->release();
}
