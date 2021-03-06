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

#include "DrmGbmAllocator.h"
#include "DrmDevice.h"
#include "DrmGbmImage.h"

#include <QVector>

DrmGbmAllocator::DrmGbmAllocator(DrmDevice* device, QObject* parent)
    : DrmAllocator(parent)
    , m_device(device)
    , m_gbm(gbm_create_device(device->fd()))
{
}

DrmGbmAllocator::~DrmGbmAllocator()
{
    if (m_gbm)
        gbm_device_destroy(m_gbm);
}

bool DrmGbmAllocator::isValid() const
{
    return m_gbm;
}

DrmImage* DrmGbmAllocator::allocate(uint32_t width, uint32_t height, uint32_t format,
    const QVector<uint64_t>& modifiers)
{
    // TODO: Without modifiers.

    gbm_bo* bo = nullptr;
    if (!modifiers.isEmpty())
        bo = gbm_bo_create_with_modifiers(m_gbm, width, height, format, modifiers.data(), modifiers.count());
    else
        bo = gbm_bo_create(m_gbm, width, height, format, GBM_BO_USE_RENDERING | GBM_BO_USE_SCANOUT);

    if (!bo)
        return nullptr;

    return new DrmGbmImage(m_device, bo);
}
