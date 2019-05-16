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

#include "DrmDumbAllocator.h"
#include "DrmDevice.h"
#include "DrmDumbImage.h"

DrmDumbAllocator::DrmDumbAllocator(DrmDevice* device, QObject* parent)
    : DrmAllocator(parent)
    , m_device(device)
    , m_isValid(device->supports(DrmDevice::DeviceCapabilityDumbBuffer))
{
}

DrmDumbAllocator::~DrmDumbAllocator()
{
}

bool DrmDumbAllocator::isValid() const
{
    return m_isValid;
}

DrmImage* DrmDumbAllocator::allocate(uint32_t width, uint32_t height, uint32_t format,
    const QVector<uint64_t>& modifiers)
{
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(format)
    Q_UNUSED(modifiers)
    return nullptr;
}
