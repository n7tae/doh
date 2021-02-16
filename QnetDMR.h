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

#if !defined(MMDVMHOST_H)
#define	MMDVMHOST_H

#include "DMRControl.h"
#include "DMRNetwork.h"
#include "DMRLookup.h"
#include "Display.h"
#include "Timer.h"
#include "Modem.h"
#include "Conf.h"

#include <string>


class CQnetDMR
{
public:
	CQnetDMR(const std::string& confFile);
	~CQnetDMR();

	int run();

private:
	CConf           m_conf;
	CModem*         m_modem;
	CDMRControl*    m_dmr;
	IDMRNetwork*    m_dmrNetwork;
	CDisplay*       m_display;
	unsigned char   m_mode;
	unsigned int    m_dstarRFModeHang;
	unsigned int    m_dmrRFModeHang;
	unsigned int    m_ysfRFModeHang;
	unsigned int    m_p25RFModeHang;
	unsigned int    m_nxdnRFModeHang;
	unsigned int    m_dstarNetModeHang;
	unsigned int    m_dmrNetModeHang;
	unsigned int    m_ysfNetModeHang;
	unsigned int    m_p25NetModeHang;
	unsigned int    m_nxdnNetModeHang;
	unsigned int    m_pocsagNetModeHang;
	CTimer          m_modeTimer;
	CTimer          m_dmrTXTimer;
	CTimer          m_cwIdTimer;
	bool            m_duplex;
	unsigned int    m_timeout;
	bool            m_dstarEnabled;
	bool            m_dmrEnabled;
	bool            m_ysfEnabled;
	bool            m_p25Enabled;
	bool            m_nxdnEnabled;
	bool            m_pocsagEnabled;
	bool            m_fmEnabled;
	unsigned int    m_cwIdTime;
	CDMRLookup*     m_dmrLookup;
	std::string     m_callsign;
	unsigned int    m_id;
	std::string     m_cwCallsign;
	bool            m_lockFileEnabled;
	std::string     m_lockFileName;
	bool            m_fixedMode;

	void readParams();
	bool createModem();
	bool createDMRNetwork();

	void remoteControl();
	void processModeCommand(unsigned char mode, unsigned int timeout);
	void processEnableCommand(bool& mode, bool enabled);

	void setMode(unsigned char mode);

	void createLockFile(const char* mode) const;
	void removeLockFile() const;
};

#endif
