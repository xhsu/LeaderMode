/*

Created Date: Mar 26 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

int g_rgiTeamScore[4] = { 0, 0, 0, 0 };

const char* CHudScoreboard::m_rgszScoreboardElemKeyName[8] =
{
	"#Career_PlayerName",
	"#Cstrike_TitlesTXT_CLASS",
	"#Cstrike_TitlesTXT_Health",
	"#LeaderMod_SBE_Money",
	"#Cstrike_TitlesTXT_SCORE",
	"#Cstrike_TitlesTXT_DEATHS",
	"#LeaderMod_SBE_KDA",
	"#Cstrike_TitlesTXT_LATENCY"
};

const char* CHudScoreboard::m_rgszRoleNamesKey[ROLE_COUNT] =
{
	"#LeaderMod_Role_UNASSIGNED",

	"#LeaderMod_Role_Commander",
	"#LeaderMod_Role_SWAT",
	"#LeaderMod_Role_Breacher",
	"#LeaderMod_Role_Sharpshooter",
	"#LeaderMod_Role_Medic",

	"#LeaderMod_Role_Godfather",
	"#LeaderMod_Role_LeadEnforcer",
	"#LeaderMod_Role_MadScientist",
	"#LeaderMod_Role_Assassin",
	"#LeaderMod_Role_Arsonist",
};

const wchar_t* CHudScoreboard::m_pwszTeamName[4] = { nullptr, nullptr, nullptr, nullptr };
const wchar_t* CHudScoreboard::m_pwszPlayerCalled = nullptr;
const wchar_t* CHudScoreboard::m_pwszDeathCalled = nullptr;
const wchar_t* CHudScoreboard::m_rgpwcScoreboardElementName[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
const wchar_t* CHudScoreboard::m_pwcTeamWinsText = nullptr;
const wchar_t* CHudScoreboard::m_rgpwcRoleNames[ROLE_COUNT] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

int CHudScoreboard::Init(void)
{
	m_bitsFlags = HUD_ENFORCE_THINK|HUD_INTERMISSION;	// this is what intermission for, right?

	gHUD::AddHudElem(this);
	return m_Baseboard.Initialize();
}

int CHudScoreboard::VidInit(void)
{
	// get team name from localise.
	m_pwszTeamName[TEAM_CT] = VGUI_LOCALISE->Find("#Cstrike_ScoreBoard_CT");
	m_pwszTeamName[TEAM_TERRORIST] = VGUI_LOCALISE->Find("#Cstrike_ScoreBoard_Ter");
	m_pwszTeamName[TEAM_SPECTATOR] = VGUI_LOCALISE->Find("#Cstrike_TitlesTXT_Spectators");
	m_pwszPlayerCalled = VGUI_LOCALISE->Find("#Cstrike_TitlesTXT_Player_plural");
	m_pwszDeathCalled = VGUI_LOCALISE->Find("#LeaderMod_SBE_KIA");
	m_pwcTeamWinsText = VGUI_LOCALISE->Find("#Cstrike_TitlesTXT_WINS");

	if (!m_pwszTeamName[TEAM_CT])
		m_pwszTeamName[TEAM_CT] = L"#Cstrike_ScoreBoard_CT";

	if (!m_pwszTeamName[TEAM_TERRORIST])
		m_pwszTeamName[TEAM_TERRORIST] = L"#Cstrike_ScoreBoard_Ter";

	if (!m_pwszTeamName[TEAM_SPECTATOR])
		m_pwszTeamName[TEAM_SPECTATOR] = L"#Cstrike_TitlesTXT_Spectators";

	if (!m_pwszPlayerCalled)
		m_pwszPlayerCalled = L"#Cstrike_TitlesTXT_Player_plural";

	if (!m_pwszDeathCalled)
		m_pwszDeathCalled = L"#LeaderMod_SBE_KIA";

	if (!m_pwcTeamWinsText)
		m_pwcTeamWinsText = L"#Cstrike_TitlesTXT_WINS";

	for (int i = 0; i < 8; i++)
	{
		m_rgpwcScoreboardElementName[i] = VGUI_LOCALISE->Find(m_rgszScoreboardElemKeyName[i]);

		if (!m_rgpwcScoreboardElementName[i])
			m_rgpwcScoreboardElementName[i] = L"TEXT NO FOUND!";
	}

	for (int i = 0; i < ROLE_COUNT; i++)
	{
		m_rgpwcRoleNames[i] = VGUI_LOCALISE->Find(m_rgszRoleNamesKey[i]);

		if (!m_rgpwcRoleNames[i])
			m_rgpwcRoleNames[i] = L"TEXT NO FOUND!";
	}

	// font.
	m_hPlayerNameFont = gFontFuncs.CreateFont();
	gFontFuncs.AddGlyphSetToFont(m_hPlayerNameFont, "Trajan Pro", 18, FW_MEDIUM, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0xFFFF);

	// base board: translucent black
	m_Baseboard.m_bitsFlags |= HUD_ACTIVE;
	m_Baseboard.SetAlpha(192);

	m_Baseboard.m_vecCoord.x = float(ScreenWidth) / 3.0f / 2.0f;
	m_Baseboard.m_vecCoord.y = float(ScreenHeight) / 3.0f / 2.0f;

	m_Baseboard.m_flWidth = float(ScreenWidth) * 2.0f / 3.0f;
	m_Baseboard.m_flHeight = float(ScreenHeight) * 2.0f / 3.0f;

	m_flChunkOffset = m_Baseboard.m_flWidth / 8.0f;	// NAME|CLASS|HP|MONEY|KILL|DEATH|KDA|PING = 8

	// server name.
	m_ServerName.m_bitsFlags |= HUD_ACTIVE;
	m_ServerName.m_vecCoord = Vector2D(7, 5);
	m_ServerName.SetANSI(g_szServerName);
	m_ServerName.SetColour(255, 255, 255, 255);
	m_ServerName.CreateFont("Century Gothic", 24, FW_BOLD, 1, 0, FONTFLAG_ANTIALIAS);

	m_Baseboard.AddChild(&m_ServerName);

	// line between players and server names.
	m_Line01.m_bitsFlags |= HUD_ACTIVE;
	m_Line01.m_flHeight = 5;
	m_Line01.m_flWidth = m_Baseboard.m_flWidth;
	m_Line01.m_vecCoord = Vector2D(0, 24 + 5 + 5);	// font height of server name & server text base ofs.
	m_Line01.SetColour(255, 255, 255, 255);

	m_Baseboard.AddChild(&m_Line01);

	return m_Baseboard.VidInit();
}

int CHudScoreboard::Draw(float flTime)
{
	m_Baseboard.Draw(flTime);

	float x = m_Line01.GetX() + 7;
	float y = m_Line01.GetY() + m_Line01.m_flHeight + 5;

	int iPlayerCounts = GetTeamCounts(TEAM_CT);
	int iTextWidth = 0, iTextHeight = 0;

	wchar_t wszText[MAX_PLAYER_NAME_LENGTH];
	_snwprintf(wszText, wcharsmax(wszText), L"%s (%d %s) %s: %d", m_pwszTeamName[TEAM_CT], iPlayerCounts, m_pwszPlayerCalled, m_pwcTeamWinsText, g_rgiTeamScore[TEAM_CT]);

	// CTs
	gFontFuncs.DrawSetTextFont(m_hPlayerNameFont);
	gFontFuncs.DrawSetTextPos(x, y);
	gFontFuncs.DrawSetTextColor(173, 201, 235, 255);	// colour of CT.
	gFontFuncs.DrawPrintText(wszText);

	float x2 = x + m_flChunkOffset * 2; // starts from column #3, "MONEY".
	for (int i = 3; i < 8; i++)
	{
		x2 += m_flChunkOffset;
		gFontFuncs.DrawSetTextFont(m_hPlayerNameFont);
		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawSetTextColor(173, 201, 235, 255);	// colour of CT.
		gFontFuncs.DrawPrintText(m_rgpwcScoreboardElementName[i]);
	}

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(173 / 255.0, 201 / 255.0, 235 / 255.0, 1);

	gFontFuncs.GetTextSize(m_hPlayerNameFont, wszText, &iTextWidth, &iTextHeight);
	DrawUtils::Draw2DQuadNoTex(m_Baseboard.GetX(), y + iTextHeight + 2, m_Baseboard.GetX() + m_Baseboard.m_flWidth, y + iTextHeight + 4);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	for (int i = 1; i <= MAX_PLAYERS; i++)
	{
		if (!IsConnected(i))
			continue;

		if (g_PlayerExtraInfo[i].m_iTeam != TEAM_CT)
			continue;

		gFontFuncs.GetTextSize(m_hPlayerNameFont, wszText, &iTextWidth, &iTextHeight);
		y += iTextHeight + 4;

		wcsncpy_s(wszText, UTF8ToUnicode(g_PlayerInfoList[i].name), MAX_PLAYER_NAME_LENGTH);

		if (g_PlayerExtraInfo[i].m_bIsDead)
			_snwprintf(wszText, wcharsmax(wszText), L"%s (%s)", UTF8ToUnicode(g_PlayerInfoList[i].name), m_pwszDeathCalled);

		gFontFuncs.DrawSetTextFont(m_hPlayerNameFont);
		gFontFuncs.DrawSetTextPos(x, y);
		gFontFuncs.DrawSetTextColor(173, 201, 235, 255);	// colour of CT.
		gFontFuncs.DrawPrintText(wszText);

		// 1. Classes
		// since we are no longer showing HP, use a bit its spaces.
		x2 = x + m_flChunkOffset * 1.5f;
		if (g_PlayerExtraInfo[i].m_iTeam == g_iTeam || g_iTeam == TEAM_SPECTATOR)
		{
			gFontFuncs.DrawSetTextPos(x2, y);
			gFontFuncs.DrawPrintText(m_rgpwcRoleNames[g_PlayerExtraInfo[i].m_iRoleType]);
		}

		// 2. HP
		// (placeholder)
		x2 += m_flChunkOffset * 0.5f;
		/*if (g_PlayerExtraInfo[i].m_iTeam == g_iTeam || g_iTeam == TEAM_SPECTATOR)
		{
			_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerExtraInfo[i].m_iHealth);
			gFontFuncs.DrawSetTextPos(x2, y);
			gFontFuncs.DrawPrintText(wszText);
		}*/

		// 3. Money
		x2 += m_flChunkOffset;
		if (g_PlayerExtraInfo[i].m_iTeam == g_iTeam || g_iTeam == TEAM_SPECTATOR)
		{
			_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerExtraInfo[i].m_iAccount);
			gFontFuncs.DrawSetTextPos(x2, y);
			gFontFuncs.DrawPrintText(wszText);
		}

		// 4. Kill
		x2 += m_flChunkOffset;
		_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerExtraInfo[i].m_iKills);
		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawPrintText(wszText);

		// 5. Deaths
		x2 += m_flChunkOffset;
		_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerExtraInfo[i].m_iDeaths);
		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawPrintText(wszText);

		// 6. KDA
		x2 += m_flChunkOffset;
		_snwprintf(wszText, wcharsmax(wszText), L"%.2f", float(g_PlayerExtraInfo[i].m_iKills) / float(Q_max(short(1), g_PlayerExtraInfo[i].m_iDeaths)));
		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawPrintText(wszText);

		// 7. PING
		x2 += m_flChunkOffset;
		if (g_PlayerInfoList[i].ping > 0)
			_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerInfoList[i].ping);
		else
			wcsncpy_s(wszText, L"Local", 6);

		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawPrintText(wszText);
	}

	// TERs
	y += iTextHeight * 2;
	iPlayerCounts = GetTeamCounts(TEAM_TERRORIST);
	_snwprintf(wszText, wcharsmax(wszText), L"%s (%d %s) %s: %d", m_pwszTeamName[TEAM_TERRORIST], iPlayerCounts, m_pwszPlayerCalled, m_pwcTeamWinsText, g_rgiTeamScore[TEAM_TERRORIST]);

	gFontFuncs.DrawSetTextFont(m_hPlayerNameFont);
	gFontFuncs.DrawSetTextPos(x, y);
	gFontFuncs.DrawSetTextColor(216, 81, 80, 255);	// colour of T.
	gFontFuncs.DrawPrintText(wszText);

	x2 = x + m_flChunkOffset * 2; // starts from column #3, "MONEY".
	for (int i = 3; i < 8; i++)
	{
		x2 += m_flChunkOffset;
		gFontFuncs.DrawSetTextFont(m_hPlayerNameFont);
		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawPrintText(m_rgpwcScoreboardElementName[i]);
	}

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(216 / 255.0, 81 / 255.0, 80 / 255.0, 1);

	gFontFuncs.GetTextSize(m_hPlayerNameFont, wszText, &iTextWidth, &iTextHeight);
	DrawUtils::Draw2DQuadNoTex(m_Baseboard.GetX(), y + iTextHeight + 2, m_Baseboard.GetX() + m_Baseboard.m_flWidth, y + iTextHeight + 4);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	for (int i = 1; i <= MAX_PLAYERS; i++)
	{
		if (!IsConnected(i))
			continue;

		if (g_PlayerExtraInfo[i].m_iTeam != TEAM_TERRORIST)
			continue;

		gFontFuncs.GetTextSize(m_hPlayerNameFont, wszText, &iTextWidth, &iTextHeight);
		y += iTextHeight + 4;

		wcsncpy_s(wszText, UTF8ToUnicode(g_PlayerInfoList[i].name), MAX_PLAYER_NAME_LENGTH);

		if (g_PlayerExtraInfo[i].m_bIsDead)
			_snwprintf(wszText, wcharsmax(wszText), L"%s (%s)", UTF8ToUnicode(g_PlayerInfoList[i].name), m_pwszDeathCalled);

		gFontFuncs.DrawSetTextFont(m_hPlayerNameFont);
		gFontFuncs.DrawSetTextPos(x, y);
		gFontFuncs.DrawSetTextColor(216, 81, 80, 255);	// colour of T.
		gFontFuncs.DrawPrintText(wszText);

		// 1. Classes
		// since we are no longer showing HP, use a bit its spaces.
		x2 = x + m_flChunkOffset * 1.5f;
		if (g_PlayerExtraInfo[i].m_iTeam == g_iTeam || g_iTeam == TEAM_SPECTATOR)
		{
			gFontFuncs.DrawSetTextPos(x2, y);
			gFontFuncs.DrawPrintText(m_rgpwcRoleNames[g_PlayerExtraInfo[i].m_iRoleType]);
		}

		// 2. HP
		// (placeholder)
		x2 += m_flChunkOffset * 0.5f;
		/*if (g_PlayerExtraInfo[i].m_iTeam == g_iTeam || g_iTeam == TEAM_SPECTATOR)
		{
			_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerExtraInfo[i].m_iHealth);
			gFontFuncs.DrawSetTextPos(x2, y);
			gFontFuncs.DrawPrintText(wszText);
		}*/

		// 3. Money
		x2 += m_flChunkOffset;
		if (g_PlayerExtraInfo[i].m_iTeam == g_iTeam || g_iTeam == TEAM_SPECTATOR)
		{
			_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerExtraInfo[i].m_iAccount);
			gFontFuncs.DrawSetTextPos(x2, y);
			gFontFuncs.DrawPrintText(wszText);
		}

		// 4. Kill
		x2 += m_flChunkOffset;
		_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerExtraInfo[i].m_iKills);
		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawPrintText(wszText);

		// 5. Deaths
		x2 += m_flChunkOffset;
		_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerExtraInfo[i].m_iDeaths);
		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawPrintText(wszText);

		// 6. KDA
		x2 += m_flChunkOffset;
		_snwprintf(wszText, wcharsmax(wszText), L"%.2f", float(g_PlayerExtraInfo[i].m_iKills) / float(Q_max(short(1), g_PlayerExtraInfo[i].m_iDeaths)));
		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawPrintText(wszText);

		// 7. PING
		x2 += m_flChunkOffset;
		if (g_PlayerInfoList[i].ping > 0)
			_snwprintf(wszText, wcharsmax(wszText), L"%d", g_PlayerInfoList[i].ping);
		else
			wcsncpy_s(wszText, L"Local", 6);

		gFontFuncs.DrawSetTextPos(x2, y);
		gFontFuncs.DrawPrintText(wszText);
	}

	// Observers
	y += iTextHeight * 2;

	gFontFuncs.DrawSetTextFont(m_hPlayerNameFont);
	gFontFuncs.DrawSetTextPos(x, y);
	gFontFuncs.DrawSetTextColor(255, 255, 255, 255);
	gFontFuncs.DrawPrintText(m_pwszTeamName[TEAM_SPECTATOR]);

	for (int i = 1; i <= MAX_PLAYERS; i++)
	{
		if (!IsConnected(i))
			continue;

		if (g_PlayerExtraInfo[i].m_iTeam == TEAM_CT || g_PlayerExtraInfo[i].m_iTeam == TEAM_TERRORIST)
			continue;	// all the rest is called observers.

		gFontFuncs.GetTextSize(m_hPlayerNameFont, wszText, &iTextWidth, &iTextHeight);
		y += iTextHeight + 2;

		wcsncpy_s(wszText, UTF8ToUnicode(g_PlayerInfoList[i].name), MAX_PLAYER_NAME_LENGTH);

		gFontFuncs.DrawSetTextFont(m_hPlayerNameFont);
		gFontFuncs.DrawSetTextPos(x, y);
		gFontFuncs.DrawSetTextColor(255, 255, 255, 255);
		gFontFuncs.DrawPrintText(wszText);
	}

	return 1;
}

void CHudScoreboard::Think(void)
{
	// Dead or in intermission? Shore scoreboard, too
	if (in_score.state & 3 || gHUD::m_bIntermission)
		m_bitsFlags |= HUD_ACTIVE;
	else
		m_bitsFlags &= ~HUD_ACTIVE;

	m_Baseboard.Think();
}
