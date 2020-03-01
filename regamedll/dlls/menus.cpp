/*

Created date: 03/01/2020

*/

#include "precompiled.h"

int g_iMenuItemCount = 0;

///////////
//	BUY
///////////

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
			AddMenuWeaponItem(pPlayer, WEAPON_P99, szMenuText);			// 6

			Q_strcat(szMenuText,	"\n"
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
			AddMenuWeaponItem(pPlayer, WEAPON_STRIKER, szMenuText);		// 2

			Q_strcat(szMenuText,	"\n"
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
			AddMenuWeaponItem(pPlayer, WEAPON_PM9, szMenuText);		// 2
			AddMenuWeaponItem(pPlayer, WEAPON_MP5N, szMenuText);	// 3
			AddMenuWeaponItem(pPlayer, WEAPON_UMP45, szMenuText);	// 4
			AddMenuWeaponItem(pPlayer, WEAPON_P90, szMenuText);		// 5

			Q_strcat(szMenuText,	"\n"
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
			AddMenuWeaponItem(pPlayer, WEAPON_QBZ95,	szMenuText);	// 1
			AddMenuWeaponItem(pPlayer, WEAPON_CM901,	szMenuText);	// 2
			AddMenuWeaponItem(pPlayer, WEAPON_AK47,		szMenuText);	// 3
			AddMenuWeaponItem(pPlayer, WEAPON_M4A1,		szMenuText);	// 4
			AddMenuWeaponItem(pPlayer, WEAPON_ACR,		szMenuText);	// 5
			AddMenuWeaponItem(pPlayer, WEAPON_SCARL,	szMenuText);	// 6
			AddMenuWeaponItem(pPlayer, WEAPON_MK46,		szMenuText);	// 7

			Q_strcat(szMenuText,	"\n"
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
			AddMenuWeaponItem(pPlayer, WEAPON_M200, szMenuText);	// 1
			AddMenuWeaponItem(pPlayer, WEAPON_M14EBR, szMenuText);	// 2
			AddMenuWeaponItem(pPlayer, WEAPON_AWP, szMenuText);		// 3
			AddMenuWeaponItem(pPlayer, WEAPON_SVD, szMenuText);		// 4

			Q_strcat(szMenuText,	"\n"
									"\\r0. \\wExit\n");

			ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4, -1, szMenuText);
			pPlayer->m_iMenu = Menu_BuySniperRifle;
			return true;
		}
		case BuyMenu_BuyEquipments:
		{
			char szMenuText[512];
			Q_sprintf(szMenuText,	"\\rBuy SMGs\n"
									"\\yRole: \\w%s\n\n", g_rgszRoleNames[pPlayer->m_iRoleType]);

			g_iMenuItemCount = 2;
			Q_strcat(szMenuText, "\\r1. \\wArmour - \\y650\\w$\n");
			Q_strcat(szMenuText, "\\r2. \\wArmour & Helmet - \\y1000\\w$\n");
			AddMenuWeaponItem(pPlayer, WEAPON_HEGRENADE, szMenuText);		// 3
			AddMenuWeaponItem(pPlayer, WEAPON_FLASHBANG, szMenuText);		// 4
			AddMenuWeaponItem(pPlayer, WEAPON_SMOKEGRENADE, szMenuText);	// 5
			Q_strcat(szMenuText, "\\r6. \\wNightvision - \\y1250\\w$\n");

			Q_strcat(szMenuText,	"\n"
									"\\r0. \\wExit\n");

			ShowMenu(pPlayer, MENU_KEY_0 | MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_6, -1, szMenuText);
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

bool MenuHandler_BuyPistols(CBasePlayer* pPlayer, int iSlot)
{
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
		return BuyWeapon(pPlayer, WEAPON_P99);

	default:
		return false;
	}
}

bool MenuHandler_BuyShotguns(CBasePlayer* pPlayer, int iSlot)
{
	switch (iSlot)
	{
	case 1:
		return BuyWeapon(pPlayer, WEAPON_KSG12);

	case 2:
		return BuyWeapon(pPlayer, WEAPON_STRIKER);

	default:
		return false;
	}
}

bool MenuHandler_BuySMGs(CBasePlayer* pPlayer, int iSlot)
{
	switch (iSlot)
	{
	case 1:
		return BuyWeapon(pPlayer, WEAPON_MP7A1);

	case 2:
		return BuyWeapon(pPlayer, WEAPON_PM9);

	case 3:
		return BuyWeapon(pPlayer, WEAPON_MP5N);

	case 4:
		return BuyWeapon(pPlayer, WEAPON_UMP45);

	case 5:
		return BuyWeapon(pPlayer, WEAPON_P90);

	default:
		return false;
	}
}

bool MenuHandler_BuyAssaultFirearms(CBasePlayer* pPlayer, int iSlot)
{
	switch (iSlot)
	{
	case 1:
		return BuyWeapon(pPlayer, WEAPON_QBZ95);

	case 2:
		return BuyWeapon(pPlayer, WEAPON_CM901);

	case 3:
		return BuyWeapon(pPlayer, WEAPON_AK47);

	case 4:
		return BuyWeapon(pPlayer, WEAPON_M4A1);

	case 5:
		return BuyWeapon(pPlayer, WEAPON_ACR);

	case 6:
		return BuyWeapon(pPlayer, WEAPON_SCARL);

	case 7:
		return BuyWeapon(pPlayer, WEAPON_MK46);

	default:
		return false;
	}
}

bool MenuHandler_BuySniperRifles(CBasePlayer* pPlayer, int iSlot)
{
	switch (iSlot)
	{
	case 1:
		return BuyWeapon(pPlayer, WEAPON_M200);

	case 2:
		return BuyWeapon(pPlayer, WEAPON_M14EBR);

	case 3:
		return BuyWeapon(pPlayer, WEAPON_AWP);

	case 4:
		return BuyWeapon(pPlayer, WEAPON_SVD);

	default:
		return false;
	}
}

bool MenuHandler_BuyEquipments(CBasePlayer* pPlayer, int iSlot)
{
	switch (iSlot)
	{
	case 1:
		BuyItem(pPlayer, MENU_SLOT_ITEM_VEST);
		return false;

	case 2:
		BuyItem(pPlayer, MENU_SLOT_ITEM_VESTHELM);
		return false;

	case 3:
		return BuyWeapon(pPlayer, WEAPON_HEGRENADE);

	case 4:
		return BuyWeapon(pPlayer, WEAPON_FLASHBANG);

	case 5:
		return BuyWeapon(pPlayer, WEAPON_SMOKEGRENADE);

	case 6:
		BuyItem(pPlayer, MENU_SLOT_ITEM_NVG);
		return false;

	default:
		return false;
	}
}

////////////////////
//	Declare Role
////////////////////

void OpenMenu_DeclareRole(CBasePlayer* pPlayer)
{
	if (pPlayer->m_iTeam != CT && pPlayer->m_iTeam != TERRORIST)
	{
		UTIL_PrintChatColor(pPlayer, GREYCHAT, "/tYou can't declare your role because you are not in the game.");
		return;
	}

	char szTitle[64];
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
	Q_snprintf(szItem, sizeof(szItem) - 1, "\\r1. \\w%s\\d", g_rgszRoleNames[Role_UNASSIGNED]);
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
		UTIL_PrintChatColor(pPlayer, GREYCHAT, "/tRole /g%s/t had been choosen by others.", g_rgszRoleNames[iRole]);
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
		UTIL_PrintChatColor(pPlayer, pPlayer->m_iTeam == CT ? BLUECHAT : REDCHAT, "/gYou are now the /t%s/g!", g_rgszRoleNames[iRole]);
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
	UTIL_PrintChatColor(pPlayer, g_rgiTacticalSchemeDescColor[iScheme], g_rgszTacticalSchemeDesc[iScheme]);

	return false;	// player may needs to read the desc.
}

////////////
//	UTIL
////////////

bool AddMenuWeaponItem(CBasePlayer *pPlayer, WeaponIdType iId, char *pszMenuText)
{
	static char szBuffer[192];
	g_iMenuItemCount++;	// start from 1.

	if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][iId] == WPN_F)
	{
		Q_sprintf(szBuffer, "\\d%d. %s - UNAVAILABLE\n", g_iMenuItemCount, g_rgszWeaponAlias[iId]);
		Q_strcat(pszMenuText, szBuffer);
	}
	else
	{
		int iCost = GetPrice(pPlayer->m_iRoleType, iId);

		Q_sprintf(szBuffer, pPlayer->m_iAccount >= iCost ? "\\r%d. \\w%s - \\y%d\\w$" : "\\r%d. \\d%s - \\r%d\\d$", g_iMenuItemCount, g_rgszWeaponAlias[iId], iCost);

		if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][iId] == WPN_D)
			Q_strcat(szBuffer, " \\y(DISCOUNTED)\n");
		else if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][iId] == WPN_P)
			Q_strcat(szBuffer, " \\r(PENALIZED)\n");
		else
			Q_strcat(szBuffer, "\n");	// in C++ code, we need a closure

		Q_strcat(pszMenuText, szBuffer);
	}

	return true;
}

int GetPrice(RoleTypes iRoleIndex, WeaponIdType iId)
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
