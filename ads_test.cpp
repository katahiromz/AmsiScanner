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
            printf("%d: %ls\n", int(i), entries[i].name.c_str());
        }

        return 0;
    }

    if (argc == 2)
    {
        ADS_ENTRY entry;
        entry.name = L"TEST";

        std::string data;
        hr = get_ads_file(L"ads_test.cpp", entry, data);
        printf("%s\n", data.c_str());
        return hr;
    }

    if (argc == 3)
    {
        ADS_ENTRY entry;
        entry.name = wargv[1];
        hr = put_ads_file(L"ads_test.cpp", entry, "TESTTEST");
        return hr;
    }

    return 1;
}
