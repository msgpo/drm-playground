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

#include "DrmDeviceManager.h"
#include "DrmDevice.h"
#include "NativeContext.h"
#include "session/SessionController.h"
#include "udev/UdevContext.h"
#include "udev/UdevDevice.h"
#include "udev/UdevEnumerator.h"
#include "udev/UdevMonitor.h"

#include <memory>

DrmDeviceManager::DrmDeviceManager(NativeContext* context, QObject* parent)
    : QObject(parent)
    , m_context(context)
{
    UdevEnumerator enumerator(*context->udev());
    enumerator.matchSeat(context->sessionController()->seat());
    enumerator.matchSubsystem(QStringLiteral("drm"));
    enumerator.matchSysfsName(QStringLiteral("card[0-9]*"));

    const QVector<UdevDevice> devices = enumerator.scan();
    for (const UdevDevice& device : devices) {
        if (!(device.types() & UdevDevice::PrimaryGpu))
            continue;

        const QString path = device.deviceNode();
        if (path.isEmpty())
            continue;

        auto dev = std::make_unique<DrmDevice>(m_context, path, this);
        if (!dev->isValid())
            continue;

        if (!dev->enable(DrmDevice::ClientCapabilityAtomic))
            continue;

        dev->scanCrtcs();
        dev->scanPlanes();
        dev->scanConnectors();

        m_devices << dev.release();
    }

    if (m_devices.isEmpty())
        return;

    m_primaryDevice = m_devices.last();

    for (const UdevDevice& device : devices)
        add(device);

    m_monitor = new UdevMonitor(context->udev(), this);
    m_monitor->filterBySubsystem(QStringLiteral("drm"));
    m_monitor->enable();

    connect(m_monitor, &UdevMonitor::deviceAdded, this, &DrmDeviceManager::add);
    connect(m_monitor, &UdevMonitor::deviceRemoved, this, &DrmDeviceManager::remove);
    connect(m_monitor, &UdevMonitor::deviceChanged, this, &DrmDeviceManager::reload);
}

DrmDeviceManager::~DrmDeviceManager()
{
    qDeleteAll(m_devices);
}

bool DrmDeviceManager::isValid() const
{
    return m_primaryDevice;
}

DrmDeviceList DrmDeviceManager::devices() const
{
    return m_devices;
}

DrmDevice* DrmDeviceManager::primaryDevice() const
{
    return m_primaryDevice;
}

void DrmDeviceManager::add(const UdevDevice& device)
{
    if (!(device.types() & UdevDevice::Gpu))
        return;

    if (device.types() & UdevDevice::PrimaryGpu)
        return;

    const QString path = device.deviceNode();
    if (path.isEmpty())
        return;

    auto dev = std::make_unique<DrmDevice>(m_context, path, this);
    if (!dev->isValid())
        return;

    if (!m_primaryDevice->supports(DrmDevice::DeviceCapabilityExportBuffer))
        return;

    if (!dev->supports(DrmDevice::DeviceCapabilityImportBuffer))
        return;

    if (!dev->enable(DrmDevice::ClientCapabilityAtomic))
        return;

    dev->scanCrtcs();
    dev->scanPlanes();
    dev->scanConnectors();

    m_devices << dev.release();
}

void DrmDeviceManager::remove(const UdevDevice& device)
{
    if (!(device.types() & UdevDevice::Gpu))
        return;

    DrmDevice* dev = findDevice(device);
    if (!dev)
        return;

    // TODO: Implement it.
}

void DrmDeviceManager::reload(const UdevDevice& device)
{
    if (!(device.types() & UdevDevice::Gpu))
        return;

    if (DrmDevice* dev = findDevice(device))
        dev->scanConnectors();
}

DrmDevice* DrmDeviceManager::findDevice(const UdevDevice& udev)
{
    for (DrmDevice* device : m_devices) {
        if (device->path() == udev.deviceNode())
            return device;
    }

    return nullptr;
}
