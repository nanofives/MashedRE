# Analysis Changelog

Append-only log of confidence promotions and demotions, written by the `re-classify` skill. One line per event.

Format: `YYYY-MM-DD  RVA  name  oldC->newC  evidence`

Demotions use `oldC<-newC` (arrow flipped).

```
2026-05-02  --------  PROJECT BOOTSTRAP — first MCP session
2026-05-02  004a4bb7  entry         C0->C1  symbol IMPORTED by Ghidra as PE entry; MSVC EH runtime visible nearby (re/analysis/entry_point.md)
2026-05-02  entry_callees-20260502  scribe-claim   bucket=entry_callees rvas=18
2026-05-02  00492370  FUN_00492370            C0->C1  structural read; 4-param; 3-phase init/teardown; strings AppInitialiseOnBootup/AppDestroy (re/analysis/entry_callees/00492370.md)
2026-05-02  004a31f3  FUN_004a31f3            C0->C1  structural read; fn-ptr table iterator 005ea03c×7 + 005ea000×14; _atexit reg (re/analysis/entry_callees/004a31f3.md)
2026-05-02  004a332b  FUN_004a332b            C0->C1  structural read; no-return wrapper for FUN_004a3258 (re/analysis/entry_callees/004a332b.md)
2026-05-02  004a334d  FUN_004a334d            C0->C1  structural read; normal-return wrapper: FUN_004a3258(0,0,1) (re/analysis/entry_callees/004a334d.md)
2026-05-02  004a3440  __chkstk                C0->C1  library match VS2003 __chkstk; stack-probe loop (re/analysis/entry_callees/004a3440.md)
2026-05-02  004a4b6e  __amsg_exit             C0->C1  library match VS2003 __amsg_exit (re/analysis/entry_callees/004a4b6e.md)
2026-05-02  004a4b93  fast_error_exit         C0->C1  library match VS2003 _fast_error_exit (re/analysis/entry_callees/004a4b93.md)
2026-05-02  004a5984  __SEH_prolog            C0->C1  library match VS __SEH_prolog (re/analysis/entry_callees/004a5984.md)
2026-05-02  004a59bf  __SEH_epilog            C0->C1  library match VS __SEH_epilog (re/analysis/entry_callees/004a59bf.md)
2026-05-02  004a78b0  FUN_004a78b0            C0->C1  structural read; fn-ptr table 005e7b84 iterator; U-0005 (re/analysis/entry_callees/004a78b0.md)
2026-05-02  004a8a04  FUN_004a8a04            C0->C1  structural read; TlsAlloc+_calloc(1,0x88)+TlsSetValue (re/analysis/entry_callees/004a8a04.md)
2026-05-02  004aa3fe  __heap_init             C0->C1  library match VS2003 __heap_init (re/analysis/entry_callees/004aa3fe.md)
2026-05-02  004abbea  FUN_004abbea            C0->C1  structural read; byte-scan cmdline ptr; checks 0x22/0x21 (re/analysis/entry_callees/004abbea.md)
2026-05-02  004abc53  __setenvp               C0->C1  library match VS2003 __setenvp (re/analysis/entry_callees/004abc53.md)
2026-05-02  004abe86  FUN_004abe86            C0->C1  structural read; __fastcall; GetModuleFileNameA (re/analysis/entry_callees/004abe86.md)
2026-05-02  004abf28  ___crtGetEnvironmentStringsA  C0->C1  library match VS2003 (re/analysis/entry_callees/004abf28.md)
2026-05-02  004ac04a  FUN_004ac04a            C0->C1  structural read; CRT file-handle table init (re/analysis/entry_callees/004ac04a.md)
2026-05-02  entry_callees-20260502  scribe-release writes=18 errors=0
```
2026-05-02  00402750  FUN_00402750            C0->C1  structural read; 57 callees; PIZ/TXD asset load chain; DAT_00771a0c vtable; U-0007 U-0008 (re/analysis/boot_app_init/00402750.md)
2026-05-02  00402a40  FUN_00402a40            C0->C1  structural read; 43 callees; teardown; DAT_00636ac8 zeroed via FUN_004c5930 (re/analysis/boot_app_init/00402a40.md)
2026-05-02  00492270  FUN_00492270            C0->C1  structural read; FUN_00493710(0) gate; 3 callees (re/analysis/boot_app_init/00492270.md)
2026-05-02  00492290  FUN_00492290            C0->C1  structural read; while-loop on DAT_00828300+FUN_00499690; 8 callees (re/analysis/boot_app_init/00492290.md)
2026-05-02  004924f0  FUN_004924f0            C0->C1  structural read; zero-fill 0xdce9 DWORDs at DAT_007f0f60; nested init loops; U-0009 (re/analysis/boot_app_init/004924f0.md)
2026-05-02  00493540  thunk_FUN_00495150      C0->C1  structural read; 4-byte thunk→0x00495150; target verified+decomped (re/analysis/boot_app_init/00493540.md)
2026-05-02  00493550  thunk_FUN_004938c0      C0->C1  structural read; 4-byte thunk→0x004938c0; target verified+decomped (re/analysis/boot_app_init/00493550.md)
2026-05-02  00493560  thunk_FUN_004954f0      C0->C1  structural read; 4-byte thunk→0x004954f0; target ShowCursor(1) conditional; returns 0 (re/analysis/boot_app_init/00493560.md)
2026-05-02  00493900  FUN_00493900            C0->C1  structural read; cmd-line tokenizer on space; sets DAT_006147bc/c0 DAT_007719e0/e4/e8; S-0001 S-0002 (re/analysis/boot_app_init/00493900.md)
2026-05-02  004963e0  FUN_004963e0            C0->C1  structural read; _fputs wrapper on DAT_00772fbc FILE* (re/analysis/boot_app_init/004963e0.md)
2026-05-02  004996f0  FUN_004996f0            C0->C1  structural read; ShowWindow+UpdateWindow on DAT_007e9584 HWND (re/analysis/boot_app_init/004996f0.md)
2026-05-02  00499ba0  FUN_00499ba0            C0->C1  structural read; CoInitialize+RegisterClassA("MASHED")+CreateWindowExA→DAT_007e9584; S-0003 (re/analysis/boot_app_init/00499ba0.md)
2026-05-02  00499cc0  FUN_00499cc0            C0->C1  structural read; DestroyWindow(DAT_007e9584); returns DAT_007e95a8 (re/analysis/boot_app_init/00499cc0.md)
2026-05-02  004c5930  FUN_004c5930            C0->C1  structural read; linked-list traversal+unlink; vtable +0x11c; U-0010 U-0011; S-0004 S-0005 (re/analysis/boot_app_init/004c5930.md)
2026-05-02  005c9d00  FUN_005c9d00            C0->C1  structural read; 2-byte function; returns 0 (re/analysis/boot_app_init/005c9d00.md)
