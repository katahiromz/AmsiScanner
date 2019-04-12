// AmsiScanner.cpp
#include "AmsiScanner.hpp"
#include <cstdlib>

AmsiScanner::AmsiScanner() :
#ifdef WRAP_AMSI
    m_hinstAMSI(NULL),
#endif
    m_hContext(NULL)
{
}

AmsiScanner::AmsiScanner(LPCWSTR appName) :
#ifdef WRAP_AMSI
    m_hinstAMSI(NULL),
#endif
    m_hContext(NULL)
{
    Load(appName);
}

BOOL AmsiScanner::Load(LPCWSTR appName)
{
    Free();

#ifdef WRAP_AMSI
    m_hinstAMSI = LoadAmsi();
    if (m_hinstAMSI == NULL)
        return FALSE;
#endif

    HRESULT hr;
    hr = AmsiInitialize(appName, &m_hContext);
    if (FAILED(hr))
    {
        Free();
        return FALSE;
    }
    return TRUE;
}

void AmsiScanner::Free()
{
    if (m_hContext)
    {
        AmsiUninitialize(m_hContext);
        m_hContext = NULL;
    }

#ifdef WRAP_AMSI
    if (m_hinstAMSI)
    {
        UnLoadAmsi(m_hinstAMSI);
        m_hinstAMSI = NULL;
    }
#endif
}

BOOL AmsiScanner::IsLoaded() const
{
#ifdef WRAP_AMSI
    return m_hinstAMSI != NULL && m_hContext != NULL;
#else
    return m_hContext != NULL;
#endif
}

AmsiScanner::~AmsiScanner()
{
    Free();
}

HRESULT AmsiScanner::OpenSession(HAMSISESSION *phSession)
{
    HRESULT hr = AmsiOpenSession(m_hContext, phSession);
    return hr;
}

void AmsiScanner::CloseSession(HAMSISESSION *phSession)
{
    AmsiCloseSession(m_hContext, *phSession);
    *phSession = NULL;
}

void AmsiScanner::FreeSample(Sample *sample)
{
    if (sample->data)
    {
        std::free(sample->data);
        sample->data = NULL;
    }
    sample->size = 0;
}

HRESULT AmsiScanner::ScanSample(HAMSISESSION hSession, const Sample *sample, ScanResult *result)
{
    result->init();

    HRESULT hr;
    hr = AmsiScanBuffer(m_hContext, sample->data, sample->size, sample->pathname,
                        hSession, &result->value);
    if (hr == S_OK)
    {
        result->IsUnknown = FALSE;
        result->IsMalware = AmsiResultIsMalware(result->value);
    }
    return hr;
}

const char *AmsiScanner::GetResultString(AMSI_RESULT result)
{
    switch (result)
    {
    case AMSI_RESULT_CLEAN:
        return "File is clean";
    case AMSI_RESULT_NOT_DETECTED:
        return "No threat detected";
    case AMSI_RESULT_BLOCKED_BY_ADMIN_START:
        return "Threat is blocked by the administrator";
    case AMSI_RESULT_BLOCKED_BY_ADMIN_END:
        return "Threat is blocked by the administrator";
    case AMSI_RESULT_DETECTED:
        return "File is considered malware";
    default:
        return "N/A";
    }
}

BOOL AmsiScanner::LoadSample(Sample *sample, const WCHAR *filename)
{
    ZeroMemory(sample, sizeof(*sample));

    GetFullPathNameW(filename, ARRAYSIZE(sample->pathname), sample->pathname, NULL);

    HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == INVALID_FILE_SIZE)
    {
        CloseHandle(hFile);
        return FALSE;
    }

    BYTE *buffer = NULL;
    if (dwFileSize > 0)
    {
        buffer = (BYTE *)std::malloc(dwFileSize);
        if (!buffer)
        {
            CloseHandle(hFile);
            return FALSE;
        }

        DWORD cbRead;
        if (!ReadFile(hFile, buffer, dwFileSize, &cbRead, NULL) ||
            cbRead != dwFileSize)
        {
            std::free(buffer);
            CloseHandle(hFile);
            return FALSE;
        }
    }

    CloseHandle(hFile);

    sample->data = buffer;
    sample->size = dwFileSize;
    return TRUE;
}
