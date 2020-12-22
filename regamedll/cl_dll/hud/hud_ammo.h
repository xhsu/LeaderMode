/*

Created Date: Mar 11 2020

*/

#pragma once

typedef struct
{
	unsigned char r, g, b, a;
}
RGBA;

class CHudAmmo : public CBaseHudElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);

public:
	client_sprite_t* m_pTxtList{ nullptr };	// original data. for others to inquiry.
	int m_iTxtListCount{ 0 };
	std::array<hSprite, AMMO_MAXTYPE> m_rghAmmoSprite;
	std::array<wrect_t, AMMO_MAXTYPE> m_rgrcAmmoSprite;
	float m_flAlpha{ 0 };
	int m_iLastDrawnClip{ 0 };
	int m_iLastDrawnBpAmmo{ 0 };
};
