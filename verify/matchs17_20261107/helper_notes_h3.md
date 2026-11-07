0x004a43d2  275  100%  100%  cxx:ok  -  _ftime64 from VC7 ftime64.c under /O1 /GS, first try
0x004aedcf  402  100%  100%  cxx:ok  -  setSBUpLow from VC7 mbctype.c under /O1 /GS, wVector[256]
0x004aefd0  406  100%  100%  cxx:ok  -  _setmbcp_lk from VC7 mbctype.c under /O1 /GS; memset decl before #pragma intrinsic
0x004aefd0 holds 0x004aed77 (CPtoLCID, static, codepage in EAX, 100%)
0x004a4bb7  423  100% (bytes 466 B identical; match.py 89.3% = Ghidra size stops in filter)  100%  cxx:ok  -  WinMainCRTStartup VC7 crt0.c /O1, __try after _RTC_Initialize; relocheck BAD only on scope table $T (tool limit)
0x004a504f  1909  100%  100%  cxx:ok  -  _output VC7 output.c /O1 /GS, 1941 B byte-identical; va_arg as struct DOUBLE
0x004a504f holds 0x004a4fc1 (write_char), 0x004a4ff4 (write_multi_char), 0x004a5018 (write_string), all 100%
