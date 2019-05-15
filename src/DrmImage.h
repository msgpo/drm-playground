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

#include <gbm.h>

class DrmImage {
public:
    DrmImage(DrmDevice* device, gbm_bo* bo);
    ~DrmImage();

    /**
     * Returns whether this image is valid.
     */
    bool isValid() const;

    /**
     * Returns the corresponding frame buffer object.
     */
    DrmBuffer* buffer() const;

    /**
     * Puts this image back to the swapchain.
     */
    void release();

private:
    /**
     * Returns whether this image is currently being used.
     */
    bool isBusy() const;

    /**
     * Sets the busy status.
     */
    void setBusy(bool busy);

    DrmBuffer* m_buffer = nullptr;
    gbm_bo* m_bo = nullptr;
    bool m_isBusy = false;

    friend class DrmSwapchain;
};
