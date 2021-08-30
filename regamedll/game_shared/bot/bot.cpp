#include "precompiled.h"

// Nasty Hack.  See client.cpp/ClientCommand()
const char *BotArgs[4] = {};
bool UseBotArgs = false;

CBot::CBot()
{
	// the profile will be attached after this instance is constructed
	m_profile = nullptr;

	// assign this bot a unique ID
	static unsigned int nextID = 1;

	// wraparound (highly unlikely)
	if (nextID == 0)
		++nextID;

	m_id = nextID++;
	m_postureStackIndex = 0;
}

// Prepare bot for action
bool CBot::Initialize(const BotProfile *profile)
{
	m_profile = profile;
	return true;
}

void CBot::Spawn()
{
	// Let CBasePlayer set some things up
	CBasePlayer::Spawn();

	// Server BOT weapon moved to here.
	m_pLastItem = nullptr;

	if (!m_bNotKilled)
	{
		m_rgAmmo.fill(0);
	}

	// Make sure everyone knows we are a bot
	pev->flags |= (FL_CLIENT | FL_FAKECLIENT);

	// Bots use their own thinking mechanism
	SetThink(nullptr);
	pev->nextthink = -1;

	m_flNextBotThink = gpGlobals->time + g_flBotCommandInterval;
	m_flNextFullBotThink = gpGlobals->time + g_flBotFullThinkInterval;
	m_flPreviousCommandTime = gpGlobals->time;

	m_isRunning = true;
	m_isCrouching = false;
	m_postureStackIndex = 0;

	m_jumpTimestamp = 0.0f;

	// Command interface variable initialization
	ResetCommand();

	// Allow derived classes to setup at spawn time
	SpawnBot();
}

Vector CBot::GetAutoaimVector(float flDelta)
{
	UTIL_MakeVectors(pev->v_angle + pev->punchangle);
	return gpGlobals->v_forward;
}

void CBot::PreThink()
{
	for (auto& p : m_rgpPlayerItems)
	{
		m_rgiPlayerItems[p->WpnInfo()->m_iSlot] = p->Id();	// Enforce sync on bot.
	}

	__super::PreThink();
}

void CBot::PostThink()
{
	__super::PostThink();

	// m_flNextAttack doesn't matter here.
	// It's up to the Think() to decide now.

	if (m_pActiveItem && !m_pTank)
		m_pActiveItem->Think();	// Should place it after the player moved?
}

// Add a weapon to the player (Item == Weapon == Selectable Object)
IWeapon* CBot::AddPlayerItem(WeaponIdType iId, int iClip /*= -1*/, unsigned bitsFlags /*= 0*/)
{
	if (iId <= WEAPON_NONE || iId > LAST_WEAPON)
		return nullptr;

	auto pWeapon = IWeapon::Give(iId, this, iClip, bitsFlags);	// Attach() included.

	if (pWeapon)
	{
		CSGameRules()->PlayerGotWeapon(this, pWeapon);

		if (pWeapon->WpnInfo()->m_iSlot == PRIMARY_WEAPON_SLOT)
			m_bHasPrimary = true;

		// enlist into inventory.
		m_rgpPlayerItems[pWeapon->WpnInfo()->m_iSlot] = pWeapon;

		// FX
		EMIT_SOUND(edict(), CHAN_WEAPON, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM);

		// #PROJ_COMMANDER_SHIELD
		// Shield status update should be place into all player.
		// Find a way to update real player as well.
		if (HasShield())
			pev->gamestate = HITGROUP_SHIELD_ENABLED;

		if (CSGameRules()->FShouldSwitchWeapon(this, pWeapon))
		{
			if (!m_bShieldDrawn)
				StartSwitchingWeapon(pWeapon);
		}

		m_iHideHUD &= ~HIDEHUD_WEAPONS;
	}

	return pWeapon;
}

bool CBot::RemovePlayerItem(WeaponIdType iId)
{
	if (m_pActiveItem->Id() == iId)
	{
		ResetAutoaim();
		ResetMaxSpeed();

		// ReGameDLL Fixes: Version 5.16.0.465
		pev->fov = m_iLastZoom = DEFAULT_FOV;
		m_bResumeZoom = false;
		m_pActiveItem = nullptr;

		pev->viewmodel = 0;
		pev->weaponmodel = 0;
	}

	// if item being removed is the last weapon, we should clear the record.
	// because the SelectLastItem() won't check the ownership of last item.
	if (m_pLastItem->Id() == iId)
		m_pLastItem = nullptr;

	// remove from server inventory.
	if (auto pWeapon = HasWeapon(iId); m_rgpPlayerItems[pWeapon->WpnInfo()->m_iSlot] == pWeapon)
	{
		m_rgpPlayerItems[pWeapon->WpnInfo()->m_iSlot] = nullptr;	// however, even if you don't call from CBaseWeapon::Kill(), WeaponsThink() would kill the weapon due to this nullptr assignment.
		return true;
	}

	return false;
}

// Returns the unique ID for the ammo, or -1 if error
bool CBot::GiveAmmo(int iAmount, AmmoIdType iId)
{
	if (iAmount <= 0)
		return false;	// never add air to your inventory.

	if (pev->flags & FL_SPECTATOR)
		return false;

	if (iId <= AMMO_NONE || iId >= AMMO_MAXTYPE)
	{
		// no ammo.
		return false;
	}

	if (!CSGameRules()->CanHaveAmmo(this, iId))	// game rules say I can't have any more of this ammo type.
		return false;

	int iAdd = Q_min(iAmount, GetAmmoInfo(iId)->m_iMax - m_rgAmmo[iId]);
	if (iAdd < 1)
		return false;

	m_rgAmmo[iId] += iAdd;

	// #WPN_UNDONE_CMD
	// Maybe add a method to regular real player such that server may send a message to ask client add ammo?

	return true;
}

bool CBot::StartSwitchingWeapon(IWeapon* pSwitchingTo)
{
	if (!pSwitchingTo)
		return false;

	// #WPN_TODO
	if ((m_pActiveItem && !m_pActiveItem->CanHolster()) /*|| !pSwitchingTo->CanDeploy()*/)
		return false;

	if (m_pActiveItem)
	{
		m_pActiveItem->HolsterStart();
		m_iWpnSwitchingTo = pSwitchingTo->Id();

		return true;
	}
	else
	{
		// no active weapon? which means we can directly deploy this one.
		return SwitchWeapon(pSwitchingTo);
	}
}

bool CBot::SwitchWeapon(IWeapon* pSwitchingTo)
{
	if (!pSwitchingTo)
		return false;

	// #WPN_TODO
	if ((m_pActiveItem && !m_pActiveItem->CanHolster()) /*|| !pSwitchingTo->CanDeploy()*/)
		return false;

	ResetAutoaim();

	if (m_pActiveItem)
	{
		m_pActiveItem->Holstered();
	}

	if (HasShield())
	{
		if (m_pActiveItem)
			m_pActiveItem->Flags() &= ~WPNSTATE_SHIELD_DRAWN;

		m_bShieldDrawn = false;
		UpdateShieldCrosshair(true);
	}

	m_pLastItem = m_pActiveItem;
	m_pActiveItem = pSwitchingTo;

	m_pActiveItem->Deploy();
	// No 'UpdateClientData' anymore. (For weapon).

	ResetMaxSpeed();

	return true;
}

bool CBot::HasAnyWeapon()
{
	for (auto& p : m_rgpPlayerItems)
	{
		if (p->IsDead())
			continue;

		return true;
	}

	return false;
}

IWeapon* CBot::HasWeapon(WeaponIdType iId)
{
	for (auto& p : m_rgpPlayerItems)
	{
		if (p->IsDead())
			continue;

		if (p->Id() == iId)
			return p;
	}

	return nullptr;
}

CWeaponBox* CBot::DropPlayerItem(WeaponIdType iId)
{
	if (iId <= WEAPON_NONE || iId >= LAST_WEAPON)
	{
		// if this string has no length, the client didn't type a name!
		// assume player wants to drop the active item.
		// make the string null to make future operations in this function easier
		iId = WEAPON_NONE;
	}

	auto pWeapon = iId ? HasWeapon(iId) : m_pActiveItem;

	if (pWeapon)
	{
		if (!pWeapon->CanDrop() && IsAlive())
		{
			ClientPrint(pev, HUD_PRINTCENTER, "#Weapon_Cannot_Be_Dropped");
			return nullptr;
		}

		// No more weapon
		if ((pev->weapons & ~(1 << WEAPON_SUIT)) == 0)
			m_iHideHUD |= HIDEHUD_WEAPONS;

		g_pGameRules->GetNextBestWeapon(this, pWeapon);
		UTIL_MakeVectors(pev->angles);

		if (pWeapon->WpnInfo()->m_iSlot == PRIMARY_WEAPON_SLOT)
			m_bHasPrimary = false;

		// the actual drop.
		CWeaponBox* pWeaponBox = nullptr;
		if (!pWeapon->Drop((void**)&pWeaponBox))
			return nullptr;

		return pWeaponBox;
	}

	return nullptr;
}

void CBot::PackDeadPlayerItems()
{
	// get the game rules
	bool bPackGun = (CSGameRules()->DeadPlayerWeapons(this) != GR_PLR_DROP_GUN_NO);
	bool bPackAmmo = (CSGameRules()->DeadPlayerAmmo(this) != GR_PLR_DROP_AMMO_NO);

	if (bPackGun)
	{
		CWeaponBox* pWeaponBox = nullptr;
		AmmoIdType iAmmoId = AMMO_NONE;

		for (int i = 0; i < MAX_ITEM_TYPES; i++)
		{
			// LUNA: in LeaderMod, we pack all weapons.

			if (!m_rgpPlayerItems[i] || !m_rgpPlayerItems[i]->CanDrop())
				continue;

			// save this value. we don't have any access to it after we pack it into CWeaponBox.
			iAmmoId = m_rgpPlayerItems[i]->AmmoInfo()->m_iId;

			if (!m_rgpPlayerItems[i]->Drop((void**)&pWeaponBox))
				continue;

			// drop randomly around player.
			pWeaponBox->pev->origin += Vector(RANDOM_FLOAT(-10, 10), RANDOM_FLOAT(-10, 10), 0);

			// pack some ammo into this weaponbox.
			if (bPackAmmo && iAmmoId != AMMO_NONE)
			{
				pWeaponBox->GiveAmmo(m_rgAmmo[iAmmoId], iAmmoId);
				m_rgAmmo[iAmmoId] = 0;
			}

		}
	}

	RemoveAllItems(true);
}

void CBot::RemoveAllItems(bool removeSuit)
{
	bool bKillProgBar = false;
	int i;

	if (m_pTank)
	{
		m_pTank->Use(this, this, USE_OFF, 0);
		m_pTank = nullptr;
	}

	if (bKillProgBar)
		SetProgressBarTime(0);

	if (m_pActiveItem)
	{
		ResetAutoaim();

		m_pActiveItem->Holstered();	// it's weapon removal anyway.
		m_pActiveItem = nullptr;
	}

	m_pLastItem = nullptr;

	for (i = 0; i < MAX_ITEM_TYPES; i++)
	{
		m_pActiveItem = m_rgpPlayerItems[i];

		if (m_pActiveItem)
		{
			m_pActiveItem->Drop();
		}

		m_rgpPlayerItems[i] = nullptr;
	}

	m_pActiveItem = nullptr;
	m_bHasPrimary = false;

	// ReGameDLL Fixes: Version 5.16.0.465
	// if (m_iFOV != DEFAULT_FOV)
	{
		pev->fov = m_iLastZoom = DEFAULT_FOV;
		m_bResumeZoom = false;
	}

	pev->viewmodel = 0;
	pev->weaponmodel = 0;

	if (removeSuit)
		pev->weapons = 0;
	else
		pev->weapons &= ~WEAPON_ALLWEAPONS;

	for (i = 0; i < MAX_AMMO_SLOTS; i++)
		m_rgAmmo[i] = 0;

	UpdateClientData();

	m_iHideHUD |= HIDEHUD_WEAPONS;

	m_bHasNightVision = false;
	SendItemStatus();
	ResetMaxSpeed();	// ReGameDLL Fixes: Version 5.16.0.465
}

void CBot::SelectLastItem()
{
	// this action can cancel grenade throw.
	if (m_pActiveItem && m_pActiveItem->Flags() & WPNSTATE_QUICK_THROWING)
	{
		m_pActiveItem->Flags() |= WPNSTATE_QT_EXIT;
		m_flNextAttack = 0;
		return;
	}

	if (m_pActiveItem && !m_pActiveItem->CanHolster())
		return;

	if (!m_pLastItem || m_pLastItem == m_pActiveItem)
	{
		for (int i = PRIMARY_WEAPON_SLOT; i < MAX_ITEM_TYPES; i++)
		{
			if (m_rgpPlayerItems[i] && m_rgpPlayerItems[i] != m_pActiveItem)
			{
				m_pLastItem = m_rgpPlayerItems[i];
				break;
			}
		}
	}

	if (!m_pLastItem || m_pLastItem == m_pActiveItem)
		return;

	auto temp = m_pActiveItem;	// save this, and we can set it to last weapon later on.
	StartSwitchingWeapon(m_pLastItem);
	m_pLastItem = temp;
}

void CBot::SelectItem(const char* pstr)
{
	if (!pstr)
	{
		return;
	}

	if (m_pActiveItem && !m_pActiveItem->CanHolster())
		return;

	auto pWeapon = HasWeapon(WeaponClassnameToID(pstr));
	if (!pWeapon || pWeapon == m_pActiveItem)
		return;

	StartSwitchingWeapon(pWeapon);
}

void CBot::BotThink()
{
	if (gpGlobals->time >= m_flNextBotThink)
	{
		m_flNextBotThink = gpGlobals->time + g_flBotCommandInterval;

		Upkeep();

		if (gpGlobals->time >= m_flNextFullBotThink)
		{
			m_flNextFullBotThink = gpGlobals->time + g_flBotFullThinkInterval;

			ResetCommand();
			Update();
		}
		ExecuteCommand();
	}
}

void CBot::MoveForward()
{
	m_forwardSpeed = GetMoveSpeed();
	m_buttonFlags |= IN_FORWARD;

	// make mutually exclusive
	m_buttonFlags &= ~IN_BACK;
}

void CBot::MoveBackward()
{
	m_forwardSpeed = -GetMoveSpeed();
	m_buttonFlags |= IN_BACK;

	// make mutually exclusive
	m_buttonFlags &= ~IN_FORWARD;
}

void CBot::StrafeLeft()
{
	m_strafeSpeed = -GetMoveSpeed();
	m_buttonFlags |= IN_MOVELEFT;

	// make mutually exclusive
	m_buttonFlags &= ~IN_MOVERIGHT;
}

void CBot::StrafeRight()
{
	m_strafeSpeed = GetMoveSpeed();
	m_buttonFlags |= IN_MOVERIGHT;

	// make mutually exclusive
	m_buttonFlags &= ~IN_MOVELEFT;
}

bool CBot::Jump(bool mustJump)
{
	if (IsJumping() || IsCrouching())
		return false;

	if (!mustJump)
	{
		const float minJumpInterval = 0.9f; // 1.5f;
		if (gpGlobals->time - m_jumpTimestamp < minJumpInterval)
			return false;
	}

	// still need sanity check for jumping frequency
	const float sanityInterval = 0.3f;
	if (gpGlobals->time - m_jumpTimestamp < sanityInterval)
		return false;

	// jump
	m_buttonFlags |= IN_JUMP;
	m_jumpTimestamp = gpGlobals->time;
	return true;
}

// Zero any MoveForward(), Jump(), etc
void CBot::ClearMovement()
{
	ResetCommand();
}

// Returns true if we are in the midst of a jump
bool CBot::IsJumping()
{
	// if long time after last jump, we can't be jumping
	if (gpGlobals->time - m_jumpTimestamp > 3.0f)
		return false;

	// if we just jumped, we're still jumping
	if (gpGlobals->time - m_jumpTimestamp < 1.0f)
		return true;

	// a little after our jump, we're jumping until we hit the ground
	if (pev->flags & FL_ONGROUND)
		return false;

	return true;
}

void CBot::Crouch()
{
	m_isCrouching = true;
}

void CBot::StandUp()
{
	m_isCrouching = false;
}

void CBot::UseEnvironment()
{
	m_buttonFlags |= IN_USE;
}

void CBot::PrimaryAttack()
{
	m_buttonFlags |= IN_ATTACK;
}

void CBot::ClearPrimaryAttack()
{
	m_buttonFlags &= ~IN_ATTACK;
}

void CBot::TogglePrimaryAttack()
{
	m_buttonFlags ^= IN_ATTACK;
}

void CBot::SecondaryAttack()
{
	m_buttonFlags |= IN_ATTACK2;
}

void CBot::Reload()
{
	m_buttonFlags |= IN_RELOAD;
}

// Returns ratio of ammo left to max ammo (1 = full clip, 0 = empty)
float CBot::GetActiveWeaponAmmoRatio() const
{
	if (!m_pActiveItem)
		return 0.0f;

	// Weapons with no ammo are always full
	if (m_pActiveItem->Clip() < 0)
		return 1.0f;

	return float(m_pActiveItem->Clip()) / float(m_pActiveItem->WpnInfo()->m_iMaxClip);
}

// Return true if active weapon has an empty clip
bool CBot::IsActiveWeaponClipEmpty() const
{
	if (m_pActiveItem && m_pActiveItem->Clip() == 0)
		return true;

	return false;
}

// Return true if active weapon has no ammo at all
bool CBot::IsActiveWeaponOutOfAmmo() const
{
	if (!m_pActiveItem)
		return true;

	if (m_pActiveItem->Clip() < 0)
		return false;

	if (m_pActiveItem->Clip() == 0 && m_rgAmmo[m_pActiveItem->AmmoInfo()->m_iId] <= 0)
		return true;

	return false;
}

// Return true if looking thru weapon's scope
bool CBot::IsUsingScope() const
{
	// if our field of view is less than 90, we're looking thru a scope (maybe only true for CS...)
	if (pev->fov < 90.0f)
		return true;

	return false;
}

void CBot::ExecuteCommand()
{
	byte adjustedMSec;

	// Adjust msec to command time interval
	adjustedMSec = ThrottledMsec();

	// player model is "munged"
	pev->angles = pev->v_angle;
	pev->angles.x /= -3.0f;

	// save the command time
	m_flPreviousCommandTime = gpGlobals->time;

	if (IsCrouching())
	{
		m_buttonFlags |= IN_DUCK;
	}

	// don't move if frozen state present
	if (pev->flags & FL_FROZEN)
	{
		adjustedMSec = 0;
		ResetCommand();
	}

	// Run the command
	PLAYER_RUN_MOVE(edict(), pev->v_angle, m_forwardSpeed, m_strafeSpeed, m_verticalSpeed, m_buttonFlags, 0, adjustedMSec);
}

void CBot::ResetCommand()
{
	m_forwardSpeed = 0.0f;
	m_strafeSpeed = 0.0f;
	m_verticalSpeed = 0.0f;
	m_buttonFlags = 0;
}

byte CBot::ThrottledMsec() const
{
	int iNewMsec;

	// Estimate Msec to use for this command based on time passed from the previous command
	iNewMsec = int((gpGlobals->time - m_flPreviousCommandTime) * 1000);

	// Doh, bots are going to be slower than they should if this happens.
	// Upgrade that CPU or use less bots!
	if (iNewMsec > 255)
		iNewMsec = 255;

	return byte(iNewMsec);
}

// Returns TRUE if given entity is our enemy
bool CBot::IsEnemy(CBaseEntity *pEntity) const
{
	// only Players (real and AI) can be enemies
	if (!pEntity->IsPlayer())
		return false;

	// corpses are no threat
	if (!pEntity->IsAlive())
		return false;

	CBasePlayer *pPlayer = static_cast<CBasePlayer *>(pEntity);

	// if they are on our team, they are our friends
	if (BotRelationship(pPlayer) == BOT_TEAMMATE)
		return false;

	// yep, we hate 'em
	return true;
}

// Return number of enemies left alive
int CBot::GetEnemiesRemaining() const
{
	int count = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
		if (!pPlayer)
			continue;

		if (FNullEnt(pPlayer->pev))
			continue;

		if (FStrEq(STRING(pPlayer->pev->netname), ""))
			continue;

		if (!IsEnemy(pPlayer))
			continue;

		if (!pPlayer->IsAlive())
			continue;

		count++;
	}

	return count;
}

// Return number of friends left alive
int CBot::GetFriendsRemaining() const
{
	int count = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
		if (!pPlayer)
			continue;

		if (FNullEnt(pPlayer->pev))
			continue;

		if (FStrEq(STRING(pPlayer->pev->netname), ""))
			continue;

		if (IsEnemy(pPlayer))
			continue;

		if (!pPlayer->IsAlive())
			continue;

		if (pPlayer == static_cast<CBaseEntity *>(const_cast<CBot *>(this)))
			continue;

		count++;
	}

	return count;
}

bool CBot::IsLocalPlayerWatchingMe() const
{
	// avoid crash during spawn
	if (!pev)
		return false;

	int myIndex = const_cast<CBot *>(this)->entindex();

	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return false;

	if (((pPlayer->pev->flags & FL_SPECTATOR) || pPlayer->m_iTeam == SPECTATOR) && pPlayer->pev->iuser2 == myIndex)
	{
		switch (pPlayer->pev->iuser1)
		{
		case OBS_CHASE_LOCKED:
		case OBS_CHASE_FREE:
		case OBS_IN_EYE:
			return true;
		}
	}

	return false;
}

NOXREF void CBot::Print(char *format, ...) const
{
	va_list varg;
	char buffer[1024];

	// prefix the message with the bot's name
	Q_sprintf(buffer, "%s: ", STRING(pev->netname));
	SERVER_PRINT(buffer);

	va_start(varg, format);
	vsprintf(buffer, format, varg);
	va_end(varg);

	SERVER_PRINT(buffer);
}

void CBot::PrintIfWatched(const char *format, ...) const
{
	if (!cv_bot_debug.value)
		return;

	if ((IsLocalPlayerWatchingMe() && (cv_bot_debug.value == 1 || cv_bot_debug.value == 3))
		|| (cv_bot_debug.value == 2 || cv_bot_debug.value == 4))
	{
		va_list varg;
		char buffer[1024];

		// prefix the message with the bot's name (this can be NULL if bot was just added)
		const char *name = pev ? STRING(pev->netname) : "(NULL pev)";
		Q_sprintf(buffer, "%s: ", name ? name : "(NULL netname)");

		SERVER_PRINT(buffer);

		va_start(varg, format);
		vsprintf(buffer, format, varg);
		va_end(varg);

		SERVER_PRINT(buffer);
	}
}

ActiveGrenade::ActiveGrenade(EquipmentIdType weaponID, CGrenade *grenadeEntity)
{
	m_id = weaponID;
	m_entity = grenadeEntity;
	m_detonationPosition = grenadeEntity->pev->origin;
	m_dieTimestamp = 0;
}

void ActiveGrenade::OnEntityGone()
{
	if (m_id == EQP_SMOKEGRENADE)
	{
		// smoke lingers after grenade is gone
		const float smokeLingerTime = 4.0f;
		m_dieTimestamp = gpGlobals->time + smokeLingerTime;
	}

	m_entity = nullptr;
}

bool ActiveGrenade::IsValid() const
{
	if (!m_entity)
	{
		if (gpGlobals->time > m_dieTimestamp)
			return false;
	}

	return true;
}

const Vector *ActiveGrenade::GetPosition() const
{
	return &m_entity->pev->origin;
}
