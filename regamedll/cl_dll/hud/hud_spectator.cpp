/*

Created Date: 10 Mar 2020

*/

#include "precompiled.h"

// came from PM_Shared.cpp
extern int iJumpSpectator;
extern float vJumpOrigin[3];
extern float vJumpAngles[3];

void CHudSpectator::Reset(void)
{
	if (Q_strcmp(m_OverviewData.map, gEngfuncs.pfnGetLevelName()))
	{
		ParseOverviewFile();
		LoadMapSprites();
	}

	Q_memset(&m_OverviewEntities, 0, sizeof(m_OverviewEntities));

	m_FOV = 90.0f;

	m_IsInterpolating = false;

	m_ChaseEntity = 0;

	SetSpectatorStartPosition();
	SetModes(m_mode->value, m_pip->value);
}

int CHudSpectator::ToggleInset(bool allowOff)
{
	int newInsetMode = (int)m_pip->value + 1;

	if (gLocalPlayer.pev->iuser1 < OBS_MAP_FREE)
	{
		if (newInsetMode > INSET_MAP_CHASE)
		{
			if (allowOff)
				newInsetMode = INSET_OFF;
			else
				newInsetMode = INSET_MAP_FREE;
		}

		if (newInsetMode == INSET_CHASE_FREE)
			newInsetMode = INSET_MAP_FREE;
	}
	else
	{
		if (newInsetMode > INSET_IN_EYE)
		{
			if (allowOff)
				newInsetMode = INSET_OFF;
			else
				newInsetMode = INSET_CHASE_FREE;
		}
	}

	return newInsetMode;
}

void CHudSpectator::CheckSettings(void)
{
	m_pip->value = (int)m_pip->value;

	if ((gLocalPlayer.pev->iuser1 < OBS_MAP_FREE) && (m_pip->value == INSET_CHASE_FREE || m_pip->value == INSET_IN_EYE))
	{
		m_pip->value = INSET_MAP_FREE;
	}

	if ((gLocalPlayer.pev->iuser1 >= OBS_MAP_FREE) && (m_pip->value >= INSET_MAP_FREE))
	{
		m_pip->value = INSET_CHASE_FREE;
	}

	if (gHUD::m_bIntermission)
		m_pip->value = INSET_OFF;

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

	if (((gLocalPlayer.m_iTeam == TEAM_TERRORIST) || (gLocalPlayer.m_iTeam == TEAM_CT)) && (gLocalPlayer.pev->iuser1 == OBS_IN_EYE))
	{
		if (m_pip->value != INSET_OFF)
		{
			gHUD::m_VGUI2Print.VGUI2HudPrint("#Spec_No_PIP", -1, ScreenHeight * 0.35, 1.0, 0.705, 0.118);
			m_pip->value = INSET_OFF;
		}
	}

	// UNDONE
	/*if (gEngfuncs.GetLocalPlayer()->index == gLocalPlayer.pev->iuser2)
	{
		gViewPortInterface->SpectatorGUIEnableInsetView(false);
	}
	else
	{
		gViewPortInterface->SpectatorGUIEnableInsetView(m_pip->value != INSET_OFF);
	}*/
}

void CHudSpectator::InitHUDData(void)
{
	m_lastPrimaryObject = m_lastSecondaryObject = 0;
	m_flNextObserverInput = 0.0f;
	m_lastHudMessage = 0;
	m_iSpectatorNumber = 0;
	iJumpSpectator = 0;
	gLocalPlayer.pev->iuser1 = gLocalPlayer.pev->iuser2 = 0;

	memset(&m_OverviewData, 0, sizeof(m_OverviewData));
	memset(&m_OverviewEntities, 0, sizeof(m_OverviewEntities));

	Reset();

	gLocalPlayer.pev->iuser2 = 0;

	gHUD::m_iFOV = gHUD::default_fov->value;
}

bool CHudSpectator::AddOverviewEntityToList(hSprite sprite, cl_entity_t* ent, double killTime)
{
	for (int i = 0; i < MAX_OVERVIEW_ENTITIES; i++)
	{
		if (m_OverviewEntities[i].entity == NULL)
		{
			m_OverviewEntities[i].entity = ent;
			m_OverviewEntities[i].hSprite = sprite;
			m_OverviewEntities[i].killTime = killTime;
			return true;
		}
	}

	return false;
}

void CHudSpectator::DeathMessage(int victim)
{
	cl_entity_t* pl = gEngfuncs.GetEntityByIndex(victim);

	if (pl && pl->player)
	{
		if (AddOverviewEntityToList(m_hsprPlayerDead, pl, gEngfuncs.GetClientTime() + 4.0f))
		{
			if (pl->index == gEngfuncs.GetLocalPlayer()->index)
			{
				m_iObserverFlags = DRC_FLAG_DRAMATIC | DRC_FLAG_FINAL;
				V_ResetChaseCam();
			}
		}
	}
}

bool CHudSpectator::AddOverviewEntity(int type, cl_entity_s* ent, const char* modelname)
{
	hSprite hSprite = 0;
	double duration = -1.0f;

	if (!ent)
		return false;

	if (type == ET_PLAYER)
	{
		if (ent->curstate.solid != SOLID_NOT)
		{
			switch (g_PlayerExtraInfo[ent->index].m_iTeam)
			{
			case TEAM_TERRORIST: hSprite = m_hsprPlayerRed; break;
			case TEAM_CT: hSprite = m_hsprPlayerBlue; break;
			default: hSprite = m_hsprPlayer; break;
			}

			if (g_PlayerExtraInfo[ent->index].m_bIsCommander)
				hSprite = m_hsprPlayerVIP;
			else if (g_PlayerExtraInfo[ent->index].m_bIsGodfather)
				hSprite = m_hsprPlayerC4;
		}
		else
			return false;
	}
	else if (type == ET_NORMAL)
	{
		if (!strcmp(modelname, "models/w_c4.mdl"))
			hSprite = m_hsprBomb;
		else if (!strcmp(modelname, "models/w_backpack.mdl"))
			hSprite = m_hsprBackpack;
		else if (!strcmp(modelname, "models/hostage") || !strcmp(modelname, "models/scientist"))
			hSprite = m_hsprHostage;
	}
	else
		return false;

	return AddOverviewEntityToList(hSprite, ent, gEngfuncs.GetClientTime() + duration);
}

void CHudSpectator::CheckOverviewEntities(void)
{
	double time = gEngfuncs.GetClientTime();

	for (int i = 0; i < MAX_OVERVIEW_ENTITIES; i++)
	{
		if (m_OverviewEntities[i].killTime < time)
			Q_memset(&m_OverviewEntities[i], 0, sizeof(overviewEntity_t));
	}
}

void CHudSpectator::DrawOverview(void)
{
	if (!gLocalPlayer.pev->iuser1)
		return;

	if (m_iDrawCycle == 0 && ((gLocalPlayer.pev->iuser1 != OBS_MAP_FREE) && (gLocalPlayer.pev->iuser1 != OBS_MAP_CHASE)))
		return;

	if (m_iDrawCycle == 1 && m_pip->value < INSET_MAP_FREE)
		return;

	// UNDONE
	/*if (gViewPortInterface->GetClientDllInterface()->InIntermission())
	{
		m_pip->value = 0;
		return;
	}*/

	DrawOverviewLayer();
	DrawOverviewEntities();
	CheckOverviewEntities();
}

// from view.cpp
extern Vector v_origin, v_angles, v_cl_angles, v_sim_org, v_lastAngles, v_lastFacing;

void CHudSpectator::DrawOverviewEntities(void)
{
	int i, ir, ig, ib;
	model_s* hSpriteModel;
	Vector origin, angles, point, forward, right, left, up, world, screen, offset;
	float x, y, z, r, g, b, sizeScale = 4.0f;
	cl_entity_t* ent;
	float rmatrix[3][4];

	float zScale = (90.0f - v_angles[0]) / 90.0f;

	z = m_OverviewData.layersHeights[0] * zScale;

	UnpackRGB(ir, ig, ib, RGB_YELLOWISH);
	r = (float)ir / 255.0f;
	g = (float)ig / 255.0f;
	b = (float)ib / 255.0f;

	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	for (i = 0; i < MAX_PLAYERS; i++)
		m_vPlayerPos[i][2] = -1;

	for (i = 0; i < MAX_OVERVIEW_ENTITIES; i++)
	{
		if (!m_OverviewEntities[i].hSprite)
			continue;

		if (m_OverviewEntities[i].entity == gEngfuncs.GetLocalPlayer() && m_OverviewEntities[i].hSprite == m_hsprPlayerDead)
			continue;

		hSpriteModel = (struct model_s*)gEngfuncs.GetSpritePointer(m_OverviewEntities[i].hSprite);
		ent = m_OverviewEntities[i].entity;

		gEngfuncs.pTriAPI->SpriteTexture(hSpriteModel, 0);
		gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);

		AngleVectors(ent->angles, right, up, NULL);
		VectorCopy(ent->origin, origin);

		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, 1.0);

		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		VectorMA(origin, 16.0f * sizeScale, up, point);
		VectorMA(point, 16.0f * sizeScale, right, point);
		point[2] *= zScale;
		gEngfuncs.pTriAPI->Vertex3fv(point);

		gEngfuncs.pTriAPI->TexCoord2f(0, 0);

		VectorMA(origin, 16.0f * sizeScale, up, point);
		VectorMA(point, -16.0f * sizeScale, right, point);
		point[2] *= zScale;
		gEngfuncs.pTriAPI->Vertex3fv(point);

		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		VectorMA(origin, -16.0f * sizeScale, up, point);
		VectorMA(point, -16.0f * sizeScale, right, point);
		point[2] *= zScale;
		gEngfuncs.pTriAPI->Vertex3fv(point);

		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		VectorMA(origin, -16.0f * sizeScale, up, point);
		VectorMA(point, 16.0f * sizeScale, right, point);
		point[2] *= zScale;
		gEngfuncs.pTriAPI->Vertex3fv(point);

		gEngfuncs.pTriAPI->End();

		if (!ent->player)
			continue;

		origin[2] *= zScale;

		gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);

		hSpriteModel = (struct model_s*)gEngfuncs.GetSpritePointer(m_hsprBeam);
		gEngfuncs.pTriAPI->SpriteTexture(hSpriteModel, 0);

		gEngfuncs.pTriAPI->Color4f(r, g, b, 0.3);

		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(origin[0] + 4, origin[1] + 4, origin[2] - zScale);
		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(origin[0] - 4, origin[1] - 4, origin[2] - zScale);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(origin[0] - 4, origin[1] - 4, z);
		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(origin[0] + 4, origin[1] + 4, z);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(origin[0] - 4, origin[1] + 4, origin[2] - zScale);
		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(origin[0] + 4, origin[1] - 4, origin[2] - zScale);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(origin[0] + 4, origin[1] - 4, z);
		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(origin[0] - 4, origin[1] + 4, z);
		gEngfuncs.pTriAPI->End();

		if (gEngfuncs.pTriAPI->WorldToScreen(origin, screen))
			continue;

		screen[0] = XPROJECT(screen[0]);
		screen[1] = YPROJECT(screen[1]);
		screen[2] = 0.0f;

		origin[0] += 32.0f;
		origin[1] += 32.0f;

		gEngfuncs.pTriAPI->WorldToScreen(origin, offset);

		offset[0] = XPROJECT(offset[0]);
		offset[1] = YPROJECT(offset[1]);
		offset[2] = 0.0f;

		VectorSubtract(offset, screen, offset);

		int playerNum = ent->index - 1;

		m_vPlayerPos[playerNum][0] = screen[0];
		m_vPlayerPos[playerNum][1] = screen[1] + Length(offset);
		m_vPlayerPos[playerNum][2] = 1;
	}

	if (!m_pip->value || !m_drawcone->value)
		return;

	if (m_pip->value == INSET_IN_EYE || gLocalPlayer.pev->iuser1 == OBS_IN_EYE)
	{
		V_GetInEyePos(gLocalPlayer.pev->iuser2, origin, angles);
	}
	else if (m_pip->value == INSET_CHASE_FREE || gLocalPlayer.pev->iuser1 == OBS_CHASE_FREE)
	{
		V_GetChasePos(gLocalPlayer.pev->iuser2, v_cl_angles, origin, angles);
	}
	else if (gLocalPlayer.pev->iuser1 == OBS_ROAMING)
	{
		VectorCopy(v_sim_org, origin);
		VectorCopy(v_cl_angles, angles);
	}
	else
		V_GetChasePos(gLocalPlayer.pev->iuser2, NULL, origin, angles);

	x = origin[0];
	y = origin[1];
	z = origin[2];

	angles[0] = 0;

	hSpriteModel = (struct model_s*)gEngfuncs.GetSpritePointer(m_hsprCamera);
	gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
	gEngfuncs.pTriAPI->SpriteTexture(hSpriteModel, 0);

	gEngfuncs.pTriAPI->Color4f(r, g, b, 1.0);

	AngleVectors(angles, forward, NULL, NULL);
	VectorScale(forward, 512.0f, forward);

	offset[0] = 0.0f;
	offset[1] = 45.0f;
	offset[2] = 0.0f;

	AngleMatrix(offset, rmatrix);
	VectorTransform(forward, rmatrix, right);

	offset[1] = -45.0f;

	AngleMatrix(offset, rmatrix);
	VectorTransform(forward, rmatrix, left);

	gEngfuncs.pTriAPI->Begin(TRI_TRIANGLES);
	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(x + right[0], y + right[1], (z + right[2]) * zScale);

	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(x, y, z * zScale);

	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(x + left[0], y + left[1], (z + left[2]) * zScale);
	gEngfuncs.pTriAPI->End();
}

void CHudSpectator::DrawOverviewLayer(void)
{
	float screenaspect, xs, ys, xStep, yStep, x, y, z;
	int ix, iy, i, xTiles, yTiles, frame;

	qboolean hasMapImage = m_MapSprite ? TRUE : FALSE;
	model_t* dummySprite = (struct model_s*)gEngfuncs.GetSpritePointer(m_hsprUnkownMap);

	if (hasMapImage)
	{
		i = m_MapSprite->numframes / (4 * 3);
		i = Q_sqrt((float)i);
		xTiles = i * 4;
		yTiles = i * 3;
	}
	else
	{
		xTiles = 8;
		yTiles = 6;
	}

	screenaspect = 4.0f / 3.0f;

	xs = m_OverviewData.origin[0];
	ys = m_OverviewData.origin[1];
	z = (90.0f - v_angles[0]) / 90.0f;
	z *= m_OverviewData.layersHeights[0];

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, 1.0);

	frame = 0;

	if (m_OverviewData.rotated)
	{
		xStep = (2 * 4096.0f / m_OverviewData.zoom) / xTiles;
		yStep = -(2 * 4096.0f / (m_OverviewData.zoom * screenaspect)) / yTiles;

		y = ys + (4096.0f / (m_OverviewData.zoom * screenaspect));

		for (iy = 0; iy < yTiles; iy++)
		{
			x = xs - (4096.0f / (m_OverviewData.zoom));

			for (ix = 0; ix < xTiles; ix++)
			{
				if (hasMapImage)
					gEngfuncs.pTriAPI->SpriteTexture(m_MapSprite, frame);
				else
					gEngfuncs.pTriAPI->SpriteTexture(dummySprite, 0);

				gEngfuncs.pTriAPI->Begin(TRI_QUADS);
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				gEngfuncs.pTriAPI->Vertex3f(x, y, z);

				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				gEngfuncs.pTriAPI->Vertex3f(x + xStep, y, z);

				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				gEngfuncs.pTriAPI->Vertex3f(x + xStep, y + yStep, z);

				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				gEngfuncs.pTriAPI->Vertex3f(x, y + yStep, z);
				gEngfuncs.pTriAPI->End();

				frame++;
				x += xStep;
			}

			y += yStep;
		}
	}
	else
	{
		xStep = -(2 * 4096.0f / m_OverviewData.zoom) / xTiles;
		yStep = -(2 * 4096.0f / (m_OverviewData.zoom * screenaspect)) / yTiles;

		x = xs + (4096.0f / (m_OverviewData.zoom * screenaspect));

		for (ix = 0; ix < yTiles; ix++)
		{
			y = ys + (4096.0f / (m_OverviewData.zoom));

			for (iy = 0; iy < xTiles; iy++)
			{
				if (hasMapImage)
					gEngfuncs.pTriAPI->SpriteTexture(m_MapSprite, frame);
				else
					gEngfuncs.pTriAPI->SpriteTexture(dummySprite, 0);

				gEngfuncs.pTriAPI->Begin(TRI_QUADS);
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				gEngfuncs.pTriAPI->Vertex3f(x, y, z);

				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				gEngfuncs.pTriAPI->Vertex3f(x + xStep, y, z);

				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				gEngfuncs.pTriAPI->Vertex3f(x + xStep, y + yStep, z);

				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				gEngfuncs.pTriAPI->Vertex3f(x, y + yStep, z);
				gEngfuncs.pTriAPI->End();

				frame++;
				y += yStep;
			}

			x += xStep;
		}
	}
}

void CHudSpectator::LoadMapSprites()
{
	// right now only support for one map layer
	if (m_OverviewData.layers > 0)
	{
		m_MapSprite = gEngfuncs.LoadMapSprite(m_OverviewData.layersImages[0]);
	}
	else
		m_MapSprite = nullptr; // the standard "unknown map" sprite will be used instead
}

bool CHudSpectator::ParseOverviewFile()
{
	char filename[255] = { 0 };
	char levelname[255] = { 0 };
	char token[1024] = { 0 };
	float height;

	char* pfile = nullptr;

	Q_memset(&m_OverviewData, 0, sizeof(m_OverviewData));

	// fill in standrd values
	m_OverviewData.insetWindowX = 4;	// upper left corner
	m_OverviewData.insetWindowY = 4;
	m_OverviewData.insetWindowHeight = 180;
	m_OverviewData.insetWindowWidth = 240;
	m_OverviewData.origin[0] = 0.0f;
	m_OverviewData.origin[1] = 0.0f;
	m_OverviewData.origin[2] = 0.0f;
	m_OverviewData.zoom = 1.0f;
	m_OverviewData.layers = 0;
	m_OverviewData.layersHeights[0] = 0.0f;
	Q_strncpy(m_OverviewData.map, gEngfuncs.pfnGetLevelName(), sizeof(m_OverviewData.map));

	if (Q_strlen(m_OverviewData.map) == 0)
		return false; // not active yet

	Q_strncpy(levelname, m_OverviewData.map + 5, sizeof(levelname));
	levelname[Q_strlen(levelname) - 4] = 0;

	Q_snprintf(filename, charsmax(filename), "overviews/%s.txt", levelname);

	pfile = (char*)gEngfuncs.COM_LoadFile(filename, 5, NULL);

	if (!pfile)
	{
		gEngfuncs.Con_Printf("Couldn't open file %s. Using default values for overiew mode.\n", filename);
		return false;
	}


	while (true)
	{
		pfile = gEngfuncs.COM_ParseFile(pfile, token);

		if (!pfile)
			break;

		if (!Q_strcmp(token, "global"))
		{
			// parse the global data
			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			if (Q_strcmp(token, "{"))
			{
				gEngfuncs.Con_Printf("Error parsing overview file %s. (expected { )\n", filename);
				return false;
			}

			pfile = gEngfuncs.COM_ParseFile(pfile, token);

			while (Q_strcmp(token, "}"))
			{
				if (!Q_strcmp(token, "zoom"))
				{
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					m_OverviewData.zoom = Q_atof(token);
				}
				else if (!Q_strcmp(token, "origin"))
				{
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					m_OverviewData.origin[0] = Q_atof(token);
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					m_OverviewData.origin[1] = Q_atof(token);
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					m_OverviewData.origin[2] = Q_atof(token);
				}
				else if (!Q_strcmp(token, "rotated"))
				{
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					m_OverviewData.rotated = Q_atoi(token);
				}
				else if (!Q_strcmp(token, "inset"))
				{
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					m_OverviewData.insetWindowX = Q_atof(token);
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					m_OverviewData.insetWindowY = Q_atof(token);
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					m_OverviewData.insetWindowWidth = Q_atof(token);
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					m_OverviewData.insetWindowHeight = Q_atof(token);

				}
				else
				{
					gEngfuncs.Con_Printf("Error parsing overview file %s. (%s unknown)\n", filename, token);
					return false;
				}

				pfile = gEngfuncs.COM_ParseFile(pfile, token); // parse next token

			}
		}
		else if (!Q_strcmp(token, "layer"))
		{
			// parse a layer data

			if (m_OverviewData.layers >= OVERVIEW_MAX_LAYERS)
			{
				gEngfuncs.Con_Printf("Error parsing overview file %s. ( too many layers )\n", filename);
				return false;
			}

			pfile = gEngfuncs.COM_ParseFile(pfile, token);


			if (Q_strcmp(token, "{"))
			{
				gEngfuncs.Con_Printf("Error parsing overview file %s. (expected { )\n", filename);
				return false;
			}

			pfile = gEngfuncs.COM_ParseFile(pfile, token);

			while (Q_strcmp(token, "}"))
			{
				if (!Q_strcmp(token, "image"))
				{
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					Q_strncpy(m_OverviewData.layersImages[m_OverviewData.layers], token, 255);
				}
				else if (!Q_strcmp(token, "height"))
				{
					pfile = gEngfuncs.COM_ParseFile(pfile, token);
					height = Q_atof(token);
					m_OverviewData.layersHeights[m_OverviewData.layers] = height;
				}
				else
				{
					gEngfuncs.Con_Printf("Error parsing overview file %s. (%s unknown)\n", filename, token);
					return false;
				}

				pfile = gEngfuncs.COM_ParseFile(pfile, token); // parse next token
			}

			m_OverviewData.layers++;

		}
	}

	gEngfuncs.COM_FreeFile(pfile);

	m_mapZoom = m_OverviewData.zoom;
	m_mapOrigin = m_OverviewData.origin;

	return true;

}

bool CHudSpectator::IsActivePlayer(cl_entity_t* ent)
{
	return (ent &&
		ent->player &&
		ent->curstate.solid != SOLID_NOT &&
		ent != gEngfuncs.GetLocalPlayer() &&
		g_PlayerInfoList[ent->index].name != NULL
		);
}

void CHudSpectator::SetModes(int iNewMainMode, int iNewInsetMode)
{
	if (iNewMainMode == -1)
		iNewMainMode = gLocalPlayer.pev->iuser1;

	if (iNewInsetMode == -1)
		iNewInsetMode = m_pip->value;

	m_pip->value = iNewInsetMode;

	// UNDONE
	//if (gViewPortInterface->GetClientDllInterface()->InIntermission())
		//m_pip->value = INSET_OFF;

	if (iNewMainMode < OBS_CHASE_LOCKED || iNewMainMode > OBS_MAP_CHASE)
	{
		gEngfuncs.Con_Printf("Invalid spectator mode.\n");
		return;
	}

	CheckSettings();

	m_IsInterpolating = false;
	m_ChaseEntity = 0;

	if (iNewMainMode != gLocalPlayer.pev->iuser1)
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

		if (!gLocalPlayer.pev->iuser2 && (iNewMainMode != OBS_ROAMING))
		{
			if (IsActivePlayer(gEngfuncs.GetEntityByIndex(m_lastPrimaryObject)))
			{
				gLocalPlayer.pev->iuser2 = m_lastPrimaryObject;
				gLocalPlayer.pev->iuser3 = m_lastSecondaryObject;
			}
			else
			{
				FindNextPlayer(false);
			}
		}

		switch (iNewMainMode)
		{
		case OBS_CHASE_LOCKED:
		{
			gLocalPlayer.pev->iuser1 = OBS_CHASE_LOCKED;
			break;
		}

		case OBS_CHASE_FREE:
		{
			gLocalPlayer.pev->iuser1 = OBS_CHASE_FREE;
			m_autoDirector->value = 0;
			break;
		}

		case OBS_ROAMING:
		{
			gLocalPlayer.pev->iuser1 = OBS_ROAMING;

			if (gLocalPlayer.pev->iuser2)
			{
				V_GetChasePos(gLocalPlayer.pev->iuser2, v_cl_angles, vJumpOrigin, vJumpAngles);
				gEngfuncs.SetViewAngles(vJumpAngles);
				iJumpSpectator = 1;
			}

			break;
		}

		case OBS_IN_EYE:
		{
			gLocalPlayer.pev->iuser1 = OBS_IN_EYE;
			break;
		}

		case OBS_MAP_FREE:
		{
			gLocalPlayer.pev->iuser1 = OBS_MAP_FREE;

			m_mapZoom = m_OverviewData.zoom;
			m_mapOrigin = m_OverviewData.origin;
			break;
		}

		case OBS_MAP_CHASE:
		{
			gLocalPlayer.pev->iuser1 = OBS_MAP_CHASE;

			m_mapZoom = m_OverviewData.zoom;
			m_mapOrigin = m_OverviewData.origin;
			break;
		}
		}

		if (gLocalPlayer.pev->iuser1 == OBS_ROAMING)
		{
			if (gEngfuncs.pfnGetCvarFloat("cl_observercrosshair") != 0.0)
			{
				// UNDONE, get this thing back.
				//gEngfuncs.pfnSetCrosshair(gHUD::m_Crosshair.m_hObserverCrosshair, gHUD::m_Crosshair.m_rcObserverCrosshair, 255, 255, 255);
			}
			else
			{
				static wrect_t nullrc;
				gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
			}
		}

		// UNDONE
		//gViewPortInterface->UpdateSpectatorPanel();
	}
}

void CHudSpectator::HandleButtonsDown(int ButtonPressed)
{
	double time = gEngfuncs.GetClientTime();

	int newMainMode = gLocalPlayer.pev->iuser1;
	int newInsetMode = m_pip->value;

	// UNDONE
	//if (!gViewPortInterface)
		//return;

	if (gHUD::m_bIntermission)
		return;

	if (!gLocalPlayer.pev->iuser1)
		return;

	if (gEngfuncs.pDemoAPI->IsPlayingback() && !gEngfuncs.IsSpectateOnly())
		return;

	if (m_flNextObserverInput > time)
		return;

	if (ButtonPressed & IN_DUCK)
	{
		// UNDONE
		//if (!gViewPortInterface->IsSpectatorBarVisible())
			//gViewPortInterface->ShowSpectatorGUIBar();
	}

	if (ButtonPressed & IN_USE)
	{
		newInsetMode = ToggleInset(true);
	}

	if (gEngfuncs.IsSpectateOnly())
	{
		if (ButtonPressed & IN_JUMP)
		{
			if (gLocalPlayer.pev->iuser1 == OBS_CHASE_LOCKED)
				newMainMode = OBS_CHASE_FREE;
			else if (gLocalPlayer.pev->iuser1 == OBS_CHASE_FREE)
				newMainMode = OBS_IN_EYE;
			else if (gLocalPlayer.pev->iuser1 == OBS_IN_EYE)
				newMainMode = OBS_ROAMING;
			else if (gLocalPlayer.pev->iuser1 == OBS_ROAMING)
				newMainMode = OBS_MAP_FREE;
			else if (gLocalPlayer.pev->iuser1 == OBS_MAP_FREE)
				newMainMode = OBS_MAP_CHASE;
			else
				newMainMode = OBS_CHASE_FREE;
		}

		if (ButtonPressed & (IN_ATTACK | IN_ATTACK2))
		{
			FindNextPlayer((ButtonPressed & IN_ATTACK2) ? true : false);

			if (gLocalPlayer.pev->iuser1 == OBS_ROAMING)
			{
				gEngfuncs.SetViewAngles(vJumpAngles);
				iJumpSpectator = 1;
				gHUD::m_Radar.m_iPlayerLastPointedAt = gLocalPlayer.pev->iuser2;
			}

			m_autoDirector->value = 0.0f;
		}
	}

	SetModes(newMainMode, newInsetMode);

	if (gLocalPlayer.pev->iuser1 == OBS_MAP_FREE)
	{
		if (ButtonPressed & IN_FORWARD)
			m_zoomDelta = 0.01f;

		if (ButtonPressed & IN_BACK)
			m_zoomDelta = -0.01f;

		if (ButtonPressed & IN_MOVELEFT)
			m_moveDelta = -12.0f;

		if (ButtonPressed & IN_MOVERIGHT)
			m_moveDelta = 12.0f;
	}

	m_flNextObserverInput = time + 0.2;
}

void CHudSpectator::HandleButtonsUp(int ButtonPressed)
{
	// UNDONE
	//if (!gViewPortInterface)
		//return;

	//if (!gViewPortInterface->IsSpectatorGUIVisible())
		//return;

	if (ButtonPressed & (IN_FORWARD | IN_BACK))
		m_zoomDelta = 0.0f;

	if (ButtonPressed & (IN_MOVELEFT | IN_MOVERIGHT))
		m_moveDelta = 0.0f;
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

	if (gLocalPlayer.pev->iuser2)
		iStart = gLocalPlayer.pev->iuser2;
	else
		iStart = 1;

	int iCurrent = iStart;
	int iDir = bReverse ? -1 : 1;

	// UNDONE
	//gViewPortInterface->GetAllPlayersInfo();

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

		gLocalPlayer.pev->iuser2 = iCurrent;
		break;

	} while (iCurrent != iStart);

	if (!gLocalPlayer.pev->iuser2)
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

	// UNDONE
	//gViewPortInterface->UpdateSpectatorPanel();
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

	gLocalPlayer.pev->iuser2 = 0;
	//gViewPortInterface->GetAllPlayersInfo();	// UNDONE

	cl_entity_t* pEnt = NULL;

	for (int i = 1; i < MAX_PLAYERS; i++)
	{
		pEnt = gEngfuncs.GetEntityByIndex(i);

		if (!IsActivePlayer(pEnt))
			continue;

		if (!Q_stricmp(g_PlayerInfoList[pEnt->index].name, name))
		{
			gLocalPlayer.pev->iuser2 = i;
			break;
		}
	}

	if (!gLocalPlayer.pev->iuser2)
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

	// UNDONE
	//gViewPortInterface->UpdateSpectatorPanel();
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
		gLocalPlayer.m_iRoleType = Role_UNASSIGNED;
		gLocalPlayer.m_iTeam = 0;

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
			if ((gLocalPlayer.pev->iuser2 != m_lastPrimaryObject) || (gLocalPlayer.pev->iuser3 != m_lastSecondaryObject))
				V_ResetChaseCam();

			gLocalPlayer.pev->iuser2 = m_lastPrimaryObject;
			gLocalPlayer.pev->iuser3 = m_lastSecondaryObject;
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
			SetModes(OBS_ROAMING, -1);
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

		strncpy(m_HUDMessageText[m_lastHudMessage], READ_STRING(), 128);
		m_HUDMessageText[m_lastHudMessage][127] = 0;

		msg->pMessage = m_HUDMessageText[m_lastHudMessage];
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
			SetModes(OBS_ROAMING, -1);
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

		SetModes(OBS_ROAMING, -1);

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
	if (gEngfuncs.Cmd_Argc() <= 1)
	{
		gEngfuncs.Con_Printf("usage:  spec_mode <Main Mode> [<Inset Mode>]\n");
		return;
	}

	if (gEngfuncs.Cmd_Argc() == 2)
	{
		gHUD::m_Spectator.SetModes(Q_atoi(gEngfuncs.Cmd_Argv(1)), -1);
	}
	else if (gEngfuncs.Cmd_Argc() == 3 && Q_atoi(gEngfuncs.Cmd_Argv(2)) != -1)
	{
		gHUD::m_Spectator.SetModes(Q_atoi(gEngfuncs.Cmd_Argv(1)), Q_atoi(gEngfuncs.Cmd_Argv(2)));
	}
	else if (gEngfuncs.Cmd_Argc() == 3 && Q_atoi(gEngfuncs.Cmd_Argv(2)) == -1)
	{
		if (gHUD::m_Spectator.m_pip->value == INSET_OFF)
			gHUD::m_Spectator.SetModes(-1, INSET_CHASE_FREE);
		else
			gHUD::m_Spectator.SetModes(-1, INSET_OFF);
	}
}

void SpectatorToggleInset(void)
{
	gHUD::m_Spectator.SetModes(-1, gHUD::m_Spectator.ToggleInset(false));
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

void SpectatorHelp(void)
{
	// UNDONE
	//if (gViewPortInterface)
		//gViewPortInterface->ShowVGUIMenu(MENU_SPECHELP);
}

void SpectatorMenu(void)
{
	if (gEngfuncs.Cmd_Argc() <= 1)
	{
		gEngfuncs.Con_Printf("usage:  spec_menu <0|1>\n");
		return;
	}

	// UNDONE
	/*if (atoi(gEngfuncs.Cmd_Argv(1)) != 0)
		gViewPortInterface->ShowSpectatorGUI();
	else
		gViewPortInterface->DeactivateSpectatorGUI();*/
}

void ToggleScores(void)
{
	// UNDONE
	/*if (gViewPortInterface)
	{
		if (gViewPortInterface->IsScoreBoardVisible())
		{
			gViewPortInterface->HideScoreBoard();
		}
		else
		{
			gViewPortInterface->ShowScoreBoard();
		}
	}*/
}

void SpectatorToggleDrawNames(void)
{
	if (gHUD::m_Spectator.m_drawnames->value)
		gEngfuncs.Cvar_SetValue("spec_drawnames_internal", 0);
	else
		gEngfuncs.Cvar_SetValue("spec_drawnames_internal", 1);
}

void SpectatorToggleDrawCone(void)
{
	if (gHUD::m_Spectator.m_drawcone->value)
		gEngfuncs.Cvar_SetValue("spec_drawcone_internal", 0);
	else
		gEngfuncs.Cvar_SetValue("spec_drawcone_internal", 1);
}

void SpectatorToggleDrawStatus(void)
{
	if (gHUD::m_Spectator.m_drawstatus->value)
		gEngfuncs.Cvar_SetValue("spec_drawstatus_internal", 0);
	else
		gEngfuncs.Cvar_SetValue("spec_drawstatus_internal", 1);
}

void SpectatorToggleAutoDirector(void)
{
	if (gHUD::m_Spectator.m_autoDirector->value)
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

int CHudSpectator::Draw(float flTime)
{
	int lx;

	char string[256];
	float* color;

	if (!gLocalPlayer.pev->iuser1)
	{
		// UNDONE
		//if (gViewPortInterface->IsSpectatorGUIVisible())
			//gViewPortInterface->HideSpectatorGUI();

		return 0;
	}

	if (m_lastAutoDirector != m_autoDirector->value)
	{
		m_lastAutoDirector = m_autoDirector->value;

		gEngfuncs.pfnClientCmd(SharedVarArgs("spec_set_ad %f", m_autoDirector->value));

		if ((m_lastAutoDirector != 0.0f) && (gLocalPlayer.pev->iuser1 == OBS_CHASE_FREE))
		{
			SetModes(OBS_CHASE_LOCKED, -1);
		}
		else if ((m_lastAutoDirector == 0.0f) && (gLocalPlayer.pev->iuser1 == OBS_CHASE_LOCKED))
		{
			SetModes(OBS_CHASE_FREE, -1);
		}
	}

	if ((m_zoomDelta != 0.0f) && (gLocalPlayer.pev->iuser1 == OBS_MAP_FREE))
	{
		m_mapZoom += m_zoomDelta;

		if (m_mapZoom > 3.0f)
			m_mapZoom = 3.0f;

		if (m_mapZoom < 0.5f)
			m_mapZoom = 0.5;
	}

	if ((m_moveDelta != 0.0f) && (gLocalPlayer.pev->iuser1 != OBS_ROAMING))
	{
		vec3_t right;
		AngleVectors(v_angles, NULL, right, NULL);
		VectorNormalize(right);
		VectorScale(right, m_moveDelta, right);
		VectorAdd(m_mapOrigin, right, m_mapOrigin);
	}

	if (gLocalPlayer.pev->iuser1 != m_mode->value)
	{
		gEngfuncs.Cvar_SetValue("spec_mode_internal", gLocalPlayer.pev->iuser1);
	}

	if (gLocalPlayer.pev->iuser1 < OBS_MAP_FREE)
		return 1;

	if (!m_drawnames->value)
		return 1;

	// UNDONE
	//gViewPortInterface->GetAllPlayersInfo();

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (m_vPlayerPos[i][2] < 0)
			continue;

		if (m_pip->value != INSET_OFF)
		{
			if (m_vPlayerPos[i][0] > m_OverviewData.insetWindowX&& m_vPlayerPos[i][1] > m_OverviewData.insetWindowY&& m_vPlayerPos[i][0] < m_OverviewData.insetWindowX + m_OverviewData.insetWindowWidth && m_vPlayerPos[i][1] < m_OverviewData.insetWindowY + m_OverviewData.insetWindowHeight)
				continue;
		}

		color = GetClientColor(i + 1);
		sprintf(string, "%s", g_PlayerInfoList[i + 1].name);
		lx = strlen(string) * 3;

		if (m_pip->value != INSET_OFF)
		{
			if (m_vPlayerPos[i][0] - lx > m_OverviewData.insetWindowX&& m_vPlayerPos[i][1] > m_OverviewData.insetWindowY&& m_vPlayerPos[i][0] - lx < m_OverviewData.insetWindowX + m_OverviewData.insetWindowWidth && m_vPlayerPos[i][1] < m_OverviewData.insetWindowY + m_OverviewData.insetWindowHeight)
				continue;
		}

		gEngfuncs.pfnDrawSetTextColor(color[0], color[1], color[2]);
		gEngfuncs.pfnDrawConsoleString(m_vPlayerPos[i][0] - lx, m_vPlayerPos[i][1], string);
	}

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

void CHudSpectator::SetCameraView(Vector pos, Vector angle, float fov)
{
	m_FOV = fov;
	VectorCopy(pos, vJumpOrigin);
	VectorCopy(angle, vJumpAngles);
	gEngfuncs.SetViewAngles(vJumpAngles);
	iJumpSpectator = 1;
}

float CHudSpectator::GetFOV(void)
{
	return m_FOV;
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
