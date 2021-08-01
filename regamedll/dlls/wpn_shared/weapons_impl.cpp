/*

Created Date: Jul 30 2021

Modern Warfare Dev Team
	Code - Luna the Reborn

*/

#include "precompiled.h"

#pragma region Detectors
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

CREATE_MEMBER_DETECTOR_CUSTOM(ATTRIB_NO_FIRE_UNDERWATER) { {T::ATTRIB_NO_FIRE_UNDERWATER == true}; };
CREATE_MEMBER_DETECTOR_CUSTOM(ATTRIB_SEMIAUTO) { {T::ATTRIB_SEMIAUTO == true}; };
CREATE_MEMBER_DETECTOR_CUSTOM(ATTRIB_AIM_FADE_FROM_BLACK) { {T::ATTRIB_AIM_FADE_FROM_BLACK > 0.0f}; };


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
#pragma endregion

#pragma region UTILs
// Check hold to aim.
#ifdef CLIENT_DLL
#define IS_HOLD_TO_AIM	((bool)cl_holdtoaim->value)
#else
#define IS_HOLD_TO_AIM	(m_pPlayer->m_bHoldToAim)
#endif
#pragma endregion



#pragma region BaseTemplate class

struct BaseWeapon : public IWeapon
{
#ifndef CLIENT_DLL
	EntityHandle<CBasePlayer>	m_pPlayer;		// one of these two must be valid. or this weapon will be removed.
	EntityHandle<CWeaponBox>	m_pWeaponBox;
#else
	CBasePlayer* m_pPlayer{ nullptr };	// local pseudo-player
#endif

	//-----------------------------------------------------------------------------
	// Purpose:	Behaviours.
	//			These are a series of abilities a weapon able to achieve.
	//-----------------------------------------------------------------------------
#pragma region Behaviours.
	/*
	* Purpose:	Will always be called every frame if this weapon is currently activated. Occurs before PostFrame() as its name indicates.
	* Usage:	Passive event.
	*/
	void	Think(void)	override
	{

	}

	/*
	* Purpose:	Prepare this weapon object to be associated to another superior object.
	* Usage:	Passive event. Should be called from a Player class or WeaponBox class.
	* Return:	'true' when successfully attached.
	*/
	virtual	bool	Attach(void* pObject) = 0;

	/*
	* Purpose:	Grab this weapon out and activate it. This will overwrite the active weapon status but does NOT trigger holster for the previously activated weapon.
	* Usage:	Try player swap function instead.
	* Return:	'true' when successfully deployed.
	*/
	virtual	bool	Deploy(void) = 0;

	/*
	* Purpose:	Pause a weapon's animation and timing function. i.e. freeze a weapon or place a time stop magic on it.
	* Usage:	Anytime when a weapon is activated.
	*/
	virtual	void	Pause(void) = 0;

	/*
	* Purpose:	Revoke the freeze status of a weapon. i.e. undo Pause() function.
	* Usage:	Anytime after a weapon is freezed.
	*/
	virtual	void	Resume(void) = 0;

	/*
	* Purpose:	Called every frame when a weapon is not performing a 'complex behaviour'.
	* Usage:	Passive event.
	* TODO:		This should be merged into Think().
	*/
	virtual void	PostFrame(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Anytime when a weapon is activated.
	*/
	virtual	void	PrimaryAttack(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Anytime when a weapon is activated.
	*/
	virtual	void	Aim(void) = 0;

	/*
	* Purpose:	The fallback passive event if a weapon just doing nothing at all.
	* Usage:	Passive event.
	*/
	virtual void	Idle(void) = 0;

	/*
	* Purpose:	Refill or make a weapon ready to fire again.
	* Usage:	Anytime when a weapon is activated and not reloading.
	* Return:	'true' when a reloading process successfully started.
	*/
	virtual	bool	Reload(void) = 0;

	/*
	* Purpose:	Perform a close-range combat with this weapon. This will cause a Pause() for the weapon.
	* Usage:	Anytime after this weapon is depolyed.
	* Return:	'true' when a melee attack successfully performed.
	*/
	virtual	bool	Melee(void) = 0;

	/*
	* Purpose:	'Use' this weapon in alternative way.
	* Usage:	Anytime after this weapon is deployed.
	* Return:	'true' if its alternative ability successfully performed.
	*/
	virtual bool	AlterAct(void) = 0;

	/*
	* Purpose:	Check whether the weapon is blocked so that it is currently malfunctioning.
	* Usage:	Should be called in pre-phase of every frame.
	* Return:	'true' if this weapon is malfunctioning.
	*/
	virtual	bool	Blockage(void) = 0;

	/*
	* Purpose:	Deactivate a weapon. However, this will not automatically activate another weapon.
	* Usage:	Use swap function in player class instead.
	* Return:	'true' if holstering process started for this weapon.
	*/
	virtual	bool	HolsterStart(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Passive event. Called when a weapon is completely deactivated.
	*/
	virtual	void	Holstered(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Passive event. Called by player class so that weapon object is shifting to dashing status.
	*/
	virtual	void	DashStart(void) = 0;

	/*
	* Purpose:	-
	* Usage:	Passive event. Called by a player class so that weapon object is shifting to normal status.
	*/
	virtual	void	DashEnd(void) = 0;

	/*
	* Purpose:	Pause the weapon and move left hand out of screen.
	* Usage:	Anytime when weapon is activated.
	* Return:	'true' if successfully freed.
	*/
	virtual	bool	FreeupLeftHand(void) = 0;

	/*
	* Purpose:	Resume the weapon and reach the left hand onto handguard again.
	* Usage:	Anytime when left hand is freed up.
	* Return:	'true' if successfully reached.
	*/
	virtual bool	ReachLeftHand(void) = 0;

	/*
	* Purpose:	Detach this object from its owner.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' if successfully detached.
	*/
	virtual bool	Detach(void) = 0;

	/*
	* Purpose:	Drop this weapon to ground.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' if it is successfully dropped on ground.
	*/
	virtual	bool	Drop(void** ppWeaponBoxReturned = nullptr) = 0;

	/*
	* Purpose:	Remove this weapon object.
	* Usage:	Anytime.
	* Return:	'true' if this object is successfully removed.
	*/
	virtual	bool	Kill(void) = 0;
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
	virtual	bool	PackData(void* pDatabase) = 0;

	/*
	* Purpose:	Unpack and override the data from database to local object.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' when successfully unpacked.
	*/
	virtual	bool	ParseData(void* pDatabase) = 0;

	/*
	* Purpose:	Precache all necessary files into buffer.
	* Usage:	Anytime. Careful for the outter game engine timing.
	*/
	virtual	void	Precache(void) = 0;
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
	virtual	bool	ShouldInvertMdl(void) = 0;

	/*
	* Purpose:	Aquire information regarding 'cl_entity_t::curstate.body' and 'edict_t::body'.
	* Usage:	Anytime when this object is valid.
	* Return:	The 'body' value, if applicable.
	*/
	virtual	int		CalcBodyParts(void) = 0;

	/*
	* Purpose:	Aquire information for first-personal view model bobing effect.
	* Usage:	Anytime when this object is valid. Normally called before bob visual is applied.
	* Return:	The multiplier of Omega factor and multiplier of Amplitude factor. Formula: Pos = A * sine(omega * t + phi).
	*/
	virtual	void	CalcBobParam(float& flOmegaModifier, float& flAmplitudeModifier) = 0;

	/*
	* Purpose:	Handling a studio event from model QC.
	* Usage:	Passive event. Called from engine when necessary.
	* Return:	'true' if the event is handled.
	*/
	virtual	bool	StudioEvent(const struct mstudioevent_s* pEvent)
	{

	}
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
	virtual	WeaponIdType Id(void) = 0;

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
	virtual	const struct AmmoInfo* AmmoInfo(void) = 0;
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
	virtual	bool	IsDead(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon is currently dual wielding.
	* Usage:	Anytime when this object is valid.
	* Return:	'true' if this weapon is currently in dual mode.
	*/
	virtual	bool	IsDualWielding(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon is currently in dashing mode.
	* Usage:	Anytime when this object is activated.
	* Return:	'true' if the owner of this weapon is currently dashing.
	* TODO:		This should be a player function.
	*/
	virtual	bool	IsDashing(void) = 0;

	/*
	* Purpose:	Inquire whether the player is aiming.
	* Usage:	Anytime when this object is activated.
	* Return:	'true' if the owner of this weapon is currently aiming.
	*/
	virtual bool	IsAiming(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon is currently being use in a way that shouldn't be interrupted.
	* Usage:	Anytime when this object is activated.
	* Return:	'true' if this weapon is currently busy.
	*/
	virtual	bool	IsBusy(void) = 0;
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
	virtual	bool	CanDepoly(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon can be holstered.
	* Usage:	Anytime when it is valid.
	* Return:	'true' if this object capable to do so.
	*/
	virtual	bool	CanHolster(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon can be dropped.
	* Usage:	Anytime when it is valid.
	* Return:	'true' if this object capable to do so.
	*/
	virtual	bool	CanDrop(void) = 0;

	/*
	* Purpose:	Inquire whether this weapon can be dual wielding.
	* Usage:	Anytime when it is valid.
	* Return:	'true' if this object capable to do so.
	*/
	virtual	bool	CanDualWield(void) = 0;
#pragma endregion

	/*
	* Purpose:	Inquire the current maxium moving speed.
	* Usage:	Anytime when it is valid.
	* Return:	Current maxium speed.
	*/
	virtual	float	GetMaxSpeed(void) = 0;

	/*
	* Purpose:	Insert certain amount of bullets into this weapon. Exceeding bullets would placed into player inventory.
	* Usage:	Anytime when it had attached onto an holder object.
	* Return:	'true' if added successfully.
	*/
	virtual	bool	InsertAmmo(int iCount) = 0;

	/*
	* Purpose:	Play an indexed animation from model sequence.
	* Usage:	Anytime when it is activated.
	*/
	virtual	void	Animate(int iWhat) = 0;

	/*
	* Purpose:	Play sound effect of this firearm when its trigger is being pulled but already emptied.
	* Usage:	Anytime when it is valid.
	*/
	virtual	void	PlayEmptySound(const Vector& vecWhere) = 0;

	/*
	* Purpose:	Play shooting sound effect of this firearm.
	* Usage:	Anytime when it is valid.
	*/
	virtual	void	PlayFireSound(const Vector& vecWhere) = 0;

	/*
	* Purpose:	Play reloading sound effect of this firearm.
	* Usage:	Anytime when it is valid.
	*/
	virtual	void	PlayReloadSound(const Vector& vecWhere) = 0;

	/*
	* Purpose:	Enforcing model displayment on its holder.
	* Usage:	Anytime when it is attached to an object.
	*/
	virtual	void	ResetModel(void) = 0;

	/*
	* Purpose:	Change the varient of this weapon.
	* Usage:	Anytime when it is valid. Called when the role of holder changed or the holder itself changed.
	*/
	virtual	void	Vary(RoleTypes iType) = 0;

	/*
	* Purpose:	Inquire current spread status.
	* Usage:	Anytime when it is valid.
	* Return:	The spread range from 0 to 1. 1 indicates spreading all across screen whereas 0 means no spread radius.
	*/
	virtual	float	GetSpread(void) = 0;
};

template <typename CWpn>
struct CBaseWeapon : public IWeapon
{
	using BaseClass = CBaseWeaponTemplate<CWpn>;

#pragma region Members
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
	// SV exclusive variables.
	EntityHandle<CBasePlayer>	m_pPlayer;		// one of these two must be valid. or this weapon will be removed.
	EntityHandle<CWeaponBox>	m_pWeaponBox;
	int		m_iClientClip		{ 0 };
	int		m_iClientWeaponState{ 0 };
#else
	// CL exclusive variables.
	CBasePlayer*	m_pPlayer		{ nullptr };	// local pseudo-player
	float			m_flBlockCheck	{ 0.0f };
	Vector			m_vecBlockOffset{ g_vecZero };
#endif
#pragma endregion

#pragma region Behaviours.
	void	Think(void) override
	{
		// Eject shell moved to QC (i.e. StudioEvent())

#pragma region Dash
		if (!(m_bitsFlags & WPNSTATE_BUSY) && m_pPlayer->pev->button & IN_RUN && m_pPlayer->pev->button & IN_FORWARD && !(m_pPlayer->pev->flags & FL_DUCKING) && m_pPlayer->pev->flags & FL_ONGROUND)
		{
			DashStart();
		}

		if (m_bitsFlags & WPNSTATE_DASHING &&
			(m_pPlayer->m_afButtonReleased & IN_RUN || !(m_pPlayer->pev->button & IN_FORWARD) || m_pPlayer->pev->flags & FL_DUCKING || !(m_pPlayer->pev->flags & FL_ONGROUND)/* || m_pPlayer->pev->velocity.Length2D() < 50.0f*/)
			)
		{
			DashEnd();
		}
#pragma endregion

		// Useage of equipments moved to player class.

		// PostFrame() was merged.
		if (m_pPlayer->m_flNextAttack > 0)
			return;

		// if we should be holster, then just do it. stop everything else.
		if (m_bitsFlags & WPNSTATE_HOLSTERING)
		{
			m_pPlayer->SwitchWeapon(m_pPlayer->m_pWpnSwitchingTo);
			return;
		}

		// we can't do anything during dash.
		if (m_bitsFlags & WPNSTATE_DASHING)
		{
			if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
				Idle();

			return;
		}

#pragma region Melee
		// since the call of BasicKnife::Swing() block the normal PostFrame() calls, this must be afterwards.
		// so the only thing we need to do here is to resume everything back to normal.
		if (m_bitsFlags & WPNSTATE_MELEE)
		{
			// we can't used a Holster() function for namespace BasicKnife{}, because it is used by all players.
			// we should just remove the melee flag for our weapon.
			m_bitsFlags &= ~WPNSTATE_MELEE;

			// if the player was reloading, then we should back to reload.
			if (m_bInReload)
			{
				ResetModel();	// you have to switch from knife model to gun model.
				Resume();		// then you may resume you anim.
			}
			else
			{
				// or, holster & re-deploy our gun.
				Holstered();
				Deploy();
			}

			// wait for at least one frame.
			return;
		}
#pragma endregion

#pragma region Automatically resume left hand.
		if (m_bitsFlags & (WPNSTATE_NO_LHAND | WPNSTATE_AUTO_LAND_UP) && m_pPlayer->m_flNextAttack <= 0.0f)
		{
			// Hide secondary view model.
#ifdef CLIENT_DLL
			gSecViewModelMgr.m_bVisible = false;
#else
			UTIL_HideSecondaryVMDL(m_pPlayer);
#endif

			ReachLeftHand();	// make LHAND back up.

			m_bitsFlags &= ~WPNSTATE_AUTO_LAND_UP;
			return;	// skip this frame.
		}
#pragma endregion

		// Return zoom level back to previous zoom level before we fired a shot.
		// It could also be used in entering a scope with a delay.
		if (m_flNextPrimaryAttack <= UTIL_WeaponTimeBase())
		{
			if (m_pPlayer->m_bResumeZoom)
			{
				// return the fade level in zoom.
				m_pPlayer->pev->fov = m_pPlayer->m_iLastZoom;
				m_pPlayer->m_bResumeZoom = false;

#ifdef CLIENT_DLL
				// we have additional thing to do on client site.
				if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ATTRIB_AIM_FADE_FROM_BLACK))
				{
					gHUD::m_SniperScope.SetFadeFromBlack(ATTRIB_AIM_FADE_FROM_BLACK);
				}
#endif
			}
		}

#pragma region Mag-feeding weapon post reload handling.
		// complete the magazine-based reload.
		if constexpr (!IsTubularMag<CWpn>)
		{
			if (m_bInReload /* Don't need to check m_pPlayer->m_flNextAttack anymore. It is covered above. */)
			{
				int j = Q_min(WpnInfo()->m_iMaxClip - m_iClip, m_pPlayer->m_rgAmmo[AmmoInfo()->m_iId]);

				// Add them to the clip
				m_iClip += j;
				m_pPlayer->m_rgAmmo[AmmoInfo()->m_iId] -= j;

				// not reloaded from empty? extra 1 bullet.
				if (!(m_bitsFlags & WPNSTATE_RELOAD_EMPTY) && m_pPlayer->m_rgAmmo[AmmoInfo()->m_iId] > 0)
				{
					m_iClip++;
					m_pPlayer->m_rgAmmo[AmmoInfo()->m_iId]--;
				}

				m_bInReload = false;
				m_bitsFlags &= ~WPNSTATE_RELOAD_EMPTY;	// remove it anyway.
			}
		}
#pragma endregion

#pragma region Blockage status check. (CL EXCLUSIVE)
		// Muzzle block check. Prevent players from hiding or peaking between covers/murder holes.
		// That's a DISHONOUR behavior. I want to stop it.
#ifdef CLIENT_DLL
		if (m_flBlockCheck <= gpGlobals->time)
		{
			pmtrace_t tr;
			Vector vecMuzzle = g_pViewEnt->attachment[0]; // this is always the muzzle.
			Vector vecLastMuzzle = g_pparams.vieworg + g_pparams.forward * m_vecBlockOffset.x + g_pparams.right * m_vecBlockOffset.y + g_pparams.up * m_vecBlockOffset.z;

			// LUNA: After moving ItemPostFrame() of client.dll into HUD_Frame(), occurationally g_pViewEnt->attachment[] snap to NaN.
			if (vecMuzzle.IsNaN())
				vecMuzzle = g_rgvecViewModelAttachments[0];	// Using backup database.

			// Why we have to use offsets?
			// If you directly Trace from player eyes to muzzle, once the BLOCK_UP anim is played, the muzzle origin will change due to this new animation.
			// Hence, the BLOCKED flag will imminently be removed because of the BLOCK_UP anim. Which leads to BLOCK_DOWN to be played.
			// It would become a "twitch" on screen.
			if (m_vecBlockOffset.LengthSquared() < 0.1f)
				vecLastMuzzle = vecMuzzle;	// first use, avoid bug.

			// Use custom traceline instead of UTIL_TraceLine().
			// Can't remember why I doing this.
			/*UTIL_TraceLine(g_pparams.vieworg, vecMuzzle, PM_STUDIO_BOX, -1, &tr, m_pPlayer->index, 2);*/

			gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(g_pparams.vieworg, vecLastMuzzle, PM_STUDIO_BOX, -1, &tr);

			// stick into enemy's face?
			int iEntIndex = PM_GetPhysEntInfo(tr.ent);	// the TR result does not directly implies ENTINDEX() in edict or cl_entity. it needs a conversion.
			if (iEntIndex < gEngfuncs.GetMaxClients() && iEntIndex > 0)
				if (g_PlayerExtraInfo[iEntIndex].m_iTeam != g_iTeam)
					tr.fraction = 1.0f;	// "nothing happens here, let's shoot!"

			// the BLOCKED condition is a player attribute.
			bool save = g_bIsBlocked;
			g_bIsBlocked = !!(tr.fraction < 1);

			if (!save && g_bIsBlocked)	// a new blocked situation.
			{
				vecMuzzle -= g_pparams.vieworg;	// become a offset first.
				m_vecBlockOffset.x = DotProduct(vecMuzzle, g_pparams.forward);
				m_vecBlockOffset.y = DotProduct(vecMuzzle, g_pparams.right);
				m_vecBlockOffset.z = DotProduct(vecMuzzle, g_pparams.up);
			}

			// check interval.
			m_flBlockCheck = gpGlobals->time + 0.05f;
		}
#endif
#pragma endregion

#pragma region Blockage handling.
		// handle block situation.
		if ((m_pPlayer->m_afButtonPressed | m_pPlayer->m_afButtonReleased) & IN_BLOCK)
		{
			// let the idle function handle it.
			// we cannot let the block anims interfere other normal anims.
			m_flTimeWeaponIdle = -1;
		}
#pragma endregion

		auto bitsUsableButtons = m_pPlayer->pev->button;

		// LUNA: there are some problems regarding client prediction.
		// sometimes, the client side m_flNextPrimaryAttack and m_flNextSecondaryAttack would be wirely re-zero and induce multiple bullet hole VFX bug.
		// thus, I decide to use message instead. (gmsgShoot and gmsgSteelSight)
		// UPDATE Mar 25: I managed to fix PrimAttack. However, due to many server-exclusive entity, the steelsight still can't be predict on client side.
		// UPDATE Oct 30: Secondary attack (aim) fixed.

		if (!(bitsUsableButtons & IN_BLOCK) && (	// you cannot aim if you are blocked.
			(!IS_HOLD_TO_AIM && bitsUsableButtons & IN_ATTACK2 && m_flNextSecondaryAttack <= UTIL_WeaponTimeBase()) ||	// PRESS to aim
			(IS_HOLD_TO_AIM && ((m_pPlayer->m_afButtonPressed & IN_ATTACK2 && !m_bInZoom) || (m_pPlayer->m_afButtonReleased & IN_ATTACK2 && m_bInZoom)))	// HOLD to aim
			))
		{
			Aim();

			// only cancel this flag in PRESS mode.
			if (!IS_HOLD_TO_AIM)
				m_pPlayer->pev->button &= ~IN_ATTACK2;
		}
		else if ((m_pPlayer->pev->button & IN_ATTACK) && m_flNextPrimaryAttack < UTIL_WeaponTimeBase() && !(bitsUsableButtons & IN_BLOCK))	// UseDecrement()
		{
			// Can't shoot during the freeze period
			// Neither can you if blocked.
			if (m_pPlayer->m_bCanShoot)
			{
				PrimaryAttack();
			}
		}
		else if ((m_pPlayer->pev->button & IN_RELOAD) && WpnInfo()->m_iMaxClip != WEAPON_NOCLIP && !m_bInReload && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
		{
			// reload when reload is pressed, or if no buttons are down and weapon is empty.
			Reload();
		}
		else if (!(bitsUsableButtons & IN_ATTACK))	// no fire buttons down
		{
			// The following code prevents the player from tapping the firebutton repeatedly
			// to simulate full auto and retaining the single shot accuracy of single fire
			if (m_bDelayRecovery)
			{
				m_bDelayRecovery = false;

				if (m_iShotsFired > 15)
					m_iShotsFired = 15;

				m_flDecreaseShotsFired = gpGlobals->time + 0.4;
			}

			// if it's a semi-auto weapon then set the shots fired to 0 after the player releases a button
			if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ATTRIB_SEMIAUTO))
			{
				m_iShotsFired = 0;
			}
			else
			{
				if (m_iShotsFired > 0 && m_flDecreaseShotsFired < gpGlobals->time)
				{
					m_flDecreaseShotsFired = gpGlobals->time + 0.0225f;
					m_iShotsFired--;
				}
			}

			if (!(m_bitsFlags & WPNSTATE_SHIELD_DRAWN))
			{
				// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
				if (!m_iClip && !(WpnInfo()->m_bitsFlags & ITEM_FLAG_NOAUTORELOAD) && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
				{
					Reload();
					return;
				}
			}
		}

		// Removal of first draw flag is up to QC now.

		// Fallback behaviour.
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			Idle();
		}
	}

	bool	Attach(void* pObject) override
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pObject);

#ifndef CLIENT_DLL
		// how can I add to someone who didn't even exist?
		if (FNullEnt(pPlayer))
			return false;

		if (pPlayer->m_rgpPlayerItems[g_rgWpnInfo[m_iId].m_iSlot] != nullptr)	// this player already got one in this slot!
			return false;

		m_pWeaponBox = nullptr;	// make the weaponbox disown me.
		m_pPlayer = pPlayer;
		m_bitsFlags |= WPNSTATE_DRAW_FIRST;	// play draw_first anim.

		Vary(pPlayer->m_iRoleType);
#endif
	}
#pragma endregion
};

#pragma region Interface manager

void IWeapon::TheWeaponsThink(void)
{
	for (auto iter = _lstWeapons.begin(); iter != _lstWeapons.end(); /* nothing */)
	{
		if ((*iter)->IsDead())
		{
			delete (*iter);	// The deleting process contains a erase operation in the list.
			iter = _lstWeapons.begin();	// Hence we have to start it over, for the iterator had been invalided.
		}
		else
			iter++;
	}
}

IWeapon* IWeapon::Give(WeaponIdType iId, void* pPlayer, int iClip, unsigned bitsFlags)
{
	return nullptr;
}

#pragma endregion