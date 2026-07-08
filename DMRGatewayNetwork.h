/*
 *   Copyright (C) 2026 by Thomas A. Early N7TAE
 *   Copyright (C) 2015,2016,2017,2018,2020 by Jonathan Naylor G4KLX
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

#include "DMRNetwork.h"
#include "UDPSocket.h"
#include "Timer.h"
#include "RingBuffer.h"
#include "DMRData.h"
#include "Defines.h"

#include <string>
#include <cstdint>
#include <random>

class CDMRGatewayNetwork : public IDMRNetwork
{
public:
	CDMRGatewayNetwork(const std::string &address, unsigned port, unsigned local, unsigned id, bool duplex, const char *version, bool slot1, bool slot2, HW_TYPE hwType, bool debug);
	virtual ~CDMRGatewayNetwork();

	virtual void setOptions(const std::string &options);

	virtual void setConfig(const std::string &callsign, unsigned rxFrequency, unsigned txFrequency, unsigned power, unsigned colorCode, float latitude, float longitude, int height, const std::string &location, const std::string &description, const std::string &url);

	virtual bool open();

	virtual void enable(bool enabled);

	virtual bool read(CDMRData& data);

	virtual bool write(const CDMRData& data);

	virtual bool writeRadioPosition(unsigned id, const uint8_t *data);

	virtual bool writeTalkerAlias(unsigned id, uint8_t type, const uint8_t *data);

	virtual bool wantsBeacon();

	virtual void clock(unsigned ms);

	virtual void close();

private:
	std::string      m_addressStr;
	sockaddr_storage m_addr;
	unsigned     m_addrLen;
	unsigned     m_port;
	uint8_t *        m_id;
	bool             m_duplex;
	const char *     m_version;
	bool             m_debug;
	CUDPSocket       m_socket;
	bool             m_enabled;
	bool             m_slot1;
	bool             m_slot2;
	HW_TYPE          m_hwType;
	uint8_t *  m_buffer;
	uint32_t*        m_streamId;
	CRingBuffer<uint8_t> m_rxData;
	bool             m_beacon;
	std::mt19937     m_random;
	std::string      m_callsign;
	unsigned     m_rxFrequency;
	unsigned     m_txFrequency;
	unsigned     m_power;
	unsigned     m_colorCode;
	CTimer           m_pingTimer;

	bool writeConfig();

	bool write(const uint8_t *data, unsigned length);
};
