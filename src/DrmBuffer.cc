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

#include "DrmBuffer.h"
#include "DrmDevice.h"

#include <drm_fourcc.h>
#include <xf86drmMode.h>

DrmBuffer::~DrmBuffer()
{
    if (m_id)
        drmModeRmFB(m_device->fd(), m_id);
}

bool DrmBuffer::isValid() const
{
    return m_id;
}

DrmDevice* DrmBuffer::device() const
{
    return m_device;
}

uint32_t DrmBuffer::id() const
{
    return m_id;
}

uint32_t DrmBuffer::format() const
{
    return m_format;
}

uint32_t DrmBuffer::width() const
{
    return m_width;
}

uint32_t DrmBuffer::height() const
{
    return m_height;
}

DrmBuffer* DrmBuffer::create(DrmDevice* device, uint32_t width, uint32_t height,
    uint32_t format, std::array<uint32_t, 4> handles, std::array<uint32_t, 4> pitches,
    std::array<uint32_t, 4> offsets)
{
    uint32_t id;

    if (drmModeAddFB2(device->fd(), width, height, format, handles.data(),
            pitches.data(), offsets.data(), &id, 0))
        return nullptr;

    auto buffer = new DrmBuffer();
    buffer->m_device = device;
    buffer->m_id = id;
    buffer->m_format = format;
    buffer->m_width = width;
    buffer->m_height = height;

    return buffer;
}

DrmBuffer* DrmBuffer::create(DrmDevice* device, uint32_t width, uint32_t height,
    uint32_t format, std::array<uint32_t, 4> handles, std::array<uint32_t, 4> pitches,
    std::array<uint32_t, 4> offsets, std::array<uint64_t, 4> modifiers)
{
    uint32_t id;

    uint32_t flags = 0;
    if (modifiers[0] != DRM_FORMAT_MOD_INVALID)
        flags = DRM_MODE_FB_MODIFIERS;

    if (drmModeAddFB2WithModifiers(device->fd(), width, height, format,
            handles.data(), pitches.data(), offsets.data(), modifiers.data(),
            &id, flags))
        return nullptr;

    auto buffer = new DrmBuffer();
    buffer->m_device = device;
    buffer->m_id = id;
    buffer->m_format = format;
    buffer->m_width = width;
    buffer->m_height = height;

    return buffer;
}
