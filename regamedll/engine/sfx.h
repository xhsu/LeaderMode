/*

Created Date: Apr 20 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

typedef struct cache_user_s
{
	void* data;	// What the fuck, Valve?

} cache_user_t;

#define MAX_QPATH 64

typedef struct sfx_s
{
	char			name[MAX_QPATH];
	cache_user_t	cache;	// Feel funny, HUH?
	int				servercount;

} sfx_t;

typedef struct sfxcache_s
{
	int 	length;
	int 	loopstart;
	int 	speed;
	int 	width;
	int 	stereo;
	byte	data[1];		// variable sized

} sfxcache_t;
