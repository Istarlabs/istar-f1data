// SPDX-FileCopyrightText: 2026 Pascal Klose
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "f1data/parser.h"

#include <QDataStream>

namespace F1Data
{
    namespace
    {
        constexpr int MaxPacketId = static_cast<int>(PacketId::CarStatus);
        constexpr int ParticipantNameBytes = 48;

        /// Little-endian reads over a datagram whose size is already checked, so a read cannot run past the end.
        class Reader
        {
        public:
            explicit Reader(const QByteArray& data) : m_stream(data)
            {
                m_stream.setByteOrder(QDataStream::LittleEndian);
                m_stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
            }

            template <typename T>
            T read()
            {
                T value{};
                m_stream >> value;
                return value;
            }

            template <typename T, std::size_t Count>
            std::array<T, Count> readArray()
            {
                std::array<T, Count> values{};

                for (T& value : values)
                {
                    value = read<T>();
                }

                return values;
            }

            /// A UTF-8 text in a fixed field, which ends at the first zero byte.
            QString readText(int fieldSize)
            {
                QByteArray field(fieldSize, '\0');
                m_stream.readRawData(field.data(), fieldSize);

                return QString::fromUtf8(field.constData());
            }

        private:
            QDataStream m_stream;
        };

        PacketHeader readHeader(Reader& reader)
        {
            PacketHeader header;
            header.packetFormat = reader.read<quint16>();
            header.packetVersion = reader.read<quint8>();
            header.packetId = static_cast<PacketId>(reader.read<quint8>());
            header.sessionUid = reader.read<quint64>();
            header.sessionTime = reader.read<float>();
            header.frameIdentifier = reader.read<quint32>();
            header.playerCarIndex = reader.read<quint8>();

            return header;
        }

        CarMotion readCarMotion(Reader& reader)
        {
            CarMotion car;
            car.positionX = reader.read<float>();
            car.positionY = reader.read<float>();
            car.positionZ = reader.read<float>();
            car.velocityX = reader.read<float>();
            car.velocityY = reader.read<float>();
            car.velocityZ = reader.read<float>();
            car.forwardDirX = reader.read<qint16>();
            car.forwardDirY = reader.read<qint16>();
            car.forwardDirZ = reader.read<qint16>();
            car.rightDirX = reader.read<qint16>();
            car.rightDirY = reader.read<qint16>();
            car.rightDirZ = reader.read<qint16>();
            car.gForceLateral = reader.read<float>();
            car.gForceLongitudinal = reader.read<float>();
            car.gForceVertical = reader.read<float>();
            car.yaw = reader.read<float>();
            car.pitch = reader.read<float>();
            car.roll = reader.read<float>();

            return car;
        }

        MotionPacket readMotion(Reader& reader)
        {
            MotionPacket packet;

            for (CarMotion& car : packet.cars)
            {
                car = readCarMotion(reader);
            }

            packet.suspensionPosition = reader.readArray<float, 4>();
            packet.suspensionVelocity = reader.readArray<float, 4>();
            packet.suspensionAcceleration = reader.readArray<float, 4>();
            packet.wheelSpeed = reader.readArray<float, 4>();
            packet.wheelSlip = reader.readArray<float, 4>();
            packet.localVelocityX = reader.read<float>();
            packet.localVelocityY = reader.read<float>();
            packet.localVelocityZ = reader.read<float>();
            packet.angularVelocityX = reader.read<float>();
            packet.angularVelocityY = reader.read<float>();
            packet.angularVelocityZ = reader.read<float>();
            packet.angularAccelerationX = reader.read<float>();
            packet.angularAccelerationY = reader.read<float>();
            packet.angularAccelerationZ = reader.read<float>();
            packet.frontWheelsAngle = reader.read<float>();

            return packet;
        }

        SessionPacket readSession(Reader& reader)
        {
            SessionPacket packet;
            packet.weather = reader.read<quint8>();
            packet.trackTemperature = reader.read<qint8>();
            packet.airTemperature = reader.read<qint8>();
            packet.totalLaps = reader.read<quint8>();
            packet.trackLength = reader.read<quint16>();
            packet.sessionType = reader.read<quint8>();
            packet.trackId = reader.read<qint8>();
            packet.era = reader.read<quint8>();
            packet.sessionTimeLeft = reader.read<quint16>();
            packet.sessionDuration = reader.read<quint16>();
            packet.pitSpeedLimit = reader.read<quint8>();
            packet.gamePaused = reader.read<quint8>() != 0;
            packet.isSpectating = reader.read<quint8>() != 0;
            packet.spectatorCarIndex = reader.read<quint8>();
            packet.sliProNativeSupport = reader.read<quint8>();
            packet.numMarshalZones = reader.read<quint8>();

            for (MarshalZone& zone : packet.marshalZones)
            {
                zone.start = reader.read<float>();
                zone.flag = reader.read<qint8>();
            }

            packet.safetyCarStatus = reader.read<quint8>();
            packet.networkGame = reader.read<quint8>();

            return packet;
        }

        Lap readLap(Reader& reader)
        {
            Lap lap;
            lap.lastLapTime = reader.read<float>();
            lap.currentLapTime = reader.read<float>();
            lap.bestLapTime = reader.read<float>();
            lap.sector1Time = reader.read<float>();
            lap.sector2Time = reader.read<float>();
            lap.lapDistance = reader.read<float>();
            lap.totalDistance = reader.read<float>();
            lap.safetyCarDelta = reader.read<float>();
            lap.carPosition = reader.read<quint8>();
            lap.currentLapNumber = reader.read<quint8>();
            lap.pitStatus = reader.read<quint8>();
            lap.sector = reader.read<quint8>();
            lap.currentLapInvalid = reader.read<quint8>() != 0;
            lap.penalties = reader.read<quint8>();
            lap.gridPosition = reader.read<quint8>();
            lap.driverStatus = reader.read<quint8>();
            lap.resultStatus = reader.read<quint8>();

            return lap;
        }

        LapPacket readLapPacket(Reader& reader)
        {
            LapPacket packet;

            for (Lap& lap : packet.laps)
            {
                lap = readLap(reader);
            }

            return packet;
        }

        EventPacket readEvent(Reader& reader)
        {
            return EventPacket{.code = reader.readText(4)};
        }

        Participant readParticipant(Reader& reader)
        {
            Participant participant;
            participant.aiControlled = reader.read<quint8>() != 0;
            participant.driverId = reader.read<quint8>();
            participant.teamId = reader.read<quint8>();
            participant.raceNumber = reader.read<quint8>();
            participant.nationality = reader.read<quint8>();
            participant.name = reader.readText(ParticipantNameBytes);

            return participant;
        }

        ParticipantsPacket readParticipants(Reader& reader)
        {
            ParticipantsPacket packet;
            packet.numCars = reader.read<quint8>();

            for (Participant& participant : packet.participants)
            {
                participant = readParticipant(reader);
            }

            return packet;
        }

        CarSetup readCarSetup(Reader& reader)
        {
            CarSetup setup;
            setup.frontWing = reader.read<quint8>();
            setup.rearWing = reader.read<quint8>();
            setup.onThrottle = reader.read<quint8>();
            setup.offThrottle = reader.read<quint8>();
            setup.frontCamber = reader.read<float>();
            setup.rearCamber = reader.read<float>();
            setup.frontToe = reader.read<float>();
            setup.rearToe = reader.read<float>();
            setup.frontSuspension = reader.read<quint8>();
            setup.rearSuspension = reader.read<quint8>();
            setup.frontAntiRollBar = reader.read<quint8>();
            setup.rearAntiRollBar = reader.read<quint8>();
            setup.frontSuspensionHeight = reader.read<quint8>();
            setup.rearSuspensionHeight = reader.read<quint8>();
            setup.brakePressure = reader.read<quint8>();
            setup.brakeBias = reader.read<quint8>();
            setup.frontTyrePressure = reader.read<float>();
            setup.rearTyrePressure = reader.read<float>();
            setup.ballast = reader.read<quint8>();
            setup.fuelLoad = reader.read<float>();

            return setup;
        }

        SetupPacket readSetups(Reader& reader)
        {
            SetupPacket packet;

            for (CarSetup& setup : packet.cars)
            {
                setup = readCarSetup(reader);
            }

            return packet;
        }

        CarTelemetry readCarTelemetry(Reader& reader)
        {
            CarTelemetry car;
            car.speed = reader.read<quint16>();
            car.throttle = reader.read<quint8>();
            car.steer = reader.read<qint8>();
            car.brake = reader.read<quint8>();
            car.clutch = reader.read<quint8>();
            car.gear = reader.read<qint8>();
            car.engineRpm = reader.read<quint16>();
            car.drs = reader.read<quint8>() != 0;
            car.revLightsPercent = reader.read<quint8>();
            car.brakesTemperature = reader.readArray<quint16, 4>();
            car.tyresSurfaceTemperature = reader.readArray<quint16, 4>();
            car.tyresInnerTemperature = reader.readArray<quint16, 4>();
            car.engineTemperature = reader.read<quint16>();
            car.tyresPressure = reader.readArray<float, 4>();

            return car;
        }

        TelemetryPacket readTelemetry(Reader& reader)
        {
            TelemetryPacket packet;

            for (CarTelemetry& car : packet.cars)
            {
                car = readCarTelemetry(reader);
            }

            packet.buttonStatus = reader.read<quint32>();

            return packet;
        }

        CarStatus readCarStatus(Reader& reader)
        {
            CarStatus car;
            car.tractionControl = reader.read<quint8>();
            car.antiLockBrakes = reader.read<quint8>() != 0;
            car.fuelMix = reader.read<quint8>();
            car.frontBrakeBias = reader.read<quint8>();
            car.pitLimiter = reader.read<quint8>() != 0;
            car.fuelInTank = reader.read<float>();
            car.fuelCapacity = reader.read<float>();
            car.maxRpm = reader.read<quint16>();
            car.idleRpm = reader.read<quint16>();
            car.maxGears = reader.read<quint8>();
            car.drsAllowed = reader.read<quint8>();
            car.tyresWear = reader.readArray<quint8, 4>();
            car.tyreCompound = reader.read<quint8>();
            car.tyresDamage = reader.readArray<quint8, 4>();
            car.frontLeftWingDamage = reader.read<quint8>();
            car.frontRightWingDamage = reader.read<quint8>();
            car.rearWingDamage = reader.read<quint8>();
            car.engineDamage = reader.read<quint8>();
            car.gearBoxDamage = reader.read<quint8>();
            car.exhaustDamage = reader.read<quint8>();
            car.vehicleFiaFlags = reader.read<qint8>();
            car.ersStoreEnergy = reader.read<float>();
            car.ersDeployMode = reader.read<quint8>();
            car.ersHarvestedThisLapMguk = reader.read<float>();
            car.ersHarvestedThisLapMguh = reader.read<float>();
            car.ersDeployedThisLap = reader.read<float>();

            return car;
        }

        StatusPacket readStatus(Reader& reader)
        {
            StatusPacket packet;

            for (CarStatus& car : packet.cars)
            {
                car = readCarStatus(reader);
            }

            return packet;
        }

        PacketBody readBody(PacketId id, Reader& reader)
        {
            switch (id)
            {
                case PacketId::Motion:
                    return readMotion(reader);
                case PacketId::Session:
                    return readSession(reader);
                case PacketId::LapData:
                    return readLapPacket(reader);
                case PacketId::Event:
                    return readEvent(reader);
                case PacketId::Participants:
                    return readParticipants(reader);
                case PacketId::CarSetups:
                    return readSetups(reader);
                case PacketId::CarTelemetry:
                    return readTelemetry(reader);
                case PacketId::CarStatus:
                    return readStatus(reader);
            }

            // Not reached: the id is checked before the body is read.
            return EventPacket{};
        }
    } // namespace

    QString ParseError::message() const
    {
        switch (kind)
        {
            case Kind::CutShort:
                return QStringLiteral("The datagram has %1 bytes, less than the %2 of a header")
                    .arg(actual)
                    .arg(HeaderSize);
            case Kind::OtherFormat:
                return QStringLiteral("The datagram is of packet format %1, not 2018").arg(packetFormat);
            case Kind::UnknownPacketId:
                return QStringLiteral("The datagram has packet id %1, which the format does not have").arg(expected);
            case Kind::WrongSize:
                return QStringLiteral("The packet has %1 bytes, its type has %2").arg(actual).arg(expected);
        }

        return {};
    }

    int packetSize(PacketId id)
    {
        switch (id)
        {
            case PacketId::Motion:
                return 1341;
            case PacketId::Session:
                return 147;
            case PacketId::LapData:
                return 841;
            case PacketId::Event:
                return 25;
            case PacketId::Participants:
                return 1082;
            case PacketId::CarSetups:
                return 841;
            case PacketId::CarTelemetry:
                return 1085;
            case PacketId::CarStatus:
                return 1061;
        }

        return 0;
    }

    std::expected<PacketHeader, ParseError> parseHeader(const QByteArray& datagram)
    {
        if (datagram.size() < HeaderSize)
        {
            return std::unexpected(
                ParseError{.kind = ParseError::Kind::CutShort, .actual = static_cast<int>(datagram.size())});
        }

        Reader reader(datagram);
        const PacketHeader header = readHeader(reader);

        if (header.packetFormat != PacketFormat)
        {
            return std::unexpected(
                ParseError{.kind = ParseError::Kind::OtherFormat, .packetFormat = header.packetFormat});
        }

        if (static_cast<int>(header.packetId) > MaxPacketId)
        {
            return std::unexpected(
                ParseError{.kind = ParseError::Kind::UnknownPacketId, .expected = static_cast<int>(header.packetId)});
        }

        return header;
    }

    std::expected<Packet, ParseError> parsePacket(const QByteArray& datagram)
    {
        const auto header = parseHeader(datagram);

        if (!header)
        {
            return std::unexpected(header.error());
        }

        const int expectedSize = packetSize(header->packetId);

        if (datagram.size() != expectedSize)
        {
            return std::unexpected(ParseError{.kind = ParseError::Kind::WrongSize,
                                              .expected = expectedSize,
                                              .actual = static_cast<int>(datagram.size())});
        }

        Reader reader(datagram);
        readHeader(reader);

        return Packet{.header = *header, .body = readBody(header->packetId, reader)};
    }
} // namespace F1Data
