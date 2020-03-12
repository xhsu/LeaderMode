/*

Created Date: Mar 11 2020

*/

#pragma once

#define TEXTCOLOR_NORMAL		1
#define TEXTCOLOR_USEOLDCOLORS	2
#define TEXTCOLOR_PLAYERNAME	3
#define TEXTCOLOR_LOCATION		4

class CHudSayText : public CBaseHUDElement
{
public:
	int Init(void);
	void InitHUDData(void);
	int Draw(float flTime);

public:
	void MsgFunc_SayText(int iSize, void* pbuf);

public:
	int GetTextPrintY(void);
	void SayTextPrint(const char* pszBuf, int iBufSize, int clientIndex = -1, char* sstr1 = NULL, char* sstr2 = NULL, char* sstr3 = NULL, char* sstr4 = NULL);
	void EnsureTextFitsInOneLineAndWrapIfHaveTo(int line);

public:
	friend class CHudSpectator;

private:
	cvar_s* m_HUD_saytext;
	cvar_s* m_HUD_saytext_time;
};
