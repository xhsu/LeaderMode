/*

Created Date: Mar 12 2020
Reincarnation Date: Nov 24 2020

*/

#include "precompiled.h"

std::array<int, 4U> g_rgiManpower = { 0, 0, 0, 0 };

int CHudScenarioStatus::Init(void)
{
	m_bitsFlags = HUD_ACTIVE;

	int iHeight = 0, iWidth = 0;

	m_iIdSpeaker = LoadDDS("texture/HUD/Items/Speaker.dds");
	m_iIdManpower = LoadDDS("texture/HUD/Items/Manpower.dds", &iWidth, &iHeight); m_flManpowerTextureRatio = float(iWidth) / float(iHeight);

	gHUD::AddHudElem(this);
	return 1;
}

int CHudScenarioStatus::VidInit(void)
{
	return 1;
}

void CHudScenarioStatus::Reset(void)
{
}

int CHudScenarioStatus::Draw(float fTime)
{
	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_HEALTH) || g_iUser1)
		return 1;

	if (gEngfuncs.IsSpectateOnly())
		return 1;

	// Start from right next to radar.
	int x = CHudRadar::RADAR_BORDER * 2 + CHudRadar::RADAR_HUD_SIZE;
	int y = CHudRadar::RADAR_BORDER;

	//
	Vector color = gHUD::GetColor(gHUD::m_iPlayerNum);

	const wchar_t* pwcsName = nullptr;
	int iTall = 0, iWidth = 0;

	for (size_t i = 0; i < MAX_CLIENTS; i++)
	{
		if (!g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0])
			continue;

		if (g_PlayerExtraInfo[i].m_iTeam != g_iTeam)
			continue;

		x = CHudRadar::RADAR_BORDER * 2 + CHudRadar::RADAR_HUD_SIZE;	// Reset X pos on each loop.

		pwcsName = UTF8ToUnicode(g_PlayerInfoList[i].name);
		gFontFuncs.GetTextSize(gHUD::m_Scoreboard.m_hPlayerNameFont, pwcsName, &iWidth, &iTall);

		// Draw class icon.
		gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
		gEngfuncs.pTriAPI->Brightness(1.0);

		// in order to make transparent fx on dds texture...
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		gEngfuncs.pTriAPI->CullFace(TRI_NONE);

		glColor4f(color.r, color.g, color.b, 1);
		glBindTexture(GL_TEXTURE_2D, gHUD::m_Radar.m_rgiRadarIcons[g_PlayerExtraInfo[i].m_iRoleType]);
		DrawUtils::Draw2DQuad(x, y, x + iTall, y + iTall);	// yeah, it's a square.

		x += iTall;	// move to the right side of that icon.

		// Draw the name text follow.
		gFontFuncs.DrawSetTextFont(gHUD::m_Scoreboard.m_hPlayerNameFont);
		gFontFuncs.DrawSetTextPos(x, y);
		gFontFuncs.DrawSetTextColor(235, 235, 235, 255);	// have to keep the text white.
		gFontFuncs.DrawPrintText(pwcsName);

		x += iWidth;	// move to the right side of that text.

		if (m_flTimeSpeakerIconHide < g_flClientTime)
		{
			gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
			gEngfuncs.pTriAPI->Brightness(1.0);

			// in order to make transparent fx on dds texture...
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			gEngfuncs.pTriAPI->CullFace(TRI_NONE);

			glColor4f(1, 1, 1, 1);
			glBindTexture(GL_TEXTURE_2D, m_iIdSpeaker);
			DrawUtils::Draw2DQuad(x, y, x + iTall, y + iTall);	// yeah, it's a square.
		}

		y += iTall;	// To the next row.
	}

	x = CHudRadar::RADAR_BORDER * 2 + CHudRadar::RADAR_HUD_SIZE;	// Reset X pos on manpower indicator.

	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);

	// in order to make transparent fx on dds texture...
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	glColor4f(1, 1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, m_iIdSpeaker);

	iTall = 32;
	iWidth = round(m_flManpowerTextureRatio * float(iTall));

	for (int i = 0; i < g_rgiManpower[g_iTeam]; i++)
	{
		DrawUtils::Draw2DQuad(x, y, x + iWidth, y + iTall);

		x += iWidth + 2;	// right shift.
	}

	return 1;
}

void CHudScenarioStatus::MsgFunc_Scenario(int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	bool wasActive = m_bitsFlags & HUD_ACTIVE;
	const char* spriteName;
	int sprIndex;
	int alpha;

	if (!READ_BYTE())
	{
		m_bitsFlags &= ~HUD_ACTIVE;
		return;
	}

	m_bitsFlags |= HUD_ACTIVE;

	spriteName = READ_STRING();
	sprIndex = gHUD::GetSpriteIndex(spriteName);

	m_hSprite = gHUD::GetSprite(sprIndex);
	m_rect = *gHUD::GetSpriteRect(sprIndex);

	alpha = READ_BYTE();

	if (alpha < MIN_ALPHA)
		alpha = MIN_ALPHA;

	m_flashAlpha = alpha;

	if (wasActive)
		alpha = m_alpha;
	else
		m_alpha = alpha;

	if (alpha == MIN_ALPHA)
		return;

	m_flashInterval = READ_SHORT() * 0.01;

	if (!m_nextFlash)
		m_nextFlash = gHUD::m_flTime;

	m_nextFlash += READ_SHORT() * 0.01;
}
