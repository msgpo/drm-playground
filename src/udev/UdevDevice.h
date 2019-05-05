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

#include <libudev.h>

class UdevDevice {
public:
    enum Type {
        Unknown = 0,
        Gpu = 1 << 0,
        PrimaryGpu = 1 << 1,
    };
    Q_DECLARE_FLAGS(Types, Type)

    explicit UdevDevice();
    UdevDevice(const UdevDevice& other);
    UdevDevice(UdevDevice&& other);
    ~UdevDevice();

    UdevDevice& operator=(const UdevDevice& other);
    UdevDevice& operator=(UdevDevice&& other);

    bool isValid() const;

    Types types() const;
    UdevDevice parent() const;

    QString sysfsPath() const;
    QString sysfsName() const;
    QString sysfsNumber() const;

    QString devicePath() const;
    QString deviceNode() const;
    dev_t deviceNumber() const;

    QString driver() const;
    QString seat() const;
    QString subsystem() const;

    QByteArray property(const QString& name) const;

    operator udev_device*() const;

private:
    UdevDevice(udev_device* device);

    udev_device* m_device;

    friend class UdevContext;
    friend class UdevMonitor;
};
