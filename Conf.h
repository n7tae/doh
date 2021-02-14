/*
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

#if !defined(CONF_H)
#define	CONF_H

#include <string>
#include <vector>

class CConf
{
public:
	CConf(const std::string& file);
	~CConf();

	bool read();

	// The General section
	std::string  getCallsign() const;
	unsigned int getId() const;
	unsigned int getTimeout() const;
	bool         getDuplex() const;
	std::string  getDisplay() const;
	bool         getDaemon() const;

	// The Info section
	unsigned int getRXFrequency() const;
	unsigned int getTXFrequency() const;
	unsigned int getPower() const;
	float        getLatitude() const;
	float        getLongitude() const;
	int          getHeight() const;
	std::string  getLocation() const;
	std::string  getDescription() const;
	std::string  getURL() const;

	// The Log section
	unsigned int getLogDisplayLevel() const;
	unsigned int getLogFileLevel() const;
	std::string  getLogFilePath() const;
	std::string  getLogFileRoot() const;
	bool         getLogFileRotate() const;

	// The CW ID section
	bool         getCWIdEnabled() const;
	unsigned int getCWIdTime() const;
	std::string  getCWIdCallsign() const;

	// The DMR Id section
	std::string  getDMRIdLookupFile() const;
	unsigned int getDMRIdLookupTime() const;

	// The Modem section
	std::string  getModemPort() const;
	std::string  getModemProtocol() const;
	unsigned int getModemAddress() const;
	bool         getModemRXInvert() const;
	bool         getModemTXInvert() const;
	bool         getModemPTTInvert() const;
	unsigned int getModemTXDelay() const;
	unsigned int getModemDMRDelay() const;
	int          getModemTXOffset() const;
	int          getModemRXOffset() const;
	int          getModemRXDCOffset() const;
	int          getModemTXDCOffset() const;
	float        getModemRFLevel() const;
	float        getModemRXLevel() const;
	float        getModemCWIdTXLevel() const;
	float        getModemDStarTXLevel() const;
	float        getModemDMRTXLevel() const;
	float        getModemYSFTXLevel() const;
	float        getModemP25TXLevel() const;
	float        getModemNXDNTXLevel() const;
	float        getModemPOCSAGTXLevel() const;
	float        getModemFMTXLevel() const;
	std::string  getModemRSSIMappingFile() const;
	bool         getModemUseCOSAsLockout() const;
	bool         getModemTrace() const;
	bool         getModemDebug() const;

	// The DMR section
	DMR_BEACONS  getDMRBeacons() const;
	unsigned int getDMRBeaconInterval() const;
	unsigned int getDMRBeaconDuration() const;
	unsigned int getDMRId() const;
	unsigned int getDMRColorCode() const;
	bool         getDMREmbeddedLCOnly() const;
	bool         getDMRDumpTAData() const;
	bool         getDMRSelfOnly() const;
	std::vector<unsigned int> getDMRPrefixes() const;
	std::vector<unsigned int> getDMRBlackList() const;
	std::vector<unsigned int> getDMRWhiteList() const;
	std::vector<unsigned int> getDMRSlot1TGWhiteList() const;
	std::vector<unsigned int> getDMRSlot2TGWhiteList() const;
	unsigned int getDMRCallHang() const;
	unsigned int getDMRTXHang() const;
	unsigned int getDMRModeHang() const;
	DMR_OVCM_TYPES getDMROVCM() const;

	// The DMR Network section
	std::string  getDMRNetworkType() const;
	std::string  getDMRNetworkAddress() const;
	unsigned int getDMRNetworkPort() const;
	unsigned int getDMRNetworkLocal() const;
	std::string  getDMRNetworkPassword() const;
	std::string  getDMRNetworkOptions() const;
	bool         getDMRNetworkDebug() const;
	unsigned int getDMRNetworkJitter() const;
	bool         getDMRNetworkSlot1() const;
	bool         getDMRNetworkSlot2() const;
	unsigned int getDMRNetworkModeHang() const;

	// The TFTSERIAL section
	std::string  getTFTSerialPort() const;
	unsigned int getTFTSerialBrightness() const;

	// The HD44780 section
	unsigned int getHD44780Rows() const;
	unsigned int getHD44780Columns() const;
	std::vector<unsigned int> getHD44780Pins() const;
	unsigned int getHD44780i2cAddress() const;
	bool         getHD44780PWM() const;
	unsigned int getHD44780PWMPin() const;
	unsigned int getHD44780PWMBright() const;
	unsigned int getHD44780PWMDim() const;
	bool         getHD44780DisplayClock() const;
	bool         getHD44780UTC() const;

	// The Nextion section
	std::string  getNextionPort() const;
	unsigned int getNextionBrightness() const;
	bool         getNextionDisplayClock() const;
	bool         getNextionUTC() const;
	unsigned int getNextionIdleBrightness() const;
	unsigned int getNextionScreenLayout() const;
	bool         getNextionTempInFahrenheit() const;

	// The OLED section
	unsigned char  getOLEDType() const;
	unsigned char  getOLEDBrightness() const;
	bool           getOLEDInvert() const;
	bool           getOLEDScroll() const;
	bool           getOLEDRotate() const;
	bool           getOLEDLogoScreensaver() const;

	// The LCDproc section
	std::string  getLCDprocAddress() const;
	unsigned int getLCDprocPort() const;
	unsigned int getLCDprocLocalPort() const;
	bool         getLCDprocDisplayClock() const;
	bool         getLCDprocUTC() const;
	bool         getLCDprocDimOnIdle() const;

private:
	std::string  m_file;
	std::string  m_callsign;
	unsigned int m_id;
	unsigned int m_timeout;
	bool         m_duplex;
	std::string  m_display;
	bool         m_daemon;

	unsigned int m_rxFrequency;
	unsigned int m_txFrequency;
	unsigned int m_power;
	float        m_latitude;
	float        m_longitude;
	int          m_height;
	std::string  m_location;
	std::string  m_description;
	std::string  m_url;

	unsigned int m_logDisplayLevel;
	unsigned int m_logFileLevel;
	std::string  m_logFilePath;
	std::string  m_logFileRoot;
	bool         m_logFileRotate;

	bool         m_cwIdEnabled;
	unsigned int m_cwIdTime;
	std::string  m_cwIdCallsign;

	std::string  m_dmrIdLookupFile;
	unsigned int m_dmrIdLookupTime;

	std::string  m_modemPort;
	std::string  m_modemProtocol;
	unsigned int m_modemAddress;
	bool         m_modemRXInvert;
	bool         m_modemTXInvert;
	bool         m_modemPTTInvert;
	unsigned int m_modemTXDelay;
	unsigned int m_modemDMRDelay;
	int          m_modemTXOffset;
	int          m_modemRXOffset;
	int          m_modemRXDCOffset;
	int          m_modemTXDCOffset;
	float        m_modemRFLevel;
	float        m_modemRXLevel;
	float        m_modemCWIdTXLevel;
	float        m_modemDStarTXLevel;
	float        m_modemDMRTXLevel;
	float        m_modemYSFTXLevel;
	float        m_modemP25TXLevel;
	float        m_modemNXDNTXLevel;
	float        m_modemPOCSAGTXLevel;
	float        m_modemFMTXLevel;
	std::string  m_modemRSSIMappingFile;
	bool         m_modemUseCOSAsLockout;
	bool         m_modemTrace;
	bool         m_modemDebug;

	DMR_BEACONS  m_dmrBeacons;
	unsigned int m_dmrBeaconInterval;
	unsigned int m_dmrBeaconDuration;
	unsigned int m_dmrId;
	unsigned int m_dmrColorCode;
	bool         m_dmrSelfOnly;
	bool         m_dmrEmbeddedLCOnly;
	bool         m_dmrDumpTAData;
	std::vector<unsigned int> m_dmrPrefixes;
	std::vector<unsigned int> m_dmrBlackList;
	std::vector<unsigned int> m_dmrWhiteList;
	std::vector<unsigned int> m_dmrSlot1TGWhiteList;
	std::vector<unsigned int> m_dmrSlot2TGWhiteList;
	unsigned int m_dmrCallHang;
	unsigned int m_dmrTXHang;
	unsigned int m_dmrModeHang;
	DMR_OVCM_TYPES m_dmrOVCM;

	std::string  m_dmrNetworkType;
	std::string  m_dmrNetworkAddress;
	unsigned int m_dmrNetworkPort;
	unsigned int m_dmrNetworkLocal;
	std::string  m_dmrNetworkPassword;
	std::string  m_dmrNetworkOptions;
	bool         m_dmrNetworkDebug;
	unsigned int m_dmrNetworkJitter;
	bool         m_dmrNetworkSlot1;
	bool         m_dmrNetworkSlot2;
	unsigned int m_dmrNetworkModeHang;

	std::string  m_tftSerialPort;
	unsigned int m_tftSerialBrightness;

	unsigned int m_hd44780Rows;
	unsigned int m_hd44780Columns;
	std::vector<unsigned int> m_hd44780Pins;
	unsigned int m_hd44780i2cAddress;
	bool         m_hd44780PWM;
	unsigned int m_hd44780PWMPin;
	unsigned int m_hd44780PWMBright;
	unsigned int m_hd44780PWMDim;
	bool         m_hd44780DisplayClock;
	bool         m_hd44780UTC;

	std::string  m_nextionPort;
	unsigned int m_nextionBrightness;
	bool         m_nextionDisplayClock;
	bool         m_nextionUTC;
	unsigned int m_nextionIdleBrightness;
	unsigned int m_nextionScreenLayout;
	bool         m_nextionTempInFahrenheit;

	unsigned char m_oledType;
	unsigned char m_oledBrightness;
	bool          m_oledInvert;
	bool          m_oledScroll;
	bool          m_oledRotate;
	bool          m_oledLogoScreensaver;

	std::string  m_lcdprocAddress;
	unsigned int m_lcdprocPort;
	unsigned int m_lcdprocLocalPort;
	bool         m_lcdprocDisplayClock;
	bool         m_lcdprocUTC;
	bool         m_lcdprocDimOnIdle;
};

#endif
