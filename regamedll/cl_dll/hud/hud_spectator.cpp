/*

Created Date: 10 Mar 2020

*/

#include "precompiled.h"

// came from PM_Shared.cpp
extern int iJumpSpectator;
extern float vJumpOrigin[3];
extern float vJumpAngles[3];


void CHudSpectator::Think(void)
{
	ButtonUpdate();
}

void CHudSpectator::Reset(void)
{
	m_FOV = 90.0f;

	m_IsInterpolating = false;

	m_ChaseEntity = 0;

	SetSpectatorStartPosition();
	SetModes(m_mode->value, m_pip->value);
}

void CHudSpectator::CheckSettings(void)
{
	if (m_chatEnabled != (gHUD::m_SayText.m_pCVar_saytext->value != 0))
	{
		m_chatEnabled = (gHUD::m_SayText.m_pCVar_saytext->value != 0);

		if (gEngfuncs.IsSpectateOnly())
		{
			char chatcmd[32];
			Q_snprintf(chatcmd, sizeof(chatcmd) - 1, "ignoremsg %i", m_chatEnabled ? 0 : 1);
			gEngfuncs.pfnServerCmd(chatcmd);
		}
	}
}

void CHudSpectator::InitHUDData(void)
{
	m_lastPrimaryObject = m_lastSecondaryObject = 0;
	m_flNextObserverInput = 0.0f;
	m_lastHudMessage = 0;
	m_iSpectatorNumber = 0;
	iJumpSpectator = 0;
	g_iUser1 = g_iUser2 = 0;

	Reset();

	g_iUser2 = 0;

	gHUD::m_iFOV = gHUD::default_fov->value;
}

void CHudSpectator::DeathMessage(int victim)
{
	cl_entity_t* pl = gEngfuncs.GetEntityByIndex(victim);

	if (pl && pl->player)
	{
		if (pl->index == gEngfuncs.GetLocalPlayer()->index)
		{
			m_iObserverFlags = DRC_FLAG_DRAMATIC | DRC_FLAG_FINAL;
			V_ResetChaseCam();
		}
	}
}

// from view.cpp
extern Vector v_origin, v_angles, v_cl_angles, v_sim_org, v_lastAngles, v_lastFacing;

bool CHudSpectator::IsActivePlayer(cl_entity_t* ent)
{
	return (ent &&
		ent->player &&
		ent->curstate.solid != SOLID_NOT &&
		ent != gEngfuncs.GetLocalPlayer() &&
		g_PlayerInfoList[ent->index].name != NULL
		);
}

void CHudSpectator::SetModes(int iNewMainMode)
{
	if (iNewMainMode == -1)
		iNewMainMode = g_iUser1;

	if (iNewMainMode < OBS_FIRST || iNewMainMode > OBS_LAST)
	{
		gEngfuncs.Con_Printf("Invalid spectator mode.\n");
		return;
	}

	CheckSettings();

	m_IsInterpolating = false;
	m_ChaseEntity = 0;

	if (iNewMainMode != g_iUser1)
	{
		if (!gEngfuncs.IsSpectateOnly())
		{
			// UNDONE
			//if (iNewMainMode == OBS_CHASE_FREE && gViewPortInterface->GetClientDllInterface()->InIntermission() != 2)
				//m_autoDirector->value = 0;

			char cmdstring[32];
			Q_snprintf(cmdstring, charsmax(cmdstring), "specmode %i", iNewMainMode);
			gEngfuncs.pfnServerCmd(cmdstring);
			return;
		}

		if (!g_iUser2 && (iNewMainMode != OBS_ROAMING))
		{
			if (IsActivePlayer(gEngfuncs.GetEntityByIndex(m_lastPrimaryObject)))
			{
				g_iUser2 = m_lastPrimaryObject;
				g_iUser3 = m_lastSecondaryObject;
			}
			else
			{
				FindNextPlayer();
			}
		}

		switch (iNewMainMode)
		{
		case OBS_CHASE_LOCKED:
		{
			g_iUser1 = OBS_CHASE_LOCKED;
			break;
		}

		case OBS_CHASE_FREE:
		{
			g_iUser1 = OBS_CHASE_FREE;
			m_autoDirector->value = 0;
			break;
		}

		case OBS_ROAMING:
		{
			g_iUser1 = OBS_ROAMING;

			if (g_iUser2)
			{
				V_GetChasePos(g_iUser2, v_cl_angles, vJumpOrigin, vJumpAngles);
				gEngfuncs.SetViewAngles(vJumpAngles);
				iJumpSpectator = 1;
			}

			break;
		}

		case OBS_IN_EYE:
		{
			g_iUser1 = OBS_IN_EYE;
			break;
		}
		}

		if (g_iUser1 == OBS_ROAMING)
		{
			// UNDONE: Observer crosshair.
		}
	}
}

void CHudSpectator::ButtonUpdate(void)
{
	// Get old buttons from previous state.
	m_bitsLastButtons = m_bitsButtons;

	// Refresh button.
	m_bitsButtons = CL_ButtonBits();

	// Which buttsons chave changed
	unsigned buttonsChanged = (m_bitsLastButtons ^ m_bitsButtons);	// These buttons have changed this frame

	// Debounced button codes for pressed/released
	// The changed ones still down are "pressed"
	m_bitsButtonPressed = buttonsChanged & m_bitsButtons;
	// The ones not down are "released"
	m_bitsButtonReleased = buttonsChanged & (~m_bitsButtons);

	if (gHUD::m_bIntermission)
		return;

	if (!g_iUser1)
		return;

	if (gEngfuncs.pDemoAPI->IsPlayingback() && !gEngfuncs.IsSpectateOnly())
		return;

	double time = g_flClientTime;
	int newMainMode = g_iUser1;

	if (m_flNextObserverInput > time)
		return;

	if (m_bitsButtonPressed & IN_DUCK)
	{
		// UNDONE
		//if (!gViewPortInterface->IsSpectatorBarVisible())
			//gViewPortInterface->ShowSpectatorGUIBar();
	}

	if (gEngfuncs.IsSpectateOnly())
	{
		if (m_bitsButtonPressed & IN_JUMP)
		{
			newMainMode = g_iUser1 + 1;

			if (newMainMode > OBS_LAST)
				newMainMode = OBS_FIRST;
		}

		if (m_bitsButtonPressed & (IN_ATTACK | IN_ATTACK2))
		{
			FindNextPlayer((m_bitsButtonPressed & IN_ATTACK2) ? true : false);

			if (g_iUser1 == OBS_ROAMING)
			{
				gEngfuncs.SetViewAngles(vJumpAngles);
				iJumpSpectator = 1;
				gHUD::m_Radar.m_iPlayerLastPointedAt = g_iUser2;
			}

			m_autoDirector->value = 0.0f;
		}
	}

	m_flNextObserverInput = time + 0.2;
	SetModes(newMainMode);
}

void CHudSpectator::FindNextPlayer(bool bReverse)
{
	int iStart;
	cl_entity_t* pEnt = NULL;

	if (gEngfuncs.IsSpectateOnly())
	{
		char cmdstring[32];
		Q_snprintf(cmdstring, charsmax(cmdstring), "follownext %i", bReverse ? 1 : 0);
		gEngfuncs.pfnServerCmd(cmdstring);
		return;
	}

	if (g_iUser2)
		iStart = g_iUser2;
	else
		iStart = 1;

	int iCurrent = iStart;
	int iDir = bReverse ? -1 : 1;

	do
	{
		iCurrent += iDir;

		if (iCurrent > MAX_PLAYERS)
			iCurrent = 1;

		if (iCurrent < 1)
			iCurrent = MAX_PLAYERS;

		pEnt = gEngfuncs.GetEntityByIndex(iCurrent);

		if (!IsActivePlayer(pEnt))
			continue;

		g_iUser2 = iCurrent;
		break;

	} while (iCurrent != iStart);

	if (!g_iUser2)
	{
		gEngfuncs.Con_DPrintf("No observer targets.\n");

		VectorCopy(m_cameraOrigin, vJumpOrigin);
		VectorCopy(m_cameraAngles, vJumpAngles);
	}
	else
	{
		VectorCopy(pEnt->origin, vJumpOrigin);
		VectorCopy(pEnt->angles, vJumpAngles);
	}

	iJumpSpectator = 1;
}

void CHudSpectator::FindPlayer(const char* name)
{
	if (!gEngfuncs.IsSpectateOnly())
	{
		char cmdstring[32];
		Q_snprintf(cmdstring, charsmax(cmdstring), "follow %s", name);
		gEngfuncs.pfnServerCmd(cmdstring);
		return;
	}

	g_iUser2 = 0;
	//gViewPortInterface->GetAllPlayersInfo();	// UNDONE

	cl_entity_t* pEnt = NULL;

	for (int i = 1; i < MAX_PLAYERS; i++)
	{
		pEnt = gEngfuncs.GetEntityByIndex(i);

		if (!IsActivePlayer(pEnt))
			continue;

		if (!Q_stricmp(g_PlayerInfoList[pEnt->index].name, name))
		{
			g_iUser2 = i;
			break;
		}
	}

	if (!g_iUser2)
	{
		gEngfuncs.Con_DPrintf("No observer targets.\n");

		VectorCopy(m_cameraOrigin, vJumpOrigin);
		VectorCopy(m_cameraAngles, vJumpAngles);
	}
	else
	{
		VectorCopy(pEnt->origin, vJumpOrigin);
		VectorCopy(pEnt->angles, vJumpAngles);
	}

	iJumpSpectator = 1;
}

bool CHudSpectator::DirectorMessage(int iSize, void* pbuf)
{
	float f1, f2;
	char* string;
	vec3_t v1, v2;
	int i1, i2, i3;

	BEGIN_READ(pbuf, iSize);

	int cmd = READ_BYTE();

	switch (cmd)
	{
	case DRC_CMD_START:
	{
		MsgFunc_InitHUD(NULL, 0, NULL);
		MsgFunc_ResetHUD(NULL, 0, NULL);

		char tempString[128];
		client_textmessage_t tst;

		Q_strncpy(tempString, "#Spec_Duck", sizeof(tempString));

		tst.y = 0.7;
		tst.fadeout = 0.7;
		tst.pName = "Spec_Duck";
		tst.effect = 2;
		tst.r1 = 40;
		tst.g1 = 255;
		tst.b1 = 40;
		tst.a1 = 200;
		tst.r2 = 0;
		tst.g2 = 255;
		tst.b2 = 0;
		tst.a2 = 200;

		tst.x = -1.0;
		tst.fadein = 0.01;
		tst.fxtime = 0.07;
		tst.holdtime = 6.0;
		tst.pMessage = tempString;

		gHUD::m_Message.MessageAdd(&tst);
		break;
	}

	case DRC_CMD_EVENT:
	{
		m_lastPrimaryObject = READ_WORD();
		m_lastSecondaryObject = READ_WORD();
		m_iObserverFlags = READ_LONG();

		if (m_autoDirector->value)
		{
			if ((g_iUser2 != m_lastPrimaryObject) || (g_iUser3 != m_lastSecondaryObject))
				V_ResetChaseCam();

			g_iUser2 = m_lastPrimaryObject;
			g_iUser3 = m_lastSecondaryObject;
			m_IsInterpolating = false;
			m_ChaseEntity = 0;
		}

		return false;
	}

	case DRC_CMD_MODE:
	{
		if (m_autoDirector->value)
			SetModes(READ_BYTE(), -1);

		break;
	}

	case DRC_CMD_CAMERA:
	{
		v1[0] = READ_COORD();
		v1[1] = READ_COORD();
		v1[2] = READ_COORD();

		v2[0] = READ_COORD();
		v2[1] = READ_COORD();
		v2[2] = READ_COORD();
		f1 = READ_BYTE();
		i1 = READ_WORD();

		if (m_autoDirector->value)
		{
			SetModes(OBS_ROAMING);
			SetCameraView(v1, v2, f1);
			m_ChaseEntity = i1;
		}

		break;
	}

	case DRC_CMD_MESSAGE:
	{
		client_textmessage_t* msg = &m_HUDMessages[m_lastHudMessage];

		msg->effect = READ_BYTE();

		UnpackRGB((int&)msg->r1, (int&)msg->g1, (int&)msg->b1, READ_LONG());

		msg->r2 = msg->r1;
		msg->g2 = msg->g1;
		msg->b2 = msg->b1;
		msg->a2 = msg->a1 = 0xFF;

		msg->x = READ_FLOAT();
		msg->y = READ_FLOAT();

		msg->fadein = READ_FLOAT();
		msg->fadeout = READ_FLOAT();
		msg->holdtime = READ_FLOAT();
		msg->fxtime = READ_FLOAT();

		m_HUDMessageText[m_lastHudMessage] = READ_STRING();

		msg->pMessage = m_HUDMessageText[m_lastHudMessage].c_str();
		msg->pName = "HUD_MESSAGE";

		gHUD::m_Message.MessageAdd(msg);

		m_lastHudMessage++;
		m_lastHudMessage %= MAX_SPEC_HUD_MESSAGES;
		break;
	}

	case DRC_CMD_SOUND:
	{
		string = READ_STRING();
		f1 = READ_FLOAT();

		gEngfuncs.pEventAPI->EV_PlaySound(0, v_origin, CHAN_BODY, string, f1, ATTN_NORM, 0, PITCH_NORM);
		break;
	}

	case DRC_CMD_TIMESCALE:
	{
		f1 = READ_FLOAT();
		break;
	}

	case DRC_CMD_STATUS:
	{
		READ_LONG();
		m_iSpectatorNumber = READ_LONG();
		READ_WORD();

		// UNDONE
		//gViewPortInterface->UpdateSpectatorPanel();
		break;
	}

	case DRC_CMD_BANNER:
	{
		// UNDONE
		//gViewPortInterface->SetSpectatorBanner(READ_STRING());
		//gViewPortInterface->UpdateSpectatorPanel();
		break;
	}

	case DRC_CMD_STUFFTEXT:
	{
		gEngfuncs.pfnClientCmd(READ_STRING());
		break;
	}

	case DRC_CMD_CAMPATH:
	{
		v1[0] = READ_COORD();
		v1[1] = READ_COORD();
		v1[2] = READ_COORD();

		v2[0] = READ_COORD();
		v2[1] = READ_COORD();
		v2[2] = READ_COORD();
		f1 = READ_BYTE();
		i1 = READ_BYTE();

		if (m_autoDirector->value)
		{
			SetModes(OBS_ROAMING);
			SetCameraView(v1, v2, f1);
		}

		break;
	}

	case DRC_CMD_WAYPOINTS:
	{
		i1 = READ_BYTE();

		m_NumWayPoints = 0;
		m_WayPoint = 0;

		for (i2 = 0; i2 < i1; i2++)
		{
			f1 = gHUD::m_flTime + (float)(READ_SHORT()) / 100.0f;

			v1[0] = READ_COORD();
			v1[1] = READ_COORD();
			v1[2] = READ_COORD();

			v2[0] = READ_COORD();
			v2[1] = READ_COORD();
			v2[2] = READ_COORD();
			f2 = READ_BYTE();
			i3 = READ_BYTE();

			AddWaypoint(f1, v1, v2, f2, i3);
		}

		if (!m_autoDirector->value)
		{
			m_NumWayPoints = 0;
			break;
		}

		SetModes(OBS_ROAMING);

		m_IsInterpolating = true;

		if (m_NumWayPoints > 2)
		{
			SetWayInterpolation(NULL, &m_CamPath[0], &m_CamPath[1], &m_CamPath[2]);
		}
		else
		{
			SetWayInterpolation(NULL, &m_CamPath[0], &m_CamPath[1], NULL);
		}

		break;
	}

	default: gEngfuncs.Con_DPrintf("CHudSpectator::DirectorMessage: unknown command %i.\n", cmd);
	}

	return true;
}

void CHudSpectator::SetSpectatorStartPosition(void)
{
	if (UTIL_FindEntityInMap("trigger_camera", m_cameraOrigin, m_cameraAngles))
	{
		iJumpSpectator = 1;
	}
	else if (UTIL_FindEntityInMap("info_player_start", m_cameraOrigin, m_cameraAngles))
	{
		iJumpSpectator = 1;
	}
	else if (UTIL_FindEntityInMap("info_player_deathmatch", m_cameraOrigin, m_cameraAngles))
	{
		iJumpSpectator = 1;
	}
	else if (UTIL_FindEntityInMap("info_player_coop", m_cameraOrigin, m_cameraAngles))
	{
		iJumpSpectator = 1;
	}
	else
	{
		VectorCopy(vec3_origin, m_cameraOrigin);
		VectorCopy(vec3_origin, m_cameraAngles);
	}

	VectorCopy(m_cameraOrigin, vJumpOrigin);
	VectorCopy(m_cameraAngles, vJumpAngles);

	iJumpSpectator = 1;
}

void SpectatorMode(void)
{
	if (gEngfuncs.Cmd_Argc() != 2)
	{
		gEngfuncs.Con_Printf("usage:  spec_mode <Main Mode>\n");
		return;
	}

	CHudSpectator::SetModes(Q_atoi(gEngfuncs.Cmd_Argv(1)));
}

void SpectatorSpray(void)
{
	Vector forward;
	char string[128];

	if (!gEngfuncs.IsSpectateOnly())
		return;

	AngleVectors(v_angles, forward, NULL, NULL);
	VectorScale(forward, 128, forward);
	VectorAdd(forward, v_origin, forward);

	pmtrace_t* trace = gEngfuncs.PM_TraceLine(v_origin, forward, PM_TRACELINE_PHYSENTSONLY, 2, -1);

	if (trace->fraction != 1.0)
	{
		Q_snprintf(string, charsmax(string), "drc_spray %.2f %.2f %.2f %i", trace->endpos[0], trace->endpos[1], trace->endpos[2], trace->ent);
		gEngfuncs.pfnServerCmd(string);
	}
}

void SpectatorToggleAutoDirector(void)
{
	if (CHudSpectator::m_autoDirector->value)
		gEngfuncs.Cvar_SetValue("spec_autodirector_internal", 0);
	else
		gEngfuncs.Cvar_SetValue("spec_autodirector_internal", 1);
}

int CHudSpectator::Init(void)
{
	gHUD::AddHudElem(this);

	m_bitsFlags |= HUD_ACTIVE;
	m_flNextObserverInput = 0.0f;
	m_zoomDelta = 0.0f;
	m_moveDelta = 0.0f;
	m_FOV = 90.0f;
	m_chatEnabled = (gHUD::m_SayText.m_pCVar_saytext->value != 0);
	iJumpSpectator = 0;
	m_lastAutoDirector = -1.0f;

	Q_memset(&m_OverviewData, 0, sizeof(m_OverviewData));
	Q_memset(&m_OverviewEntities, 0, sizeof(m_OverviewEntities));
	m_lastPrimaryObject = m_lastSecondaryObject = 0;

	gEngfuncs.pfnAddCommand("spec_mode", SpectatorMode);
	gEngfuncs.pfnAddCommand("spec_toggleinset", SpectatorToggleInset);
	gEngfuncs.pfnAddCommand("spec_decal", SpectatorSpray);
	gEngfuncs.pfnAddCommand("spec_help", SpectatorHelp);
	gEngfuncs.pfnAddCommand("spec_menu", SpectatorMenu);
	gEngfuncs.pfnAddCommand("togglescores", ToggleScores);
	gEngfuncs.pfnAddCommand("spec_drawnames", SpectatorToggleDrawNames);
	gEngfuncs.pfnAddCommand("spec_drawcone", SpectatorToggleDrawCone);
	gEngfuncs.pfnAddCommand("spec_drawstatus", SpectatorToggleDrawStatus);
	gEngfuncs.pfnAddCommand("spec_autodirector", SpectatorToggleAutoDirector);

	m_drawnames = gEngfuncs.pfnRegisterVariable("spec_drawnames_internal", "1", 0);
	m_drawcone = gEngfuncs.pfnRegisterVariable("spec_drawcone_internal", "1", 0);
	m_drawstatus = gEngfuncs.pfnRegisterVariable("spec_drawstatus_internal", "1", 0);
	m_autoDirector = gEngfuncs.pfnRegisterVariable("spec_autodirector_internal", "1", 0);
	m_mode = gEngfuncs.pfnRegisterVariable("spec_mode_internal", "1", 0);
	m_pip = gEngfuncs.pfnRegisterVariable("spec_pip", "1", 0);

	if (!m_drawnames || !m_drawcone || !m_drawstatus || !m_autoDirector || !m_pip)
	{
		gEngfuncs.Con_Printf("ERROR! Couldn't register all spectator variables.\n");
		return 0;
	}

	return 1;
}

int CHudSpectator::VidInit(void)
{
	m_hsprPlayer = gEngfuncs.pfnSPR_Load("sprites/iplayer.spr");
	m_hsprPlayerBlue = gEngfuncs.pfnSPR_Load("sprites/iplayerblue.spr");
	m_hsprPlayerRed = gEngfuncs.pfnSPR_Load("sprites/iplayerred.spr");
	m_hsprPlayerDead = gEngfuncs.pfnSPR_Load("sprites/iplayerdead.spr");
	m_hsprPlayerVIP = gEngfuncs.pfnSPR_Load("sprites/iplayervip.spr");
	m_hsprPlayerC4 = gEngfuncs.pfnSPR_Load("sprites/iplayerc4.spr");
	m_hsprUnkownMap = gEngfuncs.pfnSPR_Load("sprites/tile.spr");
	m_hsprBomb = gEngfuncs.pfnSPR_Load("sprites/ic4.spr");
	m_hsprBackpack = gEngfuncs.pfnSPR_Load("sprites/ibackpack.spr");
	m_hsprBeam = gEngfuncs.pfnSPR_Load("sprites/laserbeam.spr");
	m_hsprCamera = gEngfuncs.pfnSPR_Load("sprites/camera.spr");
	m_hsprHostage = gEngfuncs.pfnSPR_Load("sprites/ihostage.spr");

	return 1;
}

void CHudSpectator::AddWaypoint(float time, Vector pos, Vector angle, float fov, int flags)
{
	if (!flags == 0 && time == 0.0f)
	{
		SetCameraView(pos, angle, fov);
		return;
	}

	if (m_NumWayPoints >= MAX_CAM_WAYPOINTS)
	{
		gEngfuncs.Con_Printf("Too many camera waypoints!\n");
		return;
	}

	VectorCopy(angle, m_CamPath[m_NumWayPoints].angle);
	VectorCopy(pos, m_CamPath[m_NumWayPoints].position);
	m_CamPath[m_NumWayPoints].flags = flags;
	m_CamPath[m_NumWayPoints].fov = fov;
	m_CamPath[m_NumWayPoints].time = time;

	gEngfuncs.Con_DPrintf("Added waypoint %i\n", m_NumWayPoints);
	m_NumWayPoints++;
}

void CHudSpectator::SetCameraView(const Vector& pos, const Vector& angle, float fov)
{
	m_iFOV = fov;
	VectorCopy(pos, vJumpOrigin);
	VectorCopy(angle, vJumpAngles);
	gEngfuncs.SetViewAngles(vJumpAngles);
	iJumpSpectator = 1;
}

bool CHudSpectator::GetDirectorCamera(Vector& position, Vector& angle)
{
	float now = gHUD::m_flTime;
	float fov = 90.0f;

	if (m_ChaseEntity)
	{
		cl_entity_t* ent = gEngfuncs.GetEntityByIndex(m_ChaseEntity);

		if (ent)
		{
			Vector vt = ent->curstate.origin;

			if (m_ChaseEntity <= gEngfuncs.GetMaxClients())
			{
				if (ent->curstate.solid == SOLID_NOT)
				{
					vt[2] += -8;
				}
				else if (ent->curstate.usehull == 1)
				{
					vt[2] += 12;
				}
				else
				{
					vt[2] += 17;
				}
			}

			vt = vt - position;
			VectorAngles(vt, angle);
			angle[0] = -angle[0];
			return true;
		}
		else
		{
			return false;
		}
	}

	if (!m_IsInterpolating)
		return false;

	if (m_WayPoint < 0 || m_WayPoint >= (m_NumWayPoints - 1))
		return false;

	cameraWayPoint_t* wp1 = &m_CamPath[m_WayPoint];
	cameraWayPoint_t* wp2 = &m_CamPath[m_WayPoint + 1];

	if (now < wp1->time)
		return false;

	while (now > wp2->time)
	{
		m_WayPoint++;

		if (m_WayPoint >= (m_NumWayPoints - 1))
		{
			m_IsInterpolating = false;
			return false;
		}

		wp1 = wp2;
		wp2 = &m_CamPath[m_WayPoint + 1];

		if (m_WayPoint > 0)
		{
			if (m_WayPoint < (m_NumWayPoints - 1))
			{
				SetWayInterpolation(&m_CamPath[m_WayPoint - 1], wp1, wp2, &m_CamPath[m_WayPoint + 2]);
			}
			else
			{
				SetWayInterpolation(&m_CamPath[m_WayPoint - 1], wp1, wp2, NULL);
			}
		}
		else if (m_WayPoint < (m_NumWayPoints - 1))
		{
			SetWayInterpolation(NULL, wp1, wp2, &m_CamPath[m_WayPoint + 2]);
		}
		else
		{
			SetWayInterpolation(NULL, wp1, wp2, NULL);
		}
	}

	if (wp2->time <= wp1->time)
		return false;

	float fraction = (now - wp1->time) / (wp2->time - wp1->time);

	if (fraction < 0.0f)
		fraction = 0.0f;
	else if (fraction > 1.0f)
		fraction = 1.0f;

	m_WayInterpolation.Interpolate(fraction, position, angle, &fov);
	gEngfuncs.Con_Printf("Interpolate time: %.2f, fraction %.2f, point : %.2f,%.2f,%.2f\n", now, fraction, position[0], position[1], position[2]);

	SetCameraView(position, angle, fov);
	return true;
}

void CHudSpectator::SetWayInterpolation(cameraWayPoint_t* prev, cameraWayPoint_t* start, cameraWayPoint_t* end, cameraWayPoint_t* next)
{
	m_WayInterpolation.SetViewAngles(start->angle, end->angle);
	m_WayInterpolation.SetFOVs(start->fov, end->fov);
	m_WayInterpolation.SetSmoothing((start->flags & DRC_FLAG_SLOWSTART) != 0, (start->flags & DRC_FLAG_SLOWEND) != 0);

	if (prev && next)
	{
		m_WayInterpolation.SetWaypoints(&prev->position, start->position, end->position, &next->position);
	}
	else if (prev)
	{
		m_WayInterpolation.SetWaypoints(&prev->position, start->position, end->position, NULL);
	}
	else if (next)
	{
		m_WayInterpolation.SetWaypoints(NULL, start->position, end->position, &next->position);
	}
	else
	{
		m_WayInterpolation.SetWaypoints(NULL, start->position, end->position, NULL);
	}
}
