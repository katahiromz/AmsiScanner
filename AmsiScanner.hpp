// AmsiScanner.hpp
#ifndef AMSI_SCANNER_HPP_
#define AMSI_SCANNER_HPP_ 1

#include "pamsi.h"

class AmsiScanner
{
public:
    AmsiScanner();
    AmsiScanner(LPCWSTR appName);
    ~AmsiScanner();

    BOOL Load(LPCWSTR appName);
    BOOL IsLoaded() const;
    void Free();

public:
    struct ScanResult
    {
        AMSI_RESULT value;
        BOOL IsMalware;
        BOOL IsUnknown;
        ScanResult()
        {
            value = AMSI_RESULT_NOT_DETECTED;
            IsMalware = FALSE;
            IsUnknown = TRUE;
        }
    };

    struct Sample
    {
        void *data;     // malloc'ed
        DWORD size;
        WCHAR pathname[MAX_PATH];
    };

public:
    HRESULT OpenSession(HAMSISESSION *phSession);
    void CloseSession(HAMSISESSION *phSession);

public:
    HRESULT ScanSample(HAMSISESSION hSession, const Sample *sample, ScanResult *result);

    BOOL LoadSample(Sample *sample, const WCHAR *filename);
    void FreeSample(Sample *sample);

    const char *GetResultString(AMSI_RESULT result);

protected:
    #ifdef WRAP_AMSI
        HINSTANCE m_hinstAMSI;
    #endif
    HAMSICONTEXT m_hContext;
};

#endif  // ndef AMSI_SCANNER_HPP_
