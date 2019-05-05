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

#include <QString>

#include <libudev.h>

class UdevDevice;

class UdevContext {
public:
    explicit UdevContext();
    UdevContext(const UdevContext& other);
    UdevContext(UdevContext&& other);
    ~UdevContext();

    UdevContext& operator=(const UdevContext& other);
    UdevContext& operator=(UdevContext&& other);

    bool isValid() const;

    UdevDevice deviceFromFileName(const QString& fileName) const;
    UdevDevice deviceFromId(const QString& id) const;
    UdevDevice deviceFromSubSystemAndName(const QString& subSystem, const QString& name) const;
    UdevDevice deviceFromSysPath(const QString& path) const;

    operator udev*() const;

private:
    udev* m_udev;
};
