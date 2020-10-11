#pragma once

#define GetCost	GetWeaponPrice

enum BuyMenus
{
	BuyMenu_Exit = 0,

	BuyMenu_BuyPistols = 1,
	BuyMenu_BuyShotguns,
	BuyMenu_BuySMGs,
	BuyMenu_BuyAssaultFirearms,
	BuyMenu_BuySniperRifles,
	BuyMenu_BuyEquipments,

	BuyMenu_AutoBuy,
	BuyMenu_Rebuy,
	BuyMenu_SaveRebuy,
};

void OpenMenu_Buy3(CBasePlayer* pPlayer);
bool MenuHandler_Buy3(CBasePlayer* pPlayer, int iSlot);

bool MenuHandler_BuyPistols(CBasePlayer* pPlayer, int iSlot);
bool MenuHandler_BuyShotguns(CBasePlayer* pPlayer, int iSlot);
bool MenuHandler_BuySMGs(CBasePlayer* pPlayer, int iSlot);
bool MenuHandler_BuyAssaultFirearms(CBasePlayer* pPlayer, int iSlot);
bool MenuHandler_BuySniperRifles(CBasePlayer* pPlayer, int iSlot);
bool MenuHandler_BuyEquipments(CBasePlayer* pPlayer, int iSlot);

void OpenMenu_DeclareRole(CBasePlayer* pPlayer);
bool MenuHandler_DeclareRole(CBasePlayer* pPlayer, int iSlot);

void OpenMenu_VoteTacticalSchemes(CBasePlayer* pPlayer);
bool MenuHandler_VoteTacticalSchemes(CBasePlayer* pPlayer, int iSlot);

bool AddMenuWeaponItem(CBasePlayer* pPlayer, WeaponIdType iId, char* pszMenuText, size_t iMaxLength = 512);
int GetWeaponPrice(RoleTypes iRoleIndex, WeaponIdType iId);
bool AddMenuEquipmentItem(CBasePlayer* pPlayer, EquipmentIdType iId, char* pszMenuText, size_t iMaxLength = 512);
int GetEquipmentPrice(RoleTypes iRoleIndex, EquipmentIdType iId);
