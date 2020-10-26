/*

Created Date: Oct 24 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

void CPlayerLeftHand::Initiation(CBasePlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	m_ppRighthand = &pPlayer->m_pActiveItem;
	m_pLefthandWeapon = nullptr;
	m_iPhase = LH_UNUSED;
	m_flNextPhase = NULL;
}

void CPlayerLeftHand::Think(void)
{
	m_flNextPhase -= gpGlobals->frametime;

	if (m_flNextPhase < -0.001)
		m_flNextPhase = -0.001;
}

void CPlayerLeftHand::PostFrame(void)
{
	if (m_pLefthandWeapon)
	{
		m_pLefthandWeapon->Think();

		if (m_pPlayer->m_flNextAttack <= 0.0f)
			m_pLefthandWeapon->PostFrame();
	}
}
