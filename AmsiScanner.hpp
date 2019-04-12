// AmsiScanner.hpp
#ifndef AMSI_SCANNER_HPP_
#define AMSI_SCANNER_HPP_ 4

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
        BOOL IsMalware;
        BOOL IsUnknown;
        SCAN_RESULT()
        {
            init();
        }
        void init()
        {
            value = AMSI_RESULT_NOT_DETECTED;
            IsMalware = FALSE;
            IsUnknown = TRUE;
        }
        const char *result_string() const;
    };

    struct SAMPLE
    {
        void *data;     // malloc'ed
        DWORD size;
        WCHAR pathname[MAX_PATH];
        SAMPLE()
        {
            init();
        }
        void init()
        {
            data = NULL;
            size = 0;
            pathname[0] = 0;
        }
    };

public:
    HRESULT OpenSession(HAMSISESSION *phSession);
    void CloseSession(HAMSISESSION *phSession);

    HRESULT LoadSample(SAMPLE& sample, const WCHAR *filename);
    void FreeSample(SAMPLE& sample);

    HRESULT ScanSample(HAMSISESSION hSession, const SAMPLE& sample, SCAN_RESULT& result);

protected:
    HAMSICONTEXT m_hContext;
};

#endif  // ndef AMSI_SCANNER_HPP_
