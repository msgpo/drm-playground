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

#include "DrmConnector.h"
#include "DrmDevice.h"
#include "DrmMode.h"
#include "DrmPointer.h"
#include "EDID.h"

static QString makeConnectorName(const drmModeConnector* connector)
{
    QString type;
    switch (connector->connector_type) {
    case DRM_MODE_CONNECTOR_VGA:
        type = QStringLiteral("VGA");
    case DRM_MODE_CONNECTOR_DVII:
        type = QStringLiteral("DVI-I");
    case DRM_MODE_CONNECTOR_DVID:
        type = QStringLiteral("DVI-D");
    case DRM_MODE_CONNECTOR_DVIA:
        type = QStringLiteral("DVI-A");
    case DRM_MODE_CONNECTOR_Composite:
        type = QStringLiteral("Composite");
    case DRM_MODE_CONNECTOR_SVIDEO:
        type = QStringLiteral("SVIDEO");
    case DRM_MODE_CONNECTOR_LVDS:
        type = QStringLiteral("LVDS");
    case DRM_MODE_CONNECTOR_Component:
        type = QStringLiteral("Component");
    case DRM_MODE_CONNECTOR_9PinDIN:
        type = QStringLiteral("DIN");
    case DRM_MODE_CONNECTOR_DisplayPort:
        type = QStringLiteral("DisplayPort");
    case DRM_MODE_CONNECTOR_HDMIA:
        type = QStringLiteral("HDMI-A");
    case DRM_MODE_CONNECTOR_HDMIB:
        type = QStringLiteral("HDMI-B");
    case DRM_MODE_CONNECTOR_TV:
        type = QStringLiteral("TV");
    case DRM_MODE_CONNECTOR_eDP:
        type = QStringLiteral("eDP");
    case DRM_MODE_CONNECTOR_VIRTUAL:
        type = QStringLiteral("Virtual");
    case DRM_MODE_CONNECTOR_DSI:
        type = QStringLiteral("DSI");
    case DRM_MODE_CONNECTOR_DPI:
        type = QStringLiteral("DPI");
    case DRM_MODE_CONNECTOR_Unknown:
    default:
        type = QStringLiteral("Unknown");
    }

    const QString id = QString::number(connector->connector_type_id);

    return type + QLatin1Char('-') + id;
}

static bool extractOnlineStatus(const drmModeConnector* connector)
{
    return connector->connection == DRM_MODE_CONNECTED;
}

static std::unique_ptr<EDID> extractEDID(int fd, uint32_t blobId)
{
    DrmScopedPointer<drmModePropertyBlobRes> blob(drmModeGetPropertyBlob(fd, blobId));
    if (!blob)
        return nullptr;

    auto edid = std::make_unique<EDID>(blob->data, blob->length);
    if (!edid->isValid())
        return nullptr;

    return edid;
}

static DrmCrtcList findPossibleCrtcs(DrmDevice* device, const drmModeConnector* connector)
{
    uint32_t mask = 0;

    for (int i = 0; i < connector->count_encoders; ++i) {
        const uint32_t encoderId = connector->encoders[i];
        DrmScopedPointer<drmModeEncoder> encoder(drmModeGetEncoder(device->fd(), encoderId));
        if (!encoder)
            continue;
        mask |= encoder->possible_crtcs;
    }

    return device->findCrtcs(mask);
}

static DrmModeList extractModes(const drmModeConnector* connector)
{
    DrmModeList modes;
    modes.reserve(connector->count_modes);

    for (int i = 0; i < connector->count_modes; ++i)
        modes << DrmMode(connector->modes[i]);

    return modes;
}

DrmConnector::DrmConnector(DrmDevice* device, uint32_t id)
    : DrmObject(device, id, DRM_MODE_OBJECT_CONNECTOR)
{
    const int fd = device->fd();

    DrmScopedPointer<drmModeConnector> connector(drmModeGetConnector(fd, id));
    if (!connector)
        return;

    m_name = makeConnectorName(connector.get());
    m_isOnline = extractOnlineStatus(connector.get());
    if (!m_isOnline)
        return;

    forEachProperty([=](const drmModePropertyPtr property, uint64_t value) {
        if (property->name == QByteArrayLiteral("CRTC_ID")) {
            m_properties.crtcId = property->prop_id;
            return;
        }
        if (property->name == QByteArrayLiteral("DPMS")) {
            m_properties.dpms = property->prop_id;
            return;
        }
        if (property->name == QByteArrayLiteral("EDID")) {
            m_edid = extractEDID(device->fd(), uint32_t(value));
            return;
        }
    });

    m_modes = extractModes(connector.get());

    const uint32_t encoderId = connector->encoder_id;
    DrmScopedPointer<drmModeEncoder> encoder(drmModeGetEncoder(fd, encoderId));
    if (encoder)
        m_crtc = device->findCrtc(encoder->crtc_id);

    m_possibleCrtcs = findPossibleCrtcs(device, connector.get());
}

DrmConnector::~DrmConnector()
{
}

bool DrmConnector::isOnline() const
{
    return m_isOnline;
}

QString DrmConnector::name() const
{
    return m_name;
}

EDID* DrmConnector::edid() const
{
    return m_edid.get();
}

DrmModeList DrmConnector::modes() const
{
    return m_modes;
}

DrmCrtc* DrmConnector::crtc() const
{
    return m_crtc;
}

DrmCrtcList DrmConnector::possibleCrtcs() const
{
    return m_possibleCrtcs;
}
