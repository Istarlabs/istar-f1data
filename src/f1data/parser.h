// SPDX-FileCopyrightText: 2026 Pascal Klose
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "f1data/packets.h"

#include <QByteArray>
#include <QString>

#include <expected>

namespace F1Data
{
    /// Why a datagram is not a packet of F1 2018. An application that translates its messages switches over the
    /// kind; message() is the English text.
    struct ParseError
    {
        enum class Kind
        {
            /// Shorter than the 21 bytes of the header.
            CutShort,
            /// The header names another format than 2018 (another game, or a later year of the series).
            OtherFormat,
            /// A packet id the format does not have.
            UnknownPacketId,
            /// The size is not the one this packet type has.
            WrongSize,
        };

        Kind kind = Kind::CutShort;
        /// OtherFormat: the format the datagram names.
        quint16 packetFormat = 0;
        /// WrongSize: what the packet type needs and what arrived. UnknownPacketId: the id in expected.
        int expected = 0;
        int actual = 0;

        [[nodiscard]] QString message() const;
    };

    /// The size of the header every packet starts with.
    inline constexpr int HeaderSize = 21;

    /// The size of a whole datagram of this type, header included.
    [[nodiscard]] int packetSize(PacketId id);

    /// Reads one datagram. The game sends every packet type in one fixed size, so anything else is rejected whole
    /// instead of being read as far as it goes.
    std::expected<Packet, ParseError> parsePacket(const QByteArray& datagram);

    /// Only the header, for a listener that wants to look at the id before deciding what to read.
    std::expected<PacketHeader, ParseError> parseHeader(const QByteArray& datagram);
} // namespace F1Data
