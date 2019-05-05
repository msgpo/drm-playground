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

#include "DrmDevice.h"
#include "DrmConnector.h"
#include "DrmCrtc.h"
#include "DrmPlane.h"
#include "DrmPointer.h"
#include "NativeContext.h"
#include "session/SessionController.h"

#include <xf86drm.h>

static uint64_t queryCapability(int fd, uint32_t capability)
{
    uint64_t value = 0;

    if (drmGetCap(fd, capability, &value))
        return 0;

    return value;
}

DrmDevice::DrmDevice(NativeContext* context, const QString& path, QObject* parent)
    : QObject(parent)
    , m_context(context)
    , m_path(path)
{
    m_fd = m_context->sessionController()->openRestricted(path);
    if (m_fd == -1)
        return;

    // Check if this is actually a drm device.
    DrmScopedPointer<drmModeRes> resources(drmModeGetResources(m_fd));
    if (!resources)
        return;

    m_supportsExportBuffer = queryCapability(m_fd, DRM_CAP_PRIME) & DRM_PRIME_CAP_EXPORT;
    m_supportsImportBuffer = queryCapability(m_fd, DRM_CAP_PRIME) & DRM_PRIME_CAP_IMPORT;
    m_supportsBufferModifier = queryCapability(m_fd, DRM_CAP_ADDFB2_MODIFIERS);

    m_isValid = true;
}

DrmDevice::~DrmDevice()
{
    qDeleteAll(m_planes);
    qDeleteAll(m_crtcs);
    qDeleteAll(m_connectors);

    if (m_fd != -1)
        m_context->sessionController()->closeRestricted(m_fd);
}

bool DrmDevice::supports(DeviceCapability capability) const
{
    switch (capability) {
    case DeviceCapabilityExportBuffer:
        return m_supportsExportBuffer;
    case DeviceCapabilityImportBuffer:
        return m_supportsImportBuffer;
    case DeviceCapabilityBufferModifier:
        return m_supportsBufferModifier;
    default:
        return false;
    }
}

bool DrmDevice::enable(ClientCapability capability)
{
    switch (capability) {
    case ClientCapabilityAtomic:
        return !drmSetClientCap(m_fd, DRM_CLIENT_CAP_ATOMIC, 1);
    case ClientCapabilityUniversalPlanes:
        return !drmSetClientCap(m_fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
    default:
        Q_UNREACHABLE();
    }
}

bool DrmDevice::isValid() const
{
    return m_isValid;
}

int DrmDevice::fd() const
{
    return m_fd;
}

QString DrmDevice::path() const
{
    return m_path;
}

DrmConnectorList DrmDevice::connectors() const
{
    return m_connectors;
}

DrmConnector* DrmDevice::findConnector(uint32_t id) const
{
    for (DrmConnector* connector : m_connectors) {
        if (connector->id() == id)
            return connector;
    }

    return nullptr;
}

DrmCrtcList DrmDevice::crtcs() const
{
    return m_crtcs;
}

DrmCrtc* DrmDevice::findCrtc(uint32_t id) const
{
    for (DrmCrtc* crtc : m_crtcs) {
        if (crtc->id() == id)
            return crtc;
    }

    return nullptr;
}

DrmCrtcList DrmDevice::findCrtcs(uint32_t mask) const
{
    DrmCrtcList crtcs;

    for (DrmCrtc* crtc : m_crtcs) {
        if (!(mask & (1 << crtc->index())))
            continue;
        crtcs << crtc;
    }

    return crtcs;
}

DrmPlaneList DrmDevice::planes() const
{
    return m_planes;
}

DrmPlaneList DrmDevice::planes(PlaneType type) const
{
    DrmPlaneList planes;

    for (DrmPlane* plane : m_planes) {
        if (plane->type() == type)
            planes << plane;
    }

    return planes;
}

void DrmDevice::scanConnectors()
{
    DrmScopedPointer<drmModeRes> resources(drmModeGetResources(m_fd));
    if (!resources)
        return;

    DrmConnectorList connectors;

    for (int i = 0; i < resources->count_connectors; ++i) {
        const uint32_t connectorId = resources->connectors[i];

        if (DrmConnector* connector = findConnector(connectorId)) {
            connectors << connector;
            continue;
        }

        auto connector = std::make_unique<DrmConnector>(this, connectorId);
        if (!connector->isOnline())
            continue;

        connectors << connector.release();
    }

    const DrmConnectorSet gone = m_connectors.toSet() - connectors.toSet();
    qDeleteAll(gone);

    m_connectors = connectors;
}

void DrmDevice::scanCrtcs()
{
    DrmScopedPointer<drmModeRes> resources(drmModeGetResources(m_fd));
    if (!resources)
        return;

    for (int i = 0; i < resources->count_crtcs; ++i)
        m_crtcs << new DrmCrtc(this, resources->crtcs[i], i);
}

void DrmDevice::scanPlanes()
{
    DrmScopedPointer<drmModePlaneRes> resources(drmModeGetPlaneResources(m_fd));
    if (!resources)
        return;

    for (uint32_t i = 0; i < resources->count_planes; ++i)
        m_planes << new DrmPlane(this, resources->planes[i]);
}
