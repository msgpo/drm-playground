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

#include "DrmMode.h"
#include "DrmObject.h"

class DrmCrtc : public DrmObject {
public:
    DrmCrtc(DrmDevice* device, uint32_t id, uint32_t index);
    ~DrmCrtc() override;

    /**
     * This enum type is used to specify capabilities of CRTCs.
     */
    enum Capability {
        CapabilityGamma,
        CapabilityRotation
    };

    /**
     * Whether this CRTC supports the given @p capability.
     */
    bool supports(Capability capability) const;

    /**
     * Returns the current display mode.
     */
    DrmMode mode() const;

    /**
     * Returns the primary plane.
     */
    DrmPlane* primaryPlane() const;

    /**
     * Returns the cursor plane.
     *
     * If there is no such a plane, @c null is returned.
     */
    DrmPlane* cursorPlane() const;

private:
    /**
     * Returns index of this CRTC in drmModeRes array.
     */
    uint32_t index() const;

    DrmMode m_mode;
    DrmPlane* m_primaryPlane = nullptr;
    DrmPlane* m_cursorPlane = nullptr;
    uint32_t m_index;

    struct {
        uint32_t active = 0;
        uint32_t modeId = 0;

        uint32_t rotation = 0;
        uint32_t degammaTable = 0;
        uint32_t gammaTable = 0;
    } m_properties;

    friend class DrmDevice;
};
