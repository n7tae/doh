/*
 *  Copyright (C) 2026 by Thomas A. Early N7TAE
 *	Copyright (C) 2015,2016,2017 by Jonathan Naylor, G4KLX
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; version 2 of the License.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 */

#pragma once

#include <cstdint>

#include "DMRDefines.h"

class CDMRData
{
public:
	CDMRData(const CDMRData& data);
	CDMRData();
	~CDMRData();

	CDMRData& operator=(const CDMRData& data);

	unsigned getSlotNo() const;
	void setSlotNo(unsigned slotNo);

	unsigned getSrcId() const;
	void setSrcId(unsigned id);

	unsigned getDstId() const;
	void setDstId(unsigned id);

	FLCO getFLCO() const;
	void setFLCO(FLCO flco);

	uint8_t getN() const;
	void setN(uint8_t n);

	uint8_t getSeqNo() const;
	void setSeqNo(uint8_t seqNo);

	uint8_t getDataType() const;
	void setDataType(uint8_t dataType);

	uint8_t getBER() const;
	void setBER(uint8_t ber);

	uint8_t getRSSI() const;
	void setRSSI(uint8_t rssi);

	void setData(const uint8_t *buffer);
	unsigned getData(uint8_t *buffer) const;

private:
	unsigned   m_slotNo;
	uint8_t *m_data;
	unsigned   m_srcId;
	unsigned   m_dstId;
	FLCO           m_flco;
	uint8_t  m_dataType;
	uint8_t  m_seqNo;
	uint8_t  m_n;
	uint8_t  m_ber;
	uint8_t  m_rssi;
};
