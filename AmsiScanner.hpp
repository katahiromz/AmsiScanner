// AmsiScanner.hpp
#ifndef AMSI_SCANNER_HPP_
#define AMSI_SCANNER_HPP_ 10

#include "pamsixx.hpp"
#include "ads.hpp"
#include <cstdlib>

//////////////////////////////////////////////////////////////////////////////

struct AmsiSample
{
    void *m_data;     // malloc'ed
    DWORD m_size;
    std::wstring m_pathname;

    AmsiSample();
    ~AmsiSample();
    void init();
    HRESULT load(LPCWSTR filename);
    void free();

private:
    AmsiSample(const AmsiSample&);
    AmsiSample& operator=(const AmsiSample&);
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

    HRESULT OpenSession(HAMSISESSION *phSession);
    void CloseSession(HAMSISESSION *phSession);

    HRESULT DoScan(HAMSISESSION hSession, AmsiSample& sample, AmsiResult& result);
    HRESULT DoScanFile(HAMSISESSION hSession, LPCWSTR filename, AmsiResult& result);

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
    m_pathname.clear();
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
    if (IsLoaded())
    {
        HRESULT hr = AmsiOpenSession(m_hContext, phSession);
        return hr;
    }
    return E_FAIL;
}

inline void AmsiScanner::CloseSession(HAMSISESSION *phSession)
{
    if (phSession && *phSession)
    {
        if (IsLoaded())
            AmsiCloseSession(m_hContext, *phSession);
        *phSession = NULL;
    }
}

inline void AmsiScanner::Free()
{
    if (m_hContext)
    {
        if (IsLoaded())
            AmsiUninitialize(m_hContext);
        m_hContext = NULL;
    }

    PAMSIXX::unload_amsi();
}

#endif  // ndef AMSI_SCANNER_HPP_
