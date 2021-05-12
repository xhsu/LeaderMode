/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudStatusBar : public CBaseHudElement
{
public:
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
	void ParseStatusString(int line_num);

	void MsgFunc_StatusText(int& iLine, const char* szText);
	void MsgFunc_StatusValue(int& iIndex, int& iValue);

protected:
	enum
	{
		MAX_STATUSTEXT_LENGTH = 128,
		MAX_STATUSBAR_VALUES = 8,
		MAX_STATUSBAR_LINES = 2,
	};

	char m_szStatusText[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];
	char m_szStatusBar[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];
	int m_iStatusValues[MAX_STATUSBAR_VALUES];

	int m_bReparseString;

	Vector m_rgvecNameColors[MAX_STATUSBAR_LINES];
};
