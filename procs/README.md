# Procs

To actually patch the game's C++ code, we first need to know where the functions actually are inside
the executable. This is achieved using the _Procs library_, which contains the addresses of all
functions in the game bound to stably-named variables.

The variables use stable intermediate names derived from the game's debug symbols by running the
mangled names through [xxHash128](https://cyan4973.github.io/xxHash/). This way even when the game's
executable is updated, existing code can still continue to compile and run correctly despite the
game having a different executable structure.

Programming using these intermediate names would be impractical, therefore when building ABiT
locally you will get an extra `AByteInTime.Mappings.txt` file containing mappings between the hashed
names and demangled symbols. Note that unlike the headers in this directory, this file cannot be
checked in for copyright reasons.

These headers are checked into the repository so that they can be used by continuous integration.
When building ABiT locally, the build system will default to extracting the symbols from your local
game installation.

Additionally there's `GameVersion.txt`, which is the hash of `HatinTimeGame.exe` at the time of
generating these headers.
