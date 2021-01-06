# Azayaka

Azayaka is a free and open-source *Game Boy* and *Game Boy Color* emulator written in C++.

<p align="center">
    <img src="images/Pokemon - Silver Version.png" width=200>
    <img src="images/Super Mario Bros. Deluxe.png" width=200>
    <img src="images/Castlevania II - Belmont's Revenge.png" width=200>
    <img src="images/Donkey Kong Land.png" width=200>
</p>

<p align="center">
    <img src="images/Legend of Zelda, The - Link's Awakening DX.png" width=200>
    <img src="images/Pokemon - Red Version.png" width=200>
    <img src="images/Final Fantasy Adventure.png" width=200>
    <img src="images/Wario Land 3.png" width=200>
</p>

<p align="center">
    <img src="images/Super Mario Land.png" width=200>
    <img src="images/Tetris DX.png" width=200>
    <img src="images/Donkey Kong.png" width=200>
    <img src="images/Kirby's Dream Land.png" width=200>
</p>

## Features
- **Cycle-Accurate** emulation.
- [Console based **Debugger**](doc/Debugger.md).
- **Audio** emulation.
- Real-Time Clock support.
- Screen shots.
- Audio recording.
- Save states.
- **Rewind**.
- [Disassembler](doc/Disassembler.md)(With Code/Data Log).
- [Automated testing system](doc/Tester.md).
- Emulates both the Game Boy(DMG) and Game Boy Color(CGB).
- Game Boy Printer.

## Accuracy
- Passes all of blargg's tests except oam_bug.
- Passes most of gekkio's Mooneye GB tests.

[See test results](doc/TestResults.md)

## Cartridge Mappers

Azayaka supports the following cartridge-mappers:

- MBC1
- MBC1M
- MBC2
- MBC3
- MBC3+RTC
- MBC5

## Supported Platforms
- Linux
- Mac OS
- Windows (**Coming soon!**)

## Controls

### Emulated Buttons

Note: You can map different keys to the buttons by editing Azayaka's INI file.

| Button | Key |
| --- | --- |
| A | <kbd>Z</kbd> |
| B | <kbd>X</kbd> |
| Start |<kbd>Enter</kbd>
| Select | <kbd>Space</kbd> |
| Up | <kbd>↑</kbd> |
| Down | <kbd>↓</kbd> |
| Left | <kbd>←</kbd> |
| Right | <kbd>→</kbd> |


### Key Commands

| Function | Key |
| --- | --- |
| Turbo | <kbd>Shift</kbd> |
| Slow-Motion | <kbd>Alt</kbd> |
| Rewind | <kbd>Backspace</kbd> |
| Toggle Fullscreen | <kbd>Ctrl+F</kbd> |
| Pause/Unpause | <kbd>Ctrl+P</kbd> |
| Reset | <kbd>Ctrl+R</kbd> |
| Trigger Debugger | <kbd>Ctrl+C</kbd> |
| Increase Volume | <kbd>Plus</kbd> |
| Decrease Volume | <kbd>Minus</kbd> |
| Load Savestate | <kbd>F1-F10</kbd> |
| Save Savestate | <kbd>Ctrl + F1-F10</kbd> |
| Take Screenshot | <kbd>F11</kbd> |
| Start/Stop recording Audio | <kbd>F12</kbd> |

## Building

Requires [CMake](https://cmake.org)

### Dependencies
- SDL2: For the frontend.
- libpng: For **PNG** screenshots. If you don't have it, **BMPs** will be used instead.
- OpenGL: For hardware accelerated graphics. If you don't have it, SDL will be used for the rendering.

### *Nix Building

```
mkdir build
cd build
cmake ..
make
```

### Windows Building
#### Coming Soon!

Sorry, but I don't currently have a Windows Dev machine, but I should have a setup in just a couple of weeks. But in the mean time, I can't guaranty that Azayaka will compile on a Windows Box. My apologies.

## Running

### Running Azayaka-sdl

```
./Azayaka-SDL [ROM or Directory Path] [Options...]

  -d, --debug        Run the emulator in debug mode
  -s, --scale        Scales the screen by the factor given
  -g, --force-gb     Force the emulator into GameBoy mode
  -c, --force-gbc    Force the emulator into GameBoy-Color mode
  -p, --printer      Enable GameBoy printer
  -D, --dump-usage   Saves the ROM usage(CDL) to a file
      --verbose      Enable verbose debugging information
      --force-sdl    Force the emulator to use SDL as the Video-Driver
```

### Running Azayaka-tester

```
./Azayaka-tester [Test ROMs path] [Correct-Results CSV Path]

```

## Thanks

- Shay Green for blargg's tests.
- Joonas Javanainen for Mooneye GB tests.
- Antonio Niño Díaz for TCAGBD.
- Ben Smith(binji) for his article on implementing rewind.
- To all of the authors of the emulators that I used for inspiration.
- To you, for looking at my emulator!

## Copyright

Azayaka is licensed under GPL3.
Copyright © 2020-2021 Zach Collins

"Game Boy" and "Game Boy Color" are trademarks of Nintendo. Azayaka is not affiliated with Nintendo in any way.
