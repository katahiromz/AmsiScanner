// AmsiScanner.cpp
#include "AmsiScanner.hpp"

//////////////////////////////////////////////////////////////////////////////
// AmsiSample

HRESULT AmsiSample::load(LPCWSTR filename)
{
    free();

    WCHAR pathname[MAX_PATH];
    GetFullPathNameW(filename, ARRAYSIZE(pathname), pathname, NULL);
    m_pathname = pathname;

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

    LPBYTE buffer = reinterpret_cast<LPBYTE>(std::malloc(dwFileSize ? dwFileSize : 1));
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

//////////////////////////////////////////////////////////////////////////////
// AmsiResult

const char *AmsiResult::result_string() const
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

//////////////////////////////////////////////////////////////////////////////
// AmsiScanner

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

HRESULT AmsiScanner::DoScan(HAMSISESSION hSession, AmsiSample& sample,
                            AmsiResult& result)
{
    result.init();

    if (sample.m_size == 0)
    {
        result.is_unknown = FALSE;
        result.is_malware = FALSE;
        return S_OK;
    }

    HRESULT hr;
    hr = AmsiScanBuffer(m_hContext, sample.m_data, sample.m_size,
                        sample.m_pathname.c_str(), hSession, &result.value);
    if (hr == S_OK)
    {
        result.is_unknown = FALSE;
        result.is_malware = AmsiResultIsMalware(result.value);
    }
    return hr;
}

HRESULT AmsiScanner::DoScanFile(HAMSISESSION hSession, LPCWSTR filename,
                                AmsiResult& result)
{
    HRESULT hr;

    {
        AmsiSample sample;
        hr = sample.load(filename);
        if (SUCCEEDED(hr))
        {
            hr = DoScan(hSession, sample, result);
            if (result.is_malware)
                return hr;
        }
    }

    std::vector<ADS_ENTRY> entries;
    hr = ADS_get_entries(filename, entries);
    if (FAILED(hr) || entries.empty())
        return S_OK;

    WCHAR pathname[MAX_PATH];
    GetFullPathNameW(filename, ARRAYSIZE(pathname), pathname, NULL);

    BOOL is_malware = result.is_malware;
    BOOL is_unknown = result.is_unknown;
    BOOL is_failed = FALSE;

    for (size_t i = 0; i < entries.size(); ++i)
    {
        //printf("name: %ls\n", entries[i].name.c_str());

        std::string data;
        hr = ADS_get_data(filename, entries[i], data);

        AmsiSample sample;
        if (FAILED(hr) || data.size() > ULONG_MAX)
        {
            is_unknown = is_failed = TRUE;
        }
        else
        {
            sample.m_data = &data[0];
            sample.m_size = (DWORD)data.size();
            sample.m_pathname = pathname;
            sample.m_pathname += entries[i].name;

            hr = DoScan(hSession, sample, result);
            if (FAILED(hr))
            {
                is_unknown = is_failed = TRUE;
            }
        }

        if (result.is_malware)
        {
            is_malware = TRUE;
        }
        if (result.is_unknown)
        {
            is_unknown = TRUE;
        }

        sample.init();
    }

    if (is_malware)
    {
        result.is_malware = TRUE;
        result.is_unknown = FALSE;
        hr = S_OK;
    }
    else
    {
        if (is_unknown)
        {
            result.is_unknown = TRUE;
        }

        if (is_failed && SUCCEEDED(hr))
        {
            hr = E_FAIL;
        }
    }

    return hr;
}
