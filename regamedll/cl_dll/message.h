/*

Created Date: 08 Mar 2020

*/

#pragma once

// Quick def for .cpps.
#define HOOK_USER_MSG(x)	gEngfuncs.pfnHookUserMsg(#x, MsgFunc_##x)
#define MSG_FUNC(x)			int MsgFunc_##x(const char* pszName, int iSize, void* pbuf)

void Msg_Init(void);

// manually call some non-arguement message.
MSG_FUNC(InitHUD);
MSG_FUNC(ResetHUD);

// some sv vars came through message.
extern bool g_bHasDefuser;
extern bool g_bHasNightvision;
extern char g_szServerName[192];
