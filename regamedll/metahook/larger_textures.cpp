/*

Created Date: Mar 22 2020

Transplantable function: make model loadable texture larger.

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

int (*g_pfnGL_LoadTexture)(char* identifier, int textureType, int width, int height, BYTE* data, int mipmap, int iType, BYTE* pPal) = NULL;
qboolean (*g_pfnLoadTGA)(char* szFilename, unsigned char* buffer, int bufferSize, int* width, int* height) = NULL;


int GL_LoadTexture(char* identifier, int textureType, int width, int height, BYTE* data, int mipmap, int iType, BYTE* pPal)
{
	if (width * height > 294912)	// biggest known loadable: 768*384
	{
		static BYTE buffer[4096 * 4096 * 3];

		for (DWORD i = 0; i < DWORD(width * height); ++i)
		{
			buffer[(i * 3) + 0] = pPal[(data[i] * 3) + 0];
			buffer[(i * 3) + 1] = pPal[(data[i] * 3) + 1];
			buffer[(i * 3) + 2] = pPal[(data[i] * 3) + 2];
		}

		GLuint id = 0;	// used to create by g_pSurface->CreateNewTextureID();
		glGenTextures(1, &id);

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		return id;
	}

	return g_pfnGL_LoadTexture(identifier, textureType, width, height, data, mipmap, iType, pPal);
}

void LargerTexture_InstallHook(void)
{
	g_pfnGL_LoadTexture = (int (*)(char*, int, int, int, BYTE*, int, int, BYTE*))g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, GL_LOADTEXTURE_SIG, sizeof(GL_LOADTEXTURE_SIG) - 1);
	if (!g_pfnGL_LoadTexture)
		g_pfnGL_LoadTexture = (int (*)(char*, int, int, int, BYTE*, int, int, BYTE*))g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, GL_LOADTEXTURE_SIG_NEW, sizeof(GL_LOADTEXTURE_SIG_NEW) - 1);

	if (g_pfnGL_LoadTexture)
	{
		g_pMetaHookAPI->InlineHook(g_pfnGL_LoadTexture, GL_LoadTexture, (void*&)g_pfnGL_LoadTexture);
	}

	g_pfnLoadTGA = (qboolean (*)(char*, unsigned char*, int, int*, int*))g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, LOAD_TGA_SIG, sizeof(LOAD_TGA_SIG) - 1);
	if (!g_pfnLoadTGA)
		g_pfnLoadTGA = (qboolean (*)(char*, unsigned char*, int, int*, int*))g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, LOAD_TGA_SIG_NEW, sizeof(LOAD_TGA_SIG_NEW) - 1);
}
