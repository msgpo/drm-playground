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

#include "UdevContext.h"
#include "UdevDevice.h"

#include <sys/stat.h>

UdevContext::UdevContext()
    : m_udev(udev_new())
{
}

UdevContext::UdevContext(const UdevContext& other)
    : m_udev(udev_ref(other.m_udev))
{
}

UdevContext::UdevContext(UdevContext&& other)
    : m_udev(std::exchange(other.m_udev, nullptr))
{
}

UdevContext::~UdevContext()
{
    if (m_udev)
        udev_unref(m_udev);
}

UdevContext& UdevContext::operator=(const UdevContext& other)
{
    if (m_udev)
        udev_unref(m_udev);

    if (other.m_udev)
        m_udev = udev_ref(other.m_udev);
    else
        m_udev = nullptr;

    return *this;
}

UdevContext& UdevContext::operator=(UdevContext&& other)
{
    if (m_udev)
        udev_unref(m_udev);

    m_udev = std::exchange(other.m_udev, nullptr);

    return *this;
}

bool UdevContext::isValid() const
{
    return m_udev;
}

UdevDevice UdevContext::deviceFromFileName(const QString& fileName) const
{
    if (!m_udev)
        return nullptr;

    struct stat statBuf;
    if (stat(fileName.toUtf8(), &statBuf))
        return nullptr;

    char type;
    if (S_ISBLK(statBuf.st_mode))
        type = 'b';
    else if (S_ISCHR(statBuf.st_mode))
        type = 'c';
    else
        return nullptr;

    return udev_device_new_from_devnum(m_udev, type, statBuf.st_rdev);
}

UdevDevice UdevContext::deviceFromId(const QString& id) const
{
    if (m_udev)
        return udev_device_new_from_device_id(m_udev, id.toUtf8());
    return nullptr;
}

UdevDevice UdevContext::deviceFromSubSystemAndName(const QString& subSystem, const QString& name) const
{
    if (m_udev)
        return udev_device_new_from_subsystem_sysname(m_udev, subSystem.toUtf8(), name.toUtf8());
    return nullptr;
}

UdevDevice UdevContext::deviceFromSysPath(const QString& path) const
{
    if (m_udev)
        return udev_device_new_from_syspath(m_udev, path.toUtf8());
    return nullptr;
}

UdevContext::operator udev*() const
{
    return m_udev;
}
