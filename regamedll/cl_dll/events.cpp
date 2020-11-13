/*

Created Date: 08 Mar 2020

*/

#include "precompiled.h"

int g_iRShell = 0;
int g_iPShell = 0;
int g_iShotgunShell = 0;
int g_iBlackSmoke = 0;

cvar_t* cl_gunbubbles = nullptr;
cvar_t* cl_tracereffect = nullptr;

/*
=================
EV_MuzzleFlash

Flag weapon/view model for muzzle flash
=================
*/
inline void EV_MuzzleFlash(void)
{
	if (cl_lw->value)
		return;

	// Add muzzle flash to current weapon model
	cl_entity_t* ent = gEngfuncs.GetViewModel();
	if (!ent)
		return;

	// Or in the muzzle flash
	ent->curstate.effects |= EF_MUZZLEFLASH;
}

/*
=================
EV_IsPlayer

Is the entity's index in the player range?
=================
*/
inline bool EV_IsPlayer(int idx)
{
	if (idx >= 1 && idx <= gEngfuncs.GetMaxClients())
		return true;

	return false;
}

/*
=================
EV_IsLocal

Is the entity == the local player
=================
*/
bool EV_IsLocal(int idx)
{
	// check if we are in some way in first person spec mode
	if (IS_FIRSTPERSON_SPEC)
		return (g_iUser2 == idx);
	else
		return gEngfuncs.pEventAPI->EV_IsLocal(idx - 1) ? true : false;
}

/*
=================
EV_EjectBrass

Bullet shell casings
=================
*/
void EV_EjectBrass(float* origin, float* velocity, float rotation, int model, int soundtype, float flAngularVel, float life)
{
	Vector angles(0.0f, 0.0f, rotation);
	TEMPENTITY* p = gEngfuncs.pEfxAPI->R_TempModel(origin, velocity, angles, life, model, soundtype);

	if (p)
	{
		// baseline.angles		- angle velocity
		p->entity.baseline.angles *= flAngularVel;
	}
}

/*
=================
EV_GetDefaultShellInfo

Determine where to eject shells from
=================
*/
void EV_GetDefaultShellInfo(event_args_t* args, float* origin, float* velocity, float* ShellVelocity, float* ShellOrigin, float* forward, float* right, float* up, float forwardScale, float upScale, float rightScale)
{
	int i;
	vec3_t view_ofs;
	float fR, fU;

	int idx;

	idx = args->entindex;

	VectorClear(view_ofs);
	view_ofs[2] = PM_VEC_VIEW;

	if (EV_IsPlayer(idx))
	{
		if (EV_IsLocal(idx))
		{
			gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(view_ofs);
		}
		else if (args->ducking == 1)
		{
			view_ofs[2] = PM_VEC_DUCK_VIEW;
		}
	}

	fR = gEngfuncs.pfnRandomFloat(50, 70);
	fU = gEngfuncs.pfnRandomFloat(100, 150);

	for (i = 0; i < 3; i++)
	{
		ShellVelocity[i] = velocity[i] + right[i] * fR + up[i] * fU + forward[i] * 25;
		ShellOrigin[i] = origin[i] + view_ofs[i] + up[i] * upScale + forward[i] * forwardScale + right[i] * rightScale;
	}
}

/*
=================
EV_GetGunPosition

Figure out the height of the gun
=================
*/
Vector EV_GetGunPosition(event_args_t* args, Vector origin)
{
	int idx;
	Vector view_ofs;

	idx = args->entindex;

	view_ofs.z = PM_VEC_VIEW;

	if (EV_IsPlayer(idx))
	{
		// in spec mode use entity viewheigh, not own
		if (EV_IsLocal(idx) && !IS_FIRSTPERSON_SPEC)
		{
			// Grab predicted result for local player
			gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(view_ofs);
		}
		else if (args->ducking == 1)
		{
			view_ofs.z = PM_VEC_DUCK_VIEW;
		}
	}

	return origin + view_ofs;
}

int EV_HLDM_CheckTracer(int idx, Vector& vecSrc, Vector& end, Vector& forward, Vector& right, int iBulletType, int iTracerFreq, int* tracerCount)
{
	int tracer = 0;
	int i;
	bool player = idx >= 1 && idx <= gEngfuncs.GetMaxClients() ? true : false;

	if (iTracerFreq != 0 && ((*tracerCount)++ % iTracerFreq) == 0)
	{
		Vector vecTracerSrc;

		if (player)
		{
			Vector offset(0, 0, -4);

			// adjust tracer position for player
			for (i = 0; i < 3; i++)
			{
				vecTracerSrc[i] = vecSrc[i] + offset[i] + right[i] * 2 + forward[i] * 16;
			}
		}
		else
		{
			VectorCopy(vecSrc, vecTracerSrc);
		}

		if (iTracerFreq != 1)		// guns that always trace also always decal
			tracer = 1;

		gEngfuncs.pEfxAPI->R_TracerEffect(vecTracerSrc, end);
	}

	return tracer;
}

char EV_HLDM_PlayTextureSound(int idx, pmtrace_t* ptr, Vector& vecSrc, Vector& vecEnd, int iBulletType)
{
	// hit the world, try to play sound based on texture material type
	char chTextureType = CHAR_TEX_CONCRETE;
	float fvol;
	const char* rgsz[4];
	int cnt;
	float fattn = ATTN_NORM;
	int entity;
	char* pTextureName;
	char texname[64];
	char szbuffer[64];

	entity = gEngfuncs.pEventAPI->EV_IndexFromTrace(ptr);

	// FIXME check if playtexture sounds movevar is set
	//

	chTextureType = 0;

	// Player
	if (entity >= 1 && entity <= gEngfuncs.GetMaxClients())
	{
		// hit body
		chTextureType = CHAR_TEX_FLESH;
	}
	else if (entity == 0)
	{
		// get texture from entity or world (world is ent(0))
		pTextureName = (char*)gEngfuncs.pEventAPI->EV_TraceTexture(ptr->ent, vecSrc, vecEnd);

		if (pTextureName)
		{
			Q_strncpy(texname, pTextureName, sizeof(texname));
			pTextureName = texname;

			// strip leading '-0' or '+0~' or '{' or '!'
			if (*pTextureName == '-' || *pTextureName == '+')
			{
				pTextureName += 2;
			}

			if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
			{
				pTextureName++;
			}

			// '}}'
			Q_strncpy(szbuffer, pTextureName, sizeof(szbuffer));
			szbuffer[MAX_TEXTURENAME_LENGHT - 1] = 0;

			// get texture type
			chTextureType = PM_FindTextureType(szbuffer);
		}
	}

	switch (chTextureType)
	{
	default:
	case CHAR_TEX_CONCRETE:
	{
		fvol = 0.9;
		rgsz[0] = "player/pl_step1.wav";
		rgsz[1] = "player/pl_step2.wav";
		cnt = 2;
		break;
	}
	case CHAR_TEX_METAL:
	{
		fvol = 0.9;
		rgsz[0] = "player/pl_metal1.wav";
		rgsz[1] = "player/pl_metal2.wav";
		cnt = 2;
		break;
	}
	case CHAR_TEX_DIRT:
	{
		fvol = 0.9;
		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		cnt = 3;
		break;
	}
	case CHAR_TEX_VENT:
	{
		fvol = 0.5;
		rgsz[0] = "player/pl_duct1.wav";
		rgsz[1] = "player/pl_duct1.wav";
		cnt = 2;
		break;
	}
	case CHAR_TEX_GRATE:
	{
		fvol = 0.9;
		rgsz[0] = "player/pl_grate1.wav";
		rgsz[1] = "player/pl_grate4.wav";
		cnt = 2;
		break;
	}
	case CHAR_TEX_TILE:
	{
		fvol = 0.8;
		rgsz[0] = "player/pl_tile1.wav";
		rgsz[1] = "player/pl_tile3.wav";
		rgsz[2] = "player/pl_tile2.wav";
		rgsz[3] = "player/pl_tile4.wav";
		cnt = 4;
		break;
	}
	case CHAR_TEX_SLOSH:
	{
		fvol = 0.9;
		rgsz[0] = "player/pl_slosh1.wav";
		rgsz[1] = "player/pl_slosh3.wav";
		rgsz[2] = "player/pl_slosh2.wav";
		rgsz[3] = "player/pl_slosh4.wav";
		cnt = 4;
		break;
	}
	case CHAR_TEX_SNOW:
	{
		fvol = 0.7;
		rgsz[0] = "debris/pl_snow1.wav";
		rgsz[1] = "debris/pl_snow2.wav";
		rgsz[2] = "debris/pl_snow3.wav";
		rgsz[3] = "debris/pl_snow4.wav";
		cnt = 4;
		break;
	}
	case CHAR_TEX_WOOD:
	{
		fvol = 0.9;
		rgsz[0] = "debris/wood1.wav";
		rgsz[1] = "debris/wood2.wav";
		rgsz[2] = "debris/wood3.wav";
		cnt = 3;
		break;
	}
	case CHAR_TEX_GLASS:
	case CHAR_TEX_COMPUTER:
	{
		fvol = 0.8;
		rgsz[0] = "debris/glass1.wav";
		rgsz[1] = "debris/glass2.wav";
		rgsz[2] = "debris/glass3.wav";
		cnt = 3;
		break;
	}
	case CHAR_TEX_FLESH:
	{
		fvol = 1.0;
		rgsz[0] = "weapons/bullet_hit1.wav";
		rgsz[1] = "weapons/bullet_hit2.wav";
		fattn = 1.0;
		cnt = 2;
		break;
	}
	}

	// play material hit sound
	gEngfuncs.pEventAPI->EV_PlaySound(0, ptr->endpos, CHAN_STATIC, rgsz[RANDOM_LONG(0, cnt - 1)], fvol, fattn, 0, 96 + RANDOM_LONG(0, 0xf));

	return chTextureType;
}

char* EV_HLDM_DamageDecal(physent_t* pe)
{
	static char decalname[32];
	int idx;

	if (pe->classnumber == 1)
	{
		idx = RANDOM_LONG(0, 2);
		Q_snprintf(decalname, charsmax(decalname), "{break%i", idx + 1);
	}
	else if (pe->rendermode != kRenderNormal)
	{
		Q_snprintf(decalname, charsmax(decalname), "{bproof1");
	}
	else
	{
		idx = RANDOM_LONG(0, 4);
		Q_snprintf(decalname, charsmax(decalname), "{shot%i", idx + 1);
	}
	return decalname;
}

void EV_HLDM_GunshotDecalTrace(pmtrace_t* pTrace, char* decalName, char chTextureType)
{
	int iRand;
	physent_t* pe;

	gEngfuncs.pEfxAPI->R_BulletImpactParticles(pTrace->endpos);


	iRand = RANDOM_LONG(0, 0x7FFF);
	if (iRand < (0x7fff / 2))// not every bullet makes a sound.
	{
		if (chTextureType == CHAR_TEX_VENT || chTextureType == CHAR_TEX_METAL)
		{
			switch (iRand % 2)
			{
			case 0: gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric_metal-1.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM); break;
			case 1: gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric_metal-2.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM); break;
			}
		}
		else
		{
			switch (iRand % 7)
			{
			case 0:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
			case 1:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
			case 2:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
			case 3:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric4.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
			case 4:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric5.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
			case 5: gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric_conc-1.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM); break;
			case 6: gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric_conc-2.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM); break;
			}
		}

	}

	pe = gEngfuncs.pEventAPI->EV_GetPhysent(pTrace->ent);

	// Only decal brush models such as the world etc.
	if (decalName && decalName[0] && pe && (pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP))
	{
		if (CVAR_GET_FLOAT("r_decals"))
		{
			gEngfuncs.pEfxAPI->R_DecalShoot(
				gEngfuncs.pEfxAPI->Draw_DecalIndex(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(decalName)),
				gEngfuncs.pEventAPI->EV_IndexFromTrace(pTrace), 0, pTrace->endpos, 0);

		}
	}
}

void EV_SmokeRise_Wind(tempent_s* pEntity, float frametime, float currenttime)
{
	static Vector velocity;
	static int iSmokeDirectionX = 1, iSmokeDirectionY = 1;	// Default = 1, not constant...
	if (pEntity->entity.curstate.frame > 7.0)
	{
		pEntity->entity.baseline.origin *= 0.97;
		pEntity->entity.baseline.origin.z += 0.7;

		if (pEntity->entity.baseline.origin.z > 70.0)
		{
			pEntity->entity.curstate.origin.z = 70.0;
		}
	}
	if (pEntity->entity.curstate.frame > 6.0)
	{
		velocity.x += 0.075;
		if (velocity.x > 5.0)
			velocity.x = 5.0;

		if (!iSmokeDirectionX)
			pEntity->entity.baseline.origin.x -= velocity.x;
		else
			pEntity->entity.baseline.origin.x += velocity.x;

		velocity.y += 0.075;
		if (velocity.y > 5.0)
			velocity.y = 5.0;

		if (!iSmokeDirectionY)
			pEntity->entity.baseline.origin.y -= velocity.y;
		else
			pEntity->entity.baseline.origin.y += velocity.y;

		if (!gEngfuncs.pfnRandomLong(0, 10))
		{
			if (velocity.y > 3.0)
			{
				velocity.y = 0.0;
				iSmokeDirectionY = 1 - iSmokeDirectionY;
			}
		}

		if (!gEngfuncs.pfnRandomLong(0, 10))
		{
			if (velocity.x > 3.0)
			{
				velocity.x = 0.0;
				iSmokeDirectionX = 1 - iSmokeDirectionX;
			}
		}
	}
}

void EV_SmokeRise_Wind_Expand(tempent_s* pEntity, float frametime, float currenttime)
{
	if (pEntity->entity.curstate.renderamt >= 75)
		pEntity->entity.curstate.renderamt -= gEngfuncs.pfnRandomLong(0, 9) == 0;
	if (pEntity->entity.curstate.scale < pEntity->entity.baseline.fuser1)
		pEntity->entity.curstate.scale *= 1.025;
}

void EV_SmokeRise(tempent_s* pEntity, float frametime, float currenttime)
{
	if (pEntity->entity.curstate.frame > 7.0)
	{
		pEntity->entity.baseline.origin *= 0.97;
		pEntity->entity.baseline.origin.z += 0.7;

		if (pEntity->entity.baseline.origin.z > 70.0)
		{
			pEntity->entity.curstate.origin.z = 70.0;
		}
	}
}

void EV_HugWalls(tempent_s* pEntity, pmtrace_t* tr)
{
	float flLength = pEntity->entity.baseline.origin.Length();

	if (flLength > 2000.0)
		flLength = 2000.0;

	Vector vecResult = pEntity->entity.baseline.origin.Normalize();

	pEntity->entity.baseline.origin.x = ((tr->plane.normal.x * vecResult.z - tr->plane.normal.z * vecResult.x) * tr->plane.normal.z - (tr->plane.normal.y * vecResult.z - tr->plane.normal.x * vecResult.y) * tr->plane.normal.y) * flLength * 1.5;
	pEntity->entity.baseline.origin.y = ((tr->plane.normal.y * vecResult.z - tr->plane.normal.x * vecResult.y) * tr->plane.normal.x - (tr->plane.normal.z * vecResult.y - tr->plane.normal.y * vecResult.z) * tr->plane.normal.z) * flLength * 1.5;
	pEntity->entity.baseline.origin.z = ((tr->plane.normal.z * vecResult.y - tr->plane.normal.y * vecResult.z) * tr->plane.normal.y - (tr->plane.normal.x * vecResult.z - tr->plane.normal.z * vecResult.x) * tr->plane.normal.x) * flLength * 1.5;
}

void EV_HLDM_CreateSmoke(float* origin, float* dir, int speed, float scale, int r, int g, int b, EV_SmokeTypes iSmokeType, float* base_velocity, bool bWind, int framerate)
{
	char model[256];
	int model_index;
	TEMPENTITY* te;
	switch (iSmokeType)
	{
	case EV_BLACK_SMOKE:
		Q_strlcpy(model, "sprites/black_smoke1.spr");

		switch (gEngfuncs.pfnRandomLong(0, 3))
		{
		case 0:
			model[19] = '1';
			break;
		case 1:
			model[19] = '2';
			break;
		case 2:
			model[19] = '3';
			break;
		case 3:
			model[19] = '4';
			break;
		}

		model_index = gEngfuncs.pEventAPI->EV_FindModelIndex(model);

		if (gEngfuncs.pfnGetCvarFloat("fastsprites") > 1.0)
		{
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/fast_wallpuff1.spr");
		}

		if (!model_index)
			model_index = 293;
		break;

	case EV_WALL_PUFF:
		switch (gEngfuncs.pfnRandomLong(0, 3))
		{
		case 0:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/wall_puff1.spr");
			break;
		case 1:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/wall_puff2.spr");
			break;
		case 2:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/wall_puff3.spr");
			break;
		case 3:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/wall_puff4.spr");
			break;
		}
		break;

	case EV_PISTOL_SMOKE:
		if (gEngfuncs.pfnRandomLong(0, 1))
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/pistol_smoke1.spr");
		else
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/pistol_smoke2.spr");
		break;

	case EV_RIFLE_SMOKE:
		switch (gEngfuncs.pfnRandomLong(0, 2))
		{
		case 0:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/rifle_smoke1.spr");
			break;
		case 1:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/rifle_smoke2.spr");
			break;
		case 2:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/rifle_smoke3.spr");
			break;
		}
		break;
	}

	te = gEngfuncs.pEfxAPI->R_DefaultSprite(origin, model_index, framerate);

	if (te)
	{
		te->flags |= FTENT_CLIENTCUSTOM | FTENT_COLLIDEWORLD;
		te->entity.curstate.scale = scale + gEngfuncs.pfnRandomFloat(0.0, scale / 2.0);

		if (iSmokeType > 1)
		{
			te->callback = bWind ? EV_SmokeRise_Wind : EV_SmokeRise;
			te->entity.curstate.rendermode = kRenderTransAdd;
			te->entity.curstate.renderamt = 150;
			te->entity.curstate.renderfx = kRenderFxFadeSlow;
		}
		else
		{
			if (gEngfuncs.pfnGetCvarFloat("fastsprites") == 1.0)
			{
				te->entity.curstate.rendermode = kRenderTransAdd;
				te->entity.curstate.renderamt = 200;
			}
			else
			{
				if (gEngfuncs.pfnGetCvarFloat("fastsprites") > 1.0)
				{
					te->entity.curstate.rendermode = kRenderNormal;
				}
				else
				{
					te->entity.curstate.rendermode = kRenderTransAlpha;
					te->entity.curstate.renderamt = 200;
				}
			}

			te->callback = EV_SmokeRise_Wind_Expand;
			te->entity.baseline.fuser1 = (iSmokeType == EV_BLACK_SMOKE) ? 1.0 : 8.5;

			if (gEngfuncs.pfnGetCvarFloat("fastsprites") > 1.0)
				te->entity.baseline.fuser1 -= 3.0;
		}

		te->hitcallback = EV_HugWalls;
		te->entity.curstate.rendercolor.r = r;
		te->entity.curstate.rendercolor.g = g;
		te->entity.curstate.rendercolor.b = b;

		if (speed)
		{
			speed += gEngfuncs.pfnRandomLong(0, 5);
		}

		if (origin[0] != dir[0] && origin[1] != dir[1] && origin[2] != dir[2])
		{
			te->entity.origin.x = origin[0];
			te->entity.origin.y = origin[1];
			te->entity.origin.z = origin[2];

			te->entity.baseline.origin.x = dir[0] * speed;
			te->entity.baseline.origin.y = dir[1] * speed;
			te->entity.baseline.origin.z = dir[2] * speed;

			if (base_velocity)
			{
				te->entity.baseline.origin.x = dir[0] * speed + 0.9 * base_velocity[0];
				te->entity.baseline.origin.y = dir[1] * speed + 0.9 * base_velocity[1];
				te->entity.baseline.origin.z = dir[2] * speed + 0.5 * base_velocity[2];
			}
		}
	}
}

void EV_HLDM_DecalGunshot(pmtrace_t* pTrace, int iBulletType, float scale, int r, int g, int b, bool bStreakSplash, char cTextureType)
{
	int iColorIndex;
	char* decalname;

	physent_t* pe = gEngfuncs.pEventAPI->EV_GetPhysent(pTrace->ent);

	if (pe && pe->solid == SOLID_BSP)
	{
		if (bStreakSplash)
		{
			int iStreakCount = gEngfuncs.pfnRandomLong(15, 30);

			switch (iBulletType)
			{
			default:
				iColorIndex = 30;
				break;
			}

			gEngfuncs.pEfxAPI->R_StreakSplash(pTrace->endpos, gEngfuncs.pfnRandomFloat(4.0, 10.0) * pTrace->plane.normal, iColorIndex, iStreakCount, gEngfuncs.pfnRandomFloat(4.0, 10.0) * pTrace->plane.normal[2], -75, 75);
		}

		switch (iBulletType)
		{
		default:
			decalname = EV_HLDM_DamageDecal(pe);
			break;
		}

		EV_HLDM_GunshotDecalTrace(pTrace, decalname, cTextureType);
		EV_HLDM_CreateSmoke(pTrace->endpos + pTrace->plane.normal * 5.0, pTrace->plane.normal, 25, scale, r, g, b, EV_WALL_PUFF, NULL, true, 35);
		//EV_CS16Client_CreateSmoke( SMOKE_WALLPUFF, pTrace->endpos, pTrace->plane.normal, 25, 0.5, r, g, b, true );
	}
}

void EV_HLDM_FireBullets(int idx, Vector& forward, Vector& right, Vector& up, int cShots, Vector& vecSrc, Vector& vecDirShooting, const Vector& vecSpread, float flDistance, AmmoIdType iBulletType, int iTracerFreq, int* tracerCount, int iPenetration, int iAttachment, bool lefthand, float srcofs, int shared_rand)
{
	int i;
	pmtrace_t tr;
	int tracer;
	int iPenetrationPower;
	float flPenetrationDistance;
	float flCurrentDistance;
	Vector vecStart;
	char chTextureType = CHAR_TEX_GRATE;
	float flScale = 0.4;
	float flTempDistance;
	int iSourcePenetration = iPenetration;
	int r = 40, g = 40, b = 40;
	BOOL bCreatedEffects = FALSE;
	int iSeedOfs = 0;	// keep track how many times we used the shared_rand. 

	DescribeBulletTypeParameters(iBulletType, iPenetrationPower, flPenetrationDistance);

	if (lefthand == 0)
	{
		vecSrc[0] -= right[0] * srcofs;
		vecSrc[1] -= right[1] * srcofs;
		vecSrc[2] -= right[2] * srcofs;
	}
	else
	{
		vecSrc[0] += right[0] * srcofs;
		vecSrc[1] += right[1] * srcofs;
		vecSrc[2] += right[2] * srcofs;
	}

	int iPower = iPenetrationPower * 2;
	bool bStreakSplash = false;

	int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");

	if (cShots >= 1)
	{
		VectorCopy(vecSrc, vecStart);
		int iShots = cShots;
		float x, y, z;
		Vector vecDir, vecEnd;

		while (iShots > 0)
		{
			if (iBulletType == AMMO_Buckshot)
			{
				do
				{
					x = UTIL_SharedRandomFloat(shared_rand + iSeedOfs, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + iSeedOfs + 1, -0.5, 0.5);
					y = UTIL_SharedRandomFloat(shared_rand + iSeedOfs + 2, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + iSeedOfs + 3, -0.5, 0.5);
					z = x * x + y * y;

					// we used 4 times, thus we plus 4.
					iSeedOfs += 4;
				}
				while (z > 1);

				for (i = 0; i < 3; i++)
				{
					vecDir[i] = vecDirShooting[i] + x * vecSpread[0] * right[i] + y * vecSpread[1] * up[i];
					vecEnd[i] = vecSrc[i] + flDistance * vecDir[i];
				}
			}
			else
			{
				for (i = 0; i < 3; i++)
				{
					vecDir[i] = vecDirShooting[i] + vecSpread[0] * right[i] + vecSpread[1] * up[i];
					vecEnd[i] = vecSrc[i] + flDistance * vecDir[i];
				}
			}

			iPenetration = iSourcePenetration;
			bCreatedEffects = FALSE;

			gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
			gEngfuncs.pEventAPI->EV_PushPMStates();
			gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			bStreakSplash = false;

			while (iPenetration > 0)
			{
				gEngfuncs.pEventAPI->EV_PlayerTrace(vecStart, vecEnd, PM_NORMAL, -1, &tr);	// FIXME: push/pop monsters??

				tracer = EV_HLDM_CheckTracer(idx, vecStart, vecEnd, forward, right, iBulletType, iTracerFreq, tracerCount);
				flCurrentDistance = tr.fraction * flDistance;
				if (flCurrentDistance <= 0.0)
					break;

				if (!bCreatedEffects)
				{
					bCreatedEffects = TRUE;
					Vector source;
					if (EV_IsLocal(idx))
						source = gEngfuncs.GetViewModel()->attachment[iAttachment];
					else
						source = gEngfuncs.GetEntityByIndex(idx)->attachment[0];

					if (cl_tracereffect->value)
						gEngfuncs.pEfxAPI->R_TracerParticles(source, (tr.endpos - source).Normalize() * 6000, (tr.endpos - source).Length() / 6000);
					if (cl_gunbubbles->value)
					{
						Vector end, dir;
						dir = vecDir;

						for (int i = 0; i < 3; i++)
						{
							end[i] = tr.endpos[i] - 5.0 * vecDir[i];
						}

						if (gEngfuncs.PM_PointContents(end, NULL) == CONTENTS_WATER)
						{
							int sModelIndexBubbles = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/bubble.spr");

							if (cl_gunbubbles->value == 2)
							{
								cl_entity_t* ent;
								Vector src(vecSrc);

								if (EV_IsLocal(idx))
								{
									ent = gEngfuncs.GetViewModel();
									src = ent->attachment[iAttachment];
									dir = (end - src).Normalize();
								}

								Vector dir = (end - src).Normalize();

								float height, distance;

								if (gEngfuncs.PM_PointContents(src, NULL) == CONTENTS_WATER)
								{
									height = src.z - end.z;
									distance = (end - src).Length();

									if (height < 20)
										height = 20;
								}
								else
								{
									Vector test = end;

									while (gEngfuncs.PM_PointContents(test, NULL) == CONTENTS_WATER)
									{
										test.z += 2;
									}

									height = test.z - end.z - 5 - 2;
									distance = (end - src).Length();

									src = end - dir * (height / (src.z - end.z) * distance);
								}

								int count = (src - end).Length() / 8;

								if (count && height)
									gEngfuncs.pEfxAPI->R_BubbleTrail(src, end, height, sModelIndexBubbles, count, 2.0);
							}
							else
							{
								Vector mins, maxs;
								mins.x = tr.endpos[0] - 2.0;
								mins.y = tr.endpos[1] - 2.0;
								mins.z = tr.endpos[2] - 2.0;
								maxs.x = tr.endpos[0] + 15.0;
								maxs.y = tr.endpos[1] + 15.0;
								maxs.z = tr.endpos[2] + 15.0;

								gEngfuncs.pEfxAPI->R_Bubbles(mins, maxs, 20, sModelIndexBubbles, 10, 2.0);
							}
						}
					}
				}

				iPenetration--;
				if (flCurrentDistance > flPenetrationDistance)
					iPenetration = 0;
				flTempDistance = flCurrentDistance;
				chTextureType = EV_HLDM_PlayTextureSound(idx, &tr, vecStart, vecEnd, iBulletType);

				switch (chTextureType)
				{
				case CHAR_TEX_METAL:
					bStreakSplash = true;
					flCurrentDistance = iPower * 0.15;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunshot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_CONCRETE:
					bStreakSplash = true;
					r = g = b = 65;
					flCurrentDistance = iPower * 0.25;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunshot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_GRATE:
				case CHAR_TEX_VENT:
					bStreakSplash = true;
					flCurrentDistance = iPower * 0.5;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunshot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_TILE:
					bStreakSplash = true;
					flCurrentDistance = iPower * 0.65;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunshot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_COMPUTER:
					bStreakSplash = true;
					flCurrentDistance = iPower * 0.4;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunshot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_WOOD:
					bStreakSplash = false;
					r = 75;
					g = 42;
					b = 15;
					flScale = 0.5;
					EV_HLDM_DecalGunshot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				default:
					EV_HLDM_DecalGunshot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				}

				if (!iPenetration)
					break;
				flDistance = (flDistance - flTempDistance) * 0.5;
				vecStart = iPower * vecDir + tr.endpos - vecDir;
				vecEnd = vecDir * flDistance + vecStart;

				float flDepth = 1.0;

				if (iPower > 1)
				{
					while (1)
					{
						gEngfuncs.pEventAPI->EV_SetTraceHull(2);
						gEngfuncs.pEventAPI->EV_PlayerTrace(tr.endpos + vecDir * flDepth, tr.endpos + vecDir * flDepth * 2, PM_NORMAL, -1, &tr);
						if (tr.startsolid)
						{
							if (tr.inopen)
								break;
						}
						flDepth += 1.0;
						if (flDepth >= iPower)
							break;
					}
					if (flDepth < iPower)
						EV_HLDM_DecalGunshot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
				}
				continue;
			}
			gEngfuncs.pEventAPI->EV_PopPMStates();
			iShots--;
		}
	}
}

void EV_HLDM_FireBullets(int idx, Vector& forward, Vector& right, Vector& up, int cShots, Vector& vecSrc, Vector& vecDirShooting, const Vector& vecSpread, float flDistance, AmmoIdType iBulletType, int iPenetration)
{
	return EV_HLDM_FireBullets(idx, forward, right, up, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, 0, 0, iPenetration, 0, false, 0.0f, 0);
}

void EV_HLDM_FireBullets(int idx, Vector& forward, Vector& right, Vector& up, int cShots, Vector& vecSrc, Vector& vecDirShooting, const Vector& vecSpread, float flDistance, AmmoIdType iBulletType, int iPenetration, int shared_rand)
{
	return EV_HLDM_FireBullets(idx, forward, right, up, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, 0, 0, iPenetration, 0, false, 0.0f, shared_rand);
}

void EV_PlayGunFire(int idx, const char* sample, float flMaxDistance, const Vector& src, int iPitch)
{
	// to avoid the weird first personal gun fire effect, we have to determind whether we should use 3D sound.
	/*if (EV_IsLocal(idx))
	{
		PlaySound(sample, iPitch);
	}
	else
	{*/
		Play3DSound(sample, 1.0f, flMaxDistance, src, iPitch);
	//}
}

inline void EV_PlayGunFire(int idx, const char* sample, float flMaxDistance, const Vector& src)
{
	return EV_PlayGunFire(idx, sample, flMaxDistance, src, RANDOM_LONG(94, 110));
}

DECLARE_EVENT(FireAK47)
{
	int idx = args->entindex;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(UTIL_SharedRandomLong(gPseudoPlayer.random_seed, AK47_SHOOT1, AK47_SHOOT3));

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 3, 0.35, 20, 20, 20, EV_RIFLE_SMOKE, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, 10.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		ShellVelocity[2] -= 75;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iRShell, TE_BOUNCE_SHELL, 9);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, AK47_FIRE_SFX, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, AK47_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_AK47].m_iAmmoType, AK47_PENETRATION);
}

DECLARE_EVENT(FireXM8)
{
	int idx = args->entindex;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		int iAnim = XM8_FIRE;
		if (args->bparam2)	// m_bInZoom
		{
			iAnim = XM8_FIRE_AIM;

			if (args->bparam1)	// !!(m_iClip <= 0)
				iAnim = XM8_FIRE_AIM_LAST;
		}
		else if (args->bparam1)	// !!(m_iClip <= 0)
			iAnim = XM8_FIRE_LAST;

		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(iAnim);

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 3, 0.26, 15, 15, 15, EV_RIFLE_SMOKE, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 17.0, -8.0, -14.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 17.0, -8.0, 14.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.25, ShellVelocity);
		ShellVelocity[2] -= 122;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iRShell, TE_BOUNCE_SHELL, 8);

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	// original goldsrc api: VOL = 1.0, ATTN = 0.48
	EV_PlayGunFire(idx, XM8_FIRE_SFX, XM8_GUN_VOLUME, vecSrc + forward * 10.0f);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, XM8_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_XM8].m_iAmmoType, XM8_PENETRATION);
}

DECLARE_EVENT(FireAWP)
{
	int idx = args->entindex;
	Vector origin = args->origin;
	Vector angles = args->angles;
	Vector velocity = args->velocity;
	Vector forward = args->angles.MakeVector(), right, up;

	angles.pitch += args->iparam1 / 100.0;
	angles.yaw += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
		{
			if (args->bparam1)	// m_iClip > 0
				g_pCurWeapon->SendWeaponAnim(AWP_SHOOT_REC);
			else
				g_pCurWeapon->SendWeaponAnim(AWP_SHOOT_LAST);
		}

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 3, 0.5, 20, 20, 20, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 40, 0.5, 15, 15, 15, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 80, 0.5, 10, 10, 10, EV_WALL_PUFF, velocity, false, 35);
	}

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	// in awp, bparam2 is SILENCER.
	// original goldsrc api: VOL = 1.0, ATTN = 0.28
	EV_PlayGunFire(idx, AWP_FIRE_SFX, args->bparam2 ? NORMAL_GUN_VOLUME : AWP_GUN_VOLUME, vecSrc + forward * 10.0f);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, AWP_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_AWP].m_iAmmoType, AWP_PENETRATION);
}

DECLARE_EVENT(FireDEagle)
{
	int idx = args->entindex;
	bool empty = args->bparam1;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
				g_pCurWeapon->SendWeaponAnim(empty ? DEAGLE_SHOOT_EMPTY : DEAGLE_SHOOT);

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 0, 0.25, 10, 10, 10, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 25, 0.3, 15, 15, 15, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 50, 0.2, 25, 25, 25, EV_WALL_PUFF, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, -16.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] += 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iPShell, TE_BOUNCE_SHELL, 5);

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	// original goldsrc api: VOL = 1.0, ATTN = 0.6
	EV_PlayGunFire(idx, DEagle_FIRE_SFX, DEAGLE_GUN_VOLUME, vecSrc + forward * 10.0f);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, DEAGLE_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_DEAGLE].m_iAmmoType, DEAGLE_PENETRATION);
}

DECLARE_EVENT(FireM45A1)
{
	int idx = args->entindex;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
		{
			// on host ev sending, the bparams are used for first-personal shooting anim.
			int iAnim = 0;
			if (args->bparam1)	// m_iClip >= 0
			{
				if (args->bparam2)	// m_bInZoom
					iAnim = UTIL_SharedRandomLong(gPseudoPlayer.random_seed, M45A1_AIM_SHOOT_A, M45A1_AIM_SHOOT_B);
				else
					iAnim = M45A1_SHOOT;
			}
			else
			{
				if (args->bparam2)	// m_bInZoom
					iAnim = M45A1_AIM_SHOOT_LAST;
				else
					iAnim = M45A1_SHOOT_LAST;
			}

			g_pCurWeapon->SendWeaponAnim(iAnim);
		}

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 0, 0.25, 10, 10, 10, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 25, 0.3, 15, 15, 15, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 50, 0.2, 25, 25, 25, EV_WALL_PUFF, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, -16.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iPShell, TE_BOUNCE_SHELL, 5);

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	// on non-host sending, the bparam1 is reserved for silencer status.
	if (EV_IsLocal(idx))
		args->bparam1 = g_iRoleType == Role_Assassin;	// needs convertion for local EV.

	// original goldsrc api: VOL = 1.0, ATTN = 0.6
	EV_PlayGunFire(idx, M45A1_FIRE_SFX, args->bparam1 ? QUIET_GUN_VOLUME : M45A1_GUN_VOLUME, vecSrc + forward * 10.0f);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, M45A1_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_DEAGLE].m_iAmmoType, M45A1_PENETRATION);
}

DECLARE_EVENT(Fire57)
{
	int idx = args->entindex;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
		{
			// on host ev sending, the bparams are used for first-personal shooting anim.
			int iAnim = 0;
			if (args->bparam1)	// m_iClip > 0
			{
				if (args->bparam2)	// m_bInZoom
					iAnim = FIVESEVEN_AIM_SHOOT;
				else
					iAnim = FIVESEVEN_SHOOT;
			}
			else
			{
				if (args->bparam2)	// m_bInZoom
					iAnim = FIVESEVEN_AIM_SHOOT_LAST;
				else
					iAnim = FIVESEVEN_SHOOT_LAST;
			}

			g_pCurWeapon->SendWeaponAnim(iAnim);
		}

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 0, 0.25, 10, 10, 10, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 25, 0.3, 15, 15, 15, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 50, 0.2, 20, 20, 20, EV_WALL_PUFF, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, -14.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, 14.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 35.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iPShell, TE_BOUNCE_SHELL, idx, 4);

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	// on non-host sending, the bparam1 is reserved for silencer status.
	if (EV_IsLocal(idx))
		args->bparam1 = g_iRoleType == Role_Assassin;

	// original goldsrc api: VOL = 1.0, ATTN = 0.8
	EV_PlayGunFire(idx,
		args->bparam1 ? FN57_FIRE_SIL_SFX : FN57_FIRE_SFX,
		args->bparam1 ? QUIET_GUN_VOLUME : FIVESEVEN_GUN_VOLUME,
		vecSrc + forward * 10.0f);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, FIVESEVEN_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_FIVESEVEN].m_iAmmoType, FIVESEVEN_PENETRATION);
}

DECLARE_EVENT(FireSVD)
{
	int idx = args->entindex;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(UTIL_SharedRandomLong(gPseudoPlayer.random_seed, SVD_SHOOT, SVD_SHOOT2));

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 3, 0.3, 35, 35, 35, EV_RIFLE_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 35, 0.35, 30, 30, 30, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 70, 0.3, 25, 25, 25, EV_WALL_PUFF, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, 10.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.5, ShellVelocity);
		ShellVelocity[2] -= 50.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iRShell, TE_BOUNCE_SHELL, idx, 17);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, SVD_FIRE_SFX, 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, SVD_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_SVD].m_iAmmoType, SVD_PENETRATION);
}

DECLARE_EVENT(FireGALIL)
{

}

DECLARE_EVENT(Fireglock18)
{
	int idx = args->entindex;
	bool empty = args->bparam1;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		int seq = GLOCK18_SHOOT;
		if (empty)
			seq = GLOCK18_SHOOT_EMPTY;
		else
			seq = UTIL_SharedRandomLong(gPseudoPlayer.random_seed, GLOCK18_SHOOT, GLOCK18_SHOOT3);

		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(seq);

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 0, 0.2, 10, 10, 10, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 40, 0.4, 20, 20, 20, EV_WALL_PUFF, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, -14.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, 14.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.65, ShellVelocity);
		ShellVelocity[2] += 25.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iPShell, TE_BOUNCE_SHELL, idx, 4);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, G18C_FIRE_SFX, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, GLOCK18_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_GLOCK18].m_iAmmoType, GLOCK18_PENETRATION);
}

DECLARE_EVENT(Knife)
{

}

DECLARE_EVENT(FireMK46)
{
	int idx = args->entindex;
	bool bInScope = args->bparam1;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		int seq = MK46_SHOOT_UNSCOPE;
		if (bInScope)
			seq = UTIL_SharedRandomLong(gPseudoPlayer.random_seed, MK46_SHOOT1, MK46_SHOOT3);

		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(seq);

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 3, 0.26, 15, 15, 15, EV_RIFLE_SMOKE, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -10.0, -13.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -10.0, 13.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.2, ShellVelocity);
		ShellVelocity[2] -= 75;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iRShell, TE_BOUNCE_SHELL, idx, 10);

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	// original goldsrc api: VOL = 1.0, ATTN = 0.52
	EV_PlayGunFire(idx, MK46_FIRE_SFX, MK46_GUN_VOLUME, vecSrc + forward * 10.0f);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, MK46_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_MK46].m_iAmmoType, MK46_PENETRATION);
}

DECLARE_EVENT(FireKSG12)
{
	int idx = args->entindex;
	Vector origin = args->origin;
	Vector angles = args->angles;
	Vector velocity = args->velocity;
	Vector ShellVelocity;
	Vector ShellOrigin;
	Vector up, right, forward;
	cl_entity_t* ent = gEngfuncs.GetViewModel();
	int lefthand = cl_righthand->value;
	int shared_rand = args->iparam2;

	angles.pitch += args->iparam1 / 100.0f;	// only for x.

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		// shoot anim.
		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(UTIL_SharedRandomLong(gPseudoPlayer.random_seed, KSG12_FIRE1, KSG12_FIRE2));

		// first personal smoke VFX.
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.5, 25, 25, 25, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.5, 20, 20, 20, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.45, 15, 15, 15, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 75, 0.35, 7, 7, 7, EV_WALL_PUFF, velocity, false, 35);
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, KSG12_FIRE_SFX, 1.0, 0.48, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	Vector vecSrc = EV_GetGunPosition(args, origin);
	EV_HLDM_FireBullets(idx, forward, right, up, KSG12_PROJECTILE_COUNT, vecSrc, forward, KSG12_CONE_VECTOR, KSG12_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_KSG12].m_iAmmoType, 1, shared_rand);
}

DECLARE_EVENT(FireM4A1)
{
	int idx = args->entindex;
	//bool silencer_on = args->bparam1;	// disused

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(UTIL_SharedRandomLong(gPseudoPlayer.random_seed, M4A1_SHOOT_BACKWARD, M4A1_SHOOT_RIGHTWARD));

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[1], forward, 3, 0.2, 16, 16, 16, EV_RIFLE_SMOKE, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		ShellVelocity[2] -= 45;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iRShell, TE_BOUNCE_SHELL, idx, 10);

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	// original goldsrc api: VOL = 1.0, ATTN = 0.52
	EV_PlayGunFire(idx, M4A1_FIRE_SFX, M4A1_GUN_VOLUME, vecSrc + forward * 10.0f);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, M4A1_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_M4A1].m_iAmmoType, M4A1_PENETRATION);
}

DECLARE_EVENT(FireMAC10)
{

}

DECLARE_EVENT(FireMP5)
{

}

DECLARE_EVENT(FireAnaconda)
{
	Vector origin;
	Vector angles;
	Vector velocity;
	Vector up, right, forward;

	int idx = args->entindex;
	bool empty = args->bparam1;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (empty)
			g_pCurWeapon->SendWeaponAnim(ANACONDA_SHOOT_EMPTY);
		else
			g_pCurWeapon->SendWeaponAnim(UTIL_SharedRandomLong(gPseudoPlayer.random_seed, ANACONDA_SHOOT1, ANACONDA_SHOOT3));

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 0, 0.25, 10, 10, 10, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 25, 0.3, 15, 15, 15, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 50, 0.2, 20, 20, 20, EV_WALL_PUFF, velocity, false, 35);
	}

	// no shell VFX while shooting for Anaconda. since it is a revolvor.

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, Anaconda_FIRE_SFX, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, ANACONDA_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_ANACONDA].m_iAmmoType, ANACONDA_PENETRATION);
}

DECLARE_EVENT(FireP90)
{

}

DECLARE_EVENT(FireScout)
{

}

DECLARE_EVENT(FireSG550)
{

}

DECLARE_EVENT(FireSCARH)
{
	int idx = args->entindex;
	bool bLastBullet = args->bparam1;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		int seq = SCARH_SHOOT_LAST;
		if (!bLastBullet)
			seq = UTIL_SharedRandomLong(gPseudoPlayer.random_seed, SCARH_SHOOT1, SCARH_SHOOT3);

		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(seq);

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 3, 0.2, 18, 18, 18, EV_RIFLE_SMOKE, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, 10.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.65, ShellVelocity);
		ShellVelocity[2] -= 120;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iRShell, TE_BOUNCE_SHELL, idx, 15);

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, SCARH_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_SCARH].m_iAmmoType, SCARH_PENETRATION);

	// original goldsrc api: VOL = 1.0, ATTN = 0.4
	EV_PlayGunFire(idx, SCARH_FIRE_SFX, SCARH_GUN_VOLUME, vecSrc + forward * 10.0f);
}

DECLARE_EVENT(FireMP7A1)
{
	int idx = args->entindex;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(UTIL_SharedRandomLong(gPseudoPlayer.random_seed, MP7A1_SHOOT1, MP7A1_SHOOT3));

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 3, 0.2, 10, 10, 10, EV_PISTOL_SMOKE, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 32.0, -6.0, -11.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 32.0, -6.0, 11.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.2, ShellVelocity);
		ShellVelocity[2] -= 50;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iPShell, TE_BOUNCE_SHELL, 15);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, MP7A1_FIRE_SFX, 1.0, 1.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, MP7A1_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_MP7A1].m_iAmmoType, MP7A1_PENETRATION);
}

DECLARE_EVENT(FireUMP45)
{
	int idx = args->entindex;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(args->bparam1 ? UMP45_SHOOT_AIM : UMP45_SHOOT);

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 3, 0.25, 10, 10, 10, EV_PISTOL_SMOKE, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (cl_righthand->value == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 34.0, -10.0, -11.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 34.0, -10.0, 11.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		ShellVelocity[2] -= 50;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iPShell, TE_BOUNCE_SHELL, idx, 13);

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	// original API: vol = 1.0, attn = 0.64, pitch = 94~110
	EV_PlayGunFire(idx,
		args->bparam2 ? UMP45_FIRE_SIL_SFX : UMP45_FIRE_SFX,
		args->bparam2 ? QUIET_GUN_VOLUME : UMP45_GUN_VOLUME,
		vecSrc + forward * 10.0f, RANDOM_LONG(87, 105));

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vSpread, UMP45_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_UMP45].m_iAmmoType, UMP45_PENETRATION);
}

DECLARE_EVENT(FireUSP)
{
	Vector ShellVelocity;
	Vector ShellOrigin;

	bool silencer_on = !args->bparam2;	// useless
	bool empty = !args->bparam1;
	int    idx = args->entindex;
	Vector origin(args->origin);
	Vector angles(
		args->iparam1 / 100.0f + args->angles[0],
		args->iparam2 / 100.0f + args->angles[1],
		args->angles[2]
	);
	Vector velocity(args->velocity);
	Vector forward, right, up;

	AngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		int seq;
		EV_MuzzleFlash();
		if (!empty)
			seq = UTIL_SharedRandomFloat(gPseudoPlayer.random_seed, USP_SHOOT1, USP_SHOOT3);
		else
			seq = USP_SHOOT_EMPTY;

		// first personal gun smoke.
		Vector smoke_origin = g_pViewEnt->attachment[0] - forward * 3.0f;
		float base_scale = RANDOM_FLOAT(0.1, 0.25);

		EV_HLDM_CreateSmoke(smoke_origin, forward, 0, base_scale, 7, 7, 7, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 20, base_scale + 0.1, 10, 10, 10, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 40, base_scale, 13, 13, 13, EV_WALL_PUFF, velocity, false, 35);

		// shoot anim.
		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(seq, 2);

		if (!cl_righthand->value)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, -14.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, 14.0);

		ShellOrigin = g_pViewEnt->attachment[1];	// use the weapon attachment instead.
		VectorScale(ShellVelocity, 0.5, ShellVelocity);
		ShellVelocity[2] += 45.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iPShell, TE_BOUNCE_SHELL);

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	// original API: vol = 1.0, attn = 0.8, pitch = 87~105
	EV_PlayGunFire(idx, USP_FIRE_SFX, QUIET_GUN_VOLUME, vecSrc + forward * 10.0f, RANDOM_LONG(87, 105));

	EV_HLDM_FireBullets(idx,
		forward, right, up,
		1, vecSrc, forward,
		vSpread, USP_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_USP].m_iAmmoType,
		USP_PENETRATION);
}

DECLARE_EVENT(FireM1014)
{
	int idx = args->entindex;
	int lefthand = cl_righthand->value;

	Vector origin, angles, velocity;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles.pitch += args->iparam1 / 100.0f;	// only for x. args->iparam2 is used for seed.

	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shotgunshell.mdl");

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();

		if (g_pCurWeapon)
		{
			// on host ev sending, the bparams are used for first-personal shooting anim.
			int iAnim = 0;
			if (args->bparam1)	// m_iClip >= 0
			{
				if (args->bparam2)	// m_bInZoom
					iAnim = M1014_AIM_SHOOT;
				else
					iAnim = M1014_SHOOT;
			}
			else
			{
				if (args->bparam2)	// m_bInZoom
					iAnim = M1014_AIM_SHOOT_LAST;
				else
					iAnim = M1014_SHOOT_LAST;
			}

			g_pCurWeapon->SendWeaponAnim(iAnim);
		}

		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 3, 0.45, 15, 15, 15, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], forward, 40, 0.35, 9, 9, 9, EV_WALL_PUFF, velocity, false, 35);
	}

	Vector ShellVelocity, ShellOrigin;
	if (EV_IsLocal(idx))
	{
		if (lefthand == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 22.0, -9.0, -11.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 22.0, -9.0, 11.0);

		VectorCopy(g_pViewEnt->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.25, ShellVelocity);
		ShellVelocity[2] -= 50;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, shell, TE_BOUNCE_SHOTSHELL, idx, 3);

	int shared_rand = args->iparam2;
	Vector vecSrc = EV_GetGunPosition(args, origin);
	EV_HLDM_FireBullets(idx, forward, right, up, M1014_PROJECTILE_COUNT, vecSrc, forward, M1014_CONE_VECTOR, M1014_EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_M1014].m_iAmmoType, 1, shared_rand);

	// original goldsrc api: VOL = 1.0, ATTN = 0.52
	EV_PlayGunFire(idx, M1014_FIRE_SFX, M1014_GUN_VOLUME, vecSrc + forward * 10.0f);
}

DECLARE_EVENT(CreateExplo)
{
	// goal: some somke around the explosion centre...

	const model_t* pGasModel = gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/gas_puff_01.spr"));

	for (int i = 0; i < 5; i++)
	{
		// randomize smoke cloud position
		Vector org(args->origin);
		org.x += RANDOM_FLOAT(-100.0f, 100.0f);
		org.y += RANDOM_FLOAT(-100.0f, 100.0f);
		org.z += 30;

		TEMPENTITY* pTemp = gEngfuncs.pEfxAPI->CL_TempEntAlloc(org, (model_s*)pGasModel);
		if (pTemp)
		{
			// don't die when animation is ended
			pTemp->flags |= (FTENT_SPRANIMATELOOP | FTENT_COLLIDEWORLD | FTENT_CLIENTCUSTOM);
			pTemp->die = gEngfuncs.GetClientTime() + 5.0f;
			pTemp->callback = EV_Smoke_FadeOut;
			pTemp->entity.curstate.fuser3 = gEngfuncs.GetClientTime() - 10.0f; // start fading instantly
			pTemp->entity.curstate.fuser4 = gEngfuncs.GetClientTime(); // entity creation time

			pTemp->entity.curstate.rendermode = kRenderTransAlpha;	// MoE and most clients are wrong, we should use kRenderTransAlpha here...
			pTemp->entity.curstate.renderamt = 200;
			pTemp->entity.curstate.rendercolor.r = RANDOM_LONG(210, 230);
			pTemp->entity.curstate.rendercolor.g = RANDOM_LONG(210, 230);
			pTemp->entity.curstate.rendercolor.b = RANDOM_LONG(210, 230);
			pTemp->entity.curstate.scale = 5.0f;

			// make it move slowly
			pTemp->entity.baseline.origin.x = RANDOM_LONG(-5, 5);
			pTemp->entity.baseline.origin.y = RANDOM_LONG(-5, 5);
			pTemp->entity.baseline.renderamt = 18;
		}
	}
}

DECLARE_EVENT(CreateSmoke)	// smokeRadius = 115.0f. From bot_manager.cpp
{
	TEMPENTITY* pTemp;

	if (!args->bparam2) // first explosion
	{
		const model_t* pGasModel = gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/gas_puff_01.spr"));

		// regional fog VFX.
		RegionalFog RFog;
		RFog.m_Color = Vector(75, 75, 75);
		RFog.m_flDecayMultiplier = 1;	// start from 1.0f
		RFog.m_flDensity = 0.0015f;
		RFog.m_flRadius = 230;
		RFog.m_flTimeRemoval = g_flClientTime + 30;
		RFog.m_flTimeStartDecay = g_flClientTime + 15;
		RFog.m_vecOrigin = args->origin;
		g_lstRegionalFog.push_back(RFog);

		for (int i = 0; i < 20; i++)	// SMOKE_CLOUDS == 20
		{
			// randomize smoke cloud position
			Vector org(args->origin);
			org.x += RANDOM_FLOAT(-100.0f, 100.0f);
			org.y += RANDOM_FLOAT(-100.0f, 100.0f);
			org.z += 30;

			pTemp = gEngfuncs.pEfxAPI->CL_TempEntAlloc(org, (model_s*)pGasModel);
			if (pTemp)
			{
				// don't die when animation is ended
				pTemp->flags |= (FTENT_SPRANIMATELOOP | FTENT_COLLIDEWORLD | FTENT_CLIENTCUSTOM);
				pTemp->die = gEngfuncs.GetClientTime() + 30.0f;
				pTemp->callback = EV_Smoke_FadeOut;
				pTemp->entity.curstate.fuser3 = gEngfuncs.GetClientTime() + 15.0f; // start fading after 15 sec
				pTemp->entity.curstate.fuser4 = gEngfuncs.GetClientTime(); // entity creation time

				pTemp->entity.curstate.rendermode = kRenderTransAlpha;	// MoE and most clients are wrong, we should use kRenderTransAlpha here...
				pTemp->entity.curstate.renderamt = 200;
				pTemp->entity.curstate.rendercolor.r = RANDOM_LONG(210, 230);
				pTemp->entity.curstate.rendercolor.g = RANDOM_LONG(210, 230);
				pTemp->entity.curstate.rendercolor.b = RANDOM_LONG(210, 230);
				pTemp->entity.curstate.scale = 5.0f;

				// make it move slowly
				pTemp->entity.baseline.origin.x = RANDOM_LONG(-5, 5);
				pTemp->entity.baseline.origin.y = RANDOM_LONG(-5, 5);
				pTemp->entity.baseline.renderamt = 18;
			}
		}
	}
	else // second and other
	{
		pTemp = gEngfuncs.pEfxAPI->R_DefaultSprite(args->origin, g_iBlackSmoke, 6.0f);

		if (pTemp)
		{
			pTemp->flags |= (FTENT_CLIENTCUSTOM | FTENT_COLLIDEWORLD);
			pTemp->callback = EV_CS16Client_KillEveryRound;
			pTemp->entity.curstate.fuser4 = gEngfuncs.GetClientTime();

			pTemp->entity.curstate.rendermode = kRenderTransAlpha;	// MoE and most clients are wrong, we should use kRenderTransAlpha here...
			pTemp->entity.curstate.rendercolor.r = RANDOM_LONG(210, 230);
			pTemp->entity.curstate.rendercolor.g = RANDOM_LONG(210, 230);
			pTemp->entity.curstate.rendercolor.b = RANDOM_LONG(210, 230);
			pTemp->entity.curstate.renderamt = RANDOM_LONG(180, 200);

			pTemp->entity.baseline.origin[0] = RANDOM_LONG(10, 30);
		}
	}
}

DECLARE_EVENT(CryoExplo)
{
	const model_t* pGasModel = gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/gas_puff_01.spr"));
	const model_t* pSnowModel = IEngineStudio.Mod_ForName("models/VFX/snow_particle.mdl", TRUE);
	TEMPENTITY* pTemp = nullptr;

	dlight_t* dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
	dl->origin = args->origin;
	dl->radius = 240.0f;
	dl->color.r = 0;
	dl->color.g = 50;
	dl->color.b = 200;
	dl->die = gEngfuncs.GetClientTime() + 4.0f;
	dl->decay = 40.0f;

	for (int i = 0; i < 5; i++)
	{
		// randomize smoke cloud position
		Vector org(args->origin);
		org.x += RANDOM_FLOAT(-100.0f, 100.0f);
		org.y += RANDOM_FLOAT(-100.0f, 100.0f);
		org.z += 30;

		pTemp = gEngfuncs.pEfxAPI->CL_TempEntAlloc(org, (model_s*)pGasModel);
		if (pTemp)
		{
			// don't die when animation is ended
			pTemp->flags |= (FTENT_SPRANIMATELOOP | FTENT_COLLIDEWORLD | FTENT_CLIENTCUSTOM);
			pTemp->die = gEngfuncs.GetClientTime() + 7.5f;
			pTemp->callback = EV_Smoke_FadeOut;
			pTemp->entity.curstate.fuser3 = gEngfuncs.GetClientTime() - 7.5f; // start fading instantly
			pTemp->entity.curstate.fuser4 = gEngfuncs.GetClientTime(); // entity creation time

			pTemp->entity.curstate.rendermode = kRenderTransAlpha;	// MoE and most clients are wrong, we should use kRenderTransAlpha here...
			pTemp->entity.curstate.renderamt = 200;
			pTemp->entity.curstate.rendercolor.r = 0;
			pTemp->entity.curstate.rendercolor.g = 50;
			pTemp->entity.curstate.rendercolor.b = 200;
			pTemp->entity.curstate.scale = 5.0f;

			// make it move slowly
			pTemp->entity.baseline.origin.x = RANDOM_LONG(-5, 5);
			pTemp->entity.baseline.origin.y = RANDOM_LONG(-5, 5);
			pTemp->entity.baseline.renderamt = 18;
		}

		pTemp = gEngfuncs.pEfxAPI->CL_TempEntAlloc(org, (model_s*)pSnowModel);
		if (pTemp)
		{
			// don't die when animation is ended
			pTemp->flags |= (FTENT_ROTATE | FTENT_SPRANIMATELOOP | FTENT_COLLIDEWORLD | FTENT_CLIENTCUSTOM);
			pTemp->die = gEngfuncs.GetClientTime() + 15.0f;
			pTemp->callback = EV_Smoke_FadeOut;
			pTemp->entity.curstate.fuser3 = gEngfuncs.GetClientTime(); // start fading instantly
			pTemp->entity.curstate.fuser4 = gEngfuncs.GetClientTime(); // entity creation time

			pTemp->entity.curstate.rendermode = kRenderTransAdd;
			pTemp->entity.curstate.renderamt = 200;
			pTemp->entity.curstate.rendercolor.r = 255;
			pTemp->entity.curstate.rendercolor.g = 255;
			pTemp->entity.curstate.rendercolor.b = 255;
			pTemp->entity.curstate.scale = 1.0f;

			// make it floating in the air
			pTemp->entity.baseline.origin.x = RANDOM_LONG(-5, 5);
			pTemp->entity.baseline.origin.y = RANDOM_LONG(-5, 5);
			pTemp->entity.baseline.origin.z = RANDOM_LONG(-5, 5);
			pTemp->entity.baseline.angles.x = RANDOM_LONG(-100, 100);
			pTemp->entity.baseline.angles.y = RANDOM_LONG(-100, 100);
			pTemp->entity.baseline.renderamt = 36;
		}
	}
}

DECLARE_EVENT(MolotovExplo)
{
	float flRadius = args->fparam1;
	float flDuration = args->fparam2;
	Vector vecOrigin = args->origin, vecEnd, vecSrc;
	int iPointContents = CONTENTS_EMPTY;
	pmtrace_t tr;
	int iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/VFX/fire3.spr");
	TEMPENTITY* pTemp = nullptr;

	dlight_t* te = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
	te->origin = args->origin;
	te->radius = flRadius;
	te->color.r = 255;
	te->color.g = 102;
	te->color.b = 0;
	te->die = gEngfuncs.GetClientTime() + flDuration + RANDOM_FLOAT(0.5, 1);	// extend it a little bit.

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);	// ignore monsters.
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);

	for (int i = 0; i < int(flRadius / 10.0f); i++)
	{
		vecOrigin = args->origin;
		vecOrigin.x += RANDOM_FLOAT(-flRadius / 2.0f, flRadius / 2.0f);
		vecOrigin.y += RANDOM_FLOAT(-flRadius / 2.0f, flRadius / 2.0f);

		if (gEngfuncs.PM_PointContents(vecOrigin, &iPointContents) != CONTENTS_EMPTY)
			vecOrigin[2] += (vecOrigin - args->origin).Length2D();

		vecEnd = vecOrigin - Vector(0, 0, 9999);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecOrigin, vecEnd, PM_WORLD_ONLY, -1, &tr);

		if (gEngfuncs.PM_PointContents(tr.endpos, &iPointContents) != CONTENTS_EMPTY)
			continue;

		// back up our candidate.
		vecSrc = tr.endpos;

		gEngfuncs.pEventAPI->EV_PlayerTrace(Vector(tr.endpos.x, tr.endpos.y, Q_max(args->origin.z, tr.endpos.z)), Vector(args->origin.x, args->origin.y, Q_max(args->origin.z, tr.endpos.z)), PM_WORLD_ONLY, -1, &tr);
		if (tr.fraction < 1.0f)	// not visible.
			continue;

		// rise a little bit.
		vecSrc.z += RANDOM_FLOAT(40, 60);

		pTemp = gEngfuncs.pEfxAPI->R_DefaultSprite(vecSrc, iModelIndex, 10);
		pTemp->entity.curstate.rendermode = kRenderTransAdd;
		pTemp->entity.curstate.renderfx = kRenderFxNone;
		pTemp->entity.curstate.renderamt = 190;
		pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat(0.7, 2);
		pTemp->entity.curstate.frame = gEngfuncs.pfnRandomLong(0, pTemp->entity.model->numframes / 2);
		pTemp->die = gEngfuncs.GetClientTime() + 99999.0;
		pTemp->flags = (FTENT_SPRCYCLE | FTENT_CLIENTCUSTOM);
		pTemp->callback = EV_FlameDeath;
		pTemp->entity.curstate.fuser1 = gEngfuncs.GetClientTime() + flDuration;	// the death time.
		pTemp->entity.curstate.iuser1 = TRUE;	// this is a flame SPR.
	}

	// pop here, we don't have any trace use later.
	gEngfuncs.pEventAPI->EV_PopPMStates();

	iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/steam1.spr");
	int iMax = int(flRadius * flRadius * M_PI / 2500.0f);

	for (int i = 0; i < iMax / 4; i++)
	{
		vecSrc = args->origin + Vector(RANDOM_FLOAT(-flRadius / 2.0f, flRadius / 2.0f), RANDOM_FLOAT(-flRadius / 2.0f, flRadius / 2.0f), RANDOM_FLOAT(140, 175));

		pTemp = gEngfuncs.pEfxAPI->R_DefaultSprite(vecSrc, iModelIndex, 10);
		pTemp->entity.curstate.rendermode = kRenderTransAlpha;
		pTemp->entity.curstate.renderfx = kRenderFxNone;
		pTemp->entity.curstate.rendercolor.r = 20;
		pTemp->entity.curstate.rendercolor.g = 20;
		pTemp->entity.curstate.rendercolor.b = 20;
		pTemp->entity.curstate.renderamt = 190;
		pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat(3.5, 4.5);
		pTemp->entity.curstate.frame = gEngfuncs.pfnRandomLong(0, pTemp->entity.model->numframes / 2);
		pTemp->die = gEngfuncs.GetClientTime() + 99999.0;
		pTemp->flags = (FTENT_SPRCYCLE | FTENT_CLIENTCUSTOM);
		pTemp->callback = EV_FlameDeath;
		pTemp->entity.curstate.fuser1 = gEngfuncs.GetClientTime() + flDuration;
	}
}

DECLARE_EVENT(DecalReset)
{
	int decalnum = (int)(gEngfuncs.pfnGetCvarFloat("r_decals"));

	for (int i = 0; i < decalnum; i++)
		gEngfuncs.pEfxAPI->R_DecalRemoveAll(i);

	g_flRoundTime = gEngfuncs.GetClientTime();
}

DECLARE_EVENT(Vehicle)
{

}

void Events_Init(void)
{
	HOOK_EVENT(ak47, FireAK47);
	HOOK_EVENT(awp, FireAWP);
	HOOK_EVENT(deagle, FireDEagle);
	HOOK_EVENT(m45a1, FireM45A1);
	HOOK_EVENT(fiveseven, Fire57);
	HOOK_EVENT(svd, FireSVD);
	HOOK_EVENT(glock18, Fireglock18);
	HOOK_EVENT(mk46, FireMK46);
	HOOK_EVENT(ksg12, FireKSG12);
	HOOK_EVENT(m4a1, FireM4A1);
	HOOK_EVENT(mac10, FireMAC10);
	HOOK_EVENT(mp5n, FireMP5);
	HOOK_EVENT(anaconda, FireAnaconda);
	HOOK_EVENT(p90, FireP90);
	HOOK_EVENT(scarh, FireSCARH);
	HOOK_EVENT(mp7a1, FireMP7A1);
	HOOK_EVENT(ump45, FireUMP45);
	HOOK_EVENT(usp, FireUSP);
	HOOK_EVENT(m1014, FireM1014);
	HOOK_EVENT(xm8, FireXM8);

	HOOK_EVENT(createexplo, CreateExplo);
	HOOK_EVENT(createsmoke, CreateSmoke);
	HOOK_EVENT(CryoExplo, CryoExplo);
	HOOK_EVENT(Molotov, MolotovExplo);
	HOOK_EVENT(decal_reset, DecalReset);
	HOOK_EVENT(vehicle, Vehicle);

	cl_gunbubbles = gEngfuncs.pfnRegisterVariable("cl_gunbubbles", "2", FCVAR_ARCHIVE);
	cl_tracereffect = gEngfuncs.pfnRegisterVariable("cl_tracereffect", "1", FCVAR_ARCHIVE);
}
