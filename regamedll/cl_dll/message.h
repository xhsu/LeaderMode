/*

Created Date: 08 Mar 2020

*/

#pragma once

// Quick def for .cpps.
#define HOOK_USER_MSG(x)	gEngfuncs.pfnHookUserMsg(#x, MsgFunc_##x)
#define MSG_FUNC(x)			int MsgFunc_##x(const char* pszName, int iSize, void* pbuf)

void Msg_Init(void);
