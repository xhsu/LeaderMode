/*

Created Date: Mar 11 2020

*/

#pragma once

#define TEXTCOLOR_NORMAL		1
#define TEXTCOLOR_USEOLDCOLORS	2
#define TEXTCOLOR_PLAYERNAME	3
#define TEXTCOLOR_LOCATION		4

class CHudSayText : public CBaseHudElement
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
	static void GetColorFromText(int client_index, const std::wstring& wcsColorText, Vector& vecColor);

public:
	cvar_s* m_HUD_saytext;
	cvar_s* m_HUD_saytext_time;

private:
	class CSayTextSegment
	{
	public:
		CSayTextSegment(const Vector& color, const std::wstring& text) : m_vecColor(color), m_wcsText(text) {}

	public:
		Vector m_vecColor{ g_vecZero };
		std::wstring m_wcsText{ L"" };
	};

	// a set of segments make up a single linedata.
	typedef std::list<CSayTextSegment> linedata_t;

	class CSayTextLineData
	{
	public:
		CSayTextLineData(const linedata_t& linedata) : m_lstLineData(linedata) {}

	public:
		linedata_t m_lstLineData;
		float m_flTimeCreated{ gHUD::m_flUCDTime };
	};

	// a list of linedatas make up all the contents.
	std::list<CSayTextLineData> m_lstContents;
};
