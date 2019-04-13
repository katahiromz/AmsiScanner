#ifndef PAMSIXX_HPP_
#define PAMSIXX_HPP_ 2

#include "pamsi.h"

class PAMSIXX
{
public:
    PAMSIXX();
    virtual ~PAMSIXX();

    bool load_amsi();
    void unload_amsi();
    bool is_amsi_loaded() const;

#ifndef PAMSIXX_STATIC
    HINSTANCE m_hinst;
    AMSIINITIALIZE AmsiInitialize;
    AMSIUNINITIALIZE AmsiUninitialize;
    AMSIOPENSESSION AmsiOpenSession;
    AMSICLOSESESSION AmsiCloseSession;
    AMSISCANSTRING AmsiScanString;
    AMSISCANBUFFER AmsiScanBuffer;
#endif

protected:
    template <typename T_PROC>
    bool get_proc(T_PROC& fn, HINSTANCE hInst, const char *name)
    {
        fn = reinterpret_cast<T_PROC>(GetProcAddress(hInst, name));
        return fn != NULL;
    }
};

//////////////////////////////////////////////////////////////////////////////

inline PAMSIXX::PAMSIXX()
#ifndef PAMSIXX_STATIC
    : m_hinst(NULL)
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
#ifndef PAMSIXX_STATIC
    unload_amsi();
    m_hinst = LoadLibraryA(AMSI_DLL);
    if (m_hinst)
    {
# define PAMSIXX_GETPROC(name) get_proc(name, m_hinst, #name)
        if (PAMSIXX_GETPROC(AmsiInitialize) &&
            PAMSIXX_GETPROC(AmsiUninitialize) &&
            PAMSIXX_GETPROC(AmsiOpenSession) &&
            PAMSIXX_GETPROC(AmsiCloseSession) &&
            PAMSIXX_GETPROC(AmsiScanString) &&
            PAMSIXX_GETPROC(AmsiScanBuffer))
        {
            return true;
        }
# undef PAMSIXX_GETPROC
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
#else
    return true;
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
