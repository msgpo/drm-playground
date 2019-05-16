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

#include "DrmMode.h"

static uint32_t computeRefreshRate(const drmModeModeInfo& mode)
{
    uint64_t refresh = mode.clock * 1000000ll / mode.htotal + mode.vtotal / 2;
    refresh /= mode.vtotal;

    if (mode.flags & DRM_MODE_FLAG_INTERLACE)
        refresh *= 2;

    if (mode.flags & DRM_MODE_FLAG_DBLSCAN)
        refresh /= 2;

    if (mode.vscan > 1)
        refresh /= mode.vscan;

    return refresh;
}

DrmMode::DrmMode() = default;

DrmMode::DrmMode(const drmModeModeInfo& mode)
    : m_isPreferred(mode.type & DRM_MODE_TYPE_PREFERRED)
    , m_width(mode.hdisplay)
    , m_height(mode.vdisplay)
    , m_refreshRate(computeRefreshRate(mode))
    , m_data(mode)
{
}

bool DrmMode::isPreferred() const
{
    return m_isPreferred;
}

uint32_t DrmMode::width() const
{
    return m_width;
}

uint32_t DrmMode::height() const
{
    return m_height;
}

uint32_t DrmMode::refreshRate() const
{
    return m_refreshRate;
}

drmModeModeInfo DrmMode::data() const
{
    return m_data;
}
