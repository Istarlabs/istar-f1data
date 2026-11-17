// SPDX-FileCopyrightText: 2026 Pascal Klose
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "f1data/names.h"

#include <QTest>

using namespace F1Data;

class NamesTest : public QObject // NOLINT(misc-use-internal-linkage): moc needs the class at file scope
{
    Q_OBJECT

private Q_SLOTS:
    void namesWhatItKnows()
    {
        QCOMPARE(weatherName(3), QStringLiteral("Light rain"));
        QCOMPARE(sessionTypeName(12), QStringLiteral("Time trial"));
        QCOMPARE(trackName(24), QStringLiteral("Suzuka short"));
        QCOMPARE(teamName(29), QStringLiteral("Lotus 1978"));
        QCOMPARE(driverName(6), QString::fromUtf8("Kimi Räikkönen"));
        QCOMPARE(driverName(71), QStringLiteral("Jack Tremblay"));
        QCOMPARE(nationalityName(29), QStringLiteral("German"));
        QCOMPARE(nationalityName(87), QStringLiteral("Welsh"));
        QCOMPARE(tyreCompoundName(8), QStringLiteral("Wet"));
    }

    void anIdWithoutANameShowsAsItself()
    {
        QCOMPARE(driverName(1), QStringLiteral("Unknown (1)"));
        QCOMPARE(driverName(72), QStringLiteral("Unknown (72)"));
        QCOMPARE(nationalityName(0), QStringLiteral("Unknown (0)"));
        QCOMPARE(flagName(-1), QStringLiteral("Unknown (-1)"));
        QCOMPARE(weatherName(255), QStringLiteral("Unknown (255)"));
    }

    void listsTheHeldButtons()
    {
        QCOMPARE(heldButtons(0), QStringList());
        QCOMPARE(heldButtons(0x0201), (QStringList{QStringLiteral("Cross / A"), QStringLiteral("L1 / LB")}));
        // A bit without a name is left out.
        QCOMPARE(heldButtons(0x8000), QStringList());
    }
};

QTEST_APPLESS_MAIN(NamesTest)

#include "namestest.moc"
