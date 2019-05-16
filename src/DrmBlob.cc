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

#include "DrmBlob.h"
#include "DrmDevice.h"

#include <xf86drmMode.h>

DrmBlob::DrmBlob(DrmDevice* device, const void* data, size_t size)
    : m_device(device)
    , m_id(0)
{
    drmModeCreatePropertyBlob(device->fd(), data, size, &m_id);
}

DrmBlob::~DrmBlob()
{
    if (m_id)
        drmModeDestroyPropertyBlob(m_device->fd(), m_id);
}

bool DrmBlob::isValid() const
{
    return m_id;
}

uint32_t DrmBlob::id() const
{
    return m_id;
}
