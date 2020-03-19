/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"

int CHudHeadName::Draw(float flTime)
{
	if (m_iDrawType == HEADNAME_HIDE)
		return 1;

	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_ALL) || g_iUser1)
		return 1;

	if (gHUD::m_flTime > m_flNextBuild)
	{
		BuildUnicodeList();
		m_flNextBuild = gHUD::m_flTime + 1.0;
	}

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (!m_sUnicodes[i][0])
		{
			if (!g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0])
				continue;

			VGUI_LOCALISE->ConvertANSIToUnicode(g_PlayerInfoList[i].name, m_sUnicodes[i], sizeof(m_sUnicodes[i]));
		}

		if (g_PlayerExtraInfo[i].dead)
			continue;

		if (m_iDrawType == HEADNAME_TEAMMATE)
		{
			if (g_PlayerExtraInfo[i].m_iTeam != g_PlayerExtraInfo[gHUD::m_iPlayerNum].m_iTeam)
				continue;
		}

		if (i != gHUD::m_iPlayerNum)
		{
			cl_entity_t* ent = gEngfuncs.GetEntityByIndex(i);

			if (!IsValidEntity(ent))
				continue;

			model_t* model = ent->model;
			Vector origin = ent->origin;

			if (model)
				origin.z += Q_max((model->maxs.z - model->mins.z), 35.0f);
			else
				origin.z += 35.0;

			if (cl_headname->value)
			{
				Vector2D screenPos;

				if (!CalcScreen(origin, screenPos))
					continue;

				// default alpha.
				int iAlpha = 128;

				// when we scope in, remove the headname.
				if (gHUD::m_iFOV <= 40)
					iAlpha = 0;	// hide headname when we are using sniper scope.
				else if (gHUD::m_iLastFOVDiff > 0)
				{
					if (gHUD::m_iFOV >= 90)	// scoping out
						iAlpha = float(iAlpha) * (1.0f - Q_abs(gHUD::m_flDisplayedFOV - float(gHUD::m_iFOV)) / float(gHUD::m_iLastFOVDiff));
					else
						iAlpha = float(iAlpha) * (Q_abs(gHUD::m_flDisplayedFOV - float(gHUD::m_iFOV)) / float(gHUD::m_iLastFOVDiff));
				}

				// make sure the value won't overflow.
				iAlpha = Q_clamp(iAlpha, 0, 255);

				int textWide, textTall;
				VGUI_SURFACE->GetTextSize(m_hHeadFont, m_sUnicodes[i], textWide, textTall);
				VGUI_SURFACE->DrawSetTextPos(screenPos.x - ((textWide) / 2), screenPos.y - ((textTall) / 2));

				if (g_PlayerExtraInfo[i].m_iRoleType == Role_Commander)
					VGUI_SURFACE->DrawSetTextColor(0, 153, 255, float(iAlpha) * 1.5f);	// a little bit more obvious.
				else if (g_PlayerExtraInfo[i].m_iRoleType == Role_Godfather)
					VGUI_SURFACE->DrawSetTextColor(255, 51, 0, float(iAlpha) * 1.5f);
				else
					VGUI_SURFACE->DrawSetTextColor(255, 255, 255, iAlpha);

				for (size_t j = 0; j < wcslen(m_sUnicodes[i]); j++)
					VGUI_SURFACE->DrawUnicodeCharAdd(m_sUnicodes[i][j]);
			}
		}
	}
	
	return 1;
}

int CHudHeadName::Init(void)
{
	gHUD::AddHudElem(this);
	return 1;
}

int CHudHeadName::VidInit(void)
{
	m_bitsFlags |= HUD_ACTIVE;
	m_iDrawType = HEADNAME_TEAMMATE;

	if (!m_hHeadFont)
	{
		vgui::IScheme* pScheme = VGUI_SCHEME->GetIScheme(VGUI_SCHEME->GetScheme("ClientScheme"));

		if (pScheme)
		{
			m_hHeadFont = pScheme->GetFont("CreditsText");

			if (!m_hHeadFont)
			{
				pScheme = VGUI_SCHEME->GetIScheme(VGUI_SCHEME->GetDefaultScheme());

				if (pScheme)
					m_hHeadFont = pScheme->GetFont("CreditsFont");
			}
		}
	}

	return 1;
}

void CHudHeadName::Reset(void)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
		m_sUnicodes[i][0] = 0;
}

void CHudHeadName::BuildUnicodeList(void)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (!g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0])
		{
			m_sUnicodes[i][0] = 0;
			continue;
		}

		VGUI_LOCALISE->ConvertANSIToUnicode(g_PlayerInfoList[i].name, m_sUnicodes[i], sizeof(m_sUnicodes[i]));
	}
}

bool CHudHeadName::IsValidEntity(cl_entity_s* pEntity)
{
	bool bNotInPVS = (Q_abs(gEngfuncs.GetLocalPlayer()->curstate.messagenum - pEntity->curstate.messagenum) > 15);

	if (pEntity && pEntity->model && pEntity->model->name && !bNotInPVS)
		return true;

	return false;
}

void CHudHeadName::MsgFunc_HeadName(int& iDrawType)
{
	m_iDrawType = iDrawType;
}
