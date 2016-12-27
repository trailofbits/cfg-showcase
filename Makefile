CC=cl.exe
CFGFLAG=/guard:cf
CFLAGS=/W4 /nologo /Zi /EHsc
LINKFLAGS=/link mincore.lib

all: cfg_suppressed_export.dll cfg_guard_suppress.exe cfg_guard_ignore.exe cfg_vcall.exe cfg_icall.exe cfg_valid_targets.exe no_cfg_vcall.exe no_cfg_icall.exe no_cfg_valid_targets.exe no_cfg_guard_ignore.exe no_cfg_guard_suppress.exe
	  
cfg_suppressed_export.dll: cfg_suppressed_export.cpp
	 $(CC) /O2 /W4 /nologo $(CFGFLAG) /Fe$@ $** $(LINKFLAGS) /DLL $(CFGFLAG)

cfg_guard_suppress.exe: cfg_guard_suppress.cpp
	 $(CC) $(CFLAGS) $(CFGFLAG) /Fe$@  $** $(LINKFLAGS) $(CFGFLAG)

cfg_guard_ignore.exe: cfg_guard_ignore.cpp
	 $(CC) $(CFLAGS) $(CFGFLAG) /Fe$@  $** $(LINKFLAGS) $(CFGFLAG)

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

no_cfg_guard_ignore.exe: cfg_guard_ignore.cpp
	 $(CC) $(CFLAGS) /Fe$@  $** $(LINKFLAGS)

no_cfg_guard_suppress.exe: cfg_guard_suppress.cpp
	 $(CC) $(CFLAGS) $(CFGFLAG) /Fe$@  $** $(LINKFLAGS) $(CFGFLAG)

clean:
	 del *.ilk *.pdb *.obj *.exe *.exp *.dll *.lib
