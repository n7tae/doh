/*
 *	Copyright (C) 2015,2016,2017 Jonathan Naylor, G4KLX
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

#include "DMRData.h"
#include "DMRDefines.h"
#include "Utils.h"
#include "Log.h"

#include <cstdio>
#include <cstring>
#include <cassert>


CDMRData::CDMRData(const CDMRData& data) :
	m_slotNo(data.m_slotNo),
	m_data(NULL),
	m_srcId(data.m_srcId),
	m_dstId(data.m_dstId),
	m_flco(data.m_flco),
	m_dataType(data.m_dataType),
	m_seqNo(data.m_seqNo),
	m_n(data.m_n),
	m_ber(data.m_ber),
	m_rssi(data.m_rssi)
{
	m_data = new uint8_t[2U * DMR_FRAME_LENGTH_BYTES];
	::memcpy(m_data, data.m_data, 2U * DMR_FRAME_LENGTH_BYTES);
}

CDMRData::CDMRData() :
	m_slotNo(1U),
	m_data(NULL),
	m_srcId(0U),
	m_dstId(0U),
	m_flco(FLCO_GROUP),
	m_dataType(0U),
	m_seqNo(0U),
	m_n(0U),
	m_ber(0U),
	m_rssi(0U)
{
	m_data = new uint8_t[2U * DMR_FRAME_LENGTH_BYTES];
}

CDMRData::~CDMRData()
{
	delete[] m_data;
}

CDMRData& CDMRData::operator=(const CDMRData& data)
{
	if (this != &data)
	{
		::memcpy(m_data, data.m_data, DMR_FRAME_LENGTH_BYTES);

		m_slotNo   = data.m_slotNo;
		m_srcId    = data.m_srcId;
		m_dstId    = data.m_dstId;
		m_flco     = data.m_flco;
		m_dataType = data.m_dataType;
		m_seqNo    = data.m_seqNo;
		m_n        = data.m_n;
		m_ber      = data.m_ber;
		m_rssi     = data.m_rssi;
	}

	return *this;
}

unsigned CDMRData::getSlotNo() const
{
	return m_slotNo;
}

void CDMRData::setSlotNo(unsigned slotNo)
{
	assert(slotNo == 1U || slotNo == 2U);

	m_slotNo = slotNo;
}

uint8_t CDMRData::getDataType() const
{
	return m_dataType;
}

void CDMRData::setDataType(uint8_t dataType)
{
	m_dataType = dataType;
}

unsigned CDMRData::getSrcId() const
{
	return m_srcId;
}

void CDMRData::setSrcId(unsigned id)
{
	m_srcId = id;
}

unsigned CDMRData::getDstId() const
{
	return m_dstId;
}

void CDMRData::setDstId(unsigned id)
{
	m_dstId = id;
}

FLCO CDMRData::getFLCO() const
{
	return m_flco;
}

void CDMRData::setFLCO(FLCO flco)
{
	m_flco = flco;
}

uint8_t CDMRData::getSeqNo() const
{
	return m_seqNo;
}

void CDMRData::setSeqNo(uint8_t seqNo)
{
	m_seqNo = seqNo;
}

uint8_t CDMRData::getN() const
{
	return m_n;
}

void CDMRData::setN(uint8_t n)
{
	m_n = n;
}

uint8_t CDMRData::getBER() const
{
	return m_ber;
}

void CDMRData::setBER(uint8_t ber)
{
	m_ber = ber;
}

uint8_t CDMRData::getRSSI() const
{
	return m_rssi;
}

void CDMRData::setRSSI(uint8_t rssi)
{
	m_rssi = rssi;
}

unsigned CDMRData::getData(uint8_t *buffer) const
{
	assert(buffer != NULL);

	::memcpy(buffer, m_data, DMR_FRAME_LENGTH_BYTES);

	return DMR_FRAME_LENGTH_BYTES;
}

void CDMRData::setData(const uint8_t *buffer)
{
	assert(buffer != NULL);

	::memcpy(m_data, buffer, DMR_FRAME_LENGTH_BYTES);
}
