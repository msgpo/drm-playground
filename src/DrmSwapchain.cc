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

#include "DrmSwapchain.h"
#include "DrmAllocator.h"
#include "DrmDevice.h"
#include "DrmImage.h"

DrmSwapchain::DrmSwapchain(DrmDevice* device, uint32_t width, uint32_t height,
    uint32_t format, const QVector<uint64_t>& modifiers)
{
    DrmAllocator* allocator = device->allocator();
    for (int i = 0; i < 3; ++i)
        m_images << allocator->allocate(width, height, format, modifiers);
}

DrmSwapchain::~DrmSwapchain()
{
    qDeleteAll(m_images);
}

bool DrmSwapchain::isValid() const
{
    for (DrmImage* image : m_images) {
        if (!image->isValid())
            return false;
    }

    return true;
}

int DrmSwapchain::depth() const
{
    return m_images.count();
}

DrmImage* DrmSwapchain::acquire()
{
    for (DrmImage* image : m_images) {
        if (image->isBusy())
            continue;
        image->setBusy(true);
        return image;
    }

    return nullptr;
}
