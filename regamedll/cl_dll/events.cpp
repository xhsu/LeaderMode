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
void EV_EjectBrass(float* origin, float* velocity, float rotation, int model, int soundtype, float life)
{
	Vector angles(0.0f, 0.0f, rotation);
	gEngfuncs.pEfxAPI->R_TempModel(origin, velocity, angles, life, model, soundtype);
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

/*
============
EV_DescribeBulletTypeParameters

Sets iPenetrationPower and flPenetrationDistance.
If iBulletType is unknown, calls assert() and sets these two vars to 0
============
*/
void EV_DescribeBulletTypeParameters(int iBulletType, int& iPenetrationPower, float& flPenetrationDistance)
{
	// copy from mp.dll::cbase.cpp
	switch (iBulletType)
	{
	case BULLET_PLAYER_9MM:
		iPenetrationPower = 21;
		flPenetrationDistance = 800;
		break;
	case BULLET_PLAYER_45ACP:
		iPenetrationPower = 15;
		flPenetrationDistance = 500;
		break;
	case BULLET_PLAYER_50AE:
		iPenetrationPower = 30;
		flPenetrationDistance = 1000;
		break;
	case BULLET_PLAYER_762MM:
		iPenetrationPower = 39;
		flPenetrationDistance = 5000;
		break;
	case BULLET_PLAYER_556MM:
		iPenetrationPower = 35;
		flPenetrationDistance = 4000;
		break;
	case BULLET_PLAYER_338MAG:
		iPenetrationPower = 45;
		flPenetrationDistance = 8000;
		break;
	case BULLET_PLAYER_57MM:
		iPenetrationPower = 30;
		flPenetrationDistance = 2000;
		break;
	case BULLET_PLAYER_357SIG:
		iPenetrationPower = 25;
		flPenetrationDistance = 800;
		break;
	default:
		iPenetrationPower = 0;
		flPenetrationDistance = 0;
		break;
	}
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
		case 1:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/rifle_smoke2.spr");
		case 2:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/rifle_smoke3.spr");
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

void EV_HLDM_FireBullets(int idx, Vector& forward, Vector& right, Vector& up, int cShots, Vector& vecSrc, Vector& vecDirShooting, const Vector& vecSpread, float flDistance, int iBulletType, int iTracerFreq, int* tracerCount, int iPenetration, int iAttachment, bool lefthand, float srcofs, int shared_rand)
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

	EV_DescribeBulletTypeParameters(iBulletType, iPenetrationPower, flPenetrationDistance);

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
			if (iBulletType == BULLET_PLAYER_BUCKSHOT)
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

void EV_HLDM_FireBullets(int idx, Vector& forward, Vector& right, Vector& up, int cShots, Vector& vecSrc, Vector& vecDirShooting, const Vector& vecSpread, float flDistance, int iBulletType, int iPenetration)
{
	return EV_HLDM_FireBullets(idx, forward, right, up, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, 0, 0, iPenetration, 0, false, 0.0f, 0);
}

void EV_HLDM_FireBullets(int idx, Vector& forward, Vector& right, Vector& up, int cShots, Vector& vecSrc, Vector& vecDirShooting, const Vector& vecSpread, float flDistance, int iBulletType, int iPenetration, int shared_rand)
{
	return EV_HLDM_FireBullets(idx, forward, right, up, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, 0, 0, iPenetration, 0, false, 0.0f, shared_rand);
}

DECLARE_EVENT(FireAK47)
{

}

DECLARE_EVENT(FireAUG)
{

}

DECLARE_EVENT(FireAWP)
{

}

DECLARE_EVENT(FireDEAGLE)
{

}

DECLARE_EVENT(FireEliteLeft)
{

}

DECLARE_EVENT(FireEliteRight)
{

}

DECLARE_EVENT(FireFAMAS)
{

}

DECLARE_EVENT(Fire57)
{

}

DECLARE_EVENT(FireG3SG1)
{

}

DECLARE_EVENT(FireGALIL)
{

}

DECLARE_EVENT(Fireglock18)
{

}

DECLARE_EVENT(Knife)
{

}

DECLARE_EVENT(FireM249)
{

}

DECLARE_EVENT(FireM3)
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
		g_iShotsFired++;
		EV_MuzzleFlash();

		// shoot anim.
		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(KSG12_FIRE1 + UTIL_SharedRandomLong(gPseudoPlayer.random_seed, 0, 1), 2);

		// first personal smoke VFX.
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.5, 25, 25, 25, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.5, 20, 20, 20, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.45, 15, 15, 15, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 75, 0.35, 7, 7, 7, EV_WALL_PUFF, velocity, false, 35);
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/m3-1.wav", 1.0, 0.48, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	Vector vecSrc = EV_GetGunPosition(args, origin);
	EV_HLDM_FireBullets(idx, forward, right, up, KSG12_PROJECTILE_COUNT, vecSrc, forward, KSG12_CONE_VECTOR, KSG12_EFFECTIVE_RANGE, BULLET_PLAYER_BUCKSHOT, 1, shared_rand);
}

DECLARE_EVENT(FireM4A1)
{

}

DECLARE_EVENT(FireMAC10)
{

}

DECLARE_EVENT(FireMP5)
{

}

DECLARE_EVENT(FireP228)
{

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

DECLARE_EVENT(FireSG552)
{

}

DECLARE_EVENT(FireTMP)
{

}

DECLARE_EVENT(FireUMP45)
{

}

DECLARE_EVENT(FireUSP)
{
	Vector ShellVelocity;
	Vector ShellOrigin;

	bool silencer_on = !args->bparam2;
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
		++g_iShotsFired;

		int seq;
		if (silencer_on)
		{
			if (!empty)
				seq = UTIL_SharedRandomLong(gPseudoPlayer.random_seed, USP_UNSIL_SHOOT1, USP_UNSIL_SHOOT3);
			else
				seq = USP_UNSIL_SHOOT_EMPTY;
		}
		else
		{
			EV_MuzzleFlash();
			if (!empty)
				seq = UTIL_SharedRandomLong(gPseudoPlayer.random_seed, USP_SHOOT1, USP_SHOOT3);
			else
				seq = USP_SHOOT_EMPTY;
		}

		// first personal gun smoke.
		auto ent = gEngfuncs.GetViewModel();

		Vector smoke_origin = ent->attachment[0] - forward * 3.0f;
		float base_scale = RANDOM_FLOAT(0.1, 0.25);

		EV_HLDM_CreateSmoke(smoke_origin, forward, 0, base_scale, 7, 7, 7, EV_PISTOL_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 20, base_scale + 0.1, 10, 10, 10, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, base_scale, 13, 13, 13, EV_WALL_PUFF, velocity, false, 35);

		// shoot anim.
		if (g_pCurWeapon)
			g_pCurWeapon->SendWeaponAnim(seq, 2);

		if (!cl_righthand->value)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, -14.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, 14.0);

		ShellOrigin = ent->attachment[1];	// use the weapon attachment instead.
		VectorScale(ShellVelocity, 0.5, ShellVelocity);
		ShellVelocity[2] += 45.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iPShell, TE_BOUNCE_SHELL);

	// gunshot sound.
	if (!silencer_on)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/usp_unsil-1.wav", 1.0, ATTN_NORM, 0, 87 + gEngfuncs.pfnRandomLong(0, 0x12));
	else
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/usp1.wav", 1.0, 2.0, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	EV_HLDM_FireBullets(idx,
		forward, right, up,
		1, vecSrc, forward,
		vSpread, USP_EFFECTIVE_RANGE, BULLET_PLAYER_45ACP,
		USP_PENETRATION);
}

DECLARE_EVENT(FireXM1014)
{

}

DECLARE_EVENT(CreateExplo)
{

}

DECLARE_EVENT(CreateSmoke)
{

}

DECLARE_EVENT(DecalReset)
{

}

DECLARE_EVENT(Vehicle)
{

}

DECLARE_EVENT(FireCM901)
{
	Vector ShellVelocity;
	Vector ShellOrigin;

	bool silencer_on = !args->bparam2;
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
		++g_iShotsFired;

		int seq;
		if (silencer_on)
		{
			if (!empty)
				seq = RANDOM_LONG(CM901_SHOOT1, CM901_SHOOT1);
			else seq = CM901_SHOOT1;
		}
		else
		{
			EV_MuzzleFlash();
			if (!empty)
				seq = RANDOM_LONG(CM901_SHOOT1, CM901_SHOOT1);
			else seq = CM901_SHOOT1;
		}

		// first personal gun smoke.
		auto ent = gEngfuncs.GetViewModel();

		Vector smoke_origin = ent->attachment[0] - forward * 3.0f;
		float base_scale = RANDOM_FLOAT(0.1, 0.25);

		EV_HLDM_CreateSmoke(smoke_origin, forward, 0, base_scale, 7, 7, 7, EV_RIFLE_SMOKE, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 20, base_scale + 0.1, 10, 10, 10, EV_WALL_PUFF, velocity, false, 35);
		EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, base_scale, 13, 13, 13, EV_WALL_PUFF, velocity, false, 35);

		// shoot anim.
		gEngfuncs.pEventAPI->EV_WeaponAnimation(seq, 2);

		if (!cl_righthand->value)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, -14.0);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, 14.0);

		ShellOrigin = ent->attachment[1];	// use the weapon attachment instead.
		VectorScale(ShellVelocity, 0.5, ShellVelocity);
		ShellVelocity[2] += 45.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, g_iPShell, TE_BOUNCE_SHELL);

	// gunshot sound.
	if (!silencer_on)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/cm901-1.wav", 1.0, ATTN_NORM, 0, 87 + gEngfuncs.pfnRandomLong(0, 0x12));
	else
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/cm901-1.wav", 1.0, 2.0, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	Vector vecSrc = EV_GetGunPosition(args, origin);
	Vector vSpread = Vector(args->fparam1, args->fparam2, 0);

	EV_HLDM_FireBullets(idx,
		forward, right, up,
		1, vecSrc, forward,
		vSpread, CM901_EFFECTIVE_RANGE, BULLET_PLAYER_556MM,
		CM901_PENETRATION);
}

void Events_Init(void)
{
	HOOK_EVENT(ak47, FireAK47);
	HOOK_EVENT(aug, FireAUG);
	HOOK_EVENT(awp, FireAWP);
	HOOK_EVENT(deagle, FireDEAGLE);
	HOOK_EVENT(elite_left, FireEliteLeft);
	HOOK_EVENT(elite_right, FireEliteRight);
	HOOK_EVENT(famas, FireFAMAS);
	HOOK_EVENT(fiveseven, Fire57);
	HOOK_EVENT(g3sg1, FireG3SG1);
	HOOK_EVENT(galil, FireGALIL);
	HOOK_EVENT(glock18, Fireglock18);
	HOOK_EVENT(knife, Knife);
	HOOK_EVENT(m249, FireM249);
	HOOK_EVENT(m3, FireM3);
	HOOK_EVENT(m4a1, FireM4A1);
	HOOK_EVENT(mac10, FireMAC10);
	HOOK_EVENT(mp5n, FireMP5);
	HOOK_EVENT(p228, FireP228);
	HOOK_EVENT(p90, FireP90);
	HOOK_EVENT(scout, FireScout);
	HOOK_EVENT(sg550, FireSG550);
	HOOK_EVENT(sg552, FireSG552);
	HOOK_EVENT(tmp, FireTMP);
	HOOK_EVENT(ump45, FireUMP45);
	HOOK_EVENT(usp, FireUSP);
	HOOK_EVENT(xm1014, FireXM1014);

	HOOK_EVENT(createexplo, CreateExplo);
	HOOK_EVENT(createsmoke, CreateSmoke);
	HOOK_EVENT(decal_reset, DecalReset);
	HOOK_EVENT(vehicle, Vehicle);

	cl_gunbubbles = gEngfuncs.pfnRegisterVariable("cl_gunbubbles", "2", FCVAR_ARCHIVE);
	cl_tracereffect = gEngfuncs.pfnRegisterVariable("cl_tracereffect", "1", FCVAR_ARCHIVE);
}
