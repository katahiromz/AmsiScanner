// AmsiTest.cpp
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.
#include <cstdio>
#include "AmsiScanner.hpp"

extern "C"
int wmain(int argc, wchar_t **wargv)
{
    if (argc <= 1)
    {
        printf("Usage: AmsiTest [files-to-be-virus-checked]\n");
        return 1;
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

    AmsiScanner::Sample sample;
    AmsiScanner::ScanResult result;
    int total_count = 0, not_detected = 0, detected = 0, unknown = 0;
    for (int i = 1; i < argc; ++i, ++total_count)
    {
        hr = E_FAIL;
        if (scanner.LoadSample(&sample, wargv[i]))
        {
            hr = scanner.ScanSample(hSession, &sample, &result);
            if (FAILED(hr))
            {
                printf("ERROR: ScanSample failed\n");
            }
        }

        if (result.IsUnknown)
        {
            printf("[%d] %ls: UNKNOWN\n", total_count + 1, sample.pathname);
            ++unknown;
        }
        else if (result.IsMalware)
        {
            printf("[%d] %ls: MALWARE: %s\n", total_count + 1, sample.pathname,
                   scanner.GetResultString(result.value));
            ++detected;
        }
        else
        {
            printf("[%d] %ls: NOT DETECTED: %s\n", total_count + 1, sample.pathname,
                   scanner.GetResultString(result.value));
            ++not_detected;
        }

        scanner.FreeSample(&sample);
    }

    scanner.CloseSession(&hSession);

    if (detected)
    {
        printf("# DETECTED.\n");
        return 999;
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
