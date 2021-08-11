/*

Created Date: Jul 30 2021

Modern Warfare Dev Team
	Code - Luna the Reborn

*/

#include "precompiled.h"
#include <functional>
#include <string>
#include <unordered_map>

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

template <typename CWpn>
concept CanSteelSight = requires (CWpn wpn)
{
	{CWpn::AIM_FOV > 0};
	{CWpn::AIM_OFFSET} -> std::convertible_to<Vector>;
	{CWpn::ATTRIB_USE_STEEL_SIGHT == true};
};

template <typename CWpn>
concept CanScopeSight = requires (CWpn wpn)
{
	{CWpn::AIM_FOV > 0};
	{CWpn::AIM_OFFSET} -> std::convertible_to<Vector>;
	{CWpn::ATTRIB_USE_SCOPE_SIGHT == true};
};

template <typename CWpn>
concept IsIdleAnimLooped = requires (CWpn wpn)
{
	{CWpn::IDLE_TIME > 0.0f};
};
#pragma endregion

#pragma region UTILs
// Check hold to aim.
#ifdef CLIENT_DLL
#define IS_HOLD_TO_AIM	((bool)cl_holdtoaim->value)
#else
#define IS_HOLD_TO_AIM	(m_pPlayer->m_bHoldToAim)
#endif
#define AMMUNITION	m_pPlayer->m_rgAmmo[AmmoInfo()->m_iId]
#pragma endregion



#pragma region BaseTemplate class

template <typename CWpn>
struct CWeapon : public IWeapon
{
	using BaseClass = CWeapon<CWpn>;
	using ScriptMap = std::unordered_map<std::string, std::function<void(void)>>;

	enum : int
	{
		TRANSMIT_PLAYBACK_EV = 1,
	};

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
	bool			m_bAllowEmptySound : 1	{ false };	// Used for semi-auto weapon only.
	bool			m_bStartFromEmpty : 1	{ false };	// For tublar weapons. TODO: move to QC.
	float			m_flNextInsertAnim		{ -1.0f };	// For tublar weapons.
	bool			m_bSetForceStopReload:1	{ false };	// For tublar weapons.
	ScriptMap		m_QCScript;

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
	EntityHandle<CWeaponBox>	m_pWeaponBox;	// Aug 10 2021, LUNA: don't know why keeping this. The weaponbox is no longer saving a actual entity. Weapon object gets destoried when dropped on ground.
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

#pragma region General Resets
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ATTRIB_SEMIAUTO))
		{
			if (m_pPlayer->m_afButtonReleased & IN_ATTACK)
				m_bAllowEmptySound = true;	// only one empty sound per time.
		}
#pragma endregion


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
					gHUD::m_SniperScope.SetFadeFromBlack(CWpn::ATTRIB_AIM_FADE_FROM_BLACK);
				}
#endif
			}
		}

#pragma region Weapon post reload handling.
		if constexpr (!IsTubularMag<CWpn>)
		{
			// complete the magazine-based reload.
			if (m_bInReload /* Don't need to check m_pPlayer->m_flNextAttack anymore. It is covered above. */)
			{
				int j = Q_min(WpnInfo()->m_iMaxClip - m_iClip, AMMUNITION);

				// Add them to the clip
				m_iClip += j;
				AMMUNITION -= j;

				// not reloaded from empty? extra 1 bullet.
				if (!(m_bitsFlags & WPNSTATE_RELOAD_EMPTY) && AMMUNITION > 0)
				{
					m_iClip++;
					AMMUNITION--;
				}

				m_bInReload = false;
				m_bitsFlags &= ~WPNSTATE_RELOAD_EMPTY;	// remove it anyway.
			}
		}
		else
		{
			// Tublar weapon - continuous reloading.
			if (m_bInReload)
			{
				if (m_flNextInsertAnim <= gpGlobals->time && m_iClip < WpnInfo()->m_iMaxClip && AMMUNITION > 0)
				{
					Animate(CWpn::INSERT);
					m_pPlayer->SetAnimation(PLAYER_RELOAD);
					PlayReloadSound(m_pPlayer->pev->origin);	// Should be a no-host playing.

					m_flNextInsertAnim = gpGlobals->time + CWpn::TIME_INSERT;
				}

				// SFX for inserting ammo: Moved to QC. Global SFX without host is moved above.

				// Data update for inserting ammo: Moved to QC.

				if (((m_iClip >= WpnInfo()->m_iMaxClip || AMMUNITION <= 0) && m_flNextInsertAnim <= gpGlobals->time)
					|| m_bSetForceStopReload || m_pPlayer->pev->button & (IN_ATTACK | IN_RUN))
				{
					Animate(CWpn::AFTER_RELOAD);
					m_pPlayer->m_flNextAttack = CWpn::TIME_AFTER_RELOAD;
					m_flTimeWeaponIdle = CWpn::TIME_AFTER_RELOAD;

					m_bInReload = false;
					m_bSetForceStopReload = false;
				}
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
		CBaseEntity* pEntity = static_cast<CBaseEntity*>(pObject);
		CBasePlayer* pPlayer = dynamic_cast<CBasePlayer*>(pEntity);

#ifndef CLIENT_DLL
		// how can I add to someone who didn't even exist?
		if (FNullEnt(pPlayer))
		{
			// Is it a weaponbox?
			if (CWeaponBox* pWeaponBox = dynamic_cast<CWeaponBox*>(pEntity); !FNullEnt(pWeaponBox))
			{
				m_pWeaponBox = pWeaponBox;
				m_pPlayer = nullptr;

				return true;
			}

			return false;
		}

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

		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ATTRIB_SEMIAUTO))
			m_bAllowEmptySound = true;

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

	void	Pause(float flTimeAutoResume = -1, bool bEnforceUpdatePauseDatabase = false) override
	{
		// You can use this function to setup an auto-resume.
		if (m_bitsFlags & WPNSTATE_PAUSED && flTimeAutoResume > 0.0f)
		{
			m_bitsFlags |= WPNSTATE_AUTO_RESUME;
			m_flTimeAutoResume = flTimeAutoResume;
		}
		else
			m_bitsFlags &= ~WPNSTATE_AUTO_RESUME;

		if (m_bitsFlags & WPNSTATE_PAUSED && !bEnforceUpdatePauseDatabase)
			return;

		m_bitsFlags |= WPNSTATE_PAUSED;

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

	void	Resume(void) override
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

	void	DefaultShoot(float flSpread = -1.0f, float flCycleTime = -1.0f)
	{
#pragma region Semiauto check.
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ATTRIB_SEMIAUTO))
		{
			if (++m_iShotsFired > 1)
				return;
		}
#pragma endregion

#pragma region Check input variables
		if (flSpread < 0.0f)
			flSpread = GetSpread();

		if (flCycleTime < 0.0f)
		{
			if constexpr (IS_MEMBER_PRESENTED_CPP20_W(RPM))
			{
				flCycleTime = 60.0f / CWpn::RPM;
			}
			else if constexpr (IS_MEMBER_PRESENTED_CPP20_W(FIRE_INTERVAL))
			{
				flCycleTime = CWpn::FIRE_INTERVAL;
			}
			else
			{
				COMPILING_ERROR("One of two fire interval attrib must be provided: \"RPM\" or \"FIRE_INTERVAL\".");
			}
		}
#pragma endregion

#pragma region Underwater check.
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ATTRIB_NO_FIRE_UNDERWATER))
		{
			if (m_pPlayer->pev->waterlevel == 3)
			{
				PlayEmptySound(m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs);
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15f;
				return;
			}
		}
#pragma endregion

#pragma region Magazine check.
		if (m_iClip <= 0)
		{
			PlayEmptySound(m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs);
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;

#ifndef CLIENT_DLL
			if (TheBots)
			{
				TheBots->OnEvent(EVENT_WEAPON_FIRED_ON_EMPTY, m_pPlayer);
			}
#endif

			return;
		}
#pragma endregion

		m_iClip--;

#pragma region Server side visual effects
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(GUN_FLASH))
		{
			m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
			m_pPlayer->m_iWeaponFlash = CWpn::GUN_FLASH;
		}

		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(GUN_VOLUME))
			m_pPlayer->m_iWeaponVolume = CWpn::GUN_VOLUME;
		else
			m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
#pragma endregion

#pragma region Fire bullets et Playback event
		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

		void* pParam = nullptr;

		if constexpr (IsShotgun<CWpn>)
		{
			pParam = calloc(1, sizeof(int));

			*(int*)pParam = m_pPlayer->FireBuckshots(
				CWpn::PROJECTILE_COUNT,
				m_pPlayer->GetGunPosition(),
				gpGlobals->v_forward,
				CWpn::CONE_VECTOR,
				CWpn::EFFECTIVE_RANGE,
				CWpn::DAMAGE,
				CWpn::RANGE_MODIFIER,
				m_pPlayer->random_seed
			);
		}
		else
		{
			pParam = calloc(1, sizeof(Vector2D));

			*(Vector2D*)pParam = m_pPlayer->FireBullets3(
				m_pPlayer->GetGunPosition(),
				gpGlobals->v_forward,
				flSpread,
				CWpn::EFFECTIVE_RANGE,
				CWpn::PENETRATION,
				AmmoInfo()->m_iId,
				CWpn::DAMAGE,
				CWpn::RANGE_MODIFER,
				m_pPlayer->random_seed
			);
		}

		// Playback event moved to here.
		Transmit(TRANSMIT_PLAYBACK_EV, pParam);
#pragma endregion

#pragma region Client visual effects
		// Totally moved to QC to decide.
#pragma endregion

#pragma region Apply time defer to next attacks.
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(FIRE_ANIMTIME))
		{
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + CWpn::FIRE_ANIMTIME;
		}
		else
		{
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flCycleTime + 2.0f;
		}
#pragma endregion

#pragma region Apply recoils.
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ApplyRecoil))
		{
			This()->ApplyRecoil();
		}
#pragma endregion
	}

	void	PrimaryAttack(void) override
	{
		DefaultShoot();
	}

	void	DefaultSteelSight(const Vector& vecOfs, int iFOV, float flDriftingSpeed = 10, float flNextSecondaryAttack = 0.3)
	{
		m_bInZoom = !m_bInZoom;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flNextSecondaryAttack;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flNextSecondaryAttack;	// set/reset the gun bob amp.

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

		if (!g_vecGunOfsGoal.LengthSquared())
		{
			g_vecGunOfsGoal = vecOfs;
			gHUD::m_iFOV = iFOV;	// allow clients to predict the zoom.
		}
		else
		{
			g_vecGunOfsGoal = g_vecZero;
			gHUD::m_iFOV = DEFAULT_FOV;
		}

		// this model needs faster.
		g_flGunOfsMovingSpeed = flDriftingSpeed;
#else
	// just zoom a liiiiittle bit.
	// this doesn't suffer from the same bug where the gunofs does, since the FOV was actually sent from SV.
		if (m_bInZoom)
		{
			m_pPlayer->pev->fov = iFOV;
			EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, "weapons/steelsight_in.wav", 0.75f, ATTN_STATIC);
		}
		else
		{
			m_pPlayer->pev->fov = DEFAULT_FOV;
			EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, "weapons/steelsight_out.wav", 0.75f, ATTN_STATIC);
		}
#endif
	}

	void	DefaultScopeSight(const Vector& vecOfs, int iFOV, float flEnterScopeDelay = 0.25, float flDriftingSpeed = 10, float flNextSecondaryAttack = 0.3)
	{
		// this is the delay for the m_bResumeZoom.
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + flEnterScopeDelay;

		if (static_cast<int>(m_pPlayer->pev->fov) != DEFAULT_FOV)
		{
			m_pPlayer->pev->fov = DEFAULT_FOV;

#ifdef CLIENT_DLL
			// zoom out anim.
			g_vecGunOfsGoal = g_vecZero;

			// manually set fade.
			if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ATTRIB_AIM_FADE_FROM_BLACK))
				gHUD::m_SniperScope.SetFadeFromBlack(CWpn::ATTRIB_AIM_FADE_FROM_BLACK, 0);
#endif
		}
		else
		{
			// get ready to zoom in.
			m_pPlayer->m_iLastZoom = iFOV;
			m_pPlayer->m_bResumeZoom = true;

#ifdef CLIENT_DLL
			// zoom in anim.
			g_vecGunOfsGoal = vecOfs;
#endif
		}

#ifndef CLIENT_DLL
		if (TheBots)
		{
			TheBots->OnEvent(EVENT_WEAPON_ZOOMED, m_pPlayer);
		}

		// SFX only emitted from SV.
		EMIT_SOUND(m_pPlayer->edict(), CHAN_ITEM, "weapons/zoom.wav", 0.2, 2.4);
#else
		g_flGunOfsMovingSpeed = flDriftingSpeed;
#endif

		// slow down while we zooming.
		m_pPlayer->ResetMaxSpeed();

		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flNextSecondaryAttack;
	}

	void	Aim(void) override
	{
		if constexpr (CanSteelSight<CWpn>)
		{
			DefaultSteelSight(CWpn::AIM_OFFSET, CWpn::AIM_FOV);
		}
		else if constexpr (CanScopeSight<CWpn>)
		{
			DefaultScopeSight(CWpn::AIM_OFFSET, CWpn::AIM_FOV);
		}
	}

	void	Idle(void) override
	{
#ifdef CLIENT_DLL
		CalcBobParam(g_flGunBobOmegaModifier, g_flGunBobAmplitudeModifier);
#endif
		// the priority of these anims:
		// 1. Running first. You can't be BLOCKED during a RUN.
		// 2. Block. You can't aimming if you are blocked.
		// 3. Aim.

		if (m_bitsFlags & WPNSTATE_DASHING)
		{
			if constexpr (IsIdleAnimLooped<CWpn>)
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + CWpn::IDLE_TIME;

			Animate(CWpn::IDLE);
		}
		else if ((m_pPlayer->m_afButtonPressed | m_pPlayer->m_afButtonReleased) & IN_BLOCK
			|| (m_pPlayer->pev->button & IN_BLOCK && m_pPlayer->pev->weaponanim == CWpn::IDLE))
		{
			// you can't aim during a BLOCK section.
			if (m_bInZoom || m_pPlayer->pev->fov != DEFAULT_FOV)
				Aim();

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
			Blockage();	// UNDONE, are these two equivlent?
		}
		else if (m_pPlayer->pev->weaponanim != CWpn::IDLE)
		{
			Animate(CWpn::IDLE);
		}
	}

	bool	DefaultMagReload(void) requires(!IsTubularMag<CWpn>)
	{
		if (AMMUNITION <= 0 || m_iClip >= WpnInfo()->m_iMaxClip)
		{
			// KF2 style inspection when you press R and failed reload attempt.
			if constexpr (IS_MEMBER_PRESENTED_CPP20_W(CHECK_MAGAZINE))
			{
				if (m_pPlayer->pev->weaponanim != CWpn::CHECK_MAGAZINE)
				{
					if (m_bInZoom)
						Aim();

					Animate(CWpn::CHECK_MAGAZINE);
					m_flTimeWeaponIdle = CWpn::CHECK_MAGAZINE_TIME;
				}
			}

			return false;
		}

		// exit scope
		if (m_bInZoom || static_cast<int>(m_pPlayer->pev->fov) != DEFAULT_FOV)
			Aim();	// close scope when we reload.

		// 3rd personal anim & SFX
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
		PlayReloadSound(m_pPlayer->pev->origin);

		// reset accuracy data
		m_iShotsFired = 0;
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ACCURACY_BASELINE))
			m_flAccuracy = CWpn::ACCURACY_BASELINE;

		// pause weapon actions
		// Aug 06 2021, LUNA: Soft delay function moved to QC.
		m_pPlayer->m_flNextAttack = m_iClip ? CWpn::RELOAD_TIME : CWpn::RELOAD_EMPTY_TIME;
		m_flTimeWeaponIdle = m_iClip ? CWpn::RELOAD_TIME : CWpn::RELOAD_EMPTY_TIME;
		m_flNextPrimaryAttack = m_iClip ? CWpn::RELOAD_TIME : CWpn::RELOAD_EMPTY_TIME;
		m_flNextSecondaryAttack = m_iClip ? CWpn::RELOAD_TIME : CWpn::RELOAD_EMPTY_TIME;
		m_bInReload = true;

		// 1st personal anim
		Animate(m_iClip ? CWpn::RELOAD : CWpn::RELOAD_EMPTY);

		// it's currently useless.. but let's do it anyway.
		if (!m_iClip)
			m_bitsFlags |= WPNSTATE_RELOAD_EMPTY;

		return true;
	}

	bool	DefaultTublarReload(void) requires(IsTubularMag<CWpn>)
	{
		if (m_iClip >= WpnInfo()->m_iMaxClip || AMMUNITION <= 0 || m_pPlayer->pev->button & IN_ATTACK)	// you just can't hold ATTACK and attempts reload.
		{
			// KF2 style.
			if (m_iClip <= 0 && m_pPlayer->pev->weaponanim != CWpn::INSPECTION)	// inspection anim.
			{
				Animate(CWpn::INSPECTION);
				m_flTimeWeaponIdle = CWpn::INSPECTION_TIME;
			}
			else if (m_iClip > 0 && m_pPlayer->pev->weaponanim != CWpn::CHECK_MAGAZINE)
			{
				Animate(CWpn::CHECK_MAGAZINE);
				m_flTimeWeaponIdle = CWpn::CHECK_MAGAZINE_TIME;
			}

			return false;
		}

		if (m_bInZoom)
			Aim();	// close scope when we reload.

		m_iShotsFired = 0;
		m_bInReload = true;
		m_bStartFromEmpty = !!(m_iClip <= 0);
		m_pPlayer->m_flNextAttack = 0;
		m_flTimeWeaponIdle = m_bStartFromEmpty ? CWpn::TIME_START_RELOAD_FIRST : CWpn::TIME_START_RELOAD;
		m_flNextInsertAnim = gpGlobals->time + (m_bStartFromEmpty ? CWpn::TIME_START_RELOAD_FIRST : CWpn::TIME_START_RELOAD);

		Animate(m_bStartFromEmpty ? CWpn::START_RELOAD_FIRST : CWpn::START_RELOAD);
		return true;
	}

	bool	Reload(void) override
	{
		if (m_bInReload)
			return false;	// What the fuck, man?

		if constexpr (IsTubularMag<CWpn>)
			return DefaultTublarReload();
		else
			return DefaultMagReload();
	}

	bool	Melee(void) override
	{
		// you just.. can't do this.
		if (m_bitsFlags & WPNSTATE_BUSY)
			return false;

		if (m_bInZoom)
			Aim();

		// Save the current state of anim.
		Pause();

		BasicKnife::Deploy(this);
		BasicKnife::Swing();
		return true;
	}

	bool	AlterAct(void)	override {}	// Avoid calling null virtual function.

	bool	Blockage(void)	override	// UNDONE, this should be a function both checking block status and playing animation.
	{
		// we are using "play anim mid-way" method.
		// go check CBaseWeapon::DefaultDashEnd() for detailed commentary.
		bool bBlocked = !!(m_pPlayer->pev->button & IN_BLOCK);

		if (bBlocked && m_pPlayer->pev->weaponanim != CWpn::BLOCK_UP)
		{
#ifdef CLIENT_DLL
			// time to play iEnterAnim.
			if (m_pPlayer->pev->weaponanim == CWpn::BLOCK_DOWN)
			{
				float flAlreadyPlayed = gEngfuncs.GetClientTime() - g_flTimeViewModelAnimStart;

				Animate(CWpn::BLOCK_UP);

				if (flAlreadyPlayed >= CWpn::BLOCK_DOWN_TIME)
					return;

				float flAlreadyPlayedRatio = flAlreadyPlayed / CWpn::BLOCK_DOWN_TIME;

				g_flTimeViewModelAnimStart -= CWpn::BLOCK_UP_TIME * (1.0f - flAlreadyPlayedRatio);
			}
			else
#endif
			{
				Animate(CWpn::BLOCK_UP);
			}
		}

		// therefore, you should not keep calling it in Think() or Frame().
		else if (!bBlocked && m_pPlayer->pev->weaponanim != CWpn::BLOCK_DOWN)
		{
#ifdef CLIENT_DLL
			if (m_pPlayer->pev->weaponanim == CWpn::BLOCK_UP)
			{
				float flAlreadyPlayed = gEngfuncs.GetClientTime() - g_flTimeViewModelAnimStart;

				Animate(CWpn::BLOCK_DOWN);

				if (flAlreadyPlayed >= CWpn::BLOCK_UP_TIME)
					return;

				float flAlreadyPlayedRatio = flAlreadyPlayed / CWpn::BLOCK_UP_TIME;

				g_flTimeViewModelAnimStart -= CWpn::BLOCK_DOWN_TIME * (1.0f - flAlreadyPlayedRatio);
			}
			else
#endif
			{
				Animate(CWpn::BLOCK_DOWN);
			}
		}
	}

	bool	HolsterStart(void) override
	{
		m_pPlayer->m_flEjectBrass = 0;	// prevents famous AWP-shell-ejecting bug.

		Animate(CWpn::HOLSTER);
		m_pPlayer->m_flNextAttack = CWpn::HOLSTER_TIME;
		m_bitsFlags |= WPNSTATE_HOLSTERING;

		return true;
	}

	void	Holstered(void)	override	// TODO: Merge more holster stuff inside?
	{
		// if user is insist to holster weapon, we should allow that.
		// LUNA: no longer check CanHolster() before Holster().

		m_bInReload = false;
		m_bInZoom = false;

#ifdef CLIENT_DLL
		g_vecGunOfsGoal = Vector::Zero();
		g_flGunOfsMovingSpeed = 10.0f;
#endif // CLIENT_DLL

		m_pPlayer->pev->viewmodel = 0;
		m_pPlayer->pev->weaponmodel = 0;
		m_pPlayer->pev->fov = DEFAULT_FOV;
	}

	void	DashStart(void) override
	{
		if (m_bInReload)
			m_bInReload = false;

		if (m_bInZoom || m_pPlayer->pev->fov < DEFAULT_FOV)
		{
#ifndef CLIENT_DLL
			Aim();
#else
			g_vecGunOfsGoal = g_vecZero;
			g_flGunOfsMovingSpeed = 10.0f;
			gHUD::m_iFOV = DEFAULT_FOV;
#endif
		}

		Animate(CWpn::DASH_ENTER);
		m_pPlayer->m_flNextAttack = CWpn::DASH_ENTER_TIME;
		m_flTimeWeaponIdle = CWpn::DASH_ENTER_TIME;
		m_bitsFlags |= WPNSTATE_DASHING;
	}

	void	DashEnd(void) override
	{
		if (m_pPlayer->m_flNextAttack > 0.0f && m_pPlayer->pev->weaponanim == CWpn::DASH_ENTER)
		{
			// this is how much you procees to the dashing phase.
			// for example, assuming the whole length is 1.0s, you start 0.7s and decide to cancel.
			// although there's only 0.3s to the dashing phase, but turning back still requires another equally 0.7s.
			// "m_pPlayer->m_flNextAttack" is the 0.3s of full length. you need to get the rest part, i.e. the 70%.
			float flRunStartUnplayedRatio = 1.0f - m_pPlayer->m_flNextAttack / CWpn::DASH_ENTER_TIME;

			// stick on the last instance in the comment: 70% * 1.0s(full length) = 0.7s, this is the time we need to turning back.
			float flRunStopTimeLeft = CWpn::DASH_EXIT_TIME * flRunStartUnplayedRatio;

			// play the anim.
			Animate(CWpn::DASH_EXIT);

#ifdef CLIENT_DLL
			// why we are using the "0.3s" here?
			// this is because the g_flTimeViewModelAnimStart actually means how much time had passed since the anim was ordered to play.
			// if we need to play 0.7s, we have to told system we only played it for 0.3s. right?
			g_flTimeViewModelAnimStart = gEngfuncs.GetClientTime() - (CWpn::DASH_EXIT_TIME - flRunStopTimeLeft);
#endif

			// force everything else to wait.
			m_pPlayer->m_flNextAttack = m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = flRunStopTimeLeft;
		}

		// if RUN_START is normally played and finished, go normal.
		else
		{
			Animate(CWpn::DASH_EXIT);
			m_pPlayer->m_flNextAttack = CWpn::DASH_EXIT_TIME;
			m_flTimeWeaponIdle = CWpn::DASH_EXIT_TIME;
		}

		// either way, we have to remove this flag.
		m_bitsFlags &= ~WPNSTATE_DASHING;

#ifdef CLIENT_DLL
		// remove the exaggerating shaking vfx.
		g_flGunBobAmplitudeModifier = 1.0f;
#endif
	}

	bool	FreeupLeftHand(void) override
	{
		if (!(m_bitsFlags & WPNSTATE_NO_LHAND))
		{
			Pause();

			Animate(CWpn::LHAND_DOWN);
			m_pPlayer->m_flNextAttack = CWpn::LHAND_DOWN_TIME;
			m_flTimeWeaponIdle = CWpn::LHAND_DOWN_TIME;
			m_bitsFlags |= WPNSTATE_NO_LHAND;

			return true;
		}

		return false;
	}

	bool	ReachLeftHand(void) override
	{
		if (m_bitsFlags & WPNSTATE_NO_LHAND)
		{
			Pause(CWpn::LHAND_UP_TIME);	// Setup an auto-resume timer.

			Animate(CWpn::LHAND_UP);
			m_pPlayer->m_flNextAttack = CWpn::LHAND_UP_TIME;
			m_flTimeWeaponIdle = CWpn::LHAND_UP_TIME;
			m_bitsFlags &= ~WPNSTATE_NO_LHAND;

			return true;
		}

		return false;
	}

	bool	Detach(void) override
	{
		m_flNextPrimaryAttack = 0;
		m_flNextSecondaryAttack = 0;
		m_flTimeWeaponIdle = 0;
		m_bitsFlags = 0;
		m_bInReload = 0;
		m_iShotsFired = 0;
		m_flDecreaseShotsFired = 0;
		m_flAccuracy = 0;
		m_flLastFire = 0;
		m_bInZoom = 0;
		m_iVariation = Role_UNASSIGNED;
		m_bDelayRecovery = 0;
		m_flTimeAutoResume = 0;
		m_bAllowEmptySound = 0;
		m_bStartFromEmpty = 0;
		m_flNextInsertAnim = 0;
		m_bSetForceStopReload = 0;

		Q_memset(&m_Stack, NULL, sizeof(m_Stack));

#ifndef CLIENT_DLL
		// SV exclusive variables.
		m_pPlayer = nullptr;		// one of these two must be valid. or this weapon will be removed.
		m_pWeaponBox = nullptr;
		m_iClientClip = -1;	// Force these value to be updated when attach to a new player.
		m_iClientWeaponState = -1;
#else
		// CL exclusive variables.
		// UNDONE: Never reset player on client side. Should be just a simple deletion.
		m_flBlockCheck = 9999;
		m_vecBlockOffset = Vector::Zero();
#endif

		return true;
	}

	bool	Drop(void** ppWeaponBoxReturned = nullptr) override
	{
#ifndef CLIENT_DLL
		// LUNA: you cannot reject drop request here. it depents on player/other functions.

		// LUNA: you can't call it here. You have to wait it called in pWeaponBox->PackWeapon(). or this would result in packing weapon failure.
		//RemovePlayerItem(pWeapon);

		UTIL_MakeVectors(m_pPlayer->pev->angles);

		CWeaponBox* pWeaponBox = dynamic_cast<CWeaponBox*>(CBaseEntity::Create("weaponbox", m_pPlayer->pev->origin + gpGlobals->v_forward * 10, m_pPlayer->pev->angles, m_pPlayer->edict()));
		pWeaponBox->pev->angles.x = 0;
		pWeaponBox->pev->angles.z = 0;
		pWeaponBox->SetThink(&CWeaponBox::Kill);
		pWeaponBox->pev->nextthink = gpGlobals->time + item_staytime.value;
		pWeaponBox->PackWeapon(this);	// !this call would detach weapon from CBasePlayer databse and attack it to CWeaponBox database.

		if (m_pPlayer->IsAlive())	// drop by intense
			pWeaponBox->pev->velocity = m_pPlayer->pev->velocity + gpGlobals->v_forward * 300 + gpGlobals->v_forward * 100;	// this velocity would be override soon.
		else
		{
			pWeaponBox->pev->velocity = m_pPlayer->pev->velocity * 0.85f;	// drop due to death.
		}

		// if this item is using bpammo instead of magzine, we should pack all player bpammo.
		if (WpnInfo()->m_bitsFlags & ITEM_FLAG_EXHAUSTIBLE)
		{
			if (AmmoInfo()->m_iId > AMMO_NONE && AmmoInfo()->m_iId < AMMO_MAXTYPE)
			{
				pWeaponBox->GiveAmmo(m_pPlayer->m_rgAmmo[AmmoInfo()->m_iId], AmmoInfo()->m_iId);
				m_pPlayer->m_rgAmmo[AmmoInfo()->m_iId] = 0;
			}
		}

		const char* modelname = CWeaponBox::GetCSModelName(Id());
		if (modelname)
		{
			pWeaponBox->SetModel(modelname);
		}

		// if the caller is inquiring CWeaponBox, give it to him.
		if (ppWeaponBoxReturned != nullptr)
		{
			(*ppWeaponBoxReturned) = pWeaponBox;
		}

		// have to re-zero m_pPlayer.
		// otherwise, since CBasePlayer::RemovePlayerItem() had it removed from m_rgpPlayerItems[], CBaseWeapons::WeaponsThink() would consider this is an error weapon.
		m_pPlayer = nullptr;
#endif

		Kill();
		return true;

	}

	bool	Kill(void) override
	{
#ifndef CLIENT_DLL
		if (m_pPlayer.IsValid())
		{
			m_pPlayer->RemovePlayerItem(this);
			m_pPlayer = nullptr;
		}

		if (m_pWeaponBox.IsValid())
		{
			m_pWeaponBox = nullptr;
		}
#else
		m_pPlayer = nullptr;
#endif

		m_bitsFlags |= WPNSTATE_DEAD;
		return true;
	}

#pragma endregion

#pragma region Resource Handling.
	bool	PackData(void* pDatabase) override
	{ 
		return false;
	}

	bool	ParseData(void* pDatabase) override
	{
		return false;
	}

	bool	Transmit(int iType, void* pParameters) override
	{
		switch (iType)
		{
		case TRANSMIT_PLAYBACK_EV:
		{
			// This can only happen on server side.
#ifndef CLIENT_DLL

			// Shotgun.
			if constexpr (IsShotgun<CWpn> && HasEvent<CWpn>)
			{
				auto piSeedOfs = static_cast<int*>(pParameters);

				PLAYBACK_EVENT_FULL(
					FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL,
					m_pPlayer->edict(),
					CWpn::m_usEvent,	// Luna: the prefix 'template' here is not declaring a new template, it to explict state that we are referring the templated variable.
					0,	// No delay.
					m_pPlayer->GetGunPosition(),
					m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle,
					*piSeedOfs, 0.0f,	// Only param 1 is used.
					m_pPlayer->random_seed,
					m_iVariation,
					m_iClientClip > 0,	// This variable is what client reports.
					m_bInZoom
				);
			}

			// Regular weapon.
			else if constexpr (HasEvent<CWpn>)
			{
				auto pvSpread = static_cast<Vector2D*>(pParameters);

				PLAYBACK_EVENT_FULL(
					FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL,
					m_pPlayer->edict(),
					CWpn::m_usEvent,	// Luna: the prefix 'template' here is not declaring a new template, it to explict state that we are referring the templated variable.
					0,	// No delay.
					m_pPlayer->GetGunPosition(),
					m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle,
					pvSpread->x, pvSpread->y,
					m_pPlayer->random_seed,
					m_iVariation,
					m_iClientClip > 0,	// This variable is what client reports.
					m_bInZoom
				);
			}
#endif
			goto LAB_TRANSMIT_FREE_PARAM_AND_RETURN;
		}

		default:
			free(pParameters);
			return false;
		}

	LAB_TRANSMIT_FREE_PARAM_AND_RETURN:;
		free(pParameters);
		return true;
	}

	void	Precache(void)
	{
#ifndef CLIENT_DLL
		PRECACHE_MODEL(CWpn::VIEW_MODEL);
		PRECACHE_MODEL(CWpn::WORLD_MODEL);
		PRECACHE_SOUND(CWpn::FIRE_SFX);

		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(EVENT_FILE))
			m_usEvent<CWpn> = PRECACHE_EVENT(1, CWpn::EVENT_FILE);

		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(SHELL_MODEL))
			m_iShell<CWpn> = PRECACHE_MODEL(CWpn::SHELL_MODEL);
#endif // !CLIENT_DLL
	}

#pragma endregion

#pragma region Model Handling.
	bool	ShouldInvertMdl(void) override { return false; }	// Most IB's model doesn't need this.
	
	int		CalcBodyParts(void) override { return 0; }	// Determind on classes.

	void	CalcBobParam(double& flOmegaModifier, double& flAmplitudeModifier) override
	{
		// normally we don'touch any Omega stuff.
		flOmegaModifier = 1.0;

		// less shake when scoping.
		if (m_bInZoom)
			flAmplitudeModifier = 0.35;

		// common sense: running will cause your hands shake more.
		else if (m_bitsFlags & WPNSTATE_DASHING)
			flAmplitudeModifier = 10.0;

		// normal walking shake.
		else
			flAmplitudeModifier = 0.75;
	}

	bool	StudioEvent(const struct mstudioevent_s* pEvent) override
	{
#ifdef CLIENT_DLL
		if (pEvent->event == 9527)
		{
			std::vector<char*> rgszTokens;
			UTIL_Split(pEvent->options, rgszTokens, ";");

			for (auto& token : rgszTokens)
			{
				if (m_QCScript.find(token) == m_QCScript.end())
				{
#pragma region m_bitsFlags
					if (auto p = Q_strstr(token, "m_bitsFlags &= ~"); p != nullptr)
					{

#define FLAG_REG(x)	else if (!Q_strcmp(p, #x)) bitFlag = x

						p += strlen_c("m_bitsFlags &= ~");

						uint32 bitFlag = 0U;

						if (0)	RANDOM_LONG(0, 1);	// Placeholder.

						FLAG_REG(WPNSTATE_AUTO_LAND_UP);
						FLAG_REG(WPNSTATE_AUTO_RESUME);
						FLAG_REG(WPNSTATE_BUSY);
						FLAG_REG(WPNSTATE_DASHING);
						FLAG_REG(WPNSTATE_DEAD);
						FLAG_REG(WPNSTATE_DRAW_FIRST);
						FLAG_REG(WPNSTATE_HOLSTERING);
						FLAG_REG(WPNSTATE_MELEE);
						FLAG_REG(WPNSTATE_NO_LHAND);
						FLAG_REG(WPNSTATE_PAUSED);
						FLAG_REG(WPNSTATE_QT_EXIT);
						FLAG_REG(WPNSTATE_QT_RELEASE);
						FLAG_REG(WPNSTATE_QT_SHOULD_SPAWN);
						FLAG_REG(WPNSTATE_QUICK_THROWING);
						FLAG_REG(WPNSTATE_RELOAD_EMPTY);
						FLAG_REG(WPNSTATE_SHIELD_DRAWN);
						FLAG_REG(WPNSTATE_SPECIAL_STATE);
						FLAG_REG(WPNSTATE_VISUAL_FREEZED);
						FLAG_REG(WPNSTATE_XM8_CHANGING);

						else
							throw;

						m_QCScript[token] = [this, bitFlag](void) { m_bitsFlags &= ~bitFlag; };
#undef FLAG_REG
					}
#pragma endregion

#pragma region m_pPlayer->m_flNextAttack
					else if (auto p = Q_strstr(token, "m_pPlayer->m_flNextAttack = "); p != nullptr)
					{
						float flValue = std::atof()
					}
#pragma endregion
				}

				m_QCScript[token]();
			}

			return true;
		}

#endif // CLIENT_DLL

		return false;
	}

#pragma endregion

#pragma region Private to this template.
	private:
		CWpn* _pThis{ nullptr };
		inline CWpn* This() { if (!_pThis) _pThis = dynamic_cast<CWpn*>(this); return _pThis; }
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
			iter = _lstWeapons.erase(iter);
		}
		else
		{
#ifndef CLIENT_DLL
			(*iter)->BackgroundFrame(g_flTrueServerFrameRate);
#else
			(*iter)->BackgroundFrame(g_flClientTimeDelta);
#endif // !CLIENT_DLL

			iter++;
		}
	}
}

IWeapon* IWeapon::Give(WeaponIdType iId, void* pPlayer, int iClip, unsigned bitsFlags)
{
	return nullptr;
}

#pragma endregion