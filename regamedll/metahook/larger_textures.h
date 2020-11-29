/*

Created Date: Mar 22 2020

Transplantable function: make model loadable texture larger.

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define GL_LOADTEXTURE_SIG		"\xA1\xC8\x20\xEC\x01\x8B\x4C\x24\x20\x8B\x54\x24\x1C\x50\x8B\x44"
#define GL_LOADTEXTURE_SIG_NEW	"\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x8B\x4D\x24\x8B\x55\x20\x50\x8B\x45\x1C"

void LargerTexture_InstallHook(void);
