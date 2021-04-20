/*

Created Date: Nov 27 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define LOAD_TGA_SIG				"\x8B\x44\x24\x2A\x8B\x4C\x24\x2A\x8B\x54\x24\x0C\x6A\x01\x50\x8B"
#define LOAD_TGA_SIG_NEW			"\x55\x8B\xEC\x8B\x45\x18\x8B\x4D\x14\x8B\x55\x10\x6A\x00\x50\x8B\x45\x0C"
#define	KEY_NAME_FOR_BINDING_SIG	"\x55\x8B\xEC\x8D\x45\x08\x56\x50\xFF\x15\x2A\x2A\x2A\x2A\x83\xC4\x04"

typedef qboolean	(*ENGFUNC_LoadTGA)				(const char* szFilename, unsigned char* buffer, int bufferSize, int* width, int* height);
typedef const char*	(*ENGFUNC_Key_NameForBinding)	(const char* pszCommand);
typedef void		(*ENGFUNC_S_StartSound)(int entnum, int entchannel, sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch);
typedef void*		(*ENGFUNC_Cache_Check)(cache_user_t* c);
typedef sfxcache_t*	(*ENGFUNC_S_LoadSound)(sfx_t* s, /*channel_t* */void* ch);	// Due to we cannot handle channel_t*
typedef void		(*ENGFUNC_S_StopAllSounds)(bool STFU);	// @param: clear the sound buffer instantly?


// don't input these if this is loaded by client.dll.
// engine funcs got by searching its signiture.
#ifndef CLIENT_DLL

void SearchEngineFunctions(void);

extern ENGFUNC_LoadTGA g_pfnLoadTGA;
extern ENGFUNC_Key_NameForBinding g_pfnKey_NameForBinding;

#endif
