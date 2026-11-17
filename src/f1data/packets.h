// SPDX-FileCopyrightText: 2026 Pascal Klose
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QString>
#include <QtTypes>

#include <array>
#include <variant>

/// What F1 2018 sends over UDP, as plain values. docs/udp-format-2018.md describes every packet byte by byte.
///
/// Fields keep the game's numbers: an id the library has no name for stays a number, and names.h turns the numbers
/// it knows into English text. Wheel arrays are in the game's order: rear left, rear right, front left, front right.
namespace F1Data
{
    /// The game sends one packet per UDP datagram, and every packet starts with a PacketHeader.
    inline constexpr quint16 PacketFormat = 2018;
    /// The cars in a session; the game always sends all 20 slots, unused ones are zeroed or stale.
    inline constexpr int CarCount = 20;
    /// Marshal zones in a SessionPacket; numMarshalZones says how many of them are filled in.
    inline constexpr int MarshalZoneCount = 21;

    enum class PacketId : quint8
    {
        Motion = 0,
        Session = 1,
        LapData = 2,
        Event = 3,
        Participants = 4,
        CarSetups = 5,
        CarTelemetry = 6,
        CarStatus = 7,
    };

    struct PacketHeader
    {
        quint16 packetFormat = 0;
        /// Version of this packet type, all start from 1.
        quint8 packetVersion = 0;
        PacketId packetId = PacketId::Motion;
        quint64 sessionUid = 0;
        /// Seconds since the session started.
        float sessionTime = 0;
        quint32 frameIdentifier = 0;
        /// Index of the player's car in every per-car array.
        quint8 playerCarIndex = 0;
    };

    struct CarMotion
    {
        /// World space, metres.
        float positionX = 0;
        float positionY = 0;
        float positionZ = 0;
        float velocityX = 0;
        float velocityY = 0;
        float velocityZ = 0;
        /// Normalised direction vectors, as 16-bit signed integers (divide by 32767 for a unit vector).
        qint16 forwardDirX = 0;
        qint16 forwardDirY = 0;
        qint16 forwardDirZ = 0;
        qint16 rightDirX = 0;
        qint16 rightDirY = 0;
        qint16 rightDirZ = 0;
        float gForceLateral = 0;
        float gForceLongitudinal = 0;
        float gForceVertical = 0;
        /// Radians.
        float yaw = 0;
        float pitch = 0;
        float roll = 0;
    };

    /// Where every car is and how it moves, plus extra values of the player's car only.
    struct MotionPacket
    {
        std::array<CarMotion, CarCount> cars{};

        // The player's car only.
        std::array<float, 4> suspensionPosition{};
        std::array<float, 4> suspensionVelocity{};
        std::array<float, 4> suspensionAcceleration{};
        std::array<float, 4> wheelSpeed{};
        std::array<float, 4> wheelSlip{};
        float localVelocityX = 0;
        float localVelocityY = 0;
        float localVelocityZ = 0;
        float angularVelocityX = 0;
        float angularVelocityY = 0;
        float angularVelocityZ = 0;
        float angularAccelerationX = 0;
        float angularAccelerationY = 0;
        float angularAccelerationZ = 0;
        /// Radians.
        float frontWheelsAngle = 0;
    };

    struct MarshalZone
    {
        /// Fraction (0..1) of the way through the lap the zone starts.
        float start = 0;
        /// -1 unknown, 0 none, 1 green, 2 blue, 3 yellow, 4 red.
        qint8 flag = 0;
    };

    struct SessionPacket
    {
        /// 0 clear, 1 light cloud, 2 overcast, 3 light rain, 4 heavy rain, 5 storm.
        quint8 weather = 0;
        /// Degrees Celsius.
        qint8 trackTemperature = 0;
        qint8 airTemperature = 0;
        quint8 totalLaps = 0;
        /// Metres.
        quint16 trackLength = 0;
        /// 0 unknown, 1-3 practice, 4 short practice, 5-7 qualifying, 8 short qualifying, 9 one-shot qualifying,
        /// 10 race, 11 race 2, 12 time trial.
        quint8 sessionType = 0;
        /// -1 unknown, 0-24 the tracks of the game.
        qint8 trackId = 0;
        /// 0 modern, 1 classic.
        quint8 era = 0;
        /// Seconds.
        quint16 sessionTimeLeft = 0;
        quint16 sessionDuration = 0;
        /// Kilometres per hour.
        quint8 pitSpeedLimit = 0;
        bool gamePaused = false;
        bool isSpectating = false;
        quint8 spectatorCarIndex = 0;
        /// SLI Pro support: 0 inactive, 1 active.
        quint8 sliProNativeSupport = 0;
        quint8 numMarshalZones = 0;
        std::array<MarshalZone, MarshalZoneCount> marshalZones{};
        /// 0 none, 1 full safety car, 2 virtual safety car.
        quint8 safetyCarStatus = 0;
        /// 0 offline, 1 online.
        quint8 networkGame = 0;
    };

    struct Lap
    {
        /// Seconds.
        float lastLapTime = 0;
        float currentLapTime = 0;
        float bestLapTime = 0;
        float sector1Time = 0;
        float sector2Time = 0;
        /// Metres around the current lap; negative until the line is crossed.
        float lapDistance = 0;
        /// Metres in the session; negative until the line is crossed.
        float totalDistance = 0;
        /// Seconds.
        float safetyCarDelta = 0;
        quint8 carPosition = 0;
        quint8 currentLapNumber = 0;
        /// 0 none, 1 pitting, 2 in the pit area.
        quint8 pitStatus = 0;
        /// 0 sector 1, 1 sector 2, 2 sector 3.
        quint8 sector = 0;
        bool currentLapInvalid = false;
        /// Accumulated time penalties in seconds.
        quint8 penalties = 0;
        quint8 gridPosition = 0;
        /// 0 in garage, 1 flying lap, 2 in lap, 3 out lap, 4 on track.
        quint8 driverStatus = 0;
        /// 0 invalid, 1 inactive, 2 active, 3 finished, 4 disqualified, 5 not classified, 6 retired.
        quint8 resultStatus = 0;
    };

    struct LapPacket
    {
        std::array<Lap, CarCount> laps{};
    };

    /// Something happened in the session. The code is four characters; the game's documentation names two of them,
    /// "SSTA" (session started) and "SEND" (session ended).
    struct EventPacket
    {
        QString code;
    };

    struct Participant
    {
        bool aiControlled = false;
        quint8 driverId = 0;
        quint8 teamId = 0;
        quint8 raceNumber = 0;
        quint8 nationality = 0;
        /// UTF-8, cut by the game with an ellipsis when it was too long.
        QString name;
    };

    struct ParticipantsPacket
    {
        /// How many of the 20 slots are cars of this session.
        quint8 numCars = 0;
        std::array<Participant, CarCount> participants{};
    };

    struct CarSetup
    {
        quint8 frontWing = 0;
        quint8 rearWing = 0;
        /// Differential adjustment on and off throttle, percent.
        quint8 onThrottle = 0;
        quint8 offThrottle = 0;
        float frontCamber = 0;
        float rearCamber = 0;
        float frontToe = 0;
        float rearToe = 0;
        quint8 frontSuspension = 0;
        quint8 rearSuspension = 0;
        quint8 frontAntiRollBar = 0;
        quint8 rearAntiRollBar = 0;
        quint8 frontSuspensionHeight = 0;
        quint8 rearSuspensionHeight = 0;
        /// Percent.
        quint8 brakePressure = 0;
        quint8 brakeBias = 0;
        /// PSI.
        float frontTyrePressure = 0;
        float rearTyrePressure = 0;
        quint8 ballast = 0;
        float fuelLoad = 0;
    };

    struct SetupPacket
    {
        std::array<CarSetup, CarCount> cars{};
    };

    struct CarTelemetry
    {
        /// Kilometres per hour.
        quint16 speed = 0;
        /// 0 to 100.
        quint8 throttle = 0;
        /// -100 (full lock left) to 100 (full lock right).
        qint8 steer = 0;
        quint8 brake = 0;
        quint8 clutch = 0;
        /// 1-8, neutral 0, reverse -1.
        qint8 gear = 0;
        quint16 engineRpm = 0;
        bool drs = false;
        /// Percent.
        quint8 revLightsPercent = 0;
        /// Degrees Celsius.
        std::array<quint16, 4> brakesTemperature{};
        std::array<quint16, 4> tyresSurfaceTemperature{};
        std::array<quint16, 4> tyresInnerTemperature{};
        quint16 engineTemperature = 0;
        /// PSI.
        std::array<float, 4> tyresPressure{};
    };

    /// The cars' telemetry and the buttons held on the player's controller or wheel.
    struct TelemetryPacket
    {
        std::array<CarTelemetry, CarCount> cars{};
        /// Bit flags, see Button in names.h.
        quint32 buttonStatus = 0;
    };

    struct CarStatus
    {
        /// 0 off to 2 high.
        quint8 tractionControl = 0;
        bool antiLockBrakes = false;
        /// 0 lean, 1 standard, 2 rich, 3 max.
        quint8 fuelMix = 0;
        /// Percent.
        quint8 frontBrakeBias = 0;
        bool pitLimiter = false;
        /// Mass.
        float fuelInTank = 0;
        float fuelCapacity = 0;
        quint16 maxRpm = 0;
        quint16 idleRpm = 0;
        quint8 maxGears = 0;
        /// 0 not allowed, 1 allowed, 255 (-1 in the game's documentation) unknown.
        quint8 drsAllowed = 0;
        /// Percent.
        std::array<quint8, 4> tyresWear{};
        /// Modern: 0 hyper soft, 1 ultra soft, 2 super soft, 3 soft, 4 medium, 5 hard, 6 super hard, 7 inter, 8 wet.
        /// Classic: 0-6 dry, 7-8 wet.
        quint8 tyreCompound = 0;
        /// Percent.
        std::array<quint8, 4> tyresDamage{};
        quint8 frontLeftWingDamage = 0;
        quint8 frontRightWingDamage = 0;
        quint8 rearWingDamage = 0;
        quint8 engineDamage = 0;
        quint8 gearBoxDamage = 0;
        quint8 exhaustDamage = 0;
        /// -1 unknown, 0 none, 1 green, 2 blue, 3 yellow, 4 red.
        qint8 vehicleFiaFlags = 0;
        /// Joules.
        float ersStoreEnergy = 0;
        /// 0 none, 1 low, 2 medium, 3 high, 4 overtake, 5 hotlap.
        quint8 ersDeployMode = 0;
        float ersHarvestedThisLapMguk = 0;
        float ersHarvestedThisLapMguh = 0;
        float ersDeployedThisLap = 0;
    };

    struct StatusPacket
    {
        std::array<CarStatus, CarCount> cars{};
    };

    using PacketBody = std::variant<MotionPacket, SessionPacket, LapPacket, EventPacket, ParticipantsPacket,
                                    SetupPacket, TelemetryPacket, StatusPacket>;

    struct Packet
    {
        PacketHeader header;
        PacketBody body;
    };
} // namespace F1Data
