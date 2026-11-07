0x004aa23d  9  100  100  cxx:ok  -  _errno, byte-identical, relocheck OK
0x004af32d  13  100  100  cxx:ok  -  __CxxRestoreUnhandledExceptionFilter, byte-identical, relocheck OK
0x004af31a  19  100  100  cxx:ok  -  __CxxSetUnhandledExceptionFilter in own file, scored with explicit size 19 (no Ghidra size), byte-identical, relocheck OK
0x004a2be9  14  50  50  cxx:ok  c  __security_check_cookie is naked __asm in the CRT source, C always inverts the branch
0x004a2d18  11  -  -  -  c  __finally funclet of printf at 0x004a2cbc (called at 0x004a2d05 after or [ebp-4],-1; _unlock_file2(1,stdout)); no file
0x004a4158  6  -  -  -  c  __finally funclet of the SEH function at 0x004a4126 (called at 0x004a414a; _munlock via FUN_004a31ea); no file
0x004af2d4  70  100  100  cxx:ok  -  __CxxUnhandledExceptionFilter, byte-identical, relocheck OK
0x004a2c2f  23  50  50  cxx:ok  c  _fpmath ends in inline __asm fnclex, no C form emits it
0x004a2bb8  41(48)  100  100  cxx:ok  -  report_failure, whole symbol byte-identical 48 B (Ghidra size short), relocheck BAD only on the scope table (tool limit)
0x004a5e35  41  100  100  cxx:ok  -  _ms_p5_mp_test_fdiv, byte-identical, relocheck OK
0x004a5df5  64  58.3  58.3  cxx:ok  c  _ms_p5_test_fdiv core is an inline __asm block (EBP frame, fixed FDIV sequence)
0x004a2bf7  56  100  100  cxx:ok  -  _cfltcvt_init, byte-identical, relocheck OK
0x004a57e4  86  92.6  92.6  cxx:ok  b  __security_init_cookie, epilogue pop esi scheduled before the jne, 12 rewrites + permuter no gain
0x004a31f3  101  100  100  cxx:ok  -  _cinit, byte-identical, relocheck OK; section tables need unknown-bound arrays (MGLOBAL of typedef T[])
0x004a3258  211  100  100  cxx:ok  -  doexit, byte-identical, relocheck OK (static _initterm inlined)
0x004a583a  320(329)  100  100  cxx:ok  -  __security_error_handler /O1 /GS, whole symbol byte-identical 329 B (Ghidra size short), relocheck OK
