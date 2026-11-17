# Claude Code Instructions

istar-f1data reads what **F1 2018** sends: the UDP telemetry the game broadcasts (Codemasters' packet format 2018,
port 20777 by default). It is a Qt 6 Core library under LGPL-3.0-or-later; applications (istar-wheelstar first) add it
as a git submodule with `add_subdirectory()`. It parses datagrams - it does not open a socket, that stays in the
application - and it only reads.

It is the sibling of istar-truckdata: that one reads what the truck games keep on disk, this one what F1 sends. The
first goal is to get out everything the format holds; what an application makes of it is decided afterwards.

## Shared guidelines

Imported from the shared devscripts checkout, so a rule is changed there once and reaches every
project. Claude Code asks once per project whether these files outside the repository may be loaded.

@~/dev/tools/devscripts/claude/base.md
@~/dev/tools/devscripts/claude/cpp.md

## This project

- **Qt Core only.** No Gui, no Network, no KDE Frameworks, no i18n: the library's texts are English (`parseError`
  messages, the `*Name()` functions), and an application translates by switching over the enum.
- **Public API = everything under `src/f1data/`**, included as `"f1data/<file>.h"`. A rename is a breaking change.
- **Little-endian, packed, no padding** - what the game sends. Every packet type is checked against its documented size
  (the format page, see below) before it is read; a datagram of another size is an error, never a partial read.
- **Enum numbers are the game's**, never renumbered. An id the library has no name for is kept as the number.
- **The format is documented in this repository's GitHub wiki, not in the tree**
  (https://github.com/Istarlabs/istar-f1data/wiki, a separate git repository: `istar-f1data.wiki.git`), byte by byte.
  A field added to a struct is documented there in the same change, with what it was checked against (a capture, the
  spec, the reference parser) - and what is only assumed says so. The repository is public: nothing private goes into
  the code, the docs or the wiki (no paths of this machine, no homelab names).
- Nothing application-specific: no sockets, no statistics, no deduplication.
- clang-tidy is clean (`python3 Scripts/cpp.py tidy`); a `NOLINT` names its check and says why.
