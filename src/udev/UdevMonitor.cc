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

#include "UdevMonitor.h"
#include "UdevDevice.h"

#include <QSocketNotifier>

UdevMonitor::UdevMonitor(UdevContext* context, QObject* parent)
    : QObject(parent)
    , m_context(*context)
    , m_monitor(udev_monitor_new_from_netlink(*context, "udev"))
{
}

UdevMonitor::~UdevMonitor()
{
    if (m_monitor)
        udev_monitor_unref(m_monitor);
}

bool UdevMonitor::isValid() const
{
    return m_monitor;
}

void UdevMonitor::enable()
{
    if (m_enabled)
        return;

    udev_monitor_enable_receiving(m_monitor);

    const int fd = udev_monitor_get_fd(m_monitor);
    QSocketNotifier* notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &UdevMonitor::handleUdevEvents);

    m_enabled = true;
}

void UdevMonitor::filterBySubsystem(const QByteArray& subsystem)
{
    if (!m_monitor)
        return;

    udev_monitor_filter_add_match_subsystem_devtype(m_monitor, subsystem, nullptr);

    if (m_enabled)
        udev_monitor_filter_update(m_monitor);
}

void UdevMonitor::filterByTag(const QByteArray& tag)
{
    if (!m_monitor)
        return;

    udev_monitor_filter_add_match_tag(m_monitor, tag);

    if (m_enabled)
        udev_monitor_filter_update(m_monitor);
}

void UdevMonitor::handleUdevEvents()
{
    UdevDevice device(udev_monitor_receive_device(m_monitor));
    if (!device.isValid())
        return;

    const QByteArray action = udev_device_get_action(device);
    if (action.isNull())
        return;

    if (action == "add")
        emit deviceAdded(device);
    else if (action == "remove")
        emit deviceRemoved(device);
    else if (action == "change")
        emit deviceChanged(device);
}
