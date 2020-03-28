/*

Created Date: Mar 26 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

class CBaseText : public CBasePanel
{
public:
	//virtual	bool Initialize		(void) { return false; }
	//virtual	bool VidInit		(void) { return false; }
	//virtual	void InitHUDData	(void) {}
	//virtual	void OnNewRound		(void) {}
	//virtual	bool KeyEvent		(bool bDown, int iKeyIndex, const char* pszCurrentBinding) { return true; }
	//virtual	void Think			(void) {}
	virtual	bool Draw			(float flTime);
	//virtual	void Shutdown		(void) {}

public:	// utils
	void	SetUFT8				(const char* str);
	void	SetANSI				(const char* str);

	virtual	void	SetLocalise	(const char* key);
	virtual	int		CreateFont	(const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	inline	void	SetFont		(int hFont, int iSize) { m_hFont = hFont; m_iFontSize = iSize; }

public:
	wchar_t	m_wszWords[2048];
	int		m_hFont;
	int		m_iFontSize;
};
