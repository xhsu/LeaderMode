/*

Created Date: Mar 22 2020

Transplantable function: make model loadable texture larger.

Additional function is included:
	- External texture loader.

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

int (*g_pfnGL_LoadTexture)(char* identifier, int textureType, int width, int height, BYTE* data, int mipmap, int iType, BYTE* pPal) = nullptr;


int GL_LoadTexture(char* identifier, int textureType, int width, int height, BYTE* data, int mipmap, int iType, BYTE* pPal)
{
	// external texture loader.

	// analyze and decompose the name.
	// the identifier parameter is given in the format "models/weapons/xxx.mdlyyy.bmp"
	// we have to cut it down on the mark of ".mdl"
	char* p = Q_strstr(identifier, ".mdl");
	if (p != nullptr)
	{
		char* p2 = p + sizeof(char) * 4;	// 4 characters (i.e. ".mdl") later.

		// get model 'path'
		char szModel[128];
		Q_strcpy(szModel, identifier);
		Q_strtok(szModel, ".");	// only characters before ".mdl" can be saved.

		// get texture.
		char szTexture[64];
		Q_strcpy(szTexture, p2);
		Q_strtok(szTexture, ".");	// only characters before ".bmp" can be saved.
		Q_strcat(szTexture, ".dds");

		char szExternalTexturePath[192];
		Q_sprintf(szExternalTexturePath, "texture/%s/%s", szModel, szTexture);	// it would looks like this: "texture/models/weapons/xxx/yyy.dds"

		if (g_pInterface->FileSystem->FileExists(szExternalTexturePath))
		{
			return LoadDDS(szExternalTexturePath);
		}
	}

	// large texture loader.
	// it can't be both large and external texture at the same time. therefore, an elseif is used.
	if (width * height > 294912)	// biggest known loadable: 768*384
	{
		static BYTE buffer[4096 * 4096 * 3];

		for (DWORD i = 0; i < DWORD(width * height); ++i)
		{
			buffer[(i * 3) + 0] = pPal[(data[i] * 3) + 0];
			buffer[(i * 3) + 1] = pPal[(data[i] * 3) + 1];
			buffer[(i * 3) + 2] = pPal[(data[i] * 3) + 2];
		}

		GLuint id = (GLuint)VGUI_SURFACE->CreateNewTextureID();
		//glGenTextures(1, &id);

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
}
