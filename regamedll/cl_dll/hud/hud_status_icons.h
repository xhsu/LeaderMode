/*

Created Date: Mar 12 2020

*/

#pragma once

class CHudStatusIcons : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	void Reset(void);
	int Draw(float flTime);

public:
	void MsgFunc_StatusIcon(int iSize, void* pbuf);

	enum
	{
		MAX_ICONSPRITENAME_LENGTH = MAX_SPRITE_NAME_LENGTH,
		MAX_ICONSPRITES = 4,
	};

	void EnableIcon(char* pszIconName, unsigned char red, unsigned char green, unsigned char blue, bool bFlash);
	void DisableIcon(char* pszIconName);
	bool InBuyZone(void);

private:
	int m_iCrossWidth;
	bool m_bFlashOn;
	float m_tmNextFlash;

	typedef struct
	{
		char szSpriteName[MAX_ICONSPRITENAME_LENGTH];
		hSprite spr;
		wrect_t rc;
		unsigned char r, g, b;
		int bFlash;
	}
	icon_sprite_t;

	icon_sprite_t m_IconList[MAX_ICONSPRITES];
};
