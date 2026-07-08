/*
*   Copyright (C) Thomas A. Early N7TAE
*	Copyright (C) 2016 by Jonathan Naylor, G4KLX
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

class CDMRTrellis
{
public:
	CDMRTrellis();
	~CDMRTrellis();

	bool decode(const uint8_t *data, uint8_t *payload);
	void encode(const uint8_t *payload, uint8_t *data);

private:
	void deinterleave(const uint8_t *in, signed char *dibits) const;
	void interleave(const signed char *dibits, uint8_t *out) const;
	void dibitsToPoints(const signed char *dibits, uint8_t *points) const;
	void pointsToDibits(const uint8_t *points, signed char *dibits) const;
	void bitsToTribits(const uint8_t *payload, uint8_t *tribits) const;
	void tribitsToBits(const uint8_t *tribits, uint8_t *payload) const;
	bool fixCode(uint8_t *points, unsigned failPos, uint8_t *payload) const;
	unsigned checkCode(const uint8_t *points, uint8_t *tribits) const;
};
