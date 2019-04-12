/* portable AMSI interface by katahiromz */
#include "pamsi.h"

#define AMSIDLL "amsi.dll"

template <typename T_PROC>
inline bool get_proc(T_PROC& fn, HINSTANCE hInst, const char *name)
{
    fn = reinterpret_cast<T_PROC>(GetProcAddress(hInst, name));
    return fn != NULL;
}

extern "C"
{
    AMSIINITIALIZE g_pAmsiInitialize = NULL;
    AMSIUNINITIALIZE g_pAmsiUninitialize = NULL;
    AMSIOPENSESSION g_pAmsiOpenSession = NULL;
    AMSICLOSESESSION g_pAmsiCloseSession = NULL;
    AMSISCANSTRING g_pAmsiScanString = NULL;
    AMSISCANBUFFER g_pAmsiScanBuffer = NULL;

#define GET_PROC(name) get_proc(g_p##name, hinst, #name)

    HINSTANCE APIENTRY LoadAmsi(void)
    {
        HINSTANCE hinst = LoadLibraryA(AMSIDLL);
        if (hinst)
        {
            if (GET_PROC(AmsiInitialize) &&
                GET_PROC(AmsiUninitialize) &&
                GET_PROC(AmsiOpenSession) &&
                GET_PROC(AmsiCloseSession) &&
                GET_PROC(AmsiScanString) &&
                GET_PROC(AmsiScanBuffer))
            {
                return hinst;
            }

            FreeLibrary(hinst);
        }
        return NULL;
    }

    void APIENTRY UnLoadAmsi(HINSTANCE hinst)
    {
        FreeLibrary(hinst);
        g_pAmsiInitialize = NULL;
        g_pAmsiUninitialize = NULL;
        g_pAmsiOpenSession = NULL;
        g_pAmsiCloseSession = NULL;
        g_pAmsiScanString = NULL;
        g_pAmsiScanBuffer = NULL;
    }
}
