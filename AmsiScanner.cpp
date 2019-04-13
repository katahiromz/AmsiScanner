// AmsiScanner.cpp
#include "AmsiScanner.hpp"

BOOL AmsiScanner::Load(LPCWSTR appName)
{
    Free();

    if (!PAMSIXX::load_amsi())
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

HRESULT AmsiScanner::DoScan(HAMSISESSION hSession, SAMPLE& sample,
                            SCAN_RESULT& result)
{
    result.init();

    if (sample.m_size == 0)
    {
        result.is_unknown = FALSE;
        result.is_malware = FALSE;
        return S_OK;
    }

    HRESULT hr;
    hr = AmsiScanBuffer(m_hContext, sample.m_data, sample.m_size, sample.m_pathname,
                        hSession, &result.value);
    if (hr == S_OK)
    {
        result.is_unknown = FALSE;
        result.is_malware = AmsiResultIsMalware(result.value);
    }
    return hr;
}

const char *AmsiScanner::SCAN_RESULT::result_string() const
{
    if (is_unknown)
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

HRESULT AmsiScanner::SAMPLE::load(const WCHAR *filename)
{
    free();

    GetFullPathNameW(filename, ARRAYSIZE(m_pathname), m_pathname, NULL);

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

    BYTE *buffer = (BYTE *)std::malloc(dwFileSize ? dwFileSize : 1);
    if (!buffer)
    {
        ::CloseHandle(hFile);
        return E_OUTOFMEMORY;
    }

    if (dwFileSize > 0)
    {
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

    m_data = buffer;
    m_size = dwFileSize;
    return S_OK;
}
