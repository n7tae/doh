/*
 *   Copyright (C) 2011-2018,2020 by Jonathan Naylor G4KLX
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

#ifndef	MODEM_H
#define	MODEM_H

#include "SerialController.h"
#include "RingBuffer.h"
#include "Defines.h"
#include "Timer.h"

#include <string>

enum RESP_TYPE_MMDVM
{
	RTM_OK,
	RTM_TIMEOUT,
	RTM_ERROR
};

class CModem
{
public:
	CModem(const std::string& port, bool duplex, bool rxInvert, bool txInvert, bool pttInvert, unsigned int txDelay, unsigned int dmrDelay, bool useCOSAsLockout, bool trace, bool debug);
	~CModem();

	void setSerialParams(const std::string& protocol, unsigned int address);
	void setRFParams(unsigned int rxFrequency, int rxOffset, unsigned int txFrequency, int txOffset, int txDCOffset, int rxDCOffset, float rfLevel, unsigned int pocsagFrequency);
	void setLevels(float rxLevel, float cwIdTXLevel, float dstarTXLevel, float dmrTXLevel, float ysfTXLevel, float p25TXLevel, float nxdnTXLevel, float pocsagLevel);
	void setDMRParams(unsigned int colorCode);

	bool open();

	unsigned int readDMRData1(unsigned char* data);
	unsigned int readDMRData2(unsigned char* data);

	bool hasDMRSpace1() const;
	bool hasDMRSpace2() const;

	bool hasTX() const;
	bool hasCD() const;

	bool hasLockout() const;
	bool hasError() const;

	bool writeConfig();
	bool writeDMRData1(const unsigned char* data, unsigned int length);
	bool writeDMRData2(const unsigned char* data, unsigned int length);

	bool writeDMRInfo(unsigned int slotNo, const std::string& src, bool group, const std::string& dst, const char* type);

	bool writeDMRStart(bool tx);
	bool writeDMRShortLC(const unsigned char* lc);
	bool writeDMRAbort(unsigned int slotNo);

	bool writeSerial(const unsigned char* data, unsigned int length);

	unsigned char getMode() const;
	bool setMode(unsigned char mode);

	bool sendCWId(const std::string& callsign);

	HW_TYPE getHWType() const;

	void clock(unsigned int ms);

	void close();

private:
	std::string                m_port;
	unsigned int               m_dmrColorCode;
	bool                       m_ysfLoDev;
	unsigned int               m_ysfTXHang;
	unsigned int               m_p25TXHang;
	unsigned int               m_nxdnTXHang;
	bool                       m_duplex;
	bool                       m_rxInvert;
	bool                       m_txInvert;
	bool                       m_pttInvert;
	unsigned int               m_txDelay;
	unsigned int               m_dmrDelay;
	float                      m_rxLevel;
	float                      m_cwIdTXLevel;
	float                      m_dstarTXLevel;
	float                      m_dmrTXLevel;
	float                      m_ysfTXLevel;
	float                      m_p25TXLevel;
	float                      m_nxdnTXLevel;
	float                      m_pocsagTXLevel;
	float                      m_rfLevel;
	bool                       m_useCOSAsLockout;
	bool                       m_trace;
	bool                       m_debug;
	unsigned int               m_rxFrequency;
	unsigned int               m_txFrequency;
	unsigned int               m_pocsagFrequency;
	int                        m_rxDCOffset;
	int                        m_txDCOffset;
	CSerialController*         m_serial;
	unsigned char*             m_buffer;
	unsigned int               m_length;
	unsigned int               m_offset;
	CRingBuffer<unsigned char> m_rxDMRData1;
	CRingBuffer<unsigned char> m_rxDMRData2;
	CRingBuffer<unsigned char> m_txDMRData1;
	CRingBuffer<unsigned char> m_txDMRData2;
	CTimer                     m_statusTimer;
	CTimer                     m_inactivityTimer;
	CTimer                     m_playoutTimer;
	unsigned int               m_dmrSpace1;
	unsigned int               m_dmrSpace2;
	bool                       m_tx;
	bool                       m_cd;
	bool                       m_lockout;
	bool                       m_error;
	unsigned char              m_mode;
	HW_TYPE                    m_hwType;


	bool readVersion();
	bool readStatus();
	bool setConfig();
	bool setFrequency();

	void printDebug();

	RESP_TYPE_MMDVM getResponse();
};

#endif
