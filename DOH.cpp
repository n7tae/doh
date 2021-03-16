/*
 *   Copyright (C) 2015-2021 by Jonathan Naylor G4KLX
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

#include "DOH.h"
#include "DMRDirectNetwork.h"
#include "DMRGatewayNetwork.h"
#include "RSSIInterpolator.h"
#include "SerialController.h"
#include "StopWatch.h"
#include "Defines.h"
#include "Thread.h"
#include "Log.h"

#include <cstdio>
#include <vector>

#include <cstdlib>

#define VERSION "20210313"

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>

#ifndef CFG_DIR
#define CFG_DIR "/usr/local/etc"
#endif

static bool m_killed = false;
static int  m_signal = 0;
static bool m_reload = false;

static void sigHandler1(int signum)
{
	m_killed = true;
	m_signal = signum;
}

static void sigHandler2(int /*signum*/)
{
	m_reload = true;
}

const char* HEADER1 = "This software is for use on amateur radio networks only,";
const char* HEADER2 = "it is to be used for educational purposes only. Its use on";
const char* HEADER3 = "commercial networks is strictly prohibited.";
const char* HEADER4 = "Copyright(C) 2015-2020 by Jonathan Naylor, G4KLX and others";

int main(int argc, char** argv)
{
	printf("DOH version %s\n", VERSION);
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s /path/to/config/file\n", argv[0]);
		return 1;
	}

	::signal(SIGINT,  sigHandler1);
	::signal(SIGTERM, sigHandler1);
	::signal(SIGHUP,  sigHandler1);
	::signal(SIGUSR1, sigHandler2);

	int ret = 0;

	do
	{
		m_signal = 0;

		CDOH* host = new CDOH(argv[1]);
		ret = host->run();

		delete host;

		if (m_signal == 2)
			::LogInfo("DOH-%s exited on receipt of SIGINT", VERSION);

		if (m_signal == 15)
			::LogInfo("DOH-%s exited on receipt of SIGTERM", VERSION);

		if (m_signal == 1)
			::LogInfo("DOH-%s is restarting on receipt of SIGHUP", VERSION);
	}
	while (m_signal == 1);

	return ret;
}

CDOH::CDOH(const std::string& confFile) :
	m_conf(confFile),
	m_modem(NULL),
	m_dmr(NULL),
	m_dmrNetwork(NULL),
	m_mode(MODE_IDLE),
	m_dmrRFModeHang(10U),
	m_dmrNetModeHang(3U),
	m_modeTimer(1000U),
	m_dmrTXTimer(1000U),
	m_cwIdTimer(1000U),
	m_duplex(false),
	m_timeout(180U),
	m_cwIdTime(0U),
	m_dmrLookup(NULL),
	m_callsign(),
	m_id(0U),
	m_cwCallsign(),
	m_lockFileEnabled(false),
	m_lockFileName(),
	m_fixedMode(false)
{
	CUDPSocket::startup();
}

CDOH::~CDOH()
{
	CUDPSocket::shutdown();
}

int CDOH::run()
{
	bool ret = m_conf.Read();
	if (!ret)
	{
		::fprintf(stderr, "DOH: cannot Read the configuration file\n");
		return 1;
	}

	LogInitialise(m_conf.getLogLevel());

	LogInfo(HEADER1);
	LogInfo(HEADER2);
	LogInfo(HEADER3);
	LogInfo(HEADER4);

	LogMessage("DOH-%s is starting", VERSION);
	LogMessage("Built %s %s", __TIME__, __DATE__);

	readParams();

	ret = createModem();
	if (!ret)
		return 1;

	ret = createDMRNetwork();
	if (!ret)
		return 1;

	if (m_conf.getCWIdEnabled())
	{
		unsigned int time = m_conf.getCWIdTime();
		m_cwCallsign      = m_conf.getCWIdCallsign();

		LogInfo("CW Id Parameters");
		LogInfo("    Time: %u mins", time);
		LogInfo("    Callsign: %s", m_cwCallsign.c_str());

		m_cwIdTime = time * 60U;

		m_cwIdTimer.setTimeout(m_cwIdTime / 4U);
		m_cwIdTimer.start();
	}

	// For all modes we handle RSSI
	std::string rssiMappingFile = m_conf.getModemRSSIMappingFile();

	CRSSIInterpolator* rssi = new CRSSIInterpolator;
	if (!rssiMappingFile.empty())
	{
		LogInfo("RSSI");
		LogInfo("    Mapping File: %s", rssiMappingFile.c_str());
		rssi->load(rssiMappingFile);
	}

	// For DMR and P25 we try to map IDs to callsigns
	std::string lookupFile  = m_conf.getDMRIdLookupFile();
	unsigned int reloadTime = m_conf.getDMRIdLookupTime();

	LogInfo("DMR Id Lookups");
	LogInfo("    File: %s", lookupFile.length() > 0U ? lookupFile.c_str() : "None");
	if (reloadTime > 0U)
		LogInfo("    Reload: %u hours", reloadTime);

	m_dmrLookup = new CDMRLookup(lookupFile, reloadTime);
	m_dmrLookup->read();

	CStopWatch stopWatch;
	stopWatch.start();

	DMR_BEACONS dmrBeacons = DMR_BEACONS_OFF;
	CTimer dmrBeaconIntervalTimer(1000U);
	CTimer dmrBeaconDurationTimer(1000U);

	std::string dbpath(CFG_DIR);
	dbpath.append("/doh.db");
	m_dashDB.Open(dbpath.c_str());
	m_dashDB.ClearLH();

	if (true)
	{
		unsigned int id             = m_conf.getDMRId();
		unsigned int colorCode      = m_conf.getDMRColorCode();
		bool selfOnly               = m_conf.getDMRSelfOnly();
		bool embeddedLCOnly         = m_conf.getDMREmbeddedLCOnly();
		bool dumpTAData             = m_conf.getDMRDumpTAData();
		std::vector<unsigned int> prefixes  = m_conf.getDMRPrefixes();
		std::vector<unsigned int> blackList = m_conf.getDMRBlackList();
		std::vector<unsigned int> whiteList = m_conf.getDMRWhiteList();
		std::vector<unsigned int> slot1TGWhiteList = m_conf.getDMRSlot1TGWhiteList();
		std::vector<unsigned int> slot2TGWhiteList = m_conf.getDMRSlot2TGWhiteList();
		unsigned int callHang       = m_conf.getDMRCallHang();
		unsigned int txHang         = m_conf.getDMRTXHang();
		unsigned int jitter         = m_conf.getDMRNetworkJitter();
		m_dmrRFModeHang             = m_conf.getDMRModeHang();
		dmrBeacons                  = m_conf.getDMRBeacons();
		DMR_OVCM_TYPES ovcm         = m_conf.getDMROVCM();

		if (txHang > m_dmrRFModeHang)
			txHang = m_dmrRFModeHang;

		if (true)
		{
			if (txHang > m_dmrNetModeHang)
				txHang = m_dmrNetModeHang;
		}

		if (callHang > txHang)
			callHang = txHang;

		LogInfo("DMR RF Parameters");
		LogInfo("    Id: %u", id);
		LogInfo("    Color Code: %u", colorCode);
		LogInfo("    Self Only: %s", selfOnly ? "yes" : "no");
		LogInfo("    Embedded LC Only: %s", embeddedLCOnly ? "yes" : "no");
		LogInfo("    Dump Talker Alias Data: %s", dumpTAData ? "yes" : "no");
		LogInfo("    Prefixes: %u", prefixes.size());

		if (blackList.size() > 0U)
			LogInfo("    Source ID Black List: %u", blackList.size());
		if (whiteList.size() > 0U)
			LogInfo("    Source ID White List: %u", whiteList.size());
		if (slot1TGWhiteList.size() > 0U)
			LogInfo("    Slot 1 TG White List: %u", slot1TGWhiteList.size());
		if (slot2TGWhiteList.size() > 0U)
			LogInfo("    Slot 2 TG White List: %u", slot2TGWhiteList.size());

		LogInfo("    Call Hang: %us", callHang);
		LogInfo("    TX Hang: %us", txHang);
		LogInfo("    Mode Hang: %us", m_dmrRFModeHang);
		if (ovcm == DMR_OVCM_OFF)
			LogInfo("    OVCM: off");
		else if (ovcm == DMR_OVCM_RX_ON)
			LogInfo("    OVCM: on(rx only)");
		else if (ovcm == DMR_OVCM_TX_ON)
			LogInfo("    OVCM: on(tx only)");
		else if (ovcm == DMR_OVCM_ON)
			LogInfo("    OVCM: on");


		switch (dmrBeacons)
		{
		case DMR_BEACONS_NETWORK:
		{
			unsigned int dmrBeaconDuration = m_conf.getDMRBeaconDuration();

			LogInfo("    DMR Roaming Beacons Type: network");
			LogInfo("    DMR Roaming Beacons Duration: %us", dmrBeaconDuration);

			dmrBeaconDurationTimer.setTimeout(dmrBeaconDuration);
		}
		break;
		case DMR_BEACONS_TIMED:
		{
			unsigned int dmrBeaconInterval = m_conf.getDMRBeaconInterval();
			unsigned int dmrBeaconDuration = m_conf.getDMRBeaconDuration();

			LogInfo("    DMR Roaming Beacons Type: timed");
			LogInfo("    DMR Roaming Beacons Interval: %us", dmrBeaconInterval);
			LogInfo("    DMR Roaming Beacons Duration: %us", dmrBeaconDuration);

			dmrBeaconDurationTimer.setTimeout(dmrBeaconDuration);

			dmrBeaconIntervalTimer.setTimeout(dmrBeaconInterval);
			dmrBeaconIntervalTimer.start();
		}
		break;
		default:
			LogInfo("    DMR Roaming Beacons Type: off");
			break;
		}

		m_dmr = new CDMRControl(id, colorCode, callHang, selfOnly, embeddedLCOnly, dumpTAData, prefixes, blackList, whiteList, slot1TGWhiteList, slot2TGWhiteList, m_timeout, m_modem, m_dmrNetwork, m_duplex, m_dmrLookup, rssi, jitter, ovcm, &m_dashDB);

		m_dmrTXTimer.setTimeout(txHang);
	}

	setMode(MODE_IDLE);

	LogMessage("DOH-%s is running", VERSION);

	while (!m_killed)
	{
		bool lockout1 = m_modem->hasLockout();
		bool lockout2 = false;

		if ((lockout1 || lockout2) && m_mode != MODE_LOCKOUT)
			setMode(MODE_LOCKOUT);
		else if ((!lockout1 && !lockout2) && m_mode == MODE_LOCKOUT)
			setMode(MODE_IDLE);

		bool error = m_modem->hasError();
		if (error && m_mode != MODE_ERROR)
			setMode(MODE_ERROR);
		else if (!error && m_mode == MODE_ERROR)
			setMode(MODE_IDLE);

		unsigned char mode = m_modem->getMode();
		if (mode == MODE_FM && m_mode != MODE_FM)
			setMode(mode);
		else if (mode != MODE_FM && m_mode == MODE_FM)
			setMode(mode);

		unsigned char data[MODEM_DATA_LEN];
		unsigned int len;
		bool ret;

		len = m_modem->readDMRData1(data);
		if (m_dmr != NULL && len > 0U)
		{
			if (m_mode == MODE_IDLE)
			{
				if (m_duplex)
				{
					bool ret = m_dmr->processWakeup(data);
					if (ret)
					{
						m_modeTimer.setTimeout(m_dmrRFModeHang);
						setMode(MODE_DMR);
						dmrBeaconDurationTimer.stop();
					}
				}
				else
				{
					m_modeTimer.setTimeout(m_dmrRFModeHang);
					setMode(MODE_DMR);
					m_dmr->writeModemSlot1(data, len);
					dmrBeaconDurationTimer.stop();
				}
			}
			else if (m_mode == MODE_DMR)
			{
				if (m_duplex && !m_modem->hasTX())
				{
					bool ret = m_dmr->processWakeup(data);
					if (ret)
					{
						m_modem->writeDMRStart(true);
						m_dmrTXTimer.start();
					}
				}
				else
				{
					bool ret = m_dmr->writeModemSlot1(data, len);
					if (ret)
					{
						dmrBeaconDurationTimer.stop();
						m_modeTimer.start();
						if (m_duplex)
							m_dmrTXTimer.start();
					}
				}
			}
			else if (m_mode != MODE_LOCKOUT)
			{
				LogWarning("DMR modem data received when in mode %u", m_mode);
			}
		}

		len = m_modem->readDMRData2(data);
		if (m_dmr != NULL && len > 0U)
		{
			if (m_mode == MODE_IDLE)
			{
				if (m_duplex)
				{
					bool ret = m_dmr->processWakeup(data);
					if (ret)
					{
						m_modeTimer.setTimeout(m_dmrRFModeHang);
						setMode(MODE_DMR);
						dmrBeaconDurationTimer.stop();
					}
				}
				else
				{
					m_modeTimer.setTimeout(m_dmrRFModeHang);
					setMode(MODE_DMR);
					m_dmr->writeModemSlot2(data, len);
					dmrBeaconDurationTimer.stop();
				}
			}
			else if (m_mode == MODE_DMR)
			{
				if (m_duplex && !m_modem->hasTX())
				{
					bool ret = m_dmr->processWakeup(data);
					if (ret)
					{
						m_modem->writeDMRStart(true);
						m_dmrTXTimer.start();
					}
				}
				else
				{
					bool ret = m_dmr->writeModemSlot2(data, len);
					if (ret)
					{
						dmrBeaconDurationTimer.stop();
						m_modeTimer.start();
						if (m_duplex)
							m_dmrTXTimer.start();
					}
				}
			}
			else if (m_mode != MODE_LOCKOUT)
			{
				LogWarning("DMR modem data received when in mode %u", m_mode);
			}
		}

		if (!m_fixedMode)
		{
			if (m_modeTimer.isRunning() && m_modeTimer.hasExpired())
				setMode(MODE_IDLE);
		}

		if (m_dmr != NULL)
		{
			ret = m_modem->hasDMRSpace1();
			if (ret)
			{
				len = m_dmr->readModemSlot1(data);
				if (len > 0U)
				{
					if (m_mode == MODE_IDLE)
					{
						m_modeTimer.setTimeout(m_dmrNetModeHang);
						setMode(MODE_DMR);
					}
					if (m_mode == MODE_DMR)
					{
						if (m_duplex)
						{
							m_modem->writeDMRStart(true);
							m_dmrTXTimer.start();
						}
						m_modem->writeDMRData1(data, len);
						dmrBeaconDurationTimer.stop();
						m_modeTimer.start();
					}
					else if (m_mode != MODE_LOCKOUT)
					{
						LogWarning("DMR data received when in mode %u", m_mode);
					}
				}
			}

			ret = m_modem->hasDMRSpace2();
			if (ret)
			{
				len = m_dmr->readModemSlot2(data);
				if (len > 0U)
				{
					if (m_mode == MODE_IDLE)
					{
						m_modeTimer.setTimeout(m_dmrNetModeHang);
						setMode(MODE_DMR);
					}
					if (m_mode == MODE_DMR)
					{
						if (m_duplex)
						{
							m_modem->writeDMRStart(true);
							m_dmrTXTimer.start();
						}
						m_modem->writeDMRData2(data, len);
						dmrBeaconDurationTimer.stop();
						m_modeTimer.start();
					}
					else if (m_mode != MODE_LOCKOUT)
					{
						LogWarning("DMR data received when in mode %u", m_mode);
					}
				}
			}
		}

		unsigned int ms = stopWatch.elapsed();
		stopWatch.start();

		m_modem->clock(ms);

		if (!m_fixedMode)
			m_modeTimer.clock(ms);

		if (m_reload)
		{
			if (m_dmrLookup != NULL)
				m_dmrLookup->reload();

			m_reload = false;
		}
		if (m_dmr != NULL)
			m_dmr->clock();
		if (m_dmrNetwork != NULL)
			m_dmrNetwork->clock(ms);

		m_cwIdTimer.clock(ms);
		if (m_cwIdTimer.isRunning() && m_cwIdTimer.hasExpired())
		{
			if (!m_modem->hasTX())
			{
				LogDebug("sending CW ID");
				m_modem->sendCWId(m_cwCallsign);

				m_cwIdTimer.setTimeout(m_cwIdTime);
				m_cwIdTimer.start();
			}
		}

		switch (dmrBeacons)
		{
		case DMR_BEACONS_TIMED:
			dmrBeaconIntervalTimer.clock(ms);
			if (dmrBeaconIntervalTimer.isRunning() && dmrBeaconIntervalTimer.hasExpired())
			{
				if ((m_mode == MODE_IDLE || m_mode == MODE_DMR) && !m_modem->hasTX())
				{
					if (!m_fixedMode && m_mode == MODE_IDLE)
						setMode(MODE_DMR);
					dmrBeaconIntervalTimer.start();
					dmrBeaconDurationTimer.start();
				}
			}
			break;
		case DMR_BEACONS_NETWORK:
			if (m_dmrNetwork != NULL)
			{
				bool beacon = m_dmrNetwork->wantsBeacon();
				if (beacon)
				{
					if ((m_mode == MODE_IDLE || m_mode == MODE_DMR) && !m_modem->hasTX())
					{
						if (!m_fixedMode && m_mode == MODE_IDLE)
							setMode(MODE_DMR);
						dmrBeaconDurationTimer.start();
					}
				}
			}
			break;
		default:
			break;
		}

		dmrBeaconDurationTimer.clock(ms);
		if (dmrBeaconDurationTimer.isRunning() && dmrBeaconDurationTimer.hasExpired())
		{
			if (!m_fixedMode)
				setMode(MODE_IDLE);
			dmrBeaconDurationTimer.stop();
		}

		m_dmrTXTimer.clock(ms);
		if (m_dmrTXTimer.isRunning() && m_dmrTXTimer.hasExpired())
		{
			m_modem->writeDMRStart(false);
			m_dmrTXTimer.stop();
		}

		if (ms < 5U)
			CThread::sleep(5U);
	}

	setMode(MODE_QUIT);

	m_modem->close();
	delete m_modem;

	if (m_dmrLookup != NULL)
		m_dmrLookup->stop();

	if (m_dmrNetwork != NULL)
	{
		m_dmrNetwork->close();
		delete m_dmrNetwork;
	}

	delete m_dmr;

	return 0;
}

bool CDOH::createModem()
{
	std::string port             = m_conf.getModemPort();
	std::string protocol	     = m_conf.getModemProtocol();
	unsigned int address	     = m_conf.getModemAddress();
	bool rxInvert                = m_conf.getModemRXInvert();
	bool txInvert                = m_conf.getModemTXInvert();
	bool pttInvert               = m_conf.getModemPTTInvert();
	unsigned int txDelay         = m_conf.getModemTXDelay();
	unsigned int dmrDelay        = m_conf.getModemDMRDelay();
	float rxLevel                = m_conf.getModemRXLevel();
	float cwIdTXLevel            = m_conf.getModemCWIdTXLevel();
	float dmrTXLevel             = m_conf.getModemDMRTXLevel();
	bool trace                   = m_conf.getModemTrace();
	bool debug                   = m_conf.getModemDebug();
	unsigned int colorCode       = m_conf.getDMRColorCode();
	unsigned int rxFrequency     = m_conf.getRXFrequency();
	unsigned int txFrequency     = m_conf.getTXFrequency();
	unsigned int pocsagFrequency = txFrequency;
	int rxOffset                 = m_conf.getModemRXOffset();
	int txOffset                 = m_conf.getModemTXOffset();
	int rxDCOffset               = m_conf.getModemRXDCOffset();
	int txDCOffset               = m_conf.getModemTXDCOffset();
	float rfLevel                = m_conf.getModemRFLevel();
	bool useCOSAsLockout         = m_conf.getModemUseCOSAsLockout();

	LogInfo("Modem Parameters");
	LogInfo("    Port: %s", port.c_str());
	LogInfo("    Protocol: %s", protocol.c_str());
	if (protocol == "i2c")
		LogInfo("    i2c Address: %02X", address);
	LogInfo("    RX Invert: %s", rxInvert ? "yes" : "no");
	LogInfo("    TX Invert: %s", txInvert ? "yes" : "no");
	LogInfo("    PTT Invert: %s", pttInvert ? "yes" : "no");
	LogInfo("    TX Delay: %ums", txDelay);
	LogInfo("    RX Offset: %dHz", rxOffset);
	LogInfo("    TX Offset: %dHz", txOffset);
	LogInfo("    RX DC Offset: %d", rxDCOffset);
	LogInfo("    TX DC Offset: %d", txDCOffset);
	LogInfo("    RF Level: %.1f%%", rfLevel);
	LogInfo("    DMR Delay: %u (%.1fms)", dmrDelay, float(dmrDelay) * 0.0416666F);
	LogInfo("    RX Level: %.1f%%", rxLevel);
	LogInfo("    CW Id TX Level: %.1f%%", cwIdTXLevel);
	LogInfo("    DMR TX Level: %.1f%%", dmrTXLevel);
	LogInfo("    TX Frequency: %uHz (%uHz)", txFrequency, txFrequency + txOffset);
	LogInfo("    Use COS as Lockout: %s", useCOSAsLockout ? "yes" : "no");

	m_modem = new CModem(port, m_duplex, rxInvert, txInvert, pttInvert, txDelay, dmrDelay, useCOSAsLockout, trace, debug);
	m_modem->setSerialParams(protocol, address);
	m_modem->setLevels(rxLevel, cwIdTXLevel, 0.0F, dmrTXLevel, 0.0F, 0.0F, 0.0F, 0.0F);
	m_modem->setRFParams(rxFrequency, rxOffset, txFrequency, txOffset, txDCOffset, rxDCOffset, rfLevel, pocsagFrequency);
	m_modem->setDMRParams(colorCode);

	bool ret = m_modem->open();
	if (!ret)
	{
		delete m_modem;
		m_modem = NULL;
		return false;
	}

	return true;
}

bool CDOH::createDMRNetwork()
{
	std::string address  = m_conf.getDMRNetworkAddress();
	unsigned int port    = m_conf.getDMRNetworkPort();
	unsigned int local   = m_conf.getDMRNetworkLocal();
	unsigned int id      = m_conf.getDMRId();
	std::string password = m_conf.getDMRNetworkPassword();
	bool debug           = m_conf.getDMRNetworkDebug();
	unsigned int jitter  = m_conf.getDMRNetworkJitter();
	bool slot1           = m_conf.getDMRNetworkSlot1();
	bool slot2           = m_conf.getDMRNetworkSlot2();
	HW_TYPE hwType       = m_modem->getHWType();
	m_dmrNetModeHang     = m_conf.getDMRNetworkModeHang();
	std::string options  = m_conf.getDMRNetworkOptions();

	std::string type = m_conf.getDMRNetworkType();

	LogInfo("DMR Network Parameters");
	LogInfo("    Type: %s", type.c_str());
	LogInfo("    Address: %s", address.c_str());
	LogInfo("    Port: %u", port);
	if (local > 0U)
		LogInfo("    Local: %u", local);
	else
		LogInfo("    Local: random");
	LogInfo("    Jitter: %ums", jitter);
	LogInfo("    Slot 1: %s", slot1 ? "enabled" : "disabled");
	LogInfo("    Slot 2: %s", slot2 ? "enabled" : "disabled");
	LogInfo("    Mode Hang: %us", m_dmrNetModeHang);

	if (type == "Direct")
		m_dmrNetwork = new CDMRDirectNetwork(address, port, local, id, password, m_duplex, VERSION, slot1, slot2, hwType, debug);
	else
		m_dmrNetwork = new CDMRGatewayNetwork(address, port, local, id, m_duplex, VERSION, slot1, slot2, hwType, debug);

	unsigned int rxFrequency = m_conf.getRXFrequency();
	unsigned int txFrequency = m_conf.getTXFrequency();
	unsigned int power       = m_conf.getPower();
	unsigned int colorCode   = m_conf.getDMRColorCode();

	LogInfo("Info Parameters");
	LogInfo("    Callsign: %s", m_callsign.c_str());
	LogInfo("    RX Frequency: %uHz", rxFrequency);
	LogInfo("    TX Frequency: %uHz", txFrequency);
	LogInfo("    Power: %uW", power);

	if (type == "Direct")
	{
		float latitude          = m_conf.getLatitude();
		float longitude         = m_conf.getLongitude();
		int height              = m_conf.getHeight();
		std::string location    = m_conf.getLocation();
		std::string description = m_conf.getDescription();
		std::string url         = m_conf.getURL();

		LogInfo("    Latitude: %fdeg N", latitude);
		LogInfo("    Longitude: %fdeg E", longitude);
		LogInfo("    Height: %um", height);
		LogInfo("    Location: \"%s\"", location.c_str());
		LogInfo("    Description: \"%s\"", description.c_str());
		LogInfo("    URL: \"%s\"", url.c_str());

		m_dmrNetwork->setConfig(m_callsign, rxFrequency, txFrequency, power, colorCode, latitude, longitude, height, location, description, url);
	}
	else
	{
		m_dmrNetwork->setConfig(m_callsign, rxFrequency, txFrequency, power, colorCode, 0.0F, 0.0F, 0, "", "", "");
	}

	if (!options.empty())
	{
		LogInfo("    Options: %s", options.c_str());

		m_dmrNetwork->setOptions(options);
	}

	bool ret = m_dmrNetwork->open();
	if (!ret)
	{
		delete m_dmrNetwork;
		m_dmrNetwork = NULL;
		return false;
	}

	m_dmrNetwork->enable(true);

	return true;
}

void CDOH::readParams()
{
	m_duplex        = m_conf.getDuplex();
	m_callsign      = m_conf.getCallsign();
	m_id            = m_conf.getId();
	m_timeout       = m_conf.getTimeout();

	LogInfo("General Parameters");
	LogInfo("    Callsign: %s", m_callsign.c_str());
	LogInfo("    Id: %u", m_id);
	LogInfo("    Duplex: %s", m_duplex ? "yes" : "no");
	LogInfo("    Timeout: %us", m_timeout);
	LogInfo("    DMR: enabled");
}

void CDOH::setMode(unsigned char mode)
{
	assert(m_modem != NULL);

	switch (mode)
	{
	case MODE_DMR:
		if (m_dmrNetwork != NULL)
			m_dmrNetwork->enable(true);
		if (m_dmr != NULL)
			m_dmr->enable(true);
		m_modem->setMode(MODE_DMR);
		if (m_duplex)
		{
			m_modem->writeDMRStart(true);
			m_dmrTXTimer.start();
		}
		m_mode = MODE_DMR;
		m_modeTimer.start();
		m_cwIdTimer.stop();
		createLockFile("DMR");
		break;

	case MODE_LOCKOUT:
		if (m_dmrNetwork != NULL)
			m_dmrNetwork->enable(false);
		if (m_dmr != NULL)
			m_dmr->enable(false);
		if (m_mode == MODE_DMR && m_duplex && m_modem->hasTX())
		{
			m_modem->writeDMRStart(false);
			m_dmrTXTimer.stop();
		}
		m_modem->setMode(MODE_IDLE);
		m_mode = MODE_LOCKOUT;
		m_modeTimer.stop();
		m_cwIdTimer.stop();
		removeLockFile();
		break;

	case MODE_ERROR:
		LogMessage("Mode set to Error");
		if (m_dmrNetwork != NULL)
			m_dmrNetwork->enable(false);
		if (m_dmr != NULL)
			m_dmr->enable(false);
		if (m_mode == MODE_DMR && m_duplex && m_modem->hasTX())
		{
			m_modem->writeDMRStart(false);
			m_dmrTXTimer.stop();
		}
		m_mode = MODE_ERROR;
		m_modeTimer.stop();
		m_cwIdTimer.stop();
		removeLockFile();
		break;

	default:
		if (m_dmrNetwork != NULL)
			m_dmrNetwork->enable(true);
		if (m_dmr != NULL)
			m_dmr->enable(true);
		if (m_mode == MODE_DMR && m_duplex && m_modem->hasTX())
		{
			m_modem->writeDMRStart(false);
			m_dmrTXTimer.stop();
		}
		m_modem->setMode(MODE_IDLE);
		if (m_mode == MODE_ERROR)
		{
			m_modem->sendCWId(m_callsign);
			m_cwIdTimer.setTimeout(m_cwIdTime);
			m_cwIdTimer.start();
		}
		else
		{
			m_cwIdTimer.setTimeout(m_cwIdTime / 4U);
			m_cwIdTimer.start();
		}
		m_mode = MODE_IDLE;
		m_modeTimer.stop();
		removeLockFile();
		break;
	}
}

void  CDOH::createLockFile(const char* mode) const
{
	if (m_lockFileEnabled)
	{
		FILE* fp = ::fopen(m_lockFileName.c_str(), "wt");
		if (fp != NULL)
		{
			::fprintf(fp, "%s\n", mode);
			::fclose(fp);
		}
	}
}

void  CDOH::removeLockFile() const
{
	if (m_lockFileEnabled)
		::remove(m_lockFileName.c_str());
}
