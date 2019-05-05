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

#include <cstdint>

#include <xf86drmMode.h>

#include <functional>

class DrmObject {
public:
    DrmObject(DrmDevice* device, uint32_t id, uint32_t type);
    virtual ~DrmObject();

    /**
     * Returns device to which this object belongs.
     */
    DrmDevice* device() const;

    /**
     * Returns the id of this object.
     */
    uint32_t id() const;

    /**
     * Returns the type of this object.
     */
    uint32_t type() const;

protected:
    /**
     * Applies the given function for each property that this object has.
     */
    void forEachProperty(std::function<void(const drmModePropertyPtr, uint64_t)> callback);

private:
    DrmDevice* m_device;
    uint32_t m_id;
    uint32_t m_type;
};
