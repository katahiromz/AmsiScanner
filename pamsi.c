/* portable AMSI interface by katahiromz */
#include "pamsi.h"

#ifdef WRAP_AMSI
AMSIINITIALIZE g_pAmsiInitialize = NULL;
AMSIUNINITIALIZE g_pAmsiUninitialize = NULL;
AMSIOPENSESSION g_pAmsiOpenSession = NULL;
AMSICLOSESESSION g_pAmsiCloseSession = NULL;
AMSISCANSTRING g_pAmsiScanString = NULL;
AMSISCANBUFFER g_pAmsiScanBuffer = NULL;

HINSTANCE APIENTRY PAMSI_Load(void)
{
    HINSTANCE hinst = LoadLibraryA(AMSI_DLL);
    if (hinst)
    {
        g_pAmsiInitialize = (AMSIINITIALIZE)GetProcAddress(hinst, "AmsiInitialize");
        g_pAmsiUninitialize = (AMSIUNINITIALIZE)GetProcAddress(hinst, "AmsiUninitialize");
        g_pAmsiOpenSession = (AMSIOPENSESSION)GetProcAddress(hinst, "AmsiOpenSession");
        g_pAmsiCloseSession = (AMSICLOSESESSION)GetProcAddress(hinst, "AmsiCloseSession");
        g_pAmsiScanString = (AMSISCANSTRING)GetProcAddress(hinst, "AmsiScanString");
        g_pAmsiScanBuffer = (AMSISCANBUFFER)GetProcAddress(hinst, "AmsiScanBuffer");
        if (g_pAmsiInitialize &&
            g_pAmsiUninitialize &&
            g_pAmsiOpenSession &&
            g_pAmsiCloseSession &&
            g_pAmsiScanString &&
            g_pAmsiScanBuffer)
        {
            return hinst;
        }
        FreeLibrary(hinst);
    }
    return NULL;
}

void APIENTRY PAMSI_Unload(HINSTANCE hinst)
{
    FreeLibrary(hinst);
    g_pAmsiInitialize = NULL;
    g_pAmsiUninitialize = NULL;
    g_pAmsiOpenSession = NULL;
    g_pAmsiCloseSession = NULL;
    g_pAmsiScanString = NULL;
    g_pAmsiScanBuffer = NULL;
}

#endif  /* def WRAP_AMSI */
