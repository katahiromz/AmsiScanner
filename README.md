# AmsiScanner by katahiromz

## What's this?
This library provides C/C++ threat detector for Windows 10 applications by using the AMSI (Antimalware Scan Interface) component of Windows 10.
Windows 10 contains `amsi.dll` DLL file, that provides AMSI.

## Files

File `pamsi.h` is a portable C/C++ AMSI header. File `pamsi.c` is an AMSI API wrapper in C. If you want to use this wrapper, then do `#define WRAP_AMSI`.

Files `pamsixx.hpp` and `pamsixx.cpp` provide a C++ `PAMSIXX` class that manipulates the AMSI API using `amsi.dll` file.

Class `AmsiScanner` of `AmsiScanner.hpp` and `AmsiScanner.cpp` files provides a C++ high-level scanner interface for AMSI.

File `amsiscan.cpp` implements a test program of `AmsiScanner` class.

## NOTICE

The AMSI interface might change in future.
