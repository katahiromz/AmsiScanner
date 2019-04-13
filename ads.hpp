// alternative data stream
#ifndef ADS_HPP_
#define ADS_HPP_ 1

#ifndef _INC_WINDOWS
    #include <windows.h>
#endif
#include <string>
#include <vector>

struct ADS_ENTRY
{
    std::wstring name;
    LARGE_INTEGER Size;
};

HRESULT read_ads_entry(HANDLE hFile, std::vector<ADS_ENTRY>& entries, LPVOID *ppContext);
HRESULT get_ads_entries(LPCWSTR filename, std::vector<ADS_ENTRY>& entries);
HANDLE open_ads_file(LPCWSTR filename, const ADS_ENTRY& entry, BOOL bWrite);
HRESULT get_ads_file(LPCWSTR filename, ADS_ENTRY& entry, std::string& data);
HRESULT put_ads_file(LPCWSTR filename, ADS_ENTRY& entry, const std::string& data);

#endif  // ndef ADS_HPP_
