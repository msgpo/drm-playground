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

#include "UdevDevice.h"
#include "UdevContext.h"

UdevDevice::UdevDevice()
    : m_device(nullptr)
{
}

UdevDevice::UdevDevice(udev_device* device)
    : m_device(device)
{
}

UdevDevice::UdevDevice(const UdevDevice& other)
    : m_device(udev_device_ref(other.m_device))
{
}

UdevDevice::UdevDevice(UdevDevice&& other)
    : m_device(std::exchange(other.m_device, nullptr))
{
}

UdevDevice::~UdevDevice()
{
    if (m_device)
        udev_device_unref(m_device);
}

UdevDevice& UdevDevice::operator=(const UdevDevice& other)
{
    if (m_device)
        udev_device_unref(m_device);

    if (other.m_device)
        m_device = udev_device_ref(other.m_device);
    else
        m_device = nullptr;

    return *this;
}

UdevDevice& UdevDevice::operator=(UdevDevice&& other)
{
    if (m_device)
        udev_device_unref(m_device);

    m_device = std::exchange(other.m_device, nullptr);

    return *this;
}

bool UdevDevice::isValid() const
{
    return m_device;
}

UdevDevice::Types UdevDevice::types() const
{
    if (!m_device)
        return UnknownType;

    Types types = {};

    if (subsystem() == "drm") {
        types |= GpuType;

        if (udev_device* pci = udev_device_get_parent_with_subsystem_devtype(m_device, "pci", nullptr)) {
            if (qstrcmp(udev_device_get_sysattr_value(pci, "boot_vga"), "1") == 0) {
                types |= PrimaryGpuType;
            }
        }
    }

    if (property(QByteArrayLiteral("ID_INPUT_KEYBOARD")) == "1")
        types |= KeyboardType;

    if (property(QByteArrayLiteral("ID_INPUT_KEY")) == "1")
        types |= KeyboardType;

    if (property(QByteArrayLiteral("ID_INPUT_MOUSE")) == "1")
        types |= MouseType;

    if (property(QByteArrayLiteral("ID_INPUT_TOUCHPAD")) == "1")
        types |= TouchpadType;

    if (property(QByteArrayLiteral("ID_INPUT_TABLET")) == "1")
        types |= TabletType;

    if (property(QByteArrayLiteral("ID_INPUT_TOUCHSCREEN")) == "1")
        types |= TouchscreenType;

    if (property(QByteArrayLiteral("ID_INPUT_JOYSTICK")) == "1")
        types |= JoystickType;

    return types;
}

UdevDevice UdevDevice::parent() const
{
    if (m_device)
        return udev_device_get_parent(m_device);
    return nullptr;
}

QString UdevDevice::sysPath() const
{
    if (m_device)
        return QString::fromUtf8(udev_device_get_syspath(m_device));
    return QString();
}

QString UdevDevice::sysName() const
{
    if (m_device)
        return QString::fromUtf8(udev_device_get_sysname(m_device));
    return QString();
}

QString UdevDevice::sysNumber() const
{
    if (m_device)
        return QString::fromUtf8(udev_device_get_sysnum(m_device));
    return QString();
}

QString UdevDevice::devicePath() const
{
    if (m_device)
        return QString::fromUtf8(udev_device_get_devpath(m_device));
    return QString();
}

QString UdevDevice::deviceNode() const
{
    if (m_device)
        return QString::fromUtf8(udev_device_get_devnode(m_device));
    return QString();
}

dev_t UdevDevice::deviceNumber() const
{
    if (m_device)
        return udev_device_get_devnum(m_device);
    return -1;
}

QString UdevDevice::subsystem() const
{
    if (m_device)
        return QString::fromUtf8(udev_device_get_subsystem(m_device));
    return QString();
}

QString UdevDevice::driver() const
{
    if (m_device)
        return QString::fromUtf8(udev_device_get_driver(m_device));
    return QString();
}

QString UdevDevice::seat() const
{
    if (!m_device)
        return QString();

    QString seat = QString::fromLatin1(udev_device_get_property_value(m_device, "ID_SEAT"));
    if (seat.isEmpty())
        return QStringLiteral("seat0");

    return seat;
}

QByteArray UdevDevice::property(const QString& name) const
{
    if (m_device)
        return udev_device_get_property_value(m_device, name.toUtf8());
    return QByteArray();
}

UdevDevice::operator udev_device*() const
{
    return m_device;
}
