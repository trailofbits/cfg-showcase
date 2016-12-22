CC=cl.exe
CFGFLAG=/guard:cf
CFLAGS=/W4 /nologo /Zi /EHsc
LINKFLAGS=/link mincore.lib

all: cfg_vcall.exe cfg_icall.exe cfg_valid_targets.exe no_cfg_vcall.exe no_cfg_icall.exe no_cfg_valid_targets.exe
	  
cfg_vcall.exe: cfg_vcall.cpp
	 $(CC) $(CFLAGS) $(CFGFLAG) /Fe$@  $** $(LINKFLAGS) $(CFGFLAG)

cfg_icall.exe: cfg_icall.cpp
	 $(CC) $(CFLAGS) $(CFGFLAG) /Fe$@  $** $(LINKFLAGS) $(CFGFLAG)

no_cfg_icall.exe: cfg_icall.cpp
	 $(CC) $(CFLAGS) /Fe$@  $** $(LINKFLAGS)

cfg_valid_targets.exe: cfg_valid_targets.cpp
	 $(CC) $(CFLAGS) $(CFGFLAG) /Fe$@  $** $(LINKFLAGS) $(CFGFLAG)

no_cfg_valid_targets.exe: cfg_valid_targets.cpp
	 $(CC) $(CFLAGS) /Fe$@  $** $(LINKFLAGS)

no_cfg_vcall.exe: cfg_vcall.cpp
	 $(CC) $(CFLAGS) /Fe$@  $** $(LINKFLAGS)

clean:
	 del *.ilk *.pdb *.obj *.exe
