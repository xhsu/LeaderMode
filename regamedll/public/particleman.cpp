/*

Created Date: May 17 2020

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

CSysModule* g_hParticleManModule = NULL;
IParticleMan* g_pParticleMan = NULL;

void CL_UnloadParticleMan(void)
{
	Sys_UnloadModule(g_hParticleManModule);

	g_pParticleMan = NULL;
	g_hParticleManModule = NULL;
}

void CL_LoadParticleMan(void)
{
	char szPDir[512];

	if (gEngfuncs.COM_ExpandFilename(PARTICLEMAN_DLLNAME, szPDir, sizeof(szPDir)) == FALSE)
	{
		g_pParticleMan = NULL;
		g_hParticleManModule = NULL;
		return;
	}

	g_hParticleManModule = Sys_LoadModule(szPDir);
	CreateInterfaceFn particleManFactory = Sys_GetFactory(g_hParticleManModule);

	if (particleManFactory == NULL)
	{
		g_pParticleMan = NULL;
		g_hParticleManModule = NULL;
		return;
	}

	g_pParticleMan = (IParticleMan*)particleManFactory(PARTICLEMAN_INTERFACE, NULL);

	if (g_pParticleMan)
	{
		g_pParticleMan->SetUp(&gEngfuncs);

		// Add custom particle classes here BEFORE calling anything else or you will die.
		g_pParticleMan->AddCustomParticleClassSize(sizeof(CBaseParticle));
	}
}
