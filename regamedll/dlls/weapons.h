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

class CBasePlayer;

const float MAX_NORMAL_BATTERY    = 100.0f;
const float MAX_DIST_RELOAD_SOUND = 512.0f;

#define MAX_WEAPONS                 32

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

struct ItemInfo
{
	WeaponIdType m_iId;
	const char* m_pszClassName;
	AmmoIdType m_iAmmoType;
	int m_iMaxClip;
	int m_iSlot;
	int m_iPosition;
	int m_bitsFlags;
	int m_iWeight;
	int m_iCost;
};

struct AmmoInfo
{
	AmmoIdType m_iId;
	const char* m_pszName;
	int m_iMax;
	int m_iCountPerBox;
	int m_iCostPerBox;
	int m_iBulletBehavior;
};

struct MULTIDAMAGE
{
	CBaseEntity *pEntity;
	float amount;
	int type;
};

#include "ammo.h"
#include "weapontype.h"
#include "items.h"

class CGrenade: public CBaseMonster
{
public:
	static const float FROST_GR_DAMAGE;
	static const float FROST_GR_RADIUS;
	static const float FROST_GR_EFTIME;

public:
	virtual void Spawn();
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int ObjectCaps() { return 0; }
	virtual void Killed(entvars_t *pevAttacker, int iGib);
	virtual int BloodColor() { return DONT_BLEED; }
	virtual void BounceSound();

public:

	static CGrenade *ShootTimed(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time);
	static CGrenade *ShootTimed2(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time, int iTeam, unsigned short usEvent);
	static CGrenade *ShootSmokeGrenade(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time, unsigned short usEvent);

	// skill grenade
	static CGrenade* FrostGrenade(CBasePlayer* pPlayer);
	static CGrenade* HealingGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, float time, unsigned short usEvent);

public:
	void Explode(Vector vecSrc, Vector vecAim);
	void Explode(TraceResult *pTrace, int bitsDamageType);
	void Explode3(TraceResult *pTrace, int bitsDamageType);

	void EXPORT Smoke();
	void EXPORT Smoke2();
	void EXPORT Smoke3_A();
	void EXPORT Smoke3_B();
	void EXPORT Smoke3_C();
	void EXPORT SG_Smoke();
	void EXPORT BounceTouch(CBaseEntity *pOther);
	void EXPORT SlideTouch(CBaseEntity *pOther);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT DangerSoundThink();
	void EXPORT PreDetonate();
	void EXPORT Detonate();
	void EXPORT SG_Detonate();
	void EXPORT Detonate3();
	void EXPORT DetonateUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT TumbleThink();
	void EXPORT SG_TumbleThink();

	void EXPORT FrostTouch(CBaseEntity* pOther);

public:
	static TYPEDESCRIPTION m_SaveData[];

	bool m_bJustBlew;
	int m_iTeam;
	int m_iCurWave;
	int m_SGSmoke;
	int m_angle;
	unsigned short m_usEvent;
	bool m_bLightSmoke;
	bool m_bDetonated;
	Vector m_vSmokeDetonate;
	int m_iBounceCount;
	BOOL m_fRegisteredSound;
	bool m_bHealing;
};

// Items that the player has in their inventory that they can use
class CBasePlayerItem: public CBaseAnimating
{
public:
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual void SetObjectCollisionBox();
	virtual int AddToPlayer(CBasePlayer *pPlayer);							// return TRUE if the item you want the item added to the player inventory
	virtual int AddDuplicate(CBasePlayerItem *pItem) { return FALSE; }		// return TRUE if you want your duplicate removed from world
	virtual int GetItemInfo(ItemInfo* p);									// returns 0 if struct not filled out
	virtual BOOL CanDeploy() { return TRUE; }
	virtual BOOL CanDrop() { return TRUE; }									// returns is deploy was successful
	virtual BOOL Deploy() { return TRUE; }
	virtual BOOL IsWeapon() { return FALSE; }
	virtual BOOL CanHolster() { return TRUE; }								// can this weapon be put away right now?
	virtual void Holster(int skiplocal = 0);
	virtual void UpdateItemInfo() {}
	virtual void ItemPreFrame() {}											// called each frame by the player PreThink
	virtual void ItemPostFrame() {}											// called each frame by the player PostThink
	virtual void Drop();
	virtual void Kill();
	virtual void AttachToPlayer(CBasePlayer *pPlayer);
	virtual int UpdateClientData(CBasePlayer *pPlayer) { return 0; }
	virtual float GetMaxSpeed() { return 260.0f; }
	virtual const ItemInfo* iinfo() { return &m_rgItemInfo[(m_iId > 0 && m_iId < LAST_WEAPON) ? m_iId : 0]; }
	virtual const AmmoInfo* ainfo() { return &m_rgAmmoInfo[0]; }

public:
	void EXPORT DestroyItem();
	void EXPORT DefaultTouch(CBaseEntity *pOther);
	void EXPORT FallThink();
	void EXPORT Materialize();

	void FallInit();

public:
	static TYPEDESCRIPTION m_SaveData[];
	static const ItemInfo m_rgItemInfo[MAX_WEAPONS];
	static const AmmoInfo m_rgAmmoInfo[MAX_AMMO_SLOTS];

	CBasePlayer *m_pPlayer;
	CBasePlayerItem *m_pNext;
	WeaponIdType m_iId;			// WEAPON_???
	bool m_bHadBeenSold;			// used for commander passive skill.
};

// inventory items that
class CBasePlayerWeapon: public CBasePlayerItem
{
public:
	virtual void Spawn() {}
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	// generic weapon versions of CBasePlayerItem calls
	virtual int AddToPlayer(CBasePlayer *pPlayer);
	virtual int AddDuplicate(CBasePlayerItem *pItem);
	virtual BOOL CanDeploy();
	virtual BOOL IsWeapon() { return TRUE; }
	virtual void Holster(int skiplocal = 0);
	virtual void UpdateItemInfo() {};
	virtual void ItemPostFrame();
	virtual int UpdateClientData(CBasePlayer *pPlayer);
	virtual int ExtractAmmo(CBasePlayerWeapon *pWeapon);
	virtual int ExtractClipAmmo(CBasePlayerWeapon *pWeapon);
	virtual BOOL PlayEmptySound();
	virtual void ResetEmptySound();
	virtual void SendWeaponAnim(int iAnim, int skiplocal = 0);
	virtual bool AddPrimaryAmmo(int iCount);
	virtual BOOL IsUseable();
	virtual void PrimaryAttack() {};
	virtual void SecondaryAttack() {};
	virtual void Reload() {};
	virtual void WeaponIdle() {};
	virtual void RetireWeapon();
	virtual BOOL ShouldWeaponIdle() { return FALSE; }
	virtual BOOL UseDecrement() { return FALSE; }
	virtual const AmmoInfo* ainfo() { return &m_rgAmmoInfo[(m_iPrimaryAmmoType > 0 && m_iPrimaryAmmoType < AMMO_MAXTYPE) ? m_iPrimaryAmmoType : 0]; }

public:
	BOOL DefaultDeploy(char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, int skiplocal = 0);
	int DefaultReload(int iClipSize, int iAnim, float fDelay);
	void FireRemaining(int &shotsFired, float &shootTime, BOOL isGlock18);
	void KickBack(float up_base, float lateral_base, float up_modifier, float lateral_modifier, float up_max, float lateral_max, int direction_change);
	void EjectBrassLate();
	void ReloadSound();
	float GetNextAttackDelay(float delay);
	bool HasSecondaryAttack();
	BOOL IsPistol() { return m_rgItemInfo[m_iId].m_iSlot == PISTOL_SLOT; }
	void SetPlayerShieldAnim();
	void ResetPlayerShieldAnim();
	bool ShieldSecondaryFire(int iUpAnim, int iDownAnim);
	void HandleInfiniteAmmo();
	void InstantReload(bool bCanRefillBPAmmo = false);
	bool DefaultShotgunReload(int iAnim, int iStartAnim, float fDelay, float fStartDelay, const char *pszReloadSound1 = nullptr, const char *pszReloadSound2 = nullptr);

public:
	static TYPEDESCRIPTION m_SaveData[];

	int m_iPlayEmptySound;
	int m_fFireOnEmpty;
	float m_flNextPrimaryAttack;	// soonest time ItemPostFrame will call PrimaryAttack
	float m_flNextSecondaryAttack;	// soonest time ItemPostFrame will call SecondaryAttack
	float m_flTimeWeaponIdle;		// soonest time ItemPostFrame will call WeaponIdle
	AmmoIdType m_iPrimaryAmmoType;			// "primary" ammo index into players m_rgAmmo[]
	AmmoIdType m_iSecondaryAmmoType;		// "secondary" ammo index into players m_rgAmmo[]
	int m_iClip;					// number of shots left in the primary weapon clip, -1 it not used
	int m_iClientClip;				// the last version of m_iClip sent to hud dll
	int m_iClientWeaponState;		// the last version of the weapon state sent to hud dll (is current weapon, is on target)
	int m_fInReload;				// Are we in the middle of a reload;
	int m_fInSpecialReload;			// Are we in the middle of a reload for the shotguns
	int m_iDefaultAmmo;				// how much ammo you get when you pick up this weapon as placed by a level designer.
	int m_iShellId;
	float m_fMaxSpeed;
	bool m_bDelayFire;
	int m_iDirection;
	bool m_bSecondarySilencerOn;
	float m_flAccuracy;
	float m_flLastFire;
	int m_iShotsFired;
	Vector m_vVecAiming;
	string_t model_name;
	float m_flGlock18Shoot;			// time to shoot the remaining bullets of the glock18 burst fire
	int m_iGlock18ShotsFired;		// used to keep track of the shots fired during the Glock18 burst fire mode.
	float m_flFamasShoot;
	int m_iFamasShotsFired;
	float m_fBurstSpread;
	int m_iWeaponState;
	float m_flNextReload;
	float m_flDecreaseShotsFired;
	unsigned short m_usFireGlock18;
	unsigned short m_usFireFamas;
	bool m_bQuickThrow;
	bool m_bReleaseLock;

	// hle time creep vars
	float m_flPrevPrimaryAttack;
	float m_flLastFireTime;
};

class CWeaponBox: public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int ObjectCaps() { return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE; }
	virtual void SetObjectCollisionBox();
	virtual bool GiveAmmo(int iCount, AmmoIdType iId);
	virtual void Touch(CBaseEntity *pOther);
	virtual void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType = USE_OFF, float value = 0.0f);

public:
	bool IsEmpty();

	void EXPORT Kill();
	void SetModel(const char *pszModelName);

	bool HasWeapon(CBasePlayerItem *pCheckItem);
	bool PackWeapon(CBasePlayerItem *pWeapon);

public:
	static TYPEDESCRIPTION m_SaveData[];

	CBasePlayerItem *m_rgpPlayerItems[MAX_ITEM_TYPES];
	int m_rgAmmo[MAX_AMMO_SLOTS];
	bool m_bHadBeenSold;
};


const float USP_MAX_SPEED       = 250.0f;
const float USP_DAMAGE          = 34.0f;
const float USP_DAMAGE_SIL      = 30.0f;
const float USP_RANGE_MODIFER   = 0.79f;
const float USP_RELOAD_TIME     = 2.7f;
const float USP_ADJUST_SIL_TIME = 3.13f;

enum usp_e
{
	USP_IDLE,
	USP_SHOOT1,
	USP_SHOOT2,
	USP_SHOOT3,
	USP_SHOOT_EMPTY,
	USP_RELOAD,
	USP_DRAW,
	USP_ATTACH_SILENCER,
	USP_UNSIL_IDLE,
	USP_UNSIL_SHOOT1,
	USP_UNSIL_SHOOT2,
	USP_UNSIL_SHOOT3,
	USP_UNSIL_SHOOT_EMPTY,
	USP_UNSIL_RELOAD,
	USP_UNSIL_DRAW,
	USP_DETACH_SILENCER,
};

enum usp_shield_e
{
	USP_SHIELD_IDLE,
	USP_SHIELD_SHOOT1,
	USP_SHIELD_SHOOT2,
	USP_SHIELD_SHOOT_EMPTY,
	USP_SHIELD_RELOAD,
	USP_SHIELD_DRAW,
	USP_SHIELD_UP_IDLE,
	USP_SHIELD_UP,
	USP_SHIELD_DOWN,
};

class CUSP: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}
	virtual BOOL IsPistol() { return TRUE; }

public:
	void USPFire(float flSpread, float flCycleTime, BOOL fUseSemi);
	int m_iShell;

private:
	unsigned short m_usFireUSP;
};


const float MP5N_MAX_SPEED     = 250.0f;
const float MP5N_DAMAGE        = 26.0f;
const float MP5N_RANGE_MODIFER = 0.84f;
const float MP5N_RELOAD_TIME   = 2.63f;

enum mp5n_e
{
	MP5N_IDLE1,
	MP5N_RELOAD,
	MP5N_DRAW,
	MP5N_SHOOT1,
	MP5N_SHOOT2,
	MP5N_SHOOT3,
};

class CMP5N: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return MP5N_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void MP5NFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireMP5N;
};


const float SG552_MAX_SPEED      = 235.0f;
const float SG552_MAX_SPEED_ZOOM = 200.0f;
const float SG552_DAMAGE         = 33.0f;
const float SG552_RANGE_MODIFER  = 0.955f;
const float SG552_RELOAD_TIME    = 3.0f;

enum sg552_e
{
	SG552_IDLE1,
	SG552_RELOAD,
	SG552_DRAW,
	SG552_SHOOT1,
	SG552_SHOOT2,
	SG552_SHOOT3,
};

class CSG552: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void SG552Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireSG552;
};


const float AK47_MAX_SPEED     = 221.0f;
const float AK47_DAMAGE        = 36.0f;
const float AK47_RANGE_MODIFER = 0.98f;
const float AK47_RELOAD_TIME   = 2.45f;

enum ak47_e
{
	AK47_IDLE1,
	AK47_RELOAD,
	AK47_DRAW,
	AK47_SHOOT1,
	AK47_SHOOT2,
	AK47_SHOOT3,
};

class CAK47: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return AK47_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void AK47Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireAK47;
};


const float AUG_MAX_SPEED     = 240.0f;
const float AUG_DAMAGE        = 32.0f;
const float AUG_RANGE_MODIFER = 0.96f;
const float AUG_RELOAD_TIME   = 3.3f;

enum aug_e
{
	AUG_IDLE1,
	AUG_RELOAD,
	AUG_DRAW,
	AUG_SHOOT1,
	AUG_SHOOT2,
	AUG_SHOOT3,
};

class CAUG: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return AUG_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void AUGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireAug;
};


const float AWP_MAX_SPEED      = 210.0f;
const float AWP_MAX_SPEED_ZOOM = 150.0f;
const float AWP_DAMAGE         = 115.0f;
const float AWP_RANGE_MODIFER  = 0.99f;
const float AWP_RELOAD_TIME    = 2.5f;

enum awp_e
{
	AWP_IDLE,
	AWP_SHOOT,
	AWP_SHOOT2,
	AWP_SHOOT3,
	AWP_RELOAD,
	AWP_DRAW,
};

class CAWP: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void AWPFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;

private:
	unsigned short m_usFireAWP;
};


const float DEAGLE_MAX_SPEED     = 250.0f;
const float DEAGLE_DAMAGE        = 54.0f;
const float DEAGLE_RANGE_MODIFER = 0.81f;
const float DEAGLE_RELOAD_TIME   = 2.2f;

enum deagle_e
{
	DEAGLE_IDLE1,
	DEAGLE_SHOOT1,
	DEAGLE_SHOOT2,
	DEAGLE_SHOOT_EMPTY,
	DEAGLE_RELOAD,
	DEAGLE_DRAW,
};

class CDEAGLE: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}
	virtual BOOL IsPistol() { return TRUE; }

public:
	void DEAGLEFire(float flSpread, float flCycleTime, BOOL fUseSemi);

	int m_iShell;

private:
	unsigned short m_usFireDeagle;
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

class CFlashbang: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL CanDeploy();
	virtual BOOL CanDrop() { return FALSE; }
	virtual BOOL Deploy();
	virtual void Holster(int skiplocal);
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}
	virtual BOOL IsPistol() { return FALSE; }

public:
	bool ShieldSecondaryFire(int iUpAnim, int iDownAnim);
	void SetPlayerShieldAnim();
	void ResetPlayerShieldAnim();
};


const float G3SG1_MAX_SPEED      = 210.0f;
const float G3SG1_MAX_SPEED_ZOOM = 150.0f;
const float G3SG1_DAMAGE         = 80.0f;
const float G3SG1_RANGE_MODIFER  = 0.98f;
const float G3SG1_RELOAD_TIME    = 3.5f;

enum g3sg1_e
{
	G3SG1_IDLE,
	G3SG1_SHOOT,
	G3SG1_SHOOT2,
	G3SG1_RELOAD,
	G3SG1_DRAW,
};

class CG3SG1: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void G3SG1Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;

private:
	unsigned short m_usFireG3SG1;
};


const float GLOCK18_MAX_SPEED     = 250.0f;
const float GLOCK18_DAMAGE        = 25.0f;
const float GLOCK18_RANGE_MODIFER = 0.75f;
const float GLOCK18_RELOAD_TIME   = 2.2f;

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

enum glock18_shield_e
{
	GLOCK18_SHIELD_IDLE1,
	GLOCK18_SHIELD_SHOOT,
	GLOCK18_SHIELD_SHOOT2,
	GLOCK18_SHIELD_SHOOT_EMPTY,
	GLOCK18_SHIELD_RELOAD,
	GLOCK18_SHIELD_DRAW,
	GLOCK18_SHIELD_IDLE,
	GLOCK18_SHIELD_UP,
	GLOCK18_SHIELD_DOWN,
};

class CGLOCK18: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}
	virtual BOOL IsPistol() { return TRUE; }

public:
	void GLOCK18Fire(float flSpread, float flCycleTime, BOOL bFireBurst);

public:
	int m_iShell;
	bool m_bBurstFire;
};


const float HEGRENADE_MAX_SPEED        = 250.0f;
const float HEGRENADE_MAX_SPEED_SHIELD = 180.0f;

enum hegrenade_e
{
	HEGRENADE_IDLE,
	HEGRENADE_PULLPIN,
	HEGRENADE_THROW,
	HEGRENADE_DRAW,
};

class CHEGrenade: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL CanDeploy();
	virtual BOOL CanDrop() { return FALSE; }
	virtual BOOL Deploy();
	virtual void Holster(int skiplocal);
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	bool ShieldSecondaryFire(int iUpAnim, int iDownAnim);
	void SetPlayerShieldAnim();
	void ResetPlayerShieldAnim();

public:
	unsigned short m_usCreateExplosion;
};


const float KNIFE_BODYHIT_VOLUME   = 128.0f;
const float KNIFE_WALLHIT_VOLUME   = 512.0f;
const float KNIFE_MAX_SPEED        = 250.0f;
const float KNIFE_MAX_SPEED_SHIELD = 180.0f;

enum knife_e
{
	KNIFE_IDLE,
	KNIFE_ATTACK1HIT,
	KNIFE_ATTACK2HIT,
	KNIFE_DRAW,
	KNIFE_STABHIT,
	KNIFE_STABMISS,
	KNIFE_MIDATTACK1HIT,
	KNIFE_MIDATTACK2HIT,
};

enum knife_shield_e
{
	KNIFE_SHIELD_IDLE,
	KNIFE_SHIELD_SLASH,
	KNIFE_SHIELD_ATTACKHIT,
	KNIFE_SHIELD_DRAW,
	KNIFE_SHIELD_UPIDLE,
	KNIFE_SHIELD_UP,
	KNIFE_SHIELD_DOWN,
};

class CKnife: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL CanDrop() { return FALSE; }
	virtual BOOL Deploy();
	virtual void Holster(int skiplocal);
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}
	virtual void WeaponIdle();

public:
	void EXPORT SwingAgain();
	void EXPORT Smack();

	void WeaponAnimation(int iAnimation);
	BOOL Stab(BOOL fFirst);
	BOOL Swing(BOOL fFirst);

public:
	bool ShieldSecondaryFire(int iUpAnim, int iDownAnim);
	void SetPlayerShieldAnim();
	void ResetPlayerShieldAnim();

public:
	TraceResult m_trHit;
	unsigned short m_usKnife;
};


const float M249_MAX_SPEED     = 220.0f;
const float M249_DAMAGE        = 32.0f;
const float M249_RANGE_MODIFER = 0.97f;
const float M249_RELOAD_TIME   = 4.7f;

enum m249_e
{
	M249_IDLE1,
	M249_SHOOT1,
	M249_SHOOT2,
	M249_RELOAD,
	M249_DRAW,
};

class CM249: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return M249_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void M249Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireM249;
};


const float M3_MAX_SPEED   = 230.0f;
const float M3_DAMAGE      = 20.0f;
const Vector M3_CONE_VECTOR = Vector(0.0675, 0.0675, 0.0); // special shotgun spreads

enum m3_e
{
	M3_IDLE,
	M3_FIRE1,
	M3_FIRE2,
	M3_RELOAD,
	M3_PUMP,
	M3_START_RELOAD,
	M3_DRAW,
	M3_HOLSTER,
};

class CM3: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return M3_MAX_SPEED; }
	virtual BOOL PlayEmptySound();
	virtual void PrimaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	int m_iShell;
	float m_flPumpTime;

private:
	unsigned short m_usFireM3;
};


const float M4A1_MAX_SPEED         = 230.0f;
const float M4A1_DAMAGE            = 32.0f;
const float M4A1_DAMAGE_SIL        = 33.0f;
const float M4A1_RANGE_MODIFER     = 0.97f;
const float M4A1_RANGE_MODIFER_SIL = 0.95f;
const float M4A1_RELOAD_TIME       = 3.05f;

enum m4a1_e
{
	M4A1_IDLE,
	M4A1_SHOOT1,
	M4A1_SHOOT2,
	M4A1_SHOOT3,
	M4A1_RELOAD,
	M4A1_DRAW,
	M4A1_ATTACH_SILENCER,
	M4A1_UNSIL_IDLE,
	M4A1_UNSIL_SHOOT1,
	M4A1_UNSIL_SHOOT2,
	M4A1_UNSIL_SHOOT3,
	M4A1_UNSIL_RELOAD,
	M4A1_UNSIL_DRAW,
	M4A1_DETACH_SILENCER,
};

class CM4A1: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void M4A1Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireM4A1;
};


const float MAC10_MAX_SPEED     = 250.0f;
const float MAC10_DAMAGE        = 29.0f;
const float MAC10_RANGE_MODIFER = 0.82f;
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

class CMAC10: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return MAC10_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void MAC10Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireMAC10;
};


const float P228_MAX_SPEED     = 250.0f;
const float P228_DAMAGE        = 32.0f;
const float P228_RANGE_MODIFER = 0.8f;
const float P228_RELOAD_TIME   = 2.7f;

enum p228_e
{
	P228_IDLE,
	P228_SHOOT1,
	P228_SHOOT2,
	P228_SHOOT3,
	P228_SHOOT_EMPTY,
	P228_RELOAD,
	P228_DRAW,
};

enum p228_shield_e
{
	P228_SHIELD_IDLE,
	P228_SHIELD_SHOOT1,
	P228_SHIELD_SHOOT2,
	P228_SHIELD_SHOOT_EMPTY,
	P228_SHIELD_RELOAD,
	P228_SHIELD_DRAW,
	P228_SHIELD_IDLE_UP,
	P228_SHIELD_UP,
	P228_SHIELD_DOWN,
};

class CP228: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}
	virtual BOOL IsPistol() { return TRUE; }

public:
	void P228Fire(float flSpread, float flCycleTime, BOOL fUseSemi);
	void MakeBeam();
	void BeamUpdate();

public:
	int m_iShell;

private:
	unsigned short m_usFireP228;
};


const float P90_MAX_SPEED     = 245.0f;
const float P90_DAMAGE        = 21.0f;
const float P90_RANGE_MODIFER = 0.885f;
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

class CP90: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed();
	virtual void PrimaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void P90Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireP90;
};


const float SCOUT_MAX_SPEED      = 260.0f;
const float SCOUT_MAX_SPEED_ZOOM = 220.0f;
const float SCOUT_DAMAGE         = 75.0f;
const float SCOUT_RANGE_MODIFER  = 0.98f;
const float SCOUT_RELOAD_TIME    = 2.0f;

enum scout_e
{
	SCOUT_IDLE,
	SCOUT_SHOOT,
	SCOUT_SHOOT2,
	SCOUT_RELOAD,
	SCOUT_DRAW,
};

class CSCOUT: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void SCOUTFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	int m_iShell;

private:
	unsigned short m_usFireScout;
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

class CSmokeGrenade: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL CanDeploy();
	virtual BOOL CanDrop() { return FALSE; }
	virtual BOOL Deploy();
	virtual void Holster(int skiplocal);
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	bool ShieldSecondaryFire(int iUpAnim, int iDownAnim);
	void SetPlayerShieldAnim();
	void ResetPlayerShieldAnim();

public:
	unsigned short m_usCreateSmoke;
};


const float TMP_MAX_SPEED     = 250.0f;
const float TMP_DAMAGE        = 20.0f;
const float TMP_RANGE_MODIFER = 0.85f;
const float TMP_RELOAD_TIME   = 2.12f;

enum tmp_e
{
	TMP_IDLE1,
	TMP_RELOAD,
	TMP_DRAW,
	TMP_SHOOT1,
	TMP_SHOOT2,
	TMP_SHOOT3,
};

class CTMP: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return TMP_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void TMPFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

public:
	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireTMP;
};


const float XM1014_MAX_SPEED   = 240.0f;
const float XM1014_DAMAGE      = 20.0f;
const Vector XM1014_CONE_VECTOR = Vector(0.0725, 0.0725, 0.0); // special shotgun spreads

enum xm1014_e
{
	XM1014_IDLE,
	XM1014_FIRE1,
	XM1014_FIRE2,
	XM1014_RELOAD,
	XM1014_PUMP,
	XM1014_START_RELOAD,
	XM1014_DRAW,
};

class CXM1014: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return XM1014_MAX_SPEED; }
	virtual BOOL PlayEmptySound();
	virtual void PrimaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	int m_iShell;
	float m_flPumpTime;

private:
	unsigned short m_usFireXM1014;
};


const float ELITE_MAX_SPEED     = 250.0f;
const float ELITE_RELOAD_TIME   = 4.5f;
const float ELITE_DAMAGE        = 36.0f;
const float ELITE_RANGE_MODIFER = 0.75f;

enum elite_e
{
	ELITE_IDLE,
	ELITE_IDLE_LEFTEMPTY,
	ELITE_SHOOTLEFT1,
	ELITE_SHOOTLEFT2,
	ELITE_SHOOTLEFT3,
	ELITE_SHOOTLEFT4,
	ELITE_SHOOTLEFT5,
	ELITE_SHOOTLEFTLAST,
	ELITE_SHOOTRIGHT1,
	ELITE_SHOOTRIGHT2,
	ELITE_SHOOTRIGHT3,
	ELITE_SHOOTRIGHT4,
	ELITE_SHOOTRIGHT5,
	ELITE_SHOOTRIGHTLAST,
	ELITE_RELOAD,
	ELITE_DRAW,
};

class CELITE: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return ELITE_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}
	virtual BOOL IsPistol() { return TRUE; }

public:
	void ELITEFire(float flSpread, float flCycleTime, BOOL fUseSemi);

public:
	int m_iShell;
private:
	unsigned short m_usFireELITE_LEFT;
	unsigned short m_usFireELITE_RIGHT;
};


const float FIVESEVEN_MAX_SPEED     = 250.0f;
const float FIVESEVEN_DAMAGE        = 20.0f;
const float FIVESEVEN_RANGE_MODIFER = 0.885f;
const float FIVESEVEN_RELOAD_TIME   = 2.7f;

enum fiveseven_e
{
	FIVESEVEN_IDLE,
	FIVESEVEN_SHOOT1,
	FIVESEVEN_SHOOT2,
	FIVESEVEN_SHOOT_EMPTY,
	FIVESEVEN_RELOAD,
	FIVESEVEN_DRAW,
};

class CFiveSeven: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}
	virtual BOOL IsPistol() { return TRUE; }

public:
	void FiveSevenFire(float flSpread, float flCycleTime, BOOL fUseSemi);
	void MakeBeam();
	void BeamUpdate();

public:
	int m_iShell;

private:
	unsigned short m_usFireFiveSeven;
};


const float UMP45_MAX_SPEED     = 250.0f;
const float UMP45_DAMAGE        = 30.0f;
const float UMP45_RANGE_MODIFER = 0.82f;
const float UMP45_RELOAD_TIME   = 3.5f;

enum ump45_e
{
	UMP45_IDLE1,
	UMP45_RELOAD,
	UMP45_DRAW,
	UMP45_SHOOT1,
	UMP45_SHOOT2,
	UMP45_SHOOT3,
};

class CUMP45: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return UMP45_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void UMP45Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireUMP45;
};


const float SG550_MAX_SPEED      = 210.0f;
const float SG550_MAX_SPEED_ZOOM = 150.0f;
const float SG550_DAMAGE         = 70.0f;
const float SG550_RANGE_MODIFER  = 0.98f;
const float SG550_RELOAD_TIME    = 3.35f;

enum sg550_e
{
	SG550_IDLE,
	SG550_SHOOT,
	SG550_SHOOT2,
	SG550_RELOAD,
	SG550_DRAW,
};

class CSG550: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void SG550Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	int m_iShell;

private:
	unsigned short m_usFireSG550;
};


const float GALIL_MAX_SPEED     = 240.0f;
const float GALIL_DAMAGE        = 30.0f;
const float GALIL_RANGE_MODIFER = 0.98f;
const float GALIL_RELOAD_TIME   = 2.45f;

enum galil_e
{
	GALIL_IDLE1,
	GALIL_RELOAD,
	GALIL_DRAW,
	GALIL_SHOOT1,
	GALIL_SHOOT2,
	GALIL_SHOOT3,
};

class CGalil: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return GALIL_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void GalilFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

public:
	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireGalil;
};


const float FAMAS_MAX_SPEED     = 240.0f;
const float FAMAS_RELOAD_TIME   = 3.3f;
const float FAMAS_DAMAGE        = 30.0f;
const float FAMAS_DAMAGE_BURST  = 34.0f;
const float FAMAS_RANGE_MODIFER = 0.96f;

enum famas_e
{
	FAMAS_IDLE1,
	FAMAS_RELOAD,
	FAMAS_DRAW,
	FAMAS_SHOOT1,
	FAMAS_SHOOT2,
	FAMAS_SHOOT3,
};

class CFamas: public CBasePlayerWeapon
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL Deploy();
	virtual float GetMaxSpeed() { return FAMAS_MAX_SPEED; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
	#ifdef CLIENT_WEAPONS
		return TRUE;
	#else
		return FALSE;
	#endif
	}

public:
	void FamasFire(float flSpread, float flCycleTime, BOOL fUseAutoAim, BOOL bFireBurst);

public:
	int m_iShell;
	int iShellOn;
};

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
void EjectBrass(const Vector &vecOrigin, const Vector &vecLeft, const Vector &vecVelocity, float rotation, int model, int soundtype, int entityIndex);
void EjectBrass2(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype, entvars_t *pev);
void UTIL_PrecacheOtherWeapon(const char *szClassname);
BOOL CanAttack(float attack_time, float curtime, BOOL isPredicted);
