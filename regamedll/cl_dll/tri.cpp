/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

FogParameters g_FogParameters;

void RenderFog(void)
{
	FogParameters fog;
	int bOn;

	fog.color[0] = g_FogParameters.color[0];
	fog.color[1] = g_FogParameters.color[1];
	fog.color[2] = g_FogParameters.color[2];
	fog.density = g_FogParameters.density;
	fog.affectsSkyBox = g_FogParameters.affectsSkyBox;

	if (cl_fog_skybox)
		fog.affectsSkyBox = cl_fog_skybox->value;

	if (cl_fog_density)
		fog.density = cl_fog_density->value;

	if (cl_fog_r)
		fog.color[0] = cl_fog_r->value;

	if (cl_fog_g)
		fog.color[1] = cl_fog_g->value;

	if (cl_fog_b)
		fog.color[2] = cl_fog_b->value;

	if (g_iWaterLevel <= 1)
		bOn = fog.density > 0.0;
	else
		bOn = false;

	gEngfuncs.pTriAPI->FogParams(fog.density, fog.affectsSkyBox);
	gEngfuncs.pTriAPI->Fog(fog.color, 100, 2000, bOn);
}

void HUD_DrawNormalTriangles2(void)
{
	gHUD::m_Spectator.DrawOverview();
}

void HUD_DrawTransparentTriangles2(void)
{
	// UNDONE
	/*if (gConfigs.bEnableClientUI)
		g_pViewPort->RenderMapSprite();*/

	RenderFog();
}


////////////////
// Draw2DQuad //
////////////////

void DrawUtils::Draw2DQuad(float x1, float y1, float x2, float y2)
{
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);

	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(x1, y1, 0);

	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(x1, y2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(x2, y2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(x2, y1, 0);

	gEngfuncs.pTriAPI->End();
}

void DrawUtils::Draw2DQuadNoTex(float x1, float y1, float x2, float y2)
{
	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x1, y2);
	glVertex2f(x2, y2);
	glVertex2f(x2, y1);
	glEnd();
}

void DrawUtils::Draw2DProgressBar(float x, float y, float flWidth, float flFullLength, float flPercent)
{
	// the border of bar.
	glBegin(GL_LINE_LOOP);
	glVertex2f(x, y);
	glVertex2f(x + flFullLength, y);
	glVertex2f(x + flFullLength, y + flWidth);
	glVertex2f(x, y + flWidth);
	glEnd();

	// too short to be drawn.
	if (flPercent * flFullLength < 0.5f)
		return;

	// fill the bar.
	glBegin(GL_QUADS);
	glVertex2f(x + 1.0f, y + 2.0f);
	glVertex2f(x + flFullLength * flPercent - 2.0f, y + 2.0f);
	glVertex2f(x + flFullLength * flPercent - 2.0f, y + flWidth - 1.0f);
	glVertex2f(x + 1.0f, y + flWidth - 1.0f);
	glEnd();
}

void DrawUtils::Draw2DHollowQuad(float x, float y, float flWidth, float flHeight)
{
	glBegin(GL_LINE_LOOP);
	glVertex2f(x, y);
	glVertex2f(x + flWidth, y);
	glVertex2f(x + flWidth, y + flHeight);
	glVertex2f(x, y + flHeight);
	glEnd();
}
