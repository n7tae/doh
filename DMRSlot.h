/*
 *   Copyright (C) Thomas A. Early N7TAE
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

#include <cstdint>
#include <vector>
#include "RSSIInterpolator.h"
#include "DMREmbeddedData.h"
#include "DMRNetwork.h"
#include "DMRTA.h"
#include "RingBuffer.h"
#include "StopWatch.h"
#include "DMRLookup.h"
#include "AMBEFEC.h"
#include "DMRSlot.h"
#include "DMRData.h"
#include "Defines.h"
#include "DashDB.h"
#include "Timer.h"
#include "Modem.h"
#include "DMRLC.h"

enum ACTIVITY_TYPE
{
	ACTIVITY_NONE,
	ACTIVITY_VOICE,
	ACTIVITY_DATA,
	ACTIVITY_CSBK,
	ACTIVITY_EMERG
};

class CDMRSlot
{
public:
	CDMRSlot(unsigned slotNo, unsigned timeout);
	~CDMRSlot();

	bool writeModem(uint8_t *data, unsigned len);

	unsigned readModem(uint8_t *data);

	void writeNetwork(const CDMRData& data);

	void clock();

	bool isBusy() const;

	void enable(bool enabled);

	static void init(unsigned colorCode, bool embeddedLCOnly, bool dumpTAData, unsigned callHang, CModem* modem, IDMRNetwork* network, bool duplex, CDMRLookup* lookup, CRSSIInterpolator* rssiMapper, unsigned jitter, DMR_OVCM_TYPES ovcm, CDashDB *dashDB);

private:
	unsigned               m_slotNo;
	CRingBuffer<uint8_t>       m_queue;
	RPT_RF_STATE               m_rfState;
	RPT_NET_STATE              m_netState;
	CDMREmbeddedData           m_rfEmbeddedLC;
	CDMREmbeddedData*          m_rfEmbeddedData;
	unsigned               m_rfEmbeddedReadN;
	unsigned               m_rfEmbeddedWriteN;
	uint8_t                    m_rfTalkerId;
	CDMRTA                     m_rfTalkerAlias;
	CDMREmbeddedData           m_netEmbeddedLC;
	CDMREmbeddedData*          m_netEmbeddedData;
	unsigned               m_netEmbeddedReadN;
	unsigned               m_netEmbeddedWriteN;
	uint8_t                    m_netTalkerId;
	CDMRLC*                    m_rfLC;
	CDMRLC*                    m_netLC;
	uint8_t                    m_rfSeqNo;
	uint8_t                    m_rfN;
	uint8_t                    m_lastrfN;
	uint8_t                    m_netN;
	CTimer                     m_networkWatchdog;
	CTimer                     m_rfTimeoutTimer;
	CTimer                     m_netTimeoutTimer;
	CTimer                     m_packetTimer;
	CStopWatch                 m_interval;
	CStopWatch                 m_elapsed;
	unsigned               m_rfFrames;
	unsigned               m_netFrames;
	unsigned               m_netLost;
	CAMBEFEC                   m_fec;
	unsigned               m_rfBits;
	unsigned               m_netBits;
	unsigned               m_rfErrs;
	unsigned               m_netErrs;
	bool                       m_rfTimeout;
	bool                       m_netTimeout;
	uint8_t                   *m_lastFrame;
	bool                       m_lastFrameValid;
	uint8_t                    m_rssi;
	uint8_t                    m_maxRSSI;
	uint8_t                    m_minRSSI;
	unsigned               m_aveRSSI;
	unsigned               m_rssiCount;
	bool                       m_enabled;
	FILE*                      m_fp;

	static CDashDB            *m_dashDB;

	static unsigned        m_colorCode;

	static bool                m_embeddedLCOnly;
	static bool                m_dumpTAData;

	static CModem             *m_modem;
	static IDMRNetwork        *m_network;
	static bool                m_duplex;
	static CDMRLookup         *m_lookup;
	static unsigned        m_hangCount;
	static DMR_OVCM_TYPES      m_ovcm;

	static CRSSIInterpolator  *m_rssiMapper;

	static unsigned        m_jitterTime;
	static unsigned        m_jitterSlots;

	static uint8_t            *m_idle;

	static FLCO                m_flco1;
	static uint8_t             m_id1;
	static ACTIVITY_TYPE       m_activity1;
	static FLCO                m_flco2;
	static uint8_t             m_id2;
	static ACTIVITY_TYPE       m_activity2;

	void logGPSPosition(const uint8_t *data);

	void writeQueueRF(const uint8_t *data);
	void writeQueueNet(const uint8_t *data);
	void writeNetworkRF(const uint8_t *data, uint8_t dataType, uint8_t errors = 0U);
	void writeNetworkRF(const uint8_t *data, uint8_t dataType, FLCO flco, unsigned srcId, unsigned dstId, uint8_t errors = 0U);

	void writeEndRF(bool writeEnd = false);
	void writeEndNet(bool writeEnd = false);

	bool openFile();
	bool writeFile(const uint8_t *data);
	void closeFile();

	bool insertSilence(const uint8_t *data, uint8_t seqNo);
	void insertSilence(unsigned count);

	static void setShortLC(unsigned slotNo, unsigned id, FLCO flco = FLCO_GROUP, ACTIVITY_TYPE type = ACTIVITY_NONE);
};
