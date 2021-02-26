/*
 *   Copyright (C) 2015-2020 by Jonathan Naylor G4KLX
 *   Copyright (C) 2021 by Thomas A. Early N7TAE
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

#include "Conf.h"
#include "Log.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

const int BUFFER_SIZE = 500;

enum SECTION
{
	SECTION_NONE,
	SECTION_GENERAL,
	SECTION_INFO,
	SECTION_LOG,
	SECTION_CWID,
	SECTION_DMRID_LOOKUP,
	SECTION_MODEM,
	SECTION_DMR,
	SECTION_DMR_NETWORK
};

CConf::CConf(const std::string& file) :
	m_file(file),
	m_callsign(),
	m_id(0U),
	m_timeout(120U),
	m_duplex(true),
	m_display(),
	m_rxFrequency(0U),
	m_txFrequency(0U),
	m_power(0U),
	m_latitude(0.0F),
	m_longitude(0.0F),
	m_height(0),
	m_location(),
	m_description(),
	m_url(),
	m_logLevel(2U),
	m_cwIdEnabled(false),
	m_cwIdTime(10U),
	m_cwIdCallsign(),
	m_dmrIdLookupFile(),
	m_dmrIdLookupTime(0U),
	m_modemPort(),
	m_modemProtocol("uart"),
	m_modemAddress(0x22),
	m_modemRXInvert(false),
	m_modemTXInvert(false),
	m_modemPTTInvert(false),
	m_modemTXDelay(100U),
	m_modemDMRDelay(0U),
	m_modemTXOffset(0),
	m_modemRXOffset(0),
	m_modemRXDCOffset(0),
	m_modemTXDCOffset(0),
	m_modemRFLevel(100.0F),
	m_modemRXLevel(50.0F),
	m_modemCWIdTXLevel(50.0F),
	m_modemDMRTXLevel(50.0F),
	m_modemRSSIMappingFile(),
	m_modemUseCOSAsLockout(false),
	m_modemTrace(false),
	m_modemDebug(false),
	m_dmrBeacons(DMR_BEACONS_OFF),
	m_dmrBeaconInterval(60U),
	m_dmrBeaconDuration(3U),
	m_dmrId(0U),
	m_dmrColorCode(2U),
	m_dmrSelfOnly(false),
	m_dmrEmbeddedLCOnly(false),
	m_dmrDumpTAData(true),
	m_dmrPrefixes(),
	m_dmrBlackList(),
	m_dmrWhiteList(),
	m_dmrSlot1TGWhiteList(),
	m_dmrSlot2TGWhiteList(),
	m_dmrCallHang(10U),
	m_dmrTXHang(4U),
	m_dmrModeHang(10U),
	m_dmrOVCM(DMR_OVCM_OFF),
	m_dmrNetworkType("Gateway"),
	m_dmrNetworkAddress(),
	m_dmrNetworkPort(0U),
	m_dmrNetworkLocal(0U),
	m_dmrNetworkPassword(),
	m_dmrNetworkOptions(),
	m_dmrNetworkDebug(false),
	m_dmrNetworkJitter(360U),
	m_dmrNetworkSlot1(true),
	m_dmrNetworkSlot2(true),
	m_dmrNetworkModeHang(3U),
	m_tftSerialPort("/dev/ttyAMA0"),
	m_tftSerialBrightness(50U)
{
}

bool CConf::Read()
{
	FILE* fp = ::fopen(m_file.c_str(), "rt");
	if (fp == NULL)
	{
		::fprintf(stderr, "Couldn't open the .ini file - %s\n", m_file.c_str());
		return false;
	}

	SECTION section = SECTION_NONE;

	char buffer[BUFFER_SIZE];
	while (::fgets(buffer, BUFFER_SIZE, fp) != NULL)
	{
		if (buffer[0U] == '#')
			continue;

		if (buffer[0U] == '[')
		{
			if (::strncmp(buffer, "[General]", 9U) == 0)
				section = SECTION_GENERAL;
			else if (::strncmp(buffer, "[Info]", 6U) == 0)
				section = SECTION_INFO;
			else if (::strncmp(buffer, "[Log]", 5U) == 0)
				section = SECTION_LOG;
			else if (::strncmp(buffer, "[CW Id]", 7U) == 0)
				section = SECTION_CWID;
			else if (::strncmp(buffer, "[DMR Id Lookup]", 15U) == 0)
				section = SECTION_DMRID_LOOKUP;
			else if (::strncmp(buffer, "[Modem]", 7U) == 0)
				section = SECTION_MODEM;
			else if (::strncmp(buffer, "[DMR]", 5U) == 0)
				section = SECTION_DMR;
			else if (::strncmp(buffer, "[DMR Network]", 13U) == 0)
				section = SECTION_DMR_NETWORK;
			else
				section = SECTION_NONE;

			continue;
		}

		char* key   = ::strtok(buffer, " \t=\r\n");
		if (key == NULL)
			continue;

		char* value = ::strtok(NULL, "\r\n");
		if (value == NULL)
			continue;

		// Remove quotes from the value
		size_t len = ::strlen(value);
		if (len > 1U && *value == '"' && value[len - 1U] == '"')
		{
			value[len - 1U] = '\0';
			value++;
		}
		else
		{
			char *p;

			// if value is not quoted, remove after # (to make comment)
			if ((p = strchr(value, '#')) != NULL)
				*p = '\0';

			// remove trailing tab/space
			for (p = value + strlen(value) - 1;
					p >= value && (*p == '\t' || *p == ' '); p--)
				*p = '\0';
		}

		if (section == SECTION_GENERAL)
		{
			if (::strcmp(key, "Callsign") == 0)
			{
				// Convert the callsign to upper case
				for (unsigned int i = 0U; value[i] != 0; i++)
					value[i] = ::toupper(value[i]);
				m_cwIdCallsign = m_callsign = value;
			}
			else if (::strcmp(key, "Id") == 0)
				m_id = m_dmrId = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Timeout") == 0)
				m_timeout = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Duplex") == 0)
				m_duplex = ::atoi(value) == 1;
			else if (::strcmp(key, "ModeHang") == 0)
				m_dmrNetworkModeHang = m_dmrModeHang = (unsigned int)::atoi(value);
			else if (::strcmp(key, "RFModeHang") == 0)
				m_dmrModeHang = (unsigned int)::atoi(value);
			else if (::strcmp(key, "NetModeHang") == 0)
				m_dmrNetworkModeHang = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Display") == 0)
				m_display = value;
		}
		else if (section == SECTION_INFO)
		{
			if (::strcmp(key, "TXFrequency") == 0)
				m_txFrequency = (unsigned int)::atoi(value);
			else if (::strcmp(key, "RXFrequency") == 0)
				m_rxFrequency = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Power") == 0)
				m_power = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Latitude") == 0)
				m_latitude = float(::atof(value));
			else if (::strcmp(key, "Longitude") == 0)
				m_longitude = float(::atof(value));
			else if (::strcmp(key, "Height") == 0)
				m_height = ::atoi(value);
			else if (::strcmp(key, "Location") == 0)
				m_location = value;
			else if (::strcmp(key, "Description") == 0)
				m_description = value;
			else if (::strcmp(key, "URL") == 0)
				m_url = value;
		}
		else if (section == SECTION_LOG)
		{
			if (::strcmp(key, "LogLevel") == 0)
				m_logLevel = ::atoi(value);
		}
		else if (section == SECTION_CWID)
		{
			if (::strcmp(key, "Enable") == 0)
				m_cwIdEnabled = ::atoi(value) == 1;
			else if (::strcmp(key, "Time") == 0)
				m_cwIdTime = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Callsign") == 0)
			{
				// Convert the callsign to upper case
				for (unsigned int i = 0U; value[i] != 0; i++)
					value[i] = ::toupper(value[i]);
				m_cwIdCallsign = value;
			}
		}
		else if (section == SECTION_DMRID_LOOKUP)
		{
			if (::strcmp(key, "File") == 0)
				m_dmrIdLookupFile = value;
			else if (::strcmp(key, "Time") == 0)
				m_dmrIdLookupTime = (unsigned int)::atoi(value);
		}
		else if (section == SECTION_MODEM)
		{
			if (::strcmp(key, "Port") == 0)
				m_modemPort = value;
			else if (::strcmp(key, "Protocol") == 0)
				m_modemProtocol = value;
			else if (::strcmp(key, "Address") == 0)
				m_modemAddress = (unsigned int)::strtoul(value, NULL, 16);
			else if (::strcmp(key, "RXInvert") == 0)
				m_modemRXInvert = ::atoi(value) == 1;
			else if (::strcmp(key, "TXInvert") == 0)
				m_modemTXInvert = ::atoi(value) == 1;
			else if (::strcmp(key, "PTTInvert") == 0)
				m_modemPTTInvert = ::atoi(value) == 1;
			else if (::strcmp(key, "TXDelay") == 0)
				m_modemTXDelay = (unsigned int)::atoi(value);
			else if (::strcmp(key, "DMRDelay") == 0)
				m_modemDMRDelay = (unsigned int)::atoi(value);
			else if (::strcmp(key, "RXOffset") == 0)
				m_modemRXOffset = ::atoi(value);
			else if (::strcmp(key, "TXOffset") == 0)
				m_modemTXOffset = ::atoi(value);
			else if (::strcmp(key, "RXDCOffset") == 0)
				m_modemRXDCOffset = ::atoi(value);
			else if (::strcmp(key, "TXDCOffset") == 0)
				m_modemTXDCOffset = ::atoi(value);
			else if (::strcmp(key, "RFLevel") == 0)
				m_modemRFLevel = float(::atof(value));
			else if (::strcmp(key, "RXLevel") == 0)
				m_modemRXLevel = float(::atof(value));
			else if (::strcmp(key, "TXLevel") == 0)
				m_modemCWIdTXLevel = m_modemDMRTXLevel = float(::atof(value));
			else if (::strcmp(key, "CWIdTXLevel") == 0)
				m_modemCWIdTXLevel = float(::atof(value));
			else if (::strcmp(key, "DMRTXLevel") == 0)
				m_modemDMRTXLevel = float(::atof(value));
			else if (::strcmp(key, "RSSIMappingFile") == 0)
				m_modemRSSIMappingFile = value;
			else if (::strcmp(key, "UseCOSAsLockout") == 0)
				m_modemUseCOSAsLockout = ::atoi(value) == 1;
			else if (::strcmp(key, "Trace") == 0)
				m_modemTrace = ::atoi(value) == 1;
			else if (::strcmp(key, "Debug") == 0)
				m_modemDebug = ::atoi(value) == 1;
		}
		else if (section == SECTION_DMR)
		{
			if (::strcmp(key, "Beacons") == 0)
				m_dmrBeacons = ::atoi(value) == 1 ? DMR_BEACONS_NETWORK : DMR_BEACONS_OFF;
			else if (::strcmp(key, "BeaconInterval") == 0)
			{
				m_dmrBeacons = m_dmrBeacons != DMR_BEACONS_OFF ? DMR_BEACONS_TIMED : DMR_BEACONS_OFF;
				m_dmrBeaconInterval = (unsigned int)::atoi(value);
			}
			else if (::strcmp(key, "BeaconDuration") == 0)
				m_dmrBeaconDuration = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Id") == 0)
				m_dmrId = (unsigned int)::atoi(value);
			else if (::strcmp(key, "ColorCode") == 0)
				m_dmrColorCode = (unsigned int)::atoi(value);
			else if (::strcmp(key, "SelfOnly") == 0)
				m_dmrSelfOnly = ::atoi(value) == 1;
			else if (::strcmp(key, "EmbeddedLCOnly") == 0)
				m_dmrEmbeddedLCOnly = ::atoi(value) == 1;
			else if (::strcmp(key, "DumpTAData") == 0)
				m_dmrDumpTAData = ::atoi(value) == 1;
			else if (::strcmp(key, "Prefixes") == 0)
			{
				char* p = ::strtok(value, ",\r\n");
				while (p != NULL)
				{
					unsigned int prefix = (unsigned int)::atoi(p);
					if (prefix > 0U && prefix <= 999U)
						m_dmrPrefixes.push_back(prefix);
					p = ::strtok(NULL, ",\r\n");
				}
			}
			else if (::strcmp(key, "BlackList") == 0)
			{
				char* p = ::strtok(value, ",\r\n");
				while (p != NULL)
				{
					unsigned int id = (unsigned int)::atoi(p);
					if (id > 0U)
						m_dmrBlackList.push_back(id);
					p = ::strtok(NULL, ",\r\n");
				}
			}
			else if (::strcmp(key, "WhiteList") == 0)
			{
				char* p = ::strtok(value, ",\r\n");
				while (p != NULL)
				{
					unsigned int id = (unsigned int)::atoi(p);
					if (id > 0U)
						m_dmrWhiteList.push_back(id);
					p = ::strtok(NULL, ",\r\n");
				}
			}
			else if (::strcmp(key, "Slot1TGWhiteList") == 0)
			{
				char* p = ::strtok(value, ",\r\n");
				while (p != NULL)
				{
					unsigned int id = (unsigned int)::atoi(p);
					if (id > 0U)
						m_dmrSlot1TGWhiteList.push_back(id);
					p = ::strtok(NULL, ",\r\n");
				}
			}
			else if (::strcmp(key, "Slot2TGWhiteList") == 0)
			{
				char* p = ::strtok(value, ",\r\n");
				while (p != NULL)
				{
					unsigned int id = (unsigned int)::atoi(p);
					if (id > 0U)
						m_dmrSlot2TGWhiteList.push_back(id);
					p = ::strtok(NULL, ",\r\n");
				}
			}
			else if (::strcmp(key, "TXHang") == 0)
				m_dmrTXHang = (unsigned int)::atoi(value);
			else if (::strcmp(key, "CallHang") == 0)
				m_dmrCallHang = (unsigned int)::atoi(value);
			else if (::strcmp(key, "ModeHang") == 0)
				m_dmrModeHang = (unsigned int)::atoi(value);
			else if (::strcmp(key, "OVCM") == 0)
				switch(::atoi(value))
				{
				case 1:
					m_dmrOVCM = DMR_OVCM_RX_ON;
					break;
				case 2:
					m_dmrOVCM = DMR_OVCM_TX_ON;
					break;
				case 3:
					m_dmrOVCM = DMR_OVCM_ON;
					break;
				default:
					m_dmrOVCM = DMR_OVCM_OFF;
					break;
				}
		}
		else if (section == SECTION_DMR_NETWORK)
		{
			if (::strcmp(key, "Type") == 0)
				m_dmrNetworkType = value;
			else if (::strcmp(key, "Address") == 0)
				m_dmrNetworkAddress = value;
			else if (::strcmp(key, "Port") == 0)
				m_dmrNetworkPort = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Local") == 0)
				m_dmrNetworkLocal = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Password") == 0)
				m_dmrNetworkPassword = value;
			else if (::strcmp(key, "Options") == 0)
				m_dmrNetworkOptions = value;
			else if (::strcmp(key, "Debug") == 0)
				m_dmrNetworkDebug = ::atoi(value) == 1;
			else if (::strcmp(key, "Jitter") == 0)
				m_dmrNetworkJitter = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Slot1") == 0)
				m_dmrNetworkSlot1 = ::atoi(value) == 1;
			else if (::strcmp(key, "Slot2") == 0)
				m_dmrNetworkSlot2 = ::atoi(value) == 1;
			else if (::strcmp(key, "ModeHang") == 0)
				m_dmrNetworkModeHang = (unsigned int)::atoi(value);
		}
	}

	::fclose(fp);

	return true;
}

std::string CConf::getCallsign() const
{
	return m_callsign;
}

unsigned int CConf::getId() const
{
	return m_id;
}

unsigned int CConf::getTimeout() const
{
	return m_timeout;
}

bool CConf::getDuplex() const
{
	return m_duplex;
}

std::string CConf::getDisplay() const
{
	return m_display;
}

unsigned int CConf::getRXFrequency() const
{
	return m_rxFrequency;
}

unsigned int CConf::getTXFrequency() const
{
	return m_txFrequency;
}

unsigned int CConf::getPower() const
{
	return m_power;
}

float CConf::getLatitude() const
{
	return m_latitude;
}

float CConf::getLongitude() const
{
	return m_longitude;
}

int CConf::getHeight() const
{
	return m_height;
}

std::string CConf::getLocation() const
{
	return m_location;
}

std::string CConf::getDescription() const
{
	return m_description;
}

std::string CConf::getURL() const
{
	return m_url;
}

unsigned int CConf::getLogLevel() const
{
	return m_logLevel;
}

bool CConf::getCWIdEnabled() const
{
	return m_cwIdEnabled;
}

unsigned int CConf::getCWIdTime() const
{
	return m_cwIdTime;
}

std::string CConf::getCWIdCallsign() const
{
	return m_cwIdCallsign;
}

std::string CConf::getDMRIdLookupFile() const
{
	return m_dmrIdLookupFile;
}

unsigned int CConf::getDMRIdLookupTime() const
{
	return m_dmrIdLookupTime;
}

std::string CConf::getModemPort() const
{
	return m_modemPort;
}

std::string CConf::getModemProtocol() const
{
	return m_modemProtocol;
}

unsigned int CConf::getModemAddress() const
{
	return m_modemAddress;
}

bool CConf::getModemRXInvert() const
{
	return m_modemRXInvert;
}

bool CConf::getModemTXInvert() const
{
	return m_modemTXInvert;
}

bool CConf::getModemPTTInvert() const
{
	return m_modemPTTInvert;
}

unsigned int CConf::getModemTXDelay() const
{
	return m_modemTXDelay;
}

unsigned int CConf::getModemDMRDelay() const
{
	return m_modemDMRDelay;
}

int CConf::getModemRXOffset() const
{
	return m_modemRXOffset;
}

int CConf::getModemTXOffset() const
{
	return m_modemTXOffset;
}

int CConf::getModemRXDCOffset() const
{
	return m_modemRXDCOffset;
}

int CConf::getModemTXDCOffset() const
{
	return m_modemTXDCOffset;
}

float CConf::getModemRFLevel() const
{
	return m_modemRFLevel;
}

float CConf::getModemRXLevel() const
{
	return m_modemRXLevel;
}

float CConf::getModemCWIdTXLevel() const
{
	return m_modemCWIdTXLevel;
}

float CConf::getModemDMRTXLevel() const
{
	return m_modemDMRTXLevel;
}

std::string CConf::getModemRSSIMappingFile () const
{
	return m_modemRSSIMappingFile;
}

bool CConf::getModemUseCOSAsLockout() const
{
	return m_modemUseCOSAsLockout;
}

bool CConf::getModemTrace() const
{
	return m_modemTrace;
}

bool CConf::getModemDebug() const
{
	return m_modemDebug;
}

DMR_BEACONS CConf::getDMRBeacons() const
{
	return m_dmrBeacons;
}

unsigned int CConf::getDMRBeaconInterval() const
{
	return m_dmrBeaconInterval;
}

unsigned int CConf::getDMRBeaconDuration() const
{
	return m_dmrBeaconDuration;
}

unsigned int CConf::getDMRId() const
{
	return m_dmrId;
}

unsigned int CConf::getDMRColorCode() const
{
	return m_dmrColorCode;
}

bool CConf::getDMRSelfOnly() const
{
	return m_dmrSelfOnly;
}

bool CConf::getDMREmbeddedLCOnly() const
{
	return m_dmrEmbeddedLCOnly;
}

bool CConf::getDMRDumpTAData() const
{
	return m_dmrDumpTAData;
}

std::vector<unsigned int> CConf::getDMRPrefixes() const
{
	return m_dmrPrefixes;
}

std::vector<unsigned int> CConf::getDMRBlackList() const
{
	return m_dmrBlackList;
}

std::vector<unsigned int> CConf::getDMRWhiteList() const
{
	return m_dmrWhiteList;
}

std::vector<unsigned int> CConf::getDMRSlot1TGWhiteList() const
{
	return m_dmrSlot1TGWhiteList;
}

std::vector<unsigned int> CConf::getDMRSlot2TGWhiteList() const
{
	return m_dmrSlot2TGWhiteList;
}

unsigned int CConf::getDMRCallHang() const
{
	return m_dmrCallHang;
}

unsigned int CConf::getDMRTXHang() const
{
	return m_dmrTXHang;
}

unsigned int CConf::getDMRModeHang() const
{
	return m_dmrModeHang;
}

DMR_OVCM_TYPES CConf::getDMROVCM() const
{
	return m_dmrOVCM;
}

std::string CConf::getDMRNetworkType() const
{
	return m_dmrNetworkType;
}

std::string CConf::getDMRNetworkAddress() const
{
	return m_dmrNetworkAddress;
}

unsigned int CConf::getDMRNetworkPort() const
{
	return m_dmrNetworkPort;
}

unsigned int CConf::getDMRNetworkLocal() const
{
	return m_dmrNetworkLocal;
}

std::string CConf::getDMRNetworkPassword() const
{
	return m_dmrNetworkPassword;
}

std::string CConf::getDMRNetworkOptions() const
{
	return m_dmrNetworkOptions;
}

unsigned int CConf::getDMRNetworkModeHang() const
{
	return m_dmrNetworkModeHang;
}

bool CConf::getDMRNetworkDebug() const
{
	return m_dmrNetworkDebug;
}

unsigned int CConf::getDMRNetworkJitter() const
{
	return m_dmrNetworkJitter;
}

bool CConf::getDMRNetworkSlot1() const
{
	return m_dmrNetworkSlot1;
}

bool CConf::getDMRNetworkSlot2() const
{
	return m_dmrNetworkSlot2;
}
