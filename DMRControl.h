/*
 *   Copyright (C) 2026 by Thomas A. Early N7TAE
 *   Copyright (C) 2015-2020 by Jonathan Naylor G4KLX
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

#include "RSSIInterpolator.h"
#include "DMRNetwork.h"
#include "DMRLookup.h"
#include "DMRSlot.h"
#include "DMRData.h"
#include "DashDB.h"
#include "Modem.h"

#include <vector>
#include <cstdint>

class CDMRControl
{
public:
	CDMRControl(unsigned id, unsigned colorCode, unsigned callHang, bool selfOnly, bool embeddedLCOnly, bool dumpTAData, const std::vector<unsigned>& prefixes, const std::vector<unsigned>& blacklist, const std::vector<unsigned>& whitelist, const std::vector<unsigned>& slot1TGWhitelist, const std::vector<unsigned>& slot2TGWhitelist, unsigned timeout, CModem* modem, IDMRNetwork* network, bool duplex, CDMRLookup* lookup, CRSSIInterpolator* rssi, unsigned jitter, DMR_OVCM_TYPES ovcm, CDashDB *dashDB);
	~CDMRControl();

	bool processWakeup(const uint8_t *data);

	bool writeModemSlot1(uint8_t *data, unsigned len);
	bool writeModemSlot2(uint8_t *data, unsigned len);

	unsigned readModemSlot1(uint8_t *data);
	unsigned readModemSlot2(uint8_t *data);

	void clock();

	bool isBusy() const;

	void enable(bool enabled);

private:
	unsigned m_colorCode;
	CModem*      m_modem;
	IDMRNetwork* m_network;
	CDMRSlot     m_slot1;
	CDMRSlot     m_slot2;
	CDMRLookup*  m_lookup;
	CDashDB*     m_dashDB;
};
