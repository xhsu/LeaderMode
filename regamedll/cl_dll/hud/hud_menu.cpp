/*

Created Date: Mar 11 2020

*/

#include "cl_base.h"

char g_szMenuString[MAX_MENU_STRING];
char g_szPrelocalisedMenuString[MAX_MENU_STRING];

int menu_r;
int menu_g;
int menu_b;
int menu_x;
int menu_ralign;

int CHudMenu::Init(void)
{
	gHUD::AddHudElem(this);

	InitHUDData();

	return 1;
}

void CHudMenu::InitHUDData(void)
{
	m_fMenuDisplayed = 0;
	m_bitsValidSlots = 0;

	Reset();
}

void CHudMenu::Reset(void)
{
	g_szPrelocalisedMenuString[0] = 0;
	m_fWaitingForMore = FALSE;
}

int CHudMenu::Draw(float flTime)
{
	if (m_flShutoffTime > 0)
	{
		if (m_flShutoffTime <= gHUD::m_flTime)
		{
			m_fMenuDisplayed = 0;
			m_bitsFlags &= ~HUD_ACTIVE;
			return 1;
		}
	}

	// UNDONE
	/*if (gViewPortInterface && gViewPortInterface->IsScoreBoardVisible())
		return 1;*/

	int nlc = 0;

	for (int i = 0; i < MAX_MENU_STRING && g_szMenuString[i] != '\0'; i++)
	{
		if (g_szMenuString[i] == '\n')
			nlc++;
	}

	menu_x = 20;
	menu_r = 255;
	menu_g = 255;
	menu_b = 20;
	menu_ralign = 0;

	int y = (ScreenHeight / 2) - ((nlc / 2) * 12) - 40;
	const char* sptr = g_szMenuString;
	int i;
	char menubuf[80];
	const char* ptr;

	while (*sptr)
	{
		if (*sptr == '\\')
		{
			switch (*(sptr + 1))
			{
			case '\0':
			{
				sptr += 1;
				break;
			}

			case 'w':
			{
				menu_r = 255;
				menu_g = 255;
				menu_b = 255;

				sptr += 2;
				break;
			}

			case 'd':
			{
				menu_r = 100;
				menu_g = 100;
				menu_b = 100;

				sptr += 2;
				break;
			}

			case 'y':
			{
				menu_r = 255;
				menu_g = 210;
				menu_b = 64;

				sptr += 2;
				break;
			}

			case 'r':
			{
				menu_r = 210;
				menu_g = 24;
				menu_b = 0;

				sptr += 2;
				break;
			}

			case 'R':
			{
				menu_ralign = 1;
				menu_x = ScreenWidth / 2;

				sptr += 2;
				break;
			}

			default:
			{
				sptr += 2;
			}
			}

			continue;
		}

		if (*sptr == '\n')
		{
			menu_ralign = 0;
			menu_x = 20;
			y += gHUD::m_iFontEngineHeight + 2;
			sptr += 1;
			continue;
		}

		for (ptr = sptr; *sptr != '\0'; sptr++)
		{
			if (*sptr == '\n')
				break;

			if (*sptr == '\\')
				break;
		}

		i = sptr - ptr;
		Q_strncpy(menubuf, ptr, Q_min(size_t(i), sizeof(menubuf)));
		menubuf[Q_min(size_t(i), sizeof(menubuf) - 1)] = 0;

		if (menu_ralign)
		{
			menu_x = gHUD::DrawHudStringReverse(menu_x, y, 0, menubuf, menu_r, menu_g, menu_b);
		}
		else
		{
			menu_x = gHUD::DrawHudString(menu_x, y, 320, menubuf, menu_r, menu_g, menu_b);
		}
	}

	return 1;
}

void CHudMenu::MsgFunc_ShowMenu(int iSize, void* pbuf)
{
	char* temp = NULL;

	BEGIN_READ(pbuf, iSize);

	m_bitsValidSlots = READ_SHORT();

	int DisplayTime = READ_CHAR();
	int NeedMore = READ_BYTE();

	if (DisplayTime > 0)
		m_flShutoffTime = DisplayTime + gHUD::m_flTime;
	else
		m_flShutoffTime = -1;

	if (m_bitsValidSlots)
	{
		if (!m_fWaitingForMore)
			Q_strncpy(g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING);
		else
			Q_strncat(g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING - Q_strlen(g_szPrelocalisedMenuString));

		g_szPrelocalisedMenuString[MAX_MENU_STRING - 1] = 0;

		if (!NeedMore)
		{
			Q_strlcpy(g_szMenuString, gHUD::m_TextMessage.BufferedLocaliseTextString(g_szPrelocalisedMenuString));

			if (KB_ConvertString(g_szMenuString, &temp))
			{
				Q_strlcpy(g_szMenuString, temp);
				free(temp);
			}
		}

		m_fMenuDisplayed = 1;
		m_bitsFlags |= HUD_ACTIVE;
	}
	else
	{
		m_fMenuDisplayed = 0;
		m_bitsFlags &= ~HUD_ACTIVE;
	}

	m_fWaitingForMore = NeedMore;
}

bool CHudMenu::SelectMenuItem(int menu_item)
{
	char szbuf[32];

	if ((menu_item > 0) && (m_bitsValidSlots & (1 << (menu_item - 1))))
	{
		Q_snprintf(szbuf, charsmax(szbuf), "menuselect %d\n", menu_item);
		gEngfuncs.pfnClientCmd(szbuf);

		m_fMenuDisplayed = 0;
		m_bitsFlags &= ~HUD_ACTIVE;
		return true;
	}

	return false;
}
