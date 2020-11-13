/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

struct FogParameters
{
	Vector color;
	float density;
	bool affectsSkyBox;
};

extern FogParameters g_FogParameters;

struct RegionalFog
{
	Vector m_vecOrigin;
	Vector m_Color;
	float m_flRadius;
	float m_flDensity;
	double m_flTimeRemoval;
	double m_flTimeStartDecay;
	float m_flDecayMultiplier;
};

extern std::list<RegionalFog> g_lstRegionalFog;

void RegionalFogThink(void);	// update regional fog, e.g. removal.

namespace DrawUtils
{
	void Draw2DQuad(float x1, float y1, float x2, float y2);
	void Draw2DQuadNoTex(float x1, float y1, float x2, float y2);
	void Draw2DLinearProgressBar(float x, float y, float flWidth, float flFullLength, float flPercent);
	void Draw2DHollowQuad(float x, float y, float flWidth, float flHeight);
	void Draw2DQuadProgressBar(float x, float y, float flInnerWidth, float flInnerHeight, float flThickness, float flPercent);
	void Draw2DQuadProgressBar2(float x, float y, float flTotalWidth, float flTotalHeight, float flThickness, float flPercent);
}

// export funcs
void HUD_DrawNormalTriangles2(void);
void HUD_DrawTransparentTriangles2(void);
