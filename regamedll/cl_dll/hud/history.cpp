/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"

/////////////////////
// WeaponsResource //
/////////////////////

WEAPON* gpActiveSel;
WEAPON* gpLastSel;

WeaponsResource gWR;

int g_weaponselect = 0;

void WeaponsResource::LoadAllWeaponSprites(void)
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if (rgWeapons[i].iId)
			LoadWeaponSprites(&rgWeapons[i]);
	}
}

int WeaponsResource::CountAmmo(int iId)
{
	if (iId < 0)
		return 0;

	return riAmmo[iId];
}

int WeaponsResource::HasAmmo(WEAPON* p)
{
	if (!p)
		return FALSE;

	if (p->iMax1 == -1)
		return TRUE;

	return (p->iAmmoType == -1) || p->iClip > 0 || CountAmmo(p->iAmmoType) || CountAmmo(p->iAmmo2Type) || (p->iFlags & WEAPON_FLAGS_SELECTONEMPTY);
}

void WeaponsResource::LoadWeaponSprites(WEAPON* pWeapon)
{
	int i, iRes;

	if (ScreenWidth < 640)
		iRes = 320;
	else
		iRes = 640;

	char sz[128];

	if (!pWeapon)
		return;

	Q_memset(&pWeapon->rcActive, 0, sizeof(wrect_t));
	Q_memset(&pWeapon->rcInactive, 0, sizeof(wrect_t));
	Q_memset(&pWeapon->rcAmmo, 0, sizeof(wrect_t));
	Q_memset(&pWeapon->rcAmmo2, 0, sizeof(wrect_t));

	pWeapon->hInactive = 0;
	pWeapon->hActive = 0;
	pWeapon->hAmmo = 0;
	pWeapon->hAmmo2 = 0;

	if (Q_strlen(pWeapon->szExtraName))
		Q_snprintf(sz, charsmax(sz), "sprites/%s.txt", pWeapon->szExtraName);
	else
		Q_snprintf(sz, charsmax(sz), "sprites/%s.txt", pWeapon->szName);

	client_sprite_t* pList = gEngfuncs.pfnSPR_GetList(sz, &i);

	if (!pList)
		return;

	client_sprite_t* p = gHUD::GetSpriteList(pList, "crosshair", iRes, i);

	if (p)
	{
		Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
		pWeapon->hCrosshair = gEngfuncs.pfnSPR_Load(sz);
		pWeapon->rcCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = NULL;

	p = gHUD::GetSpriteList(pList, "autoaim", iRes, i);

	if (p)
	{
		Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
		pWeapon->hAutoaim = gEngfuncs.pfnSPR_Load(sz);
		pWeapon->rcAutoaim = p->rc;
	}
	else
		pWeapon->hAutoaim = 0;

	p = gHUD::GetSpriteList(pList, "zoom", iRes, i);

	if (p)
	{
		Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedCrosshair = gEngfuncs.pfnSPR_Load(sz);
		pWeapon->rcZoomedCrosshair = p->rc;
	}
	else
	{
		pWeapon->hZoomedCrosshair = pWeapon->hCrosshair;
		pWeapon->rcZoomedCrosshair = pWeapon->rcCrosshair;
	}

	p = gHUD::GetSpriteList(pList, "zoom_autoaim", iRes, i);

	if (p)
	{
		Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedAutoaim = gEngfuncs.pfnSPR_Load(sz);
		pWeapon->rcZoomedAutoaim = p->rc;
	}
	else
	{
		pWeapon->hZoomedAutoaim = pWeapon->hZoomedCrosshair;
		pWeapon->rcZoomedAutoaim = pWeapon->rcZoomedCrosshair;
	}

	p = gHUD::GetSpriteList(pList, "weapon", iRes, i);

	if (p)
	{
		Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
		pWeapon->hInactive = gEngfuncs.pfnSPR_Load(sz);
		pWeapon->rcInactive = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hInactive = 0;

	p = gHUD::GetSpriteList(pList, "weapon_s", iRes, i);

	if (p)
	{
		Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
		pWeapon->hActive = gEngfuncs.pfnSPR_Load(sz);
		pWeapon->rcActive = p->rc;
	}
	else
		pWeapon->hActive = 0;

	p = gHUD::GetSpriteList(pList, "ammo", iRes, i);

	if (p)
	{
		Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo = gEngfuncs.pfnSPR_Load(sz);
		pWeapon->rcAmmo = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo = 0;

	p = gHUD::GetSpriteList(pList, "ammo2", iRes, i);

	if (p)
	{
		Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo2 = gEngfuncs.pfnSPR_Load(sz);
		pWeapon->rcAmmo2 = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo2 = 0;
}

WEAPON* WeaponsResource::GetFirstPos(int iSlot)
{
	WEAPON* pret = NULL;

	for (int i = 0; i < MAX_WEAPON_POSITIONS; i++)
	{
		if (rgSlots[iSlot][i])
		{
			pret = rgSlots[iSlot][i];
			break;
		}
	}

	return pret;
}

WEAPON* WeaponsResource::GetNextActivePos(int iSlot, int iSlotPos)
{
	if (iSlotPos >= MAX_WEAPON_POSITIONS || iSlot >= MAX_WEAPON_SLOTS)
		return NULL;

	WEAPON* p = gWR.rgSlots[iSlot][iSlotPos + 1];

	if (!p)
		return GetNextActivePos(iSlot, iSlotPos + 1);

	return p;
}

hSprite* WeaponsResource::GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect)
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if (rgWeapons[i].iAmmoType == iAmmoId)
		{
			rect = rgWeapons[i].rcAmmo;
			return &rgWeapons[i].hAmmo;
		}
		else if (rgWeapons[i].iAmmo2Type == iAmmoId)
		{
			rect = rgWeapons[i].rcAmmo2;
			return &rgWeapons[i].hAmmo2;
		}
	}

	return NULL;
}

void WeaponsResource::SelectSlot(int iSlot)
{
	if (iSlot > MAX_WEAPON_SLOTS)
		return;

	if (gHUD::m_fPlayerDead || gHUD::m_bitsHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL))
		return;

	if (!(gHUD::m_iWeaponBits & (1 << (WEAPON_SUIT))))
		return;

	WEAPON* p = NULL;
	bool fastSwitch = CVAR_GET_FLOAT("hud_fastswitch") != 0;

	if ((gpActiveSel == NULL) || (gpActiveSel == (WEAPON*)1) || (iSlot != gpActiveSel->iSlot))
	{
		gEngfuncs.pfnPlaySoundByName("common/wpn_hudon.wav", VOL_NORM);
		p = GetFirstPos(iSlot);

		if (p && fastSwitch)
		{
			WEAPON* p2 = GetNextActivePos(p->iSlot, p->iSlotPos);

			if (!p2)
			{
				gEngfuncs.pfnServerCmd(p->szName);
				g_weaponselect = p->iId;
				return;
			}
		}
	}
	else
	{
		gEngfuncs.pfnPlaySoundByName("common/wpn_moveselect.wav", VOL_NORM);

		if (gpActiveSel)
			p = GetNextActivePos(gpActiveSel->iSlot, gpActiveSel->iSlotPos);

		if (!p)
			p = GetFirstPos(iSlot);
	}

	if (!p)
	{
		if (!fastSwitch)
			gpActiveSel = (WEAPON*)1;
		else
			gpActiveSel = NULL;
	}
	else
		gpActiveSel = p;
}

/////////////////////
// HistoryResource //
/////////////////////

HistoryResource gHR;

#define AMMO_PICKUP_GAP (gHR.iHistoryGap + 5)
#define AMMO_PICKUP_PICK_HEIGHT (72 + (gHR.iHistoryGap * 2))
#define AMMO_PICKUP_HEIGHT_MAX (ScreenHeight - 100)

#define MAX_ITEM_NAME 32
int HISTORY_DRAW_TIME = 5;

struct ITEM_INFO
{
	char szName[MAX_ITEM_NAME];
	hSprite spr;
	wrect_t rect;
};

void HistoryResource::AddToHistory(int iType, int iId, int iCount)
{
	if (iType == HISTSLOT_AMMO && !iCount)
		return;

	if ((((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= MAX_HISTORY))
		iCurrentHistorySlot = 0;

	HIST_ITEM* freeslot = &rgAmmoHistory[iCurrentHistorySlot++];
	HISTORY_DRAW_TIME = CVAR_GET_FLOAT("hud_drawhistory_time");

	freeslot->type = iType;
	freeslot->iId = iId;
	freeslot->iCount = iCount;
	freeslot->DisplayTime = gHUD::m_flTime + HISTORY_DRAW_TIME;

	if (iType == HISTSLOT_AMMO)
	{
		hSprite* pSpr = gWR.GetAmmoPicFromWeapon(iId, freeslot->rc);
		freeslot->hSpr = pSpr ? *pSpr : NULL;
	}
	else if (iType == HISTSLOT_WEAP)
	{
		WEAPON* weap = gWR.GetWeapon(iId);

		if (weap)
		{
			freeslot->hSpr = weap->hInactive;
			freeslot->rc = weap->rcInactive;
		}
		else
			freeslot->hSpr = NULL;
	}
}

void HistoryResource::AddToHistory(int iType, const char* szName, int iCount)
{
	if (iType != HISTSLOT_ITEM)
		return;

	if ((((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= MAX_HISTORY))
		iCurrentHistorySlot = 0;

	HIST_ITEM* freeslot = &rgAmmoHistory[iCurrentHistorySlot++];

	int i = gHUD::GetSpriteIndex(szName);

	if (i == -1)
		return;

	freeslot->iId = i;
	freeslot->type = iType;
	freeslot->iCount = iCount;

	if (iType == HISTSLOT_AMMO)
	{
		hSprite* pSpr = gWR.GetAmmoPicFromWeapon(i, freeslot->rc);
		freeslot->hSpr = pSpr ? *pSpr : NULL;
	}
	else if (iType == HISTSLOT_WEAP)
	{
		WEAPON* weap = gWR.GetWeapon(i);

		if (weap)
		{
			freeslot->hSpr = weap->hInactive;
			freeslot->rc = weap->rcInactive;
		}
		else
			freeslot->hSpr = NULL;
	}

	HISTORY_DRAW_TIME = CVAR_GET_FLOAT("hud_drawhistory_time");
	freeslot->DisplayTime = gHUD::m_flTime + HISTORY_DRAW_TIME;
}

void HistoryResource::CheckClearHistory(void)
{
	for (int i = 0; i < MAX_HISTORY; i++)
	{
		if (rgAmmoHistory[i].type)
			return;
	}

	iCurrentHistorySlot = 0;
}

int HistoryResource::DrawAmmoHistory(float flTime)
{
	for (int i = 0; i < MAX_HISTORY; i++)
	{
		if (rgAmmoHistory[i].type)
		{
			rgAmmoHistory[i].DisplayTime = min(rgAmmoHistory[i].DisplayTime, gHUD::m_flTime + HISTORY_DRAW_TIME);

			if (rgAmmoHistory[i].DisplayTime <= flTime)
			{
				Q_memset(&rgAmmoHistory[i], 0, sizeof(HIST_ITEM));
				CheckClearHistory();
			}
			else if (rgAmmoHistory[i].type == HISTSLOT_AMMO)
			{
				if (rgAmmoHistory[i].hSpr == NULL)
					return 1;

				int r, g, b;
				UnpackRGB(r, g, b, RGB_YELLOWISH);

				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				ScaleColors(r, g, b, Q_min(scale, 255.0f));

				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				int xpos = ScreenWidth - 24;

				gEngfuncs.pfnSPR_Set(rgAmmoHistory[i].hSpr, r, g, b);
				gEngfuncs.pfnSPR_DrawAdditive(0, xpos, ypos, &rgAmmoHistory[i].rc);

				gHUD::DrawHudNumberString(xpos - 10, ypos, xpos - 100, rgAmmoHistory[i].iCount, r, g, b);
			}
			else if (rgAmmoHistory[i].type == HISTSLOT_WEAP)
			{
				if (rgAmmoHistory[i].hSpr == NULL)
					return 1;

				WEAPON* weap = gWR.GetWeapon(rgAmmoHistory[i].iId);

				if (!weap)
					return 1;

				int r, g, b;
				UnpackRGB(r, g, b, RGB_YELLOWISH);

				if (!gWR.HasAmmo(weap))
					UnpackRGB(r, g, b, RGB_REDISH);

				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				ScaleColors(r, g, b, Q_min(scale, 255.0f));

				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				int xpos = ScreenWidth - (weap->rcInactive.right - weap->rcInactive.left);
				gEngfuncs.pfnSPR_Set(rgAmmoHistory[i].hSpr, r, g, b);
				gEngfuncs.pfnSPR_DrawAdditive(0, xpos, ypos, &rgAmmoHistory[i].rc);
			}
			else if (rgAmmoHistory[i].type == HISTSLOT_ITEM)
			{
				if (!rgAmmoHistory[i].iId)
					continue;

				wrect_t rect = gHUD::GetSpriteRect(rgAmmoHistory[i].iId);

				int r, g, b;
				UnpackRGB(r, g, b, RGB_YELLOWISH);

				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				ScaleColors(r, g, b, Q_min(scale, 255.0f));

				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				int xpos = ScreenWidth - (rect.right - rect.left) - 10;

				gEngfuncs.pfnSPR_Set(gHUD::GetSprite(rgAmmoHistory[i].iId), r, g, b);
				gEngfuncs.pfnSPR_DrawAdditive(0, xpos, ypos, &rect);
			}
		}
	}

	return 1;
}
