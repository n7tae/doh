/*
 *   Copyright (C) 2015-2020 by Jonathan Naylor G4KLX
 *   Copyright (C) 2021 by Thomas A. Early NTAE
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
#include <vector>

#include "Defines.h"

class CConf
{
public:
	CConf(const std::string &file);

	bool Read();

	// The General section
	std::string  getCallsign() const;
	unsigned getId() const;
	unsigned getTimeout() const;
	bool         getDuplex() const;

	// The Info section
	unsigned getRXFrequency() const;
	unsigned getTXFrequency() const;
	unsigned getPower() const;
	float        getLatitude() const;
	float        getLongitude() const;
	int          getHeight() const;
	std::string  getLocation() const;
	std::string  getDescription() const;
	std::string  getURL() const;

	// The Log section
	unsigned getLogLevel() const;

	// The CW ID section
	bool         getCWIdEnabled() const;
	unsigned getCWIdTime() const;
	std::string  getCWIdCallsign() const;

	// The DMR Id section
	std::string  getDMRIdLookupFile() const;
	unsigned getDMRIdLookupTime() const;

	// The Modem section
	std::string  getModemPort() const;
	std::string  getModemProtocol() const;
	unsigned getModemAddress() const;
	bool         getModemRXInvert() const;
	bool         getModemTXInvert() const;
	bool         getModemPTTInvert() const;
	unsigned getModemTXDelay() const;
	unsigned getModemDMRDelay() const;
	int          getModemTXOffset() const;
	int          getModemRXOffset() const;
	int          getModemRXDCOffset() const;
	int          getModemTXDCOffset() const;
	float        getModemRFLevel() const;
	float        getModemRXLevel() const;
	float        getModemCWIdTXLevel() const;
	float        getModemDMRTXLevel() const;
	std::string  getModemRSSIMappingFile() const;
	bool         getModemUseCOSAsLockout() const;
	bool         getModemTrace() const;
	bool         getModemDebug() const;

	// The DMR section
	bool         getDMREnabled() const;
	DMR_BEACONS  getDMRBeacons() const;
	unsigned getDMRBeaconInterval() const;
	unsigned getDMRBeaconDuration() const;
	unsigned getDMRId() const;
	unsigned getDMRColorCode() const;
	bool         getDMREmbeddedLCOnly() const;
	bool         getDMRDumpTAData() const;
	bool         getDMRSelfOnly() const;
	std::vector<unsigned> getDMRPrefixes() const;
	std::vector<unsigned> getDMRBlackList() const;
	std::vector<unsigned> getDMRWhiteList() const;
	std::vector<unsigned> getDMRSlot1TGWhiteList() const;
	std::vector<unsigned> getDMRSlot2TGWhiteList() const;
	unsigned getDMRCallHang() const;
	unsigned getDMRTXHang() const;
	unsigned getDMRModeHang() const;
	DMR_OVCM_TYPES getDMROVCM() const;

	// The DMR Network section
	std::string  getDMRNetworkType() const;
	std::string  getDMRNetworkAddress() const;
	unsigned getDMRNetworkPort() const;
	unsigned getDMRNetworkLocal() const;
	std::string  getDMRNetworkPassword() const;
	std::string  getDMRNetworkOptions() const;
	bool         getDMRNetworkDebug() const;
	unsigned getDMRNetworkJitter() const;
	bool         getDMRNetworkSlot1() const;
	bool         getDMRNetworkSlot2() const;
	unsigned getDMRNetworkModeHang() const;

private:
	std::string  m_file;
	std::string  m_callsign;
	unsigned m_id;
	unsigned m_timeout;
	bool         m_duplex;

	unsigned m_rxFrequency;
	unsigned m_txFrequency;
	unsigned m_power;
	float        m_latitude;
	float        m_longitude;
	int          m_height;
	std::string  m_location;
	std::string  m_description;
	std::string  m_url;

	unsigned m_logLevel;

	bool         m_cwIdEnabled;
	unsigned m_cwIdTime;
	std::string  m_cwIdCallsign;

	std::string  m_dmrIdLookupFile;
	unsigned m_dmrIdLookupTime;

	std::string  m_modemPort;
	std::string  m_modemProtocol;
	unsigned m_modemAddress;
	bool         m_modemRXInvert;
	bool         m_modemTXInvert;
	bool         m_modemPTTInvert;
	unsigned m_modemTXDelay;
	unsigned m_modemDMRDelay;
	int          m_modemTXOffset;
	int          m_modemRXOffset;
	int          m_modemRXDCOffset;
	int          m_modemTXDCOffset;
	float        m_modemRFLevel;
	float        m_modemRXLevel;
	float        m_modemCWIdTXLevel;
	float        m_modemDMRTXLevel;
	std::string  m_modemRSSIMappingFile;
	bool         m_modemUseCOSAsLockout;
	bool         m_modemTrace;
	bool         m_modemDebug;

	DMR_BEACONS  m_dmrBeacons;
	unsigned m_dmrBeaconInterval;
	unsigned m_dmrBeaconDuration;
	unsigned m_dmrId;
	unsigned m_dmrColorCode;
	bool         m_dmrSelfOnly;
	bool         m_dmrEmbeddedLCOnly;
	bool         m_dmrDumpTAData;
	std::vector<unsigned> m_dmrPrefixes;
	std::vector<unsigned> m_dmrBlackList;
	std::vector<unsigned> m_dmrWhiteList;
	std::vector<unsigned> m_dmrSlot1TGWhiteList;
	std::vector<unsigned> m_dmrSlot2TGWhiteList;
	unsigned m_dmrCallHang;
	unsigned m_dmrTXHang;
	unsigned m_dmrModeHang;
	DMR_OVCM_TYPES m_dmrOVCM;

	std::string  m_dmrNetworkType;
	std::string  m_dmrNetworkAddress;
	unsigned m_dmrNetworkPort;
	unsigned m_dmrNetworkLocal;
	std::string  m_dmrNetworkPassword;
	std::string  m_dmrNetworkOptions;
	bool         m_dmrNetworkDebug;
	unsigned m_dmrNetworkJitter;
	bool         m_dmrNetworkSlot1;
	bool         m_dmrNetworkSlot2;
	unsigned m_dmrNetworkModeHang;
};
