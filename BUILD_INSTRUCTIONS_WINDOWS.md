# Windows Build Instructions (Visual Studio 2022)

## 1) Install prerequisites
1. **Visual Studio 2022**
   - Workload: **Desktop development with C++**
   - Include MSVC, Windows 10/11 SDK, CMake tools.
2. **CMake** 3.22 or newer.
3. **Git** for Windows.
4. **JUCE** (clone locally):
   ```powershell
   git clone https://github.com/juce-framework/JUCE C:/dev/JUCE
   ```
5. Optional for ASIO:
   - Download Steinberg ASIO SDK and configure as required by JUCE licensing/setup.

## 2) Clone project
```powershell
git clone <your-repo-url> OreginaldProStudio
cd OreginaldProStudio
```

## 3) Configure project
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR=C:/dev/JUCE
```

## 4) Build
```powershell
cmake --build build --config Release
```

## 5) Run
- Executable path:
  - `build/source/Release/OreginaldProStudio.exe`

## 6) Run tests
```powershell
ctest --test-dir build --output-on-failure -C Release
```

## 7) Common issues
- **JUCE not found:** verify `C:/dev/JUCE/CMakeLists.txt` exists.
- **No audio output:** check Windows output device and exclusive-mode settings.
- **ASIO unavailable:** verify ASIO SDK integration and driver install.


## Required MVP command pair
```powershell
cmake -S . -B build-win -DOPS_BUILD_APP=ON -DJUCE_DIR="C:/JUCE"
cmake --build build-win --config Release
```
