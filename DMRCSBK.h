/*
 *   Copyright (C) 2026 by Thomas A. Early N7TAE
 *   Copyright (C) 2015,2016,2020 by Jonathan Naylor G4KLX
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

#include "DMRDefines.h"

enum CSBKO
{
	CSBKO_NONE           = 0x00,
	CSBKO_UUVREQ         = 0x04,
	CSBKO_UUANSRSP       = 0x05,
	CSBKO_CTCSBK         = 0x07,
	CSBKO_CALL_ALERT     = 0x1F,
	CSBKO_CALL_ALERT_ACK = 0x20,
	CSBKO_NACKRSP        = 0x26,
	CSBKO_BSDWNACT       = 0x38,
	CSBKO_PRECCSBK       = 0x3D
};

class CDMRCSBK
{
public:
	CDMRCSBK();
	~CDMRCSBK();

	bool put(const uint8_t *bytes);

	void get(uint8_t *bytes) const;

	// Generic fields
	CSBKO   getCSBKO() const;
	uint8_t getFID() const;

	// Set/Get the OVCM bit in the supported CSBKs
	bool getOVCM() const;
	void setOVCM(bool ovcm);

	// For BS Dwn Act
	unsigned  getBSId() const;

	// For Pre
	bool getGI() const;

	unsigned  getSrcId() const;
	unsigned  getDstId() const;

	bool          getDataContent() const;
	uint8_t getCBF() const;

	void          setCBF(uint8_t cbf);

private:
	uint8_t       *m_data;
	CSBKO          m_CSBKO;
	uint8_t        m_FID;
	bool           m_GI;
	unsigned   m_bsId;
	unsigned   m_srcId;
	unsigned   m_dstId;
	bool           m_dataContent;
	uint8_t        m_CBF;
};
