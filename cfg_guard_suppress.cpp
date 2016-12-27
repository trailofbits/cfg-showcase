#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

#include <cstdio>
#include <cstdlib>
#include <windows.h>

using namespace std;

typedef void (*fnptr)(int);

int main(int argc, const char *argv[]) {

    (void)(argc);
    (void)(argv);

    // load our DLL that has a normal symbol
    // and a CFG suppressed symbol
    HMODULE mod = LoadLibrary("cfg_suppressed_export.dll");
    if(nullptr == mod) {
        printf("Could not load DLL\n");
        exit(-1);
    }

    // GetProcAddress will succeed for both
    // symbols and return valid values
    fnptr suppressed = (fnptr)GetProcAddress(mod, "sensitive_function");
    fnptr normal = (fnptr)GetProcAddress(mod, "normal_function");

    if (nullptr == normal || nullptr == suppressed) {
        printf("Could not get exported function address\n");
        exit(-1);
    }

    // this will call the normal function, which will in turn 
    // call the "suppressed" function to show the suppressed code is reachable
    printf("Calling normal exported symbol...\n");
    normal(0);
    // the suppressed export is not reachable directly when
    // CFG is enabled
    printf("Calling suppressed exported symbol...\n");
    suppressed(0);

    return 0;
}
