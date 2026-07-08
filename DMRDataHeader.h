/*
 *   Copyright (C) 2026 by Thomas A. Early N7TAE
 *   Copyright (C) 2015,2016,2017 by Jonathan Naylor G4KLX
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

class CDMRDataHeader
{
public:
	CDMRDataHeader();
	~CDMRDataHeader();

	bool put(const uint8_t *bytes);

	void get(uint8_t *bytes) const;

	bool      getGI() const;

	unsigned  getSrcId() const;
	unsigned  getDstId() const;

	unsigned  getBlocks() const;

	CDMRDataHeader& operator=(const CDMRDataHeader& header);

private:
	uint8_t   *m_data;
	bool       m_GI;
	bool       m_A;
	unsigned   m_srcId;
	unsigned   m_dstId;
	unsigned   m_blocks;
	bool       m_F;
	bool       m_S;
	uint8_t    m_Ns;
};
