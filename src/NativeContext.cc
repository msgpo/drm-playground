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

#include "NativeContext.h"
#include "session/LogindSessionController.h"
#include "udev/UdevContext.h"

NativeContext::NativeContext(QObject* parent)
    : QObject(parent)
{
    m_udev = std::make_unique<UdevContext>();
    if (!m_udev->isValid())
        return;

    m_sessionController = new LogindSessionController(this);
    if (!m_sessionController->isValid())
        return;
}

NativeContext::~NativeContext()
{
}

bool NativeContext::isValid() const
{
    if (!m_udev || !m_udev->isValid())
        return false;
    if (!m_sessionController || !m_sessionController->isValid())
        return false;
    return true;
}

SessionController* NativeContext::sessionController() const
{
    return m_sessionController;
}

UdevContext* NativeContext::udev() const
{
    return m_udev.get();
}
