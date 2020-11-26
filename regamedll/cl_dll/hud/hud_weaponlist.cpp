/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

int CHudWeaponList::Init(void)
{
	gHUD::AddHudElem(this);

	Reset();

	Q_memset(&m_rghHighlightedWeaponSprite, NULL, sizeof(m_rghHighlightedWeaponSprite));
	Q_memset(&m_rghWeaponSprite, NULL, sizeof(m_rghWeaponSprite));
	Q_memset(&m_rgrcHighlightedWeaponSprite, NULL, sizeof(m_rgrcHighlightedWeaponSprite));
	Q_memset(&m_rgrcWeaponSprite, NULL, sizeof(m_rgrcWeaponSprite));
	Q_memset(&m_rgphList, NULL, sizeof(m_rgphList));
	Q_memset(&m_rgprcList, NULL, sizeof(m_rgprcList));
	Q_memset(&m_rgvecCurCoord, NULL, sizeof(m_rgvecCurCoord));
	Q_memset(&m_rgvecDestCoord, NULL, sizeof(m_rgvecDestCoord));

	m_bitsFlags = 0;
	return 1;
}

int CHudWeaponList::VidInit(void)
{
	int i;
	client_sprite_t* pList = gEngfuncs.pfnSPR_GetList("sprites/weapons.txt", &i);

	if (pList)
	{
		char sz[128];
		client_sprite_t* p = nullptr;

		for (int j = 0; j < LAST_WEAPON; j++)
		{
			if (!g_rgWpnInfo[j].m_pszInternalName || g_rgWpnInfo[j].m_pszInternalName[0] == '\0')
				continue;

			// get normal SPR
			p = gHUD::GetSpriteFromList(pList, g_rgWpnInfo[j].m_pszInternalName, 640, i);

			if (p)
			{
				Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
				m_rghWeaponSprite[j] = gEngfuncs.pfnSPR_Load(sz);
				m_rgrcWeaponSprite[j] = p->rc;
			}
			else
				m_rghWeaponSprite[j] = NULL;

			// get highlighted SPR
			Q_snprintf(sz, charsmax(sz), "%s_s", g_rgWpnInfo[j].m_pszInternalName);
			p = gHUD::GetSpriteFromList(pList, sz, 640, i);

			if (p)
			{
				Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
				m_rghHighlightedWeaponSprite[j] = gEngfuncs.pfnSPR_Load(sz);
				m_rgrcHighlightedWeaponSprite[j] = p->rc;
			}
			else
				m_rghHighlightedWeaponSprite[j] = NULL;
		}
	}

	// get font for clip/ammo display.
	m_hAmmoFont = gFontFuncs.CreateFont();
	gFontFuncs.AddGlyphSetToFont(m_hAmmoFont, "Blackadder ITC", 36, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0xFFFF);
	m_hClipFont = gFontFuncs.CreateFont();
	gFontFuncs.AddGlyphSetToFont(m_hClipFont, "Forte", 48, FW_BOLD, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0xFFFF);

	return 1;
}

int CHudWeaponList::Draw(float flTime)
{
	if (!g_pCurWeapon)
		return 0;

	if (m_iLastWpnId != g_pCurWeapon->m_iId)
	{
		// switch a weapon? play the anim.
		m_iLastWpnId = g_pCurWeapon->m_iId;
		m_iPhase = MOVING_IN;
	}

	int iTotalHeight = 0;

	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		// bind the SPR info to the pointers.
		if (m_rgiWeapons[i] <= WEAPON_NONE || m_rgiWeapons[i] >= LAST_WEAPON)
		{
			m_rgphList[i] = nullptr;
			m_rgprcList[i] = nullptr;
		}
		else if (g_pCurWeapon->m_iId == m_rgiWeapons[i])
		{
			m_rgphList[i] = &m_rghHighlightedWeaponSprite[m_rgiWeapons[i]];
			m_rgprcList[i] = &m_rgrcHighlightedWeaponSprite[m_rgiWeapons[i]];
		}
		else
		{
			m_rgphList[i] = &m_rghWeaponSprite[m_rgiWeapons[i]];
			m_rgprcList[i] = &m_rgrcWeaponSprite[m_rgiWeapons[i]];
		}

		// gap between items.
		if (iTotalHeight)
			iTotalHeight += WEAPONLIST_GAP;

		if (m_rgprcList[i] != nullptr)
			iTotalHeight += m_rgprcList[i]->bottom - m_rgprcList[i]->top;
	}

	if (!iTotalHeight)
		return 0;

	// LUNA: weaponlist would now replace the position of original Money & Clip/Ammo.
	int x, y = (ScreenHeight - iTotalHeight)/* / 2*/;

	// calculate the Dest coordinate.
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (!m_rgphList[i] || !*m_rgphList[i] || !m_rgprcList[i])
			continue;

		// x coord is the width of this SPR.
		x = ScreenWidth - (m_rgprcList[i]->right - m_rgprcList[i]->left);

		// calc of dest coord.
		m_rgvecDestCoord[i] = Vector2D(x, y);

		// if this slot is the current weapon, make it shifted out a little bit.
		if (m_rgiWeapons[i] == g_pCurWeapon->m_iId)
			m_rgvecDestCoord[i].x -= WEAPONLIST_ACTIVE_WPN_OFS;
		else	// as for the others, move it out of screen a little bit.
			m_rgvecDestCoord[i].x += WEAPONLIST_ACTIVE_WPN_OFS;

		// the global phase.
		switch (m_iPhase)
		{
		case MOVING_OUT:
			m_rgvecDestCoord[i].x = ScreenWidth;	// out of screen.
			break;

		case MOVING_IN:
		default:
			break;
		}

		// increase Y slot after slot.
		y += m_rgprcList[i]->bottom - m_rgprcList[i]->top + WEAPONLIST_GAP;
	}

	// changing alpha by HUD behaviour.
	float flTargetAlpha = 0;
	switch (m_iPhase)
	{
	case MOVING_IN:
		flTargetAlpha = 225;
		break;

	default:
		flTargetAlpha = 0;
		break;
	}

	m_flAlpha += (flTargetAlpha - m_flAlpha) * gHUD::m_flTimeDelta * WEAPONLIST_DRIFT_SPEED;

	// DRAW.
	int r, g, b;
	Vector2D vecAmmoBarCoord, vecDiff;
	Vector2D vecAmmoTextCoord, vecClipTextCoord;
	int iTextWidth = 0, iTextHeight = 0;
	wchar_t wszText[16];
	float flCardHeight, flCurHeight;
	CBaseWeapon* pWeapon;
	m_flLastY = ScreenHeight;
	m_flWidestCard = 0;

	for (size_t i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (!m_rgphList[i] || !*m_rgphList[i] || !m_rgprcList[i])
			continue;

		// find the highest (smallest) one.
		if (m_rgvecDestCoord[i].y < m_flLastY)
			m_flLastY = m_rgvecDestCoord[i].y;

		// find the widest card.
		if ((m_rgprcList[i]->right - m_rgprcList[i]->left) > m_flWidestCard)
			m_flWidestCard = m_rgprcList[i]->right - m_rgprcList[i]->left;

		pWeapon = g_rgpClientWeapons[m_rgiWeapons[i]];

		// make the current coord move to dest coord step by step.
		vecDiff = m_rgvecDestCoord[i] - m_rgvecCurCoord[i];
		m_rgvecCurCoord[i] += vecDiff * gHUD::m_flTimeDelta * WEAPONLIST_DRIFT_SPEED;

		// teleport if it's so close.
		if (vecDiff.Length() <= 0.01f)
			m_rgvecCurCoord[i] = m_rgvecDestCoord[i];

		// we should get this anyway.
		flCardHeight = m_rgprcList[i]->bottom - m_rgprcList[i]->top;

		// ammo bar & text
		if (pWeapon->m_iPrimaryAmmoType > AMMO_NONE && pWeapon->m_iPrimaryAmmoType < AMMO_MAXTYPE)
		{
			// calculate alpha for this item first.
			flTargetAlpha = pWeapon == g_pCurWeapon ? 255.0f : 0.0f;
			m_rgflFontAlpha[i] += (flTargetAlpha - m_rgflFontAlpha[i]) * gHUD::m_flTimeDelta * WEAPONLIST_DRIFT_SPEED;

			// ammo bar on the side.
			flCurHeight = (float(gPseudoPlayer.m_rgAmmo[pWeapon->m_iPrimaryAmmoType]) / float(pWeapon->m_pAmmoInfo->m_iMax)) * flCardHeight;
			vecAmmoBarCoord = m_rgvecCurCoord[i] + Vector2D(-AMMOBAR_WIDTH, flCardHeight - flCurHeight);

			UnpackRGB(r, g, b, gPseudoPlayer.m_rgAmmo[pWeapon->m_iPrimaryAmmoType] ? RGB_YELLOWISH : RGB_REDISH);
			gEngfuncs.pfnFillRGBA(vecAmmoBarCoord.x, m_rgvecCurCoord[i].y, AMMOBAR_WIDTH, flCardHeight, r, g, b, 64);

			UnpackRGB(r, g, b, gPseudoPlayer.m_rgAmmo[pWeapon->m_iPrimaryAmmoType] >= pWeapon->m_pItemInfo->m_iMaxClip ? RGB_GREENISH : RGB_REDISH);
			gEngfuncs.pfnFillRGBA(vecAmmoBarCoord.x, vecAmmoBarCoord.y, AMMOBAR_WIDTH, flCurHeight, r, g, b, 255);

			// ammo text.
			_snwprintf(wszText, sizeof(wszText), L"%d", gPseudoPlayer.m_rgAmmo[pWeapon->m_iPrimaryAmmoType]);
			gFontFuncs.GetTextSize(pWeapon->m_pItemInfo->m_iMaxClip == WEAPON_NOCLIP ? m_hClipFont : m_hAmmoFont, wszText, &iTextWidth, &iTextHeight);
			vecAmmoTextCoord = m_rgvecCurCoord[i] + Vector2D(-AMMOBAR_WIDTH - iTextWidth - WEAPONLIST_GAP, (flCardHeight - iTextHeight) / 2);

			UnpackRGB(r, g, b, gPseudoPlayer.m_rgAmmo[pWeapon->m_iPrimaryAmmoType] ? 0xFFFFFF : RGB_REDISH);
			gFontFuncs.DrawSetTextFont(pWeapon->m_pItemInfo->m_iMaxClip == WEAPON_NOCLIP ? m_hClipFont : m_hAmmoFont);	// if this weapon is directly using ammo, then we should treat ammo as clip.
			gFontFuncs.DrawSetTextPos(vecAmmoTextCoord.x, vecAmmoTextCoord.y);
			gFontFuncs.DrawSetTextColor(r, g, b, m_rgflFontAlpha[i]);
			gFontFuncs.DrawPrintText(wszText);

			// clip text
			if (pWeapon->m_pItemInfo->m_iMaxClip != WEAPON_NOCLIP)	// we must have a clip to do so.
			{
				_snwprintf(wszText, sizeof(wszText), L"%d", pWeapon->m_iClip);
				gFontFuncs.GetTextSize(m_hClipFont, wszText, &iTextWidth, &iTextHeight);

				// don't reset the coord, we need to recalculate based on it!
				vecAmmoTextCoord.x -= iTextWidth + WEAPONLIST_GAP * 5;
				vecAmmoTextCoord.y = m_rgvecCurCoord[i].y + (flCardHeight - iTextHeight) / 2;

				UnpackRGB(r, g, b, pWeapon->m_iClip ? 0xFFFFFF : RGB_REDISH);
				gFontFuncs.DrawSetTextFont(m_hClipFont);
				gFontFuncs.DrawSetTextPos(vecAmmoTextCoord.x, vecAmmoTextCoord.y);
				gFontFuncs.DrawSetTextColor(r, g, b, m_rgflFontAlpha[i]);
				gFontFuncs.DrawPrintText(wszText);
			}
		}

		// draw the item card.
		UnpackRGB(r, g, b, pWeapon->m_iClip ? 0xFFFFFFFF : RGB_REDISH);	// PURE WHITE.
		ScaleColors(r, g, b, m_flAlpha);
		gEngfuncs.pfnSPR_Set(*m_rgphList[i], r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, m_rgvecCurCoord[i].x, m_rgvecCurCoord[i].y, m_rgprcList[i]);
	}

	// the change of phases is depending on active item card.
	int iCurSlot = g_pCurWeapon->m_pItemInfo->m_iSlot;

	// almost there...
	auto len = (m_rgvecCurCoord[iCurSlot] - m_rgvecDestCoord[iCurSlot]).Length();
	if (len <= 0.01f)
	{
		m_rgvecCurCoord[iCurSlot] = m_rgvecDestCoord[iCurSlot];

		switch (m_iPhase)
		{
		case MOVING_IN:
			m_iPhase = MOVING_OUT;
			break;

		default:
			break;
		}
	}

	return 1;
}

void CHudWeaponList::Reset(void)
{
	Q_memset(&m_rgphList, NULL, sizeof(m_rgphList));
	Q_memset(&m_rgprcList, NULL, sizeof(m_rgprcList));
	Q_memset(&m_rgvecCurCoord, NULL, sizeof(m_rgvecCurCoord));
	Q_memset(&m_rgvecDestCoord, NULL, sizeof(m_rgvecDestCoord));

	m_iPhase = MOVING_IN;
	m_iLastWpnId = WEAPON_NONE;
	m_flAlpha = 0;
}

void CHudWeaponList::InitHUDData(void)
{
	Reset();

	Q_memset(&m_rgiWeapons, NULL, sizeof(m_rgiWeapons));
	m_bitsFlags &= ~HUD_ACTIVE;
}

void CHudWeaponList::MsgFunc_SetSlot(WeaponIdType iId, int iSlot)
{
	m_rgiWeapons[iSlot] = iId;

	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (m_rgiWeapons[i] > WEAPON_NONE && m_rgiWeapons[i] < LAST_WEAPON)
		{
			m_bitsFlags |= HUD_ACTIVE;
			return;
		}
	}

	m_bitsFlags &= ~HUD_ACTIVE;
}
