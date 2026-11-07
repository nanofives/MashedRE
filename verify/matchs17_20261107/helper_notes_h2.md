0x004a77eb  21  100.0  100.0  cxx:ok  -  _unlock, /O1 /Oy- (original keeps the ebp frame), relocheck OK
0x004a787f  49  100.0  100.0  cxx:ok  -  _lock, /O1 /Oy-, relocheck OK
0x004a78b0  61  100.0  100.0  cxx:ok  -  pre-init PVFV loop with __try/__except(1), /O1; whole symbol 68 B byte-identical (Ghidra size stops early, line score 86.4%); relocheck BAD only on the SEH scope-table const (self-relocs compared unmasked, tool limit)
0x004a7796  85  94.1  94.1  cxx:ok  b  _mtdeletelocks, /O1; second loop's pointer IV biased to &kind ([esi-4]/[esi] vs [esi]/[esi+4]); 10 rewrites + permuter no gain
0x004a7800  127  100.0  100.0  cxx:ok  -  _mtinitlocknum, /O1 /Oy-; this CRT version has no __try (inline unlocks), relocheck OK
0x004a467e  170  100.0  100.0  cxx:ok  -  calloc, /O1, __try/__finally; whole symbol 187 B byte-identical incl. funclet 0x004a4728 and tail (line score 89.1%), relocheck OK
0x004a3ac9  187  100.0  100.0  cxx:ok  -  strtok (_getptd()->_token), /O1 /GS, relocheck OK
0x004ae635  191  82.8  85.9  cxx:ok  b  _setlocale_get_all, /O1; original keeps two pointer IVs (prev/cur), CRT index loop gives one (68.8-71.9%); prev/cur struct-pointer form 82.8%, permuter no gain
0x004abc53  199  100.0  100.0  cxx:ok  -  _setenvp (MBCS), /O1, relocheck OK
0x004abf28  290  100.0  100.0  cxx:ok  -  __crtGetEnvironmentStringsA, /O1; W-path failure written as early FreeEnvironmentStringsW + return NULL (CRT text form 81.7%), relocheck OK
0x004ab8d6  375  100.0  100.0  cxx:ok  -  _NMSG_WRITE, /O1 /GS, unsigned table bound (sizeof), _alloca, relocheck OK
0x004a467e holds 0x004a4728 (the __finally funclet, inside the same obj symbol)
