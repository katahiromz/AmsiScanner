// AmsiScanner.hpp
#ifndef AMSI_SCANNER_HPP_
#define AMSI_SCANNER_HPP_ 8

#include "pamsixx.hpp"
#include <cstdlib>

//////////////////////////////////////////////////////////////////////////////

struct AmsiSample
{
    void *m_data;     // malloc'ed
    DWORD m_size;
    WCHAR m_pathname[MAX_PATH];

    AmsiSample();
    ~AmsiSample();
    void init();
    HRESULT load(LPCWSTR filename);
    void free();
};

//////////////////////////////////////////////////////////////////////////////

struct AmsiResult
{
    AMSI_RESULT value;
    BOOL is_malware;
    BOOL is_unknown;

    AmsiResult();
    void init();
    const char *result_string() const;
};

//////////////////////////////////////////////////////////////////////////////

class AmsiScanner : public PAMSIXX
{
public:
    AmsiScanner();  // without loading
    AmsiScanner(LPCWSTR appName);   // with loading
    ~AmsiScanner();

    BOOL Load(LPCWSTR appName);
    BOOL IsLoaded() const;
    void Free();

public:
    HRESULT OpenSession(HAMSISESSION *phSession);
    void CloseSession(HAMSISESSION *phSession);

    HRESULT DoScan(HAMSISESSION hSession, AmsiSample& sample, AmsiResult& result);

protected:
    HAMSICONTEXT m_hContext;
};

//////////////////////////////////////////////////////////////////////////////
// AmsiSample inlines

inline AmsiSample::AmsiSample()
{
    init();
}

inline void AmsiSample::free()
{
    std::free(m_data);
    m_data = NULL;
    m_size = 0;
}

inline AmsiSample::~AmsiSample()
{
    free();
}

inline void AmsiSample::init()
{
    m_data = NULL;
    m_size = 0;
    m_pathname[0] = 0;
}

//////////////////////////////////////////////////////////////////////////////
// AmsiResult inlines

inline AmsiResult::AmsiResult()
{
    init();
}

inline void AmsiResult::init()
{
    value = AMSI_RESULT_NOT_DETECTED;
    is_malware = FALSE;
    is_unknown = TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// AmsiScanner inlines

inline AmsiScanner::AmsiScanner() : m_hContext(NULL)
{
}

inline AmsiScanner::AmsiScanner(LPCWSTR appName) : m_hContext(NULL)
{
    Load(appName);
}

inline BOOL AmsiScanner::IsLoaded() const
{
    return is_amsi_loaded() && m_hContext != NULL;
}

inline AmsiScanner::~AmsiScanner()
{
    Free();
}

inline HRESULT AmsiScanner::OpenSession(HAMSISESSION *phSession)
{
    HRESULT hr = AmsiOpenSession(m_hContext, phSession);
    return hr;
}

inline void AmsiScanner::CloseSession(HAMSISESSION *phSession)
{
    if (phSession && *phSession)
    {
        AmsiCloseSession(m_hContext, *phSession);
        *phSession = NULL;
    }
}

inline void AmsiScanner::Free()
{
    if (m_hContext)
    {
        AmsiUninitialize(m_hContext);
        m_hContext = NULL;
    }

    PAMSIXX::unload_amsi();
}

#endif  // ndef AMSI_SCANNER_HPP_
