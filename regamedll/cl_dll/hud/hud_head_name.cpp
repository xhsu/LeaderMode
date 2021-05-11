/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"

GLuint CHudHeadName::m_iIdSpeaker = 0U;
int CHudHeadName::m_hHeadFont = 0;

int CHudHeadName::Draw(float flTime)
{
	if (m_iDrawType == HEADNAME_HIDE || !cl_headname->value)
		return 1;

	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_ALL) || g_iUser1)
		return 1;

	if (gHUD::m_iFOV <= 40)
		return 1;	// hide headname when we are using sniper scope.

	if (gHUD::m_flTime > m_flTimeUpdatePlayerNames)
	{
		BuildUnicodeList();
		m_flTimeUpdatePlayerNames = gHUD::m_flTime + 1.0;
	}

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (!m_rgwcsPlayerNames[i][0])
		{
			if (!g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0])
				continue;

			VGUI_LOCALISE->ConvertANSIToUnicode(g_PlayerInfoList[i].name, m_rgwcsPlayerNames[i], sizeof(m_rgwcsPlayerNames[i]));
		}

		if (g_PlayerExtraInfo[i].m_bIsDead)
			continue;

		if (m_iDrawType == HEADNAME_TEAMMATE)
		{
			if (g_PlayerExtraInfo[i].m_iTeam != g_iTeam)
				continue;
		}

		if (i != gHUD::m_iPlayerNum)
		{
			cl_entity_t* ent = gEngfuncs.GetEntityByIndex(i);

			if (!IsValidEntity(ent, false))
				continue;

			if (g_PlayerExtraInfo[i].m_bIsDead || g_PlayerExtraInfo[i].m_iHealth <= 0)
				continue;

			model_t* model = ent->model;
			Vector origin = ent->origin;
			Vector color = gHUD::GetColor(i);

			if (model)
				origin.z += Q_max((model->maxs.z - model->mins.z), 35.0f);
			else
				origin.z += 35.0;

			Vector2D screenPos;

			if (!CalcScreen(origin, screenPos))
				continue;

			// default alpha.
			float flAlpha = 192;

			if (gHUD::m_iLastFOVDiff > 0)
			{
				if (gHUD::m_iFOV >= 90)	// scoping out
					flAlpha = float(flAlpha) * (1.0f - Q_abs(gHUD::m_flDisplayedFOV - float(gHUD::m_iFOV)) / float(gHUD::m_iLastFOVDiff));
				else
					flAlpha = float(flAlpha) * (Q_abs(gHUD::m_flDisplayedFOV - float(gHUD::m_iFOV)) / float(gHUD::m_iLastFOVDiff));
			}

			// make sure the value won't overflow.
			flAlpha = Q_clamp(flAlpha, 0.0f, 255.0f) / 255.0f;

			int textWide, textTall;
			gFontFuncs::GetTextSize(m_hHeadFont, m_rgwcsPlayerNames[i], &textWide, &textTall);

			bool bSpeakerIcon = m_rgflTimeSpeakerIconHide[i] > g_flClientTime;
			bool bRoleIcon = g_PlayerExtraInfo[i].m_iRoleType != Role_UNASSIGNED;
			int iTotalLength = textWide + (bSpeakerIcon ? textTall + INTERSPACE : 0) + (bRoleIcon ? textTall + INTERSPACE : 0);
			int x = screenPos.x - iTotalLength / 2, y = screenPos.y;

			if (bRoleIcon)
			{
				gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
				gEngfuncs.pTriAPI->Brightness(1.0);

				// in order to make transparent fx on dds texture...
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				gEngfuncs.pTriAPI->CullFace(TRI_NONE);

				glColor4f(color.r, color.g, color.b, flAlpha);
				glBindTexture(GL_TEXTURE_2D, gHUD::m_Radar.m_rgiRadarIcons[g_PlayerExtraInfo[i].m_iRoleType]);
				DrawUtils::Draw2DQuad(x, y, x + textTall, y + textTall);

				x += textTall + INTERSPACE;
			}

			gFontFuncs::DrawSetTextFont(m_hHeadFont);
			gFontFuncs::DrawSetTextPos(x, y);
			gFontFuncs::DrawSetTextColor(255, 255, 255, flAlpha * 255.0);
			gFontFuncs::DrawPrintText(m_rgwcsPlayerNames[i]);

			x += textWide + INTERSPACE;

			if (bSpeakerIcon)
			{
				gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
				gEngfuncs.pTriAPI->Brightness(1.0);

				// in order to make transparent fx on dds texture...
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				gEngfuncs.pTriAPI->CullFace(TRI_NONE);

				glColor4f(1, 1, 1, flAlpha);
				glBindTexture(GL_TEXTURE_2D, m_iIdSpeaker);
				DrawUtils::Draw2DQuad(x, y, x + textTall, y + textTall);
			}
		}
	}
	
	return 1;
}

int CHudHeadName::Init(void)
{
	m_iIdSpeaker = LoadDDS("sprites/Miscellaneous/Speaker.dds");

	gHUD::AddHudElem(this);
	return 1;
}

int CHudHeadName::VidInit(void)
{
	m_bitsFlags |= HUD_ACTIVE;
	m_iDrawType = HEADNAME_TEAMMATE;
	m_rgflTimeSpeakerIconHide.fill(0.0f);

	if (!m_hHeadFont)
	{
		m_hHeadFont = gFontFuncs::CreateFont();
		gFontFuncs::AddGlyphSetToFont(m_hHeadFont, "Cambria", 20, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
		gFontFuncs::AddGlyphSetToFont(m_hHeadFont, "TW-Kai", 20, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
	}

	return 1;
}

void CHudHeadName::Reset(void)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
		m_rgwcsPlayerNames[i][0] = 0;

	m_rgflTimeSpeakerIconHide.fill(0.0f);
}

void CHudHeadName::BuildUnicodeList(void)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (!g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0])
		{
			m_rgwcsPlayerNames[i][0] = 0;
			continue;
		}

		VGUI_LOCALISE->ConvertANSIToUnicode(g_PlayerInfoList[i].name, m_rgwcsPlayerNames[i], sizeof(m_rgwcsPlayerNames[i]));
	}
}

bool CHudHeadName::IsValidEntity(cl_entity_s* pEntity, bool bCheckPVS)
{
	bool bNotInPVS = (Q_abs(gEngfuncs.GetLocalPlayer()->curstate.messagenum - pEntity->curstate.messagenum) > 15);

	if (pEntity && pEntity->model && pEntity->model->name && (!bNotInPVS || !bCheckPVS))
		return true;

	return false;
}

void CHudHeadName::MsgFunc_HeadName(int& iDrawType)
{
	m_iDrawType = iDrawType;
}
