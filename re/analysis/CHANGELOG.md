# Analysis Changelog

Append-only log of confidence promotions and demotions, written by the `re-classify` skill. One line per event.
2026-05-06  004299d0  TimeRecord::WriteTrackBest  C0->C2  Ghidra decomp; leaderboard-20260506-JJJJJ; re/analysis/leaderboard/0x004299d0.md; U-2227 filed
2026-05-06  0042d5a0  FUN_0042d5a0  stub->C1  Ghidra decomp; credits_screen-NNNNN; re/analysis/credits_screen/0x0042d5a0.md; U-2177 U-2178 U-2179 filed; S-0449 cleared
2026-05-06  00472f40  FUN_00472f40  stub->C1  Ghidra decomp; credits_screen-NNNNN; re/analysis/credits_screen/0x00472f40.md; S-0450 cleared
2026-05-06  004730b0  FUN_004730b0  stub->C1  Ghidra decomp; credits_screen-NNNNN; re/analysis/credits_screen/0x004730b0.md; S-0451 cleared

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
2026-05-02  004a2c2f  FUN_004a2c2f            C0->C1  structural read; calls FUN_004a2bf7+__ms_p5_mp_test_fdiv->DAT_00773994+FUN_004a5de3 (re/analysis/boot_crt_exit/0x004a2c2f.md)
2026-05-02  004a3258  FUN_004a3258            C0->C1  structural read; __lock(8); atexit walk 008ab6cc; fn-ptr tables 005ea05c+005ea068; U-0028+U-0029 (re/analysis/boot_crt_exit/0x004a3258.md)
2026-05-02  004a31b1  ___crtExitProcess       C0->C1  library match VS2003; GetModuleHandle(mscoree)+GetProcAddress(CorExitProcess)+ExitProcess (re/analysis/boot_crt_exit/0x004a31b1.md)
2026-05-02  004a333c  __exit                  C0->C1  library match VS2003; single call FUN_004a3258(_Code,1,0) (re/analysis/boot_crt_exit/0x004a333c.md)
2026-05-02  004a40fe  ___onexitinit           C0->C1  library match VS2003; _malloc(0x80); init DAT_008ab6cc/d0 (re/analysis/boot_crt_exit/0x004a40fe.md)
2026-05-02  004a415e  _atexit                 C0->C1  library match VS2003; delegates to __onexit; returns 0/-1 (re/analysis/boot_crt_exit/0x004a415e.md)
2026-05-02  004a467e  _calloc                 C0->C1  library match VS2003; SBH+HeapAlloc paths; new-handler loop; SEH (re/analysis/boot_crt_exit/0x004a467e.md)
2026-05-02  004a57e4  FUN_004a57e4            C0->C1  structural read; XOR seed to DAT_00616038; fallback 0xbb40e64e (re/analysis/boot_crt_exit/0x004a57e4.md)
2026-05-02  004a774d  __mtinitlocks           C0->C1  library match VS2003; 36-lock table 00616408; ___crtInitCritSecAndSpinCount spincount=4000 (re/analysis/boot_crt_exit/0x004a774d.md)
2026-05-02  004a87f7  FUN_004a87f7            C0->C1  structural read; __mtdeletelocks; TlsFree(DAT_00616658) sentinel 0xffffffff (re/analysis/boot_crt_exit/0x004a87f7.md)
2026-05-02  004aa3e4  ___heap_select          C0->C1  library match VS2003; reads DAT_0077399c+DAT_007739a8; returns 1 or 3 (re/analysis/boot_crt_exit/0x004aa3e4.md)
2026-05-02  004aa44f  ___sbh_heap_init        C0->C1  library match VS2003; HeapAlloc(008aa69c,0,0x140); inits 6 SBH globals (re/analysis/boot_crt_exit/0x004aa44f.md)
2026-05-02  004ab8d6  FUN_004ab8d6            C0->C1  structural read; error-code table scan 00616890; stderr/MessageBox path; stack-cookie 00616038; U-0030 (re/analysis/boot_crt_exit/0x004ab8d6.md)
2026-05-02  004aba4d  __FF_MSGBANNER          C0->C1  library match VS2003; mode check 007739f0+006160c8; calls FUN_004ab8d6(0xfc)+fn-ptr 00773c38+FUN_004ab8d6(0xff) (re/analysis/boot_crt_exit/0x004aba4d.md)
2026-05-02  004938c0  FUN_004938c0            C0->C1  structural read; SoftwareTidyUpBeforeExiting; 5 sequential void calls; U-0087 (re/analysis/rw_engine_teardown/0x004938c0.md)
2026-05-02  00558470  FUN_00558470            C0->C1  structural read; 2x vtable-ptr dispatch via DAT_007d3ff8+0x10c; zeros 3 globals; U-0088 (re/analysis/rw_engine_teardown/0x00558470.md)
2026-05-02  00550390  FUN_00550390            C0->C1  structural read; linked-list teardown+DeleteCriticalSection; S-0080; U-0089 (re/analysis/rw_engine_teardown/0x00550390.md)
2026-05-02  004c2f60  FUN_004c2f60            C0->C1  structural read; 2x FUN_004c2c90(0x12/3)+FUN_004d8060; writes to DAT_007d3ff8+0x124; S-0081 S-0005; U-0090 (re/analysis/rw_engine_teardown/0x004c2f60.md)
2026-05-02  004c3040  FUN_004c3040            C0->C1  structural read; FUN_004c2c90(+4,1); copies 0x4b DWORDs; frees engine struct; S-0081; U-0091 (re/analysis/rw_engine_teardown/0x004c3040.md)
2026-05-02  004c3270  FUN_004c3270            C0->C1  structural read; DAT_007d3ff4 refcount check; FUN_004d7ca0+FUN_004ccf20; S-0082 S-0083; U-0092 (re/analysis/rw_engine_teardown/0x004c3270.md)
2026-05-02  rw_engine_init-20260502-1734  scribe-claim   bucket=rw_engine_init rvas=18
2026-05-02  00493710  FUN_00493710 (RW_INIT_FN)  C0->C1  structural read; 429 bytes; 27 callees; RwEngineInit sequential chain; S-0060..S-0067; U-0067 U-0068 (re/analysis/rw_engine_init/00493710.md)
2026-05-02  0045b350  FUN_0045b350            C0->C1  structural read; zero-body artifact; body_start==body_end; U-0069 (re/analysis/rw_engine_init/0045b350.md)
2026-05-02  00493600  FUN_00493600            C0->C1  structural read; 2x FUN_004ce790(ptr-to-global, fn-ptr, fn-ptr-or-label); S-0068 (re/analysis/rw_engine_init/00493600.md)
2026-05-02  00493640  FUN_00493640            C0->C1  structural read; 18-call RenderwareAttachPlugins chain; D-0244..D-0261 (re/analysis/rw_engine_init/00493640.md)
2026-05-02  004951e0  FUN_004951e0            C0->C1  structural read; 4-byte Ghidra thunk→FUN_004955d0; U-0070 (re/analysis/rw_engine_init/004951e0.md)
2026-05-02  004951f0  FUN_004951f0            C0->C1  structural read; ShowCursor(0) conditional; FUN_004cbb60→DAT_00771e58; reads +0xC4/+0xCC masked 0xFFFF (re/analysis/rw_engine_init/004951f0.md)
2026-05-02  00495270  FUN_00495270            C0->C1  structural read; FUN_00499710()→*param_1; populates buffer for FUN_004c30b0 (re/analysis/rw_engine_init/00495270.md)
2026-05-02  004963e0  FUN_004963e0            C0->C1  structural read; _fputs wrapper on DAT_00772fbc FILE* [also boot_app_init] (re/analysis/rw_engine_init/004963e0.md)
2026-05-02  004a2cbd  FUN_004a2cbd            C0->C1  structural read; FidDB VS2003 _printf/_wprintf; __cdecl variadic; stdout DAT_00616110 (re/analysis/rw_engine_init/004a2cbd.md)
2026-05-02  004c2ed0  FUN_004c2ed0            C0->C1  structural read; FUN_004c2c90(DAT_007d3ff8+0x10, 6, param_1, 0, param_2); returns param_1 on success (re/analysis/rw_engine_init/004c2ed0.md)
2026-05-02  004c2f00  FUN_004c2f00            C0->C1  structural read; FUN_004c2c90(DAT_007d3ff8+0x10, 10, &local_4, 0, 0); returns local_4 or 0xffffffff (re/analysis/rw_engine_init/004c2f00.md)
2026-05-02  004c2fb0  FUN_004c2fb0            C0->C1  structural read; FUN_004c2c90 ids 2/3/17; FUN_004d8000+FUN_004cf160; writes DAT_007d3ff8+0x124=3 (re/analysis/rw_engine_init/004c2fb0.md)
2026-05-02  004c3040  FUN_004c3040            C0->C1  structural read; FUN_004c2c90(+4,1); copies 0x4b DWORDs; indirect via DAT_007d3fd4; decrements DAT_007d3ff4 [also rw_engine_teardown] (re/analysis/rw_engine_init/004c3040.md)
2026-05-02  004c30b0  FUN_004c30b0            C0->C1  structural read; 446 bytes; manages DAT_007d3ff8 block; indirect alloc via [0x42]; FUN_004c2c90 ids 0/4/11; sets [0x49]=2 (re/analysis/rw_engine_init/004c30b0.md)
2026-05-02  004c3270  FUN_004c3270            C0->C1  structural read; DAT_007d3ff4 check; FUN_004d7ca0+FUN_004ccf20; writes +0x124=0 [also rw_engine_teardown] (re/analysis/rw_engine_init/004c3270.md)
2026-05-02  004c32b0  FUN_004c32b0 (RwEngineInit)  C0->C1  structural read; 767 bytes; sets DAT_007d3ff8=&DAT_007d3ec8; 14x FUN_004d7de0 plugin ids; sets [0x49]=1; U-0071 (re/analysis/rw_engine_init/004c32b0.md)
2026-05-02  004c5c80  FUN_004c5c80            C0->C1  structural read; writes param_1 to *(DAT_007d4054+0x10+DAT_007d3ff8); called as (0) (re/analysis/rw_engine_init/004c5c80.md)
2026-05-02  004c9eb0  FUN_004c9eb0            C0->C1  structural read; writes param_1 to DAT_006181c4; double-indirect via DAT_007d4108+0x18/+0x1c; 3-elem loop 0x5d8b80; called as (0x3c) (re/analysis/rw_engine_init/004c9eb0.md)
2026-05-02  sweep-20260502-1827  scribe-claim-sweep  buckets=4
2026-05-02  sweep-20260502-1827  scribe-claim  bucket=boot_app_init  rvas=15  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release  bucket=boot_app_init  writes=15  errors=0  (sweep)
2026-05-02  sweep-20260502-1827  scribe-claim  bucket=boot_crt_exit  rvas=14  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release  bucket=boot_crt_exit  writes=14  errors=0  (sweep)
2026-05-02  sweep-20260502-1827  scribe-claim  bucket=rw_engine_teardown  rvas=6  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release  bucket=rw_engine_teardown  writes=6  errors=0  (sweep)
2026-05-02  sweep-20260502-1827  scribe-claim  bucket=rw_engine_init  rvas=18  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release  bucket=rw_engine_init  writes=18  errors=0  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release-sweep  buckets=4  total_writes=53  errors=0
2026-05-02  004a45fb  _malloc                          C0->C1  library match VS2003; __nh_malloc wrapper; DAT_00773c34 _NhFlag@004a45fb (re/analysis/boot_crt_env/004a45fb.md)
2026-05-02  004a460d  _free                            C0->C1  library match VS2003; SBH mode DAT_008aa6a0@004a4620; lock-id=4@004a4629; HeapFree DAT_008aa69c@004a466c (re/analysis/boot_crt_env/004a460d.md)
2026-05-02  004a9410  _strlen                          C0->C1  library match VS; dword-at-a-time 0x7efefeff@004a9442; no callees (re/analysis/boot_crt_env/004a9410.md)
2026-05-02  004aaff0  _memcpy                          C0->C1  library match VS2003; MOVSD.REP@004ab023 + backward-copy; no callees (re/analysis/boot_crt_env/004aaff0.md)
2026-05-02  004abd1a  FUN_004abd1a                     C0->C1  structural read; token parser; non-standard ABI EAX+ECX+ESI; char-class 0x8aa341@004abd61; U-0047 (re/analysis/boot_crt_env/004abd1a.md)
2026-05-02  004ac560  FUN_004ac560                     C0->C1  structural read; 7-byte thunk JMP 0x004ac5d5@004ac565 into FUN_004ac570; D-0165 (re/analysis/boot_crt_env/004ac560.md)
2026-05-02  004ae29f  ___crtInitCritSecAndSpinCount    C0->C1  library match VS2003; lazy-init DAT_00773d60@004ae2ab; fallback ___crtInitCritSecNoSpinCount@8@004ae2e1; S-0045 (re/analysis/boot_crt_env/004ae29f.md)
2026-05-02  004af2b6  ___initmbctable                  C0->C1  library match VS2003; once-init guard DAT_008ab6d4@004af2b6; __setmbcp(-3)@004af2bf; S-0046 (re/analysis/boot_crt_env/004af2b6.md)
2026-05-02  004affe0  FUN_004affe0                     C0->C1  structural read; 17-byte wrapper FUN_004affaf(param_1,0,4)@004affe8; S-0047 (re/analysis/boot_crt_env/004affe0.md)
2026-05-02  00495530  FUN_00495530                     C0->C1  DirectInput8Create init wrapper; GetModuleHandleA(NULL)+DirectInput8Create; 0x800@00495549; DAT_005d0a8c@00495544; DAT_00771e78@0049553f; U-0267 U-0268 (re/analysis/input_dinput/00495530.md)
2026-05-02  004987b0  FUN_004987b0                     C0->C1  debug printf wrapper; FUN_004a42c5(buf,fmt,va)+OutputDebugStringA; 512B local buf; cookie DAT_00616038@body; S-0260; D-0700 (re/analysis/input_dinput/004987b0.md)
2026-05-02  audio_rws_loader-20260502-1838  scribe-claim   bucket=audio_rws_loader rvas=17
2026-05-02  005a7b60  FUN_005a7b60 (AUDIO_LOAD_FN)     C0->C1  structural read; 769 bytes; 16 depth-1 callees; RWS chunk dispatch 0x80a/0x80c/0x802; alloc tag 0x30808; S-none (all callees in subset); U-0107 U-0108 U-0109 (re/analysis/audio_rws_loader/005a7b60.md)
2026-05-02  004522d0  FUN_004522d0                     C0->C1  structural read; 11 bytes; vtable dealloc trampoline DAT_007d3ff8+0x10c; jumptable unrecoverable; S-0100; U-0110 (re/analysis/audio_rws_loader/004522d0.md)
2026-05-02  004cbd30  FUN_004cbd30                     C0->C1  structural read; 317 bytes; RW stream read; 4-type dispatch (file/mem/callback); error codes 5/0x8000001a/0xe; S-0101..S-0104; U-0111 (re/analysis/audio_rws_loader/004cbd30.md)
2026-05-02  004cc050  FUN_004cc050                     C0->C1  structural read; 249 bytes; RW stream seek/skip; same 4-type dispatch; seek mode 1; S-0105; U-0112 (re/analysis/audio_rws_loader/004cc050.md)
2026-05-02  005a79a0  FUN_005a79a0                     C0->C1  structural read; 155 bytes; audio object destructor; unlinks from DAT_007dca7c pool list; walks wave list; S-0106..S-0108; U-0113 U-0114 (re/analysis/audio_rws_loader/005a79a0.md)
2026-05-02  005a7b40  FUN_005a7b40                     C0->C1  structural read; 15 bytes; swap DAT_007dcabc current audio context; returns old value (re/analysis/audio_rws_loader/005a7b40.md)
2026-05-02  005a7b50  FUN_005a7b50                     C0->C1  structural read; 5 bytes; read DAT_007dcabc; getter pair to FUN_005a7b40 (re/analysis/audio_rws_loader/005a7b50.md)
2026-05-02  005a7ee0  FUN_005a7ee0                     C0->C1  structural read; 134 bytes; audio object init; empty circular wave list at +0x0c; inserts into DAT_007dca7c pool; vtable+0xf4; S-0109..S-0111; U-0115 U-0116 (re/analysis/audio_rws_loader/005a7ee0.md)
2026-05-02  005ab380  FUN_005ab380                     C0->C1  structural read; 136 bytes; RWS 12-byte chunk header reader; legacy vs modern version decode; 5-field output struct; U-0117 (re/analysis/audio_rws_loader/005ab380.md)
2026-05-02  005ab410  FUN_005ab410                     C0->C1  structural read; 139 bytes; RWS chunk type seeker; version range 0x34fff..0x37002; loops FUN_005ab380+FUN_004cc050; U-0118 (re/analysis/audio_rws_loader/005ab410.md)
2026-05-02  005abcf0  FUN_005abcf0                     C0->C1  structural read; 61 bytes; wave node destructor; double-indirect vtable *(*(+0x0c)+0x10); S-0112..S-0115; U-0119 U-0120 (re/analysis/audio_rws_loader/005abcf0.md)
2026-05-02  005abfa0  FUN_005abfa0                     C0->C1  structural read; 619 bytes; per-wave loader; 0x803+0x804 chunks; format check DAT_005e6414/0x5e6444; PCM inner loop; S-0116..S-0120; U-0121 U-0122 U-0123 (re/analysis/audio_rws_loader/005abfa0.md)
2026-05-02  005ade10  FUN_005ade10                     C0->C1  structural read; 65 bytes; doubly-linked list remove-by-value; node[0]=prev,[1]=next,[2]=data; pool free FUN_005ae920 at DAT_009146c0; S-0121; U-0124 (re/analysis/audio_rws_loader/005ade10.md)
2026-05-02  005aea00  FUN_005aea00                     C0->C1  structural read; 11 bytes; vtable alloc trampoline DAT_007d3ff8+0x108; jumptable unrecoverable; U-0125 (re/analysis/audio_rws_loader/005aea00.md)
2026-05-02  005aea10  FUN_005aea10                     C0->C1  structural read; 42 bytes; aligned alloc wrapper; FUN_005aea00(size+4,tag); aligns 4B; stores base at aligned-4; mask 0xfffffffc (re/analysis/audio_rws_loader/005aea10.md)
2026-05-02  005aea40  FUN_005aea40                     C0->C1  structural read; 15 bytes; thin free wrapper over FUN_004522d0 (re/analysis/audio_rws_loader/005aea40.md)
2026-05-02  005aec00  FUN_005aec00                     C0->C1  structural read; 42 bytes; in-place byte-reversal; param_2/2 swap iterations; endian correction for wave count (re/analysis/audio_rws_loader/005aec00.md)
2026-05-02  audio_rws_loader-20260502-1838  scribe-release  writes=17 errors=0
2026-05-02  004c2c90  FUN_004c2c90  C0->C1  structural read; 190 bytes; vtable dispatch param_1+4 with cmd IDs 0xd-0x12; default calls FUN_004d7ff0+FUN_004d8480; U-0227 U-0228 (re/analysis/rw_engine_teardown_d2/0x004c2c90.md)
2026-05-02  004ccf20  FUN_004ccf20  C0->C1  structural read; 311 bytes; doubly-linked-list teardown loop; vtable calls +0x10c/+0x11c via DAT_007d3ff8; zeroes DAT_007d45fc/007d45f8; U-0229 U-0230 U-0231 (re/analysis/rw_engine_teardown_d2/0x004ccf20.md)
2026-05-02  004d7ca0  FUN_004d7ca0  C0->C1  structural read; 196 bytes; guards DAT_007d6c50; calls FUN_004ccce0+FUN_004cc9f0; iterates array DAT_007d6c54 freeing structs via vtable; returns 1; S-0222 S-0223 U-0232 U-0233 (re/analysis/rw_engine_teardown_d2/0x004d7ca0.md)
2026-05-02  004d8060  FUN_004d8060  C0->C1  structural read; 44 bytes; linked-list iteration calling fn ptr at node+0x24 with (param_2,*node,node[1]); advance via node+0x34; returns param_1; U-0234 (re/analysis/rw_engine_teardown_d2/0x004d8060.md)
2026-05-02  00551510  FUN_00551510  C0->C1  structural read; 49 bytes; dispatch on param_2; 1=fn ptr at param_1+0x20(param_1+0x50); 2=fn ptr at param_1+0x24(param_1+0x50); all indirect; U-0235 (re/analysis/rw_engine_teardown_d2/0x00551510.md)
2026-05-02  0047b9b0  FUN_0047b9b0                     C0->C1  LUA_INIT_FN; creates state via 0x0047b860; indirect callback (*param_3); executes via 0x0047b8d0; closes via 0x0047b880; U-0307 (re/analysis/input_lua/0047b9b0.md)
2026-05-02  0047b860  FUN_0047b860                     C0->C1  Lua state creator; FUN_004b7330(0)->DAT_006bf1e0; FUN_004c0510(state); S-0300 S-0301; U-0308 (re/analysis/input_lua/0047b860.md)
2026-05-02  0047b880  FUN_0047b880                     C0->C1  Lua state closer; FUN_004b7480(DAT_006bf1e0); zeroes DAT_006bf1e0; S-0302 (re/analysis/input_lua/0047b880.md)
2026-05-02  0047b8d0  FUN_0047b8d0                     C0->C1  Lua file reader+exec; fopen("rb"@0x005cf010)+ftell+fread into 32k stack buf; FUN_0047b8a0(buf,count,param_2); S-0303 S-0304; U-0309; D-0820 (re/analysis/input_lua/0047b8d0.md)
2026-05-02  004a2be9  __security_check_cookie     C0->C1  cookie check DAT_00616038; calls report_failure@004a2bb8 on mismatch; S-0180 (re/analysis/boot_crt_exit_d3/004a2be9.md)
2026-05-02  004a2bf7  FUN_004a2bf7                C0->C1  fn-ptr table writer 006160d8..ec; 6 slots; no callees; U-0187 U-0188 (re/analysis/boot_crt_exit_d3/004a2bf7.md)
2026-05-02  004a34b0  _strncpy                    C0->C1  library VS; dword-at-a-time 0x7efefeff; null-detect; no callees (re/analysis/boot_crt_exit_d3/004a34b0.md)
2026-05-02  004a4126  __onexit                    C0->C1  library VS2003; SEH frame; FUN_004a31e1+__onexit_lk+FUN_004a4158; S-0181..S-0183; U-0189 U-0190 (re/analysis/boot_crt_exit_d3/004a4126.md)
2026-05-02  004a4728  FUN_004a4728                C0->C1  8-byte wrapper; FUN_004a77eb(4) (re/analysis/boot_crt_exit_d3/004a4728.md)
2026-05-02  004a5de3  FUN_004a5de3                C0->C1  calls __controlfp(0x10000,0x30000); S-0184 (re/analysis/boot_crt_exit_d3/004a5de3.md)
2026-05-02  004a5e35  __ms_p5_mp_test_fdiv        C0->C1  library VS2003; KERNEL32 GetProcAddress IsProcessorFeaturePresent; fallback __ms_p5_test_fdiv@004a5df5; S-0185 (re/analysis/boot_crt_exit_d3/004a5e35.md)
2026-05-02  004a5f07  ___endstdio                 C0->C1  library VS2003; __flushall+conditional __fcloseall; DAT_007739d4; S-0186 S-0187 (re/analysis/boot_crt_exit_d3/004a5f07.md)
2026-05-02  004a7796  __mtdeletelocks             C0->C1  library VS; two-pass loop 616408..616528+8; pass1 delete+free; pass2 delete pinned (re/analysis/boot_crt_exit_d3/004a7796.md)
2026-05-02  004a77eb  FUN_004a77eb                C0->C1  LeaveCriticalSection(&DAT_00616408[param_1*2]) (re/analysis/boot_crt_exit_d3/004a77eb.md)
2026-05-02  004a787f  __lock                      C0->C1  library VS2003; lazy-init FUN_004a7800; __amsg_exit(0x11) on fail; EnterCriticalSection; S-0188 (re/analysis/boot_crt_exit_d3/004a787f.md)
2026-05-02  004aac76  ___sbh_alloc_block          C0->C1  library VS2003; SBH region/group scan+split; 008aa688 array; S-0189 S-0190; U-0191 (re/analysis/boot_crt_exit_d3/004aac76.md)
2026-05-02  004aaf72  __callnewh                  C0->C1  library VS2003; calls DAT_00773c30 handler if non-null (re/analysis/boot_crt_exit_d3/004aaf72.md)
2026-05-02  004aaf90  _memset                     C0->C1  library VS; 4-byte-aligned fill; 0x1010101 replication; no callees (re/analysis/boot_crt_exit_d3/004aaf90.md)
2026-05-02  004ac45c  ___crtMessageBoxA           C0->C1  library VS2003; lazy LoadLibrary(user32); MB_DEFAULT_DESKTOP_ONLY 0x40000; MB_SERVICE_NOTIFICATION 0x200000 (re/analysis/boot_crt_exit_d3/004ac45c.md)
2026-05-02  save_gamesave-20260502-1854  session-analysis  bucket=save_gamesave  rvas=6  U-0287..U-0288  S-0280..S-0284  D-0760..D-0765
2026-05-02  render_d3d9_device-20260502-1856  session-analysis  bucket=render_d3d9_device  rvas=12  U-0247 U-0248  D-0640..D-0646  cap_count=12
2026-05-02  004c7a70  _rwDeviceSystemFn (D3D_INIT_FN)  C0  listing-only; FPO prologue; function undefined in Ghidra; 23-case switch; Direct3DCreate9@004c7e0b; CreateDevice via vtbl+0x40@004c82b6; U-0247 (re/analysis/render_d3d9_device/0x004c7a70.md)
2026-05-02  004c8650  FUN_004c8650  C0->C1  structural read; render-state cache init; zeros 006181c8..d8 + array 007d40c0 (re/analysis/render_d3d9_device/0x004c8650.md)
2026-05-02  004c8690  FUN_004c8690  C0->C1  structural read; constant pool create/reset; 007d4568 handle; 260-slot table 007d4158; callees 004cc7f0 004ccc50 (re/analysis/render_d3d9_device/0x004c8690.md)
2026-05-02  004c8740  FUN_004c8740  C0->C1  structural read; MSAA enumeration; IDirect3D9::CheckDeviceMultiSampleType vtbl+0x2c; max type→006181b4 (re/analysis/render_d3d9_device/0x004c8740.md)
2026-05-02  004c8800  FUN_004c8800  C0->C1  structural read; D3DPRESENT_PARAMETERS builder; GetWindowRect; IDirect3D9 vtbl+0x24/28/2c/30; depth formats 0x4b..0x50; windowed/FS paths (re/analysis/render_d3d9_device/0x004c8800.md)
2026-05-02  004c8c70  FUN_004c8c70  C0->C1  structural read; D3D9 device teardown; IDirect3DDevice9 SetTexture/SetIndices/SetStreamSource/SetPixelShader/SetVertexDeclaration/SetVertexShader/Release; 5 callees (re/analysis/render_d3d9_device/0x004c8c70.md)
2026-05-02  004c8e50  FUN_004c8e50  C0->C1  structural read; raster dispatch table init; 27 rwRASTERSYSTEM_* entries 0x02..0x1c; default FUN_005c9d00 (re/analysis/render_d3d9_device/0x004c8e50.md)
2026-05-02  004cc820  FUN_004cc820  C0->C1  structural read; RwFreeListCreate; 6-param pool allocator; header layout [0..8]; pre-alloc; 007d45cc chain (re/analysis/render_d3d9_device/0x004cc820.md)
2026-05-02  004cc9f0  FUN_004cc9f0  C0->C1  structural read; RwFreeListDestroy; unlinks from 007d45cc; frees blocks+header (re/analysis/render_d3d9_device/0x004cc9f0.md)
2026-05-02  004dcf90  FUN_004dcf90  C0->C1  structural read; CPUID via EFLAGS bit-21; leaf1[+8] bit 23 (likely MMX); U-0248 (re/analysis/render_d3d9_device/0x004dcf90.md)
2026-05-02  004dcff0  FUN_004dcff0  C0->C1  structural read; CPUID via EFLAGS bit-21; leaf1[+8] bits 25|26 (likely SSE|SSE2); U-0248 (re/analysis/render_d3d9_device/0x004dcff0.md)
2026-05-02  004dd050  FUN_004dd050  C0->C1  structural read; CPUID via EFLAGS bit-21; leaf1[+8] bit 26 (likely SSE2); U-0248 (re/analysis/render_d3d9_device/0x004dd050.md)
2026-05-02 rw_engine_init_d2-20260502-1905 analysis bucket=rw_engine_init_d2 rvas=18 slot=Mashed_pool3
2026-05-02  004014b0  FUN_004014b0             C0->C1  symmetrical FUN_004e4860/FUN_004c0740/FUN_004e4d90 on 3 globals 0x636560/70/6c; U-0167; S-0163..S-0165 (re/analysis/boot_app_init_d3/0x004014b0.md)
2026-05-02  004015a0  FUN_004015a0             C0->C1  13×104-byte elem loop 0x636578..0x636ac0; FUN_004e6e00 on 10 handles/elem; float 1.0f at +0x34; U-0168; S-0162+S-0166 (re/analysis/boot_app_init_d3/0x004015a0.md)
2026-05-02  004025f0  FUN_004025f0             C0->C1  init pair for 004015a0; local_34 table 13 values; FUN_00401630; U-0169; S-0167..S-0170 (re/analysis/boot_app_init_d3/0x004025f0.md)
2026-05-02  004026d0  FUN_004026d0             C0->C1  loop param_1×; FUN_004671a0 3× varying args; local_4=0xff000000; U-0170; S-0171..S-0175 (re/analysis/boot_app_init_d3/0x004026d0.md)
2026-05-02  00402f50  FUN_00402f50             C0->C1  no callees; zeros 0x636ae8/af0/af8; writes 40.0f→0x636af4 60.0f→0x636aec; U-0171 (re/analysis/boot_app_init_d3/0x00402f50.md)
2026-05-02  00403640  FUN_00403640             C0->C1  FUN_004034a0(param_1)→DAT_0x636b78; returns 1; S-0176 (re/analysis/boot_app_init_d3/0x00403640.md)
2026-05-02  00403660  FUN_00403660             C0->C1  vtable dispatch *DAT_0x636b70+8 then null; FUN_004c7650(DAT_0x636b78) then zero; U-0172; S-0177 (re/analysis/boot_app_init_d3/0x00403660.md)
2026-05-02  00404830  FUN_00404830             C0->C1  fills 0x636c08..0x639d67 0xffffffff; 13 scalar overrides; stride-6 loop 0x6390b0 values 0..0x17; no callees; U-0173 U-0174 (re/analysis/boot_app_init_d3/0x00404830.md)
2026-05-02  0040bb30  FUN_0040bb30             C0->C1  forwarder FUN_004c5c00(DAT_0x63b8f8,param_1); S-0178 (re/analysis/boot_app_init_d3/0x0040bb30.md)
2026-05-02  0040bb50  FUN_0040bb50             C0->C1  forwarder FUN_004c5c00(DAT_0x63b8fc,param_1); S-0178 (re/analysis/boot_app_init_d3/0x0040bb50.md)
2026-05-02  0040bbb0  FUN_0040bbb0             C0->C1  loads sfx.piz; 4 TXD handles fx/badges/TrackImages/Interface; scorch(0x18)/wfall(0x10)/RWObjShad(0x32); S-0179+S-0305..S-0329 (re/analysis/boot_app_init_d3/0x0040bbb0.md)
2026-05-02  0040bd00  FUN_0040bd00             C0->C1  teardown: 13 no-arg calls + FUN_004c5930 on 4 TXD handles; S-0330..S-0342 (re/analysis/boot_app_init_d3/0x0040bd00.md)
2026-05-02  0040cf80  FUN_0040cf80             C0->C1  polls FUN_0041f320(0..3); conditional init sequence; S-0343..S-0347 (re/analysis/boot_app_init_d3/0x0040cf80.md)
2026-05-02  0040cfd0  FUN_0040cfd0             C0->C1  seq FUN_00490000+FUN_0045bed0+FUN_0045bf30; FUN_00426c00+FUN_004725f0+FUN_00426b40; FUN_00405400; U-0175; S-0348..S-0357 (re/analysis/boot_app_init_d3/0x0040cfd0.md)
2026-05-02  004113b0  FUN_004113b0             C0->C1  stack-cookie; table@0x5f29d0 sum==0x24a3c; alloc vtbl+0x108; format vtbl+0xc4; U-0176 U-0177; S-0358..S-0359 (re/analysis/boot_app_init_d3/0x004113b0.md)
2026-05-02  004114c0  FUN_004114c0             C0->C1  indirect (0x7d3ff8+0x10c)(DAT_0x8a94a8); returns 1; teardown pair for 004113b0 (re/analysis/boot_app_init_d3/0x004114c0.md)
2026-05-02  00418980  thunk_FUN_0041a060       C0->C1  thunk→0x41a060; shockwave/crosshair2/crosshair/exocet.dff/airstrike; 3 init loops 0x63bf30/0x63bde0/0x63c018; S-0307+S-0328+S-0360..S-0369 (re/analysis/boot_app_init_d3/0x00418980.md)
2026-05-02  004189e0  thunk_FUN_004196f0       C0->C1  thunk→0x4196f0; teardown loops 0x63bf70+0x63bde0; S-0161+S-0166+S-0370..S-0372 (re/analysis/boot_app_init_d3/0x004189e0.md)
2026-05-02  boot_app_init_d3-20260502-1859  session-analysis  bucket=boot_app_init_d3  rvas=18  U-0167..U-0177  S-0160..S-0179+S-0305..S-0372  D-0400..D-0401
2026-05-02  sweep-20260502-1935  scribe-claim-sweep  buckets=10
2026-05-02  sweep-20260502-1935  scribe-claim  bucket=boot_crt_env  rvas=9  (sweep)
2026-05-02  sweep-20260502-1935  scribe-release  bucket=boot_crt_env  writes=9  errors=0  (sweep)
2026-05-02  sweep-20260502-1935  scribe-claim  bucket=input_dinput  rvas=2  (sweep)
2026-05-02  sweep-20260502-1935  scribe-release  bucket=input_dinput  writes=2  errors=0  (sweep)
2026-05-02  sweep-20260502-1935  scribe-claim  bucket=rw_engine_teardown_d2  rvas=5  (sweep)
2026-05-02  sweep-20260502-1935  scribe-release  bucket=rw_engine_teardown_d2  writes=5  errors=0  (sweep)
2026-05-02  sweep-20260502-1935  scribe-halt  bucket=render_d3d9_device  rva=0x004c7a70  reason='function_at returned: no function found at 004c7a70 (FPO prologue, undefined function)'
2026-05-02  sweep-20260502-1935  scribe-release-sweep  buckets=3  total_writes=16  errors=1  (halted at render_d3d9_device; 6 buckets remain queued)
2026-05-02  sweep-20260502-1941  scribe-claim-sweep  buckets=7  (render_d3d9_device reordered last)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=rw_engine_init_d2  rvas=18  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=rw_engine_init_d2  writes=18  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=audio_rws_loader  rvas=17  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=audio_rws_loader  writes=17  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=input_lua  rvas=4  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=input_lua  writes=4  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=boot_crt_exit_d3  rvas=18  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=boot_crt_exit_d3  writes=18  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=save_gamesave  rvas=6  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=save_gamesave  writes=6  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=boot_app_init_d3  rvas=18  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=boot_app_init_d3  writes=18  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-halt  bucket=render_d3d9_device  rva=0x004c7a70  reason='no function found at 004c7a70 (FPO; expected per prior session)'
2026-05-02  sweep-20260502-1941  scribe-release-sweep  buckets=6  total_writes=81  errors=1  (halted at render_d3d9_device; 1 bucket remains queued — needs function definitions before scribe can process)
2026-05-02  video_mci-20260502-1943  analysis  bucket=video_mci  rvas=9  VIDEO_PLAY_FN=0x004944c0 (DirectShow/COM, not MCI); FUN_00494c80=small.mpg variant; 7 callees; U-0367..U-0371; S-0373..S-0376; D-1000..D-1002
2026-05-02  hud_frontend-20260502-1944  scribe-claim  bucket=hud_frontend  rvas=15  slot=Mashed_pool1  (queued; master.WIP cleared at claim)
2026-05-02  0043c5b0  FUN_0043c5b0          C0->C1  FRONTEND_FN: menu-item table dispatcher; phase gated by DAT_0067eca4; 3301 bytes; U-0447..U-0449 (re/analysis/hud_frontend/0x0043c5b0.md)
2026-05-02  0040bb50  FUN_0040bb50          C0->C1  asset lookup wrapper; calls FUN_004c5c00(DAT_0063b8fc, name); 20 bytes; U-0450; S-0440 (re/analysis/hud_frontend/0x0040bb50.md)
2026-05-02  00427e00  FUN_00427e00          C0->C1  sprite draw 6-param; full pipeline: select/prepare/state9/pos/vtxcol/draw/cleanup; U-0451; S-0441..S-0448 (re/analysis/hud_frontend/0x00427e00.md)
2026-05-02  00428140  FUN_00428140          C0->C1  sprite draw 7-param with vertical alpha gradient; dim-mode DAT_008990e4; S-0441..S-0448 (re/analysis/hud_frontend/0x00428140.md)
2026-05-02  0042aad0  FUN_0042aad0          C0->C1  dim-setter: writes 0x30 to in_EAX+3; sets DAT_008990e4=1; 14 bytes; U-0452 U-0453 (re/analysis/hud_frontend/0x0042aad0.md)
2026-05-02  0042aae0  FUN_0042aae0          C0->C1  RwIm2D fullscreen quad; 4-vert buf 0x0067ec30; alpha=DAT_0067eca8; U-0454..U-0456 (re/analysis/hud_frontend/0x0042aae0.md)
2026-05-02  0042b930  FUN_0042b930          C0->C1  frontend sub-state getter: returns DAT_0067ecb0; 5 bytes (re/analysis/hud_frontend/0x0042b930.md)
2026-05-02  0042e3a0  FUN_0042e3a0          C0->C1  menu chrome drawer: bands+borders at Y=64/416; amber ticks; scroll animation; S-0449..S-0451 (re/analysis/hud_frontend/0x0042e3a0.md)
2026-05-02  0042e5b0  FUN_0042e5b0          C0->C1  frontend BG+logo animation: 512-frame cycle; slide-in via DAT_008990e0; S-0452..S-0455 (re/analysis/hud_frontend/0x0042e5b0.md)
2026-05-02  0043bf30  FUN_0043bf30          C0->C1  frontend sub-menu dispatcher: 14 flag→callee pairs; all callees D-1240..D-1253 (re/analysis/hud_frontend/0x0043bf30.md)
2026-05-02  00472c60  FUN_00472c60          C0->C1  RwIm2D filled-quad: 4-vert; coord-scaled; shared buf 0x00898a20; S-0456 S-0457 (re/analysis/hud_frontend/0x00472c60.md)
2026-05-02  00472dc0  FUN_00472dc0          C0->C1  RwIm2D filled-triangle: 3-vert; same coord-scale; prim-count=3 (re/analysis/hud_frontend/0x00472dc0.md)
2026-05-02  00473540  FUN_00473540          C0->C1  RwIm2D gradient-quad: 4-vert; split per-vertex ARGB; U-0457 (re/analysis/hud_frontend/0x00473540.md)
2026-05-02  004739f0  FUN_004739f0          C0->C1  RwIm2D textured-quad: 12-param; UV at vert+20..+24; state9 conditional; U-0458 U-0459 (re/analysis/hud_frontend/0x004739f0.md)
2026-05-02  004a2c48  FUN_004a2c48          C0->C1  FPU ROUND(ST0)->ulonglong; banker's rounding; no callees; frame-counter source; U-0460 (re/analysis/hud_frontend/0x004a2c48.md)
2026-05-02  00418560  FUN_00418560  C0->C1  session ai_update-20260502-1952; decomp + depth-1 callees; per-vehicle AI dispatcher
2026-05-02  00407a40  FUN_00407a40  C0->C1  session ai_update-20260502-1952; depth-1 callee of 00418560; getter 0x8a9640+v*0x30c
2026-05-02  0040e350  FUN_0040e350  C0->C1  session ai_update-20260502-1952; depth-1 callee; game-mode getter DAT_0063ba8c
2026-05-02  0040e4a0  FUN_0040e4a0  C0->C1  session ai_update-20260502-1952; depth-1 callee; elapsed-time getter DAT_005f29b8
2026-05-02  00413fe0  FUN_00413fe0  C0->C1  session ai_update-20260502-1952; depth-1 callee; AI state reset 4 vehicles stride 0x74
2026-05-02  00416250  FUN_00416250  C0->C1  session ai_update-20260502-1952; depth-1 callee; primary AI control step; behavior mode 0-10
2026-05-02  00416a30  FUN_00416a30  C0->C1  session ai_update-20260502-1952; depth-1 callee; AI control step mode-4/9 variant
2026-05-02  00417180  FUN_00417180  C0->C1  session ai_update-20260502-1952; depth-1 callee; AI spline-bank switcher
2026-05-02  00417640  FUN_00417640  C0->C1  session ai_update-20260502-1952; depth-1 callee; post-step powerup override
2026-05-02  00417da0  FUN_00417da0  C0->C1  session ai_update-20260502-1952; depth-1 callee; AI control step mode-8 variant
2026-05-02  00426c00  FUN_00426c00  C0->C1  session ai_update-20260502-1952; depth-1 callee; powerup-state getter DAT_00644158
2026-05-02  sweep-20260502-2109  scribe-claim-sweep  buckets=5  (render_d3d9_device deferred)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=powerups  rvas=11  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=powerups  writes=11  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=hud_frontend  rvas=15  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=hud_frontend  writes=15  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=ai_update  rvas=11  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=ai_update  writes=11  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=video_mci  rvas=9  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=video_mci  writes=9  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=track_loader  rvas=16  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=track_loader  writes=16  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-halt  bucket=render_d3d9_device  rva=0x004c7a70  reason='no function found at 004c7a70 (FPO; expected; needs function_create before sweep)'
2026-05-02  sweep-20260502-2109  scribe-release-sweep  buckets=5  total_writes=62  errors=1  (halted at render_d3d9_device — last bucket; only render_d3d9_device remains queued)
2026-05-02  005b9f30  LAB_005b9f30          C0->C1  fn-ptr constructor; 4 fn-ptrs to audio obj; FUN_005a9e10 + FUN_005aee20 callees; U-0347..U-0350; S-0340..S-0343 (re/analysis/audio_dsound/0x005b9f30.md)
2026-05-02  005a9e10  FUN_005a9e10          C0->C1  two-call dispatcher; 33 bytes; S-0344 S-0345; U-0351 (re/analysis/audio_dsound/0x005a9e10.md)
2026-05-02  005aee20  FUN_005aee20          C0->C1  bit-scan-forward 0..31; 27 bytes; U-0352 (re/analysis/audio_dsound/0x005aee20.md)
2026-05-02  005ba1d0  LAB_005ba1d0          C0->C1  unrecognized; DirectSound init path A; listing-only; U-0353..U-0355; S-0346..S-0351; D-0940 (re/analysis/audio_dsound/0x005ba1d0.md)
2026-05-02  005bad30  LAB_005bad30          C0->C1  unrecognized; DirectSound init path B; listing-only; 459 bytes; U-0356..U-0359; S-0352; D-0941..D-0951 (re/analysis/audio_dsound/0x005bad30.md)
2026-05-02  audio_dsound-20260502-1942  session-analysis  bucket=audio_dsound  rvas=5  slot=Mashed_pool1  U-0347..U-0359  S-0340..S-0352  D-0940..D-0951
2026-05-02  sweep-20260502-2131  scribe-claim-sweep  buckets=1  (render_d3d9_device deferred)
2026-05-02  sweep-20260502-2131  scribe-claim  bucket=audio_dsound  rvas=5  (sweep)
2026-05-02  sweep-20260502-2131  scribe-release-partial  bucket=audio_dsound  writes=3  errors=2  (drained 0x005b9f30,0x005a9e10,0x005aee20; halted at 0x005ba1d0 [no function found], 0x005bad30 [no function found] — FPO undefined)
2026-05-02  sweep-20260502-2131  scribe-release-sweep  buckets=0_full+1_partial  total_writes=3  errors=2  (audio_dsound partial; 0x005ba1d0+0x005bad30 + render_d3d9_device remain queued — all need function_create before next sweep)
2026-05-02  fixup-20260502-2148  function_create  rvas=14  bucket=render_d3d9_device+audio_dsound_FPO  (disassemble_seed + function_create on FPO entries; ready for sweep)
2026-05-02  sweep-20260502-2140  scribe-claim-sweep  buckets=2  (post-fixup)
2026-05-02  sweep-20260502-2140  scribe-claim  bucket=audio_dsound  rvas=5  (sweep, post-fixup completion)
2026-05-02  sweep-20260502-2140  scribe-release  bucket=audio_dsound  writes=5  errors=0  (sweep)
2026-05-02  sweep-20260502-2140  scribe-claim  bucket=render_d3d9_device  rvas=12  (sweep, post-fixup)
2026-05-02  sweep-20260502-2140  scribe-release  bucket=render_d3d9_device  writes=12  errors=0  (sweep)
2026-05-02  sweep-20260502-2140  scribe-release-sweep  buckets=2  total_writes=17  errors=0  (queue empty)
2026-05-02 audio_music-20260502-2145 analysis bucket=audio_music rvas=4 (004623e0,0045da60,0045dd60,004631f0) stubs=S-0620..S-0631 uncertainties=U-0627..U-0641 deferred=D-1780..D-1791
2026-05-02 effects_particle-20260502-2135 analysis bucket=effects_particle rvas=16
2026-05-02 hud_ingame-20260502-2132 analysis bucket=hud_ingame rvas=14 (0040dfc0,00403160,0041a3e0,0041b630,0041c0c0,0041c300,0041ccc0,0041d870,0041db80,0041ded0,0041e850,00426ba0,0042f500,0042f6a0) stubs=S-0560..S-0572 uncertainties=U-0567..U-0579 deferred=D-1600
2026-05-02 game_state-20260502-2144 analysis bucket=game_state rvas=20 (STATE_FN 0x004929d0 + callees 0x0040b430..0x0042c220) stubs=S-0480..S-0494 uncertainties=U-0487..U-0504 deferred=D-1360
2026-05-02  0x0040b090  FUN_0040b090  C0->C1  camera_follow-20260502-2132: per-slot camera dispatch, 4-player outer loop, color-token inner dispatch
2026-05-02  0x0041e8c0  FUN_0041e8c0  C0->C1  camera_follow-20260502-2132: 8-byte indirect tail-call via DAT_0063d7e4+0x18
2026-05-02  0x0041e9b0  FUN_0041e9b0  C0->C1  camera_follow-20260502-2132: 20-byte bool comparator DAT_0063d7e4+0x10 == param_1
2026-05-02  0x0041e9e0  FUN_0041e9e0  C0->C1  camera_follow-20260502-2132: 8-byte getter DAT_0063d7e4+0x18
2026-05-02  0x00426700  FUN_00426700  C0->C1  camera_follow-20260502-2132: 124-byte camera path node-list iterator
2026-05-02  0x00426780  FUN_00426780  C0->C1  camera_follow-20260502-2132: 132-byte two-array updater (64+8 entry loops)
2026-05-02  0x00426810  FUN_00426810  C0->C1  camera_follow-20260502-2132: 671-byte camera-path position lerp
2026-05-02  0x00426ab0  FUN_00426ab0  C0->C1  camera_follow-20260502-2132: CAMERA_FN confirmed (131 bytes, per-frame)
2026-05-02  0x004671a0  FUN_004671a0  C0->C1  camera_follow-20260502-2132: 27-byte vehicle-0 getter
2026-05-02  0x00467210  FUN_00467210  C0->C1  camera_follow-20260502-2132: 53-byte vehicle sub-obj getter (*(vehicle+4)+0x10)
2026-05-02  0x00471ec0  FUN_00471ec0  C0->C1  camera_follow-20260502-2132: 642-byte camera-anim trigger checker
2026-05-02  0x0047c160  FUN_0047c160  C0->C1  camera_follow-20260502-2132: 140-byte camera-path node loop
2026-05-02  0x00491490  FUN_00491490  C0->C1  camera_follow-20260502-2132: 18-byte mode dispatcher (DAT_007f108b flag)
2026-05-02  physics_collision-20260502-1900  analysis  bucket=physics_collision  rvas=0x00492e90(C1),0x0047a020(C1)  COLLISION_FN inconclusive — BSP system entirely Lua setup code; RWP37Active vtable not traceable statically; D-1792 filed for Frida tracing; U-0642..0644 filed
2026-05-02  sweep-20260502-2217  scribe-claim-sweep  buckets=7
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=texture_loader  rvas=3  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=texture_loader  writes=3  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=effects_particle  rvas=16  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=effects_particle  writes=16  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=hud_ingame  rvas=14  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=hud_ingame  writes=14  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=camera_follow  rvas=13  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=camera_follow  writes=13  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=audio_music  rvas=4  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=audio_music  writes=4  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=game_state  rvas=20  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=game_state  writes=20  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=physics_collision  rvas=2  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=physics_collision  writes=2  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release-sweep  buckets=7  total_writes=72  errors=0  (queue empty)
2026-05-02  exception_filter-20260502-2221  analysis  bucket=exception_filter  rvas=3  slot=Mashed_pool2
2026-05-02  localization-20260502-2227  analysis  bucket=localization  rvas=4  slot=Mashed_pool11  LOC_INIT_FN=0x004274d0  LOC_FN=0x004274e0
2026-05-02  settings_config-20260502-2222  analysis  bucket=settings_config  rvas=7  slot=Mashed_pool9  CONFIG_LOAD_FN=0x00498950  CONFIG_SAVE_FN=0x004989b0  shared_helper_with_P=no (distinct: P uses FUN_004b3b70+custom_IO; this session uses _fsopen+CRT)  settings_buf=0x00773208 size=512
2026-05-02  timer-20260502-2221  analysis  bucket=timer  rvas=11  slot=Mashed_pool4 (fallback: pool1/2/3 LockException)  TIMER_INIT_FN=0x00495120  TIMER_FN=0x00492d30  QPC_HELPER=0x004950b0  PROFILER=0x004926c0  DAT_QPF_LO=0x00771e70  DAT_QPF_HI=0x00771e74  DAT_FRAME_DELTA=0x007719a8  DAT_60F_AVG=0x0077197c
2026-05-02  window_msgpump-20260502-2232  analysis  bucket=window_msgpump  rvas=5  slot=Mashed_pool7  WNDPROC_FN=LAB_00499820(no-fn)  MSGPUMP_FN=0x00499690  switch-arms=0x1,0x2,0x6,0x100  U-0647,U-0648 filed  S-0640..S-0643 filed  D-1840 filed
2026-05-02  render_d3d_reset-20260502-2221  analysis  bucket=render_d3d_reset  rvas=7  slot=Mashed_pool3  RESET_FN=0x004c9cd0  pre_release=0x004c9ad0  recreate_vb=0x004dc970  recreate_bufs=0x004db3e0  recreate_surfaces=0x004d1e30  recreate_2d=0x004e0920  rs_cache_reset=0x004d6200  D3D_DEVICE=DAT_007d4110  D3DPRESENT_PARAMS=DAT_009120e0  stubs=S-0700..S-0707  uncertainties=U-0707,U-0708  deferred=D-2020..D-2023
2026-05-02  render_lighting-20260502-2221  analysis  bucket=render_lighting  rvas=0  slot=Mashed_pool6  LIGHT_SETUP_FN=not-found  halt=RpLight-anchors-absent  nodeD3D9SubmitNoLight.csl=0x00618598  startlights.dff-loader=0x0041d8b0  D-2200..D-2201 filed  U-0767 filed
2026-05-03  sweep-20260503-0553  scribe-claim-sweep  buckets=8+1skipped (render_lighting HALT, no RVAs)
2026-05-03  fixup-inline  function_create  rvas=2  (0x004af31a, 0x00499820 — FPO; defined inline before drain)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=exception_filter  rvas=3  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=exception_filter  writes=3  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=memory_pool  rvas=2  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=memory_pool  writes=2  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=localization  rvas=4  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=localization  writes=4  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=window_msgpump  rvas=5  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=window_msgpump  writes=5  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=timer  rvas=11  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=timer  writes=11  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=settings_config  rvas=7  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=settings_config  writes=7  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=render_d3d_reset  rvas=7  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=render_d3d_reset  writes=7  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=intro_splash  rvas=13  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=intro_splash  writes=13  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-skip  bucket=render_lighting  reason=HALT=RpLight-anchors-absent  rvas=none  (sweep — moved to Drained without writes)
2026-05-03  sweep-20260503-0553  scribe-release-sweep  buckets=8+1skipped  total_writes=52  errors=0  (queue empty; +2 inline function_create)
2026-05-03  effects_particle_d2-20260503  session-complete  bucket=effects_particle_d2  rvas=4  (00534870 00535700 00535910 00538c80)  stubs-cleared=4  (S-0595 S-0596 S-0597 S-0598)  deferred-dropped=4  (D-1660..D-1663)  deferred-added=4  (D-2800..D-2803)  uncertainties-added=7  (U-0967..U-0973)  slot=Mashed_pool12  anchor=ok
2026-05-03  random_rng-20260503-0601  00534920  FUN_00534920  C0->C1  util; RNG plugin registrar; re/analysis/random_rng/0x00534920.md
2026-05-03  random_rng-20260503-0601  004b44f0  FUN_004b44f0  C0->C1  util; RandomInt(lo,hi) via FUN_00534870; re/analysis/random_rng/0x004b44f0.md
2026-05-03  random_rng-20260503-0601  004b4510  FUN_004b4510  C0->C1  util; RandomFloat(lo,hi) __thiscall via FUN_00534870; re/analysis/random_rng/0x004b4510.md
2026-05-03  game_state_d2-20260503  session-complete  bucket=game_state_d2  rvas=11  (0x0042c280 0x0042c2d0 0x0042c2e0 0x0042c2f0 0x0042f500 0x0042f6a0 0x00432080 0x004331a0 0x00448700 0x004927c0 0x005c9d00)  stubs-cleared=11  (S-0480..S-0490)  stubs-added=5  (S-1000..S-1004)  deferred-dropped=1  (D-1360)  deferred-added=5  (D-2920..D-2924)  uncertainties-added=8  (U-1007..U-1014)  slot=Mashed_pool6  anchor=ok
2026-05-03  track_loader_d2-20260503-0302  batch  C0->C1  16 functions newly mapped from D-1180 (track_loader-cont1 bucket): 00426cd0 0042a8d0 0042f510 00462950 004715a0 00478660 00479330 0047a0f0 0047c0b0 0047c0f0 00480340 00491780 004924c0 00495280 004952f0 004c1b10; D-1180 drained; S-0900..S-0919 filed; U-0907..U-0912 filed; D-2620..D-2645 depth-3 deferred
2026-05-03  hud_frontend_d2-20260503-0559  batch  C0->C1  14 functions cleared from D-1240..D-1253 (hud_frontend-cont1 bucket): 004335f0 0043a610 0042f0c0 0043af10 00434720 00430b90 00431240 004314b0 00431710 0043aa30 0042fb70 0042fe90 00430120 00439210; D-1240..D-1253 cleared; D-2740..D-2782 depth-3 deferred (43 callees); slot=Mashed_pool15; anchor=ok
2026-05-03  0x004671c0  GetOverlayCamera  C0->C1  render_frame-20260503-0611: trivial getter returns DAT_006905b4; paired with GetCamera(0x004671a0); used as overlay camera in FUN_00492e90
2026-05-03  sweep-20260503-0649  scribe-claim-sweep  buckets=3
2026-05-03  sweep-20260503-0649  scribe-claim  bucket=render_frame  rvas=1  (sweep)
2026-05-03  sweep-20260503-0649  scribe-release  bucket=render_frame  writes=1  errors=0  (sweep)
2026-05-03  sweep-20260503-0649  scribe-claim  bucket=hud_ingame_d2  rvas=14  (sweep)
2026-05-03  sweep-20260503-0649  scribe-release  bucket=hud_ingame_d2  writes=14  errors=0  (sweep)
2026-05-03  sweep-20260503-0649  scribe-claim  bucket=track_loader_d2  rvas=16  (sweep)
2026-05-03  sweep-20260503-0649  scribe-release  bucket=track_loader_d2  writes=16  errors=0  (sweep)
2026-05-03  sweep-20260503-0649  scribe-release-sweep  buckets=3  total_writes=31  errors=0  (queue empty)
2026-05-03  exception_filter_d2-20260503  session-complete  bucket=exception_filter_d2  rvas=0  deferred-kept=1  (D-1960 pickup-condition-unmet: CRT reimplementation not required)  slot=Mashed(master,read-only;pool1-lock-stale)  anchor=ok  note=early-finish/cap_count=0
2026-05-03  0042a470  FUN_0042a470  C0->C1  re/analysis/texture_loader_d2/0x0042a470.md  strings:ps2/xbox/pc/gamecube/  caller:0042a530  resolves:S-0600 D-1720  session:texture_loader_d2-20260503-0350
2026-05-03  00496400  sub_00496400  C1(existing)->C1  already-mapped-save  resolves:S-0601 D-1721  session:texture_loader_d2-20260503-0350
2026-05-03  004cc230  FUN_004cc230  C1(existing)->C1  already-mapped-frontend  resolves:S-0421 D-1724  session:texture_loader_d2-20260503-0350
2026-05-03  004cc160  FUN_004cc160  C1(existing)->C1  already-mapped-frontend  resolves:S-0424 D-1726  session:texture_loader_d2-20260503-0350
2026-05-03  004cc5e0  FUN_004cc5e0  C0->C2  re/analysis/texture_loader_d2/0x004cc5e0.md  RwStreamFindChunk pattern; version-range [0x35000..0x37002]  resolves:S-0422 D-1725  new:U-1274 U-1275  session:texture_loader_d2-20260503-0350
2026-05-03  004cf7d0  FUN_004cf7d0  C0->C2  re/analysis/texture_loader_d2/0x004cf7d0.md  RwTexDictionaryStreamRead pattern; struct+dict+loop+AddTexture  resolves:S-0602 D-1722  new:U-1267 U-1268 U-1269  session:texture_loader_d2-20260503-0350
2026-05-03  0054f8d0  FUN_0054f8d0  C0->C1  re/analysis/texture_loader_d2/0x0054f8d0.md  native-texture-bank-reader; corrects prior DFF/clump label  resolves:S-0603 D-1723  new:U-1270 U-1271 U-1272 U-1273  session:texture_loader_d2-20260503-0350
2026-05-03  004b3d80  FUN_004b3d80  C1->C1(notes-correction)  corrected 'DFF/clump' to 'native-texture-bank'; S-0603 resolved  session:texture_loader_d2-20260503-0350
2026-05-03  sweep-20260503-0725  scribe-claim-sweep  buckets=2  (race_results has 3 protected RVAs per row note)
2026-05-03  sweep-20260503-0725  scribe-claim  bucket=race_results  rvas=14  (sweep)
2026-05-03  sweep-20260503-0725  scribe-release  bucket=race_results  writes=11  bookmarks=14  errors=0  skipped_plates=3 (0x0040b6d0,0x0042f500,0x0042f6a0 prior C1 preserved per row note)
2026-05-03  sweep-20260503-0725  scribe-claim  bucket=timer_d2  rvas=13  (sweep)
2026-05-03  sweep-20260503-0725  scribe-release  bucket=timer_d2  writes=13  errors=0  (sweep)
2026-05-03  sweep-20260503-0725  scribe-release-sweep  buckets=2  total_writes=24  bookmarks=27  errors=0  skipped_plates=3  (queue empty)
2026-05-03  0049ec10  FUN_0049ec10  C0->C1  re/analysis/video_mci_d2/0x0049ec10.md  __thiscall ctor; vtable[0]+7 slots; calls FUN_0049dd60+FUN_0049cfb0; min obj 0x128b  resolves:S-0375 D-1000  new:S-1380 U-1387  session:video_mci_d2-20260503
2026-05-03  004a3b84  FUN_004a3b84  C0->C1  re/analysis/video_mci_d2/0x004a3b84.md  vsnprintf-impl via fake FILE (_flag=0x42); FUN_004a504f core  resolves:S-0376 D-1001  session:video_mci_d2-20260503
2026-05-03  00493ac0  LAB_00493ac0  C0->C1  re/analysis/video_mci_d2/0x00493ac0.md  pre-NT5 code-page: GetThreadLocale+GetLocaleInfoA(0x1004)+atoi; fallback GetACP  resolves:S-0373 D-1002  session:video_mci_d2-20260503
2026-05-03  00493b40  LAB_00493b40  C0->C1  re/analysis/video_mci_d2/0x00493ac0.md  NT5+ code-page: MOV EAX,3 (CP_THREAD_ACP); RET  resolves:S-0374  session:video_mci_d2-20260503
2026-05-03  00408af0  FUN_00408af0  C0->C1  re/analysis/ai_update_d2/0x00408af0.md  heading/vel float3 getter (+0x9c stride-0x30c)  resolves:D-1120  session:ai_update_d2-20260503-1322
2026-05-03  00414030  FUN_00414030  C0->C1  re/analysis/ai_update_d2/0x00414030.md  AI spline-bank timer reset  resolves:D-1140 S-0403  session:ai_update_d2-20260503-1322
2026-05-03  00414570  FUN_00414570  C0->C1  re/analysis/ai_update_d2/0x00414570.md  ahead-in-race targeting (progress diff+angle)  resolves:D-1121  new:D-4180..4199 U-1427..1436  session:ai_update_d2-20260503-1322
2026-05-03  004148b0  FUN_004148b0  C0->C1  re/analysis/ai_update_d2/0x004148b0.md  leader-ranking AI timer  resolves:D-1122  session:ai_update_d2-20260503-1322
2026-05-03  00414a70  FUN_00414a70  C0->C1  re/analysis/ai_update_d2/0x00414a70.md  closest-vehicle targeting (returns 1/2)  resolves:D-1123  session:ai_update_d2-20260503-1322
2026-05-03  00414c30  FUN_00414c30  C0->C1  re/analysis/ai_update_d2/0x00414c30.md  obstacle-avoidance targeting  resolves:D-1124  session:ai_update_d2-20260503-1322
2026-05-03  00414f00  FUN_00414f00  C0->C1  re/analysis/ai_update_d2/0x00414f00.md  powerup-seek targeting  resolves:D-1125  session:ai_update_d2-20260503-1322
2026-05-03  00415020  FUN_00415020  C0->C1  re/analysis/ai_update_d2/0x00415020.md  frustration timer (72000-frame mode-5 gate)  resolves:D-1126  session:ai_update_d2-20260503-1322
2026-05-03  004150e0  FUN_004150e0  C0->C1  re/analysis/ai_update_d2/0x004150e0.md  track lateral-zone query (tile grid)  resolves:D-1127  new:U-1427  session:ai_update_d2-20260503-1322
2026-05-03  00415220  FUN_00415220  C0->C1  re/analysis/ai_update_d2/0x00415220.md  AI powerup-activation (13-case switch)  resolves:D-1128  new:U-1429 U-1430  session:ai_update_d2-20260503-1322
2026-05-03  00415880  FUN_00415880  C0->C1  re/analysis/ai_update_d2/0x00415880.md  ram-from-behind targeting (latch)  resolves:D-1129  session:ai_update_d2-20260503-1322
2026-05-03  00415d00  FUN_00415d00  C0->C1  re/analysis/ai_update_d2/0x00415d00.md  wall-ahead trajectory check  resolves:D-1130  session:ai_update_d2-20260503-1322
2026-05-03  00415e20  FUN_00415e20  C0->C1  re/analysis/ai_update_d2/0x00415e20.md  steering angle calculator (ST0 return)  resolves:D-1131  new:U-1428  session:ai_update_d2-20260503-1322
2026-05-03  00416060  FUN_00416060  C0->C1  re/analysis/ai_update_d2/0x00416060.md  line-of-sight check (ray-march)  resolves:D-1132  session:ai_update_d2-20260503-1322
2026-05-03  004161e0  FUN_004161e0  C0->C1  re/analysis/ai_update_d2/0x004161e0.md  spline target-point init  resolves:D-1133  session:ai_update_d2-20260503-1322
2026-05-03  00417cf0  FUN_00417cf0  C0->C1  re/analysis/ai_update_d2/0x00417cf0.md  angle-gated targeting mode-8 variant  resolves:D-1147 S-0410  session:ai_update_d2-20260503-1322
2026-05-03  00443080  FUN_00443080  C0->C1  re/analysis/ai_update_d2/0x00443080.md  mode-6 gate flag getter (DAT_00897ffc)  resolves:D-1134  new:U-1432  session:ai_update_d2-20260503-1322
2026-05-03  00443440  FUN_00443440  C0->C1  re/analysis/ai_update_d2/0x00443440.md  spline progress+curvature calculator  resolves:D-1135  session:ai_update_d2-20260503-1322
2026-05-03  0046d4a0  FUN_0046d4a0  C0->C1  re/analysis/ai_update_d2/0x0046d4a0.md  vehicle struct pointer (base 0x881ec8 stride 0x341)  resolves:D-1136  new:U-1431  session:ai_update_d2-20260503-1322
2026-05-03  0046d510  FUN_0046d510  C0->C1  re/analysis/ai_update_d2/0x0046d510.md  vehicle velocity getter (matrix-transformed)  resolves:D-1137  session:ai_update_d2-20260503-1322
2026-05-03  0046d570  FUN_0046d570  C0->C1  re/analysis/ai_update_d2/0x0046d570.md  vehicle forward-angle projection  resolves:D-1145 S-0408  session:ai_update_d2-20260503-1322
2026-05-03  0046d6a0  FUN_0046d6a0  C0->C1  re/analysis/ai_update_d2/0x0046d6a0.md  physics scalar getter (base 0x8820ac stride 0xd04)  resolves:D-1138  new:U-1434  session:ai_update_d2-20260503-1322
2026-05-03  0046d6d0  FUN_0046d6d0  C0->C1  re/analysis/ai_update_d2/0x0046d6d0.md  vehicle spline-progress rate (+0xbc stride 0x341)  resolves:D-1139  session:ai_update_d2-20260503-1322
2026-05-03  00452160  FUN_00452160  C0->C1  re/analysis/ai_update_d2/0x00452160.md  powerup target position getter  resolves:D-1142 S-0405  session:ai_update_d2-20260503-1322
2026-05-03  00452ea0  FUN_00452ea0  C0->C1  re/analysis/ai_update_d2/0x00452ea0.md  per-vehicle powerup-active flag  resolves:D-1143 S-0406  session:ai_update_d2-20260503-1322
2026-05-03  00452eb0  FUN_00452eb0  C0->C1  re/analysis/ai_update_d2/0x00452eb0.md  powerup pursuit range getter  resolves:D-1144 S-0407  session:ai_update_d2-20260503-1322
2026-05-03  00472650  FUN_00472650  C0->C1  re/analysis/ai_update_d2/0x00472650.md  random float [min,max) via PRNG FUN_00534870  resolves:D-1141 S-0404  session:ai_update_d2-20260503-1322
2026-05-03  004c3ac0  FUN_004c3ac0  C0->C1  re/analysis/ai_update_d2/0x004c3ac0.md  fast 3-vector magnitude (two-level sqrt table)  resolves:D-1146 S-0409  session:ai_update_d2-20260503-1322
2026-05-03  005ba720  FUN_005ba720  C0->C1  re/analysis/audio_dsound_d2/0x005ba720.md  COM init guard CoInitialize/CoUninitialize  resolves:D-0945 S-0346  session:audio_dsound_d2-20260503-1735
2026-05-03  005ba760  FUN_005ba760  C0->C1  re/analysis/audio_dsound_d2/0x005ba760.md  COM cleanup FUN_005bc880+CoUninitialize jumptable-warn  resolves:D-0947 S-0348  session:audio_dsound_d2-20260503-1735
2026-05-03  005ba780  LAB_005ba780  C0->C1  re/analysis/audio_dsound_d2/0x005ba780.md  fn-ptr struct+0x34 no-Ghidra-fn 0x6f-bytes 3-vtable+0x8  resolves:D-0943 S-0342  session:audio_dsound_d2-20260503-1735
2026-05-03  005ba7f0  LAB_005ba7f0  C0->C1  re/analysis/audio_dsound_d2/0x005ba7f0.md  fn-ptr struct+0x30 no-Ghidra-fn 0x24f-bytes flags-dispatch FPU switch-table  resolves:D-0944 S-0343  session:audio_dsound_d2-20260503-1735
2026-05-03  005bac00  FUN_005bac00  C0->C1  re/analysis/audio_dsound_d2/0x005bac00.md  3x vtable+0x14 6-unreachable-blocks  resolves:D-0950 S-0351  session:audio_dsound_d2-20260503-1735
2026-05-03  005bb000  FUN_005bb000  C0->C1  re/analysis/audio_dsound_d2/0x005bb000.md  DSoundBuffer init/teardown WaitForSingleObject  resolves:D-0946 S-0347  session:audio_dsound_d2-20260503-1735
2026-05-03  005bbc10  FUN_005bbc10  C0->C1  re/analysis/audio_dsound_d2/0x005bbc10.md  format/caps query vtable+0x38-retry vtable+0x14  resolves:D-0948 S-0349  session:audio_dsound_d2-20260503-1735
2026-05-03  005bbdb0  FUN_005bbdb0  C0->C1  re/analysis/audio_dsound_d2/0x005bbdb0.md  CreateSoundBuffer-wrapper vtable+0xc flags-0x88/0x8088  resolves:D-0949 S-0350  session:audio_dsound_d2-20260503-1735
2026-05-03  005bbf30  FUN_005bbf30  C0->C1  re/analysis/audio_dsound_d2/0x005bbf30.md  COM-Release-x3 vtable+0x8 on param_1[0..2]  resolves:D-0951 S-0352  session:audio_dsound_d2-20260503-1735
2026-05-03  audio_dsound_d2-20260503-1735  OVERLAP  D-0941 0x005adfe0 + D-0942 0x005ae010 already at C1 in audio_rws_loader_d2; not re-decompiled  session:audio_dsound_d2-20260503-1735
2026-05-03  00403050  FUN_00403050  C0->C1  re/analysis/loading_screen/0x00403050.md  pre-race loading screen renderer; state-1 sub-state-0x21; sprite 0x2A4 pulsing spinner  resolves:D-0891  session:loading_screen-20260503
2026-05-03  0040ab40  FUN_0040ab40  C0->C1  re/analysis/timer_d2_cont1/0x0040ab40.md  state-machine dispatcher 6-var cluster 0x008a9584..9598  session:timer_d2_cont1-20260503-1824
2026-05-03  0040ac80  FUN_0040ac80  C0->C1  re/analysis/timer_d2_cont1/0x0040ac80.md  second dispatcher same 6-var cluster non-contiguous  session:timer_d2_cont1-20260503-1824
2026-05-03  0040b810  FUN_0040b810  C0->C1  re/analysis/timer_d2_cont1/0x0040b810.md  zeroes 21 globals 0x008a94f0/9530/9540+0x0063b8ec  session:timer_d2_cont1-20260503-1824
2026-05-03  0040de10  FUN_0040de10  C0->C1  re/analysis/timer_d2_cont1/0x0040de10.md  calls S-1132+Replay::StartLap+S-1603 writes 0x0063ba8c=1  session:timer_d2_cont1-20260503-1824
2026-05-03  0040e360  FUN_0040e360  C0->C1  re/analysis/timer_d2_cont1/0x0040e360.md  setter DAT_0063ba8c=param_1  session:timer_d2_cont1-20260503-1824
2026-05-03  0040e370  FUN_0040e370  C0->C1  re/analysis/timer_d2_cont1/0x0040e370.md  bool getter PTR_PTR_005f2770+param_1*4+0x34  session:timer_d2_cont1-20260503-1824
2026-05-03  00422b30  FUN_00422b30  C0->C1  re/analysis/timer_d2_cont1/0x00422b30.md  memset 0x138 dwords at 0x00899e80  session:timer_d2_cont1-20260503-1824
2026-05-03  00429aa0  FUN_00429aa0  C0->C1  re/analysis/timer_d2_cont1/0x00429aa0.md  two-path table-fill 0x0067d990/998/9a0  session:timer_d2_cont1-20260503-1824
2026-05-03  0042af50  FUN_0042af50  C0->C1  re/analysis/timer_d2_cont1/0x0042af50.md  three-path char-array walk or int-index walk  session:timer_d2_cont1-20260503-1824
2026-05-03  0042b900  FUN_0042b900  C0->C1  re/analysis/timer_d2_cont1/0x0042b900.md  getter DAT_0067eca4  session:timer_d2_cont1-20260503-1824
2026-05-03  0042b950  FUN_0042b950  C0->C1  re/analysis/timer_d2_cont1/0x0042b950.md  setter DAT_007f1a0c=0x1000  session:timer_d2_cont1-20260503-1824
2026-05-03  0042c150  FUN_0042c150  C0->C1  re/analysis/timer_d2_cont1/0x0042c150.md  counts nonzero ints 0x0067ea10..3f; if any: 0x0067eab4=0xff  session:timer_d2_cont1-20260503-1824
2026-05-03  00431b50  FUN_00431b50  C0->C1  re/analysis/timer_d2_cont1/0x00431b50.md  fsin(DAT_007f0f04*_DAT_005cd8f0) float10  session:timer_d2_cont1-20260503-1824
2026-05-03  00431b60  FUN_00431b60  C0->C1  re/analysis/timer_d2_cont1/0x00431b60.md  fsin(DAT_007f0f08*_DAT_005cd8f0) float10 sibling+4  session:timer_d2_cont1-20260503-1824
2026-05-03  00432290  FUN_00432290  C0->C1  re/analysis/timer_d2_cont1/0x00432290.md  predicate DAT_0067eab0!=0 && DAT_0067eabc in {FF210000,FF220000}  session:timer_d2_cont1-20260503-1824
2026-05-03  0045c480  FUN_0045c480  C0->C1  re/analysis/timer_d2_cont1/0x0045c480.md  zero-init scatter globals + 0x24-dword block 0x0088f5e0  session:timer_d2_cont1-20260503-1824
2026-05-03  0045d3a0  FUN_0045d3a0  C0->C1  re/analysis/timer_d2_cont1/0x0045d3a0.md  gated tick: loop FUN_0045d1e0 0..DAT_008aa254; FUN_004657b0; store  session:timer_d2_cont1-20260503-1824
2026-05-03  0045d7a0  FUN_0045d7a0  C0->C1  re/analysis/timer_d2_cont1/0x0045d7a0.md  701b position-lerp+cross-product writes to iVar6 transform obj  session:timer_d2_cont1-20260503-1824
2026-05-03  timer_d2_cont1-20260503-1824  EARLY-FINISH  cap_count=18 at RVA 18/39; 21 RVAs filed D-4720 (timer_d2_cont2); D-3282 re-deferred as D-4721 (Opus-only); slot=Mashed_pool5  session:timer_d2_cont1-20260503-1824
2026-05-03  00426670  sub_00426670  C0->C1  re/analysis/render_frame_d3/00426670.md  WorldRenderDispatch_Begin; D-0880 resolved; D-5020 D-5021 spawned  session:render_frame_d3-20260503
2026-05-03  0040de30  sub_0040de30  C0->C1  re/analysis/render_frame_d3/0040de30.md  MinimapCameraOrthoSetup; D-0881 resolved  session:render_frame_d3-20260503
2026-05-03  0040df20  sub_0040df20  C0->C1  re/analysis/render_frame_d3/0040df20.md  MinimapCameraRestore; D-0882 resolved  session:render_frame_d3-20260503
2026-05-03  0040df60  sub_0040df60  C0->C1  re/analysis/render_frame_d3/0040df60.md  ConditionalRenderSubPass; D-0883 resolved; D-5024 spawned  session:render_frame_d3-20260503
2026-05-03  00404320  sub_00404320  C0->C1  re/analysis/render_frame_d3/00404320.md  PerModeRenderMachine; D-0884 resolved; D-5025..D-5029 spawned  session:render_frame_d3-20260503
2026-05-03  00410b30  sub_00410b30  C0->C1  re/analysis/render_frame_d3/00410b30.md  InGameRenderDispatcher; D-0885 resolved; D-5037..D-5060 spawned (25 callees)  session:render_frame_d3-20260503
2026-05-03  00426030  sub_00426030  C0->C1  re/analysis/render_frame_d3/00426030.md  WorldRenderPrePass; D-0886 resolved; D-5022 D-5023 spawned  session:render_frame_d3-20260503
2026-05-03  004266b0  sub_004266b0  C0->C1  re/analysis/render_frame_d3/004266b0.md  WorldRenderDispatch_End; D-0887 resolved  session:render_frame_d3-20260503
2026-05-03  00492440  sub_00492440  C0->C1  re/analysis/render_frame_d3/00492440.md  RenderStatsAccumulate (leaf/mapped); D-0888 resolved  session:render_frame_d3-20260503
2026-05-03  00492e60  sub_00492e60  C0->C1  re/analysis/render_frame_d3/00492e60.md  SetDefaultViewWindow; D-0889 resolved  session:render_frame_d3-20260503
2026-05-03  00433f40  sub_00433f40  C0->C1  re/analysis/render_frame_d3/00433f40.md  RaceEndFadeOverlay; D-0890 resolved; D-5030..D-5034 spawned  session:render_frame_d3-20260503
2026-05-03  0042d390  GetRaceStateField  C0->C1  re/analysis/render_frame_d3/0042d390.md  trivial getter DAT_0067ea6c; D-0892 resolved  session:render_frame_d3-20260503
2026-05-03  0042f530  sub_0042f530  C0->C1  re/analysis/render_frame_d3/0042f530.md  ViewportSetup; D-0893 resolved; D-5035 spawned  session:render_frame_d3-20260503
2026-05-03  0042a9f0  GetFadeAlpha  C0->C1  re/analysis/render_frame_d3/0042a9f0.md  trivial getter (byte)DAT_0067eca8; D-0894 resolved  session:render_frame_d3-20260503
2026-05-03  render_frame_d3-20260503  BATCH  14 C0→C1 promotions; 41 new D-5020..D-5060; 10 U-1707..U-1716; 13 S-1700..S-1712; slot=Mashed_pool13 (pool5 orphan-locked)  session:render_frame_d3-20260503
2026-05-03  sweep-20260503-1853  scribe-claim-sweep  buckets=1  (replay_record skipped: no per-RVA files)
2026-05-03  sweep-20260503-1853  scribe-claim  bucket=game_mode  rvas=1  (sweep)
2026-05-03  sweep-20260503-1853  scribe-release  bucket=game_mode  writes=0_plates,1_bookmarks  errors=0  (sweep; plate-skipped:C0)
2026-05-03  sweep-20260503-1853  scribe-release-sweep  buckets=1  total_writes=0_plates,1_bookmarks  errors=0
2026-05-05  00420050  FUN_00420050->PerPlayerViewportRender  C0->C1  re/analysis/split_screen/00420050.md  per-player render; stride 0x2AC; player index 0..3; D-5038 cleared  session:split_screen-20260505
2026-05-05  0042f660  DefaultViewportCameraInit  C0->C1  re/analysis/split_screen/0042f660.md  creates global camera DAT_0067f190; near=0.1f far=180.0f; calls ViewportSetup  session:split_screen-20260505
2026-05-05  0041faf0  VehicleShadowRender  C0->C1  re/analysis/split_screen/0041faf0.md  vehicle shadow billboard; guarded +0x294 bit6; per-player from PerPlayerViewportRender  session:split_screen-20260505
2026-05-05  0041fcc0  TireMarkRender  C0->C1  re/analysis/split_screen/0041fcc0.md  tire-mark decal; wheel transform via +0x26c; colour table DAT_005f5fe8/6088  session:split_screen-20260505
2026-05-05  0042c960  CameraTransitionStateMachine  C0->C1  re/analysis/split_screen/0042c960.md  camera-transition slider DAT_0067ed68; S-1642 cleared; U-1716 resolved  session:split_screen-20260505
2026-05-05  split_screen-20260505  BATCH  5 C0->C1 promotions; 5 new D-5620..D-5624; 6 new U-1907..U-1912; 1 S-1642 cleared; U-1716 resolved; FOUND: split-screen IS present (4-player loop in InGameRenderDispatcher); SPLIT_FN=sub_00410b30; strategy=render_frame_call_graph  session:split_screen-20260505
2026-05-05  0040e180  FUN_0040e180  C0->C1  re/analysis/vehicle_damage_d2/0x0040e180.md  432b collision-pair finder; nested loops cars 0-3; max-distance pair via FUN_004c3ac0; resolves D-1548  session:vehicle_damage_d2-20260505
2026-05-05  00410d10  FUN_00410d10  C0->C1  re/analysis/vehicle_damage_d2/0x00410d10.md  1080b DAMAGE_FN; per-step per-mode collision dispatcher; switch DAT_007f0fd0 cases 4/5/7/8/9/10; sentinel 10.0f; writes DAT_00803320  session:vehicle_damage_d2-20260505
2026-05-05  00442df0  FUN_00442df0  C0->C1  re/analysis/vehicle_damage_d2/0x00442df0.md  6b stub; returns DAT_00898980 as float10; collision impact reader  session:vehicle_damage_d2-20260505
2026-05-05  0046cbb0  FUN_0046cbb0  C0->C1  re/analysis/vehicle_damage_d2/0x0046cbb0.md  47b; per-car state reader (car,&state_out,&extra_out); base DAT_00881f90 stride 0xd04; state 0=alive 2=slide  session:vehicle_damage_d2-20260505
2026-05-05  004922e0  FUN_004922e0  C0->C1  re/analysis/vehicle_damage_d2/0x004922e0.md  81b hit-sound/particle trigger; car→player via DAT_007f1a14; writes 4 fields to DAT_007f1058 stride 0x4c  session:vehicle_damage_d2-20260505
2026-05-05  vehicle_damage_d2-20260505  BATCH  5 C0->C1 promotions; 3 new S-1840..S-1842; 14 new U-1847..U-1860; 7 new D-5440..D-5446; 1 D-1548 cleared; slot=Mashed_pool2 (pool0/pool1 locked at JVM level)  session:vehicle_damage_d2-20260505
2026-05-06  sweep-20260506-0458  scribe-claim-sweep  buckets=1  rvas=5
2026-05-06  sweep-20260506-0458  scribe-claim  bucket=vehicle_damage_d2  rvas=5  (sweep)
2026-05-06  sweep-20260506-0458  scribe-release  bucket=vehicle_damage_d2  writes=5_plates,5_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-0458  scribe-release-sweep  buckets=1  total_writes=5_plates,5_bookmarks  errors=0
2026-05-06  track_loader_d3-20260506  BATCH  27 C0->C1 promotions; cleared S-0900 S-0904..S-0919 (17 stubs); cleared D-2620..D-2645 (26 deferred); 16 new U-1947..U-1962; 16 new D-5740..D-5755; bucket track_loader_d3-cont1; slot=Mashed_pool0; corrections: S-0909 was float3 eq-check (not in-triangle), S-0910 was cross-product (not flag-setter)  session:track_loader_d3-20260506
2026-05-06  0049dd60  FUN_0049dd60  C0->C1  re/analysis/video_mci_d3/0x0049dd60.md  247b __thiscall ctor-base; vtable writes; 3x FUN_004a1160 on offsets 0x54/0x58/0x5c; 3x InitializeCriticalSection; SetEvent on this->0x5c; resolves D-4060 S-1380; new S-2000 S-2001 U-2007 D-5920 D-5921  session:video_mci_d3-20260506-0512
2026-05-06  powerups_d3-20260506-0504  BATCH  17 analyzed (16 new C1 rows; 0x004d8060 already C1); cleared D-4240..D-4243 S-1444; new S-1920..S-1930 (11); new U-1927..U-1945 (19); new D-5680..D-5686 (7); slot=Mashed_pool4; cap_count=17  session:powerups_d3-20260506-0504
2026-05-06  hud_frontend_d3-20260506-0511  BATCH  18 C0->C1 promotions (0040ad20 0040b460 0040b620 0040b6b0 0040b6c0 0040b7a0 0040b7b0 0040bb70 0040bb90 0040e3a0 00427ad0 004282a0 00428320 00429870 00429a30 00429a70 00429a80 00429a90); cleared D-2740..D-2782 (43 deferred, 18 analyzed + 25 re-filed D-6160..D-6184); new S-2087 S-2089 S-2090 (3 new stubs; 9 others already tracked); new U-2087..U-2095 (9); new D-6160..D-6184 (25, hud_frontend_d3-cont1); early-finish at cap=18; slot=Mashed_pool12  session:hud_frontend_d3-20260506-0511
2026-05-06  sweep-20260506-0544  scribe-claim-sweep  buckets=2  rvas=19  (launch_handshake VAs corrected from bare offsets)
2026-05-06  sweep-20260506-0544  scribe-claim  bucket=launch_handshake  rvas=2  (sweep; VAs corrected from bare offsets)
2026-05-06  sweep-20260506-0544  scribe-release  bucket=launch_handshake  writes=2_plates,2_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-0544  scribe-claim  bucket=powerups_d3  rvas=17  (sweep)
2026-05-06  sweep-20260506-0544  scribe-release  bucket=powerups_d3  writes=17_plates,17_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-0544  scribe-release-sweep  buckets=2  total_writes=19_plates,19_bookmarks  errors=0
2026-05-06  vehicle_damage_d3-20260506-1244  BATCH  7 C1 rows (00405890 00408a50 00408a70 0040e340 0040e350 00417730 00423b20); cleared D-5440..D-5446 (7 deferred); resolved S-1842; new U-2167..U-2176 (10); slot=Mashed_pool0; cap_count=7; early-finish=no  session:vehicle_damage_d3-20260506-1244
2026-05-06  00429310  TimeTrial::Tick  C0->C1  string xref "time trial time is %f"; leaderboard-20260506-JJJJJ; re/analysis/leaderboard/0x00429310.md; S-2220 U-2228 filed
2026-05-06  0040d270  Course::Finish  C0->C1  xref from FUN_0040d440 row 311; tail=Replay::CreateOrLoad; leaderboard-20260506-JJJJJ; re/analysis/leaderboard/0x0040d270.md; S-2225 S-2226 U-2229 filed
2026-05-06  sweep-20260506-1326  scribe-claim-sweep  buckets=2  rvas=13
2026-05-06  sweep-20260506-1326  scribe-claim  bucket=profile_career_d3  rvas=6  (sweep)
2026-05-06  sweep-20260506-1326  scribe-release  bucket=profile_career_d3  writes=6_plates,6_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-1326  scribe-claim  bucket=vehicle_damage_d3  rvas=7  (sweep)
2026-05-06  sweep-20260506-1326  scribe-release  bucket=vehicle_damage_d3  writes=7_plates,7_bookmarks  errors=0  (sweep; 0x00408a50/0x00408a70 overwrite prior race_results plates)
2026-05-06  sweep-20260506-1326  scribe-release-sweep  buckets=2  total_writes=13_plates,13_bookmarks  errors=0
2026-05-06  save_gamesave_d2-20260506-1508  analysis  bucket=save_gamesave_d2  rvas=5  new=2(004cbe80,00550b00)  xref=3(004cbd30,004cc160,004cc230)  stubs_cleared=S-0280..S-0284  stubs_added=S-2320  U_added=U-2327..U-2332  D_added=D-6880  slot=Mashed_pool8

2026-05-06  0x00431d90  FUN_00431d90  C0->C1  options_menu-20260506; panel-flag mass-clear; plate re/analysis/options_menu/0x00431d90.md
2026-05-06  0x00431f30  FUN_00431f30  C0->C1  options_menu-20260506; page-ID dispatch switch; param_1=10=options page; plate re/analysis/options_menu/0x00431f30.md
2026-05-06  0x0043df00  FUN_0043df00  C0->C1  options_menu-20260506; frontend game-session initializer; S-2380 U-2387..U-2389; plate re/analysis/options_menu/0x0043df00.md2026-05-06  sweep-20260506-1624  scribe-claim-sweep  buckets=3  rvas=11  (replay_record skipped: still HOLD)
2026-05-06  sweep-20260506-1624  scribe-claim  bucket=options_menu  rvas=3  (sweep)
2026-05-06  sweep-20260506-1624  scribe-release  bucket=options_menu  writes=3_plates,3_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-1624  scribe-claim  bucket=memory_pool_d2  rvas=1  (sweep)
2026-05-06  sweep-20260506-1624  scribe-release  bucket=memory_pool_d2  writes=1_plate,1_bookmark  errors=0  (sweep; _longjmp already named via library match)
2026-05-06  sweep-20260506-1624  scribe-claim  bucket=intro_splash_d2  rvas=7  (sweep)
2026-05-06  sweep-20260506-1624  scribe-release  bucket=intro_splash_d2  writes=7_plates,7_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-1624  scribe-release-sweep  buckets=3  total_writes=11_plates,11_bookmarks  errors=0
2026-05-06  0047b8a0  FUN_0047b8a0  C0->C1  structural read; input_lua_d2-20260506-1854; Lua exec wrapper; FUN_004b7a70(DAT_006bf1e0,p1,p2,0); cond write to *p3; returns iVar1==0; S-2400 U-2407 (re/analysis/input_lua_d2/0x0047b8a0.md)
2026-05-06  004b7330  FUN_004b7330  C0->C1  structural read; input_lua_d2-20260506-1854; alloc 0x70 block; zero-init 16 fields; sentinels +0x4c/+0x58=0xffffffff +0x5c=0x7ffffffd +0x60=0x70 +0x6c=1; setup via FUN_004b7be0; fail→FUN_004b7480; +0x5c=+0x60*2; S-2401 S-2402 U-2408..U-2411 (re/analysis/input_lua_d2/0x004b7330.md)
2026-05-06  004c0510  FUN_004c0510  C0->C1  structural read; input_lua_d2-20260506-1854; 5-call sequential chain into param_1; FUN_004c06c0+thunk_FUN_004b7fd0+FUN_004b9730+FUN_004b7140(0x54442d18,0x400921fb)+FUN_004b7250; S-2403..S-2407 U-2412 U-2413 (re/analysis/input_lua_d2/0x004c0510.md)
2026-05-06  004b7480  FUN_004b7480  C0->C1  structural read; input_lua_d2-20260506-1854; teardown; FUN_004ba210(1)+FUN_004b9850; arith updates on +0x60 via 5x FUN_004ba1b0 pattern; S-2408 S-2409 U-2414..U-2417 (re/analysis/input_lua_d2/0x004b7480.md)
2026-05-06  004b6520  FUN_004b6520  C0->C1  structural read; input_lua_d2-20260506-1854; 20-byte wrapper; FUN_004b64e0(param_1,0,param_2); S-2410 U-2418 (re/analysis/input_lua_d2/0x004b6520.md)
2026-05-06  audio_sfx_dispatch-20260506  analysis  bucket=audio_sfx_dispatch  rvas=8  new=8(004669b0,0045d460,004627f0,004625b0,00462dd0,005a6710,0045e040,00466a50)  U_added=U-2487..U-2494  D_added=D-7360..D-7375(16)  slot=Mashed_pool4  cap_count=10  early-finish=no  session=audio_sfx_dispatch-20260506
2026-05-06  title_screen-20260506-VVVVV  analysis  bucket=title_screen  rvas=7  new=7(00428a30,00429240,00429290,00428bf0,00428d30,00427c90,00428390)  stubs_added=S-2460..S-2466  U_added=U-2467..U-2470  D_added=D-7300..D-7306  slot=Mashed_pool15  cap_count=0  early-finish=no  session=title_screen-20260506-VVVVV
2026-05-06  0x00428a30  FUN_00428a30  C0->C1  title_screen-20260506-VVVVV; TITLE_FN; title screen renderer; string 0x222 at (580.0,140.0); attract timer 24M ticks→DAT_0067d960=1; S-2460 S-2461 S-2462 U-2467 U-2470; plate re/analysis/title_screen/0x00428a30.md
2026-05-06  0x00429240  FUN_00429240  C0->C1  title_screen-20260506-VVVVV; state dispatcher 50b; switch DAT_0067d960; plate re/analysis/title_screen/0x00429240.md
2026-05-06  0x00429290  FUN_00429290  C0->C1  title_screen-20260506-VVVVV; per-frame tick 39b; called ×3 from FUN_004669b0; plate re/analysis/title_screen/0x00429290.md
2026-05-06  0x00428bf0  FUN_00428bf0  C0->C1  title_screen-20260506-VVVVV; attract mode renderer 312b; S-2463 U-2469; plate re/analysis/title_screen/0x00428bf0.md
2026-05-06  0x00428d30  FUN_00428d30  C0->C1  title_screen-20260506-VVVVV; lobby renderer 1291b; S-2464 S-2465 S-2466; plate re/analysis/title_screen/0x00428d30.md
2026-05-06  0x00427c90  FUN_00427c90  C0->C1  title_screen-20260506-VVVVV; assets-ready getter 5b; returns DAT_0067d84c; plate re/analysis/title_screen/0x00427c90.md
2026-05-06  0x00428390  FUN_00428390  C0->C1  title_screen-20260506-VVVVV; state setter 9b; DAT_0067d960=param_1; plate re/analysis/title_screen/0x00428390.md
2026-05-06  audio_music_d3-20260506-1930  scribe-queued  bucket=audio_music_d3  rvas=2
2026-05-06  004288a0  FUN_004288a0  C0->C1  title_screen_d2-20260506; menu layout renderer; 8 elements (image+7 sprites); re/analysis/title_screen_d2/0x004288a0.md; S-2461 cleared
2026-05-06  00428450  FUN_00428450  stub-S-2460-cleared  title_screen_d2-20260506; already C1 (hud_ingame_d3); D-7300 resolved; U-2470 resolved (param_1=X_offset param_2=Y_offset)
2026-05-06  00428320  FUN_00428320  stub-S-2462-cleared  title_screen_d2-20260506; already C1 (hud_frontend_d3); D-7302 resolved
2026-05-06  0042e590  FUN_0042e590  C0->C1  title_screen_d2-20260506; 2-insn wrapper→FUN_0040bb70(C1)→FUN_004c5c00 string search; re/analysis/title_screen_d2/0x0042e590.md; U-2547 filed; S-2463 cleared; S-2540 new
2026-05-06  0040d250  FUN_0040d250  C0->C1  title_screen_d2-20260506; indexed ptr dereference getter 16b; re/analysis/title_screen_d2/0x0040d250.md; S-2464 cleared
2026-05-06  00401ee0  FUN_00401ee0  C0->C1  title_screen_d2-20260506; object-select+RW-matrix+RpClumpRender chain; re/analysis/title_screen_d2/0x00401ee0.md; U-2548 filed; S-2465 cleared; S-2541..S-2544 new
2026-05-06  0042f0b0  FUN_0042f0b0  C0->C1  title_screen_d2-20260506; int getter DAT_0067f17c+0x49; re/analysis/title_screen_d2/0x0042f0b0.md; S-2466 cleared
2026-05-06  004c5c00  FUN_004c5c00  C0->C1  title_screen_d2-20260506; case-insensitive linked-list string search 114b; re/analysis/title_screen_d2/0x0042e590.md (inline); S-2540
2026-05-06  00401570  FUN_00401570  C0->C1  title_screen_d2-20260506; table scan 36b; re/analysis/title_screen_d2/0x00401ee0.md (inline); S-2541
2026-05-06  00401da0  FUN_00401da0  C0->C1  title_screen_d2-20260506; RW matrix setup+dirty 308b; re/analysis/title_screen_d2/0x00401ee0.md (inline); S-2542 S-2543
2026-05-06  00426cf0  FUN_00426cf0  C0->C1  title_screen_d2-20260506; addr getter 5b; re/analysis/title_screen_d2/0x00401ee0.md (inline); S-2544
2026-05-06  004c1480  FUN_004c1480  C0->C1  title_screen_d2-20260506; RW frame dirty-list insert 145b; re/analysis/title_screen_d2/0x00401ee0.md (inline); S-2543; D-7540 new (FUN_004c52f0)
2026-05-06  004b3c60  FUN_004b3c60  stub->C1  track_loader_d4-20260506-2007; BSP stream reader 81b; chunk 0xb; S-2480..S-2483 U-2487 U-2488; re/analysis/track_loader_d4/0x004b3c60.md
2026-05-06  00558df0  FUN_00558df0  stub->C1  track_loader_d4-20260506-2007; UVAnim loader 577b; S-2484..S-2494 U-2489 U-2490; re/analysis/track_loader_d4/0x00558df0.md
2026-05-06  004b3cc0  FUN_004b3cc0  stub->C1  track_loader_d4-20260506-2007; spline stream reader 81b; chunk 0xc; S-2495 U-2491; re/analysis/track_loader_d4/0x004b3cc0.md
2026-05-06  004b3de0  FUN_004b3de0  stub->C1  track_loader_d4-20260506-2007; anim stream reader 81b; chunk 0x1b; S-2496 U-2492; re/analysis/track_loader_d4/0x004b3de0.md
2026-05-06  00479030  LAB_00479030  stub->C0  track_loader_d4-20260506-2007; post-load callback 174b; no Ghidra fn object; needs function_create; U-2493 U-2494; re/analysis/track_loader_d4/0x00479030.md
2026-05-06  00474fb0  FUN_00474fb0  stub->C1  track_loader_d4-20260506-2007; DFF clump iterator 27b; &LAB_00474f90 callback; U-2495 U-2496; re/analysis/track_loader_d4/0x00474fb0.md
2026-05-06  00474f30  FUN_00474f30  stub->C1  track_loader_d4-20260506-2007; per-node callback 31b; FUN_00552020 test; U-2497; re/analysis/track_loader_d4/0x00474f30.md
2026-05-06  0047f4c0  FUN_0047f4c0  stub->C1  track_loader_d4-20260506-2007; physics world ctor 522b; AABB ±1000.0f; U-2498 U-2499; re/analysis/track_loader_d4/0x0047f4c0.md
2026-05-06  0047d080  FUN_0047d080  stub->C1  track_loader_d4-20260506-2007; activate physics body 104b; DAT_006c71d8 array; U-2500 U-2501; re/analysis/track_loader_d4/0x0047d080.md
2026-05-06  0047d100  FUN_0047d100  stub->C1  track_loader_d4-20260506-2007; secondary enable 35b; FUN_004b5240 no-arg; U-2502; re/analysis/track_loader_d4/0x0047d100.md
2026-05-06  00487280  FUN_00487280  stub->C1  track_loader_d4-20260506-2007; broadphase registration 2678b; 14 callees; U-2503 U-2504; re/analysis/track_loader_d4/0x00487280.md
2026-05-06  0047be80  FUN_0047be80  stub->C1  track_loader_d4-20260506-2007; triangle normal calc 234b; __fastcall; cross product; U-2505; re/analysis/track_loader_d4/0x0047be80.md
2026-05-06  0047bcc0  FUN_0047bcc0  stub->C1  track_loader_d4-20260506-2007; half-edge adj builder 433b; in_EAX=tri_count; U-2506; re/analysis/track_loader_d4/0x0047bcc0.md
2026-05-06  004b53b0  FUN_004b53b0  stub->C1  track_loader_d4-20260506-2007; bounding sphere 430b; max-sq-dist; sqrt; re/analysis/track_loader_d4/0x004b53b0.md
2026-05-06  004c3d90  FUN_004c3d90  stub->C1  track_loader_d4-20260506-2007; dispatch shim 42b; DAT_007d3ffc+0xc+DAT_007d3ff8; re/analysis/track_loader_d4/0x004c3d90.md
2026-05-06  00546380  FUN_00546380  stub->C1  track_loader_d4-20260506-2007; waypoint set ctor 426b; type 1/2; FUN_00545340 init; re/analysis/track_loader_d4/0x00546380.md
2026-05-06  sweep-20260506-2157  scribe-claim  bucket=sweep-multi rvas=23 (input_lua_d2=5, audio_music_d3=2, track_loader_d4=16; replay_record HOLD-skipped)
2026-05-06  input_lua_d2-20260506-1854  scribe-claim  bucket=input_lua_d2 rvas=5
2026-05-06  input_lua_d2-20260506-1854  scribe-release bucket=input_lua_d2 writes=10 errors=0  drained-by=sweep-20260506-2157; 5 plates, 5 bookmarks, 0 renames
2026-05-06  audio_music_d3-20260506-1930  scribe-claim  bucket=audio_music_d3 rvas=2
2026-05-06  audio_music_d3-20260506-1930  scribe-release bucket=audio_music_d3 writes=4 errors=0  drained-by=sweep-20260506-2157; 2 plates, 2 bookmarks, 0 renames
2026-05-06  track_loader_d4-20260506-2007  scribe-claim  bucket=track_loader_d4 rvas=16
2026-05-06  track_loader_d4-20260506-2007  scribe-release bucket=track_loader_d4 writes=32 errors=0  drained-by=sweep-20260506-2157; 16 plates, 16 bookmarks, 0 renames; 0x00479030 listing-level (no function object)
2026-05-06  sweep-20260506-2157  scribe-skip  bucket=replay_record reason="HOLD missing-per-rva-files" (left in Queued; not drained)
2026-05-06  sweep-20260506-2157  scribe-release bucket=sweep-multi writes=46 errors=0 buckets_drained=3 buckets_skipped=1 (input_lua_d2=10, audio_music_d3=4, track_loader_d4=32)
2026-05-07  sweep-20260507-0133  scribe-claim  buckets=1 queued, 1 skipped-HOLD
2026-05-07  physics_collision_d2-20260507-0106  scribe-release  bucket=physics_collision_d2  writes=6  errors=0
2026-05-07  sweep-20260507-0133  scribe-skip  bucket=replay_record reason="HOLD missing-per-rva-files" (left in Queued; not drained)
2026-05-07  sweep-20260507-0133  scribe-release bucket=sweep-multi writes=6 errors=0 buckets_drained=1 buckets_skipped=1 (physics_collision_d2=6)
2026-05-07  sweep-20260507-1913  scribe-claim  buckets=1 queued, 1 skipped-HOLD
2026-05-07  sweep-20260507-1913  scribe-release  bucket=sky_weather_d2  writes=12  errors=0
2026-05-07  sweep-20260507-1913  scribe-release  bucket=race_results_d2  writes=5  errors=0
2026-05-07  sweep-20260507-1913  scribe-release  buckets=2 drained  errors=0
2026-05-07 librw_plugin_compat-20260507-1950 analysis bucket=librw_plugin_compat rvas=28 (callback addresses) + FUN_004d7de0 (re-read; already C1) U-2887..U-2891 D-8560 slot=Mashed_pool12
2026-05-07 piz_fsmanager_handler-20260507 analysis bucket=piz_fsmanager_handler rvas=20 (12 RtFSHandler vtable entries + RtFSHandler::Install + RtFSManager::FindHandler + PizWin32Open + PizWin32Read + PizOpenAndParse + PizOpen + OpenPizFile + ClosePizFile) shim_verdict=PathC (no .piz integration via FSManager; .piz bypass sites: 0x004b6710 CreateFileA + 0x004b67e0 ReadFile) U-2907..U-2909 slot=Mashed_pool13

2026-05-07  sweep-20260507-2002  scribe-claim  buckets=23 queued, 1 skipped-HOLD
2026-05-07  sweep-20260507-2002  scribe-release  bucket=effects_particle_d3  writes=4  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=game_state_d3  writes=5  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=game_state_d4  writes=3  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=hud_ingame_d3  writes=2  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=loading_screen  writes=1  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=hud_frontend_d2  writes=14  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=input_dinput_d2  writes=30  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=render_d3d_reset_d2  writes=14  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=render_frame_d3  writes=15  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=render_pipeline_d2  writes=1  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=audio_sfx_dispatch_d2  writes=14  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=render_pipeline_d3  writes=10  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=rw_engine_init_d3  writes=19  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=rw_engine_teardown_d3  writes=1  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=texture_loader_d2  writes=4  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=texture_loader_d3  writes=12  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=title_screen_d2  writes=5  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=track_loader_d3  writes=19  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=vehicle_dynamics  writes=4  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=vehicle_update_d2  writes=10  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=vehicle_update_d3  writes=8  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=video_mci_d2  writes=3  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=window_fullscreen  writes=1  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=input_lua_d3  writes=5  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  buckets=19 drained  errors=0

2026-05-07  sweep-20260507-2353  scribe-claim  buckets=17 queued, 1 skipped-HOLD
2026-05-07  sweep-20260507-2353  scribe-release  buckets=0 drained (no-op: sweep-20260507-2002 had already completed all 17 drainable buckets; 1 HOLD replay_record)  errors=0

2026-05-08  sweep-20260508-0358  scribe-claim  buckets=8 queued, 1 skipped-HOLD
2026-05-08  sweep-20260508-0358  scribe-release  bucket=audio_dsound_d3  writes=15  errors=0
2026-05-08  sweep-20260508-0358  scribe-release  bucket=game_state_d5  writes=5  errors=0
2026-05-08  sweep-20260508-0358  scribe-release  bucket=game_state_d5-cont1  writes=2  errors=0
2026-05-08  sweep-20260508-0358  scribe-release  bucket=render_frame_d4  writes=20  errors=0
2026-05-08  sweep-20260508-0358  scribe-release  bucket=settings_config_d3  writes=8  errors=0  note=0x004991f0 written as listing-level (no Ghidra function object — C0 plate)
