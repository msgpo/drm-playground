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

#include "LogindSessionController.h"

#include <QCoreApplication>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDBusUnixFileDescriptor>
#include <QVector>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

struct LogindSession {
    QString id;
    QDBusObjectPath path;
};

QDBusArgument& operator<<(QDBusArgument& argument, const LogindSession& session)
{
    argument.beginStructure();
    argument << session.id << session.path;
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, LogindSession& session)
{
    argument.beginStructure();
    argument >> session.id >> session.path;
    argument.endStructure();
    return argument;
}

Q_DECLARE_METATYPE(LogindSession)

typedef QVector<LogindSession> LogindSessionList;

const QDBusArgument& operator<<(QDBusArgument& argument, const LogindSessionList& sessions)
{
    argument.beginArray(qMetaTypeId<LogindSession>());
    for (const LogindSession& session : sessions)
        argument << session;
    argument.endArray();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, LogindSessionList& sessions)
{
    argument.beginArray();
    while (!argument.atEnd()) {
        LogindSession session;
        argument >> session;
        sessions.append(session);
    }
    argument.endArray();
    return argument;
}

const static QVector<QString> s_graphicalSessionTypes {
    QStringLiteral("wayland"),
    QStringLiteral("x11"),
    QStringLiteral("mir")
};

const static QVector<QString> s_graphicalSessionStates {
    QStringLiteral("active"),
    QStringLiteral("online")
};

static QString findProcessSessionPath()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"),
        QStringLiteral("/org/freedesktop/login1"),
        QStringLiteral("org.freedesktop.login1.Manager"),
        QStringLiteral("GetSessionByPID"));
    message.setArguments({ QCoreApplication::applicationPid() });

    const QDBusMessage reply = QDBusConnection::systemBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return QString();

    return reply.arguments().first().toString();
}

static bool isGraphicalSession(const QString& session)
{
    const QDBusInterface interface(
        QStringLiteral("org.freedesktop.login1"),
        session,
        QStringLiteral("org.freedesktop.login1.Session"),
        QDBusConnection::systemBus());

    if (!interface.isValid())
        return false;

    const QVariant type = interface.property("Type");
    if (!type.isValid())
        return false;
    if (!s_graphicalSessionTypes.contains(type.toString()))
        return false;

    const QVariant state = interface.property("State");
    if (!state.isValid())
        return false;
    if (!s_graphicalSessionStates.contains(state.toString()))
        return false;

    return true;
}

template <typename T>
static T unbox(const QDBusMessage& message)
{
    const QDBusVariant box = message.arguments().first().value<QDBusVariant>();
    return qdbus_cast<T>(box.variant().value<QDBusArgument>());
}

static QString findDisplaySessionPath()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"),
        QStringLiteral("/org/freedesktop/login1/user/self"),
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("Get"));
    message.setArguments({ QStringLiteral("org.freedesktop.login1.User"),
        QStringLiteral("Display") });

    const QDBusMessage reply = QDBusConnection::systemBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return QString();

    const LogindSession session = unbox<LogindSession>(reply);

    return session.path.path();
}

static QString findGreeterSessionPath()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"),
        QStringLiteral("/org/freedesktop/login1/user/self"),
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("Get"));
    message.setArguments({ QStringLiteral("org.freedesktop.login1.User"),
        QStringLiteral("Sessions") });

    const QDBusMessage reply = QDBusConnection::systemBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return QString();

    const LogindSessionList sessions = unbox<LogindSessionList>(reply);
    for (const LogindSession& session : sessions) {
        QDBusMessage message = QDBusMessage::createMethodCall(
            QStringLiteral("org.freedesktop.login1"),
            session.path.path(),
            QStringLiteral("org.freedesktop.DBus.Properties"),
            QStringLiteral("Get"));
        message.setArguments({ QStringLiteral("org.freedesktop.login1.Session"),
            QStringLiteral("Class") });

        const QDBusMessage reply = QDBusConnection::systemBus().call(message);
        if (reply.type() == QDBusMessage::ErrorMessage)
            continue;

        const QString class_ = reply.arguments().first().toString();
        if (class_ != QStringLiteral("greeter"))
            continue;

        return session.path.path();
    }

    return QString();
}

static QString findSessionPath()
{
    QString sessionPath = findProcessSessionPath();
    if (!sessionPath.isEmpty())
        return sessionPath;

    sessionPath = findDisplaySessionPath();
    if (!sessionPath.isEmpty() && isGraphicalSession(sessionPath))
        return sessionPath;

    sessionPath = findGreeterSessionPath();
    if (!sessionPath.isEmpty() && isGraphicalSession(sessionPath))
        return sessionPath;

    return QString();
}

LogindSessionController::LogindSessionController(QObject* parent)
    : SessionController(parent)
{
    qDBusRegisterMetaType<LogindSession>();

    m_sessionPath = findSessionPath();
    if (m_sessionPath.isEmpty())
        return;

    const QDBusInterface seatInterface(
        QStringLiteral("org.freedesktop.login1"),
        QStringLiteral("/org/freedesktop/login1/seat/self"),
        QStringLiteral("org.freedesktop.login1.Seat"),
        QDBusConnection::systemBus());
    if (!seatInterface.isValid())
        return;

    m_seat = seatInterface.property("Id").toString();

    const QDBusInterface sessionInterface(
        QStringLiteral("org.freedesktop.login1"),
        m_sessionPath,
        QStringLiteral("org.freedesktop.login1.Session"),
        QDBusConnection::systemBus());
    if (!sessionInterface.isValid())
        return;

    m_terminal = sessionInterface.property("VTNr").toUInt();
    m_sessionId = sessionInterface.property("Id").toString();
    m_isActive = sessionInterface.property("Active").toBool();

    if (!activate())
        return;

    if (!takeControl())
        return;

    QDBusConnection::systemBus().connect(
        QStringLiteral("org.freedesktop.login1"),
        m_sessionPath,
        QStringLiteral("org.freedesktop.login1.Session"),
        QStringLiteral("PauseDevice"),
        this,
        SLOT(slotPauseDevice(uint, uint, QString)));

    QDBusConnection::systemBus().connect(
        QStringLiteral("org.freedesktop.login1"),
        m_sessionPath,
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("PropertiesChanged"),
        this,
        SLOT(slotPropertiesChanged(QString, QVariantMap)));

    m_isValid = true;
}

LogindSessionController::~LogindSessionController()
{
    if (m_isValid)
        releaseControl();
}

bool LogindSessionController::isActive() const
{
    return m_isActive;
}

bool LogindSessionController::isValid() const
{
    return m_isValid;
}

QString LogindSessionController::seat() const
{
    return m_seat;
}

QString LogindSessionController::session() const
{
    return m_sessionId;
}

uint LogindSessionController::terminal() const
{
    return m_terminal;
}

int LogindSessionController::openRestricted(const QString& path)
{
    struct stat st;
    if (stat(path.toUtf8(), &st) < 0)
        return -1;

    QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"),
        m_sessionPath,
        QStringLiteral("org.freedesktop.login1.Session"),
        QStringLiteral("TakeDevice"));
    message.setArguments({ major(st.st_rdev), minor(st.st_rdev) });

    const QDBusMessage reply = QDBusConnection::systemBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return -1;

    const QDBusUnixFileDescriptor descriptor =
        reply.arguments().first().value<QDBusUnixFileDescriptor>();
    if (!descriptor.isValid())
        return -1;

    return fcntl(descriptor.fileDescriptor(), F_DUPFD_CLOEXEC, 0);
}

void LogindSessionController::closeRestricted(int fd)
{
    struct stat st;
    if (fstat(fd, &st) < 0)
        return;

    QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"),
        m_sessionPath,
        QStringLiteral("org.freedesktop.login1.Session"),
        QStringLiteral("ReleaseDevice"));
    message.setArguments({ major(st.st_dev), minor(st.st_dev) });

    QDBusConnection::systemBus().asyncCall(message);

    close(fd);
}

void LogindSessionController::switchTo(int terminal)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"),
        QStringLiteral("/org/freedesktop/login1/seat/self"),
        QStringLiteral("org.freedesktop.login1.Seat"),
        QStringLiteral("SwitchTo"));
    message.setArguments({ terminal });

    QDBusConnection::systemBus().asyncCall(message);
}

bool LogindSessionController::takeControl()
{
    const QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"),
        m_sessionPath,
        QStringLiteral("org.freedesktop.login1.Session"),
        QStringLiteral("TakeControl"));

    const QDBusMessage reply = QDBusConnection::systemBus().call(message);

    return reply.type() != QDBusMessage::ErrorMessage;
}

void LogindSessionController::releaseControl()
{
    const QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"),
        m_sessionPath,
        QStringLiteral("org.freedesktop.login1.Session"),
        QStringLiteral("ReleaseControl"));

    QDBusConnection::systemBus().asyncCall(message);
}

bool LogindSessionController::activate()
{
    const QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"),
        m_sessionPath,
        QStringLiteral("org.freedesktop.login1.Session"),
        QStringLiteral("Activate"));

    const QDBusMessage reply = QDBusConnection::systemBus().call(message);

    return reply.type() != QDBusMessage::ErrorMessage;
}

void LogindSessionController::slotPauseDevice(uint major, uint minor, const QString& type)
{
    if (type == QLatin1String("pause")) {
        QDBusMessage message = QDBusMessage::createMethodCall(
            QStringLiteral("org.freedesktop.login1"),
            m_sessionPath,
            QStringLiteral("org.freedesktop.login1.Session"),
            QStringLiteral("PauseDeviceComplete"));
        message.setArguments({ major, minor });

        QDBusConnection::systemBus().asyncCall(message);
    }
}

void LogindSessionController::slotPropertiesChanged(const QString& interfaceName, const QVariantMap& properties)
{
    if (interfaceName != QLatin1String("org.freedesktop.login1.Session"))
        return;

    if (properties.contains(QStringLiteral("Active")))
        return;

    const QDBusInterface sessionInterface(
        QStringLiteral("org.freedesktop.login1"),
        m_sessionPath,
        QStringLiteral("org.freedesktop.login1.Session"),
        QDBusConnection::systemBus());
    if (!sessionInterface.isValid())
        return;

    const bool active = sessionInterface.property("Active").toBool();
    if (m_isActive == active)
        return;

    m_isActive = active;

    emit activeChanged();
}
