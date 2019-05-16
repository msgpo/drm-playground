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
#include "DrmDumbAllocator.h"
#include "DrmGbmAllocator.h"
#include "DrmImage.h"
#include "DrmOutput.h"
#include "DrmOutputManager.h"
#include "DrmPlane.h"
#include "DrmPointer.h"
#include "NativeContext.h"
#include "session/SessionController.h"

#include <QBitArray>
#include <QCoreApplication>
#include <QSocketNotifier>

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

    m_supportsDumbBuffer = queryCapability(m_fd, DRM_CAP_DUMB_BUFFER);
    m_supportsExportBuffer = queryCapability(m_fd, DRM_CAP_PRIME) & DRM_PRIME_CAP_EXPORT;
    m_supportsImportBuffer = queryCapability(m_fd, DRM_CAP_PRIME) & DRM_PRIME_CAP_IMPORT;
    m_supportsBufferModifier = queryCapability(m_fd, DRM_CAP_ADDFB2_MODIFIERS);

    switch (m_context->allocatorType()) {
    case AllocatorDumb:
        m_allocator = new DrmDumbAllocator(this);
        break;
    case AllocatorGbm:
        m_allocator = new DrmGbmAllocator(this);
        break;
    }

    if (!m_allocator->isValid())
        return;

    auto notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &DrmDevice::dispatchEvents);

    m_isValid = true;
}

DrmDevice::~DrmDevice()
{
    qDeleteAll(m_outputs);
    qDeleteAll(m_planes);
    qDeleteAll(m_crtcs);
    qDeleteAll(m_connectors);

    delete m_allocator;

    if (m_fd != -1)
        m_context->sessionController()->closeRestricted(m_fd);
}

bool DrmDevice::supports(DeviceCapability capability) const
{
    switch (capability) {
    case DeviceCapabilityDumbBuffer:
        return m_supportsDumbBuffer;
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

NativeContext* DrmDevice::context() const
{
    return m_context;
}

DrmAllocator* DrmDevice::allocator() const
{
    return m_allocator;
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
        if (!(mask & (1 << crtc->pipe())))
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

DrmOutputList DrmDevice::outputs() const
{
    return m_outputs;
}

DrmOutput* DrmDevice::findOutput(const DrmConnector* connector) const
{
    for (DrmOutput* output : m_outputs) {
        if (output->connector() == connector)
            return output;
    }

    return nullptr;
}

DrmOutput* DrmDevice::findOutput(const DrmCrtc* crtc) const
{
    return findOutput(crtc->id());
}

DrmOutput* DrmDevice::findOutput(uint32_t crtcId) const
{
    for (DrmOutput* output : m_outputs) {
        DrmCrtc* crtc = output->crtc();
        if (!crtc)
            continue;
        if (crtc->id() == crtcId)
            return output;
    }

    return nullptr;
}

NativeRenderer* DrmDevice::renderer() const
{
    return m_renderer;
}

bool DrmDevice::isFrozen() const
{
    return m_freezeCounter;
}

void DrmDevice::freeze()
{
    m_freezeCounter++;
}

void DrmDevice::thaw()
{
    m_freezeCounter--;
}

bool DrmDevice::isIdle() const
{
    for (DrmOutput* output : m_outputs) {
        if (output->pendingImage())
            return false;
    }

    return true;
}

void DrmDevice::waitIdle()
{
    freeze();

    while (!isIdle())
        QCoreApplication::processEvents();

    thaw();
}

static std::chrono::nanoseconds makeTimestamp(uint tv_sec, uint tv_usec)
{
    const auto seconds = std::chrono::seconds(tv_sec);
    const auto nanoseconds = std::chrono::nanoseconds(tv_usec * 1000);
    return seconds + nanoseconds;
}

static void deviceEventHandler(int,
    unsigned int sequence,
    unsigned int tv_sec,
    unsigned int tv_usec,
    unsigned int crtc_id,
    void* user_data)
{
    DrmDevice* device = static_cast<DrmDevice*>(user_data);

    DrmOutput* output = device->findOutput(crtc_id);
    if (!output)
        return;

    DrmImage* image = output->pendingImage();
    image->release();

    output->setPendingImage(nullptr);
    output->setSequence(sequence);
    output->setTimestamp(makeTimestamp(tv_sec, tv_usec));

    if (device->isFrozen())
        return;

    const NativeContext* context = device->context();
    if (!context->sessionController()->isActive())
        return;

    // TODO: Notify Compositor.
}

void DrmDevice::dispatchEvents()
{
    drmEventContext context = {};
    context.version = 3;
    context.page_flip_handler2 = deviceEventHandler;

    drmHandleEvent(m_fd, &context);
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

    const DrmConnectorSet previousConnectors = m_connectors.toSet();
    const DrmConnectorSet currentConnectors = connectors.toSet();
    m_connectors = connectors;

    const DrmConnectorSet added = currentConnectors - previousConnectors;
    const DrmConnectorSet removed = previousConnectors - currentConnectors;

    for (DrmConnector* connector : added) {
        DrmOutput* output = new DrmOutput(connector, this);
        m_context->outputManager()->prepare(output);
        m_outputs << output;
    }

    Q_UNUSED(removed)

    reroute();
}

void DrmDevice::scanCrtcs()
{
    DrmScopedPointer<drmModeRes> resources(drmModeGetResources(m_fd));
    if (!resources)
        return;

    for (int i = 0; i < resources->count_crtcs; ++i)
        m_crtcs << new DrmCrtc(this, resources->crtcs[i], i);
}

static DrmPlane* findSpecialPlane(DrmCrtc* crtc, PlaneType type)
{
    const DrmDevice* device = crtc->device();
    const DrmPlaneList planes = device->planes(type);

    for (DrmPlane* plane : planes) {
        if (plane->crtc() && plane->crtc() == crtc)
            return plane;

        if (plane->crtc())
            continue;

        if (plane->possibleCrtcs().contains(crtc))
            return plane;
    }

    return nullptr;
}

void DrmDevice::scanPlanes()
{
    DrmScopedPointer<drmModePlaneRes> resources(drmModeGetPlaneResources(m_fd));
    if (!resources)
        return;

    for (uint32_t i = 0; i < resources->count_planes; ++i)
        m_planes << new DrmPlane(this, resources->planes[i]);

    for (DrmCrtc* crtc : m_crtcs) {
        if (DrmPlane* plane = findSpecialPlane(crtc, PlanePrimary)) {
            plane->setCrtc(crtc);
            crtc->setPrimaryPlane(plane);
        }

        if (DrmPlane* plane = findSpecialPlane(crtc, PlaneCursor)) {
            plane->setCrtc(crtc);
            crtc->setCursorPlane(plane);
        }
    }
}

struct MatchContext {
    DrmConnectorList connectors;
    DrmCrtcList crtcs;
    DrmCrtcMap configuration;
    QBitArray taken;
};

static bool testConfiguration(const MatchContext& context)
{
    Q_UNUSED(context)
    return true;
}

static bool findConfiguration(MatchContext& context, int depth = 0)
{
    if (context.connectors.count() == depth)
        return testConfiguration(context);

    DrmConnector* connector = context.connectors.at(depth);

    for (DrmCrtc* crtc : context.crtcs) {
        if (context.taken.testBit(crtc->pipe()))
            continue;

        context.configuration.insert(connector, crtc);
        context.taken.setBit(crtc->pipe());

        const bool found = findConfiguration(context, depth + 1);
        if (found)
            return true;

        context.configuration.remove(connector);
        context.taken.clearBit(crtc->pipe());
    }

    return false;
}

void DrmDevice::reroute()
{
    MatchContext context;

    context.crtcs = m_crtcs;
    context.taken.resize(m_crtcs.count());

    for (DrmOutput* output : m_outputs) {
        if (!output->isEnabled())
            continue;
        context.connectors << output->connector();
    }

    // Make sure that we prefer existing associations.
    for (int i = 0; i < context.connectors.count(); ++i) {
        const DrmConnector* connector = context.connectors[i];
        if (!connector->crtc())
            continue;

        const int j = context.crtcs.indexOf(connector->crtc());
        if (i == j)
            continue;

        std::swap(context.crtcs[i], context.crtcs[j]);
    }

    if (!findConfiguration(context))
        return;

    for (DrmConnector* connector : context.connectors) {
        DrmCrtc* crtc = context.configuration.value(connector);

        DrmOutput* previousOutput = findOutput(crtc);
        DrmOutput* currentOutput = findOutput(connector);

        if (previousOutput) {
            previousOutput->destroySwapchain();
            previousOutput->setCrtc(nullptr);
            previousOutput->setNeedsModeset(true);
        }

        currentOutput->setCrtc(crtc);
        currentOutput->createSwapchain();
        currentOutput->setNeedsModeset(true);
    }
}
