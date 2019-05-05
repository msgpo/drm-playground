/*
 * Copyright (C) 2019 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "EDID.h"

static bool verifyHeader(const uint8_t* data)
{
    if (data[0] != 0x0 || data[7] != 0x0)
        return false;

    return std::all_of(data + 1, data + 7,
        [](uint8_t byte) { return byte == 0xff; });
}

static QSize parsePhysicalSize(const uint8_t* data)
{
    // Convert physical size to millimeters.
    return QSize(data[21], data[22]) * 10;
}

static QByteArray parseMonitorName(const uint8_t* data)
{
    for (int i = 72; i <= 108; i += 18) {
        // Skip the block if it isn't used as monitor descriptor.
        if (data[i])
            continue;
        if (data[i + 1])
            continue;

        // We have found the monitor name, it's stored as ASCII.
        if (data[i + 3] == 0xfc)
            return QByteArray(reinterpret_cast<const char*>(&data[i + 5]), 12).trimmed();
    }

    return QByteArray();
}

static QByteArray parseManufacturer(const uint8_t* data)
{
    const int manufacturerId = (data[8] << 8) | data[9];

#define ID(a, b, c) ((((a)&0x1f) << 10) | (((b)&0x1f) << 5) | ((c)&0x1f))
    switch (manufacturerId) {
    case ID('A', 'A', 'A'):
        return QByteArrayLiteral("Avolites Ltd");
    case ID('A', 'C', 'I'):
        return QByteArrayLiteral("Ancor Communications Inc");
    case ID('A', 'C', 'R'):
        return QByteArrayLiteral("Acer Technologies");
    case ID('A', 'D', 'A'):
        return QByteArrayLiteral("Addi-Data GmbH");
    case ID('A', 'P', 'P'):
        return QByteArrayLiteral("Apple Computer Inc");
    case ID('A', 'S', 'K'):
        return QByteArrayLiteral("Ask A/S");
    case ID('A', 'V', 'T'):
        return QByteArrayLiteral("Avtek (Electronics) Pty Ltd");
    case ID('B', 'M', 'D'):
        return QByteArrayLiteral("Blackmagic Design");
    case ID('B', 'N', 'O'):
        return QByteArrayLiteral("Bang & Olufsen");
    case ID('B', 'O', 'E'):
        return QByteArrayLiteral("BOE");
    case ID('C', 'M', 'N'):
        return QByteArrayLiteral("Chimei Innolux Corporation");
    case ID('C', 'M', 'O'):
        return QByteArrayLiteral("Chi Mei Optoelectronics corp.");
    case ID('C', 'R', 'O'):
        return QByteArrayLiteral("Extraordinary Technologies PTY Limited");
    case ID('D', 'E', 'L'):
        return QByteArrayLiteral("Dell Inc.");
    case ID('D', 'G', 'C'):
        return QByteArrayLiteral("Data General Corporation");
    case ID('D', 'O', 'N'):
        return QByteArrayLiteral("DENON, Ltd.");
    case ID('E', 'N', 'C'):
        return QByteArrayLiteral("Eizo Nanao Corporation");
    case ID('E', 'P', 'H'):
        return QByteArrayLiteral("Epiphan Systems Inc.");
    case ID('E', 'X', 'P'):
        return QByteArrayLiteral("Data Export Corporation");
    case ID('F', 'N', 'I'):
        return QByteArrayLiteral("Funai Electric Co., Ltd.");
    case ID('F', 'U', 'S'):
        return QByteArrayLiteral("Fujitsu Siemens Computers GmbH");
    case ID('G', 'S', 'M'):
        return QByteArrayLiteral("Goldstar Company Ltd");
    case ID('H', 'I', 'Q'):
        return QByteArrayLiteral("Kaohsiung Opto Electronics Americas, Inc.");
    case ID('H', 'S', 'D'):
        return QByteArrayLiteral("HannStar Display Corp");
    case ID('H', 'T', 'C'):
        return QByteArrayLiteral("Hitachi Ltd");
    case ID('H', 'W', 'P'):
        return QByteArrayLiteral("Hewlett Packard");
    case ID('I', 'N', 'T'):
        return QByteArrayLiteral("Interphase Corporation");
    case ID('I', 'N', 'X'):
        return QByteArrayLiteral("Communications Supply Corporation (A division of WESCO)");
    case ID('I', 'T', 'E'):
        return QByteArrayLiteral("Integrated Tech Express Inc");
    case ID('I', 'V', 'M'):
        return QByteArrayLiteral("Iiyama North America");
    case ID('L', 'E', 'N'):
        return QByteArrayLiteral("Lenovo Group Limited");
    case ID('M', 'A', 'X'):
        return QByteArrayLiteral("Rogen Tech Distribution Inc");
    case ID('M', 'E', 'G'):
        return QByteArrayLiteral("Abeam Tech Ltd");
    case ID('M', 'E', 'I'):
        return QByteArrayLiteral("Panasonic Industry Company");
    case ID('M', 'T', 'C'):
        return QByteArrayLiteral("Mars-Tech Corporation");
    case ID('M', 'T', 'X'):
        return QByteArrayLiteral("Matrox");
    case ID('N', 'E', 'C'):
        return QByteArrayLiteral("NEC Corporation");
    case ID('N', 'E', 'X'):
        return QByteArrayLiteral("Nexgen Mediatech Inc.,");
    case ID('O', 'N', 'K'):
        return QByteArrayLiteral("ONKYO Corporation");
    case ID('O', 'R', 'N'):
        return QByteArrayLiteral("ORION ELECTRIC CO., LTD.");
    case ID('O', 'T', 'M'):
        return QByteArrayLiteral("Optoma Corporation");
    case ID('O', 'V', 'R'):
        return QByteArrayLiteral("Oculus VR, Inc.");
    case ID('P', 'H', 'L'):
        return QByteArrayLiteral("Philips Consumer Electronics Company");
    case ID('P', 'I', 'O'):
        return QByteArrayLiteral("Pioneer Electronic Corporation");
    case ID('P', 'N', 'R'):
        return QByteArrayLiteral("Planar Systems, Inc.");
    case ID('Q', 'D', 'S'):
        return QByteArrayLiteral("Quanta Display Inc.");
    case ID('R', 'A', 'T'):
        return QByteArrayLiteral("Rent-A-Tech");
    case ID('R', 'E', 'N'):
        return QByteArrayLiteral("Renesas Technology Corp.");
    case ID('S', 'A', 'M'):
        return QByteArrayLiteral("Samsung Electric Company");
    case ID('S', 'A', 'N'):
        return QByteArrayLiteral("Sanyo Electric Co.,Ltd.");
    case ID('S', 'C', 'E'):
        return QByteArrayLiteral("Sun Corporation");
    case ID('S', 'E', 'C'):
        return QByteArrayLiteral("Seiko Epson Corporation");
    case ID('S', 'H', 'P'):
        return QByteArrayLiteral("Sharp Corporation");
    case ID('S', 'I', 'I'):
        return QByteArrayLiteral("Silicon Image, Inc.");
    case ID('S', 'N', 'Y'):
        return QByteArrayLiteral("Sony");
    case ID('S', 'T', 'D'):
        return QByteArrayLiteral("STD Computer Inc");
    case ID('S', 'V', 'S'):
        return QByteArrayLiteral("SVSI");
    case ID('S', 'Y', 'N'):
        return QByteArrayLiteral("Synaptics Inc");
    case ID('T', 'C', 'L'):
        return QByteArrayLiteral("Technical Concepts Ltd");
    case ID('T', 'O', 'P'):
        return QByteArrayLiteral("Orion Communications Co., Ltd.");
    case ID('T', 'S', 'B'):
        return QByteArrayLiteral("Toshiba America Info Systems Inc");
    case ID('T', 'S', 'T'):
        return QByteArrayLiteral("Transtream Inc");
    case ID('V', 'E', 'S'):
        return QByteArrayLiteral("Vestel Elektronik Sanayi ve Ticaret A. S.");
    case ID('V', 'I', 'T'):
        return QByteArrayLiteral("Visitech AS");
    case ID('V', 'I', 'Z'):
        return QByteArrayLiteral("VIZIO, Inc");
    case ID('V', 'S', 'C'):
        return QByteArrayLiteral("ViewSonic Corporation");
    case ID('Y', 'M', 'H'):
        return QByteArrayLiteral("Yamaha Corporation");
    case ID('U', 'N', 'K'):
    default:
        return QByteArrayLiteral("Unknown");
    }
#undef ID
}

static QByteArray parseSerialNumber(const uint8_t* data)
{
    uint32_t serialNumber = data[12];
    serialNumber |= uint32_t(data[13]) << 8;
    serialNumber |= uint32_t(data[14]) << 16;
    serialNumber |= uint32_t(data[15]) << 24;
    if (serialNumber)
        return QByteArray::number(serialNumber);

    for (int i = 72; i <= 108; i += 18) {
        // Skip the block if it isn't used as monitor descriptor.
        if (data[i])
            continue;
        if (data[i + 1])
            continue;

        // We have found the serial number, it's stored as ASCII.
        if (data[i + 3])
            return QByteArray(reinterpret_cast<const char*>(&data[i + 5]), 12).trimmed();
    }

    return QByteArray();
}

static int parseManufactureWeek(const uint8_t* data)
{
    return data[16];
}

static int parseManufactureYear(const uint8_t* data)
{
    return 1990 + data[17];
}

EDID::EDID(const void* data, uint32_t size)
{
    const uint8_t* bytes = static_cast<const uint8_t*>(data);

    if (size < 128)
        return;

    if (!verifyHeader(bytes))
        return;

    m_physicalSize = parsePhysicalSize(bytes);
    m_monitorName = parseMonitorName(bytes);
    m_manufacturer = parseManufacturer(bytes);
    m_serialNumber = parseSerialNumber(bytes);
    m_manufactureWeek = parseManufactureWeek(bytes);
    m_manufactureYear = parseManufactureYear(bytes);
    m_valid = true;
}

bool EDID::isValid() const
{
    return m_valid;
}

QByteArray EDID::monitorName() const
{
    return m_monitorName;
}

QSize EDID::physicalSize() const
{
    return m_physicalSize;
}

QByteArray EDID::manufacturer() const
{
    return m_manufacturer;
}

QByteArray EDID::serialNumber() const
{
    return m_serialNumber;
}

int EDID::manufactureWeek() const
{
    return m_manufactureWeek;
}

int EDID::manufactureYear() const
{
    return m_manufactureYear;
}
