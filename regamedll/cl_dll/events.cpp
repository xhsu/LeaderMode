/*

Created Date: 08 Mar 2020

*/

#include "cl_base.h"

DECLARE_EVENT(FireAK47)
{

}

DECLARE_EVENT(FireAUG)
{

}

DECLARE_EVENT(FireAWP)
{

}

DECLARE_EVENT(FireDEAGLE)
{

}

DECLARE_EVENT(FireEliteLeft)
{

}

DECLARE_EVENT(FireEliteRight)
{

}

DECLARE_EVENT(FireFAMAS)
{

}

DECLARE_EVENT(Fire57)
{

}

DECLARE_EVENT(FireG3SG1)
{

}

DECLARE_EVENT(FireGALIL)
{

}

DECLARE_EVENT(Fireglock18)
{

}

DECLARE_EVENT(Knife)
{

}

DECLARE_EVENT(FireM249)
{

}

DECLARE_EVENT(FireM3)
{

}

DECLARE_EVENT(FireM4A1)
{

}

DECLARE_EVENT(FireMAC10)
{

}

DECLARE_EVENT(FireMP5)
{

}

DECLARE_EVENT(FireP228)
{

}

DECLARE_EVENT(FireP90)
{

}

DECLARE_EVENT(FireScout)
{

}

DECLARE_EVENT(FireSG550)
{

}

DECLARE_EVENT(FireSG552)
{

}

DECLARE_EVENT(FireTMP)
{

}

DECLARE_EVENT(FireUMP45)
{

}

DECLARE_EVENT(FireUSP)
{

}

DECLARE_EVENT(FireXM1014)
{

}

DECLARE_EVENT(CreateExplo)
{

}

DECLARE_EVENT(CreateSmoke)
{

}

DECLARE_EVENT(DecalReset)
{

}

DECLARE_EVENT(Vehicle)
{

}

void Events_Init(void)
{
	HOOK_EVENT(ak47, FireAK47);
	HOOK_EVENT(aug, FireAUG);
	HOOK_EVENT(awp, FireAWP);
	HOOK_EVENT(deagle, FireDEAGLE);
	HOOK_EVENT(elite_left, FireEliteLeft);
	HOOK_EVENT(elite_right, FireEliteRight);
	HOOK_EVENT(famas, FireFAMAS);
	HOOK_EVENT(fiveseven, Fire57);
	HOOK_EVENT(g3sg1, FireG3SG1);
	HOOK_EVENT(galil, FireGALIL);
	HOOK_EVENT(glock18, Fireglock18);
	HOOK_EVENT(knife, Knife);
	HOOK_EVENT(m249, FireM249);
	HOOK_EVENT(m3, FireM3);
	HOOK_EVENT(m4a1, FireM4A1);
	HOOK_EVENT(mac10, FireMAC10);
	HOOK_EVENT(mp5n, FireMP5);
	HOOK_EVENT(p228, FireP228);
	HOOK_EVENT(p90, FireP90);
	HOOK_EVENT(scout, FireScout);
	HOOK_EVENT(sg550, FireSG550);
	HOOK_EVENT(sg552, FireSG552);
	HOOK_EVENT(tmp, FireTMP);
	HOOK_EVENT(ump45, FireUMP45);
	HOOK_EVENT(usp, FireUSP);
	HOOK_EVENT(xm1014, FireXM1014);

	HOOK_EVENT(createexplo, CreateExplo);
	HOOK_EVENT(createsmoke, CreateSmoke);
	HOOK_EVENT(decal_reset, DecalReset);
	HOOK_EVENT(vehicle, Vehicle);
}
