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
class NativeRenderer;

class DrmDevice : public QObject {
    Q_OBJECT

public:
    DrmDevice(NativeContext* context, const QString& path, QObject* parent);
    ~DrmDevice() override;

    /**
     * This enum type is used to specify device capabilities.
     */
    enum DeviceCapability {
        /**
         * This device can export buffers over dma-buf.
         */
        DeviceCapabilityExportBuffer,
        /**
         * This device can import buffer over dma-buf.
         */
        DeviceCapabilityImportBuffer,
        /**
         * This device supports buffer modifiers.
         */
        DeviceCapabilityBufferModifier,
    };

    /**
     * Checks whether this device supports given capability.
     *
     * If the capability is supported, @c true is returned. Otherwise @c false.
     */
    bool supports(DeviceCapability capability) const;

    /**
     * This enum type is used to specify capabilities requested
     * by the client.
     */
    enum ClientCapability {
        /**
         * The client would like to use atomic modesetting API.
         */
        ClientCapabilityAtomic,
        /**
         * The client would like to have access to planes.
         */
        ClientCapabilityUniversalPlanes,
    };

    /**
     * Enables the given client capability.
     *
     * If the device couldn't enable the given capability, @c false is returned.
     */
    bool enable(ClientCapability capability);

    /**
     * Whether this device is valid.
     */
    bool isValid() const;

    /**
     * Returns the file descriptor of the corresponding device.
     */
    int fd() const;

    /**
     * Returns path of the corresponding drm device.
     */
    QString path() const;

    /**
     * Returns the device memory allocator.
     */
    DrmAllocator* allocator() const;

    /**
     * Returns the list of available connectors on this device.
     */
    DrmConnectorList connectors() const;

    /**
     * Returns a connector with the given id.
     *
     * If there is no such connector, @c null is returned.
     */
    DrmConnector* findConnector(uint32_t id) const;

    /**
     * Returns the list of available crtcs on this device.
     */
    DrmCrtcList crtcs() const;

    /**
     * Returns a CRTC with the given id.
     *
     * If there is no such CRTC, @c null is returned.
     */
    DrmCrtc* findCrtc(uint32_t id) const;

    /**
     * Returns all CRTCs matching the corresponding mask.
     */
    DrmCrtcList findCrtcs(uint32_t mask) const;

    /**
     * Returns the list of available planes on this device.
     */
    DrmPlaneList planes() const;

    /**
     * Returns all planes of the given type.
     */
    DrmPlaneList planes(PlaneType type) const;

    /**
     * Returns a list of outputs connected to this device.
     */
    DrmOutputList outputs() const;

    /**
     * Returns the renderer.
     **/
    NativeRenderer* renderer() const;

signals:
    /**
     * Emitted when a connector is connected or disconnected.
     */
    void connectorsChanged();

private:
    void scanConnectors();
    void scanCrtcs();
    void scanPlanes();
    void reroute();

    NativeContext* m_context;
    NativeRenderer* m_renderer = nullptr;
    DrmAllocator* m_allocator = nullptr;
    DrmConnectorList m_connectors;
    DrmCrtcList m_crtcs;
    DrmPlaneList m_planes;
    DrmOutputList m_outputs;
    QString m_path;
    int m_fd = -1;
    bool m_supportsExportBuffer = false;
    bool m_supportsImportBuffer = false;
    bool m_supportsBufferModifier = false;
    bool m_isValid = false;

    friend class DrmDeviceManager;

    Q_DISABLE_COPY(DrmDevice)
};
