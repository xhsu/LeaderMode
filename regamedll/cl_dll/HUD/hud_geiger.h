/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudGeiger : public CBaseHudElement
{
public:
	int Init(void);
	int Draw(float flTime);

	void MsgFunc_Geiger(int& value);

private:
	int m_iGeigerRange;
};
