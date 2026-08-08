# dumbLoader

## **Lightning fast, blazingly simple.**

A Geometry Dash mod loader that does one thing and one thing only.

### How to use

1. Build the project (or grab a release)
2. Put `XInput1_4.dll` next to `GeometryDash.exe`
3. Create a folder named `mods` in the same directory
4. Drop your mod `.dll` files into the `mods` folder
5. Launch the game

### Optional

- Put a file named `disable` (or `disable.txt`) inside the `mods` folder → safe mode, skips loading all mods
- Mods are loaded in alphabetical order. Prefix filenames with `01_`, `02_`, etc. if you need a specific order
- Errors are written to `dumbLoader.log` (with timestamps) next to the DLL. On success the log stays empty

### Building

```bash
git clone https://github.com/quickkernel/dumbLoader.git
cd dumbLoader
mkdir build
cd build
cmake .. -A x64
cmake --build . --config Release
```

### Notes
- x64 only (current Geometry Dash)
- Controllers still work
- No menu, no config, no updates, no browser, no ideology
- If the mods folder doesn’t exist, it just acts as a pure XInput proxy

Do whatever you want with it.
