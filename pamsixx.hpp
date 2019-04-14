#ifndef PAMSIXX_HPP_
#define PAMSIXX_HPP_ 4

#include "pamsi.h"

class PAMSIXX
{
public:
    PAMSIXX();
    ~PAMSIXX();

    bool load_amsi();
    void unload_amsi();
    bool is_amsi_loaded() const;

#ifndef PAMSIXX_STATIC
    HINSTANCE m_hinst;
    FN_AmsiInitialize AmsiInitialize;
    FN_AmsiUninitialize AmsiUninitialize;
    FN_AmsiOpenSession AmsiOpenSession;
    FN_AmsiCloseSession AmsiCloseSession;
    FN_AmsiScanString AmsiScanString;
    FN_AmsiScanBuffer AmsiScanBuffer;
#endif
};

//////////////////////////////////////////////////////////////////////////////

inline PAMSIXX::PAMSIXX()
#ifndef PAMSIXX_STATIC
  : m_hinst(NULL),
    AmsiInitialize(NULL),
    AmsiUninitialize(NULL),
    AmsiOpenSession(NULL),
    AmsiCloseSession(NULL),
    AmsiScanString(NULL),
    AmsiScanBuffer(NULL)
#endif
{
}

inline PAMSIXX::~PAMSIXX()
{
#ifndef PAMSIXX_STATIC
    ::FreeLibrary(m_hinst);
    m_hinst = NULL;
#endif
}

inline bool PAMSIXX::load_amsi()
{
#ifdef PAMSIXX_STATIC
    return true;
#else
    unload_amsi();
    m_hinst = LoadLibraryA(AMSI_DLL);
    if (m_hinst)
    {
# define PAMSIXX_GET_PROC(fn) \
    (fn = reinterpret_cast<FN_##fn>(GetProcAddress(m_hinst, #fn))) != NULL
        if (PAMSIXX_GET_PROC(AmsiInitialize) &&
            PAMSIXX_GET_PROC(AmsiUninitialize) &&
            PAMSIXX_GET_PROC(AmsiOpenSession) &&
            PAMSIXX_GET_PROC(AmsiCloseSession) &&
            PAMSIXX_GET_PROC(AmsiScanString) &&
            PAMSIXX_GET_PROC(AmsiScanBuffer))
        {
            return true;
        }
# undef PAMSIXX_GET_PROC
        FreeLibrary(m_hinst);
    }
    AmsiInitialize = NULL;
    AmsiUninitialize = NULL;
    AmsiOpenSession = NULL;
    AmsiCloseSession = NULL;
    AmsiScanString = NULL;
    AmsiScanBuffer = NULL;
    m_hinst = NULL;
    return false;
#endif
}

inline void PAMSIXX::unload_amsi()
{
#ifndef PAMSIXX_STATIC
    if (m_hinst)
    {
        FreeLibrary(m_hinst);
        m_hinst = NULL;
    }
    AmsiUninitialize = NULL;
    AmsiOpenSession = NULL;
    AmsiCloseSession = NULL;
    AmsiScanString = NULL;
    AmsiScanBuffer = NULL;
#endif
}

inline bool PAMSIXX::is_amsi_loaded() const
{
#ifndef PAMSIXX_STATIC
    return m_hinst != NULL;
#else
    return true;
#endif
}

#endif  // ndef PAMSIXX_HPP_
