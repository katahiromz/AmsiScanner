# AmsiScanner by katahiromz

This library and program provide threat detector for Windows 10 applications by using the AMSI (Antimalware Scan Interface) component of Windows 10.

File `pamsi.h` is a portable C/C++ AMSI header.

File `pamsi.cpp` provides an AMSI wrapper. If you want to use the wrapper, then do `#define WRAP_AMSI`.

Class `AmsiScanner` provides a high-level scanner interface.

File `AmsiScannerMain.cpp` implements a test program of `AmsiScanner` class.

The AMSI interface might change in future.
