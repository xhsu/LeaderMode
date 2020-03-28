/*

Created Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

class CBaseButton : public CBasePanel
{
public:
	//virtual	bool Initialize		(void) { return false; }
	//virtual	bool VidInit		(void) { return false; }
	//virtual	void InitHUDData	(void) {}
	//virtual	void OnNewRound		(void) {}
	virtual	bool KeyEvent		(bool bDown, int iKeyIndex, const char* pszCurrentBinding);
	virtual	void Think			(void);
	//virtual	bool Draw			(float flTime) { return false; }
	//virtual	void Shutdown		(void) {}

public:
	virtual	void	OnPlace			(void) {}
	virtual	void	OnPress			(void) {}
	virtual	void	OnRelease		(void);
	virtual	void	OnMoveAway		(void) {}
	virtual	bool	IsMouseInside	(void);
	inline	void	SetCommand		(const char* szCommand) { Q_strlcpy(m_szCommand, szCommand); }

public:
	bool	m_bInSide;
	bool	m_bClickedOn;	// don't have to release.
	char	m_szCommand[192];
};

class CBaseTextButton : public CBaseButton
{
public:
	virtual	bool	Initialize		(void);
	virtual	bool	Draw			(float flTime);

public:
	virtual	void	SetSize			(float flWidth, float flHeight);
	inline	void	SetSize			(void) { SetSize(m_flWidth, m_flHeight); }	// just a refresh/update.
	inline	void	SetNoBackground	(void) { m_Background.m_bitsFlags &= ~HUD_ACTIVE; }
	inline	void	SetBackgroundOn	(void) { m_Background.m_bitsFlags |= HUD_ACTIVE; }

public:
	CBaseText	m_Text;
	CBasePanel	m_Background;
};

class CBaseTextButton2 : public CBaseTextButton
{
public:
	virtual	bool	Draw			(float flTime);

public:
	virtual	void	SetSize			(float flWidth, float flHeight);
	virtual	void	SetGap			(float flGap);
};
