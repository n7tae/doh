/*
 *   Copyright (C) Thomas A. Early N7TAE
 *   Copyright (C) 2009-2011,2013,2015,2016,2020 by Jonathan Naylor G4KLX
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

#pragma once
#include <string>

#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#ifndef UDP_SOCKET_MAX
#define UDP_SOCKET_MAX	1
#endif

enum IPMATCHTYPE
{
	IMT_ADDRESS_AND_PORT,
	IMT_ADDRESS_ONLY
};

class CUDPSocket
{
public:
	CUDPSocket(const std::string &address, unsigned port = 0U);
	CUDPSocket(unsigned port = 0U);
	~CUDPSocket();

	bool open(unsigned af = AF_UNSPEC);
	bool open(const sockaddr_storage& address);
	bool open(const unsigned index, const unsigned af, const std::string &address, const unsigned port);

	int  read(uint8_t *buffer, unsigned length, sockaddr_storage& address, unsigned &address_length);
	bool write(const uint8_t *buffer, unsigned length, const sockaddr_storage& address, unsigned address_length);

	void close();
	void close(const unsigned index);

	static void startup();
	static void shutdown();

	static int lookup(const std::string &hostName, unsigned port, sockaddr_storage& address, unsigned& address_length);
	static int lookup(const std::string &hostName, unsigned port, sockaddr_storage& address, unsigned& address_length, struct addrinfo& hints);

	static bool match(const sockaddr_storage& addr1, const sockaddr_storage& addr2, IPMATCHTYPE type = IMT_ADDRESS_AND_PORT);

	static bool isNone(const sockaddr_storage& addr);

private:
	std::string    m_address_save;
	unsigned short m_port_save;
	std::string    m_address[UDP_SOCKET_MAX];
	unsigned short m_port[UDP_SOCKET_MAX];
	unsigned   m_af[UDP_SOCKET_MAX];
	int            m_fd[UDP_SOCKET_MAX];
	unsigned   m_counter;
};
