// SPDX-FileCopyrightText: 2026 Pascal Klose
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "f1data/parser.h"

#include <QDataStream>
#include <QTest>

using namespace F1Data;

namespace
{
    /// A datagram of the size the type has, zero-filled, with a header that says what it is. The tests put their
    /// values in at offsets written down by hand from the game's documentation, so they check the layout the parser
    /// reads and do not just repeat it.
    QByteArray datagram(PacketId id, quint16 format = PacketFormat)
    {
        QByteArray data(packetSize(id), '\0');
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        stream << format << quint8{1} << static_cast<quint8>(id) << quint64{0x1122334455667788ULL} << 12.5F
               << quint32{4711} << quint8{3};

        return data;
    }

    template <typename T>
    void put(QByteArray& data, int offset, T value)
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.device()->seek(offset);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        stream << value;
    }

    template <typename T>
    const T& bodyOf(const Packet& packet)
    {
        return std::get<T>(packet.body);
    }
} // namespace

class ParserTest : public QObject // NOLINT(misc-use-internal-linkage): moc needs the class at file scope
{
    Q_OBJECT

private Q_SLOTS:
    void everyTypeHasItsDocumentedSize_data()
    {
        QTest::addColumn<int>("id");
        QTest::addColumn<int>("size");

        QTest::newRow("motion") << 0 << 1341;
        QTest::newRow("session") << 1 << 147;
        QTest::newRow("lap data") << 2 << 841;
        QTest::newRow("event") << 3 << 25;
        QTest::newRow("participants") << 4 << 1082;
        QTest::newRow("car setups") << 5 << 841;
        QTest::newRow("car telemetry") << 6 << 1085;
        QTest::newRow("car status") << 7 << 1061;
    }

    void everyTypeHasItsDocumentedSize()
    {
        QFETCH(int, id); // NOLINT(misc-const-correctness): the macro declares it
        QFETCH(int, size); // NOLINT(misc-const-correctness): the macro declares it

        QCOMPARE(packetSize(static_cast<PacketId>(id)), size);

        const auto packet = parsePacket(datagram(static_cast<PacketId>(id)));
        QVERIFY(packet.has_value());
        QCOMPARE(static_cast<int>(packet->header.packetId), id);
    }

    void readsTheHeader()
    {
        const auto packet = parsePacket(datagram(PacketId::Session));

        QVERIFY(packet.has_value());
        QCOMPARE(packet->header.packetFormat, PacketFormat);
        QCOMPARE(packet->header.packetVersion, quint8{1});
        QCOMPARE(packet->header.sessionUid, quint64{0x1122334455667788ULL});
        QCOMPARE(packet->header.sessionTime, 12.5F);
        QCOMPARE(packet->header.frameIdentifier, quint32{4711});
        QCOMPARE(packet->header.playerCarIndex, quint8{3});
    }

    void readsTelemetryOfOneCarAndTheButtons()
    {
        QByteArray data = datagram(PacketId::CarTelemetry);
        constexpr int car = 21 + (53 * 3);
        put(data, car + 0, quint16{287});
        put(data, car + 2, quint8{100});
        put(data, car + 3, qint8{-40});
        put(data, car + 4, quint8{12});
        put(data, car + 6, qint8{-1});
        put(data, car + 7, quint16{11500});
        put(data, car + 9, quint8{1});
        put(data, car + 11 + 6, quint16{650});
        put(data, car + 27 + 4, quint16{98});
        put(data, car + 35, quint16{104});
        put(data, car + 37 + 12, 22.5F);
        put(data, 1081, quint32{0x0201});

        const auto packet = parsePacket(data);

        QVERIFY(packet.has_value());
        const auto& telemetry = bodyOf<TelemetryPacket>(*packet);
        const CarTelemetry& player = telemetry.cars.at(3);
        QCOMPARE(player.speed, quint16{287});
        QCOMPARE(player.throttle, quint8{100});
        QCOMPARE(player.steer, qint8{-40});
        QCOMPARE(player.brake, quint8{12});
        QCOMPARE(player.gear, qint8{-1});
        QCOMPARE(player.engineRpm, quint16{11500});
        QVERIFY(player.drs);
        QCOMPARE(player.brakesTemperature.at(3), quint16{650});
        QCOMPARE(player.tyresInnerTemperature.at(2), quint16{98});
        QCOMPARE(player.engineTemperature, quint16{104});
        QCOMPARE(player.tyresPressure.at(3), 22.5F);
        QCOMPARE(telemetry.buttonStatus, quint32{0x0201});
        QCOMPARE(telemetry.cars.at(2).speed, quint16{0});
    }

    void readsLapData()
    {
        QByteArray data = datagram(PacketId::LapData);
        constexpr int car = 21 + (41 * 19);
        put(data, car + 0, 83.5F);
        put(data, car + 4, 12.25F);
        put(data, car + 8, 81.75F);
        put(data, car + 24, -3.5F);
        put(data, car + 32, quint8{7});
        put(data, car + 33, quint8{4});
        put(data, car + 36, quint8{1});
        put(data, car + 37, quint8{5});
        put(data, car + 40, quint8{2});

        const auto packet = parsePacket(data);

        QVERIFY(packet.has_value());
        const Lap& lap = bodyOf<LapPacket>(*packet).laps.at(19);
        QCOMPARE(lap.lastLapTime, 83.5F);
        QCOMPARE(lap.currentLapTime, 12.25F);
        QCOMPARE(lap.bestLapTime, 81.75F);
        QCOMPARE(lap.totalDistance, -3.5F);
        QCOMPARE(lap.carPosition, quint8{7});
        QCOMPARE(lap.currentLapNumber, quint8{4});
        QVERIFY(lap.currentLapInvalid);
        QCOMPARE(lap.penalties, quint8{5});
        QCOMPARE(lap.resultStatus, quint8{2});
    }

    void readsTheSessionAndItsMarshalZones()
    {
        QByteArray data = datagram(PacketId::Session);
        put(data, 21, quint8{3});
        put(data, 22, qint8{-2});
        put(data, 24, quint8{58});
        put(data, 25, quint16{5303});
        put(data, 27, quint8{10});
        put(data, 28, qint8{11});
        put(data, 30, quint16{3600});
        put(data, 35, quint8{1});
        put(data, 39, quint8{2});
        put(data, 40, 0.5F);
        put(data, 44, qint8{3});
        put(data, 40 + (5 * 20), 0.75F);
        put(data, 40 + (5 * 20) + 4, qint8{4});
        put(data, 145, quint8{2});
        put(data, 146, quint8{1});

        const auto packet = parsePacket(data);

        QVERIFY(packet.has_value());
        const auto& session = bodyOf<SessionPacket>(*packet);
        QCOMPARE(session.weather, quint8{3});
        QCOMPARE(session.trackTemperature, qint8{-2});
        QCOMPARE(session.totalLaps, quint8{58});
        QCOMPARE(session.trackLength, quint16{5303});
        QCOMPARE(session.sessionType, quint8{10});
        QCOMPARE(session.trackId, qint8{11});
        QCOMPARE(session.sessionTimeLeft, quint16{3600});
        QVERIFY(session.gamePaused);
        QVERIFY(!session.isSpectating);
        QCOMPARE(session.numMarshalZones, quint8{2});
        QCOMPARE(session.marshalZones.at(0).start, 0.5F);
        QCOMPARE(session.marshalZones.at(0).flag, qint8{3});
        QCOMPARE(session.marshalZones.at(20).start, 0.75F);
        QCOMPARE(session.marshalZones.at(20).flag, qint8{4});
        QCOMPARE(session.safetyCarStatus, quint8{2});
        QCOMPARE(session.networkGame, quint8{1});
    }

    void readsParticipantsAndStopsTheNameAtTheZeroByte()
    {
        QByteArray data = datagram(PacketId::Participants);
        constexpr int slot = 22 + (53 * 1);
        put(data, 21, quint8{2});
        put(data, slot + 0, quint8{1});
        put(data, slot + 1, quint8{7});
        put(data, slot + 2, quint8{0});
        put(data, slot + 3, quint8{44});
        put(data, slot + 4, quint8{10});
        // The field is 48 bytes and the name 11 of them: the zero bytes after it are the terminator.
        data.replace(slot + 5, 11, QString::fromUtf8("Räikkönen").toUtf8());

        const auto packet = parsePacket(data);

        QVERIFY(packet.has_value());
        const auto& participants = bodyOf<ParticipantsPacket>(*packet);
        QCOMPARE(participants.numCars, quint8{2});
        const Participant& driver = participants.participants.at(1);
        QVERIFY(driver.aiControlled);
        QCOMPARE(driver.driverId, quint8{7});
        QCOMPARE(driver.raceNumber, quint8{44});
        QCOMPARE(driver.name, QString::fromUtf8("Räikkönen"));
    }

    void readsMotionIncludingThePlayersExtraValues()
    {
        QByteArray data = datagram(PacketId::Motion);
        constexpr int car = 21 + (60 * 2);
        put(data, car + 0, 100.5F);
        put(data, car + 24, qint16{-32767});
        put(data, car + 36, 1.5F);
        put(data, car + 56, -0.25F);
        put(data, 1221 + 12, 7.5F);
        put(data, 1285 + 4, 0.125F);
        put(data, 1337, 0.5F);

        const auto packet = parsePacket(data);

        QVERIFY(packet.has_value());
        const auto& motion = bodyOf<MotionPacket>(*packet);
        QCOMPARE(motion.cars.at(2).positionX, 100.5F);
        QCOMPARE(motion.cars.at(2).forwardDirX, qint16{-32767});
        QCOMPARE(motion.cars.at(2).gForceLateral, 1.5F);
        QCOMPARE(motion.cars.at(2).roll, -0.25F);
        QCOMPARE(motion.suspensionPosition.at(3), 7.5F);
        QCOMPARE(motion.wheelSlip.at(1), 0.125F);
        QCOMPARE(motion.frontWheelsAngle, 0.5F);
    }

    void readsCarStatus()
    {
        QByteArray data = datagram(PacketId::CarStatus);
        constexpr int car = 21 + (52 * 3);
        put(data, car + 0, quint8{2});
        put(data, car + 1, quint8{1});
        put(data, car + 5, 42.5F);
        put(data, car + 13, quint16{12000});
        put(data, car + 19 + 3, quint8{31});
        put(data, car + 23, quint8{1});
        put(data, car + 34, qint8{-1});
        put(data, car + 35, 4.0e6F);
        put(data, car + 39, quint8{4});
        put(data, car + 48, 1.5e6F);

        const auto packet = parsePacket(data);

        QVERIFY(packet.has_value());
        const CarStatus& status = bodyOf<StatusPacket>(*packet).cars.at(3);
        QCOMPARE(status.tractionControl, quint8{2});
        QVERIFY(status.antiLockBrakes);
        QCOMPARE(status.fuelInTank, 42.5F);
        QCOMPARE(status.maxRpm, quint16{12000});
        QCOMPARE(status.tyresWear.at(3), quint8{31});
        QCOMPARE(status.tyreCompound, quint8{1});
        QCOMPARE(status.vehicleFiaFlags, qint8{-1});
        QCOMPARE(status.ersStoreEnergy, 4.0e6F);
        QCOMPARE(status.ersDeployMode, quint8{4});
        QCOMPARE(status.ersDeployedThisLap, 1.5e6F);
    }

    void readsCarSetups()
    {
        QByteArray data = datagram(PacketId::CarSetups);
        constexpr int car = 21 + (41 * 3);
        put(data, car + 0, quint8{6});
        put(data, car + 4, -3.5F);
        put(data, car + 27, quint8{58});
        put(data, car + 32, 22.5F);
        put(data, car + 37, 80.0F);

        const auto packet = parsePacket(data);

        QVERIFY(packet.has_value());
        const CarSetup& setup = bodyOf<SetupPacket>(*packet).cars.at(3);
        QCOMPARE(setup.frontWing, quint8{6});
        QCOMPARE(setup.frontCamber, -3.5F);
        QCOMPARE(setup.brakeBias, quint8{58});
        QCOMPARE(setup.rearTyrePressure, 22.5F);
        QCOMPARE(setup.fuelLoad, 80.0F);
    }

    void readsTheEventCode()
    {
        QByteArray data = datagram(PacketId::Event);
        data.replace(21, 4, "SSTA");

        const auto packet = parsePacket(data);

        QVERIFY(packet.has_value());
        QCOMPARE(bodyOf<EventPacket>(*packet).code, QStringLiteral("SSTA"));
    }

    void rejectsWhatIsNotAPacketOf2018()
    {
        const auto cut = parsePacket(QByteArray(20, '\0'));
        QVERIFY(!cut.has_value());
        QCOMPARE(cut.error().kind, ParseError::Kind::CutShort);

        const auto other = parsePacket(datagram(PacketId::LapData, 2019));
        QVERIFY(!other.has_value());
        QCOMPARE(other.error().kind, ParseError::Kind::OtherFormat);
        QCOMPARE(other.error().packetFormat, quint16{2019});

        QByteArray unknown = datagram(PacketId::Event);
        put(unknown, 3, quint8{8});
        QCOMPARE(parsePacket(unknown).error().kind, ParseError::Kind::UnknownPacketId);

        QByteArray longer = datagram(PacketId::LapData);
        longer.append('\0');
        const auto wrong = parsePacket(longer);
        QVERIFY(!wrong.has_value());
        QCOMPARE(wrong.error().kind, ParseError::Kind::WrongSize);
        QCOMPARE(wrong.error().expected, 841);
        QCOMPARE(wrong.error().actual, 842);
        QVERIFY(!wrong.error().message().isEmpty());
    }

    void readsTheHeaderAlone()
    {
        const auto header = parseHeader(datagram(PacketId::CarStatus).first(HeaderSize));

        QVERIFY(header.has_value());
        QCOMPARE(header->packetId, PacketId::CarStatus);
    }
};

QTEST_APPLESS_MAIN(ParserTest)

#include "parsertest.moc"
