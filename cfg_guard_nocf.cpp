#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <intrin.h>

using namespace std;

typedef double(*fnptr)(double);


// The directies __declspec(guard(ignore)) and __declspec(guard(nocf)) are very similar.
// Both disable CFG for indirect calls within the function,
// but guard(nocf) will track function pointer references in the function, while
// guard(ignore) will *not* track these references.
//
// Effectively, function pointers that escape a function decorated with guard(ignore)
// are not callable (unless they are also referenced elsewhere)
//
// This example demonstrates that difference

// returns a function pointer to ceil(double)
// this function pointer is not callable, since the
// _declspec(guard(ignore)) directive tells the compiler
// to ignore function pointer references in this function.
//
// These function pointer for ceil *not* added to the CFG valid
// destination map
//
__declspec(guard(ignore)) static uintptr_t ignores_address_taken() {
    printf("Returning a non-callable function pointer to ceil\n");
    return  (uintptr_t)((double(*)(double))ceil);
}

// returns a function pointer to floor(double)
// this function pointer *is* callable, since the
// _declspec(guard(nocf)) directive still tracks function
// pointer references in the function.
//
// The function pointer for floor *will* be added to the 
// CFG valid destination map
__declspec(guard(nocf)) static uintptr_t checks_address_taken() {
    printf("Returning a callable function pointer to floor\n");
    return  (uintptr_t)((double(*)(double))floor);
}

static void usage(const char *app) {
    printf("Usage: %s <option>\n", app);
    printf("Option values:\n");
    printf("\t0\tCall a function pointer to ceil from a function decorated with __declspec(guard(ignore)) [should cause a runtime CFG error]\n");
    printf("\t1\tCall a function pointer to floor from a function decorated with __declspec(guard(nocf)) [should succeed]\n");

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
  
    double res = 0.0;
    if(idx == 0) {
        fnptr ceil_ptr = (fnptr)ignores_address_taken();
        res = ceil_ptr(3.141592653);
    } else {
        fnptr floor_ptr = (fnptr)checks_address_taken();
        res = floor_ptr(3.141592653);
    }

    printf("Operation result is: %lf\n", res);
  
    return 0;
}
