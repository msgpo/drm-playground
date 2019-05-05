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

#include <QCommandLineParser>
#include <QCoreApplication>

#include "DrmBackend.h"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption deviceOption(QStringLiteral("option"),
        QString("Path to the DRM device"),
        QStringLiteral("device"),
        QStringLiteral("/dev/dri/card0"));
    parser.addOption(deviceOption);

    parser.process(app);

    DrmBackend* backend = new DrmBackend(parser.value(deviceOption));
    if (!backend->isValid())
        return -1;

    return app.exec();
}
