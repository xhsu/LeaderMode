/*

Created Date: Mar 12 2020
Reincarnation Date: Nov 24 2020
Remastered Date: May 14 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#include "precompiled.h"

//#define DEBUG_MATCH_STATUS

std::array<unsigned, 4U> g_rgiManpower = { 0U, 0U, 0U, 0U };
std::array<TacticalSchemes, SCHEMES_COUNT> g_rgiTeamSchemes;
std::array<std::wstring, ROLE_COUNT> g_rgwcsRoleNames;
std::array<std::wstring, SCHEMES_COUNT> g_rgwcsSchemeNames;

void CHudMatchStatus::Initialize(void)
{
	gHUD::m_lstElements.push_back({
		Initialize,
		nullptr,
		Reset,
		Draw,
		Think,
		nullptr,
		Reset,
		});

	g_rgwcsSchemeNames[Scheme_UNASSIGNED] = UTIL_GetLocalisation("#LeaderMod_Scheme_UNASSIGNED");
	g_rgwcsSchemeNames[Doctrine_GrandBattleplan] = UTIL_GetLocalisation("#LeaderMod_Doctrine_GrandBattleplan");
	g_rgwcsSchemeNames[Doctrine_MassAssault] = UTIL_GetLocalisation("#LeaderMod_Doctrine_MassAssault");
	g_rgwcsSchemeNames[Doctrine_MobileWarfare] = UTIL_GetLocalisation("#LeaderMod_Doctrine_MobileWarfare");
	g_rgwcsSchemeNames[Doctrine_SuperiorFirepower] = UTIL_GetLocalisation("#LeaderMod_Doctrine_SuperiorFirepower");

	g_rgwcsRoleNames[Role_UNASSIGNED] = UTIL_GetLocalisation("#LeaderMod_Role_UNASSIGNED");

	g_rgwcsRoleNames[Role_Commander] = UTIL_GetLocalisation("#LeaderMod_Role_Commander");
	g_rgwcsRoleNames[Role_SWAT] = UTIL_GetLocalisation("#LeaderMod_Role_SWAT");
	g_rgwcsRoleNames[Role_Breacher] = UTIL_GetLocalisation("#LeaderMod_Role_Breacher");
	g_rgwcsRoleNames[Role_Sharpshooter] = UTIL_GetLocalisation("#LeaderMod_Role_Sharpshooter");
	g_rgwcsRoleNames[Role_Medic] = UTIL_GetLocalisation("#LeaderMod_Role_Medic");

	g_rgwcsRoleNames[Role_Godfather] = UTIL_GetLocalisation("#LeaderMod_Role_Godfather");
	g_rgwcsRoleNames[Role_LeadEnforcer] = UTIL_GetLocalisation("#LeaderMod_Role_LeadEnforcer");
	g_rgwcsRoleNames[Role_MadScientist] = UTIL_GetLocalisation("#LeaderMod_Role_MadScientist");
	g_rgwcsRoleNames[Role_Assassin] = UTIL_GetLocalisation("#LeaderMod_Role_Assassin");
	g_rgwcsRoleNames[Role_Arsonist] = UTIL_GetLocalisation("#LeaderMod_Role_Arsonist");
}

void CHudMatchStatus::Draw(float flTime, bool bIntermission)
{
	if (gHUD::m_bitsHideHUDDisplay & HIDEHUD_ALL)	// This should display even at intermission.
		return;

	if (m_rgflTeamPower[TEAM_CT] <= DBL_EPSILON && m_rgflTeamPower[TEAM_TERRORIST] <= DBL_EPSILON)
		return;	// Game does not start yet.

	if (m_flTerroristPercentage <= DBL_EPSILON && m_flCTPercentage <= DBL_EPSILON)
		return;	// Same as above.

	Vector2D vecLT, vecRB;
	float flAlpha = MIN_ALPHA + (255 - MIN_ALPHA) * gHUD::GetOscillation(2.0 * M_PI / BOP_HIGHLIGHT_PERIOD);

	DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, BOP_DEFAULT_ALPHA);
	DrawUtils::Draw2DQuadProgressBar2(BOP_ANCHOR, BOP_SIZE, BOP_BORDER_THICKNESS, 1);	// Outter border.

	vecLT = BOP_INNERBLOCK_ANCHOR;
	vecRB = BOP_INNERBLOCK_ANCHOR + Vector2D(BOP_INNERBLOCK_SIZE.width * m_flTerroristPercentage, BOP_INNERBLOCK_SIZE.height);
	DrawUtils::glSetColor(RGB_T_COLOUR, g_iTeam == TEAM_TERRORIST ? flAlpha : BOP_DEFAULT_ALPHA);
	DrawUtils::Draw2DQuadNoTex(vecLT, vecRB);

	vecLT.x += BOP_INNERBLOCK_SIZE.width * m_flTerroristPercentage;
	vecRB = vecLT + Vector2D(BOP_INNERBLOCK_SIZE.width * m_flCTPercentage, BOP_INNERBLOCK_SIZE.height);
	DrawUtils::glSetColor(RGB_CT_COLOUR, g_iTeam == TEAM_CT ? flAlpha : BOP_DEFAULT_ALPHA);
	DrawUtils::Draw2DQuadNoTex(vecLT, vecRB);

	DrawUtils::glRegularPureColorDrawingExit();

#ifdef DEBUG_MATCH_STATUS
	auto mp_ct = g_rgiManpower[TEAM_CT] * FACTOR_MANPOWER;
	auto mp_t = m_rgflTeamPower[TEAM_TERRORIST] = g_rgiManpower[TEAM_TERRORIST] * FACTOR_MANPOWER;
	double hp[4] = { 0, 0, 0, 0 }, ap[4] = { 0, 0, 0, 0 };

	for (int i = 0; i <= gEngfuncs.GetMaxClients(); i++)
	{
		if (g_PlayerInfoList[i].name == nullptr)	// existence of a player.
			continue;

		auto iTeam = g_PlayerExtraInfo[i].m_iTeam;
		if (iTeam != TEAM_CT && iTeam != TEAM_TERRORIST)	// filter spectator as well.
			continue;

		hp[iTeam] += g_PlayerExtraInfo[i].m_iHealth * FACTOR_HP;
		ap[iTeam] += g_PlayerExtraInfo[i].m_iAccount * FACTOR_FUND;
	}

	std::wstring wcs_ct = L"CT: manpower: " + std::to_wstring(mp_ct) + std::wstring(L", hp: ") + std::to_wstring(hp[TEAM_CT]) + std::wstring(L", ap: ") + std::to_wstring(ap[TEAM_CT]);
	std::wstring wcs_t = L"T: manpower: " + std::to_wstring(mp_t) + std::wstring(L", hp: ") + std::to_wstring(hp[TEAM_TERRORIST]) + std::wstring(L", ap: ") + std::to_wstring(ap[TEAM_TERRORIST]);
	gFontFuncs::DrawSetTextFont(gHUD::m_hCambriaFont);
	gFontFuncs::DrawSetTextPos(0, 0);
	gFontFuncs::DrawSetTextColor(RGB_CT_COLOUR, 255);	// colour of CT.
	gFontFuncs::DrawPrintText(wcs_ct.c_str());

	gFontFuncs::DrawSetTextPos(0, 36);
	gFontFuncs::DrawSetTextColor(RGB_T_COLOUR, 255);	// colour of T.
	gFontFuncs::DrawPrintText(wcs_t.c_str());
#endif // DEBUG_MATCH_STATUS
}

void CHudMatchStatus::Think(void)
{
	// just as if it was re-zero.
	m_rgflTeamPower[TEAM_CT] = g_rgiManpower[TEAM_CT] * FACTOR_MANPOWER;
	m_rgflTeamPower[TEAM_TERRORIST] = g_rgiManpower[TEAM_TERRORIST] * FACTOR_MANPOWER;

	for (int i = 0; i <= gEngfuncs.GetMaxClients(); i++)
	{
		if (g_PlayerInfoList[i].name == nullptr)	// existence of a player.
			continue;

		auto iTeam = g_PlayerExtraInfo[i].m_iTeam;
		if (iTeam != TEAM_CT && iTeam != TEAM_TERRORIST)	// filter spectator as well.
			continue;

		m_rgflTeamPower[iTeam] += g_PlayerExtraInfo[i].m_iHealth * FACTOR_HP;
		m_rgflTeamPower[iTeam] += g_PlayerExtraInfo[i].m_iAccount * FACTOR_FUND;
	}

	m_flTerroristPercentage = m_rgflTeamPower[TEAM_TERRORIST] / (m_rgflTeamPower[TEAM_CT] + m_rgflTeamPower[TEAM_TERRORIST]);
	m_flCTPercentage = 1.0 - m_flTerroristPercentage;	// The rest must be CT.
}

void CHudMatchStatus::Reset(void)
{
	BOP_ANCHOR = Vector2D((ScreenWidth - BOP_SIZE.width) / 2, 0);
	BOP_INNERBLOCK_ANCHOR = BOP_ANCHOR + Vector2D(BOP_INNERBLOCK_MARGIN) + Vector2D(BOP_BORDER_THICKNESS);

	m_rgflTeamPower.fill(0);
	g_rgiTeamSchemes.fill(Scheme_UNASSIGNED);
	g_rgiManpower.fill(0);

	m_flTerroristPercentage = 0;
	m_flCTPercentage = 0;

	Q_memset(&g_PlayerExtraInfo[0], NULL, sizeof(g_PlayerExtraInfo));
}
