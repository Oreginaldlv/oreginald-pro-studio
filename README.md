This repository contains an original implementation developed by Oreginald Inc.
# Oreginald Pro Studio

No proprietary source code, assets, or internal documentation from third-party
products were used in the creation of this software.
Oreginald Pro Studio is an original Windows-first desktop DAW MVP built in **C++20 + JUCE + CMake**. It focuses on real local music creation workflows: transport, arrangement/session workspaces, MIDI editing, audio engine bootstrapping, project save/load, browser indexing, and extensible architecture for advanced modules.

## Current Feature Status

### Phase 1 MVP (implemented now)
- JUCE desktop app shell with dark theme and split workspace layout.
- Transport controls: Play, Stop, Record arm, BPM, metronome and loop toggles, timeline readout.
- Arrangement view and Session view tabs with quick switch action.
- Built-in synth voice playback path through JUCE `Synthesiser`.
- Browser panel and local file index/search model.
- Project management service using `ValueTree` + XML persistence.
- Piano-roll note entry (click-to-add), MIDI clip serialization parser.
- Settings panel with current audio device readout.
- Future-module interfaces for stem/AI/live tooling.

### In-progress / planned (see roadmap)
- Full multitrack clip engine, clip drag/resize loop behavior, drum rack, audio recording lanes, effect rack UI, robust mixer metering.

## Windows Build (Visual Studio 2022)

See [BUILD_INSTRUCTIONS_WINDOWS.md](BUILD_INSTRUCTIONS_WINDOWS.md) for full setup.

Quick start:
1. Install Visual Studio 2022 with **Desktop development with C++**.
2. Install CMake 3.22+.
3. Clone JUCE repo.
4. Configure:
   ```powershell
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR=C:/dev/JUCE
   ```
5. Build:
   ```powershell
   cmake --build build --config Release
   ```
6. Run `build/source/Release/OreginaldProStudio.exe`.



## Verified Windows Configure/Build Commands
Use the exact commands below on Windows 10/11:

```powershell
cmake -S . -B build-win -DOPS_BUILD_APP=ON -DJUCE_DIR="C:/JUCE"
cmake --build build-win --config Release
```

## Dependencies
- JUCE 8.x (local clone referenced via `-DJUCE_DIR=...`)
- Visual Studio 2022 toolchain
- CMake 3.22+
- Optional: ASIO SDK (for native ASIO driver support)

## Keyboard Shortcuts (current)
- `Space`: play/stop (planned command mapping)
- `R`: record arm (planned command mapping)
- `Tab`: arrangement/session switch (planned command mapping)

## Known Limitations
- MIDI editor currently supports note creation only (move/delete/quantize are Phase 1.1 tasks).
- Session triggering and arrangement clip manipulation are visual MVP scaffolds.
- Recording engine interface exists; disk writer finalization is pending.
- DSP devices beyond synth voice are architecture-ready but not all are wired in UI.

## Roadmap
See [ROADMAP.md](ROADMAP.md).

## Architecture
See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Screenshots
- Placeholder: add first Windows release screenshots under `docs/screenshots/`.

## Troubleshooting
- If CMake fails with JUCE not found, verify `-DJUCE_DIR` points to JUCE repo root.
- If no audio device appears, open Settings tab and check Windows default device + driver availability.
- If ASIO device missing, ensure ASIO SDK is installed and JUCE ASIO support is enabled.

## Release Build Notes
- Use `-DCMAKE_BUILD_TYPE=Release` on single-config generators, or `--config Release` on Visual Studio.
- Bundle required runtime DLLs with app folder before distribution.