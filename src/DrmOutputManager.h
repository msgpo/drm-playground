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

class DrmOutputManager : public QObject {
    Q_OBJECT

public:
    explicit DrmOutputManager(QObject* parent = nullptr);
    ~DrmOutputManager() override;

    /**
     * Returns whether this output manager is valid.
     */
    bool isValid() const;

    /**
     * Prepares the initial state of the given output.
     */
    void prepare(DrmOutput* output);

private:
    Q_DISABLE_COPY(DrmOutputManager)
};
