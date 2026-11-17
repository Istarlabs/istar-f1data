# istar-f1data

A Qt 6 library that parses what **F1 2018** sends over UDP: the game's telemetry (packet format 2018, port 20777 by
default) - motion, session, lap data, events, participants, car setups, car telemetry with the held buttons, and car
status - into plain structs, with the names of the game's ids in English.

It parses datagrams. It does not open a socket (the application owns that) and it only reads.

## Using it

As a git submodule, added with CMake:

```bash
git submodule add https://github.com/Istarlabs/istar-f1data.git external/istar-f1data
```

```cmake
add_subdirectory(external/istar-f1data)
target_link_libraries(your-app PRIVATE IstarF1Data::F1Data)
```

It needs Qt 6.8 or newer (Core only) and a C++23 compiler. Fedora: `sudo dnf install cmake gcc-c++ qt6-qtbase-devel`.

```cpp
#include "f1data/names.h"
#include "f1data/parser.h"

using namespace F1Data;

// datagram: what a QUdpSocket on port 20777 received
const auto packet = parsePacket(datagram);

if (!packet)
{
    qWarning() << packet.error().message();
    return;
}

if (const auto* telemetry = std::get_if<TelemetryPacket>(&packet->body))
{
    const CarTelemetry& player = telemetry->cars.at(packet->header.playerCarIndex);
    qInfo() << player.speed << "km/h, gear" << player.gear << heldButtons(telemetry->buttonStatus);
}
```

Errors are `ParseError`: a `Kind` to switch over, and `message()` in English. A translated application maps the
kinds to its own texts.

## Building on its own

```bash
cmake -B build && cmake --build build && ctest --test-dir build
```

The tests are built when the library is the top-level project (`ISTAR_F1DATA_TESTS`), not when an application adds
it. The `Scripts/` files are the maintainer's shims into a shared tool checkout; plain CMake is all a user needs.

## The format

The [wiki of this repository](https://github.com/Istarlabs/istar-f1data/wiki) describes every packet byte by byte,
what it was checked against and what is only assumed. **Nothing has been checked against a capture of the real game
yet.**

## Licence

LGPL-3.0-or-later (`LICENSES/LGPL-3.0-or-later.txt`, which builds on `LICENSES/GPL-3.0-or-later.txt`): any application
may use the library, also one under another licence; changes to the library itself stay open.

A sibling of [istar-truckdata](https://github.com/Istarlabs/istar-truckdata); meant for
[istar-wheelstar](https://github.com/Istarlabs/istar-wheelstar).
