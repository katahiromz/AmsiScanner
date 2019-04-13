// ads_test.cpp
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.coM.
// This file is public domain software.

#include "ads.hpp"
#include <cstdio>

extern "C"
int wmain(int argc, WCHAR **wargv)
{
    HRESULT hr;

    if (argc <= 1)
    {
        std::vector<ADS_ENTRY> entries;
        hr = get_ads_entries(L"ads_test.cpp", entries);

        for (size_t i = 0; i < entries.size(); ++i)
        {
            std::string data;
            hr = get_ads_file(L"ads_test.cpp", entries[i], data);

            printf("%d: %ls: %s\n", int(i), entries[i].name.c_str(), data.c_str());
        }

        return 0;
    }

    if (argc == 2)
    {
        ADS_ENTRY entry;
        entry.name = wargv[1];
        if (entry.name[0] != L':')
        {
            printf("ERROR\n");
            return -1;
        }

        std::string data;
        hr = get_ads_file(L"ads_test.cpp", entry, data);
        printf("%s\n", data.c_str());

        return hr;
    }

    if (argc == 3)
    {
        ADS_ENTRY entry;
        entry.name = wargv[1];
        if (entry.name[0] != L':')
        {
            printf("ERROR\n");
            return -1;
        }

        hr = put_ads_file(L"ads_test.cpp", entry, "TESTTEST");
        return hr;
    }

    return 1;
}
