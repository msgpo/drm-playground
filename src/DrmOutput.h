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

#include <QObject>

class DrmOutput : public QObject {
    Q_OBJECT

public:
    DrmOutput(DrmConnector* connector, QObject* parent = nullptr);
    ~DrmOutput() override;

    /**
     * Returns a DRM device this output connected to.
     */
    DrmDevice* device() const;

    /**
     * Returns connector this output attached to.
     */
    DrmConnector* connector() const;

    /**
     * Returns a CRTC that drives this output.
     */
    DrmCrtc* crtc() const;

    /**
     * Returns the swapchain of this output.
     */
    DrmSwapchain* swapchain() const;

private:
    void createSwapchain();

    DrmConnector* m_connector = nullptr;
    DrmCrtc* m_crtc = nullptr;
    DrmDevice* m_device = nullptr;
    DrmSwapchain* m_swapchain = nullptr;

    Q_DISABLE_COPY(DrmOutput)
};
