/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudTrain : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);

	void MsgFunc_Train(int& iStatus);

private:
	hSprite m_hSprite;
	int m_iPos;
};
