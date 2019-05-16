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

#pragma once

#include <cstdint>
#include <cstdlib>

#include <xf86drmMode.h>

class DrmMode {
public:
    explicit DrmMode();
    DrmMode(const drmModeModeInfo& mode);

    /**
     * Whether this mode is preferred.
     */
    bool isPreferred() const;

    /**
     * Returns the width.
     */
    uint32_t width() const;

    /**
     * Returns the height.
     */
    uint32_t height() const;

    /**
     * Returns the refresh rate, in MHz.
     */
    uint32_t refreshRate() const;

    /**
     * Returns the raw drm mode info.
     */
    drmModeModeInfo data() const;

private:
    bool m_isPreferred = false;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_refreshRate = 0;
    drmModeModeInfo m_data;
};
