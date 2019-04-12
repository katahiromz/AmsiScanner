// AmsiScanner.cpp
#include "AmsiScanner.hpp"

AmsiScanner::AmsiScanner() : m_hContext(NULL)
{
}

AmsiScanner::AmsiScanner(LPCWSTR appName) : m_hContext(NULL)
{
    Load(appName);
}

BOOL AmsiScanner::Load(LPCWSTR appName)
{
    Free();

    if (!PAMSIXX::load())
        return FALSE;

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

    PAMSIXX::unload();
}

BOOL AmsiScanner::IsLoaded() const
{
    return m_hinst != NULL && m_hContext != NULL;
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
    if (phSession && *phSession)
    {
        AmsiCloseSession(m_hContext, *phSession);
        *phSession = NULL;
    }
}

void AmsiScanner::FreeSample(SAMPLE& sample)
{
    if (sample.data)
    {
        std::free(sample.data);
        sample.data = NULL;
    }
    sample.size = 0;
}

HRESULT AmsiScanner::ScanSample(HAMSISESSION hSession, const SAMPLE& sample,
                                SCAN_RESULT& result)
{
    result.init();

    HRESULT hr;
    hr = AmsiScanBuffer(m_hContext, sample.data, sample.size, sample.pathname,
                        hSession, &result.value);
    if (hr == S_OK)
    {
        result.IsUnknown = FALSE;
        result.IsMalware = AmsiResultIsMalware(result.value);
    }
    return hr;
}

const char *AmsiScanner::SCAN_RESULT::result_string() const
{
    if (IsUnknown)
        return "Unknown";

    switch (value)
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

HRESULT AmsiScanner::LoadSample(SAMPLE& sample, const WCHAR *filename)
{
    sample.init();

    GetFullPathNameW(filename, ARRAYSIZE(sample.pathname), sample.pathname, NULL);

    HANDLE hFile = ::CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
                                 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return E_FAIL;
    }

    DWORD dwFileSize = ::GetFileSize(hFile, NULL);
    if (dwFileSize == INVALID_FILE_SIZE)
    {
        ::CloseHandle(hFile);
        return E_FAIL;
    }

    BYTE *buffer = NULL;
    if (dwFileSize > 0)
    {
        buffer = (BYTE *)std::malloc(dwFileSize);
        if (!buffer)
        {
            ::CloseHandle(hFile);
            return E_OUTOFMEMORY;
        }

        DWORD cbRead;
        if (!::ReadFile(hFile, buffer, dwFileSize, &cbRead, NULL) ||
            cbRead != dwFileSize)
        {
            std::free(buffer);
            ::CloseHandle(hFile);
            return E_FAIL;
        }
    }

    ::CloseHandle(hFile);

    sample.data = buffer;
    sample.size = dwFileSize;
    return S_OK;
}
