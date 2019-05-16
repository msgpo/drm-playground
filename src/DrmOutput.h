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

#include "DrmMode.h"

#include <QObject>

#include <memory>

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
     * Sets CRTC that drives this output.
     */
    void setCrtc(DrmCrtc* crtc);

    /**
     * Returns the swapchain of this output.
     */
    DrmSwapchain* swapchain() const;

    /**
     *
     */
    uint sequence() const;

    /**
     *
     */
    void setSequence(uint sequence);

    /**
     *
     */
    std::chrono::nanoseconds timestamp() const;

    /**
     *
     */
    void setTimestamp(const std::chrono::nanoseconds& timestamp);

    /**
     * Returns whether this output is enabled.
     */
    bool isEnabled() const;

    /**
     * Enables or disables this output.
     */
    void setEnabled(bool enabled);

    /**
     * Returns whether this output needs a modeset.
     */
    bool needsModeset() const;

    /**
     * Sets whether this output needs to perform modesetting.
     */
    void setNeedsModeset(bool set);

    /**
     * Returns the desired mode.
     */
    DrmMode desiredMode() const;

    /**
     * Sets the desired mode.
     */
    void setDesiredMode(const DrmMode& mode);

    /**
     * Returns image about to be presented.
     */
    DrmImage* pendingImage() const;

    /**
     * Sets an image about to be presented.
     */
    void setPendingImage(DrmImage* image);

    /**
     *
     */
    void present();

private:
    void createSwapchain();
    void destroySwapchain();

    DrmMode m_desiredMode;
    DrmConnector* m_connector = nullptr;
    DrmCrtc* m_crtc = nullptr;
    DrmDevice* m_device = nullptr;
    DrmSwapchain* m_swapchain = nullptr;
    DrmImage* m_pendingImage = nullptr;
    std::unique_ptr<DrmBlob> m_modeBlob;
    std::chrono::nanoseconds m_timestamp;
    uint m_sequence = 0;
    bool m_isEnabled = false;
    bool m_needsModeset = false;

    friend class DrmDevice;

    Q_DISABLE_COPY(DrmOutput)
};
