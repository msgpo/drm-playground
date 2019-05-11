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

#include <memory>

class EDID;

class DrmConnector : public DrmObject {
public:
    DrmConnector(DrmDevice* device, uint32_t id);
    ~DrmConnector() override;

    /**
     * Whether this connector is connected.
     */
    bool isOnline() const;

    /**
     * Returns logical name of this connector.
     */
    QString name() const;

    /**
     * Returns metadata of the connected display.
     *
     * If the connector is offline, @c null is returned.
     */
    EDID* edid() const;

    /**
     * Returns a list of supported display modes.
     */
    DrmModeList modes() const;

    /**
     * Returns CRTC that currently drives this connector.
     */
    DrmCrtc* crtc() const;

    /**
     * Returns a list of CRTCs that can drive this connector.
     */
    DrmCrtcList possibleCrtcs() const;

private:
    DrmModeList m_modes;
    DrmCrtcList m_possibleCrtcs;
    DrmCrtc* m_crtc = nullptr;
    std::unique_ptr<EDID> m_edid;
    QString m_name;
    bool m_isOnline = false;

    struct {
        uint32_t crtcId = 0;
        uint32_t dpms = 0;
    } m_properties;
};
