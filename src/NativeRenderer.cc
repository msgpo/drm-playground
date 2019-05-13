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

#include "NativeRenderer.h"
#include "DrmDevice.h"

NativeRenderer::NativeRenderer(DrmDevice* device, QObject* parent)
    : QObject(parent)
    , m_device(device)
{
}

NativeRenderer::~NativeRenderer()
{
}

bool NativeRenderer::isValid() const
{
    return true;
}

void NativeRenderer::beginFrame(DrmOutput* output)
{
    Q_UNUSED(output)
}

void NativeRenderer::finishFrame(DrmOutput* output, const QRegion& damaged)
{
    Q_UNUSED(output)
    Q_UNUSED(damaged)
}
