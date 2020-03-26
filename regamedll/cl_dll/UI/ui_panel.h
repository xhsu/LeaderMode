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
	CBasePanel() {}
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
	CBasePanel* m_pParent;
	vec_t			m_flWidth;
	vec_t			m_flHeight;

public:
	virtual	bool Initialize		(void);
	virtual	bool VidInit		(void);
	virtual	void InitHUDData	(void);
	virtual	void NewRound		(void);
	virtual	void Think			(void);
	virtual	bool Draw			(float flTime);
	virtual	void Shutdown		(void);

public:
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

	inline	void	SetColour	(unsigned long ulRGB)
	{
		m_ubColour.r = (ulRGB & 0xFF0000) >> 16;
		m_ubColour.g = (ulRGB & 0xFF00) >> 8;
		m_ubColour.b = ulRGB & 0xFF;

		m_flScaledColour.r = float(m_ubColour.r) / 255.0f;
		m_flScaledColour.g = float(m_ubColour.g) / 255.0f;
		m_flScaledColour.b = float(m_ubColour.b) / 255.0f;
	}

	inline	void	SetAlpha	(int a)
	{
		m_ubColour.a = a;
		m_flScaledColour.a = float(a) / 255.0f;
	}
};
