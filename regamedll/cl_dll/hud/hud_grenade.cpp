/*

Created Date: Mar 23 2020
Remastered Date: May 12 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Model		- Matoilet
	Artist		- HL&CL

*/

#include "precompiled.h"

void CHudEquipments::Initialize(void)
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

	m_hFont = gFontFuncs::CreateFont();
	gFontFuncs::AddGlyphSetToFont(m_hFont, "716", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0xFFFF);

	// Default function: Counts.
	CUSTOM_DRAW_FUNCS.fill(&DrawCount);

	// Default
	EQUIPMENT_ICONS[EQP_NONE] = LoadDDS("sprites/ClassesIcon/Doraemon.dds");

	// Armor
	EQUIPMENT_ICONS[EQP_KEVLAR] = LoadDDS("sprites/Inventory/Kevlar.dds");
	EQUIPMENT_ICONS[EQP_ASSAULT_SUIT] = LoadDDS("sprites/Inventory/AssaultSuit.dds");

	// Grenades
	EQUIPMENT_ICONS[EQP_HEGRENADE] = LoadDDS("sprites/Inventory/HE.dds");
	EQUIPMENT_ICONS[EQP_FLASHBANG] = LoadDDS("sprites/Inventory/Flashbang.dds");
	EQUIPMENT_ICONS[EQP_SMOKEGRENADE] = LoadDDS("sprites/Inventory/SmokeGrenade.dds");
	EQUIPMENT_ICONS[EQP_CRYOGRENADE] = LoadDDS("sprites/Inventory/Cryo.dds");
	EQUIPMENT_ICONS[EQP_INCENDIARY_GR] = LoadDDS("sprites/Inventory/Molotov.dds");
	EQUIPMENT_ICONS[EQP_HEALING_GR] = LoadDDS("sprites/Inventory/HealingGrenade.dds");
	EQUIPMENT_ICONS[EQP_GAS_GR] = LoadDDS("sprites/Inventory/NerveGas.dds");
	EQUIPMENT_ICONS[EQP_C4] = LoadDDS("sprites/Inventory/C4.dds");

	// Equipments
	EQUIPMENT_ICONS[EQP_DETONATOR] = LoadDDS("sprites/Inventory/Detonator.dds");
	EQUIPMENT_ICONS[EQP_NVG] = LoadDDS("sprites/Inventory/NVG.dds");
	EQUIPMENT_ICONS[EQP_FLASHLIGHT] = LoadDDS("sprites/Inventory/Flashlight.dds");
}

void CHudEquipments::Draw(float flTime, bool bIntermission)
{
	if (bIntermission)
		return;

	if (gHUD::m_bitsHideHUDDisplay & HIDEHUD_WEAPONS)
		return;

	if (gPseudoPlayer.m_iUsingGrenadeId == EQP_NONE)
		return;

	switch (m_iStage)
	{
	case MOVING_IN:
	default:
		DrawUtils::glRegularTexDrawingInit(0xFFFFFF, m_flAlpha);
		DrawUtils::glSetTexture(EQUIPMENT_ICONS[gPseudoPlayer.m_iUsingGrenadeId]);
		DrawUtils::Draw2DQuad(m_rgvSlotCurOrigin[2], m_rgvSlotCurOrigin[2] + SIZE);

		if (CUSTOM_DRAW_FUNCS[gPseudoPlayer.m_iUsingGrenadeId])
			(*CUSTOM_DRAW_FUNCS[gPseudoPlayer.m_iUsingGrenadeId])(m_rgvSlotCurOrigin[2], gPseudoPlayer.m_iUsingGrenadeId, 2);

		break;

	case MOVING_OUT:
		if (m_flNextStageIncrease < flTime)
		{
			m_iStage = MOVING_IN;
			m_flAlpha = 255;
		}

		// Current one.
		DrawUtils::glRegularTexDrawingInit(0xFFFFFF, m_rgflSlotCurAlpha[2]);	// Use this instead of m_flAlpha. Since we are selecting equipment, always highlight.
		DrawUtils::glSetTexture(EQUIPMENT_ICONS[gPseudoPlayer.m_iUsingGrenadeId]);
		DrawUtils::Draw2DQuad(m_rgvSlotCurOrigin[2], m_rgvSlotCurOrigin[2] + m_rgvSlotCurSize[2]);

		if (CUSTOM_DRAW_FUNCS[gPseudoPlayer.m_iUsingGrenadeId])
			(*CUSTOM_DRAW_FUNCS[gPseudoPlayer.m_iUsingGrenadeId])(m_rgvSlotCurOrigin[2], gPseudoPlayer.m_iUsingGrenadeId, 2);

		// Draw the next(s) first.
		EquipmentIdType iOtherEquipment = FindNextEquipment(false);
		if (iOtherEquipment != EQP_NONE)
		{
			DrawUtils::glRegularTexDrawingInit(0xFFFFFF, m_rgflSlotCurAlpha[3]);
			DrawUtils::glSetTexture(EQUIPMENT_ICONS[iOtherEquipment]);
			DrawUtils::Draw2DQuad(m_rgvSlotCurOrigin[3], m_rgvSlotCurOrigin[3] + m_rgvSlotCurSize[3]);

			if (CUSTOM_DRAW_FUNCS[iOtherEquipment])
				(*CUSTOM_DRAW_FUNCS[iOtherEquipment])(m_rgvSlotCurOrigin[3], iOtherEquipment, 3);

			// Once more.
			iOtherEquipment = FindNextEquipment(false, iOtherEquipment);

			// Draw the next 2 only if the 'next' exists!
			if (iOtherEquipment != EQP_NONE && iOtherEquipment != gPseudoPlayer.m_iUsingGrenadeId)	// You can't loop back to original equipment!
			{
				DrawUtils::glRegularTexDrawingInit(0xFFFFFF, m_rgflSlotCurAlpha[4]);
				DrawUtils::glSetTexture(EQUIPMENT_ICONS[iOtherEquipment]);
				DrawUtils::Draw2DQuad(m_rgvSlotCurOrigin[4], m_rgvSlotCurOrigin[4] + m_rgvSlotCurSize[4]);

				if (CUSTOM_DRAW_FUNCS[iOtherEquipment])
					(*CUSTOM_DRAW_FUNCS[iOtherEquipment])(m_rgvSlotCurOrigin[4], iOtherEquipment, 4);
			}
		}

		// Draw prev.
		iOtherEquipment = FindLastEquipment(false);
		if (iOtherEquipment != EQP_NONE)
		{
			DrawUtils::glRegularTexDrawingInit(0xFFFFFF, m_rgflSlotCurAlpha[1]);
			DrawUtils::glSetTexture(EQUIPMENT_ICONS[iOtherEquipment]);
			DrawUtils::Draw2DQuad(m_rgvSlotCurOrigin[1], m_rgvSlotCurOrigin[1] + m_rgvSlotCurSize[1]);

			if (CUSTOM_DRAW_FUNCS[iOtherEquipment])
				(*CUSTOM_DRAW_FUNCS[iOtherEquipment])(m_rgvSlotCurOrigin[1], iOtherEquipment, 1);

			// Once more.
			iOtherEquipment = FindLastEquipment(false, iOtherEquipment);

			// Draw the last 2 only if the 'prev' exists!
			if (iOtherEquipment != EQP_NONE && iOtherEquipment != gPseudoPlayer.m_iUsingGrenadeId)	// You can't loop back to original equipment!
			{
				DrawUtils::glRegularTexDrawingInit(0xFFFFFF, m_rgflSlotCurAlpha[0]);
				DrawUtils::glSetTexture(EQUIPMENT_ICONS[iOtherEquipment]);
				DrawUtils::Draw2DQuad(m_rgvSlotCurOrigin[0], m_rgvSlotCurOrigin[0] + m_rgvSlotCurSize[0]);

				if (CUSTOM_DRAW_FUNCS[iOtherEquipment])
					(*CUSTOM_DRAW_FUNCS[iOtherEquipment])(m_rgvSlotCurOrigin[0], iOtherEquipment, 0);
			}
		}
	}
}

void CHudEquipments::Think(void)
{
	// Wait, it is proper to do it here??
	auto iAmmoId = GetAmmoIdOfEquipment(gPseudoPlayer.m_iUsingGrenadeId);
	if ((!iAmmoId || gPseudoPlayer.m_rgAmmo[iAmmoId] <= 0) && !gPseudoPlayer.m_rgbHasEquipment[gPseudoPlayer.m_iUsingGrenadeId])
		CommandFunc_NextEquipment();

	m_flAlpha = Q_clamp<float>(m_flAlpha - gHUD::m_flUCDTimeDelta * 20.0f, MIN_ALPHA, 255);

	switch (m_iStage)
	{
	case MOVING_IN:
	default:
		m_rgvSlotCurOrigin[2] += (ANCHOR - m_rgvSlotCurOrigin[2]) * gHUD::m_flUCDTimeDelta * DRIFT_SPEED;
		for (BYTE i = 0U; i < 5U; i++)
		{
			if (i == 2)
				continue;

			m_rgvSlotCurSize[i] += (Vector2D() - m_rgvSlotCurSize[i]) * gHUD::m_flUCDTimeDelta * DRIFT_SPEED;	// Shrink to oblivion.
			m_rgflSlotCurAlpha[i] += (0.0f - m_rgflSlotCurAlpha[i]) * gHUD::m_flUCDTimeDelta * DRIFT_SPEED;	// Fade out.
		}
		break;

	case MOVING_OUT:
		for (BYTE i = 0U; i < 5U; i++)
		{
			m_rgvSlotCurOrigin[i] += (ANCHOR_FOR_EACH_SLOT[i] - m_rgvSlotCurOrigin[i]) * gHUD::m_flUCDTimeDelta * DRIFT_SPEED;
			m_rgvSlotCurSize[i] += (SIZE_FOR_EACH_SLOT[i] - m_rgvSlotCurSize[i]) * gHUD::m_flUCDTimeDelta * DRIFT_SPEED;
			m_rgflSlotCurAlpha[i] += (ALPHA_FOR_EACH_SLOT[i] - m_rgflSlotCurAlpha[i]) * gHUD::m_flUCDTimeDelta * DRIFT_SPEED;
		}
		break;
	}
}

void CHudEquipments::OnPrev(void)
{
	m_rgvSlotCurOrigin.pop_back();
	m_rgvSlotCurOrigin.insert(m_rgvSlotCurOrigin.begin(), ANCHOR_FOR_EACH_SLOT[0] - SIZE_FOR_EACH_SLOT[0]);

	m_rgflSlotCurAlpha.pop_back();
	m_rgflSlotCurAlpha.insert(m_rgflSlotCurAlpha.begin(), 0);

	m_rgvSlotCurSize.pop_back();
	m_rgvSlotCurSize.insert(m_rgvSlotCurSize.begin(), Vector2D());

	WakeUp();
}

void CHudEquipments::OnNext(void)
{
	m_rgvSlotCurOrigin.erase(m_rgvSlotCurOrigin.begin());
	m_rgvSlotCurOrigin.push_back(ANCHOR_FOR_EACH_SLOT[4] + SIZE_FOR_EACH_SLOT[4]);

	m_rgflSlotCurAlpha.erase(m_rgflSlotCurAlpha.begin());
	m_rgflSlotCurAlpha.push_back(0);

	m_rgvSlotCurSize.erase(m_rgvSlotCurSize.begin());
	m_rgvSlotCurSize.push_back(Vector2D());

	WakeUp();
}

void CHudEquipments::WakeUp(void)
{
	m_flNextStageIncrease = gHUD::m_flTime + TIME_MOVING_OUT;
	m_iStage = MOVING_OUT;
}

void CHudEquipments::Reset(void)
{
	ANCHOR = Vector2D(ScreenWidth, ScreenHeight) - MARGIN - SIZE, ANCHOR_MOVING_OUT = ANCHOR - SIZE * 2;
	ANCHOR_FOR_EACH_SLOT[0] = ANCHOR_MOVING_OUT - Vector2D(MARGIN_BETWEEN_ITEMS * 2 + SIZE_FOR_EACH_SLOT[0].width + SIZE_FOR_EACH_SLOT[1].width, 0);
	ANCHOR_FOR_EACH_SLOT[1] = ANCHOR_MOVING_OUT - Vector2D(MARGIN_BETWEEN_ITEMS + SIZE_FOR_EACH_SLOT[1].width, 0);
	ANCHOR_FOR_EACH_SLOT[2] = ANCHOR_MOVING_OUT;
	ANCHOR_FOR_EACH_SLOT[3] = ANCHOR_MOVING_OUT + Vector2D(MARGIN_BETWEEN_ITEMS + SIZE_FOR_EACH_SLOT[2].width, 0);
	ANCHOR_FOR_EACH_SLOT[4] = ANCHOR_MOVING_OUT + Vector2D(MARGIN_BETWEEN_ITEMS * 2 + SIZE_FOR_EACH_SLOT[2].width + SIZE_FOR_EACH_SLOT[3].width, 0);

	m_rgvSlotCurOrigin.clear();
	m_rgvSlotCurOrigin.resize(5, ANCHOR_MOVING_OUT);
	m_rgvSlotCurSize.clear();
	m_rgvSlotCurSize.resize(5, SIZE);
	m_rgflSlotCurAlpha.clear();
	m_rgflSlotCurAlpha.resize(5, 0);

	m_flAlpha = 255;	// byte
	m_iStage = MOVING_IN;
	m_flNextStageIncrease = 0;
}

void CHudEquipments::DrawCount(Vector2D vecOrigin, EquipmentIdType iEqpId, BYTE iSlotPos)
{
	auto iAmmoId = GetAmmoIdOfEquipment(iEqpId);	// no grenade, no drawing.
	if ((!iAmmoId || gPseudoPlayer.m_rgAmmo[iAmmoId] <= 0) && !gPseudoPlayer.m_rgbHasEquipment[iEqpId])	// Alternatively, if this is a special usable item, you can take it on your hand.
		return;

	std::wstring wsz = std::to_wstring(gPseudoPlayer.m_rgAmmo[iAmmoId]);
	int iNumberWidth = 0;
	gFontFuncs::GetTextSize(m_hFont, wsz.c_str(), &iNumberWidth, nullptr);

	// Move to the top-right corner.
	vecOrigin.x += m_rgvSlotCurSize[iSlotPos].width;
	vecOrigin.x -= iNumberWidth;

	// Apply margin
	vecOrigin.x -= MARGIN_NUMBER_IN_ICON * 2;
	vecOrigin.y += MARGIN_NUMBER_IN_ICON;

	// Draw equipment counts on the top-right of icon.
	gFontFuncs::DrawSetTextFont(m_hFont);
	gFontFuncs::DrawSetTextPos(vecOrigin);
	gFontFuncs::DrawSetTextColor(0xFFFFFF, m_iStage == MOVING_OUT ? m_rgflSlotCurAlpha[iSlotPos] : m_flAlpha);
	gFontFuncs::DrawPrintText(wsz.c_str());
}

int CHudGrenade::Init(void)
{
	gHUD::AddHudElem(this);

	Q_memset(&m_rghGrenadeIcons, NULL, sizeof(m_rghGrenadeIcons));
	Q_memset(&m_rgrcGrenadeIcons, NULL, sizeof(m_rgrcGrenadeIcons));

	m_bitsFlags = HUD_ACTIVE;
	return 1;
}

int CHudGrenade::VidInit(void)
{
	// this HUD is actually depent on gHUD::m_Ammo.
	// therefore, this HUD must be loaded AFTER gHUD::m_Ammo.

	// reset our database.
	Q_memset(&m_rghGrenadeIcons, NULL, sizeof(m_rghGrenadeIcons));
	Q_memset(&m_rgrcGrenadeIcons, NULL, sizeof(m_rgrcGrenadeIcons));

	// find ammo id first, and we can get a SPR from gHUD::m_Ammo.
	AmmoIdType iAmmoId = AMMO_NONE;
	for (int i = EQP_NONE; i < EQP_COUNT; i++)
	{
		iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);

		if (iAmmoId)
		{
			m_rghGrenadeIcons[i] = gHUD::m_rghAmmoSprite[iAmmoId];
			m_rgrcGrenadeIcons[i] = gHUD::m_rgrcAmmoSprite[iAmmoId];
		}
		else
		{
			auto p = gHUD::GetSpriteFromList(gHUD::m_pAmmoTxtList, g_rgEquipmentInfo[i].m_pszInternalName, 640, gHUD::m_iAmmoTxtListCount);
			gHUD::GetSprite(p, m_rghGrenadeIcons[i], m_rgrcGrenadeIcons[i]);
		}
	}

	return 1;
}

int CHudGrenade::Draw(float flTime)
{
	int x = ScreenWidth / 2;
	int y = ScreenHeight - 24;	// 24 is the normal height of a GR icon.

	int r = 255, g = 255, b = 255;
	int iAlpha = 255, iAlphaStep = 255;
	int iIconWidth = 24;

	AmmoIdType iAmmoId = AMMO_NONE;

	for (int i = EQP_NONE; i < EQP_COUNT; i++)
	{
		// we can't draw voidness. TODO: maybe a default icon later??
		if (!m_rghGrenadeIcons[i])
			continue;

		iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);	// no grenade, no drawing.
		if ((!iAmmoId || gPseudoPlayer.m_rgAmmo[iAmmoId] <= 0) && !gPseudoPlayer.m_rgbHasEquipment[i])	// Alternatively, if this is a special usable item, you can take it on your hand.
			continue;

		y = ScreenHeight - (m_rgrcGrenadeIcons[i].bottom - m_rgrcGrenadeIcons[i].top);	// Y is not a constant, it depents on icon.
		iIconWidth = m_rgrcGrenadeIcons[i].right - m_rgrcGrenadeIcons[i].left;

		iAlphaStep = 255 / Q_max(gPseudoPlayer.m_rgAmmo[iAmmoId], 1);	// CAREFUL! don't divide it by naught!

		for (iAlpha = 255; iAlpha > 0; iAlpha -= iAlphaStep)
		{
			UnpackRGB(r, g, b, gPseudoPlayer.m_iUsingGrenadeId == i ? 0xFFFFFF : RGB_YELLOWISH);
			ScaleColors(r, g, b, iAlpha);

			gEngfuncs.pfnSPR_Set(m_rghGrenadeIcons[i], r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &m_rgrcGrenadeIcons[i]);
			x += iIconWidth / 2;
		}

		// for the last one, give some space for next set of icons.
		x += iIconWidth;
	}

	return 1;
}
