/*
 *   Copyright (C) 2026 by Thomas A. Early N7TAE
 *   Copyright (C) 2015,2016,2019 by Jonathan Naylor G4KLX
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

#include "DMRDefines.h"

class CDMRLC
{
public:
	CDMRLC(FLCO flco, unsigned srcId, unsigned dstId);
	CDMRLC(const uint8_t *bytes);
	CDMRLC(const bool *bits);
	CDMRLC();
	~CDMRLC();

	void getData(uint8_t *bytes) const;
	void getData(bool *bits) const;

	bool getPF() const;
	void setPF(bool pf);

	FLCO getFLCO() const;
	void setFLCO(FLCO flco);

	bool getOVCM() const;
	void setOVCM(bool ovcm);

	uint8_t getFID() const;
	void setFID(uint8_t fid);

	unsigned getSrcId() const;
	void setSrcId(unsigned id);

	unsigned getDstId() const;
	void setDstId(unsigned id);

private:
	bool          m_PF;
	bool          m_R;
	FLCO          m_FLCO;
	uint8_t       m_FID;
	uint8_t       m_options;
	unsigned  m_srcId;
	unsigned  m_dstId;
};


