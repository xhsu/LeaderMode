#include "precompiled.h"


void BasicKnife::Precache()
{
#ifndef CLIENT_DLL
	PRECACHE_MODEL("models/weapons/v_knife.mdl");
	PRECACHE_MODEL("models/p_knife.mdl");

	PRECACHE_SOUND("weapons/knife_deploy1.wav");
	PRECACHE_SOUND("weapons/knife_hit1.wav");
	PRECACHE_SOUND("weapons/knife_hit2.wav");
	PRECACHE_SOUND("weapons/knife_hit3.wav");
	PRECACHE_SOUND("weapons/knife_hit4.wav");
	PRECACHE_SOUND("weapons/knife_slash1.wav");
	PRECACHE_SOUND("weapons/knife_slash2.wav");
	PRECACHE_SOUND("weapons/knife_stab.wav");
	PRECACHE_SOUND("weapons/knife_hitwall1.wav");
#endif
}

bool BasicKnife::Deploy(IWeapon* pWeapon)
{
	m_pWeapon = pWeapon;
	m_pPlayer = static_cast<CBasePlayer*>(pWeapon->GetOwner());
	*m_pWeapon->Flags() |= WPNSTATE_MELEE;	// mark for further process.

	m_pPlayer->pev->fov = DEFAULT_FOV;
	m_pPlayer->m_iLastZoom = DEFAULT_FOV;
	m_pPlayer->m_bResumeZoom = false;

	Q_strlcpy(m_pPlayer->m_szAnimExtention, "knife");

#ifndef CLIENT_DLL
	m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/v_knife.mdl");
	m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_knife.mdl");
#else
	m_pPlayer = &gPseudoPlayer;	// reclaim him.
	g_pViewEnt->model = gEngfuncs.CL_LoadModel("models/weapons/v_knife.mdl", &m_pPlayer->pev->viewmodel);
#endif

	return true;
}

#ifndef CLIENT_DLL
void FindHullIntersection(const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity)
{
	int i, j, k;
	float distance;
	float *minmaxs[2] = { mins, maxs };
	TraceResult tmpTrace;
	Vector vecHullEnd = tr.vecEndPos;
	Vector vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc) * 2);
	UTIL_TraceLine(vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace);

	if (tmpTrace.flFraction < 1.0f)
	{
		tr = tmpTrace;
		return;
	}

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			for (k = 0; k < 2; k++)
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace);

				if (tmpTrace.flFraction < 1.0f)
				{
					real_t thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();

					if (thisDistance < distance)
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}
#endif

void BasicKnife::Swing()
{
#ifndef CLIENT_DLL
	BOOL fDidHit = FALSE;
	TraceResult tr;
	Vector vecSrc, vecEnd;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);

	vecSrc = m_pPlayer->GetGunPosition();
	vecEnd = vecSrc + gpGlobals->v_forward * 48.0f;

	// some cheap screen feeling FX.
	m_pPlayer->pev->punchangle.x += -6;	// upward
	m_pPlayer->pev->punchangle.y += 5;	// left
	m_pPlayer->pev->punchangle.z += 7;	// clockwise

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, m_pPlayer->edict(), &tr);

	if (tr.flFraction >= 1.0f)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, m_pPlayer->edict(), &tr);

		if (tr.flFraction < 1.0f)
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);

			if (!pHit || pHit->IsBSPModel())
			{
				FindHullIntersection(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict());
			}

			// This is the point on the actual surface (the hull could have hit space)
			vecEnd = tr.vecEndPos;
		}
	}

	if (tr.flFraction >= 1.0f)
	{
		m_pWeapon->Animate(KNIFE_QUICK_SLASH);	// this anim msg has to be send!
		m_pPlayer->m_flNextAttack = KNIFE_QUICK_SLASH_TIME;

		// play wiff or swish sound
		if (RANDOM_LONG(0, 1))
			EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "weapons/knife_slash1.wav", VOL_NORM, ATTN_NORM, 0, 94);
		else
			EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "weapons/knife_slash2.wav", VOL_NORM, ATTN_NORM, 0, 94);

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	}
	else
	{
		// hit
		fDidHit = TRUE;

		m_pWeapon->Animate(KNIFE_QUICK_SLASH);	// this anim msg has to be send!
		m_pPlayer->m_flNextAttack = KNIFE_QUICK_SLASH_TIME;

		// play thwack, smack, or dong sound
		float flVol = 1.0f;
		int fHitWorld = TRUE;

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		// back..slash?? we don't have stab here.
		float flModifier = 1.0f;
		if (DotProduct(gpGlobals->v_forward, pEntity->pev->angles.MakeVector()) > 0.0f)
			flModifier = 3.0f;	// original CS.

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
		ClearMultiDamage();
		pEntity->TraceAttack(m_pPlayer->pev, KNIFE_QUICK_SLASH_DMG * flModifier, gpGlobals->v_forward, &tr, (DMG_NEVERGIB | DMG_BULLET));
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE && pEntity->Classify() != CLASS_VEHICLE)
		{
			// play thwack or smack sound
			switch (RANDOM_LONG(0, 3))
			{
			case 0: EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/knife_hit1.wav", VOL_NORM, ATTN_NORM); break;
			case 1: EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/knife_hit2.wav", VOL_NORM, ATTN_NORM); break;
			case 2: EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/knife_hit3.wav", VOL_NORM, ATTN_NORM); break;
			case 3: EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/knife_hit4.wav", VOL_NORM, ATTN_NORM); break;
			}

			m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;

			if (!pEntity->IsAlive())
				return;
			else
				flVol = 0.1f;

			fHitWorld = FALSE;
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		if (fHitWorld)
		{
			float fvolbar = TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);
			//fvolbar = 1.0;

			if (RANDOM_LONG(0, 1) > 1)
			{
				fHitWorld = FALSE;
			}
		}

		if (!fHitWorld)
		{
			m_pPlayer->m_iWeaponVolume = int(flVol * KNIFE_WALLHIT_VOLUME);
		}
		else
		{
			// also play knife strike
			EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, "weapons/knife_hitwall1.wav", VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(0, 3) + 98);
		}
	}
#else
	m_pWeapon->Animate(KNIFE_QUICK_SLASH);
	m_pPlayer->m_flNextAttack = KNIFE_QUICK_SLASH_TIME;
#endif
}
