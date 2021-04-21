/*

Created Date: Apr 21 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define S_StartStaticSound_SIG	"\x55\x8B\xEC\x83\xEC\x44\x53\x56\x57\x8B\x7D\x10\x85\xFF\xC7\x45\xFC\x00\x00\x00\x00"
#define S_StartDynamicSound_SIG	"\x55\x8B\xEC\x83\xEC\x48\xA1\x2A\x2A\x2A\x2A\x53\x56\x57\x85\xC0"
#define S_LoadSound_SIG			"\x55\x8B\xEC\x81\xEC\x2A\x2A\x2A\x2A\x53\x8B\x5D\x08\x56\x57\x8A\x03"
#define S_StopAllSounds_SIG		"\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x85\xC0\x74\x2A\x56\xC7\x05\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x2A\xBE"

typedef void		(*ENGFUNC_S_StartSound)			(int entnum, int entchannel, sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch);
typedef sfxcache_t* (*ENGFUNC_S_LoadSound)			(sfx_t* s, /*channel_t* */void* ch);	// Due to we cannot handle channel_t*
typedef void		(*ENGFUNC_S_StopAllSounds)		(bool STFU);	// @param: clear the sound buffer instantly?

#ifndef CLIENT_DLL
void Sound_InstallHook();

namespace engine
{
	extern ENGFUNC_S_StartSound S_StartStaticSound;
	extern ENGFUNC_S_StartSound S_StartDynamicSound;
	extern ENGFUNC_S_LoadSound S_LoadSound;
	extern ENGFUNC_S_StopAllSounds S_StopAllSounds;
};
#endif
