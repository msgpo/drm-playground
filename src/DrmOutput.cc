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

#include "DrmOutput.h"
#include "DrmBlob.h"
#include "DrmBuffer.h"
#include "DrmConnector.h"
#include "DrmCrtc.h"
#include "DrmDevice.h"
#include "DrmImage.h"
#include "DrmPlane.h"
#include "DrmSwapchain.h"

#include <drm_fourcc.h>

DrmOutput::DrmOutput(DrmConnector* connector, QObject* parent)
    : QObject(parent)
    , m_connector(connector)
    , m_device(connector->device())
{
}

DrmOutput::~DrmOutput()
{
    delete m_swapchain;
}

DrmDevice* DrmOutput::device() const
{
    return m_device;
}

DrmConnector* DrmOutput::connector() const
{
    return m_connector;
}

DrmCrtc* DrmOutput::crtc() const
{
    return m_crtc;
}

void DrmOutput::setCrtc(DrmCrtc* crtc)
{
    m_crtc = crtc;
}

DrmSwapchain* DrmOutput::swapchain() const
{
    return m_swapchain;
}

uint DrmOutput::sequence() const
{
    return m_sequence;
}

void DrmOutput::setSequence(uint sequence)
{
    m_sequence = sequence;
}

std::chrono::nanoseconds DrmOutput::timestamp() const
{
    return m_timestamp;
}

void DrmOutput::setTimestamp(const std::chrono::nanoseconds& timestamp)
{
    m_timestamp = timestamp;
}

bool DrmOutput::isEnabled() const
{
    return m_isEnabled;
}

void DrmOutput::setEnabled(bool enabled)
{
    m_isEnabled = enabled;
}

bool DrmOutput::needsModeset() const
{
    return m_needsModeset;
}

void DrmOutput::setNeedsModeset(bool set)
{
    m_needsModeset = set;
}

DrmMode DrmOutput::desiredMode() const
{
    return m_desiredMode;
}

void DrmOutput::setDesiredMode(const DrmMode& mode)
{
    m_desiredMode = mode;
}

DrmImage* DrmOutput::pendingImage() const
{
    return m_pendingImage;
}

void DrmOutput::setPendingImage(DrmImage* image)
{
    m_pendingImage = image;
}

void DrmOutput::createSwapchain()
{
    destroySwapchain();

    const uint32_t width = m_desiredMode.width();
    const uint32_t height = m_desiredMode.height();
    const uint32_t format = DRM_FORMAT_XRGB8888;

    // TODO: Fill modifiers.
    const QVector<uint64_t> modifiers;

    m_swapchain = new DrmSwapchain(m_device, width, height, format, modifiers);
}

void DrmOutput::destroySwapchain()
{
    delete m_swapchain;
    m_swapchain = nullptr;
}

void DrmOutput::present()
{
    if (needsModeset()) {
        const drmModeModeInfo mode = m_desiredMode.data();
        m_modeBlob = std::make_unique<DrmBlob>(device(), &mode, sizeof(mode));
    }

    const DrmPlane* plane = m_crtc->primaryPlane();
    const DrmBuffer* buffer = m_pendingImage->buffer();

    const ConnectorProperties connectorProperties = m_connector->properties();
    const CrtcProperties crtcProperties = m_crtc->properties();
    const PlaneProperties planeProperties = plane->properties();

    const uint32_t connectorId = m_connector->id();
    const uint32_t crtcId = m_crtc->id();
    const uint32_t planeId = plane->id();

    drmModeAtomicReqPtr request = drmModeAtomicAlloc();

    drmModeAtomicAddProperty(request, connectorId, connectorProperties.crtcId, crtcId);

    drmModeAtomicAddProperty(request, crtcId, crtcProperties.modeId, m_modeBlob->id());
    drmModeAtomicAddProperty(request, crtcId, crtcProperties.active, 1);

    drmModeAtomicAddProperty(request, planeId, planeProperties.srcX, 0);
    drmModeAtomicAddProperty(request, planeId, planeProperties.srcY, 0);
    drmModeAtomicAddProperty(request, planeId, planeProperties.srcWidth, static_cast<uint64_t>(buffer->width()) << 16);
    drmModeAtomicAddProperty(request, planeId, planeProperties.srcHeight, static_cast<uint64_t>(buffer->height()) << 16);
    drmModeAtomicAddProperty(request, planeId, planeProperties.crtcX, 0);
    drmModeAtomicAddProperty(request, planeId, planeProperties.crtcY, 0);
    drmModeAtomicAddProperty(request, planeId, planeProperties.crtcWidth, buffer->width());
    drmModeAtomicAddProperty(request, planeId, planeProperties.crtcHeight, buffer->height());
    drmModeAtomicAddProperty(request, planeId, planeProperties.crtcId, crtcId);
    drmModeAtomicAddProperty(request, planeId, planeProperties.frameBufferId, buffer->id());

    uint32_t flags = DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_ATOMIC_NONBLOCK;
    if (needsModeset())
        flags |= DRM_MODE_ATOMIC_ALLOW_MODESET;

    drmModeAtomicCommit(device()->fd(), request, flags, device());

    drmModeAtomicFree(request);
}
