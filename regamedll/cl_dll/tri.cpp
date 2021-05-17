/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

FogParameters g_FogParameters;
std::list<RegionalFog> g_lstRegionalFog;

void RegionalFogThink(void)
{
	auto iter = g_lstRegionalFog.begin();
	while (iter != g_lstRegionalFog.end())
	{
		if (iter->m_flTimeRemoval < g_flClientTime)
		{
			iter = g_lstRegionalFog.erase(iter);
		}
		else
		{
			// decay
			if (iter->m_flTimeStartDecay < g_flClientTime)
			{
				iter->m_flDecayMultiplier = Q_abs(iter->m_flTimeRemoval - g_flClientTime) / Q_abs(iter->m_flTimeRemoval - iter->m_flTimeStartDecay);
			}

			// move on
			iter++;
		}
	}
}

void RenderFog(void)
{
	float flDensity = g_FogParameters.density;
	auto pPlayer = gEngfuncs.GetLocalPlayer();
	float flDistance = 0, flFraction = 0;
	Vector rgbColor = g_FogParameters.color;
	short iColorCount = g_FogParameters.density > 0 ? 1 : 0;	// if this map has no fog at first, don't event start counting from 1.

	for (const auto& elem : g_lstRegionalFog)
	{
		flDistance = (pPlayer->origin - elem.m_vecOrigin).Length();
		if (flDistance > elem.m_flRadius)
			continue;

		flFraction = Q_clamp(1.0f - flDistance / elem.m_flRadius, 0.0f, 1.0f);

		// accumulate the fog density.
		flDensity += flFraction * elem.m_flDensity * elem.m_flDecayMultiplier;

		// "accumulate the color.
		rgbColor += elem.m_Color;
		iColorCount++;
	}

	// prevents divide by 0.
	if (iColorCount <= 0)
		iColorCount = 1;

	// take the average of the color sum.
	rgbColor /= float(iColorCount);

	// render.
	gEngfuncs.pTriAPI->FogParams(flDensity, true);
	gEngfuncs.pTriAPI->Fog(rgbColor, 100, 2000, flDensity > 0);
}

void HUD_DrawNormalTriangles2(void)
{
	// LUNA: draw global full overview here.
}

#define glVertex3fVec(vec)	glVertex3f(vec.x, vec.y, vec.z)

void HUD_DrawTransparentTriangles2(void)
{
	// UNDONE
	/*if (gConfigs.bEnableClientUI)
		g_pViewPort->RenderMapSprite();*/

	/*if (CL_IsDead() || !g_pCurWeapon)
		return;
	g_flSpread = g_pCurWeapon->GetSpread();
	Vector vecOrigin = gPseudoPlayer.GetGunPosition();
	Vector vDir = g_pparams.viewangles.MakeVector();

	Vector vUp = vecOrigin + (vDir + g_pparams.up * g_flSpread) * 4096;
	Vector vDown = vecOrigin + (vDir - g_pparams.up * g_flSpread) * 4096;
	Vector vRight = vecOrigin + (vDir + g_pparams.right * g_flSpread) * 4096;
	Vector vLeft = vecOrigin + (vDir - g_pparams.right * g_flSpread) * 4096;

	glPushAttrib(GL_ALL_ATTRIB_BITS);				// Save current depth range value
	glDepthRange(0.0001, 0.0002);					// Change depth range for 3D HUD drawing

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 1, 1, 1);

	glBegin(GL_LINE_LOOP);
	glVertex3fVec(vUp);
	glVertex3fVec(vRight);
	glVertex3fVec(vDown);
	glVertex3fVec(vLeft);
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glPopAttrib();*/									// Restore depth range value

	if (g_pParticleMan)
		g_pParticleMan->Update();

	// Rain stuff.
	ProcessFXObjects();
	ProcessRain();
	DrawRain();
	DrawFXObjects();

	// Fog can of course cover rain.
	RenderFog();
}

//////////////
// GL Tools //
//////////////

void DrawUtils::glRegularTexDrawingInit(const Vector& color, const float& alpha)
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(color.r, color.g, color.b, alpha);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
}

void DrawUtils::glRegularTexDrawingInit(const unsigned long& ulRGB, byte alpha)
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glSetColor(ulRGB, alpha);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
}

void DrawUtils::glRegularPureColorDrawingInit(const Vector& color, const float& alpha)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(color.r, color.g, color.b, alpha);
}

void DrawUtils::glRegularPureColorDrawingInit(const unsigned long& ulRGB, byte alpha)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glSetColor(ulRGB, alpha);
}

void DrawUtils::glRegularPureColorDrawingExit()
{
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

void DrawUtils::glSetColor(const unsigned long& ulRGB, byte alpha)
{
	glColor4ub((ulRGB & 0xFF0000) >> 16, (ulRGB & 0xFF00) >> 8, ulRGB & 0xFF, alpha);
}

void DrawUtils::glSetColor(const Vector& color, const float& alpha)
{
	glColor4f(color.r, color.g, color.b, alpha);
}

void DrawUtils::glSetTexture(const GLuint& iId)
{
	glBindTexture(GL_TEXTURE_2D, iId);
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

void DrawUtils::Draw2DQuadCustomTex(float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2)
{
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);

	gEngfuncs.pTriAPI->TexCoord2f(u1, v1);
	gEngfuncs.pTriAPI->Vertex3f(x1, y1, 0);

	gEngfuncs.pTriAPI->TexCoord2f(u1, v2);
	gEngfuncs.pTriAPI->Vertex3f(x1, y2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(u2, v2);
	gEngfuncs.pTriAPI->Vertex3f(x2, y2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(u2, v1);
	gEngfuncs.pTriAPI->Vertex3f(x2, y1, 0);

	gEngfuncs.pTriAPI->End();
}

void DrawUtils::Draw2DQuadCustomTex(float x1, float y1, float x2, float y2, const Vector2D* vecs)
{
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);

	gEngfuncs.pTriAPI->TexCoord2f(vecs[0].x, vecs[0].y);
	gEngfuncs.pTriAPI->Vertex3f(x1, y1, 0);

	gEngfuncs.pTriAPI->TexCoord2f(vecs[1].x, vecs[1].y);
	gEngfuncs.pTriAPI->Vertex3f(x2, y1, 0);

	gEngfuncs.pTriAPI->TexCoord2f(vecs[2].x, vecs[2].y);
	gEngfuncs.pTriAPI->Vertex3f(x2, y2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(vecs[3].x, vecs[3].y);
	gEngfuncs.pTriAPI->Vertex3f(x1, y2, 0);

	gEngfuncs.pTriAPI->End();
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

static float flInnerBlockWidth = 0.0f;
static float flInnerBlockHeight = 0.0f;

void DrawUtils::Draw2DLinearProgressBar(float x, float y, float flTotalWidth, float flTotalHeight, float flThickness, float flGapSize, float flPercentage)
{
	flPercentage = Q_clamp(flPercentage, 0.0f, 1.0f);
	flInnerBlockWidth = (flTotalWidth - flThickness * 2 - flGapSize * 2) * flPercentage;
	flInnerBlockHeight = flTotalHeight - flThickness * 2 - flGapSize * 2;

	// first bar: -x => +x
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + flTotalWidth, y);
	glVertex2f(x + flTotalWidth, y + flThickness * 1);
	glVertex2f(x, y + flThickness * 1);
	glEnd();

	// second bar: -y => +y
	glBegin(GL_QUADS);
	glVertex2f(x + flTotalWidth - flThickness * 1, y + flThickness * 1);
	glVertex2f(x + flTotalWidth, y + flThickness * 1);
	glVertex2f(x + flTotalWidth, y + flThickness + (flTotalHeight - flThickness));
	glVertex2f(x + flTotalWidth - flThickness * 1, y + flThickness + (flTotalHeight - flThickness));
	glEnd();

	// third bar: +x => -x
	glBegin(GL_QUADS);
	glVertex2f(x + (flTotalWidth - flThickness * 1), y + flTotalHeight - flThickness * 1);
	glVertex2f(x + flTotalWidth - flThickness * 1, y + flTotalHeight - flThickness * 1);
	glVertex2f(x + flTotalWidth - flThickness * 1, y + flTotalHeight);
	glVertex2f(x + (flTotalWidth - flThickness * 1), y + flTotalHeight);
	glEnd();

	// fourth bar: +y => -y
	glBegin(GL_QUADS);
	glVertex2f(x, y + flThickness * 1 + (flTotalHeight - flThickness * 2));
	glVertex2f(x + flThickness * 1, y + flThickness * 1 + (flTotalHeight - flThickness * 2));
	glVertex2f(x + flThickness * 1, y + flTotalHeight - flThickness * 1);
	glVertex2f(x, y + flTotalHeight - flThickness * 1);
	glEnd();

	// Inner block
	glBegin(GL_QUADS);
	glVertex2f(x + flThickness + flGapSize, y + flThickness + flGapSize);
	glVertex2f(x + flThickness + flGapSize + flInnerBlockWidth, y + flThickness + flGapSize);
	glVertex2f(x + flThickness + flGapSize + flInnerBlockWidth, y + flThickness + flGapSize + flInnerBlockHeight);
	glVertex2f(x + flThickness + flGapSize, y + flThickness + flGapSize + flInnerBlockHeight);
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
	// let the x and y become the coord of inner quad.
	x -= flThickness;
	y -= flThickness;

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
		glVertex2f(x + flInnerWidth + flThickness * 2, y + (flInnerHeight + flThickness) * factors[1] + flThickness);	// this thickness value should stayed outside of the scalar, since it is a portion of BASE_Y.
		glVertex2f(x + flInnerWidth + flThickness * 1, y + (flInnerHeight + flThickness) * factors[1] + flThickness);
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

void DrawUtils::Draw2DQuadProgressBar2(float x, float y, float flTotalWidth, float flTotalHeight, float flThickness, float flPercent)
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
		glVertex2f(x + flTotalWidth * factors[0], y);
		glVertex2f(x + flTotalWidth * factors[0], y + flThickness * 1);
		glVertex2f(x, y + flThickness * 1);
		glEnd();
	}

	// second bar: -y => +y
	if (factors[1] > 0.0f)
	{
		glBegin(GL_QUADS);
		glVertex2f(x + flTotalWidth - flThickness * 1, y + flThickness * 1);
		glVertex2f(x + flTotalWidth, y + flThickness * 1);
		glVertex2f(x + flTotalWidth, y + flThickness + (flTotalHeight - flThickness) * factors[1]);
		glVertex2f(x + flTotalWidth - flThickness * 1, y + flThickness + (flTotalHeight - flThickness) * factors[1]);
		glEnd();
	}

	// third bar: +x => -x
	// this one is kind of tricky, since we have to retract this bar from the left-side.
	if (factors[2] > 0.0f)
	{
		glBegin(GL_QUADS);
		glVertex2f(x + (flTotalWidth - flThickness * 1) * (1.0f - factors[2]), y + flTotalHeight - flThickness * 1);
		glVertex2f(x + flTotalWidth - flThickness * 1, y + flTotalHeight - flThickness * 1);
		glVertex2f(x + flTotalWidth - flThickness * 1, y + flTotalHeight);
		glVertex2f(x + (flTotalWidth - flThickness * 1) * (1.0f - factors[2]), y + flTotalHeight);
		glEnd();
	}

	// fourth bar: +y => -y
	// same as above. we have to retract this bar from up-side.
	if (factors[3] > 0.0f)
	{
		glBegin(GL_QUADS);
		glVertex2f(x, y + flThickness * 1 + (flTotalHeight - flThickness * 2) * (1.0f - factors[3]));
		glVertex2f(x + flThickness * 1, y + flThickness * 1 + (flTotalHeight - flThickness * 2) * (1.0f - factors[3]));
		glVertex2f(x + flThickness * 1, y + flTotalHeight - flThickness * 1);
		glVertex2f(x, y + flTotalHeight - flThickness * 1);
		glEnd();
	}
}
