/* portable AMSI interface by katahiromz */
#include "pamsi.h"

#ifdef WRAP_AMSI
    FN_AmsiInitialize g_pAmsiInitialize = NULL;
    FN_AmsiUninitialize g_pAmsiUninitialize = NULL;
    FN_AmsiOpenSession g_pAmsiOpenSession = NULL;
    FN_AmsiCloseSession g_pAmsiCloseSession = NULL;
    FN_AmsiScanString g_pAmsiScanString = NULL;
    FN_AmsiScanBuffer g_pAmsiScanBuffer = NULL;

    HINSTANCE APIENTRY PAMSI_Load(void)
    {
        HINSTANCE hinst = LoadLibraryA(AMSI_DLL);
        if (hinst)
        {
    #define PAMSI_GET_PROC(fn) (g_p##fn = (FN_##fn)GetProcAddress(hinst, #fn)) != NULL
            if (PAMSI_GET_PROC(AmsiInitialize) &&
                PAMSI_GET_PROC(AmsiUninitialize) &&
                PAMSI_GET_PROC(AmsiOpenSession) &&
                PAMSI_GET_PROC(AmsiCloseSession) &&
                PAMSI_GET_PROC(AmsiScanString) &&
                PAMSI_GET_PROC(AmsiScanBuffer))
            {
                return hinst;
            }
            FreeLibrary(hinst);
    #undef PAMSI_GET_PROC
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
