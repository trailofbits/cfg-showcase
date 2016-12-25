#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <intrin.h>

using namespace std;

typedef int (*fnptr)(int);

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


    printf("If CFG is enabled, we should not reach this statement.\n");
    printf("CFG ensures control flow only transfers to potentially valid destinations.\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    // need to exit or the program will segfault anyway,
    // since the indirect call skipped the function preamble
    exit(arg);
}

// the declspec(guard(ignore)) ensures that CFG is disabled for indirect calls in this function
__declspec(guard(ignore)) static int unsafe_calls(fnptr f) {
    printf("CFG is disabled for indirect calls in this function\n");
    return f(0);
}

// CFG is enbled for indirect calls in this function
static int safe_calls(fnptr f) {
    printf("CFG will protect calls in this function\n");
    return f(1);
}

static void usage(const char *app) {
    printf("Usage: %s <option>\n", app);
    printf("Option values:\n");
    printf("\t0\tCall using safe dispatcher (will trigger CFG)\n");
    printf("\t1\tCall using CFG-disabled dispatcher (will not trigger CFG)\n");

    exit(1);
}

int main(int argc, const char *argv[]) {

    // need at least one argument
    if( argc != 2 ) {
        usage(argv[0]);
    }
    
    // and the argument must be '0' or '1'
    int idx = argv[1][0] - '0';
    if(idx != 0 && idx != 1 ) {
        usage(argv[0]);
    }
  
    // make sure we get an address that is not valid for CFG
    // but still in the nop sled
    uintptr_t base = (uintptr_t)not_entry_point;
    fnptr fp = (fnptr)(base+0x20);
  
    if(idx == 0) {
        // use the CFG enabled dispatcher
        safe_calls(fp); 
    } else {
        // use the CFG disabled dispatcher
        unsafe_calls(fp);
    }
  
    return 0;
}
