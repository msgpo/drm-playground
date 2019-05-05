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

#include "DrmObject.h"
#include "DrmDevice.h"
#include "DrmPointer.h"

DrmObject::DrmObject(DrmDevice* device, uint32_t id, uint32_t type)
    : m_device(device)
    , m_id(id)
    , m_type(type)
{
}

DrmObject::~DrmObject()
{
}

DrmDevice* DrmObject::device() const
{
    return m_device;
}

uint32_t DrmObject::id() const
{
    return m_id;
}

uint32_t DrmObject::type() const
{
    return m_type;
}

void DrmObject::forEachProperty(std::function<void(const drmModePropertyPtr, uint64_t)> callback)
{
    const int fd = m_device->fd();

    DrmScopedPointer<drmModeObjectProperties> properties(
        drmModeObjectGetProperties(fd, m_id, m_type));
    if (!properties)
        return;

    for (uint32_t i = 0; i < properties->count_props; ++i) {
        const uint32_t propertyId = properties->props[i];
        const uint64_t value = properties->prop_values[i];
        DrmScopedPointer<drmModePropertyRes> property(drmModeGetProperty(fd, propertyId));
        if (!property)
            continue;
        callback(property.get(), value);
    }
}
