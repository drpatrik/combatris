# Combatris - Tetris Battle

![screenshots](screenshots/combatris-demo-1.png)

An modern C++11/14/17 implementation of Tetris Battle using the scoring system as of 2006. The game uses the
BPS Random Generator and Wall Kicks accoring to the Super Rotating system. It is fairly easy to change both
the random generator and wall kicks / alternativly no-wall kicks. At some time I might make the systems plugable.

The multiplayer part is not developed yet, I have one animation more to add and then I cannot procrastrinate the
network part any longer.

**Status**
- [x] Rotate left / right
- [x] Soft / Hard drop
- [x] BPS Random Generator
- [x] Wall kicks (Super Rotating System)
- [X] Hold piece
- [x] T-Spin (scoring)
- [x] T-Spin mini (scoring)
- [x] B2B T-Spin (scoring)
- [x] B2B Tetris (scoring)
- [x] Combos (scoring)
- [X] Perfect clear (cleared matrix after game started)
- [X] Hard Drop & Soft Drop (scoring)
- [X] Panes for score / next tetrominos / hold piece / level and high score
- [X] Pane showing the last clearing move (B2B / Combo / T-Spin / Single / Double /etc.)
- [X] Joystick support (PS3 joystick tested)
- [X] Hot add / remove joystick device
- [ ] Configure which joystick to use (if many available)
- [X] Tetromino Locked (animations)
- [X] Level up (animations)
- [X] Score animation
- [X] Count down before game start (animations)
- [X] Game Over Screen
- [X] Pause Screen
- [X] Splash Screen
- [ ] Sound
- [ ] Network functionality (P2P based on UDP)
- [ ] Own display for Battle game statistics
- [ ] Unit tests // Two in place right now

**Keyboard Commands**

Key | Action
--- | ------
N  | New game
F1 / ESC / P | Pause
A | Rotate counter clockwise
Up / S | Rotate clockwise
Left | Move Left
Right | Move Right
Down | Soft Drop
Space | Hard Drop
Shift | Hold piece
Q | Quit

**Joystick Commands (PS3 Controller)**

Key | Action
--- | ------
Start  | New game
Select | Pause
Square | Rotate counter clockwise
Circle | Rotate clockwise
Left | Move Left
Right | Move Right
Down | Soft Drop
X | Hard Drop
Triangle | Hold piece
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

Builds the project, default is a release build. Ninja is used if available:

```bash
make
```

Starts the game:
```bash
make run
```

Runs the test suit:

```bash
make test
```

Run cppcheck (if installed) on the codebase with all checks turned-on:

```bash
make cppcheck
```

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

* Wall kick tables from https://harddrop.com/wiki/SRS
* Tetris info from http://tetris.wikia.com and https://tetris.wiki
* Tetrominoes from https://commons.wikimedia.org/wiki/File:Tetrominoes_IJLO_STZ_Worlds.svg
* All sfx and music are from www.freesound.org
* Fonts http://www.dafont.com/cabin.font
* Original FindSDL2_mixer https://github.com/rlsosborne/doom
* DeltaTimer adapted from http://headerphile.com/sdl2/sdl2-part-9-no-more-delays/
* https://github.com/philsquared/Catch
