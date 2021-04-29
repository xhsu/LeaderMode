#include "precompiled.h"

void InstallCommands()
{
	static bool installedCommands = false;
	if (installedCommands)
		return;

	ADD_SERVER_COMMAND("perf_test", SV_LoopPerformance_f);
	ADD_SERVER_COMMAND("print_ent", SV_PrintEntities_f);

	installedCommands = true;
}

void SV_LoopPerformance_f()
{
	CCounter loopCounter;
	loopCounter.Init();

	double start, end;
	int i;

	start = loopCounter.GetCurTime();

	for (i = 0; i < 100; i++)
	{
		CBaseEntity *pSpot;
		for (pSpot = UTIL_FindEntityByString_Old(nullptr, "classname", "info_player_start"); pSpot; pSpot = UTIL_FindEntityByString_Old(pSpot, "classname", "info_player_start"))
			;

		for (pSpot = UTIL_FindEntityByString_Old(nullptr, "classname", "info_player_deathmatch"); pSpot; pSpot = UTIL_FindEntityByString_Old(pSpot, "classname", "info_player_deathmatch"))
			;

		for (pSpot = UTIL_FindEntityByString_Old(nullptr, "classname", "player"); pSpot; pSpot = UTIL_FindEntityByString_Old(pSpot, "classname", "player"))
			;

		for (pSpot = UTIL_FindEntityByString_Old(nullptr, "classname", "bodyque"); pSpot; pSpot = UTIL_FindEntityByString_Old(pSpot, "classname", "bodyque"))
			;
	}

	end = loopCounter.GetCurTime();
	CONSOLE_ECHO(" Time in old search loop %.4f\n", (end - start) * 1000.0);

	// check time new search loop
	start = loopCounter.GetCurTime();

	for (i = 0; i < 100; i++)
	{
		CBaseEntity *pSpot;
		for (pSpot = UTIL_FindEntityByClassname(nullptr, "info_player_start"); pSpot; pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_start"))
			;

		for (pSpot = UTIL_FindEntityByClassname(nullptr, "info_player_deathmatch"); pSpot; pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch"))
			;

		for (pSpot = UTIL_FindEntityByClassname(nullptr, "player"); pSpot; pSpot = UTIL_FindEntityByClassname(pSpot, "player"))
			;

		for (pSpot = UTIL_FindEntityByClassname(nullptr, "bodyque"); pSpot; pSpot = UTIL_FindEntityByClassname(pSpot, "bodyque"))
			;
	}

	end = loopCounter.GetCurTime();
	CONSOLE_ECHO(" Time in new search loop %.4f\n", (end - start) * 1000.0);
}

void SV_PrintEntities_f()
{
	for (int i = 0; i < stringsHashTable.Count(); i++)
	{
		hash_item_t *item = &stringsHashTable[i];

		if (item->pev)
		{
			UTIL_LogPrintf("Print: %s %i %p\n", STRING(stringsHashTable[i].pev->classname), ENTINDEX(ENT(item->pev)), item->pev);
		}

		for (item = stringsHashTable[i].next; item; item = item->next)
		{
			UTIL_LogPrintf("Print: %s %i %p\n", STRING(item->pev->classname), ENTINDEX(ENT(item->pev)), item->pev);
		}
	}
}
