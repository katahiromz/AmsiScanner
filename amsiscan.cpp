// amsiscan.cpp
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.
#include <cstdio>
#include "AmsiScanner.hpp"

void show_help(void)
{
    printf("Usage: amsiscan [files-to-be-virus-checked]\n");
}

void show_version(void)
{
    printf("amsiscan 1.1 by katahiromz\n");
}

extern "C"
int wmain(int argc, wchar_t **wargv)
{
    if (argc <= 1 || lstrcmpiW(wargv[1], L"--help") == 0)
    {
        show_help();
        return 0;
    }

    if (lstrcmpiW(wargv[1], L"--version") == 0)
    {
        show_version();
        return 0;
    }

    AmsiScanner scanner(L"katahiromz's AmsiScanner");

    if (!scanner.IsLoaded())
    {
        printf("ERROR: Unable to initialize the AMSI module.\n");
        return 2;
    }

    HAMSISESSION hSession;
    HRESULT hr = scanner.OpenSession(&hSession);
    if (FAILED(hr))
    {
        printf("ERROR: Unable to open AMSI session.\n");
        return 3;
    }

    AmsiScanner::SAMPLE sample;
    AmsiScanner::SCAN_RESULT result;
    int total_count = 0, not_detected = 0, detected = 0, unknown = 0;
    for (int i = 1; i < argc; ++i, ++total_count)
    {
        result.init();

        hr = scanner.LoadSample(sample, wargv[i]);
        if (SUCCEEDED(hr))
        {
            hr = scanner.ScanSample(hSession, sample, result);
        }
        if (FAILED(hr))
        {
            printf("ERROR: ScanSample failed\n");
        }

        if (result.IsUnknown)
        {
            printf("[%d] %ls: UNKNOWN: %s\n", total_count + 1, sample.pathname,
                   result.result_string());
            ++unknown;
        }
        else if (result.IsMalware)
        {
            printf("[%d] %ls: MALWARE: %s\n", total_count + 1, sample.pathname,
                   result.result_string());
            ++detected;
        }
        else
        {
            printf("[%d] %ls: NOT DETECTED: %s\n", total_count + 1, sample.pathname,
                   result.result_string());
            ++not_detected;
        }

        scanner.FreeSample(sample);
    }

    scanner.CloseSession(&hSession);

    if (detected)
    {
        printf("# DETECTED.\n");
        return 2;
    }
    if (total_count == not_detected)
    {
        printf("# PASSED.\n");
        return 0;
    }
    else
    {
        printf("# UNKNOWN.\n");
        return 1;
    }
}

int main(int argc, char **argv)
{
    int argc0;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc0);
    int ret = wmain(argc0, wargv);
    LocalFree(wargv);
    return ret;
}
