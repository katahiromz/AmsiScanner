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
        fprintf(stderr, "ERROR: unable to initialize the AMSI module.\n");
        return 2;
    }

    HAMSISESSION hSession;
    HRESULT hr = scanner.OpenSession(&hSession);
    if (FAILED(hr))
    {
        fprintf(stderr, "ERROR: unable to open AMSI session.\n");
        return 3;
    }

    int total_count = 0, not_detected = 0, detected = 0, unknown = 0;
    for (int i = 1; i < argc; ++i, ++total_count)
    {
        if (wargv[i][0] == L'-' || wargv[i][0] == L'/')
        {
            fprintf(stderr, "ERROR: invalid argument '%ls'.\n", wargv[i]);
            hr = E_INVALIDARG;
            break;
        }
        if (GetFileAttributesW(wargv[i]) == 0xFFFFFFFF)
        {
            fprintf(stderr, "ERROR: file not found '%ls'.\n", wargv[i]);
            hr = E_INVALIDARG;
            break;
        }

        AmsiScanner::SCAN_RESULT result;
        AmsiScanner::SAMPLE sample;

        hr = sample.load(wargv[i]);
        if (SUCCEEDED(hr))
        {
            hr = scanner.ScanSample(hSession, sample, result);
        }
        if (FAILED(hr))
        {
            fprintf(stderr, "ERROR: scan failed.\n");
            break;
        }

        if (result.IsUnknown)
        {
            fprintf(stderr,
                    "[%d] %ls: UNKNOWN: %s\n", total_count + 1, wargv[i],
                    result.result_string());
            ++unknown;
        }
        else if (result.IsMalware)
        {
            fprintf(stderr,
                    "[%d] %ls: MALWARE: %s\n", total_count + 1, wargv[i],
                    result.result_string());
            ++detected;
        }
        else
        {
            fprintf(stderr,
                    "[%d] %ls: NOT DETECTED: %s\n", total_count + 1, wargv[i],
                    result.result_string());
            ++not_detected;
        }
    }

    scanner.CloseSession(&hSession);

    if (FAILED(hr))
    {
        return 3;
    }
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
