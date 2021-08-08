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

#pragma once
#ifndef SHARED_WEAPONS_H
#define SHARED_WEAPONS_H

// debug macro
//#define RANDOM_SEED_CALIBRATION 1
//#define CHECKING_NEXT_PRIM_ATTACK_SYNC	1
#define CLIENT_PREDICT_PRIM_ATK	1
#define CLIENT_PREDICT_AIM	1
#define _DEBUG_CUSTOM_CLIENT_TO_SERVER_MESSAGE 1

// util macro
#define PRECACHE_NECESSARY_FILES(x)	PRECACHE_MODEL(x##_VIEW_MODEL);	\
									PRECACHE_MODEL(x##_WORLD_MODEL);	\
									PRECACHE_SOUND(x##_FIRE_SFX)

#ifndef CLIENT_DLL
#define DECLARE_STANDARD_RESET_MODEL_FUNC(x)	void C##x::ResetModel(void)	\
												{	\
													m_pPlayer->pev->viewmodel = MAKE_STRING(x##_VIEW_MODEL);	\
													m_pPlayer->pev->weaponmodel = MAKE_STRING(x##_WORLD_MODEL);	\
												}
#else
#define DECLARE_STANDARD_RESET_MODEL_FUNC(x)	void C##x::ResetModel(void)	\
												{	\
													g_pViewEnt->model = gEngfuncs.CL_LoadModel(x##_VIEW_MODEL, &m_pPlayer->pev->viewmodel);	\
												}
#endif

#define IS_MEMBER_PRESENTED_CPP20_W(x)		DETECT_##x<CWpn>
#define COMPILING_ERROR(error_msg)	[]<bool flag = false>()	\
										{static_assert(flag, error_msg); }()	// C++20 exclusive.
#define SCE_FIELD	static constexpr auto

#define DUMMY_VIEW_MODEL	"models/weapons/v_nothing.mdl"

class CBaseEntity;
class CBasePlayer;
class CWeaponBox;

using pcchar = const char* const;

constexpr float MAX_NORMAL_BATTERY    = 100.0f;
constexpr float MAX_DIST_RELOAD_SOUND = 512.0f;

#define ITEM_FLAG_NONE				0
#define ITEM_FLAG_SELECTONEMPTY     1
#define ITEM_FLAG_NOAUTORELOAD      2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY 4
#define ITEM_FLAG_LIMITINWORLD      8
#define ITEM_FLAG_EXHAUSTIBLE       16 // A player can totally exhaust their ammo supply and lose this weapon

#define WEAPON_IS_ONTARGET          0x40

// the maximum amount of ammo each weapon's clip can hold
#define WEAPON_NOCLIP               -1

#define LOUD_GUN_VOLUME             1000
#define NORMAL_GUN_VOLUME           600
#define QUIET_GUN_VOLUME            200

#define BRIGHT_GUN_FLASH            512
#define NORMAL_GUN_FLASH            256
#define DIM_GUN_FLASH               128

#define BIG_EXPLOSION_VOLUME        2048
#define NORMAL_EXPLOSION_VOLUME     1024
#define SMALL_EXPLOSION_VOLUME      512

#define WEAPON_ACTIVITY_VOLUME      64

// spawn flags
#define SF_DETONATE                 BIT(0) // Grenades flagged with this will be triggered when the owner calls detonateSatchelCharges

#define ARMOR_NONE		0.0f	// No armor
#define ARMOR_KEVLAR	1.0f	// Body vest only
#define ARMOR_VESTHELM	2.0f	// Vest and helmet

#ifndef CLIENT_DLL	// client.dll doesn't need these.
struct MULTIDAMAGE
{
	CBaseEntity *pEntity;
	float amount;
	int type;
};
#endif

#include "ammo.h"
#include "weapontype.h"
#include "player_classes.h"
#include "game_shared/shared_util.h"
#include "game_shared/QCScriptExt.h"
#include "public/Color.h"
#include <list>
#include <concepts>

class IWeapon
{
//-----------------------------------------------------------------------------
// Purpose:	Interface manager.
//			Managing a pure-virtual interface class.
//-----------------------------------------------------------------------------
#pragma region Interface manager.
	static inline std::list<IWeapon*> _lstWeapons;

	void* operator new(size_t size) { return calloc(1, size); }	// Generally speaking, calloc() is faster than malloc() with memset().
	void operator delete(void* ptr) { free(ptr); }

	IWeapon() noexcept { _lstWeapons.push_back(this); }
	IWeapon(const IWeapon&) = delete;
	IWeapon(IWeapon&&) = delete;
	IWeapon& operator=(const IWeapon&) = delete;
	IWeapon& operator=(IWeapon&&) = delete;

protected:
	virtual ~IWeapon()
	{
		for (auto iter = _lstWeapons.begin(); iter != _lstWeapons.end(); /* nothing */)
		{
			if (*iter == this)
				iter = _lstWeapons.erase(iter);
			else
				iter++;
		}
	}

public:

	/*
	* Purpose:	Version constant for comparison.
	*/
	static constexpr unsigned __int16 VERSION = 1U;

	/*
	* Purpose:	Free the allocated memory from the dead weapons.
	* Usage:	In a global thinking function. Better have it done after general weapon thinking, i.e. after PostFrame().
	*/
	static void		TheWeaponsThink	(void);

	/*
	* Purpose:	Spawn a weapon object, and give it to a player if applicable.
	*/
	static IWeapon*	Give			(WeaponIdType iId, void* pPlayer = nullptr, int iClip = 0, unsigned bitsFlags = 0);

#pragma endregion

//-----------------------------------------------------------------------------
// Purpose:	Behaviours.
//			These are a series of abilities a weapon able to achieve.
//-----------------------------------------------------------------------------
#pragma region Behaviours.
	/*
	* Purpose:	Will always be called every frame if this weapon is currently activated. Occurs before PostFrame() as its name indicates.
	* Usage:	Passive event.
	*/
	virtual	void	Think			(void) = 0;

	/*
	* Purpose:	Will always be called every frame by TheWeaponsThink() no matter what.
	* Usage:	Passive event. Valid until object marked as dead.
	*/
	virtual void	BackgroundFrame	(double flFrameRate) = 0;

	/*
	* Purpose:	Prepare this weapon object to be associated to another superior object.
	* Usage:	Passive event. Should be called from a Player class or WeaponBox class.
	* Return:	'true' when successfully attached.
	*/
	virtual	bool	Attach			(void* pObject) = 0;

	/*
	* Purpose:	Grab this weapon out and activate it. This will overwrite the active weapon status but does NOT trigger holster for the previously activated weapon.
	* Usage:	Try player swap function instead.
	* Return:	'true' when successfully deployed.
	*/
	virtual	bool	Deploy			(void) = 0;

	/*
	* Purpose:	Pause a weapon's animation and timing function. i.e. freeze a weapon or place a time stop magic on it.
	* Usage:	Anytime when a weapon is activated.
	* @param	[flTimeAutoResume] - How long should it automatically recovered from pause status? -1.0 or any negative number to not auto resume.
	* @param	[bEnforceUpdatePauseDatabase] - If it is already in pause, should the function enforce the data recording and overwrite the last one?
	*/
	virtual	void	Pause			(float flTimeAutoResume = -1.0f, bool bEnforceUpdatePauseDatabase = false) = 0;

	/*
	* Purpose:	Revoke the freeze status of a weapon. i.e. undo Pause() function.
	* Usage:	Anytime after a weapon is freezed.
	*/
	virtual	void	Resume			(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Anytime when a weapon is activated.
	*/
	virtual	void	PrimaryAttack	(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Anytime when a weapon is activated.
	*/
	virtual	void	Aim				(void) = 0;

	/*
	* Purpose:	The fallback passive event if a weapon just doing nothing at all.
	* Usage:	Passive event.
	*/
	virtual void	Idle			(void) = 0;

	/*
	* Purpose:	Refill or make a weapon ready to fire again.
	* Usage:	Anytime when a weapon is activated and not reloading.
	* Return:	'true' when a reloading process successfully started.
	*/
	virtual	bool	Reload			(void) = 0;

	/*
	* Purpose:	Perform a close-range combat with this weapon. This will cause a Pause() for the weapon.
	* Usage:	Anytime after this weapon is depolyed.
	* Return:	'true' when a melee attack successfully performed.
	*/
	virtual	bool	Melee			(void) = 0;

	/*
	* Purpose:	'Use' this weapon in alternative way.
	* Usage:	Anytime after this weapon is deployed.
	* Return:	'true' if its alternative ability successfully performed.
	*/
	virtual bool	AlterAct		(void) = 0;

	/*
	* Purpose:	Check whether the weapon is blocked so that it is currently malfunctioning. 
	* Usage:	Should be called in pre-phase of every frame.
	* Return:	'true' if this weapon is malfunctioning.
	*/
	virtual	bool	Blockage		(void) = 0;

	/*
	* Purpose:	Deactivate a weapon. However, this will not automatically activate another weapon.
	* Usage:	Use swap function in player class instead.
	* Return:	'true' if holstering process started for this weapon.
	*/
	virtual	bool	HolsterStart	(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Passive event. Called when a weapon is completely deactivated.
	*/
	virtual	void	Holstered		(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Passive event. Called by player class so that weapon object is shifting to dashing status.
	*/
	virtual	void	DashStart		(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Passive event. Called by a player class so that weapon object is shifting to normal status.
	*/
	virtual	void	DashEnd			(void) = 0;

	/*
	* Purpose:	Pause the weapon and move left hand out of screen.
	* Usage:	Anytime when weapon is activated.
	* Return:	'true' if successfully freed.
	*/
	virtual	bool	FreeupLeftHand	(void) = 0;

	/*
	* Purpose:	Resume the weapon and reach the left hand onto handguard again.
	* Usage:	Anytime when left hand is freed up.
	* Return:	'true' if successfully reached.
	*/
	virtual bool	ReachLeftHand	(void) = 0;

	/*
	* Purpose:	Detach this object from its owner.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' if successfully detached.
	*/
	virtual bool	Detach			(void) = 0;

	/*
	* Purpose:	Drop this weapon to ground.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' if it is successfully dropped on ground.
	*/
	virtual	bool	Drop			(void** ppWeaponBoxReturned = nullptr) = 0;

	/*
	* Purpose:	Remove this weapon object.
	* Usage:	Anytime.
	* Return:	'true' if this object is successfully removed.
	*/
	virtual	bool	Kill			(void) = 0;
#pragma endregion

//-----------------------------------------------------------------------------
// Purpose:	Resource Handling.
//-----------------------------------------------------------------------------
#pragma region Resource Handling.
	/*
	* Purpose:	Pack and compress data of this weapon.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' when successfully packed.
	*/
	virtual	bool	PackData		(void* pDatabase) = 0;

	/*
	* Purpose:	Unpack and override the data from database to local object.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' when successfully unpacked.
	*/
	virtual	bool	ParseData		(void* pDatabase) = 0;

	/*
	* Purpose:		Start a transmitting process.
	* Usage:		Anytime when this object is valid.
	* Return:		'true' when data transmitted successfully.
	* @pParameters:	This pointer will always be freed unless it is nullptr. NOT deleted, but freed.
	*/
	virtual bool	Transmit		(int iType, void* pParameters) = 0;

	/*
	* Purpose:	Precache all necessary files into buffer.
	* Usage:	Anytime. Careful for the outter game engine timing.
	*/
	virtual	void	Precache		(void) = 0;
#pragma endregion

//-----------------------------------------------------------------------------
// Purpose:	Model Handling.
//-----------------------------------------------------------------------------
#pragma region Model Handling.
	/*
	* Purpose:	Aquire information regarding model inversion.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' for mirroring model, 'false' for doing nothing.
	*/
	virtual	bool	ShouldInvertMdl	(void) = 0;

	/*
	* Purpose:	Aquire information regarding 'cl_entity_t::curstate.body' and 'edict_t::body'.
	* Usage:	Anytime when this object is valid.
	* Return:	The 'body' value, if applicable.
	*/
	virtual	int		CalcBodyParts	(void) = 0;

	/*
	* Purpose:	Aquire information for first-personal view model bobing effect.
	* Usage:	Anytime when this object is valid. Normally called before bob visual is applied.
	* Return:	The multiplier of Omega factor and multiplier of Amplitude factor. Formula: Pos = A * sine(omega * t + phi).
	*/
	virtual	void	CalcBobParam	(double& flOmegaModifier, double& flAmplitudeModifier) = 0;

	/*
	* Purpose:	Handling a studio event from model QC.
	* Usage:	Passive event. Called from engine when necessary.
	* Return:	'true' if the event is handled.
	*/
	virtual	bool	StudioEvent		(const struct mstudioevent_s* pEvent) = 0;
#pragma endregion

//-----------------------------------------------------------------------------
// Purpose:	Static Data Manager.
//-----------------------------------------------------------------------------
#pragma region Data API.
	/*
	* Purpose:	Aquire information regarding weapon type index.
	* Usage:	Anytime when this object is valid.
	* Return:	Enumerator index value.
	*/
	virtual	WeaponIdType Id					(void) = 0;

	/*
	* Purpose:	Aquire information regarding weapon general database.
	* Usage:	Anytime when this object is valid.
	* Return:	Non-modifiable data structure.
	*/
	virtual	const struct WeaponInfo* WpnInfo(void) = 0;

	/*
	* Purpose:	Aquire information regarding the ammunition this weapon is using.
	* Usage:	Anytime when this object is valid.
	* Return:	Non-modifiable data structure.
	*/
	virtual	const struct AmmoInfo* AmmoInfo	(void) = 0;
#pragma endregion

//-----------------------------------------------------------------------------
// Purpose:	Status Inquiry Functions.
//-----------------------------------------------------------------------------
#pragma region Status Inquiry APIs.
	/*
	* Purpose:	Inquire whether this object is invalided.
	* Usage:	Anytime.
	* Return:	'true' if this object is already invalid.
	*/
	virtual	bool	IsDead			(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon is currently dual wielding.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' if this weapon is currently in dual mode.
	*/
	virtual	bool	IsDualWielding	(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon is currently in dashing mode.
	* Usage:	Anytime when this object is activated.
	* Return:	'true' if the owner of this weapon is currently dashing.
	* TODO:		This should be a player function.
	*/
	virtual	bool	IsDashing		(void) = 0;

	/*
	* Purpose:	Inquire whether the player is aiming.
	* Usage:	Anytime when this object is activated.
	* Return:	'true' if the owner of this weapon is currently aiming.
	*/
	virtual bool	IsAiming		(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon is currently being use in a way that shouldn't be interrupted.
	* Usage:	Anytime when this object is activated.
	* Return:	'true' if this weapon is currently busy.
	*/
	virtual	bool	IsBusy			(void) = 0;
#pragma endregion

//-----------------------------------------------------------------------------
// Purpose:	Capability Inquiry Functions.
//-----------------------------------------------------------------------------
#pragma region Capability Inquiry APIs.
	/*
	* Purpose:	Inquire whether this weapon can be depolyed.
	* Usage:	Anytime when it is valid.
	* Return:	'true' if this object capable to do so.
	*/
	virtual	bool	CanDepoly		(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon can be holstered.
	* Usage:	Anytime when it is valid.
	* Return:	'true' if this object capable to do so.
	*/
	virtual	bool	CanHolster		(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon can be dropped.
	* Usage:	Anytime when it is valid.
	* Return:	'true' if this object capable to do so.
	*/
	virtual	bool	CanDrop			(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon can be dual wielding.
	* Usage:	Anytime when it is valid.
	* Return:	'true' if this object capable to do so.
	*/
	virtual	bool	CanDualWield	(void) = 0;
#pragma endregion

	/*
	* Purpose:	Inquire the current maxium moving speed.
	* Usage:	Anytime when it is valid.
	* Return:	Current maxium speed.
	*/
	virtual	float	GetMaxSpeed		(void) = 0;

	/*
	* Purpose:	Insert certain amount of bullets into this weapon. Exceeding bullets would placed into player inventory.
	* Usage:	Anytime when it had attached onto an holder object.
	* Return:	'true' if added successfully.
	*/
	virtual	bool	InsertAmmo		(int iCount) = 0;

	/*
	* Purpose:	Play an indexed animation from model sequence.
	* Usage:	Anytime when it is activated.
	*/
	virtual	void	Animate			(int iWhat) = 0;

	/*
	* Purpose:	Play sound effect of this firearm when its trigger is being pulled but already emptied.
	* Usage:	Anytime when it is valid.
	*/
	virtual	void	PlayEmptySound	(const Vector& vecWhere) = 0;

	/*
	* Purpose:	Play shooting sound effect of this firearm.
	* Usage:	Anytime when it is valid.
	*/
	virtual	void	PlayFireSound	(const Vector& vecWhere) = 0;

	/*
	* Purpose:	Play reloading sound effect of this firearm.
	* Usage:	Anytime when it is valid.
	*/
	virtual	void	PlayReloadSound	(const Vector& vecWhere) = 0;

	/*
	* Purpose:	Enforcing model displayment on its holder.
	* Usage:	Anytime when it is attached to an object.
	*/
	virtual	void	ResetModel		(void) = 0;

	/*
	* Purpose:	Change the varient of this weapon.
	* Usage:	Anytime when it is valid. Called when the role of holder changed or the holder itself changed.
	*/
	virtual	void	Vary			(RoleTypes iType) = 0;

	/*
	* Purpose:	Inquire current spread status.
	* Usage:	Anytime when it is valid.
	* Return:	The spread range from 0 to 1. 1 indicates spreading all across screen whereas 0 means no spread radius.
	*/
	virtual	float	GetSpread		(void) = 0;

	/*
	* Purpose:	Inquire current weapon flags.
	* Usage:	Anytime when it is valid.
	* Return:	The pointer to flag variable.
	*/
	virtual uint32*	Flags			(void) = 0;

	/*
	* Purpose:	Inquire current weapon owner. Could be a player or a weaponbox or even a nullptr.
	* Usage:	Anytime when it is valid.
	* Return:	The pointer to owner.
	*/
	virtual void*	GetOwner		(void) = 0;
};

// something can place on your hand.
class CBaseWeapon
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

protected:	// you can remove weapons only via its manager function.
	virtual ~CBaseWeapon() {}

public:
	static	void			TheWeaponsThink	(void);
	static	CBaseWeapon*	Give(WeaponIdType iId, CBasePlayer* pPlayer = nullptr, int iClip = 0, unsigned bitsFlags = 0);

public:
	static inline std::list<CBaseWeapon*>	m_lstWeapons;

public:
	WeaponIdType	m_iId					{ WEAPON_NONE };
	const WeaponInfo* m_pItemInfo			{ &g_rgWpnInfo[WEAPON_NONE] };
	const AmmoInfo* m_pAmmoInfo				{ &g_rgAmmoInfo[AMMO_NONE] };
	float			m_flNextPrimaryAttack	{ 0.0f };
	float			m_flNextSecondaryAttack	{ 0.0f };
	float			m_flTimeWeaponIdle		{ 0.0f };
	unsigned		m_bitsFlags				{ 0 };
	int				m_iClip					{ 0 };
	bool			m_bInReload : 1			{ false };
	int				m_iShotsFired			{ 0 };
	float			m_flDecreaseShotsFired	{ 0.0f };
	bool			m_bDirection : 1		{ false };
	float			m_flAccuracy			{ 0.0f };	// TODO: this should be remove later.
	float			m_flLastFire			{ 0.0f };
	AmmoIdType		m_iPrimaryAmmoType		{ AMMO_NONE };			// "primary" ammo index into players m_rgAmmo[]
	AmmoIdType		m_iSecondaryAmmoType	{ AMMO_NONE };		// "secondary" ammo index into players m_rgAmmo[]
	bool			m_bInZoom : 1			{ false };
	RoleTypes		m_iVariation			{ Role_UNASSIGNED };	// weapons suppose to variegate accroading to their owner.
	bool			m_bDelayRecovery : 1	{ false };

	struct	// this structure is for anim push and pop. it save & restore weapon state.
	{
		// on player.
		int		m_iSequence{ 0 };
		float	m_flNextAttack{ 0.0f };
		float	m_flEjectBrass{ 0.0f };
		int		m_iShellModelIndex{ 0 };

		// on weapon.
		float	m_flNextPrimaryAttack{ 0.0f };
		float	m_flNextSecondaryAttack{ 0.0f };
		float	m_flTimeWeaponIdle{ 0.0f };

#ifdef CLIENT_DLL
		// for model. these will be applied on g_pViewEnt.
		float	m_flTimeAnimStarted{ 0.0f };
		float	m_flFramerate{ 0.0f };
		float	m_flFrame{ 0.0f };
#endif
	}
	m_Stack;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	EntityHandle<CBasePlayer>	m_pPlayer;		// one of these two must be valid. or this weapon will be removed.
	EntityHandle<CWeaponBox>	m_pWeaponBox;
	int		m_iClientClip		{ 0 };
	int		m_iClientWeaponState{ 0 };
#else
public:	// CL exclusive variables.
	CBasePlayer*	m_pPlayer		{ nullptr };	// local pseudo-player
	float			m_flBlockCheck	{ 0.0f };
	Vector			m_vecBlockOffset{ g_vecZero };
#endif

public:	// basic logic funcs
	virtual void	Think			(void);		// called by PlayerPreThink.
	virtual bool	AddToPlayer		(CBasePlayer* pPlayer);	// should only be called by CBasePlayer::AddPlayerItem();
	virtual bool	Deploy			(void) { return false; }	// called when attempting to take it out.
	virtual void	PostFrame		(void);		// called by PlayerPostThink.
	virtual void	PrimaryAttack	(void) {}	// IN_MOUSE1
	virtual void	SecondaryAttack	(void) {}	// IN_MOUSE2
	virtual void	WeaponIdle		(void) {}	// constantly called when nothing else to do.
	virtual bool	Reload			(void) { return false; }	// you know what it is, right?
	virtual bool	Melee			(void);		// quick knife.
	virtual bool	QuickThrowStart	(EquipmentIdType iId);	// quick grenade (maybe something else in the future?).
	virtual bool	QuickThrowRelease(void);	// triggered when +qtg button released.
	virtual bool	AlterAct		(void) { return false; }	// special use. for instance, XM8 "morph".
	virtual bool	HolsterStart	(void);		// play holster anim, initialize holstering.
	virtual void	Holstered		(void);		// majorlly reset the weapon data. no visual stuff.
	virtual	void	DashStart		(void) { m_bitsFlags |= WPNSTATE_DASHING; }		// called when system thinks it's time to dash.
	virtual void	DashEnd			(void) { m_bitsFlags &= ~WPNSTATE_DASHING; }		// called when system thinks it's time to stop dash.
	virtual bool	Drop			(void **ppWeaponBoxReturned = nullptr);		// called when attempting to drop it on ground. ppWeaponBoxReturned is the CWeaponBox to be returned. (NOT avaliable on client side.)
	virtual bool	Kill			(void);		// called when attempting to remove it from your inventory.

#ifndef CLIENT_DLL
public:	// SV exclusive functions.
	virtual void	UpdateClientData(void);
	virtual void	Precache		(void) {}
#else
public:	// CL xclusive functions.
	virtual	bool	UsingInvertedVMDL(void)		{ return true; }	// by default, original CS/CZ vmdls are inverted displaying.
	virtual int		CalcBodyParam(void)			{ return 0; }		// allow user to varient weapon body.
	virtual	void	UpdateBobParameters(void);						// in which you may set some bob parameters.
	virtual	bool	StudioEvent(const struct mstudioevent_s* pEvent)	{ return false; }	// Handle VMDL studio events. @return: Did you handle the event by yourself?
#endif

public:	// basic API and behaviour for weapons.
	bool	DefaultDeploy	(const char* szViewModel, const char* szWeaponModel, int iAnim, const char* szAnimExt, float flDeployTime = 0.75f);
	void	DefaultIdle		(int iDashingAnim, int iIdleAnim = 0, float flDashLoop = 20.0f);
	bool	DefaultReload	(int iClipSize, int iAnim, float flTotalDelay, float flSoftDelay = 0.5f);
	bool	DefaultHolster	(int iHolsterAnim, float flHolsterDelay);
	void	DefaultSteelSight(const Vector& vecOfs, int iFOV, float flDriftingSpeed = 10.0f, float flNextSecondaryAttack = 0.3f);
	void	DefaultScopeSight(const Vector& vecOfs, int iFOV, float flEnterScopeDelay = 0.25f, float flFadeFromBlack = 5.0f, float flDriftingSpeed = 10.0f, float flNextSecondaryAttack = 0.3f);
	void	DefaultDashStart(int iEnterAnim, float flEnterTime);
	void	DefaultDashEnd	(int iEnterAnim, float flEnterTime, int iExitAnim, float flExitTime);
	bool	DefaultSetLHand	(bool bAppear, int iLHandUpAnim, float flLHandUpTime, int iLHandDownAnim, float flLHandDownTime);
	void	DefaultBlock	(int iEnterAnim, float flEnterTime, int iExitAnim, float flExitTime);
	float	DefaultSpread	(float flBaseline, float flAimingMul, float flDuckingMul, float flWalkingMul, float flJumpingMul);

public:	// util funcs
	virtual	WeaponIdType Id			(void) { return m_iId; }
	virtual	const WeaponInfo* WInfo	(void) { return m_pItemInfo; }
	virtual	const AmmoInfo* AInfo	(void) { return m_pAmmoInfo; }
	inline	bool	IsDead			(void) { return !!(m_bitsFlags & WPNSTATE_DEAD); }
	virtual	float	GetMaxSpeed		(void) { return 260.0f; }
	virtual	bool	AddPrimaryAmmo	(int iCount);	// fill in clip first, then bpammo.
	inline	bool	IsPistol		(void) { return m_pItemInfo->m_iSlot == PISTOL_SLOT; }
	virtual	void	SendWeaponAnim	(int iAnim, bool bSkipLocal = true);
	virtual	void	PlayEmptySound	(void);
	virtual	void	ReloadSound		(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	virtual	bool	CanHolster		(void);	// smells, looks and tastes like a duck...
	virtual	bool	CanDrop			(void) { return true; }
	virtual void	KickBack		(float up_base, float lateral_base, float up_modifier, float lateral_modifier, float up_max, float lateral_max, int direction_change);	// recoil
	virtual void	ResetModel		(void) { }	// used after Melee() and QuickThrowRelease().
	virtual bool	SetVariation	(RoleTypes iType) { m_iVariation = iType; return true; }
	virtual bool	SetLeftHand		(bool bAppear) { return false; }
	virtual void	PlayBlockAnim	(void) { }
	virtual float	GetSpread		(void) { return 0.0f; }
};

// Upstream messages.
struct primaryattack_message_s
{
	Vector m_vecSrc{ g_vecZero }, m_vecViewAngles{ g_vecZero };
	WeaponIdType m_iId{ WEAPON_NONE };
	int m_iClip{ 0 }, m_iRandomSeed{ 0 };

	static inline std::shared_ptr<primaryattack_message_s> Empty(void)
	{
		return std::make_shared<primaryattack_message_s>();
	}
};

using primatk_msg_ptr = std::shared_ptr<primaryattack_message_s>;

// Declare detectors
CREATE_MEMBER_DETECTOR_CUSTOM(m_usEvent) { {T::m_usEvent} -> std::convertible_to<unsigned short>; };

CREATE_MEMBER_DETECTOR_CUSTOM(ApplyClientFPFiringVisual) { t.ApplyClientFPFiringVisual(Vector2D::Zero()); };
CREATE_MEMBER_DETECTOR_CUSTOM(ApplyRecoil) { t.ApplyRecoil(); };

CREATE_MEMBER_DETECTOR_STATIC(ACCURACY_BASELINE);
CREATE_MEMBER_DETECTOR_STATIC(MAX_SPEED_ZOOM);
CREATE_MEMBER_DETECTOR_STATIC(MAX_SPEED);
CREATE_MEMBER_DETECTOR_STATIC(SPREAD_BASELINE);
CREATE_MEMBER_DETECTOR_STATIC(CONE_VECTOR);
CREATE_MEMBER_DETECTOR_STATIC(RPM);
CREATE_MEMBER_DETECTOR_STATIC(FIRE_INTERVAL);
CREATE_MEMBER_DETECTOR_STATIC(GUN_VOLUME);
CREATE_MEMBER_DETECTOR_STATIC(GUN_FLASH);
CREATE_MEMBER_DETECTOR_STATIC(SHELL_MODEL);
CREATE_MEMBER_DETECTOR_STATIC(EVENT_FILE);

CREATE_MEMBER_DETECTOR_STATIC(SHOOT);
CREATE_MEMBER_DETECTOR_STATIC(FIRE_ANIMTIME);
CREATE_MEMBER_DETECTOR_STATIC(RELOAD_SOFT_DELAY_TIME);
CREATE_MEMBER_DETECTOR_STATIC(RELOAD_EMPTY_SOFT_DELAY_TIME);
CREATE_MEMBER_DETECTOR_STATIC(CHECK_MAGAZINE);

CREATE_MEMBER_DETECTOR_STATIC(ATTRIB_NO_FIRE_UNDERWATER);
CREATE_MEMBER_DETECTOR_CUSTOM(ATTRIB_SEMIAUTO) { {T::ATTRIB_SEMIAUTO == true}; };

template <typename CWpn>
concept IsShotgun = requires (CWpn wpn)
{
	{CWpn::CONE_VECTOR} -> std::convertible_to<Vector2D>;
	{CWpn::PROJECTILE_COUNT > 1};
	{!IS_MEMBER_PRESENTED_CPP20_W(SPREAD_BASELINE)};
};

template <typename CWpn>
concept HasEvent = requires (CWpn wpn)
{
	{CWpn::EVENT_FILE} -> std::convertible_to<const char*>;
	{CWpn::ApplyClientTPFiringVisual};
};

template <typename CWpn>
concept IsTubularMag = requires (CWpn wpn)
{
	{CWpn::START_RELOAD > 0};
	{CWpn::INSERT > 0};
	{CWpn::AFTER_RELOAD > 0};
};

template <typename CWpn>
concept IsManualRechamberWpn = requires (CWpn wpn)
{
	{CWpn::RECHAMBER > 0};	// Rechamber anim.
	{CWpn::RECHAMBER_TIME > 0};
	{CWpn::BITS_RECHAMBER_ANIM > 0};
};

// General template.
template <typename CWpn>
class CBaseWeaponTemplate : public CBaseWeapon
{
protected:
	using BaseClass = CBaseWeaponTemplate<CWpn>;

public:	// basic logic funcs
	bool	Deploy			(void) override;
	void	PrimaryAttack	(void) override { if constexpr (IsShotgun<CWpn> || IS_MEMBER_PRESENTED_CPP20_W(SPREAD_BASELINE)) DefaultShoot(); }
	void	WeaponIdle		(void) override { return DefaultIdle(CWpn::DASHING); }
	bool	Reload			(void) override;
	bool	HolsterStart	(void) override	{ return DefaultHolster(CWpn::HOLSTER, CWpn::HOLSTER_TIME); }
	void	DashStart		(void) override { return DefaultDashStart(CWpn::DASH_ENTER, CWpn::DASH_ENTER_TIME); }
	void	DashEnd			(void) override { return DefaultDashEnd(CWpn::DASH_ENTER, CWpn::DASH_ENTER_TIME, CWpn::DASH_EXIT, CWpn::DASH_EXIT_TIME); }

	template <DETECT_SHELL_MODEL T = CWpn> static inline int m_iShell = 0;
#ifndef CLIENT_DLL
	template <DETECT_EVENT_FILE T = CWpn> static inline unsigned short m_usEvent = 0;

	void	Precache		(void) override;	// Generalized precache function - precache basical files only.
#endif

public:	// util funcs
	WeaponIdType Id			(void) override;
	const WeaponInfo* WInfo	(void) override	{ return &g_rgWpnInfo[Id()]; }
	const AmmoInfo* AInfo	(void) override { return &g_rgAmmoInfo[WInfo()->m_iAmmoType]; }
	float	GetMaxSpeed		(void) override;
	void	ResetModel		(void) override;
	bool	SetLeftHand		(bool bAppear) override { return DefaultSetLHand(bAppear, CWpn::LHAND_UP, CWpn::LHAND_UP_TIME, CWpn::LHAND_DOWN, CWpn::LHAND_DOWN_TIME); }
	void	PlayBlockAnim	(void) override { return DefaultBlock(CWpn::BLOCK_UP, CWpn::BLOCK_UP_TIME, CWpn::BLOCK_DOWN, CWpn::BLOCK_DOWN_TIME); }
	float	GetSpread		(void) override { if constexpr (IS_MEMBER_PRESENTED_CPP20_W(SPREAD_BASELINE)) return CWpn::SPREAD_BASELINE; else return CBaseWeapon::GetSpread(); }

	// New functions.
	int	DefaultShoot		(void) requires(IsShotgun<CWpn>);	// @Return: iSeedOfs
	Vector2D DefaultShoot	(float flSpread = -1, float flCycleTime = -1) requires(IS_MEMBER_PRESENTED_CPP20_W(SPREAD_BASELINE));	// @Return: vecSpreadResult
	bool DefaultMagReload	(void) requires(!IsTubularMag<CWpn>);	// Inspection anim is included.

	virtual	void	ApplyServerFiringVisual	(void);
	virtual	int		AcquireShootAnim		(void)	// This take place after the m_iClip is subtracted. Hence detecting the last shot should be (m_iClip < 1) == true
	{
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(SHOOT))
		{
			return CWpn::SHOOT;
		}

		return 0;
	}
	virtual	void	PlaybackEvent			(const Vector2D& vSpread);	// What message do you wish to send to non-m_pPlayer players? Received at static member RegisterEvent().
//	virtual	void	ApplyClientFPFiringVisual(const Vector2D& vSpread);	// Called after m_iClip subtracted.
//	static	void	ApplyClientTPFiringVisual(struct event_args_s* args);	// Like it says, handle the TP visual effect only.

#ifdef CLIENT_DLL
	static	inline	auto	RegisterEvent			(void) requires(HasEvent<CWpn>)	{ return gEngfuncs.pfnHookEvent(CWpn::EVENT_FILE, CWpn::ApplyClientTPFiringVisual); }
			inline	void	EV_PlayShootAnim		(void) { return SendWeaponAnim(AcquireShootAnim()); }
			inline	void	EV_HLDM_CreateSmoke		(int speed, float scale, const Color& color, enum EV_SmokeTypes iSmokeType, bool bWind, int framerate) { ::EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], gpGlobals->v_forward, speed, scale, color.r(), color.g(), color.b(), iSmokeType, m_pPlayer->pev->velocity, bWind, framerate); }	// First personal only!
			inline	void	EV_GetDefaultShellInfo	(Vector& ShellVelocity, Vector& ShellOrigin, float forwardScale, float upScale, float rightScale)	// First personal ONLY!
			{
				::EV_GetDefaultShellInfo(
					gEngfuncs.GetLocalPlayer()->index,
					gEngfuncs.pEventAPI->EV_LocalPlayerDucking(),
					m_pPlayer->pev->origin, m_pPlayer->pev->velocity,
					ShellVelocity, ShellOrigin,
					gpGlobals->v_forward, gpGlobals->v_right, gpGlobals->v_up,
					forwardScale, upScale, rightScale
				);
			}
			inline	void	EV_HLDM_FireBullets		(const Vector2D& vecSpread) requires(!IsShotgun<CWpn>)	// First personal ONLY!
			{
				auto vecSrc = EV_GetGunPosition(
					gEngfuncs.GetLocalPlayer()->index,
					gEngfuncs.pEventAPI->EV_LocalPlayerDucking(),
					m_pPlayer->pev->origin
				);

				::EV_HLDM_FireBullets(
					gEngfuncs.GetLocalPlayer()->index,
					gpGlobals->v_forward, gpGlobals->v_right, gpGlobals->v_up,
					1, vecSrc, gpGlobals->v_forward,
					vecSpread, CWpn::EFFECTIVE_RANGE, m_iPrimaryAmmoType,
					CWpn::PENETRATION,
					m_pPlayer->random_seed
				);
			}
			inline	void	EV_HLDM_FireBullets		(void) requires(IsShotgun<CWpn>)	// First personal ONLY!
			{
				auto vecSrc = EV_GetGunPosition(
					gEngfuncs.GetLocalPlayer()->index,
					gEngfuncs.pEventAPI->EV_LocalPlayerDucking(),
					m_pPlayer->pev->origin
				);

				::EV_HLDM_FireBullets(
					gEngfuncs.GetLocalPlayer()->index,
					gpGlobals->v_forward, gpGlobals->v_right, gpGlobals->v_up,
					CWpn::PROJECTILE_COUNT, vecSrc, gpGlobals->v_forward,
					CWpn::CONE_VECTOR, CWpn::EFFECTIVE_RANGE, m_iPrimaryAmmoType,
					1,
					m_pPlayer->random_seed
				);
			}
			inline	void	EV_PlayGunFire2			(void)
			{
				auto vecSrc = EV_GetGunPosition(
					gEngfuncs.GetLocalPlayer()->index,
					gEngfuncs.pEventAPI->EV_LocalPlayerDucking(),
					m_pPlayer->pev->origin
				);

				::EV_PlayGunFire2(vecSrc + gpGlobals->v_forward * 10.0f, CWpn::FIRE_SFX, CWpn::GUN_VOLUME);
			}
#endif

private:
	CWpn* _pThis{ nullptr };
	inline CWpn* This() { if (!_pThis) _pThis = dynamic_cast<CWpn*>(this); return _pThis; }
};



struct CUSP : public CBaseWeaponTemplate<CUSP>
{
#pragma region USP database
	enum usp_e
	{
		IDLE = 0,
		SHOOT,
		SHOOT_LAST,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		RELOAD,
		RELOAD_EMPTY,
		CHECK_MAGAZINE,
		LHAND_DOWN,
		LHAND_UP,
		BLOCK_UP,
		BLOCK_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	// Slide stop available anims.
	static constexpr auto	BITS_SLIDE_STOP_ANIM =	(1 << IDLE) |
													(1 << DEPLOY) |
													(1 << HOLSTER) |
													(1 << CHECK_MAGAZINE) |
													(1 << LHAND_DOWN) | (1 << LHAND_UP) |
													(1 << BLOCK_DOWN) | (1 << BLOCK_UP) |
													(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT)/* |
													(1 << SH_DASH_ENTER) | (1 << SH_DASHING) | (1 << SH_DASH_EXIT)*/;	// UNDONE, TODO

	static constexpr auto	VIEW_MODEL			= "models/weapons/v_usp.mdl";
	static constexpr auto	WORLD_MODEL			= "models/weapons/w_usp.mdl";
	static constexpr auto	FIRE_SFX			= "weapons/usp/usp_fire.wav";
	static constexpr auto	POSTURE				= "onehanded";
	static constexpr auto	MAX_SPEED			= 250.0f;
	static constexpr auto	DAMAGE				= 32;
	static constexpr auto	RANGE_MODIFER		= 1.187260896;	// 80% damage @650 inches.
	static constexpr auto	FIRE_INTERVAL		= 0.15f;
	static constexpr auto	EFFECTIVE_RANGE		= 4096.0f;
	static constexpr auto	PENETRATION			= 1;	// 1 means it can't penetrate anything.
	static constexpr auto	SPREAD_BASELINE		= 1.2f;
	static constexpr auto	ACCURACY_BASELINE	= 0.92f;
	static constexpr auto	GUN_VOLUME			= QUIET_GUN_VOLUME;
	static constexpr auto	GUN_FLASH			= DIM_GUN_FLASH;
	static constexpr auto	SHELL_MODEL			= "models/pshell.mdl";
	static constexpr auto	EVENT_FILE			= "events/usp.sc";

	// Anim time
	static constexpr auto	FIRE_ANIMTIME					= 13.0 / 30.0;
	static constexpr auto	DEPLOY_TIME						= 16.0 / 30.0;
	static constexpr auto	DRAW_FIRST_TIME					= 41.0 / 30.0;
	static constexpr auto	HOLSTER_TIME					= 16.0 / 30.0;
	static constexpr auto	RELOAD_TIME						= 66.0 / 30.0;
	static constexpr auto	RELOAD_SOFT_DELAY_TIME			= RELOAD_TIME - 44.0 / 30.0;
	static constexpr auto	RELOAD_EMPTY_TIME				= 66.0 / 30.0;
	static constexpr auto	RELOAD_EMPTY_SOFT_DELAY_TIME	= RELOAD_EMPTY_TIME - 45.0 / 30.0;
	static constexpr auto	CHECK_MAGAZINE_TIME				= 76.0 / 30.0;
	static constexpr auto	LHAND_DOWN_TIME					= 11.0 / 30.0;
	static constexpr auto	LHAND_UP_TIME					= 11.0 / 30.0;
	static constexpr auto	BLOCK_UP_TIME					= 11.0 / 30.0;
	static constexpr auto	BLOCK_DOWN_TIME					= 11.0 / 30.0;
	static constexpr auto	DASH_ENTER_TIME					= 11.0 / 30.0;
	static constexpr auto	DASH_EXIT_TIME					= 11.0 / 30.0;

	// Attrib
	static constexpr auto	ATTRIB_SEMIAUTO		= true;
#pragma endregion

	// basic logic funcs
	void	SecondaryAttack	(void) final;

	// util funcs
	float	GetSpread		(void) final;

#ifdef CLIENT_DLL
	bool	UsingInvertedVMDL	(void) final { return false; }	// Model designed by InnocentBlue is not inverted.
	int		CalcBodyParam		(void) final;
#endif

	// new funcs
	int		AcquireShootAnim			(void) final { return m_iClip >= 1 ? SHOOT : SHOOT_LAST; }
	void	ApplyClientFPFiringVisual	(const Vector2D& vSpread);
	void	ApplyRecoil					(void);

	static	void	ApplyClientTPFiringVisual(struct event_args_s* args);
};

#define MP5N_VIEW_MODEL		"models/weapons/v_mp5.mdl"
#define MP5N_WORLD_MODEL	"models/weapons/w_mp5.mdl"
#define MP5N_FIRE_SFX		"weapons/mp5/mp5_fire.wav"

constexpr float MP5N_MAX_SPEED     = 250.0f;
constexpr float MP5N_DAMAGE        = 26.0f;
constexpr float MP5N_RANGE_MODIFER = 1.124618621;	// 80% damage @950 inches.
constexpr float MP5N_RELOAD_TIME   = 2.63f;

enum mp5n_e
{
	MP5N_IDLE1,
	MP5N_RELOAD,
	MP5N_DRAW,
	MP5N_SHOOT1,
	MP5N_SHOOT2,
	MP5N_SHOOT3,
};

struct CSCARH : public CBaseWeaponTemplate<CSCARH>
{
#pragma region SCAR-H Database
	enum scarh_anim_e
	{
		IDLE,
		SHOOT1,
		SHOOT2,
		SHOOT3,
		SHOOT_LAST,
		SHOOT_ATTACHMENTS,
		M870MCS_PUMP,
		RELOAD,
		RELOAD_EMPTY,
		EGLM_RELOAD,
		XM26_RELOAD,
		XM26_RELOAD_EMPTY,
		M870MCS_RELOAD_START,
		M870MCS_RELOAD_FIRST_INSERT,
		M870MCS_RELOAD_INSERT,
		M870MCS_RELOAD_END,
		M870MCS_RELOAD_END_EMPTY,
		DRAW_FIRST,
		DEPLOY,
		JUMP,
		CHECK_MAGAZINE,
		SWITCH_SELECTOR,
		HOLSTER,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	static constexpr auto	VIEW_MODEL			= "models/weapons/v_scarh.mdl";
	static constexpr auto	WORLD_MODEL			= "models/weapons/w_scarl.mdl";
	static constexpr auto	FIRE_SFX			= "weapons/SCARH/mk17_shoot.wav";
	static constexpr auto	POSTURE				= "mp5";
	static constexpr auto	MAX_SPEED			= 235.0f;
	static constexpr auto	DAMAGE				= 54;
	static constexpr auto	RANGE_MODIFER		= 1.057371263;	// 80% damage @2000 inches.
	static constexpr auto	RPM					= 550.0f;
	static constexpr auto	EFFECTIVE_RANGE		= 8192.0f;
	static constexpr auto	PENETRATION			= 2;
	static constexpr auto	SPREAD_BASELINE		= 0.15f;
	static constexpr auto	ACCURACY_BASELINE	= 0.25f;
	static constexpr auto	GUN_VOLUME			= NORMAL_GUN_VOLUME;
	static constexpr auto	GUN_FLASH			= BRIGHT_GUN_FLASH;
	static constexpr auto	SHELL_MODEL			= "models/rshell.mdl";
	static constexpr auto	EVENT_FILE			= "events/scarh.sc";

	// Anim time
	static constexpr auto FIRE_ANIMTIME					= 6.0 / 20.0;
	static constexpr auto shoot_Parts_TIME				= 16.0 / 33.0;
	static constexpr auto rechamber_m870_TIME			= 31.0 / 30.0;
	static constexpr auto RELOAD_TIME					= 86.0 / 33.0;
	static constexpr auto RELOAD_SOFT_DELAY_TIME		= RELOAD_TIME - 63.0 / 33.0;
	static constexpr auto RELOAD_EMPTY_TIME				= 96.0 / 33.0;
	static constexpr auto RELOAD_EMPTY_SOFT_DELAY_TIME	= RELOAD_EMPTY_TIME - 75.0 / 33.0;
	static constexpr auto reload_eglm_TIME				= 115.0 / 33.0;
	static constexpr auto reload_xm26_TIME				= 61.0 / 33.0;
	static constexpr auto reload_xm26_empty_TIME		= 78.0 / 30.0;
	static constexpr auto reload_m870_start_TIME		= 13.0 / 30.0;
	static constexpr auto reload_m870_first_TIME		= 66.0 / 30.0;
	static constexpr auto reload_m870_loop_TIME			= 16.0 / 23.0;
	static constexpr auto reload_m870_end_TIME			= 16.0 / 27.0;
	static constexpr auto reload_m870_empty_TIME		= 38.0 / 33.0;
	static constexpr auto DRAW_FIRST_TIME				= 86.0 / 33.0;
	static constexpr auto DEPLOY_TIME					= 36.0 / 37.0;
	static constexpr auto Jump_TIME						= 31.0 / 30.0;
	static constexpr auto CHECK_MAGAZINE_TIME			= 101.0 / 33.0;
	static constexpr auto Switch_TIME					= 16.0 / 25.0;
	static constexpr auto HOLSTER_TIME					= 26.0 / 35.0;
	static constexpr auto BLOCK_UP_TIME					= 11.0 / 33.0;
	static constexpr auto BLOCK_DOWN_TIME				= 13.0 / 33.0;
	static constexpr auto LHAND_UP_TIME					= 21.0 / 38.0;
	static constexpr auto LHAND_DOWN_TIME				= 16.0 / 38.0;
	static constexpr auto DASH_ENTER_TIME				= 16.0 / 33.0;
	static constexpr auto DASH_EXIT_TIME				= 16.0 / 33.0;
#pragma endregion

	// basic logic funcs
	void	SecondaryAttack	(void) final;

	// util funcs
	float	GetSpread		(void) final;

#ifdef CLIENT_DLL
	// CL exclusive functions.
	void	Think			(void) final;
	int		CalcBodyParam	(void) final;
#endif

	// new funcs
	int		AcquireShootAnim			(void) final;
	void	ApplyClientFPFiringVisual	(const Vector2D& vSpread);
	void	ApplyRecoil					(void);

	static	void	ApplyClientTPFiringVisual(struct event_args_s* args);
};

#define AK47_VIEW_MODEL		"models/weapons/v_ak47.mdl"
#define AK47_WORLD_MODEL	"models/weapons/w_ak47.mdl"
#define AK47_FIRE_SFX		"weapons/ak47/ak47_fire.wav"

constexpr float AK47_MAX_SPEED			= 221.0f;
constexpr float AK47_DAMAGE				= 48;
constexpr float AK47_RANGE_MODIFER		= 1.077217345;	// 80% damage @1500 inches.
constexpr float AK47_RELOAD_TIME		= 2.44f;
constexpr float AK47_RPM				= 600.0f;
constexpr float	AK47_EFFECTIVE_RANGE	= 8192.0f;
constexpr int	AK47_PENETRATION		= 2;
constexpr float	AK47_SPREAD_BASELINE	= 0.4f;

enum ak47_e
{
	AK47_IDLE1,
	AK47_RELOAD,
	AK47_DRAW,
	AK47_SHOOT1,
	AK47_SHOOT2,
	AK47_SHOOT3,
};

class CAK47 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return AK47Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void) { return AK47_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new functions
	void AK47Fire(float flSpread, float flCycleTime = (60.0f / AK47_RPM));
};

class CXM8 : public CBaseWeaponTemplate<CXM8>
{
public:	// Constants / Database
	enum xm8_anim_e
	{
		IDLE = 0,
		FIRE,
		FIRE_LAST,
		FIRE_AIM,
		FIRE_AIM_LAST,
		RELOAD,
		RELOAD_EMPTY,
		DRAW_FIRST,
		DEPLOY,
		HOLSTER,
		CHECK_MAGAZINE,
		SELECTOR_SEMI,
		SELECTOR_AUTO,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_DOWN,
		LHAND_UP,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		SWITCH_TO_SHARPSHOOTER,
		SWITCH_TO_CARBINE
	};

	static constexpr pcchar	VIEW_MODEL				= "models/weapons/v_xm8.mdl";
	static constexpr pcchar	WORLD_MODEL				= "models/weapons/w_xm8.mdl";
	static constexpr pcchar	FIRE_SFX				= "weapons/xm8/xm8_shoot.wav";
	static constexpr pcchar POSTURE					= "carbine";
	static constexpr float	MAX_SPEED				= 240.0f;
	static constexpr float	MAX_SPEED_ZOOM			= 150.0f;
	static constexpr float	DAMAGE					= 32.0f;
	static constexpr float	RANGE_MODIFER			= 1.057371263;	// 80% damage @2000 inches
	static constexpr float	RELOAD_TIME				= 2.33f;
	static constexpr float	RELOAD_EMPTY_TIME		= 3.03f;
	static constexpr float	DRAW_FIRST_TIME			= 1.3F;
	static constexpr float	DEPLOY_TIME				= 0.7F;
	static constexpr float	HOLSTER_TIME			= 0.7F;
	static constexpr float	CHECK_MAGAZINE_TIME		= 2.2667F;
	static constexpr float	DASH_ENTER_TIME			= 0.8F;
	static constexpr float	DASH_EXIT_TIME			= 0.533F;
	static constexpr float	TO_SHARPSHOOTER_TIME	= 8.8F;
	static constexpr float	TO_CARBIN_TIME			= 8.8F;
	static constexpr float	BLOCK_UP_TIME			= 0.5333f;
	static constexpr float	BLOCK_DOWN_TIME			= 0.5333f;
	static constexpr float	LHAND_UP_TIME			= 0.5333f;
	static constexpr float	LHAND_DOWN_TIME			= 0.5333f;
	static constexpr float	RPM						= 800.0f;
	static constexpr int	PENETRATION				= 2;
	static constexpr float	EFFECTIVE_RANGE			= 8192.0f;
	static constexpr int	GUN_VOLUME				= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE			= 0.1f;
	static constexpr float	ACCURACY_BASELINE		= 0.25f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual void	PostFrame		(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual bool	AlterAct		(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void XM8Fire(float flSpread, float flCycleTime = (60.0f / RPM));
};

struct CAWP : public CBaseWeaponTemplate<CAWP>
{
#pragma region L115A3 Database
	enum EL115A3Anims : BYTE
	{
		IDLE,
		SHOOT,
		RECHAMBER,
		SHOOT_LAST,
		RELOAD,
		RELOAD_EMPTY,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		CHECK_MAGAZINE,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	
	// Pin static anims.
	static constexpr int BITS_PIN_UNINVOLVED_ANIM = (1 << IDLE) |
													(1 << DEPLOY) |
													(1 << RELOAD) |	// normal reload could not bypass this rechamber flag.
													(1 << HOLSTER) |
													(1 << CHECK_MAGAZINE) |
													(1 << LHAND_DOWN) | (1 << LHAND_UP) |
													(1 << BLOCK_DOWN) | (1 << BLOCK_UP) |
													(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT);

	// Rechamber acceptable anims.
	static constexpr int BITS_RECHAMBER_ANIM = (1 << RECHAMBER) | (1 << SHOOT) | (1 << RELOAD_EMPTY);

	// Anim time.
	static constexpr auto FIRE_ANIMTIME			= 45.0 / 30.0;
	static constexpr auto RECHAMBER_TIME		= 36.0 / 30.0;
	static constexpr auto SHOOT_LAST_TIME		= 17.0 / 30.0;
	static constexpr auto RELOAD_TIME			= 107.0 / 30.0;
	static constexpr auto RELOAD_EMPTY_TIME		= 135.0 / 30.0;
	static constexpr auto DEPLOY_TIME			= 22.0 / 30.0;
	static constexpr auto DRAW_FIRST_TIME		= 46.0 / 30.0;
	static constexpr auto HOLSTER_TIME			= 22.0 / 30.0;
	static constexpr auto CHECK_MAGAZINE_TIME	= 61.0 / 30.0;
	static constexpr auto BLOCK_UP_TIME			= 16.0 / 30.0;
	static constexpr auto BLOCK_DOWN_TIME		= 16.0 / 30.0;
	static constexpr auto LHAND_UP_TIME			= 21.0 / 30.0;
	static constexpr auto LHAND_DOWN_TIME		= 21.0 / 30.0;
	static constexpr auto DASH_ENTER_TIME		= 14.0 / 30.0;
	static constexpr auto DASH_EXIT_TIME		= 14.0 / 30.0;

	static constexpr auto	VIEW_MODEL			= "models/weapons/v_awp.mdl";
	static constexpr auto	WORLD_MODEL			= "models/weapons/w_awp.mdl";
	static constexpr auto	FIRE_SFX			= "weapons/l115a3/l115a3_fire.wav";
	static constexpr auto	POSTURE				= "rifle";
	static constexpr auto	MAX_SPEED			= 210.0f;
	static constexpr auto	MAX_SPEED_ZOOM		= 150.0f;
	static constexpr auto	DAMAGE				= 125.0f;
	static constexpr auto	RANGE_MODIFER		= 1.047585759;	// 80% damage @2400 inches.
	static constexpr auto	FIRE_INTERVAL		= FIRE_ANIMTIME;
	static constexpr auto	EFFECTIVE_RANGE		= 8192.0f;
	static constexpr auto	PENETRATION			= 3;
	static constexpr auto	SPREAD_BASELINE		= 0.001f;
	static constexpr auto	GUN_VOLUME			= BIG_EXPLOSION_VOLUME;
	static constexpr auto	GUN_FLASH			= NORMAL_GUN_FLASH;
	static constexpr auto	SHELL_MODEL			= "models/rshell_big.mdl";
	static constexpr auto	EVENT_FILE			= "events/awp.sc";

	// Attrib
	static constexpr auto	ATTRIB_SEMIAUTO		= true;
#pragma endregion

	// basic logic funcs
	void	PrimaryAttack	(void) final;
	void	SecondaryAttack	(void) final { return DefaultScopeSight(Vector(-6.2f, -2, 1.1f), 25); }

	// util funcs
	float	GetSpread		(void) final;

#ifdef CLIENT_DLL
public:	// CL exclusive functions.
	bool	UsingInvertedVMDL	(void) final	{ return false; }	// Model designed by InnocentBlue is not inverted.
	int		CalcBodyParam		(void) final;
	bool	StudioEvent			(const struct mstudioevent_s* pEvent) final;
#endif

	bool	m_bChamberCleared{ true };
	QCScript::OperationSet m_QCScript;

	// new funcs
	int		AcquireShootAnim			(void) final { return m_iClip >= 1 ? SHOOT : SHOOT_LAST; }
	void	ApplyClientFPFiringVisual	(const Vector2D& vSpread);
	void	ApplyRecoil					(void);

	static	void	ApplyClientTPFiringVisual(struct event_args_s* args);
};

class CDEagle : public CBaseWeaponTemplate<CDEagle>
{
public:	// Constants / Database
	enum deagle_anim_e
	{
		IDLE = 0,
		SHOOT,
		SHOOT_EMPTY,
		RELOAD,
		RELOAD_EMPTY,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		CHECK_MAGAZINE,
		LHAND_DOWN,
		LHAND_UP,
		BLOCK_UP,
		BLOCK_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		SH_RELOAD,
		SH_RELOAD_EMPTY,
		SH_DASH_ENTER,
		SH_DASHING,
		SH_DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_deagle.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/weapons/w_deagle.mdl";
	static constexpr pcchar	FIRE_SFX			= "weapons/deagle/deagle_fire.wav";
	static constexpr pcchar	POSTURE				= "onehanded";
	static constexpr float	MAX_SPEED			= 245.0f;
	static constexpr float	DAMAGE				= 60;
	static constexpr float	RANGE_MODIFER		= 1.106751211;	// 80% damage @1100 inches.
	static constexpr float	RELOAD_TIME			= 1.97f;
	static constexpr float	RELOAD_EMPTY_TIME	= 2.02f;
	static constexpr float	DEPLOY_TIME			= 0.7f;
	static constexpr float	DRAW_FIRST_TIME		= 1.8f;
	static constexpr float	HOLSTER_TIME		= 0.7f;
	static constexpr float	CHECK_MAGAZINE_TIME	= 2.32f;
	static constexpr float	BLOCK_UP_TIME		= 0.5333F;
	static constexpr float	BLOCK_DOWN_TIME		= 0.5333F;
	static constexpr float	LHAND_UP_TIME		= 0.6666F;
	static constexpr float	LHAND_DOWN_TIME		= 0.6666F;
	static constexpr float	DASH_ENTER_TIME		= 0.8667f;
	static constexpr float	DASH_EXIT_TIME		= 0.3667f;
	static constexpr float	SH_RELOAD_TIME		= 2.2333f;
	static constexpr float	SH_RELOAD_EMPTY_TIME= 2.7f;
	static constexpr float	SH_DASH_ENTER_TIME	= 0.7f;
	static constexpr float	SH_DASH_EXIT_TIME	= 0.7f;
	static constexpr float	FIRE_INTERVAL		= 0.225f;
	static constexpr int	PENETRATION			= 2;
	static constexpr float	EFFECTIVE_RANGE		= 4096.0f;
	static constexpr int	GUN_VOLUME			= BIG_EXPLOSION_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 4;
	static constexpr float	ACCURACY_BASELINE	= 0.9f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_SLIDE_STOP_ANIM =	(1 << IDLE) |
												(1 << DEPLOY) |
												(1 << HOLSTER) |
												(1 << CHECK_MAGAZINE) |
												(1 << LHAND_DOWN) | (1 << LHAND_UP) |
												(1 << BLOCK_DOWN) | (1 << BLOCK_UP) |
												(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT) |
												(1 << SH_DASH_ENTER) | (1 << SH_DASHING) | (1 << SH_DASH_EXIT);

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return DEagleFire(GetSpread()); }
	virtual void	SecondaryAttack	(void)	{ return DefaultSteelSight(Vector(-1.905f, -2, 1.1f), 85); }
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void DEagleFire(float flSpread, float flCycleTime = FIRE_INTERVAL);
};

const float FLASHBANG_MAX_SPEED        = 250.0f;
const float FLASHBANG_MAX_SPEED_SHIELD = 180.0f;

enum flashbang_e
{
	FLASHBANG_IDLE,
	FLASHBANG_PULLPIN,
	FLASHBANG_THROW,
	FLASHBANG_DRAW,
};

#define SVD_VIEW_MODEL	"models/weapons/v_svd.mdl"
#define SVD_WORLD_MODEL	"models/weapons/w_svd.mdl"
#define SVD_FIRE_SFX	"weapons/svd/svd_fire.wav"

constexpr float SVD_MAX_SPEED		= 210.0f;
constexpr float SVD_MAX_SPEED_ZOOM	= 150.0f;
constexpr float SVD_DAMAGE			= 115;
constexpr float SVD_RANGE_MODIFER	= 1.077137738;	// 70% damage @2400 inches.
constexpr float SVD_DEPLOY_TIME		= 1.13f;
constexpr float SVD_RELOAD_TIME		= 3.2f;
constexpr float SVD_FIRE_INTERVAL	= 0.25f;
constexpr int	SVD_PENETRATION		= 3;
constexpr float	SVD_EFFECTIVE_RANGE	= 8192.0f;
constexpr float	SVD_SPREAD_BASELINE = 0.0251f;

enum svd_e
{
	SVD_IDLE,
	SVD_SHOOT,
	SVD_SHOOT2,
	SVD_RELOAD,
	SVD_DRAW,
};

class CSVD : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return SVDFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void);
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void)	{ return DefaultSpread(SVD_SPREAD_BASELINE, 0.25f, 0.75f, 2.0f, 5.0f); }

public:	// new functions
	void SVDFire(float flSpread, float flCycleTime = SVD_FIRE_INTERVAL);
};

#define G18C_VIEW_MODEL		"models/weapons/v_glock18.mdl"
#define G18C_WORLD_MODEL	"models/w_glock18.mdl"	// FIXME
#define G18C_FIRE_SFX		"weapons/glock18/glock18_fire.wav"

constexpr float GLOCK18_MAX_SPEED		= 245.0f;
constexpr float GLOCK18_DAMAGE			= 17;
constexpr float GLOCK18_RANGE_MODIFER	= 1.187260896;	// 80% damage @650 inches.
constexpr float GLOCK18_DEPLOY_TIME		= 0.34f;
constexpr float GLOCK18_RELOAD_TIME		= 1.87f;
constexpr float GLOCK18_RPM				= 1200.0f;
constexpr int	GLOCK18_PENETRATION		= 1;
constexpr float	GLOCK18_EFFECTIVE_RANGE	= 4096.0f;
constexpr float	GLOCK18_SPREAD_BASELINE = 0.33f;	// don't mul it my 10 like other pistols. IT'S A DE FACTO SMG.

enum glock18_e
{
	GLOCK18_IDLE1,
	GLOCK18_IDLE2,
	GLOCK18_IDLE3,
	GLOCK18_SHOOT,
	GLOCK18_SHOOT2,
	GLOCK18_SHOOT3,
	GLOCK18_SHOOT_EMPTY,
	GLOCK18_RELOAD,
	GLOCK18_DRAW,
	GLOCK18_HOLSTER,
	GLOCK18_ADD_SILENCER,
	GLOCK18_DRAW2,
	GLOCK18_RELOAD2,
};

class CG18C : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return GLOCK18Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return GLOCK18_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new functions
	void GLOCK18Fire(float flSpread, float flCycleTime = 60.0f / GLOCK18_RPM);
};

#define THROWABLE_VIEW_MODEL	"models/weapons/v_throwable.mdl"

constexpr float TIME_GR_IDLE_LOOP			= (11.0f / 3.0f);
constexpr float TIME_GR_QT_READY			= 0.86f;
constexpr float TIME_GR_QT_READY_2			= 0.86f;
constexpr float TIME_FB_QT_READY			= 0.96f;
constexpr float TIME_SG_QT_READY			= 0.96f;
constexpr float TIME_QT_THROWING_FAR		= 0.78f;
constexpr float TIME_SP_QT_THROWING_FAR		= 0.25f;
constexpr float TIME_QT_THROWING_LONGBOW	= 0.79f;
constexpr float TIME_SP_QT_THROWING_LB		= 0.18f;
constexpr float TIME_QT_THROWING_SOFT		= 0.79f;
constexpr float TIME_SP_QT_THROWING_SOFT	= 0.21f;

enum throwable_e
{
	GR_IDLE = 0,

	GR_QT_READY = 12,
	GR_QT_READY_2 = 25,

	FB_QT_READY = 38,

	SG_QT_READY = 51,

	QT_THROWING_FAR = 52,
	QT_THROWING_LONGBOW = 53,
	QT_THROWING_SOFT = 54,
};

constexpr float KNIFE_BODYHIT_VOLUME   = 128.0f;
constexpr float KNIFE_WALLHIT_VOLUME   = 512.0f;
constexpr float KNIFE_MAX_SPEED        = 250.0f;
constexpr float KNIFE_MAX_SPEED_SHIELD = 180.0f;
constexpr float KNIFE_QUICK_SLASH_TIME = 0.939f;
constexpr float KNIFE_QUICK_SLASH_DMG  = 55.0f;

enum knife_e
{
	KNIFE_IDLE,
	KNIFE_QUICK_SLASH = 22,	// model from HLMW.
};

namespace BasicKnife
{
	// player currently running this code.
#ifndef CLIENT_DLL
	inline EntityHandle<CBasePlayer>	m_pPlayer;
#else
	inline CBasePlayer* m_pPlayer;
#endif

	// the weapon which calling quick slash.
	inline IWeapon* m_pWeapon;

	void Precache();
	bool Deploy(IWeapon* pWeapon);
	void Swing();
};

class CMK46 : public CBaseWeaponTemplate<CMK46>
{
public:	// Constants / Database
	enum mk46_anim_e
	{
		IDLE,
		SHOOT_UNSCOPE,
		SHOOT1,
		SHOOT2,
		SHOOT3,
		RELOAD_EMPTY,
		RELOAD,
		DRAW_FIRST,
		DEPLOY,
		JUMP,
		HOLSTER,
		INSPECTION,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL		= "models/weapons/v_mk46.mdl";
	static constexpr pcchar	WORLD_MODEL		= "models/weapons/w_mk46.mdl";
	static constexpr pcchar	FIRE_SFX		= "weapons/mk46/mk46_fire.wav";
	static constexpr pcchar	POSTURE			= "m249";
	static constexpr float	MAX_SPEED		= 210.0f;
	static constexpr float	DAMAGE			= 32.0f;
	static constexpr float	RANGE_MODIFER	= 1.077217345;	// 80% damage @1500 inches.
	static constexpr float	DEPLOY_TIME		= 1.0F;
	static constexpr float	DRAW_FIRST_TIME	= 1.68F;
	static constexpr float	RELOAD_TIME		= 6.767f;
	static constexpr float	RELOAD_EMPTY_TIME= 6.232f;
	static constexpr float	HOLSTER_TIME	= 0.6F;
	static constexpr float	INSPECTION_TIME	= 1.5556f;
	static constexpr float	BLOCK_UP_TIME	= 0.525f;
	static constexpr float	BLOCK_DOWN_TIME	= 0.6f;
	static constexpr float	LHAND_UP_TIME	= 0.7333f;
	static constexpr float	LHAND_DOWN_TIME	= 0.7333f;
	static constexpr float	DASH_ENTER_TIME	= 0.68F;
	static constexpr float	DASH_EXIT_TIME	= 0.68F;
	static constexpr float	RPM				= 750.0f;
	static constexpr int	PENETRATION		= 2;
	static constexpr float	EFFECTIVE_RANGE	= 8192.0f;
	static constexpr int	GUN_VOLUME		= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE	= 0.3f;
	static constexpr float	ACCURACY_BASELINE= 0.35f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual void	PrimaryAttack	(void)	{ return MK46Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void MK46Fire(float flSpread, float flCycleTime = (60.0f / RPM));
};

#define KSG12_VIEW_MODEL	"models/weapons/v_ksg12.mdl"
#define KSG12_WORLD_MODEL	"models/weapons/w_ksg12.mdl"
#define KSG12_FIRE_SFX		"weapons/ksg12/ksg12_fire.wav"

constexpr float KSG12_MAX_SPEED			= 230.0f;
constexpr float KSG12_DAMAGE			= 20.0f;
constexpr int	KSG12_PROJECTILE_COUNT	= 9;
constexpr float	KSG12_EFFECTIVE_RANGE	= 3000.0f;
constexpr float KSG12_FIRE_INTERVAL		= 0.95f;
constexpr float KSG12_SHELL_EJECT		= 0.66f;
constexpr float KSG12_TIME_START_RELOAD = 0.566f;
constexpr float KSG12_TIME_INSERT		= 0.75f;
constexpr float KSG12_TIME_ADD_AMMO		= 0.3f;
constexpr float KSG12_TIME_AFTER_RELOAD = 1.033f;
constexpr auto	KSG12_CONE_VECTOR		= Vector2D(0.0675f, 0.0675f); // special shotgun spreads
constexpr float KSG12_RANGE_MODIFIER	= 1.172793196; // 80% damage @700 inches.

enum ksg12_e
{
	KSG12_IDLE,
	KSG12_FIRE1,
	KSG12_FIRE2,
	KSG12_INSERT,
	KSG12_AFTER_RELOAD,
	KSG12_START_RELOAD,
	KSG12_DRAW,
	KSG12_HOLSTER,
};

class CKSG12 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:
	bool	m_bAllowNextEmptySound;
	float	m_flNextInsertAnim;
	float	m_flNextAddAmmo;
	bool	m_bSetForceStopReload;

	struct	// shotgun needs to expand these push-pop stuff a little bit.
	{
		float	m_flNextInsertAnim;
		float	m_flNextAddAmmo;
	}
	m_Stack2;

public:	// basic logic funcs
	virtual void	Think			(void);
	virtual bool	Deploy			(void);
	virtual void	PostFrame		(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual void	WeaponIdle		(void);
	virtual	bool	Reload			(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return KSG12_MAX_SPEED; }
	virtual	void	PlayEmptySound	(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void)	{ return KSG12_CONE_VECTOR.x; }
};

class CM4A1 : public CBaseWeaponTemplate<CM4A1>
{
public:	// Constants / Database
	enum m4a1_anim_e
	{
		IDLE,
		SHOOT_BACKWARD,
		SHOOT_LEFTWARD,
		SHOOT_RIGHTWARD,
		SHOOT_ATTACHMENTS,
		M870MCS_PUMP,
		RELOAD,
		RELOAD_EMPTY,
		M203_RELOAD,
		XM26_RELOAD,
		XM26_RELOAD_EMPTY,
		M870MCS_RELOAD_START,
		M870MCS_RELOAD_FIRST_INSERT,
		M870MCS_RELOAD_INSERT,
		M870MCS_RELOAD_END,
		M870MCS_RELOAD_END_EMPTY,
		DRAW_FIRST,
		DEPLOY,
		JUMP,
		CHECK_MAGAZINE,
		SWITCH_SELECTOR,
		HOLSTER,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_m4a1.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/weapons/w_m4a1.mdl";
	static constexpr pcchar	FIRE_SFX			= "weapons/AR15/ar15_shoot.wav";
	static constexpr pcchar	POSTURE				= "rifle";
	static constexpr float	MAX_SPEED			= 230.0f;
	static constexpr float	DAMAGE				= 32.0f;
	static constexpr float	RANGE_MODIFER		= 1.072221173;	// 80% damage @1600 inches.
	static constexpr float	DEPLOY_TIME			= 0.743f;
	static constexpr float	DRAW_FIRST_TIME		= 2.45f;
	static constexpr float	RELOAD_TIME			= 2.033f;
	static constexpr float	RELOAD_EMPTY_TIME	= 2.6f;
	static constexpr float	CHECK_MAGAZINE_TIME	= 3.06f;
	static constexpr float	HOLSTER_TIME		= 0.6f;
	static constexpr float	BLOCK_UP_TIME		= 0.3333f;
	static constexpr float	BLOCK_DOWN_TIME		= 0.3333f;
	static constexpr float	LHAND_UP_TIME		= 0.5526f;
	static constexpr float	LHAND_DOWN_TIME		= 0.4210f;
	static constexpr float	DASH_ENTER_TIME		= 0.485f;
	static constexpr float	DASH_EXIT_TIME		= 0.485f;
	static constexpr float	RPM					= 700.0f;	// 700~950 RPM
	static constexpr int	PENETRATION			= 2;
	static constexpr float	EFFECTIVE_RANGE		= 8192.0f;
	static constexpr int	GUN_VOLUME			= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.14f;
	static constexpr float	ACCURACY_BASELINE	= 0.25f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual void	PrimaryAttack	(void)	{ return M4A1Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void M4A1Fire(float flSpread, float flCycleTime = (60.0f / RPM));
};

#define PM9_VIEW_MODEL	"models/weapons/v_pm9.mdl"
#define PM9_WORLD_MODEL	"models/weapons/w_pm9.mdl"
#define PM9_FIRE_SFX	"weapons/pm9/pm9_fire.wav"

const float MAC10_MAX_SPEED     = 250.0f;
const float MAC10_DAMAGE        = 29.0f;
const float MAC10_RANGE_MODIFER = 1.149658245;	// 80% damage @800 inches.
const float MAC10_RELOAD_TIME   = 3.15f;

enum mac10_e
{
	MAC10_IDLE1,
	MAC10_RELOAD,
	MAC10_DRAW,
	MAC10_SHOOT1,
	MAC10_SHOOT2,
	MAC10_SHOOT3,
};

#define Anaconda_VIEW_MODEL		"models/weapons/v_anaconda.mdl"
#define Anaconda_WORLD_MODEL	"models/weapons/w_anaconda.mdl"
#define Anaconda_FIRE_SFX		"weapons/anaconda/anaconda_fire.wav"

constexpr float ANACONDA_MAX_SPEED			= 250.0f;
constexpr float ANACONDA_DAMAGE				= 55;
constexpr float ANACONDA_RANGE_MODIFER		= 1.052022439;	// 80% damage @2200 inches.
constexpr float ANACONDA_RELOAD_TIME		= 2.68f;
constexpr float ANACONDA_DEPLOY_TIME		= 0.367f;	// this gun has a extremely short deploy time.
constexpr float ANACONDA_FIRE_INTERVAL		= 0.15f;
constexpr int	ANACONDA_PENETRATION		= 1;
constexpr float	ANACONDA_EFFECTIVE_RANGE	= 4096.0f;
constexpr float ANACONDA_SPREAD_BASELINE	= 0.24;

enum anaconda_e
{
	ANACONDA_IDLE,
	ANACONDA_SHOOT1,
	ANACONDA_SHOOT2,
	ANACONDA_SHOOT3,
	ANACONDA_SHOOT_EMPTY,
	ANACONDA_RELOAD,
	ANACONDA_DRAW,
};

class CAnaconda : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive variables.
	float m_flShellRain;

public:	// we need some CL-exclusive VFX.
	virtual void	Think			(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return AnacondaFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual float	GetMaxSpeed		(void) { return ANACONDA_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new funcs
	void AnacondaFire(float flSpread, float flCycleTime = ANACONDA_FIRE_INTERVAL);
};

#define P90_VIEW_MODEL	"models/weapons/v_p90.mdl"
#define P90_WORLD_MODEL	"models/weapons/w_p90.mdl"
#define P90_FIRE_SFX	"weapons/p90/p90_fire.wav"

const float P90_MAX_SPEED     = 245.0f;
const float P90_DAMAGE        = 24;
const float P90_RANGE_MODIFER = 1.106751211;	// 80% damage @1100 inches.
const float P90_RELOAD_TIME   = 3.4f;

enum p90_e
{
	P90_IDLE1,
	P90_RELOAD,
	P90_DRAW,
	P90_SHOOT1,
	P90_SHOOT2,
	P90_SHOOT3,
};

#define M200_VIEW_MODEL		"models/weapons/v_m200.mdl"
#define M200_WORLD_MODEL	"models/weapons/w_m200.mdl"
#define M200_FIRE_SFX		"weapons/m200/m200_fire.wav"

const float SCOUT_MAX_SPEED      = 260.0f;
const float SCOUT_MAX_SPEED_ZOOM = 220.0f;
const float SCOUT_DAMAGE         = 100;
const float SCOUT_RANGE_MODIFER  = 1.061766218;	// 75% damage @2400 inches.
const float SCOUT_RELOAD_TIME    = 2.0f;

enum scout_e
{
	SCOUT_IDLE,
	SCOUT_SHOOT,
	SCOUT_SHOOT2,
	SCOUT_RELOAD,
	SCOUT_DRAW,
};

const float SMOKEGRENADE_MAX_SPEED        = 250.0f;
const float SMOKEGRENADE_MAX_SPEED_SHIELD = 180.0f;

enum smokegrenade_e
{
	SMOKEGRENADE_IDLE,
	SMOKEGRENADE_PINPULL,
	SMOKEGRENADE_THROW,
	SMOKEGRENADE_DRAW,
};

#define MP7A1_VIEW_MODEL	"models/weapons/v_mp7a1.mdl"
#define MP7A1_WORLD_MODEL	"models/weapons/w_mp7a1.mdl"
#define MP7A1_FIRE_SFX		"weapons/mp7a1/mp7a1_fire.wav"

constexpr float MP7A1_MAX_SPEED			= 250.0f;
constexpr float MP7A1_DAMAGE			= 22;
constexpr float MP7A1_RANGE_MODIFER		= 1.106751211;	// 80% damage @1100 inches.
constexpr float MP7A1_RELOAD_TIME		= 2.575f;
constexpr float MP7A1_RPM				= 850.0f;
constexpr int	MP7A1_PENETRATION		= 1;
constexpr float	MP7A1_EFFECTIVE_RANGE	= 8192.0f;
constexpr float MP7A1_SPREAD_BASELINE	= 0.2f;

enum mp7a1_e
{
	MP7A1_IDLE1,
	MP7A1_RELOAD,
	MP7A1_DRAW,
	MP7A1_SHOOT1,
	MP7A1_SHOOT2,
	MP7A1_SHOOT3,
};

class CMP7A1 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return MP7A1Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual float	GetMaxSpeed		(void) { return MP7A1_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new funcs
	void MP7A1Fire(float flSpread, float flCycleTime = (60.0f / MP7A1_RPM));
};

class CM1014 : public CBaseWeaponTemplate<CM1014>
{
public:	// Constants / Database
	enum m1014_anim_e
	{
		IDLE,
		SHOOT,
		SHOOT_LAST,
		AIM_SHOOT,
		AIM_SHOOT_LAST,
		START_RELOAD,
		START_RELOAD_FIRST,
		INSERT,
		AFTER_RELOAD,
		DRAW_FIRST,
		DEPLOY,
		HOLSTER,
		CHECK_MAGAZINE,
		INSPECTION,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL				= "models/weapons/v_m1014.mdl";
	static constexpr pcchar	WORLD_MODEL				= "models/weapons/w_m1014.mdl";
	static constexpr pcchar	FIRE_SFX				= "weapons/m1014/m1014_fire.wav";
	static constexpr pcchar	POSTURE					= "mp5";
	static constexpr float	MAX_SPEED				= 240.0f;
	static constexpr float	DAMAGE					= 20.0f;
	static constexpr int	PROJECTILE_COUNT		= 6;
	static constexpr float	EFFECTIVE_RANGE			= 3048.0f;
	static constexpr float	FIRE_INTERVAL			= 0.25f;
	static constexpr float	TIME_START_RELOAD		= 0.8f;
	static constexpr float	TIME_START_RELOAD_FIRST	= 2.333f;
	static constexpr float	TIME_INSERT				= 0.4f;
	static constexpr float	TIME_ADD_AMMO			= 0.2f;
	static constexpr float	TIME_ADD_AMMO_FIRST		= 1.0667f;
	static constexpr float	TIME_INSERT_SFX			= 0.2f;
	static constexpr float	TIME_SIDELOAD_SFX		= 1.0f;
	static constexpr float	TIME_AFTER_RELOAD		= 0.7667f;
	static constexpr float	DRAW_FIRST_TIME			= 1.9f;
	static constexpr float	DEPLOY_TIME				= 0.833f;
	static constexpr float	HOLSTER_TIME			= 0.833f;
	static constexpr float	CHECK_MAGAZINE_TIME			= 2.3f;
	static constexpr float	INSPECTION_TIME			= 1.8f;
	static constexpr float	BLOCK_UP_TIME			= 0.366F;
	static constexpr float	BLOCK_DOWN_TIME			= 0.366F;
	static constexpr float	LHAND_UP_TIME			= 0.7f;
	static constexpr float	LHAND_DOWN_TIME			= 0.7f;
	static constexpr float	DASH_ENTER_TIME			= 0.366F;
	static constexpr float	DASH_EXIT_TIME			= 0.366F;
	static constexpr auto	CONE_VECTOR				= Vector2D(0.0725, 0.0725); // special shotgun spreads
	static constexpr int	GUN_VOLUME				= LOUD_GUN_VOLUME;
	static constexpr float	RANGE_MODIFIER			= 1.172793196;	// 80% damage @700 inches.

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

public:
	// Bolt stop available anims.
	static constexpr int BITS_BOLT_STOP_ANIM =	(1 << IDLE) |
												(1 << DEPLOY) |
												(1 << HOLSTER) |
												(1 << INSPECTION) |
												(1 << BLOCK_UP) | (1 << BLOCK_DOWN) |
												(1 << LHAND_UP) | (1 << LHAND_DOWN) |
												(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT);


public:
	bool	m_bAllowNextEmptySound;
	float	m_flNextInsertAnim;
	float	m_flNextAddAmmo;
	float	m_flNextInsertionSFX;
	bool	m_bSetForceStopReload;
	bool	m_bStartFromEmpty;

	struct	// shotgun needs to expand these push-pop stuff a little bit.
	{
		float	m_flNextInsertAnim;
		float	m_flNextAddAmmo;
		float	m_flNextInsertionSFX;
	}
	m_Stack2;

public:	// basic logic funcs
	virtual void	Think			(void);
	virtual bool	Deploy			(void);
	virtual void	PostFrame		(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);

public:	// util funcs
	virtual	void	PlayEmptySound	(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	virtual float	GetSpread		(void)	{ return CONE_VECTOR.x; }
};

class CM45A1 : public CBaseWeaponTemplate<CM45A1>
{
public:	// Constants / Database
	enum m45a1_anim_e
	{
		IDLE = 0,
		SHOOT,
		SHOOT_LAST,
		AIM_SHOOT_A,
		AIM_SHOOT_B,
		AIM_SHOOT_LAST,
		RELOAD_EMPTY,
		RELOAD,
		DRAW_FIRST,
		DEPLOY,
		JUMP,
		HOLSTER,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_DOWN,
		LHAND_UP,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		SH_RELOAD_EMPTY,
		SH_RELOAD,
		SH_DASH_ENTER,
		SH_DASHING,
		SH_DASH_EXIT,
		CHECK_MAGAZINE
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_m45a1.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/w_elite.mdl";	// FIXME, BUGBUG
	static constexpr pcchar	FIRE_SFX			= "weapons/m45a1/m45a1_fire.wav";
	static constexpr pcchar	FIRE_SFX_SIL		= "weapons/m45a1/m45a1_fire_sil.wav";
	static constexpr pcchar	POSTURE				= "onehanded";
	static constexpr float	MAX_SPEED			= 250.0f;
	static constexpr float	RELOAD_EMPTY_TIME	= 2.6f;
	static constexpr float	RELOAD_TIME			= 2.166f;
	static constexpr float	DRAW_FIRST_TIME		= 1.067F;
	static constexpr float	DEPLOY_TIME			= 0.7F;
	static constexpr float	HOLSTER_TIME		= 0.5F;
	static constexpr float	DASH_ENTER_TIME		= 0.5333F;
	static constexpr float	DASH_EXIT_TIME		= 0.5333F;
	static constexpr float	BLOCK_UP_TIME		= 0.4f;
	static constexpr float	BLOCK_DOWN_TIME		= 0.4333f;
	static constexpr float	LHAND_UP_TIME		= 0.4f;
	static constexpr float	LHAND_DOWN_TIME		= 0.3333f;
	static constexpr float	SH_RELOAD_EMPTY_TIME= 2.833F;
	static constexpr float	SH_RELOAD_TIME		= 2.1667F;
	static constexpr float	SH_DASH_ENTER_TIME	= 0.33333F;
	static constexpr float	SH_DASH_EXIT_TIME	= 0.65F;
	static constexpr float	CHECK_MAGAZINE_TIME	= 2.06667F;
	static constexpr float	DAMAGE				= 36.0f;
	static constexpr float	RANGE_MODIFER		= 1.131980371;	// 80% damage @900 inches.
	static constexpr float	FIRE_INTERVAL		= 0.2f;
	static constexpr int	PENETRATION			= 1;
	static constexpr float	EFFECTIVE_RANGE		= 4096.0f;
	static constexpr int	GUN_VOLUME			= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.9f;
	static constexpr float	ACCURACY_BASELINE	= 0.88f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual int		CalcBodyParam(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_SLIDE_STOP_ANIM = (1 << IDLE) |
												(1 << DEPLOY) |
												(1 << HOLSTER) |
												(1 << CHECK_MAGAZINE) |
												(1 << LHAND_DOWN) | (1 << LHAND_UP) |
												(1 << BLOCK_DOWN) | (1 << BLOCK_UP) |
												(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT) |
												(1 << SH_DASH_ENTER) | (1 << SH_DASHING) | (1 << SH_DASH_EXIT);

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return M45A1Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void M45A1Fire(float flSpread, float flCycleTime = FIRE_INTERVAL);
};

class CFN57 : public CBaseWeaponTemplate<CFN57>
{
public:	// Constants / Database
	enum fiveseven_anim_e
	{
		IDLE = 0,
		SHOOT,
		SHOOT_LAST,
		AIM_SHOOT,
		AIM_SHOOT_LAST,
		RELOAD,
		RELOAD_EMPTY,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		CHECK_MAGAZINE,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		SH_RELOAD,
		SH_RELOAD_EMPTY,
		SH_DASH_ENTER,
		SH_DASHING,
		SH_DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_fiveseven.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/weapons/w_fiveseven.mdl";
	static constexpr pcchar	FIRE_SFX			= "weapons/fiveseven/fiveseven_fire.wav";
	static constexpr pcchar	FIRE_SFX_SIL		= "weapons/fiveseven/fiveseven_fire-sil.wav";
	static constexpr pcchar	POSTURE				= "onehanded";
	static constexpr float	MAX_SPEED			= 250.0f;
	static constexpr float	DAMAGE				= 24;
	static constexpr float	RANGE_MODIFER		= 1.149658245;	// 80% damage @800 inches.
	static constexpr float	RELOAD_TIME			= 1.94F;
	static constexpr float	RELOAD_EMPTY_TIME	= 1.86F;
	static constexpr float	DRAW_FIRST_TIME		= 1.56F;
	static constexpr float	DEPLOY_TIME			= 0.7F;
	static constexpr float	HOLSTER_TIME		= 0.7F;
	static constexpr float	CHECK_MAGAZINE_TIME	= 2.033F;
	static constexpr float	DASH_ENTER_TIME		= 0.8F;
	static constexpr float	DASH_EXIT_TIME		= 0.37F;
	static constexpr float	BLOCK_UP_TIME		= 0.4666f;
	static constexpr float	BLOCK_DOWN_TIME		= 0.4666f;
	static constexpr float	LHAND_UP_TIME		= 0.2666f;
	static constexpr float	LHAND_DOWN_TIME		= 0.2666f;
	static constexpr float	FIRE_INTERVAL		= 0.132f;
	static constexpr float	EFFECTIVE_RANGE		= 4096.0f;
	static constexpr int	PENETRATION			= 1;
	static constexpr int	GUN_VOLUME			= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.4f;
	static constexpr float	ACCURACY_BASELINE	= 0.92f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_SLIDE_STOP_ANIM = (1 << IDLE) |
												(1 << DEPLOY) |
												(1 << HOLSTER) |
												(1 << CHECK_MAGAZINE) |
												(1 << LHAND_DOWN) | (1 << LHAND_UP) |
												(1 << BLOCK_DOWN) | (1 << BLOCK_UP) |
												(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT) |
												(1 << SH_DASH_ENTER) | (1 << SH_DASHING) | (1 << SH_DASH_EXIT);

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return FiveSevenFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void FiveSevenFire(float flSpread, float flCycleTime = FIRE_INTERVAL);
};

class CUMP45 : public CBaseWeaponTemplate<CUMP45>
{
public:	// Constants / Database
	enum ump45_anim_e
	{
		IDLE,
		SHOOT,
		SHOOT_AIM,
		RELOAD,
		RELOAD_EMPTY,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		CHECK_MAGAZINE,
		TO_SEMI,
		TO_AUTO,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_DOWN,
		LHAND_UP,
		DASH_ENTER,
		DASHING,
		DASH_EXIT
	};

	static constexpr auto	VIEW_MODEL		= "models/weapons/v_ump45.mdl";
	static constexpr auto	WORLD_MODEL		= "models/weapons/w_ump45.mdl";
	static constexpr auto	FIRE_SFX		= "weapons/ump45/ump45_fire.wav";
	static constexpr auto	FIRE_SIL_SFX	= "weapons/ump45/ump45_fire.wav";	// FIXME
	static constexpr auto	POSTURE			= "m249";
	static constexpr float	MAX_SPEED		= 250.0f;
	static constexpr float	DAMAGE			= 32;
	static constexpr float	RANGE_MODIFER	= 1.149658245;	// 80% damage @800 inches.
	static constexpr float	RELOAD_TIME		= 2.8667F;
	static constexpr float	RELOAD_EMPTY_TIME = 3.0667F;
	static constexpr float	DEPLOY_TIME		= 0.6333F;
	static constexpr float	DRAW_FIRST_TIME	= 1.2F;
	static constexpr float	HOLSTER_TIME	= 0.6333F;
	static constexpr float	CHECK_MAGAZINE_TIME	= 3.6F;
	static constexpr float	BLOCK_UP_TIME	= 0.3667F;
	static constexpr float	BLOCK_DOWN_TIME	= 0.3667F;
	static constexpr float	LHAND_DOWN_TIME	= 0.7F;
	static constexpr float	LHAND_UP_TIME	= 0.7F;
	static constexpr float	DASH_ENTER_TIME	= 0.3667F;
	static constexpr float	DASH_EXIT_TIME	= 0.3667F;
	static constexpr float	RPM				= 600.0f;
	static constexpr float	EFFECTIVE_RANGE	= 8192.0f;
	static constexpr int	PENETRATION		= 1;
	static constexpr float	SPREAD_BASELINE	= 0.15f;
	static constexpr int	GUN_VOLUME		= NORMAL_GUN_VOLUME;
	static constexpr float	ACCURACY_BASELINE= 0.4f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual void	PrimaryAttack	(void)	{ return UMP45Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void UMP45Fire(float flSpread, float flCycleTime = 60.0f / RPM);
};

class CPSG1 : public CBaseWeaponTemplate<CPSG1>
{
public:	// Constants / Database
	enum psg1_anim_e
	{
		IDLE,
		SHOOT,
		RELOAD,
		RELOAD_EMPTY,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		CHECK_MAGAZINE,
		BLOCK_UP,
		BLOCK_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		LHAND_DOWN,
		LHAND_UP,
	};

	static constexpr auto	VIEW_MODEL			= "models/weapons/v_psg1.mdl";
	static constexpr auto	WORLD_MODEL			= "models/weapons/w_psg1.mdl";
	static constexpr auto	FIRE_SFX			= "weapons/m14ebr/m14ebr_fire.wav";	// UNDONE
	static constexpr auto	POSTURE				= "rifle";
	static constexpr float	MAX_SPEED			= 210.0f;
	static constexpr float	MAX_SPEED_ZOOM		= 150.0f;
	static constexpr float	DAMAGE				= 90.0f;
	static constexpr float	RANGE_MODIFER		= 1.03443782f;	// 85% damage @2400 inches.
	static constexpr float	FIRE_INTERVAL		= 0.125f;
	static constexpr float	FIRE_ANIMTIME		= 0.4333f;
	static constexpr float	RELOAD_TIME			= 3.0667F;
	static constexpr float	RELOAD_EMPTY_TIME	= 3.8667F;
	static constexpr float	DEPLOY_TIME			= 0.8333F;
	static constexpr float	DRAW_FIRST_TIME		= 2.0333F;
	static constexpr float	HOLSTER_TIME		= 0.8333F;
	static constexpr float	CHECK_MAGAZINE_TIME	= 3.0667F;
	static constexpr float	BLOCK_UP_TIME		= 0.3667F;
	static constexpr float	BLOCK_DOWN_TIME		= 0.3667F;
	static constexpr float	LHAND_UP_TIME		= 0.4333F;
	static constexpr float	LHAND_DOWN_TIME		= 0.4333F;
	static constexpr float	DASH_ENTER_TIME		= 0.3667F;
	static constexpr float	DASH_EXIT_TIME		= 0.3667F;
	static constexpr int	PENETRATION			= 2;
	static constexpr float	EFFECTIVE_RANGE		= 8192.0f;
	static constexpr int	GUN_VOLUME			= BIG_EXPLOSION_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.0025f;
	static constexpr float	ACCURACY_BASELINE	= 0.2f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void) final;
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) final { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam(void) final;
#endif

public:	// basic logic funcs
	void	PrimaryAttack	(void) final { return PSG1Fire(GetSpread()); }
	void	SecondaryAttack	(void) final { return DefaultScopeSight(Vector(-4.9f, 5, 0.5f), 25); }
	bool	Reload			(void) final;
	bool	HolsterStart	(void) final;

public:	// util funcs
	float	GetSpread		(void) final;

public:	// new funcs
	void PSG1Fire(float flSpread, float flCycleTime = FIRE_INTERVAL);
};

#define C4_VIEW_MODEL	"models/weapons/v_c4.mdl"
#define C4_WORLD_MODEL	"models/weapons/w_c4.mdl"
#define C4_PLACED_SFX	"zombieriot/plant.wav"

constexpr float C4_TIME_DRAW			= 0.7f;
constexpr float C4_TIME_HOLSTER			= 0.7f;
constexpr float C4_TIME_THROW			= 1.0333f;
constexpr float C4_TIME_THROW_SPAWN		= 0.5f;
constexpr float C4_TIME_PLACE			= 1.0333f;
constexpr float C4_TIME_PLACE_SPAWN		= 0.5f;
constexpr float C4_TIME_DET_ANIM		= 1.0333f;
constexpr float C4_TIME_DETONATE		= 0.4f;
constexpr float C4_TIME_PICKUP			= 0.7f;
constexpr float C4_TIME_PICKUP_HAPPEN	= 0.3333f;

enum c4_e
{
	C4_IDLE = 0,
	C4_DRAW,
	C4_HOLSTER,
	C4_THROW,
	C4_PLACE,
	C4_DETONATE,
	C4_PICKUP,
};

// Index to typename converter.
template <WeaponIdType iId>
struct _Internal_GetTypename
{
	using result =

		// Pistols
		std::conditional_t < iId == WEAPON_GLOCK18, CG18C,
		std::conditional_t < iId == WEAPON_USP, CUSP,
		std::conditional_t < iId == WEAPON_ANACONDA, CAnaconda,
		std::conditional_t < iId == WEAPON_DEAGLE, CDEagle,
		std::conditional_t < iId == WEAPON_FIVESEVEN, CFN57,
		std::conditional_t < iId == WEAPON_M45A1, CM45A1,

		// Shotgun
		std::conditional_t < iId == WEAPON_KSG12, CKSG12,
		std::conditional_t < iId == WEAPON_M1014, CM1014,
		std::conditional_t < iId == WEAPON_AA12, void,

		// SMGs
		std::conditional_t < iId == WEAPON_MP7A1, CMP7A1,
		std::conditional_t < iId == WEAPON_MAC10, void,
		std::conditional_t < iId == WEAPON_MP5N, void,
		std::conditional_t < iId == WEAPON_UMP45, CUMP45,
		std::conditional_t < iId == WEAPON_P90, void,
		std::conditional_t < iId == WEAPON_VECTOR, void,

		// Assault Rifle
		std::conditional_t < iId == WEAPON_AK47, CAK47,
		std::conditional_t < iId == WEAPON_M4A1, CM4A1,
		std::conditional_t < iId == WEAPON_SCARH, CSCARH,
		std::conditional_t < iId == WEAPON_XM8, CXM8,

		// Sniper Rifle
		std::conditional_t < iId == WEAPON_SRS, void,
		std::conditional_t < iId == WEAPON_SVD, CSVD,
		std::conditional_t < iId == WEAPON_AWP, CAWP,
		std::conditional_t < iId == WEAPON_PSG1, CPSG1,

		// LMGs
		std::conditional_t < iId == WEAPON_MK46, CMK46,
		std::conditional_t < iId == WEAPON_RPD, void,

		// Fallback
		void

		>>>>>>
		>>>
		>>>>>>
		>>>>
		>>>>
		>>;
};

template <WeaponIdType iId>
using GetTypename = typename _Internal_GetTypename<iId>::result;

#ifndef CLIENT_DLL
extern short g_sModelIndexLaser;
extern short g_sModelIndexLaserDot;

extern short g_sModelIndexFireball;
extern short g_sModelIndexSmoke;
extern short g_sModelIndexWExplosion;
extern short g_sModelIndexBubbles;
extern short g_sModelIndexBloodDrop;
extern short g_sModelIndexBloodSpray;
extern short g_sModelIndexSmokePuff;
extern short g_sModelIndexFireball2;
extern short g_sModelIndexFireball3;
extern short g_sModelIndexFireball4;
extern short g_sModelIndexCTGhost;
extern short g_sModelIndexTGhost;
extern short g_sModelIndexC4Glow;

extern short g_sModelIndexRadio;
extern MULTIDAMAGE gMultiDamage;

void WeaponsPrecache();
void FindHullIntersection(const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity);
void AnnounceFlashInterval(float interval, float offset = 0);

void ClearMultiDamage();
void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker);
void AddMultiDamage(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType);
void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage);
int DamageDecal(CBaseEntity *pEntity, int bitsDamageType);
void DecalGunshot(TraceResult *pTrace, int iBulletType, bool ClientOnly, entvars_t *pShooter, bool bHitMetal);
void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype, int entityIndex);
void UTIL_PrecacheOtherWeapon(WeaponIdType iId);
BOOL CanAttack(float attack_time, float curtime, BOOL isPredicted);
primatk_msg_ptr InterpretPrimaryAttackMessage(void);
#endif

#endif	// SHARED_WEAPONS_H
