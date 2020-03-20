/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

struct FogParameters
{
	float color[3];
	float density;
	bool affectsSkyBox;
};

extern FogParameters g_FogParameters;

namespace DrawUtils
{
	void Draw2DQuad(float x1, float y1, float x2, float y2);
}

// export funcs
void HUD_DrawNormalTriangles2(void);
void HUD_DrawTransparentTriangles2(void);
