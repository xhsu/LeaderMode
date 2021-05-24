/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#ifndef NETADR_H
#define NETADR_H

#ifdef _WIN32
#pragma once
#endif

#ifdef _WIN32
#pragma once
#endif

#include "steam/steamtypes.h"

#ifdef SetPort
#undef SetPort
#endif

typedef enum
{
	NA_UNUSED,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX,
}
netadrtype_t;

typedef struct netadr_s
{
public:
	netadr_s(void) { SetIP(0); SetPort(0); SetType(NA_IP); }
	netadr_s(uint32 unIP, uint16 usPort) { SetIP(unIP); SetPort(usPort); SetType(NA_IP); }
	netadr_s(const char* pch) { SetFromString(pch); }

public:
	void Clear(void);

	void SetType(netadrtype_t type);
	void SetPort(unsigned short port);
	bool SetFromSockadr(const struct sockaddr* s);
	void SetIP(uint8 b1, uint8 b2, uint8 b3, uint8 b4);
	void SetIP(uint32 unIP);
	void SetIPAndPort(uint32 unIP, unsigned short usPort) { SetIP(unIP); SetPort(usPort); }
	void SetFromString(const char* pch, bool bUseDNS = false);

	bool CompareAdr(const netadr_s& a, bool onlyBase = false) const;
	bool CompareClassBAdr(const netadr_s& a) const;
	bool CompareClassCAdr(const netadr_s& a) const;

	netadrtype_t GetType(void) const;
	unsigned short GetPort(void) const;
	const char* ToString(bool onlyBase = false) const;
	void ToSockadr(struct sockaddr* s) const;
	unsigned int GetIP(void) const;

	bool IsLocalhost(void) const;
	bool IsLoopback(void) const;
	bool IsReservedAdr(void) const;
	bool IsValid(void) const;
	void SetFromSocket(int hSocket);

	unsigned long addr_ntohl(void) const;
	unsigned long addr_htonl(void) const;
	bool operator == (const netadr_s& netadr) const { return (CompareAdr(netadr)); }
	bool operator < (const netadr_s& netadr) const;

public:
	netadrtype_t type;
	unsigned char ip[4];
	unsigned char ipx[10];
	unsigned short port;
}
netadr_t;

#endif // NETADR_H
