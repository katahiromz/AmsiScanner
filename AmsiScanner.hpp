// AmsiScanner.hpp
#ifndef AMSI_SCANNER_HPP_
#define AMSI_SCANNER_HPP_ 6

#include "pamsixx.hpp"
#include <cstdlib>

class AmsiScanner : public PAMSIXX
{
public:
    AmsiScanner();  // without loading
    AmsiScanner(LPCWSTR appName);   // with loading
    virtual ~AmsiScanner();

    BOOL Load(LPCWSTR appName);
    BOOL IsLoaded() const;
    void Free();

public:
    struct SCAN_RESULT
    {
        AMSI_RESULT value;
        BOOL is_malware;
        BOOL is_unknown;

        SCAN_RESULT();
        void init();
        const char *result_string() const;
    };

    struct SAMPLE
    {
        void *m_data;     // malloc'ed
        DWORD m_size;
        WCHAR m_pathname[MAX_PATH];

        SAMPLE();
        ~SAMPLE();
        void init();
        HRESULT load(const WCHAR *filename);
    };

public:
    HRESULT OpenSession(HAMSISESSION *phSession);
    void CloseSession(HAMSISESSION *phSession);

    HRESULT DoScan(HAMSISESSION hSession, SAMPLE& sample, SCAN_RESULT& result);

protected:
    HAMSICONTEXT m_hContext;
};

//////////////////////////////////////////////////////////////////////////////

inline AmsiScanner::AmsiScanner() : m_hContext(NULL)
{
}

inline AmsiScanner::AmsiScanner(LPCWSTR appName) : m_hContext(NULL)
{
    Load(appName);
}

inline AmsiScanner::SCAN_RESULT::SCAN_RESULT()
{
    init();
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

inline void AmsiScanner::SCAN_RESULT::init()
{
    value = AMSI_RESULT_NOT_DETECTED;
    is_malware = FALSE;
    is_unknown = TRUE;
}

inline AmsiScanner::SAMPLE::SAMPLE()
{
    init();
}

inline AmsiScanner::SAMPLE::~SAMPLE()
{
    std::free(m_data);
    m_data = NULL;
    m_size = 0;
}

inline void AmsiScanner::SAMPLE::init()
{
    m_data = NULL;
    m_size = 0;
    m_pathname[0] = 0;
}

#endif  // ndef AMSI_SCANNER_HPP_
