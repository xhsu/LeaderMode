/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#include "precompiled.h"


cl_enginefunc_t gEngfuncs;
IBaseFileSystem* g_pFileSystem = nullptr;

IGameUIFuncs *gameuifuncs = NULL;
vgui::IEngineVGui *enginevguifuncs = NULL;
vgui::ISurface *enginesurfacefuncs = NULL;
cl_enginefunc_t *engine = NULL;

static CBasePanel *staticPanel = NULL;
IKeyValuesSystem* (*KeyValuesSystem)(void) = nullptr;

void DisplayOptionsDialog(void)
{
}

void DisplayCreateMultiplayerGameDialog(void)
{
}

IGameUI::IGameUI()
{
	m_bActivatedUI = false;
}

IGameUI::~IGameUI()
{
}

void IGameUI::Initialize(CreateInterfaceFn *factories, int count)
{
	char szDllName[512];

	// load and initialize vgui
	CreateInterfaceFn thisFactory = Sys_GetFactoryThis();
	CreateInterfaceFn engineFactory = factories[ 0 ];
	CreateInterfaceFn vguiFactory = factories[ 1 ];
	CreateInterfaceFn fileSystemFactory = factories[ 2 ];

	g_pFileSystem = (IFileSystem *)fileSystemFactory(FILESYSTEM_INTERFACE_VERSION, NULL);

	// load the VGUI library
	g_pFileSystem->GetLocalPath("cl_dlls\\vgui_dll.dll", szDllName, sizeof(szDllName));
	CreateInterfaceFn vguiDllFactory = Sys_GetFactory(Sys_LoadModule(szDllName));

	// Access MH module.
	HMODULE hMetahookDLL = GetModuleHandle("lm_metahook_module.dll");
	if (!hMetahookDLL)
	{
		Sys_Error("lm_metahook_module.dll no found!");
		return;
	}

	*(void**)&KeyValuesSystem = GetProcAddress(hMetahookDLL, "GetKeyValueSystem");

	if (!KeyValuesSystem)
	{
		Sys_Error("lm_metahook_module.dll export function \"KeyValuesSystem\" no found!");
		return;
	}

	// setup the factory list
	CreateInterfaceFn factoryList[5] =
	{
		thisFactory,
		engineFactory,
		vguiFactory,
		fileSystemFactory,
		vguiDllFactory,
	};

	// setup VGUI library
	IVGuiDLL *staticVGuiDllFuncs = (IVGuiDLL *)vguiDllFactory(VGUI_IVGUIDLL_INTERFACE_VERSION, NULL);
	staticVGuiDllFuncs->Init(factories, count);

	ConnectTier3Libraries(factoryList, _countof(factoryList));

	// setup vgui controls
	vgui::VGui_InitInterfacesList("GameUI", factories, count);

	// load localization file
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/vgui_%language%.txt");
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/gameui_%language%.txt");

	enginevguifuncs = (vgui::IEngineVGui *)engineFactory(VENGINE_VGUI_VERSION, NULL);
	enginesurfacefuncs = (vgui::ISurface *)vguiDllFactory(VGUI_SURFACE_INTERFACE_VERSION, NULL);
	gameuifuncs = (IGameUIFuncs *)engineFactory(VENGINE_GAMEUIFUNCS_VERSION, NULL);

	if (!enginesurfacefuncs || !gameuifuncs || !enginevguifuncs)
	{
		Sys_Error("IGameUI::Initialize() failed to get necessary interfaces\n");
	}

	int swide, stall;
	g_pVGuiSurface->GetScreenSize(swide, stall);

	// setup base panel
	staticPanel = new CBasePanel();
	staticPanel->SetBounds(0, 0, swide, stall);
	staticPanel->SetPaintBorderEnabled(false);
	staticPanel->SetPaintBackgroundEnabled(true);
	staticPanel->SetPaintEnabled(false);
	staticPanel->SetVisible(true);
	staticPanel->SetMouseInputEnabled(false);
	staticPanel->SetKeyBoardInputEnabled(false);

	vgui::VPANEL rootpanel = enginevguifuncs->GetPanel(vgui::PANEL_GAMEUIDLL);
	staticPanel->SetParent(rootpanel);
}

void IGameUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, IBaseSystem* system)
{
	memcpy(&gEngfuncs, engineFuncs, sizeof(gEngfuncs));
	engine = &gEngfuncs;

	vgui::scheme()->LoadSchemeFromFile( "Resource/SourceScheme.res", "SourceScheme" );

	//char cdkey[256];
	//
	//vgui::system()->GetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Half-Life\\Settings\\ValveKey", cdkey, sizeof(cdkey) - 1);
	//
	//if (!strlen(cdkey))
	//{
	//	vgui::system()->GetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Half-Life\\Settings\\yeK1", cdkey, sizeof(cdkey) - 1);
	//
	//	if (!strlen(cdkey))
	//	{
	//		vgui::system()->GetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Half-Life\\Settings\\yeK2", cdkey, sizeof(cdkey) - 1);
	//
	//		if (!strlen(cdkey))
	//		{
	//			if (!g_pCDKeyEntryDialog)
	//			{
	//				g_pCDKeyEntryDialog = new CCDKeyEntryDialog(staticPanel, false);
	//			}
	//
	//			g_pCDKeyEntryDialog->Activate();
	//		}
	//	}
	//}
}

void IGameUI::Shutdown(void)
{
}

int IGameUI::ActivateGameUI(void)
{
	if ( IsGameUIActive() )
		return 1;

	m_bActivatedUI = true;

	staticPanel->SetVisible( true );
	return 1;
}

int IGameUI::ActivateDemoUI(void)
{
	return 1;
}

int IGameUI::HasExclusiveInput(void)
{
	return IsGameUIActive();
}

void IGameUI::RunFrame(void)
{
	staticPanel->RunFrame();
}

void IGameUI::ConnectToServer(const char *game, int IP, int port)
{
}

void IGameUI::DisconnectFromServer(void)
{
}

void IGameUI::HideGameUI(void)
{
	const char *level = gEngfuncs.pfnGetLevelName();

	if (level && *level)
	{
		staticPanel->SetVisible( false );
	}
}

bool IGameUI::IsGameUIActive(void)
{
	if ( m_bActivatedUI )
	{
		return staticPanel->IsVisible();
	}

	return false;
}

void IGameUI::LoadingStarted(const char *resourceType, const char *resourceName)
{
	//gEngfuncs.Con_Printf("LoadingStarted: Type:%s Name:%s\n", resourceType, resourceName);
}

void IGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
	//gEngfuncs.Con_Printf("LoadingFinished: Type:%s Name:%s\n", resourceType, resourceName);
}

static bool g_skipfirstset = false;
void IGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
}

int IGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	//gEngfuncs.Con_Printf("ContinueProgressBar: Point:%d Fraction:%f\n", progressPoint, progressFraction);

	if (g_skipfirstset && progressPoint == 11)
	{
		g_skipfirstset = false;
		return 1;
	}

	return 1;
}

void IGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
}

int IGameUI::SetProgressBarStatusText(const char *statusText)
{
	return 1;
}

void IGameUI::SetSecondaryProgressBar(float progress)
{
}

void IGameUI::SetSecondaryProgressBarText(const char *statusText)
{
}

void IGameUI::ValidateCDKey(bool force, bool inConnect)
{
}

void IGameUI::OnDisconnectFromServer(int eSteamLoginFailure, const char *username)
{
}

EXPOSE_SINGLE_INTERFACE(IGameUI, IGameUI, GAMEUI_INTERFACE_VERSION);
