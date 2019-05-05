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

#include <QObject>

class SessionController : public QObject {
    Q_OBJECT

public:
    explicit SessionController(QObject* parent = nullptr);

    virtual bool isActive() const = 0;
    virtual bool isValid() const = 0;

    virtual QString seat() const = 0;
    virtual QString session() const = 0;
    virtual uint terminal() const = 0;

    virtual int openRestricted(const QString& path) = 0;
    virtual void closeRestricted(int fd) = 0;

    virtual void switchTo(int terminal) = 0;

signals:
    void activeChanged();

private:
    Q_DISABLE_COPY(SessionController)
};
