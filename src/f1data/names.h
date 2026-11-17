// SPDX-FileCopyrightText: 2026 Pascal Klose
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QString>
#include <QStringList>
#include <QtTypes>

namespace F1Data
{
    /// The bits of TelemetryPacket::buttonStatus: the buttons held on the player's controller or wheel, named as the
    /// game's documentation names them (the PlayStation button first, then the Xbox one). Not read from the
    /// reference parser, which keeps the field as a number: see docs/udp-format-2018.md for how far it is checked.
    enum class Button : quint32
    {
        CrossOrA = 0x0001,
        TriangleOrY = 0x0002,
        CircleOrB = 0x0004,
        SquareOrX = 0x0008,
        DPadLeft = 0x0010,
        DPadRight = 0x0020,
        DPadUp = 0x0040,
        DPadDown = 0x0080,
        OptionsOrMenu = 0x0100,
        L1OrLb = 0x0200,
        R1OrRb = 0x0400,
        L2OrLt = 0x0800,
        R2OrRt = 0x1000,
        LeftStickClick = 0x2000,
        RightStickClick = 0x4000,
    };

    /// The names of the buttons held in a buttonStatus value; a bit the library has no name for is left out.
    [[nodiscard]] QStringList heldButtons(quint32 buttonStatus);

    // The names of the game's ids, in English. An id the library has no name for comes back as "Unknown (n)", so a
    // new value shows up as itself instead of as nothing. TrackName and the driver and team names are the 2018
    // game's; the classic cars and drivers are in them.

    [[nodiscard]] QString weatherName(int weather);
    [[nodiscard]] QString sessionTypeName(int sessionType);
    [[nodiscard]] QString trackName(int trackId);
    [[nodiscard]] QString eraName(int era);
    [[nodiscard]] QString safetyCarStatusName(int status);
    [[nodiscard]] QString flagName(int flag);
    [[nodiscard]] QString pitStatusName(int status);
    [[nodiscard]] QString driverStatusName(int status);
    [[nodiscard]] QString resultStatusName(int status);
    [[nodiscard]] QString driverName(int driverId);
    [[nodiscard]] QString teamName(int teamId);
    [[nodiscard]] QString nationalityName(int nationality);
    [[nodiscard]] QString fuelMixName(int fuelMix);
    [[nodiscard]] QString tyreCompoundName(int compound);
    [[nodiscard]] QString ersDeployModeName(int mode);
} // namespace F1Data
