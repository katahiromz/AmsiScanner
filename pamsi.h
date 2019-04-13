/* portable AMSI interface by katahiromz */
#ifndef PAMSI_H_
#define PAMSI_H_    3

#ifndef _INC_WINDOWS
    #include <windows.h>
#endif
#ifndef _BASETYPS_H_
    #include <basetyps.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_AMSI_H
    #include <amsi.h>
#else
    DECLARE_HANDLE(HAMSICONTEXT);
    DECLARE_HANDLE(HAMSISESSION);

    typedef enum AMSI_RESULT
    {
        AMSI_RESULT_CLEAN = 0,
        AMSI_RESULT_NOT_DETECTED = 1,
        AMSI_RESULT_BLOCKED_BY_ADMIN_START = 0x4000,
        AMSI_RESULT_BLOCKED_BY_ADMIN_END = 0x4FFF,
        AMSI_RESULT_DETECTED = 0x8000
    } AMSI_RESULT;

    STDAPI AmsiInitialize(
        LPCWSTR appName,
        HAMSICONTEXT* amsiContext);
    STDAPI_(VOID) AmsiUninitialize(
        HAMSICONTEXT amsiContext);
    STDAPI AmsiOpenSession(
        HAMSICONTEXT amsiContext,
        HAMSISESSION* amsiSession);
    STDAPI_(VOID) AmsiCloseSession(
        HAMSICONTEXT amsiContext,
        HAMSISESSION amsiSession);
    STDAPI AmsiScanString(
        HAMSICONTEXT amsiContext,
        LPCWSTR string,
        LPCWSTR contentName,
        HAMSISESSION amsiSession,
        AMSI_RESULT* result);
    STDAPI AmsiScanBuffer(
        HAMSICONTEXT amsiContext,
        PVOID buffer,
        ULONG length,
        LPCWSTR contentName,
        HAMSISESSION amsiSession,
        AMSI_RESULT* result);

    #define AmsiResultIsMalware(r) \
        ((r) >= AMSI_RESULT_DETECTED)

    #define AmsiResultIsBlockedByAdmin(r) \
        ((r) >= AMSI_RESULT_BLOCKED_BY_ADMIN_START && \
         (r) <= AMSI_RESULT_BLOCKED_BY_ADMIN_END)
#endif  // ndef HAVE_AMSI_H

/* for LoadLibrary */
typedef HRESULT (STDAPICALLTYPE *FN_AmsiInitialize)(
    LPCWSTR appName,
    HAMSICONTEXT* amsiContext);
typedef VOID (STDAPICALLTYPE *FN_AmsiUninitialize)(
    HAMSICONTEXT amsiContext);
typedef HRESULT (STDAPICALLTYPE *FN_AmsiOpenSession)(
    HAMSICONTEXT amsiContext,
    HAMSISESSION* amsiSession);
typedef VOID (STDAPICALLTYPE *FN_AmsiCloseSession)(
    HAMSICONTEXT amsiContext,
    HAMSISESSION amsiSession);
typedef HRESULT (STDAPICALLTYPE *FN_AmsiScanString)(
    HAMSICONTEXT amsiContext,
    LPCWSTR string,
    LPCWSTR contentName,
    HAMSISESSION amsiSession,
    AMSI_RESULT* result);
typedef HRESULT (STDAPICALLTYPE *FN_AmsiScanBuffer)(
    HAMSICONTEXT amsiContext,
    PVOID buffer,
    ULONG length,
    LPCWSTR contentName,
    HAMSISESSION amsiSession,
    AMSI_RESULT* result);

#define AMSI_DLL "amsi.dll"

#ifdef WRAP_AMSI
    HINSTANCE APIENTRY PAMSI_Load(void);
    void APIENTRY PAMSI_Unload(HINSTANCE hinst);

    extern FN_AmsiInitialize g_pAmsiInitialize;
    extern FN_AmsiUninitialize g_pAmsiUninitialize;
    extern FN_AmsiOpenSession g_pAmsiOpenSession;
    extern FN_AmsiCloseSession g_pAmsiCloseSession;
    extern FN_AmsiScanString g_pAmsiScanString;
    extern FN_AmsiScanBuffer g_pAmsiScanBuffer;

    #define AmsiInitialize (*g_pAmsiInitialize)
    #define AmsiUninitialize (*g_pAmsiUninitialize)
    #define AmsiOpenSession (*g_pAmsiOpenSession)
    #define AmsiCloseSession (*g_pAmsiCloseSession)
    #define AmsiScanString (*g_pAmsiScanString)
    #define AmsiScanBuffer (*g_pAmsiScanBuffer)
#endif  // def WRAP_AMSI

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /* ndef PAMSI_H_ */
