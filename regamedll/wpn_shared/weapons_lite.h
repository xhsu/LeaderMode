/*

Created Date: Apr 30 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#include <stdlib.h>
#include "../common/mathlib.h"
#include "../dlls/vector.h"

class CBaseWeaponLite
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

public:	// Construct & Destruct
	CBaseWeaponLite() = default;
	CBaseWeaponLite(const CBaseWeaponLite& s) = default;
	CBaseWeaponLite(CBaseWeaponLite&& s) = default;
	CBaseWeaponLite& operator=(const CBaseWeaponLite& s) = default;
	CBaseWeaponLite& operator=(CBaseWeaponLite&& s) = default;
	virtual ~CBaseWeaponLite() {}

public:	// Members
	float	m_flNextAttack;

public:
	void Fire() {}
};
