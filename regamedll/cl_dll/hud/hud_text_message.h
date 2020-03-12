/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudTextMessage : public CBaseHUDElement
{
public:
	static char* LocaliseTextString(const char* msg, char* dst_buffer, int buffer_size);
	static char* BufferedLocaliseTextString(const char* msg);

public:
	int Init(void);
	char* LookupString(const char* msg, int* msg_dest = NULL);

public:
	void MsgFunc_TextMsg(int iSize, void* pbuf);
};
