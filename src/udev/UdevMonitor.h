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

#include <QObject>

#include <libudev.h>

namespace Carbon {

class UdevMonitor : public QObject {
    Q_OBJECT

public:
    explicit UdevMonitor(UdevContext* context, QObject* parent = nullptr);
    ~UdevMonitor() override;

    bool isValid() const;

    void enable();

    void filterBySubsystem(const QByteArray& subsystem);
    void filterByTag(const QByteArray& tag);

signals:
    void deviceAdded(const UdevDevice& device);
    void deviceRemoved(const UdevDevice& device);
    void deviceChanged(const UdevDevice& device);

private:
    void handleUdevEvents();

    UdevContext m_context;
    udev_monitor* m_monitor;
    bool m_enabled = false;

    Q_DISABLE_COPY(UdevMonitor)
};

} // namespace Carbon
