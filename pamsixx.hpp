#ifndef PAMSIXX_HPP_
#define PAMSIXX_HPP_ 1

#include "pamsi.h"

class PAMSIXX
{
public:
    PAMSIXX();
    virtual ~PAMSIXX();

    bool load_amsi();
    void unload_amsi();
    bool is_amsi_loaded() const;

    HINSTANCE m_hinst;
    AMSIINITIALIZE AmsiInitialize;
    AMSIUNINITIALIZE AmsiUninitialize;
    AMSIOPENSESSION AmsiOpenSession;
    AMSICLOSESESSION AmsiCloseSession;
    AMSISCANSTRING AmsiScanString;
    AMSISCANBUFFER AmsiScanBuffer;
};

#endif  // ndef PAMSIXX_HPP_
