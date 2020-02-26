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

// Begin the hunt
void HuntState::OnEnter(CCSBot *me)
{
	// lurking death
	if (me->IsUsingKnife() && me->IsWellPastSafe() && !me->IsHurrying())
		me->Walk();
	else
		me->Run();

	me->StandUp();
	me->SetDisposition(CCSBot::ENGAGE_AND_INVESTIGATE);
	me->SetTask(CCSBot::SEEK_AND_DESTROY);
	me->DestroyPath();
}

// Hunt down our enemies
void HuntState::OnUpdate(CCSBot *me)
{
	// if we've been hunting for a long time, drop into Idle for a moment to
	// select something else to do
	const float huntingTooLongTime = 30.0f;
	if (gpGlobals->time - me->GetStateTimestamp() > huntingTooLongTime)
	{
		// stop being a rogue and do the scenario, since there must not be many enemies left to hunt
		me->PrintIfWatched("Giving up hunting, and being a rogue\n");
		me->SetRogue(false);
		me->Idle();
		return;
	}

	// scenario logic

	// listen for enemy noises
	if (me->ShouldInvestigateNoise())
	{
		me->InvestigateNoise();
		return;
	}

	// look around
	me->UpdateLookAround();

	// if we have reached our destination area, pick a new one
	// if our path fails, pick a new one
	if (me->GetLastKnownArea() == m_huntArea || me->UpdatePathMovement() != CCSBot::PROGRESSING)
	{
		m_huntArea = nullptr;
		float oldest = 0.0f;

		int areaCount = 0;
		const float minSize = 150.0f;
		for (auto area : TheNavAreaList)
		{
			areaCount++;

			// skip the small areas
			const Extent *extent = area->GetExtent();
			if (extent->hi.x - extent->lo.x < minSize || extent->hi.y - extent->lo.y < minSize)
				continue;

			// keep track of the least recently cleared area
			real_t age = gpGlobals->time - area->GetClearedTimestamp(me->m_iTeam - 1);
			if (age > oldest)
			{
				oldest = age;
				m_huntArea = area;
			}
		}

		// if all the areas were too small, pick one at random
		int which = RANDOM_LONG(0, areaCount - 1);

		areaCount = 0;
		for (auto area : TheNavAreaList)
		{
			m_huntArea = area;

			if (which == areaCount)
				break;

			which--;
		}

		if (m_huntArea)
		{
			// create a new path to a far away area of the map
			me->ComputePath(m_huntArea, nullptr, SAFEST_ROUTE);
		}
	}
}

// Done hunting
void HuntState::OnExit(CCSBot *me)
{
	;
}
