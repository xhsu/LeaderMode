/*

Created Date: Mar 11 2020
Reincarnation Data: Dec 01 2020

*/

#pragma once


class CHudSayText : public CBaseHudElement
{
public:
	int Init(void);
	void Reset(void);
	int Draw(float flTime);
	void Think(void);

public:
	static int GetTextPrintY(void);
	static void GetColorFromText(int client_index, const std::wstring& wcsColorText, Vector& vecColor);
	void AddToSayText(int iClientIndex, const char* pszText, const char* pszArg1 = "", const char* pszArg2 = "", const char* pszArg3 = "", const char* pszArg4 = "");
	void AddToSayText(int iClientIndex, const wchar_t* pwcsText, const wchar_t* pwcsArg1 = L"", const wchar_t* pwcsArg2 = L"", const wchar_t* pwcsArg3 = L"", const wchar_t* pwcsArg4 = L"");

public:
	cvar_s* m_pCVar_saytext;
	cvar_s* m_pCVar_saytext_time;

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
