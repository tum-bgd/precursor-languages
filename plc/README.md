# Compiling on Windows

Manually as follows:

- use win_flex and win_bison from https://github.com/lexxmark/winflexbison/releases
- Decompress them into a subfolder winbison
- Run bootstrap.bat
- Then cmake flow (build directory, configure, build).
- Preferably cmake --build . --config=Release not to depend on debug libs

# Compiling on Linux

make
