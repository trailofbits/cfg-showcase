#include <iostream>
#include <intrin.h>

struct Base {
    Base() {}
    virtual ~Base() {}
    virtual void printMe() { 
        std::cout << "Base::printMe\n";
    }
};

struct Derived : Base {
    Derived() {}
    virtual ~Derived() {}

    virtual void printMe() {
        std::cout << "Derived::printMe\n";
    }
};

// imagine this is an attacker-created structure 
// in memory
struct Evil {
    Evil() {}
    virtual ~Evil() {}

    virtual void makeAdmin() {
        std::cout << "CFG does *not* prevent this control flow\n";
        std::cout << "Evil::makeAdmin\n";
    }
};

static void evilFunction() {
    // create a nop sled so we can jump into the middle of this function
    // this is a very realistic scenario, as one is very unlikely to find
    // a function that does exactly what one needs, and you'd normally start
    // a ROP chain in the middle of some function
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); 

    std::cout << "CFG Prevents this control flow\n";
    std::cout << "in evilFunction() by simulating a fake C++ object\n";
    exit(-1);
}

#pragma pack(push,1)
// a bare minimum virtual function table for a "Derived" object
// mem1 will be the printMe() function
struct vtbl {
    void* mem0;
    void* mem1;
};

// create a fake version of a "Derived" object
// the bare minimum we need is a virtual function table pointer
struct FakeObj {
    vtbl *table;    
};
#pragma pack(pop)

int main(int argc, const char *argv[]) {

    Evil *eptr = new Evil();
    Derived* dptr = new Derived();
    
    // set up a fake object to simulate an attacker
    // doing a type confusion or use-after-free or similar vulnerability 
    // The slow for printMe is populated by evilFunction
    vtbl table = {nullptr, (void*)((uintptr_t)evilFunction+0x20) };
    FakeObj fake = {&table};

    (void)(argc);
    (void)(argv);

    dptr->printMe();
    
    // imagine a type confusion vulnerability
    // that does something similar
    dptr = reinterpret_cast<Derived*>(eptr);
    dptr->printMe();

    // again, imagine an attacker could craft their own
    // fake object and use it.
    dptr = reinterpret_cast<Derived*>(&fake);
    // CFG will block this call
    dptr->printMe();

    return 0;
}
