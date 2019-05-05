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

#include "globals.h"

#include <array>
#include <cstdint>

class DrmBuffer {
public:
    ~DrmBuffer();

    /**
     * Whether this frame buffer is valid.
     */
    bool isValid() const;

    /**
     * Returns device to which this frame buffer belongs to.
     */
    DrmDevice* device() const;

    /**
     * Returns the id of this frame buffer.
     */
    uint32_t id() const;

    /**
     * Returns format of this frame buffer.
     */
    uint32_t format() const;

    /**
     * Returns width of this frame buffer.
     */
    uint32_t width() const;

    /**
     * Returns height of this frame buffer.
     */
    uint32_t height() const;

    static DrmBuffer* create(DrmDevice* device, uint32_t width, uint32_t height,
        uint32_t format, std::array<uint32_t, 4> handles, std::array<uint32_t, 4> pitches,
        std::array<uint32_t, 4> offsets);

    static DrmBuffer* create(DrmDevice* device, uint32_t width, uint32_t height,
        uint32_t format, std::array<uint32_t, 4> handles, std::array<uint32_t, 4> pitches,
        std::array<uint32_t, 4> offsets, std::array<uint64_t, 4> modifiers);

private:
    DrmDevice* m_device;
    uint32_t m_id;
    uint32_t m_format;
    uint32_t m_width;
    uint32_t m_height;
};
