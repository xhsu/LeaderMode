/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"

int CHudStatusBar::Init(void)
{
	gHUD::AddHudElem(this);

	Reset();
	return 1;
}

int CHudStatusBar::Draw(float fTime)
{
	if (m_bReparseString)
	{
		for (int i = 0; i < MAX_STATUSBAR_LINES; i++)
		{
			m_rgvecNameColors[i] = VEC_YELLOWISH;
			ParseStatusString(i);
		}

		m_bReparseString = FALSE;
	}

	for (int i = 0; i < MAX_STATUSBAR_LINES; i++)
	{
		int TextHeight, TextWidth;
		gEngfuncs.pfnDrawConsoleStringLen(m_szStatusBar[i], &TextWidth, &TextHeight);

		int x = Q_max(0, Q_max(2, (ScreenWidth - TextWidth)) / 2);
		int y = (ScreenHeight / 2) + (TextHeight);

		if (m_rgvecNameColors[i])
			gEngfuncs.pfnDrawSetTextColor(m_rgvecNameColors[i][0], m_rgvecNameColors[i][1], m_rgvecNameColors[i][2]);

		gEngfuncs.pfnDrawConsoleString(x, y, m_szStatusBar[i]);
	}

	return 1;
}

void CHudStatusBar::Reset(void)
{
	int i = 0;

	m_bitsFlags &= ~HUD_ACTIVE;

	for (i = 0; i < MAX_STATUSBAR_LINES; i++)
		m_szStatusText[i][0] = 0;

	Q_memset(m_iStatusValues, 0, sizeof m_iStatusValues);

	m_iStatusValues[0] = 1;

	for (i = 0; i < MAX_STATUSBAR_LINES; i++)
		m_rgvecNameColors[i] = VEC_YELLOWISH;
}

void CHudStatusBar::ParseStatusString(int line_num)
{
	char szBuffer[MAX_STATUSTEXT_LENGTH];
	Q_memset(szBuffer, 0, sizeof szBuffer);
	gHUD::m_TextMessage.LocaliseTextString(m_szStatusText[line_num], szBuffer, MAX_STATUSTEXT_LENGTH);

	Q_memset(m_szStatusBar[line_num], 0, MAX_STATUSTEXT_LENGTH);

	char* src = szBuffer;
	char* dst = m_szStatusBar[line_num];

	client_textmessage_t* tempMessage;
	char* src_start = src, * dst_start = dst;

	while (*src != 0)
	{
		while (*src == '\n')
			src++;

		if (((src - src_start) >= MAX_STATUSTEXT_LENGTH) || ((dst - dst_start) >= MAX_STATUSTEXT_LENGTH))
			break;

		int index = Q_atoi(src);

		if ((index >= 0 && index < MAX_STATUSBAR_VALUES) && (m_iStatusValues[index] != 0))
		{
			while (*src >= '0' && *src <= '9')
				src++;

			if (*src == '\n' || *src == 0)
				continue;

			while (*src != '\n' && *src != 0)
			{
				if (*src != '%')
				{
					*dst = *src;
					dst++, src++;
				}
				else
				{
					char valtype = *(++src);

					if (valtype == '%')
					{
						*dst = valtype;
						dst++, src++;
						continue;
					}

					index = Q_atoi(++src);

					while (*src >= '0' && *src <= '9')
						src++;

					if (index >= 0 && index < MAX_STATUSBAR_VALUES)
					{
						int indexval = m_iStatusValues[index];
						char szRepString[MAX_PLAYER_NAME_LENGTH];

						switch (valtype)
						{
						case 'p':
						{
							gEngfuncs.pfnGetPlayerInfo(indexval, &g_PlayerInfoList[indexval]);

							if (g_PlayerInfoList[indexval].name != NULL)
							{
								Q_strncpy(szRepString, g_PlayerInfoList[indexval].name, MAX_PLAYER_NAME_LENGTH);
								gHUD::m_Radar.m_iPlayerLastPointedAt = indexval;
								m_rgvecNameColors[line_num] = gHUD::GetColor(indexval);
							}
							else
							{
								Q_strlcpy(szRepString, "******");
							}

							break;
						}

						case 'i':
						{
							Q_snprintf(szRepString, charsmax(szRepString), "%d", indexval);
							break;
						}

						case 'h':
						{
							tempMessage = gEngfuncs.pfnTextMessageGet("Health");

							if (tempMessage != NULL)
							{
								Q_strncpy(szRepString, tempMessage->pMessage, MAX_PLAYER_NAME_LENGTH);
							}
							else
							{
								Q_strlcpy(szRepString, "Health");
							}

							break;
						}

						case 'c':
						{
							switch (indexval)
							{
							case 1:
							{
								tempMessage = gEngfuncs.pfnTextMessageGet("Friend");

								if (tempMessage != NULL)
								{
									Q_strncpy(szRepString, tempMessage->pMessage, MAX_PLAYER_NAME_LENGTH);
								}
								else
								{
									Q_strlcpy(szRepString, "Friend");
								}

								break;
							}

							case 2:
							{
								tempMessage = gEngfuncs.pfnTextMessageGet("Enemy");

								if (tempMessage != NULL)
								{
									Q_strncpy(szRepString, tempMessage->pMessage, MAX_PLAYER_NAME_LENGTH);
								}
								else
								{
									Q_strlcpy(szRepString, "Enemy");
								}

								break;
							}

							case 3:
							{
								tempMessage = gEngfuncs.pfnTextMessageGet("Hostage");

								if (tempMessage != NULL)
								{
									Q_strncpy(szRepString, tempMessage->pMessage, MAX_PLAYER_NAME_LENGTH);
								}
								else
								{
									Q_strlcpy(szRepString, "Hostage");
								}

								break;
							}
							}

							break;
						}

						default:
						{
							szRepString[0] = 0;
						}
						}

						for (char* cp = szRepString; *cp != 0 && ((dst - dst_start) < MAX_STATUSTEXT_LENGTH); cp++, dst++)
							*dst = *cp;
					}
				}
			}
		}
		else
		{
			while (*src != 0 && *src != '\n')
				src++;
		}
	}
}

void CHudStatusBar::MsgFunc_StatusText(int& iLine, const char* szText)
{
	if (iLine < 0 || iLine > MAX_STATUSBAR_LINES)
		return;

	Q_strncpy(m_szStatusText[iLine], szText, MAX_STATUSTEXT_LENGTH);
	m_szStatusText[iLine][MAX_STATUSTEXT_LENGTH - 1] = 0;

	m_bitsFlags |= HUD_ACTIVE;
	m_bReparseString = TRUE;
}

void CHudStatusBar::MsgFunc_StatusValue(int& iIndex, int& iValue)
{
	if (iIndex < 1 || iIndex > MAX_STATUSBAR_VALUES)
		return;

	m_iStatusValues[iIndex] = iValue;
	m_bReparseString = TRUE;
}
