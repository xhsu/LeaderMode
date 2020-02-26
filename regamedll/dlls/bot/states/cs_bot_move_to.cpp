/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#include "precompiled.h"

// Move to a potentially far away position.
void MoveToState::OnEnter(CCSBot *me)
{
	if (me->IsUsingKnife() && me->IsWellPastSafe() && !me->IsHurrying())
	{
		me->Walk();
	}
	else
	{
		me->Run();
	}

	// if we need to find the bomb, get there as quick as we can
	RouteType route;
	switch (me->GetTask())
	{
	case CCSBot::MOVE_TO_LAST_KNOWN_ENEMY_POSITION:
		route = FASTEST_ROUTE;
		break;

	default:
		route = SAFEST_ROUTE;
		break;
	}

	// build path to, or nearly to, goal position
	me->ComputePath(TheNavAreaGrid.GetNavArea(&m_goalPosition), &m_goalPosition, route);

	m_radioedPlan = false;
	m_askedForCover = false;
}

// Move to a potentially far away position.
void MoveToState::OnUpdate(CCSBot *me)
{
	// assume that we are paying attention and close enough to know our enemy died
	if (me->GetTask() == CCSBot::MOVE_TO_LAST_KNOWN_ENEMY_POSITION)
	{
		// TODO: Account for reaction time so we take some time to realized the enemy is dead
		CBaseEntity *pVictim = me->GetTaskEntity();
		if (!pVictim || !pVictim->IsAlive())
		{
			me->PrintIfWatched("The enemy I was chasing was killed - giving up.\n");
			me->Idle();
			return;
		}
	}

	// look around
	me->UpdateLookAround();

	// Scenario logic
	switch (TheCSBots()->GetScenario())
	{
	default:
		break;
	}

	if (me->UpdatePathMovement() != CCSBot::PROGRESSING)
	{
		// reached destination
		switch (me->GetTask())
		{
		case CCSBot::MOVE_TO_LAST_KNOWN_ENEMY_POSITION:
		{
			CBaseEntity *pVictim = me->GetTaskEntity();
			if (pVictim && pVictim->IsAlive())
			{
				// if we got here and haven't re-acquired the enemy, we lost him
				me->GetChatter()->Say("LostEnemy");
			}
			break;
		}
		}

		// default behavior when destination is reached
		me->Idle();
		return;
	}
}

void MoveToState::OnExit(CCSBot *me)
{
	// reset to run in case we were walking near our goal position
	me->Run();
	me->SetDisposition(CCSBot::ENGAGE_AND_INVESTIGATE);
	//me->StopAiming();
}
