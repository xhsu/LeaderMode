/*

Created Date: Mar 12 2020
Reincarnation Date: Nov 24 2020

*/

#include "precompiled.h"

std::array<unsigned, 4U> g_rgiManpower = { 0U, 0U, 0U, 0U };
std::array<TacticalSchemes, SCHEMES_COUNT> g_rgiTeamSchemes;
std::array<std::wstring, ROLE_COUNT> g_rgwcsRoleNames;
std::array<std::wstring, SCHEMES_COUNT> g_rgwcsSchemeNames;

int CHudScenarioStatus::Init(void)
{
	m_bitsFlags = HUD_ACTIVE;

	int iHeight = 0, iWidth = 0;

	m_iIdManpower = LoadDDS("texture/HUD/Items/Manpower.dds", &iWidth, &iHeight); m_flManpowerTextureRatio = float(iWidth) / float(iHeight);

	m_rgiIdSchemeTexture[Scheme_UNASSIGNED]				= LoadDDS("texture/HUD/ClassesIcon/Doraemon.dds");	// FIXME
	m_rgiIdSchemeTexture[Doctrine_GrandBattleplan]		= LoadDDS("texture/HUD/Schemes/Doctrine_GrandBattleplan.dds");
	m_rgiIdSchemeTexture[Doctrine_MassAssault]			= LoadDDS("texture/HUD/Schemes/Doctrine_MassAssault.dds");
	m_rgiIdSchemeTexture[Doctrine_MobileWarfare]		= LoadDDS("texture/HUD/Schemes/Doctrine_MobileWarfare.dds");
	m_rgiIdSchemeTexture[Doctrine_SuperiorFirepower]	= LoadDDS("texture/HUD/Schemes/Doctrine_SuperiorFirepower.dds");

	g_rgwcsSchemeNames[Scheme_UNASSIGNED]			= UTIL_GetLocalisation("#LeaderMod_Scheme_UNASSIGNED");
	g_rgwcsSchemeNames[Doctrine_GrandBattleplan]	= UTIL_GetLocalisation("#LeaderMod_Doctrine_GrandBattleplan");
	g_rgwcsSchemeNames[Doctrine_MassAssault]		= UTIL_GetLocalisation("#LeaderMod_Doctrine_MassAssault");
	g_rgwcsSchemeNames[Doctrine_MobileWarfare]		= UTIL_GetLocalisation("#LeaderMod_Doctrine_MobileWarfare");
	g_rgwcsSchemeNames[Doctrine_SuperiorFirepower]	= UTIL_GetLocalisation("#LeaderMod_Doctrine_SuperiorFirepower");

	g_rgwcsRoleNames[Role_UNASSIGNED]	= UTIL_GetLocalisation("#LeaderMod_Role_UNASSIGNED");

	g_rgwcsRoleNames[Role_Commander]	= UTIL_GetLocalisation("#LeaderMod_Role_Commander");
	g_rgwcsRoleNames[Role_SWAT]			= UTIL_GetLocalisation("#LeaderMod_Role_SWAT");
	g_rgwcsRoleNames[Role_Breacher]		= UTIL_GetLocalisation("#LeaderMod_Role_Breacher");
	g_rgwcsRoleNames[Role_Sharpshooter]	= UTIL_GetLocalisation("#LeaderMod_Role_Sharpshooter");
	g_rgwcsRoleNames[Role_Medic]		= UTIL_GetLocalisation("#LeaderMod_Role_Medic");

	g_rgwcsRoleNames[Role_Godfather]	= UTIL_GetLocalisation("#LeaderMod_Role_Godfather");
	g_rgwcsRoleNames[Role_LeadEnforcer]	= UTIL_GetLocalisation("#LeaderMod_Role_LeadEnforcer");
	g_rgwcsRoleNames[Role_MadScientist]	= UTIL_GetLocalisation("#LeaderMod_Role_MadScientist");
	g_rgwcsRoleNames[Role_Assassin]		= UTIL_GetLocalisation("#LeaderMod_Role_Assassin");
	g_rgwcsRoleNames[Role_Arsonist]		= UTIL_GetLocalisation("#LeaderMod_Role_Arsonist");

	gHUD::AddHudElem(this);
	return 1;
}

int CHudScenarioStatus::VidInit(void)
{
	return 1;
}

void CHudScenarioStatus::Reset(void)
{
	g_rgiManpower.fill(0);
	g_rgiTeamSchemes.fill(Scheme_UNASSIGNED);
}

static std::wstring wcsKeyName;
static constexpr Vector VEC_WHITE_RED_DIFF = VEC_REDISH - Vector(1, 1, 1);

int CHudScenarioStatus::Draw(float fTime)
{
	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_HEALTH) || g_iUser1)
		return 1;

	if (gEngfuncs.IsSpectateOnly())
		return 1;

	if (gHUD::m_bPlayerDead)
		return 1;

	// Start from right next to radar.
	int x = CHudRadar::BORDER_GAP * 2 + CHudRadar::HUD_SIZE;
	int y = CHudRadar::BORDER_GAP;

	// Manpower Indicator
	x = CHudRadar::BORDER_GAP * 2 + CHudRadar::HUD_SIZE;	// Reset X pos on manpower indicator.
	y += GAP_PLAYERNAME_MANPOWER;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);

	// in order to make transparent fx on dds texture...
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	glColor4f(1, 1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, m_iIdManpower);

	auto iTall = MANPOWER_ICON_SIZE;
	auto iWidth = round(m_flManpowerTextureRatio * float(iTall));

	// no more than 10 manpower icon. use text and numbers for the rests.
	for (size_t i = 0; i < Q_min(g_rgiManpower[g_iTeam], 10U); i++)
	{
		glColor4f(1, 1, 1, MANPOWER_ALPHAS[i]);
		DrawUtils::Draw2DQuad(x, y, x + iWidth, y + iTall);

		x += iWidth + GAP_MANPOWER_INTERICON;	// right shift.
	}

	if (g_rgiManpower[g_iTeam] > 10)
	{
		gFontFuncs::DrawSetTextPos(x, y);
		gFontFuncs::DrawPrintText(m_rgwcsManpowerTexts[g_iTeam].c_str());
	}

	// Scheme Indicator

	// show no scheme indicator if there is a menu on screen.
	if (gHUD::m_Menu.m_bMenuDisplayed)
		return TRUE;

	// completely move x and y to another location.
	x = CHudRadar::BORDER_GAP;
	y = CHudRadar::BORDER_GAP + CHudRadar::HUD_SIZE + 10;

	// red-white flashing if its contesting.
	if (g_rgiTeamSchemes[g_iTeam] == Scheme_UNASSIGNED)
	{
		auto vecColor = Vector(1, 1, 1) + VEC_WHITE_RED_DIFF * gHUD::GetOscillation();
		glColor4f(vecColor.r, vecColor.g, vecColor.b, 1);
		gFontFuncs::DrawSetTextColor(vecColor, 1);
	}
	else
	{
		glColor4f(1, 1, 1, 1);
	}

	glBindTexture(GL_TEXTURE_2D, m_rgiIdSchemeTexture[g_rgiTeamSchemes[g_iTeam]]);
	DrawUtils::Draw2DQuad(x, y, x + SCHEME_ICON_SIZE, y + SCHEME_ICON_SIZE);

	// text of scheme name
	gFontFuncs::DrawSetTextFont(gHUD::m_ClassIndicator.m_hClassFont);
	gFontFuncs::DrawSetTextPos(x + SCHEME_ICON_SIZE + GAP_SCHEMEICON_TEXT, y);
	gFontFuncs::DrawPrintText(g_rgwcsSchemeNames[g_rgiTeamSchemes[g_iTeam]].c_str());

	wcsKeyName = L"[" + std::wstring(ANSIToUnicode(gExtFuncs.pfnKey_NameForBinding("votescheme"))) + L"]";
	gFontFuncs::DrawSetTextPos(x + SCHEME_ICON_SIZE + GAP_SCHEMEICON_TEXT, y + SCHEME_ICON_SIZE - CHudClassIndicator::FONT_TALL);
	gFontFuncs::DrawPrintText(wcsKeyName.c_str());

	// for CHudMoney, the below element.
	m_flLastY = y + SCHEME_ICON_SIZE;
	return 1;
}
