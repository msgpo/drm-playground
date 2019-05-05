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

#include <xf86drmMode.h>

#include <memory>

template <typename T>
struct DrmDeleter;

template <>
struct DrmDeleter<drmModeAtomicReq> {
    void operator()(drmModeAtomicReq* req) const
    {
        drmModeAtomicFree(req);
    }
};

template <>
struct DrmDeleter<drmModeConnector> {
    void operator()(drmModeConnector* connector) const
    {
        drmModeFreeConnector(connector);
    }
};

template <>
struct DrmDeleter<drmModeCrtc> {
    void operator()(drmModeCrtc* crtc) const
    {
        drmModeFreeCrtc(crtc);
    }
};

template <>
struct DrmDeleter<drmModeFB> {
    void operator()(drmModeFB* fb) const
    {
        drmModeFreeFB(fb);
    }
};

template <>
struct DrmDeleter<drmModeEncoder> {
    void operator()(drmModeEncoder* encoder) const
    {
        drmModeFreeEncoder(encoder);
    }
};

template <>
struct DrmDeleter<drmModeModeInfo> {
    void operator()(drmModeModeInfo* info) const
    {
        drmModeFreeModeInfo(info);
    }
};

template <>
struct DrmDeleter<drmModeObjectProperties> {
    void operator()(drmModeObjectProperties* properties) const
    {
        drmModeFreeObjectProperties(properties);
    }
};

template <>
struct DrmDeleter<drmModePlane> {
    void operator()(drmModePlane* plane) const
    {
        drmModeFreePlane(plane);
    }
};

template <>
struct DrmDeleter<drmModePlaneRes> {
    void operator()(drmModePlaneRes* resources) const
    {
        drmModeFreePlaneResources(resources);
    }
};

template <>
struct DrmDeleter<drmModePropertyRes> {
    void operator()(drmModePropertyRes* property) const
    {
        drmModeFreeProperty(property);
    }
};

template <>
struct DrmDeleter<drmModePropertyBlobRes> {
    void operator()(drmModePropertyBlobRes* blob) const
    {
        drmModeFreePropertyBlob(blob);
    }
};

template <>
struct DrmDeleter<drmModeRes> {
    void operator()(drmModeRes* resources) const
    {
        drmModeFreeResources(resources);
    }
};

template <typename T>
using DrmScopedPointer = std::unique_ptr<T, DrmDeleter<T>>;
