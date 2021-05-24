/*

Copied Date: May 22 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#pragma once

#include <Interface/INetAPI.h>

class CNetAPI : public INetAPI
{
public:
	virtual void NetAdrToSockAddr(netadr_t* a, struct sockaddr* s);
	virtual void SockAddrToNetAdr(struct sockaddr* s, netadr_t* a);
	virtual char* AdrToString(netadr_t* a);
	virtual bool StringToAdr(const char* s, netadr_t* a);
	virtual void GetSocketAddress(int socket, netadr_t* a);
	virtual bool CompareAdr(netadr_t* a, netadr_t* b);
	virtual void GetLocalIP(netadr_t* a);
};

