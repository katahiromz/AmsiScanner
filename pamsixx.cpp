/* portable AMSI interface by katahiromz */
#include "pamsixx.hpp"

PAMSIXX::PAMSIXX() : m_hinst(NULL)
{
}

PAMSIXX::~PAMSIXX()
{
    ::FreeLibrary(m_hinst);
    m_hinst = NULL;
}

template <typename T_PROC>
inline bool get_proc(T_PROC& fn, HINSTANCE hInst, const char *name)
{
    fn = reinterpret_cast<T_PROC>(GetProcAddress(hInst, name));
    return fn != NULL;
}

bool PAMSIXX::load_amsi()
{
    unload_amsi();
    m_hinst = LoadLibraryA(AMSI_DLL);
    if (m_hinst)
    {
#define GETPROC(name) get_proc(name, m_hinst, #name)
        if (GETPROC(AmsiInitialize) &&
            GETPROC(AmsiUninitialize) &&
            GETPROC(AmsiOpenSession) &&
            GETPROC(AmsiCloseSession) &&
            GETPROC(AmsiScanString) &&
            GETPROC(AmsiScanBuffer))
        {
            return true;
        }
#undef GETPROC
        FreeLibrary(m_hinst);
    }
    AmsiInitialize = NULL;
    AmsiUninitialize = NULL;
    AmsiOpenSession = NULL;
    AmsiCloseSession = NULL;
    AmsiScanString = NULL;
    AmsiScanBuffer = NULL;
    m_hinst = NULL;
}

void PAMSIXX::unload_amsi()
{
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
}

bool PAMSIXX::is_amsi_loaded() const
{
    return m_hinst != NULL;
}
