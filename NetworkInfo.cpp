/*
 *   Copyright (C) 2017 by Lieven De Samblanx ON7LDS
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "NetworkInfo.h"
#include "Log.h"

#include <cstdio>
#include <cassert>
#include <cstring>
#include <ctime>
#include <clocale>

#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

CNetworkInfo::CNetworkInfo()
{
}

CNetworkInfo::~CNetworkInfo()
{
}

void CNetworkInfo::getNetworkInterface(unsigned char* info)
{
	LogInfo("Interfaces Info");

	::strcpy((char*)info, "(address unknown)");

	char* dflt = NULL;

	FILE* fp = ::fopen("/proc/net/route" , "r");	// IPv4 routing
	if (fp == NULL) {
		LogError("Unabled to open /proc/route");
		return;
	}

	char line[100U];
	while (::fgets(line, 100U, fp)) {
		char* p1 = strtok(line , " \t");
		char* p2 = strtok(NULL , " \t");

		if (p1 != NULL && p2 != NULL) {
			if (::strcmp(p2, "00000000") == 0) {
				dflt = p1;
				break;
			}
		}
	}

	::fclose(fp);
	if (dflt == NULL) {
		LogError("Unable to find the default route");
		return;
	}

	const unsigned int IFLISTSIZ = 25U;
	char interfacelist[IFLISTSIZ][50+INET6_ADDRSTRLEN];
	for (unsigned int n = 0U; n < IFLISTSIZ; n++)
		interfacelist[n][0] = 0;

	struct ifaddrs* ifaddr;
	if (::getifaddrs(&ifaddr) == -1) {
		LogError("getifaddrs failure");
		return;
	}

	unsigned int ifnr = 0U;
	for (struct ifaddrs* ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

		int family = ifa->ifa_addr->sa_family;
		if (family == AF_INET || family == AF_INET6) {
			char host[NI_MAXHOST];
			int s = ::getnameinfo(ifa->ifa_addr, family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (s != 0) {
				LogError("getnameinfo() failed: %s\n", gai_strerror(s));
				continue;
			}

			if (family == AF_INET) {
				::sprintf(interfacelist[ifnr], "%s:%s", ifa->ifa_name, host);
				LogInfo("    IPv4: %s", interfacelist[ifnr]);
				ifnr++;
			} else {
				::sprintf(interfacelist[ifnr], "%s:%s", ifa->ifa_name, host);
				LogInfo("    IPv6: %s", interfacelist[ifnr]);
				// due to default routing is for IPv4, other
				// protocols are not candidate to display.
			}
		}
	}

	::freeifaddrs(ifaddr);

	LogInfo("    Default interface is : %s" , dflt);

	for (unsigned int n = 0U; n < ifnr; n++) {
		char* p = ::strchr(interfacelist[n], '%');
		if (p != NULL)
			*p = 0;

		if (::strstr(interfacelist[n], dflt) != 0) {
			::strcpy((char*)info, interfacelist[n]);
			break;
		}
	}

	LogInfo("    IP to show: %s", info);
}
