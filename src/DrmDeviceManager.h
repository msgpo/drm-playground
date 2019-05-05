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

#include "globals.h"

#include <QObject>

class NativeContext;
class UdevDevice;
class UdevMonitor;

class DrmDeviceManager : public QObject {
    Q_OBJECT

public:
    explicit DrmDeviceManager(NativeContext* context, QObject* parent = nullptr);
    ~DrmDeviceManager() override;

    /**
     * Whether this device manager is valid, i.e. there is a primary device.
     */
    bool isValid() const;

    /**
     * Returns all devices on this platform.
     */
    DrmDeviceList devices() const;

    /**
     * Returns the primary device.
     */
    DrmDevice* primaryDevice() const;

private:
    void add(const UdevDevice& device);
    void remove(const UdevDevice& device);
    void reload(const UdevDevice& device);
    DrmDevice* findDevice(const UdevDevice& device);

    NativeContext* m_context;
    UdevMonitor* m_monitor;
    DrmDeviceList m_devices;
    DrmDevice* m_primaryDevice = nullptr;

    Q_DISABLE_COPY(DrmDeviceManager)
};
