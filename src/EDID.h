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

#pragma once

#include <QByteArray>
#include <QSize>

class EDID {
public:
    EDID(const void* data, uint32_t size);

    /**
     * Whether this instance of EDID is valid.
     */
    bool isValid() const;

    /**
     * Returns physical dimensions of the monitor, in millimeters.
     */
    QSize physicalSize() const;

    /**
     * Returns human readable name of the monitor.
     */
    QByteArray monitorName() const;

    /**
     * Returns manufacturer of the monitor.
     */
    QByteArray manufacturer() const;

    /**
     * Returns the serial number of the monitor.
     */
    QByteArray serialNumber() const;

    /**
     * Returns week when the monitor was manufactured.
     */
    int manufactureWeek() const;

    /**
     * Returns year when the monitor was manufactured.
     *
     * It is the model year if manufactureWeek() returns 255.
     */
    int manufactureYear() const;

private:
    QSize m_physicalSize;
    QByteArray m_monitorName;
    QByteArray m_manufacturer;
    QByteArray m_serialNumber;
    int m_manufactureWeek;
    int m_manufactureYear;
    bool m_isValid = false;
};
