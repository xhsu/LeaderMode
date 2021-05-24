/*

Created Date: Mar 11 2020

*/

#pragma once

#define MAX_MENU_STRING 512

class CHudMenu : public CBaseHudElement
{
public:
	int Init(void);
	void InitHUDData(void);
	void Reset(void);
	int Draw(float flTime);

public:
	void MsgFunc_ShowMenu(int iSize, void* pbuf);

public:
	bool SelectMenuItem(int menu_item);

public:
	bool m_bMenuDisplayed;
	int m_bitsValidSlots;
	float m_flShutoffTime;
	bool m_bWaitingForMore;
};
