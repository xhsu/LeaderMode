#include "precompiled.h"

static IGameConsole g_GameConsole;

IGameConsole &GameConsole()
{
	return g_GameConsole;
}

IGameConsole::IGameConsole()
{
	m_bInitialized = false;
}

IGameConsole::~IGameConsole()
{
	m_bInitialized = false;
}

void IGameConsole::Activate(void)
{
	if (!m_bInitialized)
		return;

	vgui::surface()->RestrictPaintToSinglePanel(NULL);
	m_pConsole->Activate();
}

void IGameConsole::Initialize(void)
{
	m_pConsole = vgui::SETUP_PANEL( new CGameConsoleDialog() );

	int swide, stall;
	vgui::surface()->GetScreenSize(swide, stall);
	int offset = vgui::scheme()->GetProportionalScaledValue(16);

	m_pConsole->SetBounds(
		swide / 2 - (offset * 4),
		offset,
		(swide / 2) + (offset * 3),
		stall - (offset * 8));

	m_pConsole->SetMinimumSize(368, 352);

	m_bInitialized = true;
}

void IGameConsole::Hide(void)
{
	if (!m_bInitialized)
		return;

	m_pConsole->Hide();
}

void IGameConsole::Clear(void)
{
	if (!m_bInitialized)
		return;

	m_pConsole->Clear();
}

bool IGameConsole::IsConsoleVisible(void)
{
	if (!m_bInitialized)
		return false;
	
	return m_pConsole->IsVisible();
}

void IGameConsole::Printf(const char *format, ...)
{
	if (!m_bInitialized)
		return;

	static char buffer[1024];

	va_list ap;
	va_start(ap, format);
	_vsnprintf(buffer, sizeof(buffer)-1, format, ap);
	va_end(ap);

	m_pConsole->Print(buffer);
}

void IGameConsole::DPrintf(const char *format, ...)
{
	if (!m_bInitialized)
		return;

	static char buffer[1024];

	va_list ap;
	va_start(ap, format);
	_vsnprintf(buffer, sizeof(buffer)-1, format, ap);
	va_end(ap);

	m_pConsole->DPrint(buffer);
}

void IGameConsole::SetParent(int parent)
{
	if (!m_bInitialized)
		return;

	m_pConsole->SetParent( (vgui::VPANEL)parent );
}

EXPOSE_SINGLE_INTERFACE(IGameConsole, IGameConsole, GAMECONSOLE_INTERFACE_VERSION);
