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

#include "SessionController.h"

#include <QVariantMap>

class LogindSessionController : public SessionController {
    Q_OBJECT

public:
    explicit LogindSessionController(QObject* parent = nullptr);
    ~LogindSessionController() override;

    bool isActive() const override;
    bool isValid() const override;

    QString seat() const override;
    QString session() const override;
    uint terminal() const override;

    int openRestricted(const QString& path) override;
    void closeRestricted(int fd) override;

    void switchTo(int terminal) override;

private slots:
    void slotPauseDevice(uint major, uint minor, const QString& type);
    void slotPropertiesChanged(const QString& interfaceName, const QVariantMap& properties);

private:
    bool takeControl();
    void releaseControl();
    bool activate();

    QString m_seat;
    uint m_terminal = 0;
    QString m_sessionId;
    QString m_sessionPath;
    bool m_active = false;
    bool m_valid = false;

    Q_DISABLE_COPY(LogindSessionController)
};
