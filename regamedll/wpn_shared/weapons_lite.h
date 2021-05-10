/*

Created Date: Apr 30 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#include <stdlib.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
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
	virtual bool	QuickThrowStart(EquipmentIdType iId) { return false; }	// quick grenade (maybe something else in the future?).
	virtual bool	QuickThrowRelease(void) { return false; }				// triggered when +qtg button released.
	virtual bool	AlterAct(void) { return false; }		// special use. for instance, XM8 "morph".
	virtual bool	HolsterStart(void) { return false; }					// play holster anim, initialize holstering.
	virtual void	Holstered(void) {}						// majorlly reset the weapon data. no visual stuff.
	virtual	void	DashStart(void) { m_bitsFlags |= WPNSTATE_DASHING; }		// called when system thinks it's time to dash.
	virtual void	DashEnd(void) { m_bitsFlags &= ~WPNSTATE_DASHING; }			// called when system thinks it's time to stop dash.
	virtual bool	Drop(void** ppWeaponBoxReturned = nullptr) { return false; }		// called when attempting to drop it on ground. ppWeaponBoxReturned is the CWeaponBox to be returned. (NOT avaliable on client side.)
	virtual bool	Kill(void) { return true; }							// called when attempting to remove it from your inventory.

public:	// Util funcs
	virtual std::string	Info(void) const { return"CBaseWeaponInterface"; }
};

class CBaseWpnCmpt : public std::enable_shared_from_this<CBaseWpnCmpt>
{
public:
	typedef std::shared_ptr<CBaseWpnCmpt> PTR;
	typedef std::weak_ptr<CBaseWpnCmpt> W_PTR;

public:
	virtual ~CBaseWpnCmpt() { std::cout << Info() << " gets deleted!\n"; }

	virtual std::string Info() const { return "CBaseWpnCmpt"; }

protected:
	CBaseWeaponInterface* m_pWeapon	{ nullptr };

public:
	template <class CComponent>
	static std::shared_ptr<CComponent> Create(CBaseWeaponInterface* pWeapon)
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
	static_assert((std::is_base_of_v<CBaseWpnCmpt, CComponents> && ...), "'CComponents' arguments must inherit from 'CBaseWpnCmpt'.");

public:
	CBaseWeaponTemplate() : m_rgpComponents{ CBaseWpnCmpt::Create<CComponents>(this)... } { }

	friend std::ostream& operator << (std::ostream& os, CBaseWeaponTemplate const& weapon_with_attachments)
	{
		for (auto const& a : weapon_with_attachments.m_rgpComponents)
			std::cout << a->Info() << std::endl;

		return os;
	}

protected:
	std::tuple<CComponents...>		m_AttachmentTypes;
	std::vector<CBaseWpnCmpt::PTR>	m_rgpComponents;	// The existance of components depending on weapon, not vice versa.
};
