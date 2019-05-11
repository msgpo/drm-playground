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

class NativeOutput;

class NativeRenderer : public QObject {
    Q_OBJECT

public:
    explicit NativeRenderer(DrmDevice* device, QObject* parent = nullptr);
    ~NativeRenderer() override;

    /**
     * Whether this renderer was initiliazed successfully.
     */
    bool isValid() const;

    /**
     * Marks the beginning of rendering of a frame.
     */
    void beginFrame(NativeOutput* output);

    /**
     * Finalizes the rendering of the frame.
     */
    void finishFrame(NativeOutput* output, const QRegion& damaged);

private:
    DrmDevice* m_device;

    Q_DISABLE_COPY(NativeRenderer)
};
