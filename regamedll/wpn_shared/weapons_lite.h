/*

Created Date: Apr 30 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#include <stdlib.h>
#include <memory>
#include <array>
#include "../common/mathlib.h"
#include "../dlls/vector.h"


class CBaseWeaponInterface : public std::enable_shared_from_this<CBaseWeaponInterface>
{
public:
	typedef std::shared_ptr<CBaseWeaponInterface> PTR;

public:
	unsigned		m_bitsFlags	{ 0U };

public:	// basic logic funcs
	virtual void	Think(void) {}							// called by PlayerPreThink.
	virtual bool	AddToPlayer(void* pPlayer) { return false; }			// should only be called by CBasePlayer::AddPlayerItem();
	virtual bool	Deploy(void) { return false; }			// called when attempting to take it out.
	virtual void	PostFrame(void) {}						// called by PlayerPostThink.
	virtual void	PrimaryAttack(void) {}					// IN_MOUSE1
	virtual void	SecondaryAttack(void) {}				// IN_MOUSE2
	virtual void	WeaponIdle(void) {}						// constantly called when nothing else to do.
	virtual bool	Reload(void) { return false; }			// you know what it is, right?
	virtual bool	Melee(void) { return false; }			// quick knife.
	virtual bool	QuickThrowStart(EquipmentIdType iId) {}	// quick grenade (maybe something else in the future?).
	virtual bool	QuickThrowRelease(void) {}				// triggered when +qtg button released.
	virtual bool	AlterAct(void) { return false; }		// special use. for instance, XM8 "morph".
	virtual bool	HolsterStart(void) {}					// play holster anim, initialize holstering.
	virtual void	Holstered(void) {}						// majorlly reset the weapon data. no visual stuff.
	virtual	void	DashStart(void) { m_bitsFlags |= WPNSTATE_DASHING; }		// called when system thinks it's time to dash.
	virtual void	DashEnd(void) { m_bitsFlags &= ~WPNSTATE_DASHING; }			// called when system thinks it's time to stop dash.
	virtual bool	Drop(void** ppWeaponBoxReturned = nullptr) {}		// called when attempting to drop it on ground. ppWeaponBoxReturned is the CWeaponBox to be returned. (NOT avaliable on client side.)
	virtual bool	Kill(void) {};							// called when attempting to remove it from your inventory.
};

class CBaseWpnCmpt : public std::enable_shared_from_this<CBaseWpnCmpt>
{
public:
	typedef std::shared_ptr<CBaseWpnCmpt> PTR;

private:
	CBaseWpnCmpt() = default;

public:
	CBaseWpnCmpt(const CBaseWpnCmpt& s) = default;
	CBaseWpnCmpt(CBaseWpnCmpt&& s) = default;
	CBaseWpnCmpt& operator=(const CBaseWpnCmpt& s) = default;
	CBaseWpnCmpt& operator=(CBaseWpnCmpt&& s) = default;
	virtual ~CBaseWpnCmpt() {}

protected:
	CBaseWeaponInterface::PTR m_pWeapon	{ nullptr };

public:
	template <class CComponent>
	static std::shared_ptr<CComponent> Create(CBaseWeaponInterface::PTR pWeapon)
	{
		std::shared_ptr<CComponent> pComponent = std::make_shared<CComponent>();
		pComponent->m_pWeapon = pWeapon;

		return pComponent;
	}
};

template <	class CWeapon,
			class ...CComponents
>
class CBaseWeaponTemplate : public CBaseWeaponInterface
{
public:
	std::array<CBaseWpnCmpt::PTR, sizeof...(CComponents)> m_rgpComponents;

public:
	virtual void	SecondaryAttack(void)
	{
		for (auto& pComponent : m_rgpComponents)
		{
			pComponent->SecondaryAttack();
		}
	}
};

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
	float	m_flNextAttack{ 0.0f };
	int		m_iClip{ 0 };

public:
	void Fire() {}
};
