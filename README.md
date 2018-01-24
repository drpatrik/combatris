# Combatris

![screenshots](screenshots/combatris-demo-1.png)

A new old take on the Tetris game where network concepts are introduced.

This is very much work in progress and right now focus is on getting the game
elements to work - after that work on the network capabilities will start.

**Status**
- [x] Game is fully playable
- [x] Status such as score, level, next piece, etc
- [x] Rotate left / right
- [x] Soft / Hard drop
- [x] BPS Random Generator
- [x] Wall kicks
- [ ] Hold piece
- [ ] Panes for score / next etc.
- [ ] Unit tests
- [ ] Sound
- [ ] Customizible controls
- [ ] Controller / mouse
- [ ] Cleared lines animations
- [ ] Tetromino Locked animations
- [ ] Level up animations
- [ ] Count down before game start animations
- [ ] Next Tetromino Queue animations
- [ ] Game Over animations

**Keyboard Command**
Key           | Action
------------- | ------
N            | New game
Arrow Up / S | Rotate Counter clockwise
A            | Rotate Counter clockwise
Arrow Left   | Move Left
Arrow Right  | Move Right
Arrow Down   | Soft Drop
Space        | Hard Drop
Shift        | Hold piece (not yet implemented)

## Build Combatris

**Dependencies:**
* C++17 compliant compiler (e.g. clang 4, clang-9.0.0, Visual Studio 2017.3 [P2])
* C++14 compliant STL implementation
* cmake 2.8.8 or later
* git
* SDL2 (x64 only)
* SDL2_ttf (x64 only)
* SDL2_mixer (x64 only)
* Network support when implemented will require boost

The test suit requires catch - Catch will be automatically downloaded and installed
as part of the build process

**OSX / Linux**

Install the required libraries with (linux)brew or any other package manager.

make

will build the project, default is a release build. Ninja is used if available

make test # will run the test suit.

make run # will start the game

**64-bit Windows 10**

Set the following environment variables (see System Properties/Environment Variables...):

SDL2DIR
SDL2MIXER
SDL2TTFDIR

*Example:*
SDL2DIR C:\SDL2-2.0.5

The PATH should include all three libraries lib\x64 directories

*Example:*
PATH C:\SDL2-2.0.5\lib\x64;C:\SDL2_mixer\lib\x64;C:\SDL2_ttf-2.0.14\lib\x64

Generate Visual Studio project files with CMakeSetup.exe

## Credits

        Wall kick tables from https://harddrop.com/wiki/SRS
        Tetrominoes from https://commons.wikimedia.org/wiki/File:Tetrominoes_IJLO_STZ_Worlds.svg
        All sfx and music are from www.freesound.org
        Fonts http://www.dafont.com/cabin.font
        FindSDL2_mixer https://github.com/rlsosborne/doom
        DeltaTimer adapted from http://headerphile.com/sdl2/sdl2-part-9-no-more-delays/
        https://github.com/philsquared/Catch
