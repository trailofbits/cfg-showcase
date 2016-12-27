#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

#include <cstdio>
#include <cstdlib>
#include <windows.h>

using namespace std;

// XXX: This DLL *must* be built in Release mode for 
// __declspec(guard(suppress)) to suppress exports.
// The compiler directive has no effect in Debug mode.

// Use CFG to prevent this export from
// being called indirectly, via __declspec(guard(suppress))
extern "C" __declspec(dllexport) __declspec(guard(suppress)) void sensitive_function(int arg) {
    printf("In %s; which should not be callable via pointer from GetProcAddress\n", __FUNCTION__);
    printf("But can be called directly\n");
    printf("\targ value is: %d\n", arg);
}

// have a normal, callable export that will 
// call the "senstive" code to show the sensitive function is callable
extern "C" __declspec(dllexport) void normal_function(int arg) {
    printf("In %s; going to call sensitive function\n", __FUNCTION__);
    sensitive_function(arg);
}

// standard DLL boilterplate
BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) 
{
  UNREFERENCED_PARAMETER(hinstDLL);
  UNREFERENCED_PARAMETER(fdwReason);
  UNREFERENCED_PARAMETER(lpvReserved);

  return TRUE;
}
