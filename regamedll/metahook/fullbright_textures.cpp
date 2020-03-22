/*

Created Date: Mar 22 2020

Complete a long forgotten model feature: STUDIO_NF_FULLBRIGHT

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"


void (*g_pfnR_StudioSetupSkin)(studiohdr_t* ptexturehdr, int index) = NULL;
void (*g_pfnR_LightLambert)(float light[3][4], float* normal, float* src, float* lambert) = NULL;


// This texture should be high lighting ?
bool g_bHighLighting = false;


void R_StudioSetupSkin(studiohdr_t* ptexturehdr, int index)
{
	mstudiotexture_t* ptexture = (mstudiotexture_t*)((byte*)ptexturehdr + ptexturehdr->textureindex);

	g_bHighLighting = (ptexture[index].flags & STUDIO_NF_FULLBRIGHT) ? true : false;

	g_pfnR_StudioSetupSkin(ptexturehdr, index);
}

void R_LightLambert(float light[3][4], float* normal, float* src, float* lambert)
{
	if (g_bHighLighting)
	{
		lambert[0] = lambert[1] = lambert[2] = 1.0f;
	}
	else
	{
		g_pfnR_LightLambert(light, normal, src, lambert);
	}
}


void FullBLight_InstallHook(void)
{
	*(void**)&g_pfnR_StudioSetupSkin = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, R_STUDIOSETUPSKIN_SIG, sizeof(R_STUDIOSETUPSKIN_SIG) - 1);
	if (!g_pfnR_StudioSetupSkin)
		*(void**)&g_pfnR_StudioSetupSkin = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, R_STUDIOSETUPSKIN_SIG_NEW, sizeof(R_STUDIOSETUPSKIN_SIG_NEW) - 1);

	if (g_pfnR_StudioSetupSkin)
	{
		g_pMetaHookAPI->InlineHook(g_pfnR_StudioSetupSkin, R_StudioSetupSkin, (void*&)g_pfnR_StudioSetupSkin);
	}

	*(void**)&g_pfnR_LightLambert = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, R_LIGHTLAMBERT_SIG, sizeof(R_LIGHTLAMBERT_SIG) - 1);
	if (!g_pfnR_LightLambert)
		*(void**)&g_pfnR_LightLambert = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, R_LIGHTLAMBERT_SIG_NEW, sizeof(R_LIGHTLAMBERT_SIG_NEW) - 1);

	if (g_pfnR_LightLambert)
	{
		g_pMetaHookAPI->InlineHook(g_pfnR_LightLambert, R_LightLambert, (void*&)g_pfnR_LightLambert);
	}
}
