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
	void Draw2DQuadCustomTex(float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2);
	void Draw2DQuadCustomTex(float x1, float y1, float x2, float y2, const Vector2D* vecs);
	void Draw2DLinearProgressBar(float x, float y, float flWidth, float flFullLength, float flPercent);
	void Draw2DHollowQuad(float x, float y, float flWidth, float flHeight);
	void Draw2DQuadProgressBar(float x, float y, float flInnerWidth, float flInnerHeight, float flThickness, float flPercent);
	void Draw2DQuadProgressBar2(float x, float y, float flTotalWidth, float flTotalHeight, float flThickness, float flPercent);

	inline void Draw2DQuad(const Vector2D& lefttop, const Vector2D& rightbottom) { Draw2DQuad(lefttop.x, lefttop.y, rightbottom.x, rightbottom.y); }
	inline void Draw2DQuadNoTex(const Vector2D& lefttop, const Vector2D& rightbottom) { Draw2DQuadNoTex(lefttop.x, lefttop.y, rightbottom.x, rightbottom.y); }
	inline void Draw2DQuadCustomTex(const Vector2D& lefttop, const Vector2D& rightbottom, const Vector2D& texLT, const Vector2D& texRB) { Draw2DQuadCustomTex(lefttop.x, lefttop.y, rightbottom.x, rightbottom.y, texLT.x, texLT.y, texRB.x, texRB.y); }
	inline void Draw2DQuadCustomTex(const Vector2D& lefttop, const Vector2D& rightbottom, const Vector2D* vecs) { Draw2DQuadCustomTex(lefttop.x, lefttop.y, rightbottom.x, rightbottom.y, vecs); }
}

// export funcs
void HUD_DrawNormalTriangles2(void);
void HUD_DrawTransparentTriangles2(void);
