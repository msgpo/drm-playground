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

#include "UdevContext.h"

#include <QVector>

#include <libudev.h>

namespace Carbon {

class UdevDevice;

class UdevEnumerator {
public:
    explicit UdevEnumerator(const UdevContext& context);
    UdevEnumerator(const UdevEnumerator& other);
    UdevEnumerator(UdevEnumerator&& other);
    ~UdevEnumerator();

    UdevEnumerator& operator=(const UdevEnumerator& other);
    UdevEnumerator& operator=(UdevEnumerator&& other);

    bool isValid() const;

    void matchSubsystem(const QByteArray& subsystem);
    void matchSysname(const QByteArray& name);
    void matchSeat(const QByteArray& seat);

    QVector<UdevDevice> scan() const;

private:
    UdevContext m_context;
    udev_enumerate* m_enumerate;
    QByteArray m_seat;
};

} // namespace Carbon
