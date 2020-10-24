/*

Created Date: Oct 24 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

// dummy.

class CPlayerLeftHand
{
public:
	// avoid the complex memset();
	void* operator new(size_t size)
	{
		return calloc(1, size);
	}
	void operator delete(void* ptr)
	{
		free(ptr);
	}
	CPlayerLeftHand() {}
	virtual ~CPlayerLeftHand() {}

public:
	enum PHASE
	{
		LH_UNUSED = 0,
		LH_DOWN,
		LH_UP,
		LH_WORKING
	};

public:
	inline CBaseWeapon* RIGHT_HAND() { return m_ppRighthand ? *m_ppRighthand : nullptr; }

public:
	void	Initialization	(CBasePlayer* pPlayer);
	void	Think			(void);
	void	PostFrame		(void);

public:
	CBasePlayer*	m_pPlayer;
	CBaseWeapon**	m_ppRighthand;
	CBaseWeapon*	m_pLefthandWeapon;
	PHASE			m_iPhase;
	float			m_flNextPhase;
};
