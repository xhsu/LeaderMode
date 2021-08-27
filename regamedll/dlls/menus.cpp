/*

Created date: 03/01/2020

*/

#include "precompiled.h"

int g_iMenuItemCount = 0;

const int g_rgiBuyMenuClassify[] =
{
	WEAPON_NONE,
	(1 << WEAPON_GLOCK18) | (1 << WEAPON_USP) | (1 << WEAPON_ANACONDA) | (1 << WEAPON_DEAGLE) | (1 << WEAPON_FIVESEVEN) | (1 << WEAPON_M45A1),
	(1 << WEAPON_KSG12) | (1 << WEAPON_M1014)|(1<<WEAPON_AA12),
	(1 << WEAPON_MP7A1) | (1 << WEAPON_MAC10) | (1 << WEAPON_MP5N) | (1 << WEAPON_UMP45) | (1 << WEAPON_P90)|(1<<WEAPON_VECTOR),
	(1 << WEAPON_AK47) | (1 << WEAPON_M4A1) | (1 << WEAPON_SCARH) | (1 << WEAPON_XM8) | (1 << WEAPON_MK46) | (1<<WEAPON_RPD),
	(1 << WEAPON_SRS) | (1 << WEAPON_SVD) | (1 << WEAPON_AWP) | (1 << WEAPON_PSG1),
};

const char* g_rgszBuyMenuItemName[] =
{
	"",
	"Pistols",
	"Shotguns",
	"SMGs",
	"Assault Firearms",
	"Sniper Rifles",
	"Equipments",
};

#define BUYMENU_EQP_INDEX	6

///////////
//	BUY
///////////

void OpenMenu_Buy3(CBasePlayer* pPlayer)
{
	std::string szMenu;
	szMenu.clear();

	char szTitle[64];
	Q_sprintf(szTitle,	"\\rBuy Menu\n"
						"\\yRole: \\w%s\n\n", g_rgszRoleNames[pPlayer->m_iRoleType]);

	szMenu += szTitle;

	char szItem[128];
	int iAvailableCount = 0, iAffordableCount = 0;
	g_iMenuItemCount = 0;

	for (int i = 1; i <= 5; i++)	// equipments should not be included here.
	{
		iAvailableCount = 0;
		iAffordableCount = 0;

		for (int iId = 1; iId < LAST_WEAPON; iId++)
		{
			if ((1 << iId) & g_rgiBuyMenuClassify[i])	// is this iId a part of this weapon class?
			{
				if (GetCost(pPlayer->m_iRoleType, (WeaponIdType)iId) <= pPlayer->m_iAccount)
					iAffordableCount++;

				if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][iId] != WPN_F)
					iAvailableCount++;
			}
		}

		if (!iAvailableCount)
			Q_snprintf(szItem, sizeof(szItem) - 1, "\\d%d. %s - [NONE AVAILABLE]\n", ++g_iMenuItemCount, g_rgszBuyMenuItemName[i]);
		else if (!iAffordableCount)
			Q_snprintf(szItem, sizeof(szItem) - 1, "\\r%d. %s - [NONE AFFORDABLE]\n", ++g_iMenuItemCount, g_rgszBuyMenuItemName[i]);
		else
			Q_snprintf(szItem, sizeof(szItem) - 1, "\\r%d. \\w%s - \\r[\\y%d\\wAVAILABLE\\r|\\y%d\\wAFFORDABLE\\r]\n", ++g_iMenuItemCount, g_rgszBuyMenuItemName[i], iAvailableCount, iAffordableCount);

		szMenu += szItem;
	}

	// the EQUIPMENTS part.
	iAvailableCount = 0;
	iAffordableCount = 0;

	for (int iId = 1; iId < EQP_COUNT; iId++)
	{
		if (GetEquipmentPrice(pPlayer->m_iRoleType, (EquipmentIdType)iId) <= pPlayer->m_iAccount)
			iAffordableCount++;

		if (g_rgRoleEquipmentsAccessibility[pPlayer->m_iRoleType][iId] != WPN_F)
			iAvailableCount++;
	}

	if (!iAvailableCount)
		Q_snprintf(szItem, sizeof(szItem) - 1, "\\d%d. %s - [NONE AVAILABLE]\n", ++g_iMenuItemCount, g_rgszBuyMenuItemName[BUYMENU_EQP_INDEX]);
	else if (!iAffordableCount)
		Q_snprintf(szItem, sizeof(szItem) - 1, "\\r%d. %s - [NONE AFFORDABLE]\n", ++g_iMenuItemCount, g_rgszBuyMenuItemName[BUYMENU_EQP_INDEX]);
	else
		Q_snprintf(szItem, sizeof(szItem) - 1, "\\r%d. \\w%s - \\r[\\y%d\\wAVAILABLE\\r|\\y%d\\wAFFORDABLE\\r]\n", ++g_iMenuItemCount, g_rgszBuyMenuItemName[BUYMENU_EQP_INDEX], iAvailableCount, iAffordableCount);

	szMenu += szItem;

	/*const char szItem[] =
		"\n"
		"\\r1.\\w Pistols\n"
		"\\r2.\\w Shotguns\n"
		"\\r3.\\w SMGs\n"
		"\\r4.\\w Assault Firearms\n"
		"\\r5.\\w Sniper Rifle\n"
		"\\r6.\\w Equipments\n"
		"\n"
		"\\r7.\\w Auto Buy\n"
		"\\r8.\\w Rebuy\n"
		"\\r9.\\w Save Rebuy\n"
		"\n"
		"\\r0.\\w Exit\n"
		;*/

	// the function part.
	szMenu += "\n";
	szMenu += "\\r7.\\w Auto Buy\n";
	szMenu += "\\r8.\\w Rebuy\n";
	szMenu += "\\r9.\\w Save Rebuy\n";

	// never forget the exit item.
	szMenu += "\n";
	szMenu += "\\r0.\\w Exit\n";

	ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_6 | MENU_KEY_7 | MENU_KEY_8 | MENU_KEY_9, -1, szMenu);
	pPlayer->m_iMenu = Menu_Buy3;
}

bool MenuHandler_Buy3(CBasePlayer* pPlayer, int iSlot)
{
	switch (iSlot)
	{
		case BuyMenu_BuyPistols:
		{
			char szMenuText[512];
			Q_sprintf(szMenuText,	"\\rBuy Pistols\n"
									"\\yRole: \\w%s\n\n", g_rgszRoleNames[pPlayer->m_iRoleType]);

			g_iMenuItemCount = 0;
			AddMenuWeaponItem(pPlayer, WEAPON_GLOCK18, szMenuText);		// 1
			AddMenuWeaponItem(pPlayer, WEAPON_USP, szMenuText);			// 2
			AddMenuWeaponItem(pPlayer, WEAPON_ANACONDA, szMenuText);	// 3
			AddMenuWeaponItem(pPlayer, WEAPON_DEAGLE, szMenuText);		// 4
			AddMenuWeaponItem(pPlayer, WEAPON_FIVESEVEN, szMenuText);	// 5
			AddMenuWeaponItem(pPlayer, WEAPON_M45A1, szMenuText);		// 6

			Q_strlcat(szMenuText,	"\n"
									"\\r0. \\wExit\n");

			ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_6, -1, szMenuText);
			pPlayer->m_iMenu = Menu_BuyPistols;
			return true;
		}
		case BuyMenu_BuyShotguns:
		{
			char szMenuText[512];
			Q_sprintf(szMenuText,	"\\rBuy Shotguns\n"
									"\\yRole: \\w%s\n\n", g_rgszRoleNames[pPlayer->m_iRoleType]);

			g_iMenuItemCount = 0;
			AddMenuWeaponItem(pPlayer, WEAPON_KSG12, szMenuText);		// 1
			AddMenuWeaponItem(pPlayer, WEAPON_M1014, szMenuText);		// 2
			AddMenuWeaponItem(pPlayer, WEAPON_AA12,	szMenuText);		// 3

			Q_strlcat(szMenuText,	"\n"
									"\\r0. \\wExit\n");

			ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2, -1, szMenuText);
			pPlayer->m_iMenu = Menu_BuyShotguns;
			return true;
		}
		case BuyMenu_BuySMGs:
		{
			char szMenuText[512];
			Q_sprintf(szMenuText,	"\\rBuy SMGs\n"
									"\\yRole: \\w%s\n\n", g_rgszRoleNames[pPlayer->m_iRoleType]);

			g_iMenuItemCount = 0;
			AddMenuWeaponItem(pPlayer, WEAPON_MP7A1, szMenuText);	// 1
			AddMenuWeaponItem(pPlayer, WEAPON_MAC10, szMenuText);	// 2
			AddMenuWeaponItem(pPlayer, WEAPON_MP5N, szMenuText);	// 3
			AddMenuWeaponItem(pPlayer, WEAPON_UMP45, szMenuText);	// 4
			AddMenuWeaponItem(pPlayer, WEAPON_P90, szMenuText);		// 5
			AddMenuWeaponItem(pPlayer, WEAPON_VECTOR, szMenuText);	// 6

			Q_strlcat(szMenuText,	"\n"
									"\\r0. \\wExit\n");

			ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5, -1, szMenuText);
			pPlayer->m_iMenu = Menu_BuySMGs;
			return true;
		}
		case BuyMenu_BuyAssaultFirearms:
		{
			char szMenuText[512];
			Q_sprintf(szMenuText,	"\\rBuy Assault Firearms\n"
									"\\yRole: \\w%s\n\n", g_rgszRoleNames[pPlayer->m_iRoleType]);

			g_iMenuItemCount = 0;
			AddMenuWeaponItem(pPlayer, WEAPON_AK47,		szMenuText);	// 1
			AddMenuWeaponItem(pPlayer, WEAPON_M4A1,		szMenuText);	// 2
			AddMenuWeaponItem(pPlayer, WEAPON_XM8,		szMenuText);	// 3
			AddMenuWeaponItem(pPlayer, WEAPON_SCARH,	szMenuText);	// 4
			AddMenuWeaponItem(pPlayer, WEAPON_MK46,		szMenuText);	// 5
			AddMenuWeaponItem(pPlayer, WEAPON_RPD,		szMenuText);	// 6

			Q_strlcat(szMenuText,	"\n"
									"\\r0. \\wExit\n");

			ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_6 | MENU_KEY_7, -1, szMenuText);
			pPlayer->m_iMenu = Menu_BuyAssaultFirearms;
			return true;
		}
		case BuyMenu_BuySniperRifles:
		{
			char szMenuText[512];
			Q_sprintf(szMenuText,	"\\rBuy Sniper Rifles\n"
									"\\yRole: \\w%s\n\n", g_rgszRoleNames[pPlayer->m_iRoleType]);

			g_iMenuItemCount = 0;
			AddMenuWeaponItem(pPlayer, WEAPON_SRS, szMenuText);		// 1
			AddMenuWeaponItem(pPlayer, WEAPON_SVD, szMenuText);		// 2
			AddMenuWeaponItem(pPlayer, WEAPON_AWP, szMenuText);		// 3
			AddMenuWeaponItem(pPlayer, WEAPON_PSG1, szMenuText);	// 4

			Q_strlcat(szMenuText,	"\n"
									"\\r0. \\wExit\n");

			ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4, -1, szMenuText);
			pPlayer->m_iMenu = Menu_BuySniperRifle;
			return true;
		}
		case BuyMenu_BuyEquipments:
		{
			pPlayer->m_vMenuItems.clear();
			pPlayer->m_vMenuItems.resize(EQP_COUNT, EQP_NONE);
			g_iMenuItemCount = 0;

			char szMenuText[512];
			Q_sprintf(szMenuText,	"\\rBuy Equipments\n"
									"\\yRole: \\w%s\n\n", g_rgszRoleNames[pPlayer->m_iRoleType]);

			for (int i = 0; i < EQP_COUNT; i++)
				AddMenuEquipmentItem(pPlayer, (EquipmentIdType)i, szMenuText);

			Q_strlcat(szMenuText,	"\n"
									"\\r0. \\wExit\n");

			int bitsValidButtons = MENU_KEY_0;
			for (int i = 0; i < g_iMenuItemCount; i++)
				bitsValidButtons |= (1U << i);	// MENU_KEY_1 is BIT(0), so...

			ShowMenu(pPlayer, bitsValidButtons, -1, szMenuText);
			pPlayer->m_iMenu = Menu_BuyEquipments;
			return true;
		}
		case BuyMenu_AutoBuy:
		{
			pPlayer->ParseAutoBuy();
			return true;
		}
		case BuyMenu_Rebuy:
		{
			pPlayer->ParseRebuy();
			return true;
		}
		case BuyMenu_SaveRebuy:
		{
			pPlayer->SaveRebuy();
			return true;
		}
		default:
			return false;
	}
}

bool MenuHandler_BuyPistols(CBasePlayer* pl, int iSlot)
{
	CBot* pPlayer = dynamic_cast<CBot*>(pl);
	if (!pPlayer)
		return false;

	switch (iSlot)
	{
	case 1:
		return BuyWeapon(pPlayer, WEAPON_GLOCK18);

	case 2:
		return BuyWeapon(pPlayer, WEAPON_USP);

	case 3:
		return BuyWeapon(pPlayer, WEAPON_ANACONDA);

	case 4:
		return BuyWeapon(pPlayer, WEAPON_DEAGLE);

	case 5:
		return BuyWeapon(pPlayer, WEAPON_FIVESEVEN);

	case 6:
		return BuyWeapon(pPlayer, WEAPON_M45A1);

	default:
		return false;
	}
}

bool MenuHandler_BuyShotguns(CBasePlayer* pl, int iSlot)
{
	CBot* pPlayer = dynamic_cast<CBot*>(pl);
	if (!pPlayer)
		return false;

	switch (iSlot)
	{
	case 1:
		return BuyWeapon(pPlayer, WEAPON_KSG12);

	case 2:
		return BuyWeapon(pPlayer, WEAPON_M1014);

	case 3:
		return BuyWeapon(pPlayer, WEAPON_AA12);

	default:
		return false;
	}
}

bool MenuHandler_BuySMGs(CBasePlayer* pl, int iSlot)
{
	CBot* pPlayer = dynamic_cast<CBot*>(pl);
	if (!pPlayer)
		return false;

	switch (iSlot)
	{
	case 1:
		return BuyWeapon(pPlayer, WEAPON_MP7A1);

	case 2:
		return BuyWeapon(pPlayer, WEAPON_MAC10);

	case 3:
		return BuyWeapon(pPlayer, WEAPON_MP5N);

	case 4:
		return BuyWeapon(pPlayer, WEAPON_UMP45);

	case 5:
		return BuyWeapon(pPlayer, WEAPON_P90);

	case 6:
		return BuyWeapon(pPlayer, WEAPON_VECTOR);

	default:
		return false;
	}
}

bool MenuHandler_BuyAssaultFirearms(CBasePlayer* pl, int iSlot)
{
	CBot* pPlayer = dynamic_cast<CBot*>(pl);
	if (!pPlayer)
		return false;

	switch (iSlot)
	{
	case 1:
		return BuyWeapon(pPlayer, WEAPON_AK47);

	case 2:
		return BuyWeapon(pPlayer, WEAPON_M4A1);

	case 3:
		return BuyWeapon(pPlayer, WEAPON_XM8);

	case 4:
		return BuyWeapon(pPlayer, WEAPON_SCARH);

	case 5:
		return BuyWeapon(pPlayer, WEAPON_MK46);

	case 6:
		return BuyWeapon(pPlayer, WEAPON_RPD);

	default:
		return false;
	}
}

bool MenuHandler_BuySniperRifles(CBasePlayer* pl, int iSlot)
{
	CBot* pPlayer = dynamic_cast<CBot*>(pl);
	if (!pPlayer)
		return false;

	switch (iSlot)
	{
	case 1:
		return BuyWeapon(pPlayer, WEAPON_SRS);

	case 2:
		return BuyWeapon(pPlayer, WEAPON_SVD);

	case 3:
		return BuyWeapon(pPlayer, WEAPON_AWP);

	case 4:
		return BuyWeapon(pPlayer, WEAPON_PSG1);

	default:
		return false;
	}
}

bool MenuHandler_BuyEquipments(CBasePlayer* pl, int iSlot)
{
	CBot* pPlayer = dynamic_cast<CBot*>(pl);
	if (!pPlayer)
		return false;

	BuyEquipment(pPlayer, (EquipmentIdType)pPlayer->m_vMenuItems[iSlot]);
	return true;
}

////////////////////
//	Declare Role
////////////////////

void OpenMenu_DeclareRole(CBasePlayer* pPlayer)
{
	if (pPlayer->m_iTeam != CT && pPlayer->m_iTeam != TERRORIST)
	{
		UTIL_SayText(pPlayer, "#LeaderMod_IlligalDecalre");
		return;
	}

	char szTitle[128];
	Q_sprintf(szTitle,	"\\rDeclare New Role\n"
						"\\wYour current role: \\y%s\n\n", g_rgszRoleNames[pPlayer->m_iRoleType]);

	int iStart = Role_Commander;
	int iEnd = Role_Medic;
	if (pPlayer->m_iTeam == TERRORIST)
	{
		iStart = Role_Godfather;
		iEnd = Role_Arsonist;
	}

	char szBuffer[1024], szItem[128], szPlayers[128];
	Q_memset(szBuffer, NULL, sizeof(szBuffer));
	g_iMenuItemCount = 1;

	// Noobie is special.
	Q_snprintf(szItem, sizeof(szItem) - 1, "\\r1. \\w%s \\d", g_rgszRoleNames[Role_UNASSIGNED]);
	Q_strlcat(szBuffer, szItem);

	CBasePlayer* pOthers = nullptr;
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		pOthers = UTIL_PlayerByIndex(i);

		if (!pOthers || FNullEnt(pOthers) || pOthers->IsDormant())
			continue;

		if (pOthers->m_iRoleType == Role_UNASSIGNED && pOthers->m_iTeam == pPlayer->m_iTeam)	// list all noobie from same team.
		{
			Q_strlcat(szBuffer, " [");
			Q_strlcat(szBuffer, STRING(pOthers->pev->netname));
			Q_strlcat(szBuffer, "]");
		}
	}

	Q_strlcat(szBuffer, "\n");	// start a new line.

	for (int j = iStart; j <= iEnd; j++)	// the other roles.
	{
		Q_memset(szPlayers, NULL, sizeof(szPlayers));

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			pOthers = UTIL_PlayerByIndex(i);

			if (!pOthers || FNullEnt(pOthers) || pOthers->IsDormant())
				continue;

			if (pOthers->m_iRoleType == j && pOthers->m_iTeam == pPlayer->m_iTeam)	// list all noobie from same team.
			{
				Q_strlcat(szPlayers, " [");
				Q_strlcat(szPlayers, STRING(pOthers->pev->netname));
				Q_strlcat(szPlayers, "]");
			}
		}

		Q_snprintf(szItem, sizeof(szItem) - 1, Q_strlen(szPlayers) ? "\\d%d. %s" : "\\r%d. \\w%s", ++g_iMenuItemCount, g_rgszRoleNames[j]);
		Q_strlcat(szItem, szPlayers);
		Q_strlcat(szBuffer, szItem);
		Q_strlcat(szBuffer, "\n");	// start a new line.
	}

	Q_strlcat(szBuffer, "\n\\r0. \\wExit\n");
	std::string szMenu = std::string(szTitle) + std::string(szBuffer);

	ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_6, -1, szMenu);
	pPlayer->m_iMenu = Menu_DeclareRole;
}

bool MenuHandler_DeclareRole(CBasePlayer* pPlayer, int iSlot)
{
	RoleTypes iRole = Role_UNASSIGNED;

	if (iSlot == 1)
	{
		// the role is Role_UNASSIGNED.
		// nothing has to change.
	}
	else if (pPlayer->m_iTeam == CT)
	{
		iRole = (RoleTypes)(iSlot - 1);
	}
	else if (pPlayer->m_iTeam == TERRORIST)
	{
		iRole = (RoleTypes)(iSlot + 4);
	}
	else
		return false;	// something must be wrong here.

	if (iRole != Role_UNASSIGNED && CSGameRules()->HasRoleOccupied(iRole, pPlayer->m_iTeam))	// you can always decide to be a noobie.
	{
		UTIL_SayText(pPlayer, "#LeaderMod_RoleOccupied", g_rgszRoleNames[iRole], STRING(CSGameRules()->m_rgpCharacters[iRole]->pev->netname));
		return false;
	}

	if (iRole == Role_Commander)
	{
		CSGameRules()->AssignCommander(pPlayer);
	}
	else if (iRole == Role_Godfather)
	{
		CSGameRules()->AssignGodfather(pPlayer);
	}
	else
	{
		pPlayer->AssignRole(iRole);
		UTIL_SayText(pPlayer, "#LeaderMod_Got_a_Role", g_rgszRoleNames[iRole]);
	}
	
	return true;
}

/////////////////////////////
//	Team Tactical Schemes
/////////////////////////////

void OpenMenu_VoteTacticalSchemes(CBasePlayer* pPlayer)
{
	if (pPlayer->m_iTeam != CT && pPlayer->m_iTeam != TERRORIST)
		return;

	char szBuffer[1024];
	Q_snprintf(szBuffer, sizeof(szBuffer) - 1,	"\\rCurrent Team Tactical Scheme: \n\\y%s\n\n"
												"\\wVote to execute another one:\n", g_rgszTacticalSchemeNames[CSGameRules()->m_rgTeamTacticalScheme[pPlayer->m_iTeam]]);

	int rgiBallotBoxes[SCHEMES_COUNT] = { 0, 0, 0, 0, 0 };
	CSGameRules()->GetTSVoteDetail(pPlayer->m_iTeam, rgiBallotBoxes);

	char szItem[128];
	for (int i = Scheme_UNASSIGNED; i < SCHEMES_COUNT; i++)
	{
		if (i != pPlayer->m_iVotedTS)
			Q_snprintf(szItem, sizeof(szItem) - 1, rgiBallotBoxes[i] > 0 ? "\\r%d. \\w%s (\\y%d\\w people supported)\n" : "\\r%d. \\w%s\n", i + 1, g_rgszTacticalSchemeNames[i], rgiBallotBoxes[i]);
		else
			Q_snprintf(szItem, sizeof(szItem) - 1, "\\r%d. \\w%s (\\y%d\\w people supported) - \\rVOTED\n", i + 1, g_rgszTacticalSchemeNames[i], rgiBallotBoxes[i]);	// at least this player is supported.

		Q_strlcat(szBuffer, szItem);
	}

	std::string szMenu = std::string(szBuffer) + "\n\\r0. \\wExit\n";
	ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5, -1, szMenu);
	pPlayer->m_iMenu = Menu_VoteTS;
}

bool MenuHandler_VoteTacticalSchemes(CBasePlayer* pPlayer, int iSlot)
{
	TacticalSchemes iScheme = (TacticalSchemes)(iSlot - 1);

	pPlayer->m_iVotedTS = iScheme;
	UTIL_SayText(pPlayer, g_rgszTacticalSchemeDesc[iScheme]);

	return false;	// player may needs to read the desc.
}

////////////
//	UTIL
////////////

bool AddMenuWeaponItem(CBasePlayer *pPlayer, WeaponIdType iId, char *pszMenuText, size_t iMaxLength)
{
	static char szBuffer[192];
	g_iMenuItemCount++;	// start from 1.

	if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][iId] == WPN_F)
	{
		Q_sprintf(szBuffer, "\\d%d. %s - UNAVAILABLE\n", g_iMenuItemCount, g_rgWpnInfo[iId].m_pszExternalName);
		Q_strcat(pszMenuText, iMaxLength, szBuffer);
	}
	else
	{
		int iCost = GetWeaponPrice(pPlayer->m_iRoleType, iId);

		Q_sprintf(szBuffer, pPlayer->m_iAccount >= iCost ? "\\r%d. \\w%s - \\y%d\\w$" : "\\r%d. \\d%s - \\r%d\\d$", g_iMenuItemCount, g_rgWpnInfo[iId].m_pszExternalName, iCost);

		if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][iId] == WPN_D)
			Q_strlcat(szBuffer, " \\g(DISCOUNTED)\n");
		else if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][iId] == WPN_P)
			Q_strlcat(szBuffer, " \\r(PENALIZED)\n");
		else
			Q_strlcat(szBuffer, "\n");	// in C++ code, we need a closure

		Q_strcat(pszMenuText, iMaxLength, szBuffer);
	}

	return true;
}

int GetWeaponPrice(RoleTypes iRoleIndex, WeaponIdType iId)
{
	int iCost = GetWeaponInfo(iId)->m_iCost;

	switch (g_rgRoleWeaponsAccessibility[iRoleIndex][iId])
	{
		case WPN_F:
			iCost = 160001;
			break;

		case WPN_D:
			iCost /= 2;
			break;

		case WPN_P:
			iCost *= 2;
			break;

		default:
			break;
	}

	return iCost;
}

bool AddMenuEquipmentItem(CBasePlayer* pPlayer, EquipmentIdType iId, char* pszMenuText, size_t iMaxLength)
{
	static char szBuffer[192];

	if (g_rgRoleEquipmentsAccessibility[pPlayer->m_iRoleType][iId] == WPN_F)
	{
		// in this case, just hide the item which is not available to this player.
		return false;
	}
	else
	{
		g_iMenuItemCount++;	// only increase this when we sure about it.
		int iCost = GetEquipmentPrice(pPlayer->m_iRoleType, iId);

		Q_sprintf(szBuffer, pPlayer->m_iAccount >= iCost ? "\\r%d. \\w%s - \\y%d\\w$" : "\\r%d. \\d%s - \\r%d\\d$", g_iMenuItemCount, g_rgEquipmentInfo[iId].m_pszExternalName, iCost);

		if (g_rgRoleEquipmentsAccessibility[pPlayer->m_iRoleType][iId] == WPN_D)
			Q_strlcat(szBuffer, " \\g(DISCOUNTED)\n");
		else if (g_rgRoleEquipmentsAccessibility[pPlayer->m_iRoleType][iId] == WPN_P)
			Q_strlcat(szBuffer, " \\r(PENALIZED)\n");
		else
			Q_strlcat(szBuffer, "\n");	// in C++ code, we need a closure

		Q_strcat(pszMenuText, iMaxLength, szBuffer);

		// at the same time, add this item index to player's menu list.
		pPlayer->m_vMenuItems[g_iMenuItemCount] = iId;
	}

	return true;
}

int GetEquipmentPrice(RoleTypes iRoleIndex, EquipmentIdType iId)
{
	int iCost = g_rgEquipmentInfo[iId].m_iCost;

	switch (g_rgRoleEquipmentsAccessibility[iRoleIndex][iId])
	{
	case WPN_F:
		iCost = 160001;
		break;

	case WPN_D:
		iCost /= 2;
		break;

	case WPN_P:
		iCost *= 2;
		break;

	default:
		break;
	}

	return iCost;
}
