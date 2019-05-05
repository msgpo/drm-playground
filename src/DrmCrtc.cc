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

#include "DrmCrtc.h"

DrmCrtc::DrmCrtc(DrmDevice* device, uint32_t id, uint32_t index)
    : DrmObject(device, id, DRM_MODE_OBJECT_CRTC)
    , m_index(index)
{
    forEachProperty([this](const drmModePropertyPtr property, uint64_t value) {
        Q_UNUSED(value)
        if (property->name == QByteArrayLiteral("ACTIVE")) {
            m_properties.active = property->prop_id;
            return;
        }
        if (property->name == QByteArrayLiteral("DEGAMMA_LUT")) {
            return;
        }
        if (property->name == QByteArrayLiteral("DEGAMMA_LUT_SIZE")) {
            return;
        }
        if (property->name == QByteArrayLiteral("GAMMA_LUT")) {
            return;
        }
        if (property->name == QByteArrayLiteral("GAMMA_LUT_SIZE")) {
            return;
        }
        if (property->name == QByteArrayLiteral("MODE_ID")) {
            m_properties.modeId = property->prop_id;
            return;
        }
    });
}

DrmCrtc::~DrmCrtc()
{
}

uint32_t DrmCrtc::index() const
{
    return m_index;
}
