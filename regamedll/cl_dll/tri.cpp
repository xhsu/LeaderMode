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

void DrawUtils::Draw2DLinearProgressBar(float x, float y, float flWidth, float flFullLength, float flPercent)
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

static float factors[4];

void DrawUtils::Draw2DQuadProgressBar(float x, float y, float flInnerWidth, float flInnerHeight, float flThickness, float flPercent)
{
	flPercent = Q_clamp(flPercent, 0.0f, 1.0f);

	if (flPercent >= 0.75f)
	{
		factors[0] = 1.0f;
		factors[1] = 1.0f;
		factors[2] = 1.0f;
		factors[3] = (flPercent - 0.75f) / 0.25f;
	}
	else if (flPercent >= 0.50f)
	{
		factors[0] = 1.0f;
		factors[1] = 1.0f;
		factors[2] = (flPercent - 0.5f) / 0.25f;
		factors[3] = 0.0f;
	}
	else if (flPercent >= 0.25f)
	{
		factors[0] = 1.0f;
		factors[1] = (flPercent - 0.25f) / 0.25f;
		factors[2] = 0.0f;
		factors[3] = 0.0f;
	}
	else
	{
		factors[0] = flPercent / 0.25f;
		factors[1] = 0.0f;
		factors[2] = 0.0f;
		factors[3] = 0.0f;
	}

	// first bar: -x => +x
	if (factors[0] > 0.0f)
	{
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + (flInnerWidth + flThickness * 2) * factors[0], y);
		glVertex2f(x + (flInnerWidth + flThickness * 2) * factors[0], y + flThickness * 1);
		glVertex2f(x, y + flThickness * 1);
		glEnd();
	}

	// second bar: -y => +y
	if (factors[1] > 0.0f)
	{
		glBegin(GL_QUADS);
		glVertex2f(x + flInnerWidth + flThickness * 1, y + flThickness * 1);
		glVertex2f(x + flInnerWidth + flThickness * 2, y + flThickness * 1);
		glVertex2f(x + flInnerWidth + flThickness * 2, y + (flInnerHeight + flThickness * 2) * factors[1]);
		glVertex2f(x + flInnerWidth + flThickness * 1, y + (flInnerHeight + flThickness * 2) * factors[1]);
		glEnd();
	}

	// third bar: +x => -x
	// this one is kind of tricky, since we have to retract this bar from the left-side.
	if (factors[2] > 0.0f)
	{
		glBegin(GL_QUADS);
		glVertex2f(x + (flInnerWidth + flThickness * 1) * (1.0f - factors[2]), y + flInnerHeight + flThickness * 1);
		glVertex2f(x + flInnerWidth + flThickness * 1, y + flInnerHeight + flThickness * 1);
		glVertex2f(x + flInnerWidth + flThickness * 1, y + flInnerHeight + flThickness * 2);
		glVertex2f(x + (flInnerWidth + flThickness * 1) * (1.0f - factors[2]), y + flInnerHeight + flThickness * 2);
		glEnd();
	}

	// fourth bar: +y => -y
	// same as above. we have to retract this bar from up-side.
	if (factors[3] > 0.0f)
	{
		glBegin(GL_QUADS);
		glVertex2f(x, y + flThickness * 1 + flInnerHeight * (1.0f - factors[3]));
		glVertex2f(x + flThickness * 1, y + flThickness * 1 + flInnerHeight * (1.0f - factors[3]));
		glVertex2f(x + flThickness * 1, y + flInnerHeight + flThickness * 1);
		glVertex2f(x, y + flInnerHeight + flThickness * 1);
		glEnd();
	}
}
