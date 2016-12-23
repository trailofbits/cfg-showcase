#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <intrin.h>

#include <windows.h>
#include <psapi.h>

// To set a target as invalid, we just need to ensure
// the CFG_CALL_TARGET_VALID bit is not defined, which this will
// do as it clears all flags bits
#define CFG_CALL_TARGET_INVALID (0x00000000)

using namespace std;

typedef int (*int_arg_fn)(int);
typedef int (*float_arg_fn)(float);

static int int_arg(int arg) {
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}

static int float_arg(float arg) {
    printf("CFG should protect transfer to here\n");
    printf("In %s: (%f)\n", __FUNCTION__, (double)arg);
    return 0;
}

static int bad_int_arg(int arg) {
    printf("CFG will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}

static int not_entry_point(int arg) {
    // nop sled for x86 / x86-64
    // these instructions act as a buffer
    // for an indirect control flow transfer to skip
    // a valid function entry point, but continue
    // to execute normal code
    
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 

    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 


    printf("CFG ensures control flow only transfers to potentially valid destinations\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    // need to exit or the program will segfault anyway,
    // since the indirect call skipped the function preamble
    exit(arg);
}

struct foo {
    int_arg_fn int_funcs[1];
    int_arg_fn bad_int_funcs[1];
    float_arg_fn float_funcs[1];  
    int_arg_fn not_entries[1];
};

// the struct aligns the function pointer arrays
// so indexing past the end will reliably
// call working function pointers
static struct foo f = { {int_arg}, {bad_int_arg}, {float_arg}, {(int_arg_fn)((uintptr_t)(not_entry_point)+0x20)} };

static const ULONG kCallTargets = 3;

int main(int argc, const char *argv[]) {

    MODULEINFO mi;
    CFG_CALL_TARGET_INFO call_dests[kCallTargets];

    // Get the base address and size of the currently loaded module
    // so that we can figure out where each function entry point is
    // in relation to the start of the allocation of this process image
    if(0 == GetModuleInformation(GetCurrentProcess(), 
          GetModuleHandle(nullptr),
          &mi,
          sizeof(mi))) {
      printf("Could not get the base address for this module\n");
      exit(-1);
    }

    printf("Base of loaded image: %p\n", mi.lpBaseOfDll);
    printf("Size of loaded image: %x\n", mi.SizeOfImage);

    // remove float_arg from the valid CFG mapping
    call_dests[0].Flags = CFG_CALL_TARGET_INVALID;
    call_dests[0].Offset = (ULONG_PTR)(float_arg) - (ULONG_PTR)(mi.lpBaseOfDll);

    // remove bad_int_arg from the valid CFG mapping
    call_dests[1].Flags = CFG_CALL_TARGET_INVALID;
    call_dests[1].Offset = (ULONG_PTR)(bad_int_arg) - (ULONG_PTR)(mi.lpBaseOfDll);

    // ensure the int_arg function is in the valid CFG mapping
    call_dests[2].Flags = CFG_CALL_TARGET_VALID;
    call_dests[2].Offset = (ULONG_PTR)(int_arg) - (ULONG_PTR)(mi.lpBaseOfDll);


    // update valid CFG targets
    if(TRUE != SetProcessValidCallTargets(
        GetCurrentProcess(),
        mi.lpBaseOfDll,
        mi.SizeOfImage,
        kCallTargets,
        call_dests)) {
      printf("WARNING: Failed on SetProcessValidCallTargets: %08x\n", GetLastError());
    }

    if(argc != 2) {
        printf("Usage: %s <option>\n", argv[0]);
        printf("Option values:\n");
        printf("\t0\tCall correct function\n");
        printf("\t1\tCall the wrong function but with the same signature\n");
        printf("\t2\tCall a float function with an int function signature\n");
        printf("\t3\tCall into the middle of a function\n");
        printf("\n");
        printf("\tAll other options are undefined, but should be caught by CFI :)\n");
        printf("\n\n");
        printf("Here are some pointers so members of `struct foo f` are not optimized away:\n");
        printf("\tint_funcs: %p\n", (void*)f.int_funcs);
        printf("\tbad_int_funcs: %p\n", (void*)f.bad_int_funcs);
        printf("\tfloat_funcs: %p\n", (void*)f.float_funcs);
        printf("\tnot_entries: %p\n", (void*)f.not_entries);
        return 1;
    }

    printf("Calling a function:\n");

    int idx = argv[1][0] - '0';

    return f.int_funcs[idx](idx);
}
