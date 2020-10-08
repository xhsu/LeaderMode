/*

Created Date: 05 Mar 2020
entity.cpp

According to CRSKY, this .cpp should handle these export functions:

HUD_AddEntity
HUD_TxferLocalOverrides
HUD_ProcessPlayerState
HUD_TxferPredictionData
HUD_CreateEntities
HUD_StudioEvent
HUD_TempEntUpdate
HUD_GetUserEntity

*/

#include "precompiled.h"

int iOnTrain[MAX_PLAYERS];

// MOVEME : hud_scoreboard.cpp
int g_iUser1;
int g_iUser2;
int g_iUser3;
int g_iTeamNumber;
int g_iPlayerClass;
int g_iPlayerFlags;
Vector g_vPlayerVelocity;
float g_flPlayerSpeed;
int g_iWeaponFlags;
int g_iWaterLevel;
float g_flRoundTime = 0;
RoleTypes g_iRoleType = Role_UNASSIGNED;

extra_player_info_t	g_PlayerExtraInfo[MAX_PLAYERS]; // additional player info sent directly to the client.dll

/*
========================
HUD_AddEntity
	Return 0 to filter entity from visible list for rendering
========================
*/
int HUD_AddEntity2(int iType, cl_entity_s* pEntity, const char* szModelName)
{
	switch (iType)
	{
	case ET_NORMAL:
	case ET_PLAYER:
		if (pEntity->player && iOnTrain[pEntity->index])
		{
			VectorCopy(pEntity->curstate.origin, pEntity->origin);
			VectorCopy(pEntity->curstate.angles, pEntity->angles);
		}
		break;
	case ET_BEAM:
	case ET_TEMPENTITY:
	case ET_FRAGMENTED:
	default:
		break;
	}

	// each frame every entity passes this function, so the overview hooks it to filter the overview entities
	// in spectator mode:
	// each frame every entity passes this function, so the overview hooks 
	// it to filter the overview entities

	if (g_iUser1)
	{
		gHUD::m_Spectator.AddOverviewEntity(iType, pEntity, szModelName);

		if ((g_iUser1 == OBS_IN_EYE || gHUD::m_Spectator.m_pip->value == INSET_IN_EYE) &&
			pEntity->index == g_iUser2)
			return 0;	// don't draw the player we are following in eye

	}

	return 1;
}

/*
=========================
HUD_TxferLocalOverrides

The server sends us our origin with extra precision as part of the clientdata structure, not during the normal
playerstate update in entity_state_t.  In order for these overrides to eventually get to the appropriate playerstate
structure, we need to copy them into the state structure at this point.
=========================
*/
void HUD_TxferLocalOverrides2(entity_state_s* pState, const clientdata_s* pClient)
{
	VectorCopy(pClient->origin, pState->origin);

	// Spectator
	pState->iuser1 = pClient->iuser1;
	pState->iuser2 = pClient->iuser2;

	// Duck prevention
	pState->iuser3 = pClient->iuser3;

	// Fire prevention
	pState->iuser4 = pClient->iuser4;
}

/*
=========================
HUD_ProcessPlayerState

We have received entity_state_t for this player over the network.  We need to copy appropriate fields to the
playerstate structure
=========================
*/
void HUD_ProcessPlayerState2(entity_state_s* pDestination, const entity_state_s* pSource)
{
	// Copy in network data
	VectorCopy(pSource->origin, pDestination->origin);
	VectorCopy(pSource->angles, pDestination->angles);

	VectorCopy(pSource->velocity, pDestination->velocity);

	pDestination->frame = pSource->frame;
	pDestination->modelindex = pSource->modelindex;
	pDestination->skin = pSource->skin;
	pDestination->effects = pSource->effects;
	pDestination->weaponmodel = pSource->weaponmodel;
	pDestination->movetype = pSource->movetype;
	pDestination->sequence = pSource->sequence;
	pDestination->animtime = pSource->animtime;

	pDestination->solid = pSource->solid;

	pDestination->rendermode = pSource->rendermode;
	pDestination->renderamt = pSource->renderamt;
	pDestination->rendercolor.r = pSource->rendercolor.r;
	pDestination->rendercolor.g = pSource->rendercolor.g;
	pDestination->rendercolor.b = pSource->rendercolor.b;
	pDestination->renderfx = pSource->renderfx;

	pDestination->framerate = pSource->framerate;
	pDestination->body = pSource->body;

	Q_memcpy(&pDestination->controller[0], &pSource->controller[0], 4 * sizeof(byte));
	Q_memcpy(&pDestination->blending[0], &pSource->blending[0], 2 * sizeof(byte));

	VectorCopy(pSource->basevelocity, pDestination->basevelocity);

	pDestination->friction = pSource->friction;
	pDestination->gravity = pSource->gravity;
	pDestination->gaitsequence = pSource->gaitsequence;
	pDestination->spectator = pSource->spectator;
	pDestination->usehull = pSource->usehull;
	pDestination->playerclass = pSource->playerclass;
	pDestination->team = pSource->team;
	pDestination->colormap = pSource->colormap;

	// Save off some data so other areas of the Client DLL can get to it
	cl_entity_t* pPlayer = gEngfuncs.GetLocalPlayer();	// Get the local player's index
	if (pDestination->number == pPlayer->index)
	{
		g_iTeamNumber = g_PlayerExtraInfo[pDestination->number].m_iTeam;

		pDestination->iuser1 = g_iUser1 = pSource->iuser1;
		pDestination->iuser2 = g_iUser2 = pSource->iuser2;
		pDestination->iuser3 = g_iUser3 = pSource->iuser3;
	}

	pDestination->fuser2 = pSource->fuser2;

	if (pSource->number > 0 && pSource->number < MAX_PLAYERS)
	{
		iOnTrain[pSource->number] = pSource->iuser4;
	}
}

/*
=========================
HUD_TxferPredictionData

Because we can predict an arbitrary number of frames before the server responds with an update, we need to be able to copy client side prediction data in
 from the state that the server ack'd receiving, which can be anywhere along the predicted frame path ( i.e., we could predict 20 frames into the future and the server ack's
 up through 10 of those frames, so we need to copy persistent client-side only state from the 10th predicted frame to the slot the server
 update is occupying.
=========================
*/
void HUD_TxferPredictionData2(entity_state_s* ps, const entity_state_s* pps, clientdata_s* pcd, const clientdata_s* ppcd, weapon_data_s* wd, const weapon_data_s* pwd)
{
	ps->oldbuttons = pps->oldbuttons;
	ps->flFallVelocity = pps->flFallVelocity;
	ps->iStepLeft = pps->iStepLeft;
	ps->playerclass = pps->playerclass;
	ps->iuser4 = pps->iuser4;

	pcd->viewmodel = ppcd->viewmodel;
	pcd->m_iId = ppcd->m_iId;
	pcd->ammo_shells = ppcd->ammo_shells;
	pcd->ammo_nails = ppcd->ammo_nails;
	pcd->ammo_cells = ppcd->ammo_cells;
	pcd->ammo_rockets = ppcd->ammo_rockets;
	pcd->m_flNextAttack = ppcd->m_flNextAttack;
	pcd->fov = ppcd->fov;
	pcd->weaponanim = ppcd->weaponanim;
	pcd->tfstate = ppcd->tfstate;
	pcd->maxspeed = ppcd->maxspeed;
	pcd->deadflag = ppcd->deadflag;
	if (gEngfuncs.IsSpectateOnly())
	{
		pcd->iuser1 = g_iUser1;	// observer mode
		pcd->iuser2 = g_iUser2; // first target
		pcd->iuser3 = g_iUser3; // second target
	}
	else
	{
		pcd->iuser1 = ppcd->iuser1;
		pcd->iuser2 = ppcd->iuser2;
		pcd->iuser3 = ppcd->iuser3;
	}
	pcd->iuser4 = ppcd->iuser4;
	pcd->fuser2 = ppcd->fuser2;
	pcd->fuser3 = ppcd->fuser3;
	pcd->vuser2 = ppcd->vuser2;
	pcd->vuser3 = ppcd->vuser3;
	pcd->vuser4 = ppcd->vuser4;

	Q_memcpy(wd, pwd, sizeof(weapon_data_t) * 32);
}

/*
=========================
HUD_CreateEntities

Gives us a chance to add additional entities to the render this frame
=========================
*/
void HUD_CreateEntities2(void)
{
	// Add in any game specific objects

	// this is from CSMoE
	//GetClientVoiceMgr()->CreateEntities();
}

/*
=========================
HUD_StudioEvent

The entity's studio model description indicated an event was
fired during this frame, handle the event by it's tag ( e.g., muzzleflash, sound )
=========================
*/
void HUD_StudioEvent2(const mstudioevent_s* pEvent, const cl_entity_s* pEntity)
{
	switch (pEvent->event)
	{
	case 5001:
		gEngfuncs.pEfxAPI->R_MuzzleFlash((float*)&pEntity->attachment[0], Q_atoi(pEvent->options));
		break;
	case 5011:
		gEngfuncs.pEfxAPI->R_MuzzleFlash((float*)&pEntity->attachment[1], Q_atoi(pEvent->options));
		break;
	case 5021:
		gEngfuncs.pEfxAPI->R_MuzzleFlash((float*)&pEntity->attachment[2], Q_atoi(pEvent->options));
		break;
	case 5031:
		gEngfuncs.pEfxAPI->R_MuzzleFlash((float*)&pEntity->attachment[3], Q_atoi(pEvent->options));
		break;
	case 5002:
		gEngfuncs.pEfxAPI->R_SparkEffect((float*)&pEntity->attachment[0], Q_atoi(pEvent->options), -100, 100);
		break;

		// Client side sound
	case 5004:
		gEngfuncs.pfnPlaySoundByName((char*)pEvent->options, VOL_NORM);	// no more engine sound API.

		//char szFilePath[128];
		//Q_snprintf(szFilePath, charsmax(szFilePath), "sound/%s", pEvent->options);

		//PlaySoundViaWinmm(szFilePath);
		break;

	default:
		break;
	}
}

/*
=================
CL_UpdateTEnts

Simulation and cleanup of temporary entities
=================
*/
void HUD_TempEntUpdate2 (
	double flFrameTime,   // Simulation time
	double flClientTime, // Absolute time on client
	double flClientGravity,  // True gravity on client
	TEMPENTITY** ppTempEntFree,   // List of freed temporary ents
	TEMPENTITY** ppTempEntActive, // List 
	int		(*Callback_AddVisibleEntity)(cl_entity_t* pEntity),
	void	(*Callback_TempEntPlaySound)(TEMPENTITY* pTemp, float damp))
{
	static int gTempEntFrame = 0;
	int			i;
	TEMPENTITY* pTemp, * pnext, * pprev;
	float		gravity, gravitySlow, life, fastFreq;

	// Nothing to simulate
	if (!*ppTempEntActive)
		return;

	// in order to have tents collide with players, we have to run the player prediction code so
	// that the client has the player list. We run this code once when we detect any COLLIDEALL 
	// tent, then set this BOOL to true so the code doesn't get run again if there's more than
	// one COLLIDEALL ent for this update. (often are).
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers (-1);

	// !!!BUGBUG	-- This needs to be time based
	gTempEntFrame = (gTempEntFrame + 1) & 31;

	pTemp = *ppTempEntActive;

	// !!! Don't simulate while paused....  This is sort of a hack, revisit.
	if (flFrameTime <= 0)
	{
		while (pTemp)
		{
			if (!(pTemp->flags & FTENT_NOMODEL))
			{
				Callback_AddVisibleEntity(&pTemp->entity);
			}
			pTemp = pTemp->next;
		}
		goto finish;
	}

	pprev = NULL;
	fastFreq = flClientTime * 5.5;
	gravity = -flFrameTime * flClientGravity;
	gravitySlow = gravity * 0.5;

	while (pTemp)
	{
		int active;

		active = 1;

		life = pTemp->die - flClientTime;
		pnext = pTemp->next;
		if (life < 0)
		{
			if (pTemp->flags & FTENT_FADEOUT)
			{
				if (pTemp->entity.curstate.rendermode == kRenderNormal)
					pTemp->entity.curstate.rendermode = kRenderTransTexture;
				pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt * (1 + life * pTemp->fadeSpeed);
				if (pTemp->entity.curstate.renderamt <= 0)
					active = 0;

			}
			else
				active = 0;
		}
		if (!active)		// Kill it
		{
			pTemp->next = *ppTempEntFree;
			*ppTempEntFree = pTemp;
			if (!pprev)	// Deleting at head of list
				*ppTempEntActive = pnext;
			else
				pprev->next = pnext;
		}
		else
		{
			pprev = pTemp;

			VectorCopy(pTemp->entity.origin, pTemp->entity.prevstate.origin);

			if (pTemp->flags & FTENT_SPARKSHOWER)
			{
				// Adjust speed if it's time
				// Scale is next think time
				if (flClientTime > pTemp->entity.baseline.scale)
				{
					// Show Sparks
					gEngfuncs.pEfxAPI->R_SparkEffect(pTemp->entity.origin, 8, -200, 200);

					// Reduce life
					pTemp->entity.baseline.framerate -= 0.1;

					if (pTemp->entity.baseline.framerate <= 0.0)
					{
						pTemp->die = flClientTime;
					}
					else
					{
						// So it will die no matter what
						pTemp->die = flClientTime + 0.5;

						// Next think
						pTemp->entity.baseline.scale = flClientTime + 0.1;
					}
				}
			}
			else if (pTemp->flags & FTENT_PLYRATTACHMENT)
			{
				cl_entity_t* pClient;

				pClient = gEngfuncs.GetEntityByIndex(pTemp->clientIndex);

				VectorAdd(pClient->origin, pTemp->tentOffset, pTemp->entity.origin);
			}
			else if (pTemp->flags & FTENT_SINEWAVE)
			{
				pTemp->x += pTemp->entity.baseline.origin[0] * flFrameTime;
				pTemp->y += pTemp->entity.baseline.origin[1] * flFrameTime;

				pTemp->entity.origin[0] = pTemp->x + sin(pTemp->entity.baseline.origin[2] + flClientTime * pTemp->entity.prevstate.frame) * (10 * pTemp->entity.curstate.framerate);
				pTemp->entity.origin[1] = pTemp->y + sin(pTemp->entity.baseline.origin[2] + fastFreq + 0.7) * (8 * pTemp->entity.curstate.framerate);
				pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * flFrameTime;
			}
			else if (pTemp->flags & FTENT_SPIRAL)
			{
				/*
				float s, c;
				s = sin( pTemp->entity.baseline.origin[2] + fastFreq );
				c = cos( pTemp->entity.baseline.origin[2] + fastFreq );
				*/

				pTemp->entity.origin[0] += pTemp->entity.baseline.origin[0] * flFrameTime + 8 * sin(flClientTime * 20 + (long long)(void*)pTemp);
				pTemp->entity.origin[1] += pTemp->entity.baseline.origin[1] * flFrameTime + 4 * sin(flClientTime * 30 + (long long)(void*)pTemp);
				pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * flFrameTime;
			}

			else
			{
				for (i = 0; i < 3; i++)
					pTemp->entity.origin[i] += pTemp->entity.baseline.origin[i] * flFrameTime;
			}

			if (pTemp->flags & FTENT_SPRANIMATE)
			{
				pTemp->entity.curstate.frame += flFrameTime * pTemp->entity.curstate.framerate;
				if (pTemp->entity.curstate.frame >= pTemp->frameMax)
				{
					pTemp->entity.curstate.frame = pTemp->entity.curstate.frame - (int)(pTemp->entity.curstate.frame);

					if (!(pTemp->flags & FTENT_SPRANIMATELOOP))
					{
						// this animating sprite isn't set to loop, so destroy it.
						pTemp->die = flClientTime;
						pTemp = pnext;
						continue;
					}
				}
			}
			else if (pTemp->flags & FTENT_SPRCYCLE)
			{
				pTemp->entity.curstate.frame += flFrameTime * 10;
				if (pTemp->entity.curstate.frame >= pTemp->frameMax)
				{
					pTemp->entity.curstate.frame = pTemp->entity.curstate.frame - (int)(pTemp->entity.curstate.frame);
				}
			}
			// Experiment
#if 0
			if (pTemp->flags & FTENT_SCALE)
				pTemp->entity.curstate.framerate += 20.0 * (flFrameTime / pTemp->entity.curstate.framerate);
#endif

			if (pTemp->flags & FTENT_ROTATE)
			{
				pTemp->entity.angles[0] += pTemp->entity.baseline.angles[0] * flFrameTime;
				pTemp->entity.angles[1] += pTemp->entity.baseline.angles[1] * flFrameTime;
				pTemp->entity.angles[2] += pTemp->entity.baseline.angles[2] * flFrameTime;

				VectorCopy(pTemp->entity.angles, pTemp->entity.latched.prevangles);
			}

			if (pTemp->flags & (FTENT_COLLIDEALL | FTENT_COLLIDEWORLD) && !(pTemp->flags & FTENT_IGNOREGRAVITY))
			{
				vec3_t	traceNormal;
				float	traceFraction = 1;

				if (pTemp->flags & FTENT_COLLIDEALL)
				{
					pmtrace_t pmtrace;
					physent_t* pe;

					gEngfuncs.pEventAPI->EV_SetTraceHull(2);

					gEngfuncs.pEventAPI->EV_PlayerTrace(pTemp->entity.prevstate.origin, pTemp->entity.origin, PM_STUDIO_BOX, -1, &pmtrace);

					if (pmtrace.fraction != 1)
					{
						pe = gEngfuncs.pEventAPI->EV_GetPhysent(pmtrace.ent);

						if (!pmtrace.ent || (pe->info != pTemp->clientIndex))
						{
							traceFraction = pmtrace.fraction;
							VectorCopy(pmtrace.plane.normal, traceNormal);

							if (pTemp->hitcallback)
							{
								(*pTemp->hitcallback)(pTemp, &pmtrace);
							}
						}
					}
				}
				else if (pTemp->flags & FTENT_COLLIDEWORLD)
				{
					pmtrace_t pmtrace;

					gEngfuncs.pEventAPI->EV_SetTraceHull(2);

					gEngfuncs.pEventAPI->EV_PlayerTrace(pTemp->entity.prevstate.origin, pTemp->entity.origin, PM_STUDIO_BOX | PM_WORLD_ONLY, -1, &pmtrace);

					if (pmtrace.fraction != 1)
					{
						traceFraction = pmtrace.fraction;
						VectorCopy(pmtrace.plane.normal, traceNormal);

						if (pTemp->flags & FTENT_SPARKSHOWER)
						{
							// Chop spark speeds a bit more
							//
							VectorScale(pTemp->entity.baseline.origin, 0.6, pTemp->entity.baseline.origin);

							if (VectorLength(pTemp->entity.baseline.origin) < 10)
							{
								pTemp->entity.baseline.framerate = 0.0;
							}
						}

						if (pTemp->hitcallback)
						{
							(*pTemp->hitcallback)(pTemp, &pmtrace);
						}
					}
				}

				if (traceFraction != 1)	// Decent collision now, and damping works
				{
					float  proj, damp;

					// Place at contact point
					VectorMA(pTemp->entity.prevstate.origin, traceFraction * flFrameTime, pTemp->entity.baseline.origin, pTemp->entity.origin);
					// Damp velocity
					damp = pTemp->bounceFactor;
					if (pTemp->flags & (FTENT_GRAVITY | FTENT_SLOWGRAVITY))
					{
						damp *= 0.5;
						if (traceNormal[2] > 0.9)		// Hit floor?
						{
							if (pTemp->entity.baseline.origin[2] <= 0 && pTemp->entity.baseline.origin[2] >= gravity * 3)
							{
								damp = 0;		// Stop
								pTemp->flags &= ~(FTENT_ROTATE | FTENT_GRAVITY | FTENT_SLOWGRAVITY | FTENT_COLLIDEWORLD | FTENT_SMOKETRAIL);
								pTemp->entity.angles[0] = 0;
								pTemp->entity.angles[2] = 0;
							}
						}
					}

					if (pTemp->hitSound)
					{
						Callback_TempEntPlaySound(pTemp, damp);
					}

					if (pTemp->flags & FTENT_COLLIDEKILL)
					{
						// die on impact
						pTemp->flags &= ~FTENT_FADEOUT;
						pTemp->die = flClientTime;
					}
					else
					{
						// Reflect velocity
						if (damp != 0)
						{
							proj = DotProduct(pTemp->entity.baseline.origin, traceNormal);
							VectorMA(pTemp->entity.baseline.origin, -proj * 2, traceNormal, pTemp->entity.baseline.origin);
							// Reflect rotation (fake)

							pTemp->entity.angles[1] = -pTemp->entity.angles[1];
						}

						if (damp != 1)
						{

							VectorScale(pTemp->entity.baseline.origin, damp, pTemp->entity.baseline.origin);
							VectorScale(pTemp->entity.angles, 0.9, pTemp->entity.angles);
						}
					}
				}
			}


			if ((pTemp->flags & FTENT_FLICKER) && gTempEntFrame == pTemp->entity.curstate.effects)
			{
				dlight_t* dl = gEngfuncs.pEfxAPI->CL_AllocDlight (0);
				VectorCopy (pTemp->entity.origin, dl->origin);
				dl->radius = 60;
				dl->color.r = 255;
				dl->color.g = 120;
				dl->color.b = 0;
				dl->die = flClientTime + 0.01;
			}

			if (pTemp->flags & FTENT_SMOKETRAIL)
			{
				gEngfuncs.pEfxAPI->R_RocketTrail (pTemp->entity.prevstate.origin, pTemp->entity.origin, 1);
			}

			if (!(pTemp->flags & FTENT_IGNOREGRAVITY))
			{
				if (pTemp->flags & FTENT_GRAVITY)
					pTemp->entity.baseline.origin[2] += gravity;
				else if (pTemp->flags & FTENT_SLOWGRAVITY)
					pTemp->entity.baseline.origin[2] += gravitySlow;
			}

			if (pTemp->flags & FTENT_CLIENTCUSTOM)
			{
				if (pTemp->callback)
				{
					(*pTemp->callback)(pTemp, flFrameTime, flClientTime);
				}
			}

			// Cull to PVS (not frustum cull, just PVS)
			if (!(pTemp->flags & FTENT_NOMODEL))
			{
				if (!Callback_AddVisibleEntity(&pTemp->entity))
				{
					if (!(pTemp->flags & FTENT_PERSIST))
					{
						pTemp->die = flClientTime;			// If we can't draw it this frame, just dump it.
						pTemp->flags &= ~FTENT_FADEOUT;	// Don't fade out, just die
					}
				}
			}
		}
		pTemp = pnext;
	}

finish:
	// Restore state info
	gEngfuncs.pEventAPI->EV_PopPMStates();
}

/*
=================
HUD_GetUserEntity

If you specify negative numbers for beam start and end point entities, then
  the engine will call back into this function requesting a pointer to a cl_entity_t
  object that describes the entity to attach the beam onto.

Indices must start at 1, not zero.
=================
*/
cl_entity_t* HUD_GetUserEntity2(int index)
{
	return NULL;
}

///////////////////////////////
// CUSTOM VFX FOR TEMPENTITY //
///////////////////////////////

void EV_CS16Client_KillEveryRound(TEMPENTITY* te, float frametime, float current_time)
{
	if (g_flRoundTime > te->entity.curstate.fuser4)
	{
		// Mark it die on next TempEntUpdate
		te->die = 0.0f;
		// Set null renderamt, so it will be invisible now
		// Also it will die immediately, if FTEMP_FADEOUT was set
		te->entity.curstate.renderamt = 0;
	}
}

void EV_Smoke_FadeOut(struct tempent_s* te, float frametime, float currenttime)
{
	if (te->entity.curstate.renderamt > 0 && currenttime >= te->entity.curstate.fuser3)
	{
		te->entity.curstate.renderamt = 255.0f - (currenttime - te->entity.curstate.fuser3) * te->entity.baseline.renderamt;

		if (te->entity.curstate.renderamt < 0)
			te->entity.curstate.renderamt = 0;
	}

	EV_CS16Client_KillEveryRound(te, frametime, currenttime);
}

void EV_FlameDeath(struct tempent_s* ent, float frametime, float currenttime)
{
	if (ent->entity.curstate.fuser1 > gEngfuncs.GetClientTime())	// fuser1 is the death time.
		return;

	ent->flags &= ~FTENT_CLIENTCUSTOM;
	ent->flags |= FTENT_FADEOUT;
	ent->entity.curstate.framerate = ent->fadeSpeed = 12;
	ent->die = gEngfuncs.GetClientTime() + ((ent->entity.model->numframes - ent->entity.curstate.frame) / ent->entity.curstate.framerate);

	// randomize smoke cloud position
	Vector org(ent->entity.origin);
	org.x += RANDOM_FLOAT(-100.0f, 100.0f);
	org.y += RANDOM_FLOAT(-100.0f, 100.0f);
	org.z += 30;

	// post-smoke VFX.
	const model_t* pGasModel = gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/gas_puff_01.spr"));
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
		pTemp->entity.curstate.rendercolor.r = 75;
		pTemp->entity.curstate.rendercolor.g = 75;
		pTemp->entity.curstate.rendercolor.b = 75;
		pTemp->entity.curstate.scale = 5.0f;

		// make it move slowly
		pTemp->entity.baseline.origin.x = RANDOM_LONG(-5, 5);
		pTemp->entity.baseline.origin.y = RANDOM_LONG(-5, 5);
		pTemp->entity.baseline.renderamt = 18;
	}
}
