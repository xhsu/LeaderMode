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
	double			m_flTimeAutoResume		{ -1.0 };

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

	template <DETECT_SHELL_MODEL T = CWpn> static inline int m_iShell = 0;
#ifndef CLIENT_DLL
	template <DETECT_EVENT_FILE T = CWpn> static inline unsigned short m_usEvent = 0;
#endif
#pragma endregion

#pragma region Behaviours.
	void	Think(void) override
	{
		if (m_bitsFlags & WPNSTATE_PAUSED)
		{
			if (m_bitsFlags & WPNSTATE_AUTO_RESUME && m_flTimeAutoResume < UTIL_WeaponTimeBase())
			{
				Resume();
			}

#ifdef CLIENT_DLL
			else if (m_bitsFlags & WPNSTATE_VISUAL_FREEZED)
			{

				g_pViewEnt->curstate.frame = m_Stack.m_flFrame;
				g_pViewEnt->curstate.framerate = m_Stack.m_flFramerate;
				g_flTimeViewModelAnimStart = gEngfuncs.GetClientTime() - m_Stack.m_flTimeAnimStarted;
				g_pViewEnt->curstate.sequence = m_Stack.m_iSequence;	// you have to set all these 2 places at CL side.
				m_pPlayer->pev->weaponanim = m_Stack.m_iSequence;

			}
#endif

			return;
		}

		// Eject shell moved to QC (i.e. StudioEvent())

#pragma region Dash TODO: move to player.
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
#ifdef CLIENT_DLL
			m_pPlayer->SwitchWeapon(m_pPlayer->m_pWpnSwitchingTo);
#else
			m_pPlayer->SwitchWeapon(m_pPlayer->GetItemById(m_pPlayer->m_iWpnSwitchingTo));
#endif
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

	void	BackgroundFrame(double flFrameRate) override
	{
		m_flNextPrimaryAttack = Q_max(0.0, m_flNextPrimaryAttack - flFrameRate);
		m_flNextSecondaryAttack = Q_max(0.0, m_flNextSecondaryAttack - flFrameRate);
		m_flTimeWeaponIdle = Q_max(0.0, m_flTimeWeaponIdle - flFrameRate);
		m_flTimeAutoResume = Q_max(0.0, m_flTimeAutoResume - flFrameRate);
	}

	bool	Attach(void* pObject) override
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pObject);

#ifndef CLIENT_DLL
		// how can I add to someone who didn't even exist?
		if (FNullEnt(pPlayer))
			return false;

		if (pPlayer->m_rgpPlayerItems[WpnInfo()->m_iSlot] != nullptr)	// this player already got one in this slot!
			return false;

		m_pWeaponBox = nullptr;	// make the weaponbox disown me.
#endif
		m_pPlayer = pPlayer;
		m_bitsFlags |= WPNSTATE_DRAW_FIRST;	// play draw_first anim.

		Vary(pPlayer->m_iRoleType);
		return true;
	}

	bool	Deploy(void) override
	{
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ACCURACY_BASELINE))
			m_flAccuracy = CWpn::ACCURACY_BASELINE;

#ifdef CLIENT_DLL
		// Initialize shell model.
		// Technically it needs to be done only once.
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(SHELL_MODEL))
			m_iShell<CWpn> = gEngfuncs.pEventAPI->EV_FindModelIndex(CWpn::SHELL_MODEL);
#endif

#ifndef CLIENT_DLL
		m_pPlayer->pev->viewmodel = MAKE_STRING(CWpn::VIEW_MODEL);
		m_pPlayer->pev->weaponmodel = MAKE_STRING(CWpn::WORLD_MODEL);
#else
		g_pViewEnt->model = gEngfuncs.CL_LoadModel(CWpn::VIEW_MODEL, &m_pPlayer->pev->viewmodel);
#endif // !CLIENT_DLL

		Q_strlcpy(m_pPlayer->m_szAnimExtention, CWpn::POSTURE);
		Animate((m_bitsFlags & WPNSTATE_DRAW_FIRST) ? CWpn::DRAW_FIRST : CWpn::DEPLOY);

		m_pPlayer->m_flNextAttack = (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? CWpn::DRAW_FIRST_TIME : CWpn::DEPLOY_TIME;
		m_flTimeWeaponIdle = (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? CWpn::DRAW_FIRST_TIME : CWpn::DEPLOY_TIME + 0.75f;
		m_flDecreaseShotsFired = gpGlobals->time;
		m_bitsFlags &= ~WPNSTATE_HOLSTERING;	// remove this marker on deploy. !
		m_iShotsFired = 0;

		m_pPlayer->pev->fov = DEFAULT_FOV;
		m_pPlayer->m_iLastZoom = DEFAULT_FOV;
		m_pPlayer->m_bResumeZoom = false;
		m_pPlayer->m_vecVAngleShift = Vector::Zero();

		return true;
	}

	void	Pause(float flTimeAutoResume, bool bEnforceUpdatePauseDatabase) override
	{
		if (m_bitsFlags & WPNSTATE_PAUSED && !bEnforceUpdatePauseDatabase)
			return;

		m_bitsFlags |= WPNSTATE_PAUSED;

		if (flTimeAutoResume > 0.0f)
		{
			m_bitsFlags |= WPNSTATE_AUTO_RESUME;
			m_flTimeAutoResume = flTimeAutoResume;
		}

		m_Stack.m_flEjectBrass			= m_pPlayer->m_flEjectBrass - gpGlobals->time;
		m_Stack.m_flNextAttack			= m_pPlayer->m_flNextAttack;
		m_Stack.m_flNextPrimaryAttack	= m_flNextPrimaryAttack;
		m_Stack.m_flNextSecondaryAttack	= m_flNextSecondaryAttack;
		m_Stack.m_flTimeWeaponIdle		= m_flTimeWeaponIdle;
		m_Stack.m_iShellModelIndex		= m_pPlayer->m_iShellModelIndex;

#ifdef CLIENT_DLL
		m_Stack.m_flFrame				= g_pViewEnt->curstate.frame;
		m_Stack.m_flFramerate			= g_pViewEnt->curstate.framerate;
		m_Stack.m_flTimeAnimStarted		= gEngfuncs.GetClientTime() - g_flTimeViewModelAnimStart;
		m_Stack.m_iSequence				= g_pViewEnt->curstate.sequence;
#else
		m_Stack.m_iSequence				= m_pPlayer->pev->weaponanim;
#endif // CLIENT_DLL
	}

	void	Resume(void)
	{
		if (!(m_bitsFlags & WPNSTATE_PAUSED))
			return;

		m_pPlayer->m_flEjectBrass		= m_Stack.m_flEjectBrass + gpGlobals->time;
		m_pPlayer->m_flNextAttack		= m_Stack.m_flNextAttack;
		m_flNextPrimaryAttack			= m_Stack.m_flNextPrimaryAttack;
		m_flNextSecondaryAttack			= m_Stack.m_flNextSecondaryAttack;
		m_flTimeWeaponIdle				= m_Stack.m_flTimeWeaponIdle;
		m_pPlayer->pev->weaponanim		= m_Stack.m_iSequence;
		m_pPlayer->m_iShellModelIndex	= m_Stack.m_iShellModelIndex;

#ifdef CLIENT_DLL
		// Same stuff we wrote when handling WPNSTATE_VISUAL_FREEZED.
		g_pViewEnt->curstate.frame		= m_Stack.m_flFrame;
		g_pViewEnt->curstate.framerate	= m_Stack.m_flFramerate;
		g_flTimeViewModelAnimStart		= gEngfuncs.GetClientTime() - m_Stack.m_flTimeAnimStarted;
		g_pViewEnt->curstate.sequence	= m_Stack.m_iSequence;	// you have to set all these 2 places at CL side.
		m_pPlayer->pev->weaponanim		= m_Stack.m_iSequence;
#endif

		// LUNA: I don't know why, but execute this can prevent anim-restart over. This has to be done on both side.
		Animate(m_Stack.m_iSequence);

		// clear stack data, remove the flags
		Q_memset(&m_Stack, NULL, sizeof(m_Stack));
		m_bitsFlags &= ~(WPNSTATE_PAUSED | WPNSTATE_AUTO_RESUME | WPNSTATE_VISUAL_FREEZED);
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

	// Iterate through again, this time run the frame.
	for (auto iter = _lstWeapons.begin(); iter != _lstWeapons.end(); /* nothing */)
	{
#ifndef CLIENT_DLL
		(*iter)->BackgroundFrame(g_flTrueServerFrameRate);
#else
		(*iter)->BackgroundFrame(g_flClientTimeDelta);
#endif // !CLIENT_DLL
	}
}

IWeapon* IWeapon::Give(WeaponIdType iId, void* pPlayer, int iClip, unsigned bitsFlags)
{
	return nullptr;
}

#pragma endregion