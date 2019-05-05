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

#include "DrmObject.h"

class DrmCrtc : public DrmObject {
public:
    DrmCrtc(DrmDevice* device, uint32_t id, uint32_t index);
    ~DrmCrtc() override;

private:
    /**
     * Returns index of this CRTC in drmModeRes array.
     */
    uint32_t index() const;

    uint32_t m_index;

    struct {
        uint32_t active = 0;
        uint32_t modeId = 0;
    } m_properties;

    friend class DrmDevice;
};
