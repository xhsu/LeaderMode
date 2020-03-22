/*

Created Date: Mar 22 2020

Transplantable function: make model loadable texture larger.

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define GL_LOADTEXTURE_SIG		"\xA1\xC8\x20\xEC\x01\x8B\x4C\x24\x20\x8B\x54\x24\x1C\x50\x8B\x44"
#define GL_LOADTEXTURE_SIG_NEW	"\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x8B\x4D\x24\x8B\x55\x20\x50\x8B\x45\x1C"

#define LOAD_TGA_SIG			"\x8B\x44\x24\x2A\x8B\x4C\x24\x2A\x8B\x54\x24\x0C\x6A\x01\x50\x8B"
#define LOAD_TGA_SIG_NEW		"\x55\x8B\xEC\x8B\x45\x18\x8B\x4D\x14\x8B\x55\x10\x6A\x00\x50\x8B\x45\x0C"

void LargerTexture_InstallHook(void);

// searched by sig.
extern qboolean (*g_pfnLoadTGA)(char* szFilename, unsigned char* buffer, int bufferSize, int* width, int* height);
