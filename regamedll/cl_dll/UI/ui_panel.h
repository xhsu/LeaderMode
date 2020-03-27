/*

Created Date: Mar 26 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

class CBasePanel
{
public:
	// avoid the complex memset();
	void* operator new(size_t size)
	{
		return calloc(1, size);
	}
	void operator delete(void* ptr)
	{
		free(ptr);
	}
	CBasePanel();
	virtual ~CBasePanel();

public:
	typedef std::list<CBasePanel*> elements_t;
	typedef struct { byte r, g, b, a; } ubcolour_t;
	typedef struct { float r, g, b, a; } flcolour_t;

public:
	ubcolour_t		m_ubColour;
	flcolour_t		m_flScaledColour;
	Vector2D		m_vecCoord;
	elements_t		m_lstChildren;
	CBasePanel*		m_pParent;
	vec_t			m_flWidth;
	vec_t			m_flHeight;
	int				m_bitsFlags;

public:	// events
	virtual	bool	Initialize		(void);
	virtual	bool	VidInit			(void);
	virtual	void	InitHUDData		(void);
	virtual	void	OnNewRound		(void);
	virtual bool	KeyEvent		(bool bDown, int iKeyIndex, const char* pszCurrentBinding);	// Return true to allow engine to process the key, otherwise, act on it as needed
	virtual	void	Think			(void);
	virtual	bool	Draw			(float flTime);
	virtual	void	Shutdown		(void);

public:	// utils
	inline	void	SetColour	(int r, int g, int b, int a)
	{
		m_ubColour.r = r;
		m_flScaledColour.r = float(r) / 255.0f;
		m_ubColour.g = g;
		m_flScaledColour.g = float(g) / 255.0f;
		m_ubColour.b = b;
		m_flScaledColour.b = float(b) / 255.0f;
		m_ubColour.a = a;
		m_flScaledColour.a = float(a) / 255.0f;
	}

	inline	void	SetColour	(int r, int g, int b)
	{
		m_ubColour.r = r;
		m_flScaledColour.r = float(r) / 255.0f;
		m_ubColour.g = g;
		m_flScaledColour.g = float(g) / 255.0f;
		m_ubColour.b = b;
		m_flScaledColour.b = float(b) / 255.0f;
	}

	inline	void	SetColour	(unsigned long ulRGB)
	{
		m_ubColour.r = (ulRGB & 0xFF0000) >> 16;
		m_ubColour.g = (ulRGB & 0xFF00) >> 8;
		m_ubColour.b = ulRGB & 0xFF;

		m_flScaledColour.r = float(m_ubColour.r) / 255.0f;
		m_flScaledColour.g = float(m_ubColour.g) / 255.0f;
		m_flScaledColour.b = float(m_ubColour.b) / 255.0f;
	}

	inline	void	SetColour	(unsigned long ulRGB, int a)
	{
		SetColour(ulRGB);
		SetAlpha(a);
	}

	inline	void	SetAlpha	(int a)
	{
		m_ubColour.a = a;
		m_flScaledColour.a = float(a) / 255.0f;
	}

	inline	vec_t	GetX		(void)
	{
		if (m_pParent)
			return m_pParent->GetX() + m_vecCoord.x;

		return m_vecCoord.x;
	}

	inline	vec_t	GetY		(void)
	{
		if (m_pParent)
			return m_pParent->GetY() + m_vecCoord.y;

		return m_vecCoord.y;
	}

	inline	bool	AddChild	(CBasePanel* pChild)
	{
		for (auto pElem : m_lstChildren)
		{
			if (pElem == pChild)
				return false;	// already in the list.
		}

		pChild->m_pParent = this;
		m_lstChildren.push_back(pChild);
		return true;
	}
};

/*
class CBaseExample : public CBasePanel
{
public:
	virtual	bool Initialize		(void) { return false; }
	virtual	bool VidInit		(void) { return false; }
	virtual	void InitHUDData	(void) {}
	virtual	void OnNewRound		(void) {}
	virtual bool KeyEvent		(bool bDown, int iKeyIndex, const char* pszCurrentBinding)	{ return true; }
	virtual	void Think			(void) {}
	virtual	bool Draw			(float flTime) { return false; }
	virtual	void Shutdown		(void) {}
};
*/
