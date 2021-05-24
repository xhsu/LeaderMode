/*

Created Date: Mar 12 2020

*/

#include "precompiled.h"
#include "Interface/IVGuiDLL.h"


class CClientVGUI : public IClientVGUI
{
public:
	virtual void Initialize(CreateInterfaceFn* factories, int count);
	virtual void Start(void);
	virtual void SetParent(vgui::VPANEL parent);
	virtual bool UseVGUI1(void);
	virtual void HideScoreBoard(void);
	virtual void HideAllVGUIMenu(void);
	virtual void ActivateClientUI(void);
	virtual void HideClientUI(void);
};

CClientVGUI* g_pClientVGUI = nullptr;
vgui::CViewPort* g_pViewport = nullptr;

EXPOSE_SINGLE_INTERFACE(CClientVGUI, IClientVGUI, CLIENTVGUI_INTERFACE_VERSION);


//-----------------------------------------------------------------------------
// Purpose: finds a particular interface in the factory set
//-----------------------------------------------------------------------------
static void* InitializeInterface(char const* interfaceName, CreateInterfaceFn* factoryList, int numFactories)
{
	void* retval;

	for (int i = 0; i < numFactories; i++)
	{
		CreateInterfaceFn factory = factoryList[i];
		if (!factory)
			continue;

		retval = factory(interfaceName, NULL);
		if (retval)
			return retval;
	}

	// No provider for requested interface!!!
	// Assert( !"No provider for requested interface!!!" );

	return NULL;
}

void CClientVGUI::Initialize(CreateInterfaceFn* factoryList, int count)
{
	g_pClientVGUI = this;

	// load and initialize vgui
	CreateInterfaceFn thisFactory = Sys_GetFactoryThis();
	CreateInterfaceFn engineFactory = factoryList[0];
	CreateInterfaceFn vguiFactory = factoryList[1];
	CreateInterfaceFn fileSystemFactory = factoryList[2];

	IFileSystem* fs = (IFileSystem*)fileSystemFactory(FILESYSTEM_INTERFACE_VERSION, NULL);

	// load the VGUI library
	char szDllName[512];
	fs->GetLocalPath("cl_dlls\\vgui_dll.dll", szDllName, charsmax(szDllName));
	CreateInterfaceFn vguiDllFactory = Sys_GetFactory(Sys_LoadModule(szDllName));

	// setup the factory list
	CreateInterfaceFn myFactoryList[5] =
	{
		thisFactory,
		engineFactory,
		vguiFactory,
		fileSystemFactory,
		vguiDllFactory,
	};

	ConnectTier3Libraries(myFactoryList, _countof(myFactoryList));

	// setup VGUI library
	IVGuiDLL* staticVGuiDllFuncs = (IVGuiDLL*)vguiDllFactory(VGUI_IVGUIDLL_INTERFACE_VERSION, NULL);
	staticVGuiDllFuncs->Init(factoryList, count);

	vgui::VGui_InitInterfacesList("ClientUI", factoryList, count);

	// Do it as soon as we get IFileSystem.
	gHUD::AddFontFiles();

	if (FILE_SYSTEM)
	{
		// LUNA: add CZero and consequently, CStrike into resource lib.
		// both client and server are required to do so.
		// you have to write fallback_dir parameter in liblist.gam as well.
		// check this link from Valve Inc.:https://developer.valvesoftware.com/wiki/IFileSystemV009
		/*FILE_SYSTEM->AddSearchPath("czero", "GAME_FALLBACK");*/
	}
	/* UNDONE
	if (!vgui::VGui_InitInterfacesList("ClientUI", factories, count))
		return;

	enginevgui = (IEngineVGui*)factories[0](VENGINE_VGUI_VERSION, NULL);
	gameuifuncs = (IGameUIFuncs*)factories[0](VENGINE_GAMEUIFUNCS_VERSION, NULL);

	g_pClientVGUI = (IClientVGUI*)((CreateInterfaceFn)gExportfuncs.ClientFactory())(CLIENTVGUI_INTERFACE_VERSION, NULL);
	g_pClientVGUI->Initialize(factories, count);
	*/
}

void CClientVGUI::Start(void)
{
	/* UNDONE
	g_pClientVGUI->Start();
	g_pViewPort = new CViewport();
	g_pViewPort->Start();
	*/

	vgui::scheme()->LoadSchemeFromFile("Resource/SourceScheme.res", "SourceScheme");

	int swide, stall;
	g_pVGuiSurface->GetScreenSize(swide, stall);

	g_pViewport = new vgui::CViewPort();
	g_pViewport->SetBounds(0, 0, swide, stall);
	g_pViewport->SetPaintBorderEnabled(false);
	g_pViewport->SetPaintBackgroundEnabled(true);
	g_pViewport->SetPaintEnabled(false);
	g_pViewport->SetVisible(false);
	g_pViewport->SetMouseInputEnabled(false);
	g_pViewport->SetKeyBoardInputEnabled(false);
}

void CClientVGUI::SetParent(vgui::VPANEL parent)
{
/* UNDONE
	g_pClientVGUI->SetParent(parent);
	g_pViewPort->SetParent(parent);
	*/
	if (g_pViewport)
	{
		g_pViewport->SetParent(parent);
	}
}

bool CClientVGUI::UseVGUI1(void)
{
/* UNDONE
	return g_pClientVGUI->UseVGUI1();
	*/
	return false;
}

void CClientVGUI::HideScoreBoard(void)
{
/* UNDONE
	g_pViewPort->HideScoreBoard();
	g_pClientVGUI->HideScoreBoard();
	*/
}

void CClientVGUI::HideAllVGUIMenu(void)
{
/* UNDONE
	g_pViewPort->HideAllVGUIMenu();
	g_pClientVGUI->HideAllVGUIMenu();
	*/
}

void CClientVGUI::ActivateClientUI(void)
{
/* UNDONE
	g_pViewPort->ActivateClientUI();
	g_pClientVGUI->HideClientUI();
	*/

	// Mouse activation code moved into viewport.

	if (g_pViewport)
	{
		g_pViewport->OnActivateClientUI();
	}
}

void CClientVGUI::HideClientUI(void)
{
/* UNDONE
	g_pViewPort->HideClientUI();
	g_pClientVGUI->HideClientUI();
	*/

	if (g_pViewport)
	{
		g_pViewport->OnHideClientUI();
	}
}
