/*
 *   Copyright (C) 2026 by Thomas A. Early N7TAE
 *   Copyright (C) 2015,2016,2017,2018 by Jonathan Naylor G4KLX
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

const uint8_t MODE_IDLE    = 0U;
const uint8_t MODE_DSTAR   = 1U;
const uint8_t MODE_DMR     = 2U;
const uint8_t MODE_YSF     = 3U;
const uint8_t MODE_P25     = 4U;
const uint8_t MODE_NXDN    = 5U;
const uint8_t MODE_POCSAG  = 6U;

const uint8_t MODE_FM      = 10U;

const uint8_t MODE_CW      = 98U;
const uint8_t MODE_LOCKOUT = 99U;
const uint8_t MODE_ERROR   = 100U;
const uint8_t MODE_QUIT    = 110U;

const uint8_t TAG_HEADER = 0x00U;
const uint8_t TAG_DATA   = 0x01U;
const uint8_t TAG_LOST   = 0x02U;
const uint8_t TAG_EOT    = 0x03U;

const unsigned  MODEM_DATA_LEN = 220U;

enum HW_TYPE
{
	HWT_MMDVM,
	HWT_DVMEGA,
	HWT_MMDVM_ZUMSPOT,
	HWT_MMDVM_HS_HAT,
	HWT_MMDVM_HS_DUAL_HAT,
	HWT_NANO_HOTSPOT,
	HWT_NANO_DV,
	HWT_D2RG_MMDVM_HS,
	HWT_MMDVM_HS,
	HWT_OPENGD77_HS,
	HWT_SKYBRIDGE,
	HWT_UNKNOWN
};

enum RPT_RF_STATE
{
	RS_RF_LISTENING,
	RS_RF_LATE_ENTRY,
	RS_RF_AUDIO,
	RS_RF_DATA,
	RS_RF_REJECTED,
	RS_RF_INVALID
};

enum RPT_NET_STATE
{
	RS_NET_IDLE,
	RS_NET_AUDIO,
	RS_NET_DATA
};

enum DMR_BEACONS
{
	DMR_BEACONS_OFF,
	DMR_BEACONS_NETWORK,
	DMR_BEACONS_TIMED
};

enum DMR_OVCM_TYPES
{
	DMR_OVCM_OFF,
	DMR_OVCM_RX_ON,
	DMR_OVCM_TX_ON,
	DMR_OVCM_ON
};
