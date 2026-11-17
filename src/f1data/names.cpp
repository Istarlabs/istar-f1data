// SPDX-FileCopyrightText: 2026 Pascal Klose
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "f1data/names.h"

#include <array>
#include <string_view>
#include <utility>

namespace F1Data
{
    namespace
    {
        /// The name at an index of a table, or "Unknown (n)". An empty entry is a gap in the game's numbering.
        template <std::size_t Count>
        QString nameAt(const std::array<std::string_view, Count>& names, int value)
        {
            if (value >= 0 && std::cmp_less(value, Count) && !names.at(static_cast<std::size_t>(value)).empty())
            {
                const std::string_view name = names.at(static_cast<std::size_t>(value));

                return QString::fromUtf8(name.data(), static_cast<qsizetype>(name.size()));
            }

            return QStringLiteral("Unknown (%1)").arg(value);
        }

        constexpr std::array<std::string_view, 6> Weathers = {
            "Clear", "Light cloud", "Overcast", "Light rain", "Heavy rain", "Storm",
        };

        constexpr std::array<std::string_view, 13> SessionTypes = {
            "Unknown",      "Practice 1",   "Practice 2",   "Practice 3",       "Short practice",
            "Qualifying 1", "Qualifying 2", "Qualifying 3", "Short qualifying", "One-shot qualifying",
            "Race",         "Race 2",       "Time trial",
        };

        constexpr std::array<std::string_view, 25> Tracks = {
            "Melbourne",
            "Paul Ricard",
            "Shanghai",
            "Sakhir",
            "Catalunya",
            "Monaco",
            "Montreal",
            "Silverstone",
            "Hockenheim",
            "Hungaroring",
            "Spa-Francorchamps",
            "Monza",
            "Singapore",
            "Suzuka",
            "Abu Dhabi",
            "Texas",
            "Brazil",
            "Austria",
            "Sochi",
            "Mexico",
            "Baku",
            "Sakhir short",
            "Silverstone short",
            "Texas short",
            "Suzuka short",
        };

        constexpr std::array<std::string_view, 2> Eras = {"Modern", "Classic"};

        constexpr std::array<std::string_view, 3> SafetyCarStatuses = {
            "No safety car",
            "Full safety car",
            "Virtual safety car",
        };

        constexpr std::array<std::string_view, 5> Flags = {"None", "Green", "Blue", "Yellow", "Red"};

        constexpr std::array<std::string_view, 3> PitStatuses = {"None", "Pitting", "In the pit area"};

        constexpr std::array<std::string_view, 5> DriverStatuses = {
            "In garage", "Flying lap", "In lap", "Out lap", "On track",
        };

        constexpr std::array<std::string_view, 7> ResultStatuses = {
            "Invalid", "Inactive", "Active", "Finished", "Disqualified", "Not classified", "Retired",
        };

        // The gaps are ids the game does not use.
        constexpr std::array<std::string_view, 72> Drivers = {
            "Carlos Sainz",
            "",
            "Daniel Ricciardo",
            "Fernando Alonso",
            "",
            "",
            "Kimi Räikkönen",
            "Lewis Hamilton",
            "Marcus Ericsson",
            "Max Verstappen",
            "Nico Hülkenberg",
            "Kevin Magnussen",
            "Romain Grosjean",
            "Sebastian Vettel",
            "Sergio Perez",
            "Valtteri Bottas",
            "",
            "Esteban Ocon",
            "Stoffel Vandoorne",
            "Lance Stroll",
            "Arron Barnes",
            "Martin Giles",
            "Alex Murray",
            "Lucas Roth",
            "Igor Correia",
            "Sophie Levasseur",
            "Jonas Schiffer",
            "Alain Forest",
            "Jay Letourneau",
            "Esto Saari",
            "Yasar Atiyeh",
            "Callisto Calabresi",
            "Naota Izum",
            "Howard Clarke",
            "Wilheim Kaufmann",
            "Marie Laursen",
            "Flavio Nieves",
            "Peter Belousov",
            "Klimek Michalski",
            "Santiago Moreno",
            "Benjamin Coppens",
            "Noah Visser",
            "Gert Waldmuller",
            "Julian Quesada",
            "Daniel Jones",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "Charles Leclerc",
            "Pierre Gasly",
            "Brendon Hartley",
            "Sergey Sirotkin",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "Ruben Meijer",
            "Rashid Nair",
            "Jack Tremblay",
        };

        constexpr std::array<std::string_view, 30> Teams = {
            "Mercedes",      "Ferrari",      "Red Bull",      "Williams",      "Force India",  "Renault",
            "Toro Rosso",    "Haas",         "McLaren",       "Sauber",        "McLaren 1988", "McLaren 1991",
            "Williams 1992", "Ferrari 1995", "Williams 1996", "McLaren 1998",  "Ferrari 2002", "Ferrari 2004",
            "Renault 2006",  "Ferrari 2007", "McLaren 2008",  "Red Bull 2010", "Ferrari 1976", "McLaren 1976",
            "Lotus 1972",    "Ferrari 1979", "McLaren 1982",  "Williams 2003", "Brawn 2009",   "Lotus 1978",
        };

        // Starts at 1: 0 is no nationality.
        constexpr std::array<std::string_view, 88> Nationalities = {
            "",
            "American",
            "Argentinean",
            "Australian",
            "Austrian",
            "Azerbaijani",
            "Bahraini",
            "Belgian",
            "Bolivian",
            "Brazilian",
            "British",
            "Bulgarian",
            "Cameroonian",
            "Canadian",
            "Chilean",
            "Chinese",
            "Colombian",
            "Costa Rican",
            "Croatian",
            "Cypriot",
            "Czech",
            "Danish",
            "Dutch",
            "Ecuadorian",
            "Emirian",
            "English",
            "Estonian",
            "Finnish",
            "French",
            "German",
            "Ghanaian",
            "Greek",
            "Guatemalan",
            "Honduran",
            "Hong Konger",
            "Hungarian",
            "Icelander",
            "Indian",
            "Indonesian",
            "Irish",
            "Israeli",
            "Italian",
            "Jamaican",
            "Japanese",
            "Jordanian",
            "Kuwaiti",
            "Latvian",
            "Lebanese",
            "Lithuanian",
            "Luxembourger",
            "Malaysian",
            "Maltese",
            "Mexican",
            "Monegasque",
            "New Zealander",
            "Nicaraguan",
            "North Korean",
            "Northern Irish",
            "Norwegian",
            "Omani",
            "Pakistani",
            "Panamanian",
            "Paraguayan",
            "Peruvian",
            "Polish",
            "Portuguese",
            "Qatari",
            "Romanian",
            "Russian",
            "Salvadoran",
            "Saudi",
            "Scottish",
            "Serbian",
            "Singaporean",
            "Slovakian",
            "Slovenian",
            "South African",
            "South Korean",
            "Spanish",
            "Swedish",
            "Swiss",
            "Taiwanese",
            "Thai",
            "Turkish",
            "Ukrainian",
            "Uruguayan",
            "Venezuelan",
            "Welsh",
        };

        constexpr std::array<std::string_view, 4> FuelMixes = {"Lean", "Standard", "Rich", "Max"};

        constexpr std::array<std::string_view, 9> TyreCompounds = {
            "Hyper soft", "Ultra soft", "Super soft", "Soft", "Medium", "Hard", "Super hard", "Intermediate", "Wet",
        };

        constexpr std::array<std::string_view, 6> ErsDeployModes = {
            "None", "Low", "Medium", "High", "Overtake", "Hotlap",
        };

        struct ButtonName
        {
            Button button;
            const char* name;
        };

        constexpr std::array<ButtonName, 15> ButtonNames = {{
            {.button = Button::CrossOrA, .name = "Cross / A"},
            {.button = Button::TriangleOrY, .name = "Triangle / Y"},
            {.button = Button::CircleOrB, .name = "Circle / B"},
            {.button = Button::SquareOrX, .name = "Square / X"},
            {.button = Button::DPadLeft, .name = "D-pad left"},
            {.button = Button::DPadRight, .name = "D-pad right"},
            {.button = Button::DPadUp, .name = "D-pad up"},
            {.button = Button::DPadDown, .name = "D-pad down"},
            {.button = Button::OptionsOrMenu, .name = "Options / Menu"},
            {.button = Button::L1OrLb, .name = "L1 / LB"},
            {.button = Button::R1OrRb, .name = "R1 / RB"},
            {.button = Button::L2OrLt, .name = "L2 / LT"},
            {.button = Button::R2OrRt, .name = "R2 / RT"},
            {.button = Button::LeftStickClick, .name = "Left stick click"},
            {.button = Button::RightStickClick, .name = "Right stick click"},
        }};
    } // namespace

    QStringList heldButtons(quint32 buttonStatus)
    {
        QStringList held;

        for (const ButtonName& entry : ButtonNames)
        {
            if ((buttonStatus & static_cast<quint32>(entry.button)) != 0)
            {
                held.append(QString::fromLatin1(entry.name));
            }
        }

        return held;
    }

    QString weatherName(int weather)
    {
        return nameAt(Weathers, weather);
    }

    QString sessionTypeName(int sessionType)
    {
        return nameAt(SessionTypes, sessionType);
    }

    QString trackName(int trackId)
    {
        return nameAt(Tracks, trackId);
    }

    QString eraName(int era)
    {
        return nameAt(Eras, era);
    }

    QString safetyCarStatusName(int status)
    {
        return nameAt(SafetyCarStatuses, status);
    }

    QString flagName(int flag)
    {
        // The game's -1 for "unknown" is outside the table, so it shows as "Unknown (-1)".
        return nameAt(Flags, flag);
    }

    QString pitStatusName(int status)
    {
        return nameAt(PitStatuses, status);
    }

    QString driverStatusName(int status)
    {
        return nameAt(DriverStatuses, status);
    }

    QString resultStatusName(int status)
    {
        return nameAt(ResultStatuses, status);
    }

    QString driverName(int driverId)
    {
        return nameAt(Drivers, driverId);
    }

    QString teamName(int teamId)
    {
        return nameAt(Teams, teamId);
    }

    QString nationalityName(int nationality)
    {
        return nameAt(Nationalities, nationality);
    }

    QString fuelMixName(int fuelMix)
    {
        return nameAt(FuelMixes, fuelMix);
    }

    QString tyreCompoundName(int compound)
    {
        return nameAt(TyreCompounds, compound);
    }

    QString ersDeployModeName(int mode)
    {
        return nameAt(ErsDeployModes, mode);
    }
} // namespace F1Data
