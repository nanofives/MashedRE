# Stubs

Every time a reimplementation calls a function that has not itself been reversed, the placeholder is recorded here. **Stubs block S-DoD.** A subsystem cannot be marked DONE while it has open stubs.

A "stub" is one of:
- A passthrough that calls the original at its RVA (`call_original_0x00xxxxxx()`).
- A no-op that returns a hardcoded value.
- A function with the body `assert(0 && "TODO");`.

Each stub gets one row. Resolve by reversing the target function (preferred) or by promoting the stub to `DEFERRED.md` with a wontfix rationale (rare).

## Active stubs

| ID | RVA called | Caller (RVA / name) | Subsystem | Type | Inserted | Notes |
|----|-----------|---------------------|-----------|------|----------|-------|
| S-0001 | 0x00499730 | 0x00493900 sub_00493900 | boot | passthrough | 2026-05-02 | Returns command-line string pointer; called as FUN_00499730(); depth-3 |
| S-0002 | 0x004a3ac9 | 0x00493900 sub_00493900 | boot | passthrough | 2026-05-02 | strtok-like tokenizer; takes (str, delimiter); depth-3 |
| S-0003 | 0x00499890 | 0x00499ba0 sub_00499ba0 | boot | passthrough | 2026-05-02 | Called after CreateWindowExA; no visible args; depth-3 |
| S-0004 | 0x004c5a60 | 0x004c5930 sub_004c5930 | boot | passthrough | 2026-05-02 | Linked-list traversal helper; takes (ptr-2, 0); depth-3 |
| S-0005 | 0x004d8060 | 0x004c5930 sub_004c5930 | boot | passthrough | 2026-05-02 | Called with (&DAT_00618150, param_1); depth-3 |
| S-0020 | 0x004a2bf7 | 0x004a2c2f FUN_004a2c2f | boot | passthrough | 2026-05-02 | depth-3 of FUN_004a2c2f callee |
| S-0021 | 0x004a5e35 | 0x004a2c2f FUN_004a2c2f | boot | passthrough | 2026-05-02 | __ms_p5_mp_test_fdiv; depth-3; return stored to DAT_00773994 |
| S-0022 | 0x004a5de3 | 0x004a2c2f FUN_004a2c2f | boot | passthrough | 2026-05-02 | depth-3 of FUN_004a2c2f callee |
| S-0023 | 0x004a5f07 | 0x004a3258 FUN_004a3258 | boot | passthrough | 2026-05-02 | ___endstdio; in fn-ptr table 005ea05c-005ea070 |
| S-0024 | 0x004a77eb | 0x004a3258 FUN_004a3258 | boot | passthrough | 2026-05-02 | FUN_004a77eb; called with arg 8 when param_3!=0 |
| S-0025 | 0x004a787f | 0x004a3258 FUN_004a3258 | boot | passthrough | 2026-05-02 | __lock; called with lock index 8 (also index 4 from _calloc) |
| S-0026 | 0x004af32d | 0x004a3258 FUN_004a3258 | boot | passthrough | 2026-05-02 | FUN_004af32d; in fn-ptr table 005ea05c-005ea070 |
| S-0027 | 0x004a45fb | 0x004a40fe ___onexitinit | boot | passthrough | 2026-05-02 | _malloc; alloc 0x80 bytes for atexit list |
| S-0028 | 0x004a4126 | 0x004a415e _atexit | boot | passthrough | 2026-05-02 | __onexit; _atexit delegates entirely to __onexit |
| S-0029 | 0x004a4728 | 0x004a467e _calloc | boot | passthrough | 2026-05-02 | FUN_004a4728; called after ___sbh_alloc_block (unlock?) |
| S-0030 | 0x004a5984 | 0x004a467e _calloc | boot | passthrough | 2026-05-02 | __SEH_prolog; SEH frame setup for _calloc |
| S-0031 | 0x004a59bf | 0x004a467e _calloc | boot | passthrough | 2026-05-02 | __SEH_epilog; SEH frame teardown for _calloc |
| S-0032 | 0x004aac76 | 0x004a467e _calloc | boot | passthrough | 2026-05-02 | ___sbh_alloc_block; SBH allocation path |
| S-0033 | 0x004aaf72 | 0x004a467e _calloc | boot | passthrough | 2026-05-02 | __callnewh; new-handler loop in _calloc |
| S-0034 | 0x004aaf90 | 0x004a467e _calloc | boot | passthrough | 2026-05-02 | _memset; zero-fills SBH allocation |
| S-0035 | 0x004ae29f | 0x004a774d __mtinitlocks | boot | passthrough | 2026-05-02 | ___crtInitCritSecAndSpinCount; init per-lock critical section |
| S-0036 | 0x004a7796 | 0x004a87f7 FUN_004a87f7 | boot | passthrough | 2026-05-02 | __mtdeletelocks; called unconditionally before TlsFree |
| S-0037 | 0x004a2be9 | 0x004ab8d6 FUN_004ab8d6 | boot | passthrough | 2026-05-02 | __security_check_cookie; stack-cookie check injection |
| S-0038 | 0x004a3440 | 0x004ab8d6 FUN_004ab8d6 | boot | passthrough | 2026-05-02 | __chkstk; alloca_probe injection |
| S-0039 | 0x004a34b0 | 0x004ab8d6 FUN_004ab8d6 | boot | passthrough | 2026-05-02 | _strncpy; used in path truncation with "..." |
| S-0060 | 0x004cbc60 | 0x00493710 FUN_00493710 (RW_INIT_FN) | render | passthrough | 2026-05-02 | FUN_004cbc60; receives &LAB_00493700 as arg; depth-1 of RW_INIT_FN; DEFERRED rw_engine_init-cont1 |
| S-0061 | 0x004cbc70 | 0x00493710 FUN_00493710 (RW_INIT_FN) | render | passthrough | 2026-05-02 | FUN_004cbc70; return value stored to DAT_007719dc; depth-1 of RW_INIT_FN; DEFERRED rw_engine_init-cont1 |
| S-0062 | 0x004cbc80 | 0x00493710 FUN_00493710 (RW_INIT_FN) | render | passthrough | 2026-05-02 | FUN_004cbc80; receives &LAB_004936f0 as arg; depth-1 of RW_INIT_FN; DEFERRED rw_engine_init-cont1 |
| S-0063 | 0x004cbc90 | 0x00493710 FUN_00493710 (RW_INIT_FN) | render | passthrough | 2026-05-02 | FUN_004cbc90; return value stored to DAT_007719d8; depth-1 of RW_INIT_FN; DEFERRED rw_engine_init-cont1 |
| S-0064 | 0x00550350 | 0x00493710 FUN_00493710 (RW_INIT_FN) | render | passthrough | 2026-05-02 | FUN_00550350; called with arg 0xffffffff; result checked; depth-1 of RW_INIT_FN; DEFERRED rw_engine_init-cont1 |
| S-0065 | 0x00550390 | 0x00493710 FUN_00493710 (RW_INIT_FN) | render | passthrough | 2026-05-02 | FUN_00550390; cleanup call on 3 failure paths; depth-1 of RW_INIT_FN; DEFERRED rw_engine_init-cont1 |
| S-0066 | 0x005584c0 | 0x00493710 FUN_00493710 (RW_INIT_FN) | render | passthrough | 2026-05-02 | FUN_005584c0; called once on success path; no visible args; DEFERRED rw_engine_init-cont1 |
| S-0067 | 0x005c9d00 | 0x00493710 FUN_00493710 (RW_INIT_FN) | render | passthrough | 2026-05-02 | FUN_005c9d00; called 3× (twice with local_28, once with 0x200000); DEFERRED rw_engine_init-cont1 |
| S-0068 | 0x004ce790 | 0x00493600 FUN_00493600 | render | passthrough | 2026-05-02 | FUN_004ce790; called twice with (ptr-to-global, fn-ptr, fn-ptr-or-label) 3-arg pattern; DEFERRED depth-2 |
| S-0040 | 0x004a45cf | 0x004a45fb _malloc | boot | passthrough | 2026-05-02 | __nh_malloc; depth-2 of _malloc; DEFERRED D-0160 |
| S-0041 | 0x004a4660 | 0x004a460d _free | boot | passthrough | 2026-05-02 | FUN_004a4660 unlock wrapper; depth-2 of _free; DEFERRED D-0161 |
| S-0042 | 0x004a787f | 0x004a460d _free | boot | passthrough | 2026-05-02 | __lock; depth-2 of _free; DEFERRED D-0162 |
| S-0043 | 0x004aa497 | 0x004a460d _free | boot | passthrough | 2026-05-02 | ___sbh_find_block; depth-2 of _free; DEFERRED D-0163 |
| S-0044 | 0x004aa4c2 | 0x004a460d _free | boot | passthrough | 2026-05-02 | ___sbh_free_block; depth-2 of _free; DEFERRED D-0164 |
| S-0045 | 0x004ae28f | 0x004ae29f ___crtInitCritSecAndSpinCount | boot | passthrough | 2026-05-02 | ___crtInitCritSecNoSpinCount@8; depth-2; DEFERRED D-0166 |
| S-0046 | 0x004af166 | 0x004af2b6 ___initmbctable | boot | passthrough | 2026-05-02 | __setmbcp; depth-2 of ___initmbctable; DEFERRED D-0167 |
| S-0047 | 0x004affaf | 0x004affe0 FUN_004affe0 | boot | passthrough | 2026-05-02 | FUN_004affaf; depth-2 of FUN_004affe0; DEFERRED D-0168 |
| S-0700 | 0x(DAT_007d459c indirect) | 0x004c9cd0 RESET_FN | render | passthrough | 2026-05-02 | indirect callback via DAT_007d459c; called in RESET_FN after successful Reset(); DEFERRED D-2020 |
| S-0701 | 0x004dc9e0 | 0x004c9ad0 FUN_004c9ad0 | render | passthrough | 2026-05-02 | FUN_004dc9e0; called in pre-reset resource release; unknown args; DEFERRED D-2020 |
| S-0702 | 0x004cb8a0 | 0x004db3e0 FUN_004db3e0, 0x004e0920 FUN_004e0920 | render | passthrough | 2026-05-02 | FUN_004cb8a0; locks/creates vertex buffer from descriptor; shared across multiple callers; DEFERRED D-2020 |
| S-0703 | 0x004cba80 | 0x004db3e0 FUN_004db3e0, 0x004e0920 FUN_004e0920 | render | passthrough | 2026-05-02 | FUN_004cba80; releases buffer object; shared across multiple callers; DEFERRED D-2020 |
| S-0704 | 0x004d5480 | 0x004d6200 FUN_004d6200 | render | passthrough | 2026-05-02 | FUN_004d5480; render state setter wrapper; called with (opcode, value) pairs; DEFERRED D-2020 |
| S-0705 | 0x004d54f0 | 0x004d6200 FUN_004d6200 | render | passthrough | 2026-05-02 | FUN_004d54f0; sampler state setter wrapper; called with (stage, state, value); DEFERRED D-2020 |
| S-0706 | 0x004d5570 | 0x004d6200 FUN_004d6200 | render | passthrough | 2026-05-02 | FUN_004d5570; texture stage state setter wrapper; called with (stage, state, value); DEFERRED D-2020 |
| S-0707 | 0x004d53b0 | 0x004d6200 FUN_004d6200 | render | passthrough | 2026-05-02 | FUN_004d53b0; state restore epilogue; no visible args; DEFERRED D-2020 |

## Resolved stubs (audit trail — do not delete)

| ID | RVA | Caller | Resolved date | Resolution |
|----|-----|--------|---------------|------------|
| S-0373 | 0x00493ac0 LAB_00493ac0 | 0x00493b50 FUN_00493b50 | 2026-05-03 | C1 analyzed video_mci_d2; pre-NT5 code-page path; GetThreadLocale→GetLocaleInfoA(0x1004)→atoi; fallback GetACP |
| S-0374 | 0x00493b40 LAB_00493b40 | 0x00493b50 FUN_00493b50 | 2026-05-03 | C1 analyzed video_mci_d2; NT5+ code-page path; MOV EAX,3 (CP_THREAD_ACP); RET |
| S-0375 | 0x0049ec10 FUN_0049ec10 | 0x00493c00 FUN_00493c00 | 2026-05-03 | C1 analyzed video_mci_d2; __thiscall ctor; vtable[0]+7 offsets; calls FUN_0049dd60+FUN_0049cfb0 |
| S-0376 | 0x004a3b84 FUN_004a3b84 | 0x00493f00 FUN_00493f00 | 2026-05-03 | C1 analyzed video_mci_d2; vsnprintf-impl via fake FILE (_flag=0x42); calls FUN_004a504f |

## Conventions

- ID format: `S-NNNN`, monotonic, never reused.
- Every stub must have a corresponding `// STUB S-NNNN` comment in source.
- `re-classify` skill writes new rows; `hook-author` skill enforces that no new C3 row in `hooks.csv` lands while the function still has unresolved stubs.
| S-0080 | 0x00551510 | 0x00550390 sub_00550390 | render | passthrough | 2026-05-02 | FUN_00551510; called as FUN_00551510(node_ptr, 2) per list-node iteration; depth-2 of RW_TEAR_FN |
| S-0081 | 0x004c2c90 | 0x004c2f60 sub_004c2f60 + 0x004c3040 sub_004c3040 | render | passthrough | 2026-05-02 | FUN_004c2c90; called with (DAT_007d3ff8+0x10,0x12/3,0,0,0) and (DAT_007d3ff8+4,1,0,0,0); likely core RW engine state-transition function; depth-2 of RW_TEAR_FN |
| S-0082 | 0x004d7ca0 | 0x004c3270 sub_004c3270 | render | passthrough | 2026-05-02 | FUN_004d7ca0; called when DAT_007d3ff4==0 in final teardown step; depth-2 of RW_TEAR_FN |
| S-0083 | 0x004ccf20 | 0x004c3270 sub_004c3270 | render | passthrough | 2026-05-02 | FUN_004ccf20; called when DAT_007d3ff4==0 in final teardown step; depth-2 of RW_TEAR_FN |
| S-0100 | 0x004522d6 | 0x004522d0 FUN_004522d0 | audio | passthrough | 2026-05-02 | Indirect dispatch target through DAT_007d3ff8+0x10c; not statically traceable |
| S-0220 | 0x004d7ff0 | 0x004c2c90 FUN_004c2c90 | render | passthrough | 2026-05-02 | FUN_004d7ff0; called with (0x18, param_2) in default handler of FUN_004c2c90; depth-3; already D-0233, S-0101 |
| S-0221 | 0x004d8480 | 0x004c2c90 FUN_004c2c90 | render | passthrough | 2026-05-02 | FUN_004d8480; called with &uStack_8 in default handler of FUN_004c2c90; depth-3; already D-0234, S-0102 |
| S-0222 | 0x004ccce0 | 0x004d7ca0 FUN_004d7ca0 | render | passthrough | 2026-05-02 | FUN_004ccce0; called with (DAT_007d6c50, &LAB_004d7d70, DAT_007d6c50); depth-3; DEFERRED D-0580 |
| S-0223 | 0x004cc9f0 | 0x004d7ca0 FUN_004d7ca0 | render | passthrough | 2026-05-02 | FUN_004cc9f0; called with DAT_007d6c50 as single arg; depth-3; DEFERRED D-0581 |
| S-0180 | 0x004a2bb8 | 0x004a2be9 __security_check_cookie | boot | passthrough | 2026-05-02 | report_failure; depth-4 of boot_crt_exit_d3; D-0462 |
| S-0181 | 0x004a31e1 | 0x004a4126 __onexit | boot | passthrough | 2026-05-02 | FUN_004a31e1; called before __onexit_lk; role unknown; depth-4; D-0463 |
| S-0182 | 0x004a407e | 0x004a4126 __onexit | boot | passthrough | 2026-05-02 | __onexit_lk; core logic of __onexit; depth-4; D-0464 |
| S-0183 | 0x004a4158 | 0x004a4126 __onexit | boot | passthrough | 2026-05-02 | FUN_004a4158; called after __onexit_lk; role unknown; depth-4; D-0465 |
| S-0184 | 0x004ad33b | 0x004a5de3 FUN_004a5de3 | boot | passthrough | 2026-05-02 | __controlfp; called with (0x10000,0x30000); depth-4; D-0466 |
| S-0185 | 0x004a5df5 | 0x004a5e35 __ms_p5_mp_test_fdiv | boot | passthrough | 2026-05-02 | __ms_p5_test_fdiv; fallback FPU test; depth-4; D-0467 |
| S-0186 | 0x004a9744 | 0x004a5f07 ___endstdio | boot | passthrough | 2026-05-02 | __flushall; unconditional flush; depth-4; D-0468 |
| S-0187 | 0x004ad351 | 0x004a5f07 ___endstdio | boot | passthrough | 2026-05-02 | __fcloseall; conditional close; depth-4; D-0469 |
| S-0188 | 0x004a7800 | 0x004a787f __lock | boot | passthrough | 2026-05-02 | FUN_004a7800; lazy init of lock slot; depth-4; D-0470 |
| S-0189 | 0x004aa7da | 0x004aac76 ___sbh_alloc_block | boot | passthrough | 2026-05-02 | ___sbh_alloc_new_region; allocates new SBH region; depth-4; D-0471 |
| S-0190 | 0x004aa891 | 0x004aac76 ___sbh_alloc_block | boot | passthrough | 2026-05-02 | ___sbh_alloc_new_group; initialises new SBH group; depth-4; D-0472 |
| S-0300 | 0x004b7330 | 0x0047b860 FUN_0047b860 | input | passthrough | 2026-05-02 | FUN_004b7330; called FUN_004b7330(0); returns ptr stored in DAT_006bf1e0; lua_open/lua_newstate equivalent; depth-2 of LUA_INIT_FN; filed D-0820 |
| S-0301 | 0x004c0510 | 0x0047b860 FUN_0047b860 | input | passthrough | 2026-05-02 | FUN_004c0510; called FUN_004c0510(DAT_006bf1e0); takes Lua state pointer; luaL_openlibs equivalent; depth-2 of LUA_INIT_FN; filed D-0820 |
| S-0302 | 0x004b7480 | 0x0047b880 FUN_0047b880 | input | passthrough | 2026-05-02 | FUN_004b7480; called FUN_004b7480(DAT_006bf1e0); takes Lua state pointer; lua_close equivalent; depth-2 of LUA_INIT_FN; filed D-0820 |
| S-0303 | 0x0047b8a0 | 0x0047b8d0 FUN_0047b8d0 | input | passthrough | 2026-05-02 | FUN_0047b8a0; called FUN_0047b8a0(buf, byte_count, param_2); Lua script executor; lua_dostring/lua_dobuffer equivalent; depth-2 of LUA_INIT_FN; filed D-0820 |
| S-0304 | 0x004b6520 | 0x0047b8d0 FUN_0047b8d0 | input | passthrough | 2026-05-02 | FUN_004b6520; called FUN_004b6520(buf, 0x8000); wraps FUN_004b64e0(buf,0,len); zero-fill; depth-2 of LUA_INIT_FN; filed D-0820 |
| S-0280 | 0x00550b00 FUN_00550b00 | 0x00404f80 sub_00404f80 | save | resolved | 2026-05-06 | file-exists check; analyzed in save_gamesave_d2 session; re/analysis/save_gamesave_d2/00550b00.md; VFS router |
| S-0281 | 0x004cc230 FUN_004cc230 | 0x004b3b70 sub_004b3b70 + 0x004b3bb0 sub_004b3bb0 | save | resolved | 2026-05-06 | stream-context open; analyzed in localization session; re/analysis/localization/004cc230.md; cross-ref save_gamesave_d2/004cc230.md |
| S-0282 | 0x004cbd30 FUN_004cbd30 | 0x004b3b70 sub_004b3b70 | save | resolved | 2026-05-06 | RW stream read; analyzed in audio_rws_loader session; re/analysis/audio_rws_loader/004cbd30.md; cross-ref save_gamesave_d2/004cbd30.md |
| S-0283 | 0x004cc160 FUN_004cc160 | 0x004b3b70 sub_004b3b70 + 0x004b3bb0 sub_004b3bb0 | save | resolved | 2026-05-06 | stream-context close; analyzed in localization session; re/analysis/localization/004cc160.md; cross-ref save_gamesave_d2/004cc160.md; U-0288 partially addressed |
| S-0284 | 0x004cbe80 FUN_004cbe80 | 0x004b3bb0 sub_004b3bb0 | save | resolved | 2026-05-06 | RW stream write; analyzed in save_gamesave_d2 session; re/analysis/save_gamesave_d2/004cbe80.md; U-0287 4th arg not seen in decomp |
| S-0202 | 0x00498950 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_00498950; called for saved-subsystem check; return non-zero copies DAT_0077338c/DAT_00773390; depth-3 of rw_engine_init_d2 |
| S-0209 | 0x004c2f30 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_004c2f30; called FUN_004c2f30(DAT_00773200); depth-3 of rw_engine_init_d2; not in rw_engine_init session D |
| S-0160 | 0x004e4860 | 0x004014b0 FUN_004014b0 | boot | passthrough | 2026-05-02 | FUN_004e4860; called with (DAT_636560, DAT_636570) and (DAT_636560, DAT_63656c); depth-3 |
| S-0161 | 0x004c0c20 | 0x004014b0 FUN_004014b0 | boot | passthrough | 2026-05-02 | FUN_004c0c20; called with *(DAT_636570+4) if non-zero; depth-3 |
| S-0162 | 0x004c0740 | 0x004014b0 FUN_004014b0 | boot | passthrough | 2026-05-02 | FUN_004c0740; called with (global, 0); depth-3 |
| S-0163 | 0x004e4d90 | 0x004014b0 FUN_004014b0 | boot | passthrough | 2026-05-02 | FUN_004e4d90; called with global arg; depth-3 |
| S-0164 | 0x004e45b0 | 0x004014b0 FUN_004014b0 | boot | passthrough | 2026-05-02 | FUN_004e45b0; called with (DAT_636560, DAT_636564); depth-3 |
| S-0165 | 0x004e5700 | 0x004014b0 FUN_004014b0 | boot | passthrough | 2026-05-02 | FUN_004e5700; called with DAT_636560; depth-3 |
| S-0166 | 0x004e6e00 | 0x004015a0 FUN_004015a0 | boot | passthrough | 2026-05-02 | FUN_004e6e00; called on 10 handle-slots per element and on DAT_636560/DAT_636564; depth-3 |
| S-0167 | 0x00401630 | 0x004025f0 FUN_004025f0 | boot | passthrough | 2026-05-02 | FUN_00401630; called with index value from local_34; result stored to *puVar4; depth-3 |
| S-0168 | 0x00401690 | 0x004025f0 FUN_004025f0 | boot | passthrough | 2026-05-02 | FUN_00401690; called with no args per loop iteration; depth-3 |
| S-0169 | 0x00402240 | 0x004025f0 FUN_004025f0 | boot | passthrough | 2026-05-02 | FUN_00402240; called 4x per iteration; result stored to puVar4[0x10..0x13]; depth-3 |
| S-0170 | 0x00402590 | 0x004025f0 FUN_004025f0 | boot | passthrough | 2026-05-02 | FUN_00402590; called 6x per iteration; result stored to puVar4[0x14..0x19]; depth-3 |
| S-0171 | 0x004671a0 | 0x004026d0 FUN_004026d0 | boot | passthrough | 2026-05-02 | FUN_004671a0; called with 1-3 args per iter: (0,&0xff000000,3)/(0)/(0,0,1); depth-3 |
| S-0172 | 0x004c1bb0 | 0x004026d0 FUN_004026d0 | boot | passthrough | 2026-05-02 | FUN_004c1bb0; called with result of FUN_004671a0(0,&local,3); depth-3 |
| S-0173 | 0x004c1a00 | 0x004026d0 FUN_004026d0 | boot | passthrough | 2026-05-02 | FUN_004c1a00; called with result of FUN_004671a0(0); return checked nonzero; depth-3 |
| S-0174 | 0x004c19f0 | 0x004026d0 FUN_004026d0 | boot | passthrough | 2026-05-02 | FUN_004c19f0; called conditionally when FUN_004c1a00 returns nonzero; depth-3 |
| S-0175 | 0x004c1be0 | 0x004026d0 FUN_004026d0 | boot | passthrough | 2026-05-02 | FUN_004c1be0; called with result of FUN_004671a0(0,0,1); depth-3 |
| S-0176 | 0x004034a0 | 0x00403640 FUN_00403640 | boot | passthrough | 2026-05-02 | FUN_004034a0; called with param_1; result stored to DAT_0x636b78; depth-3 |
| S-0177 | 0x004c7650 | 0x00403660 FUN_00403660 | boot | passthrough | 2026-05-02 | FUN_004c7650; called with DAT_0x636b78 if nonzero; depth-3 |
| S-0178 | 0x004c5c00 | 0x0040bb30 FUN_0040bb30 | boot | passthrough | 2026-05-02 | FUN_004c5c00; called with (global, param_1) from two forwarders (0x63b8f8 and 0x63b8fc); depth-3 |
| S-0179 | 0x00401000 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_00401000; no args; called in sfx/TXD init sequence; depth-3 |
| S-0305 | 0x00413b80 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_00413b80; no args; called near end of sfx init; depth-3 |
| S-0306 | 0x004210b0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_004210b0; no args; depth-3 of FUN_0040bbb0 |
| S-0307 | 0x0042a6b0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_0042a6b0; called with (name, 0, 0) to load TXD/asset handles; depth-3 |
| S-0308 | 0x00475a00 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_00475a00; called with (handle, 0x18)/(handle, 0x10) for scorch/wfall; depth-3 |
| S-0309 | 0x004759b0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_004759b0; called with arg 2; depth-3 |
| S-0310 | 0x00476390 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_00476390; no args; depth-3 |
| S-0311 | 0x004775b0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_004775b0; no args; depth-3 |
| S-0312 | 0x00477e40 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_00477e40; no args; depth-3 |
| S-0313 | 0x00485d90 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_00485d90; no args; depth-3 |
| S-0314 | 0x004862d0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_004862d0; no args; depth-3 |
| S-0315 | 0x00487e00 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_00487e00; called with (0x32, handle) for RWObjShad; depth-3 |
| S-0316 | 0x00489290 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_00489290; no args; depth-3 |
| S-0317 | 0x0048a460 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_0048a460; no args; last call in FUN_0040bbb0; depth-3 |
| S-0318 | 0x0048a830 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_0048a830; called with arg 0; depth-3 |
| S-0319 | 0x0048ae00 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_0048ae00; no args; depth-3 |
| S-0320 | 0x0048bbe0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_0048bbe0; no args; called after first FUN_0042a6b0; depth-3 |
| S-0321 | 0x0048e820 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_0048e820; called with arg 0x20; depth-3 |
| S-0322 | 0x0048eac0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_0048eac0; no args; depth-3 |
| S-0323 | 0x0048ff20 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_0048ff20; no args; early in sfx init sequence; depth-3 |
| S-0324 | 0x00495280 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_00495280; called with sfx.piz path string; depth-3 |
| S-0325 | 0x004952f0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_004952f0; no args; depth-3 |
| S-0326 | 0x004a332b | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_004a332b; called with arg 0; no-return path after wprintf error; depth-3 |
| S-0327 | 0x004c5bc0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_004c5bc0; called with (DAT_0x63b8f8, FUN_004c5cb0 result); depth-3 |
| S-0328 | 0x004c5c80 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_004c5c80; called with 0 and with DAT_0x63c60c; depth-3 |
| S-0329 | 0x004c5cb0 | 0x0040bbb0 FUN_0040bbb0 | boot | passthrough | 2026-05-02 | FUN_004c5cb0; called with (&DAT_005ccca8, 0); result nonzero check; depth-3 |
| S-0330 | 0x0048bc10 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_0048bc10; first call in teardown sequence; depth-3 |
| S-0331 | 0x0048ffd0 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_0048ffd0; no args; depth-3 |
| S-0332 | 0x00477870 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_00477870; no args; depth-3 |
| S-0333 | 0x00475ea0 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_00475ea0; no args; depth-3 |
| S-0334 | 0x00475d90 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_00475d90; no args; depth-3 |
| S-0335 | 0x0045b350 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_0045b350; called twice in FUN_0040bd00 and once in FUN_0040cf80; depth-3 |
| S-0336 | 0x0048fdd0 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_0048fdd0; no args; depth-3 |
| S-0337 | 0x0048af70 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_0048af70; no args; depth-3 |
| S-0338 | 0x00487140 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_00487140; no args; depth-3 |
| S-0339 | 0x0048cf70 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_0048cf70; no args; depth-3 |
| S-0340 | 0x00485e10 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_00485e10; no args; depth-3 |
| S-0341 | 0x00486350 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_00486350; no args; depth-3 |
| S-0342 | 0x00413bb0 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_00413bb0; no args; depth-3 |
| S-0343 | 0x0041f320 | 0x0040cf80 FUN_0040cf80 | boot | passthrough | 2026-05-02 | FUN_0041f320; called with index 0..3; return checked nonzero; depth-3 |
| S-0344 | 0x0041a980 | 0x0040cf80 FUN_0040cf80 | boot | passthrough | 2026-05-02 | FUN_0041a980; no args; called when any FUN_0041f320 result nonzero; depth-3 |
| S-0345 | 0x00422140 | 0x0040cf80 FUN_0040cf80 | boot | passthrough | 2026-05-02 | FUN_00422140; no args; depth-3 |
| S-0346 | 0x0041ec00 | 0x0040cf80 FUN_0040cf80 | boot | passthrough | 2026-05-02 | FUN_0041ec00; called with index 0..3; depth-3 |
| S-0347 | 0x0040cf40 | 0x0040cf80 FUN_0040cf80 | boot | passthrough | 2026-05-02 | FUN_0040cf40; no args; last call when bVar1 true; depth-3 |
| S-0348 | 0x00490000 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_00490000; no args; first call in FUN_0040cfd0; depth-3 |
| S-0349 | 0x0045bed0 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_0045bed0; no args; depth-3 |
| S-0350 | 0x0045bf30 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_0045bf30; no args; depth-3 |
| S-0351 | 0x00426c00 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_00426c00; no args; result forwarded to FUN_00426b40; depth-3 |
| S-0352 | 0x004725f0 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_004725f0; no args; between FUN_00426c00 and FUN_00426b40; depth-3 |
| S-0353 | 0x00426b40 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_00426b40; called with FUN_00426c00 return value; depth-3 |
| S-0354 | 0x00405400 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_00405400; no args; depth-3 |
| S-0355 | 0x00484c90 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_00484c90; no args; depth-3 |
| S-0356 | 0x00471df0 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_00471df0; no args; depth-3 |
| S-0357 | 0x004114e0 | 0x0040cfd0 FUN_0040cfd0 | boot | passthrough | 2026-05-02 | FUN_004114e0; no args; last call in FUN_0040cfd0; depth-3 |
| S-0358 | 0x00482900 | 0x004113b0 FUN_004113b0 | boot | passthrough | 2026-05-02 | FUN_00482900; called with (table_entry, 10); result summed toward 0x24a3c; depth-3 |
| S-0359 | 0x004987b0 | 0x004113b0 FUN_004113b0 | boot | passthrough | 2026-05-02 | FUN_004987b0; called on sum mismatch error; depth-3 |
| S-0360 | 0x004770c0 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_004770c0; called with (&DAT_0x63bfd8, 0x80c, 8, shockwave_handle); depth-3 |
| S-0361 | 0x0042a5d0 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_0042a5d0; called with (dff_name,0,0) and (target_name,0,0); depth-3 |
| S-0362 | 0x00419090 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_00419090; called with loop index 0..1; depth-3 |
| S-0363 | 0x004b3f90 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_004b3f90; called with DAT_0x63c608 result; return forwarded to FUN_004e69a0; depth-3 |
| S-0364 | 0x004e69a0 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_004e69a0; called with FUN_004b3f90 result in loop 2; result stored to *puVar5; depth-3 |
| S-0365 | 0x004c0b30 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_004c0b30; no args; result passed to FUN_004e7e30; depth-3 |
| S-0366 | 0x004e7e30 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_004e7e30; called with (*puVar5, FUN_004c0b30 result); depth-3 |
| S-0367 | 0x00418a00 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_00418a00; no args; called per iteration in loop 3; depth-3 |
| S-0368 | 0x00418a30 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_00418a30; no args; called after FUN_00418a00 each iteration; depth-3 |
| S-0369 | 0x00419ff0 | 0x00418980 thunk_FUN_0041a060 | boot | passthrough | 2026-05-02 | FUN_00419ff0; no args; called after all 3 loops; depth-3 |
| S-0370 | 0x004e6920 | 0x004189e0 thunk_FUN_004196f0 | boot | passthrough | 2026-05-02 | FUN_004e6920; called with *piVar2 in teardown loop 2; depth-3 |
| S-0371 | 0x004768c0 | 0x004189e0 thunk_FUN_004196f0 | boot | passthrough | 2026-05-02 | FUN_004768c0; called with &DAT_0x63bfd8; depth-3 |
| S-0372 | 0x00418f40 | 0x004189e0 thunk_FUN_004196f0 | boot | passthrough | 2026-05-02 | FUN_00418f40; no args; last teardown call; depth-3 |
| S-0460 | 0x004b6520 | 0x0045ba10 FUN_0045ba10 | vehicle | passthrough | 2026-05-02 | FUN_004b6520; no args; called in per-slot reset before field zeroing; depth-2 |
| S-0461 | 0x0042a530 | 0x0042a6b0 FUN_0042a6b0 | vehicle | passthrough | 2026-05-02 | FUN_0042a530; TXD/DFF lookup; called with (&DAT_0067e1a8, &DAT_0067dfa8); returns 0/1/other; depth-2 |
| S-0462 | 0x004b3d80 | 0x0042a6b0 FUN_0042a6b0 | vehicle | passthrough | 2026-05-02 | FUN_004b3d80; result returned when FUN_0042a530 returns 1; depth-2 |
| S-0463 | 0x004b3d20 | 0x0042a6b0 FUN_0042a6b0 | vehicle | passthrough | 2026-05-02 | FUN_004b3d20; result returned when FUN_0042a530 returns non-0 non-1; depth-2 |
| S-0464 | 0x0047b860 | 0x0047b9b0 FUN_0047b9b0 | vehicle | passthrough | 2026-05-02 | FUN_0047b860; Lua pre-execution setup; no args; depth-2 |
| S-0465 | 0x0047b8d0 | 0x0047b9b0 FUN_0047b9b0 | vehicle | passthrough | 2026-05-02 | FUN_0047b8d0; Lua script executor; called with (filename, buf); depth-2 |
| S-0466 | 0x0047b880 | 0x0047b9b0 FUN_0047b9b0 | vehicle | passthrough | 2026-05-02 | FUN_0047b880; Lua post-execution teardown; no args; depth-2 |
| S-0467 | 0x004781b0 | 0x00411f30 FUN_00411f30 | vehicle | passthrough | 2026-05-02 | FUN_004781b0; DFF loader variant; called with ("LaserTower.dff", 0); differs from FUN_0042a5d0 usage; depth-2 |
| S-0468 | 0x004b5320 | 0x00411f30 FUN_00411f30 | vehicle | passthrough | 2026-05-02 | FUN_004b5320; model flag/property setter; called with (uVar1, 0x40, 1); depth-2 |
| S-0469 | 0x004b5580 | 0x00411f30 FUN_00411f30 | vehicle | passthrough | 2026-05-02 | FUN_004b5580; applies 4-byte color buffer to mesh; called with (uVar1, &local_408); depth-2 |
| S-0420 | 0x004095a0 | FUN_00409710 | render | depth-2 | 2026-05-02 | FUN_004095a0; called with arg 1 before FUN_00495280 in line-exception loader |
| S-0423 | 0x004cbd30 | FUN_00409710/FUN_004235b0 | render | depth-2 | 2026-05-02 | FUN_004cbd30; called with (handle, dest_buf, size); reads chunk into float array |
| S-0425 | 0x004b3f90 | FUN_00412050 | render | depth-2 | 2026-05-02 | FUN_004b3f90; called with (DAT_0063bc40, &DAT_005cd004, 0, 0.1f, ~1.0f, 3.0f, index); U-0428 |
| S-0426 | 0x0047fad0 | FUN_00412050 | render | depth-2 | 2026-05-02 | FUN_0047fad0; called on result of FUN_004b3f90; returns value stored in DAT_0063bb60 region |
| S-0427 | 0x0047cdc0 | FUN_00412050 | render | depth-2 | 2026-05-02 | FUN_0047cdc0; called with (stored_value, 6.0f) twice per iter |
| S-0428 | 0x00423480 | FUN_004235b0 | render | depth-2 | 2026-05-02 | FUN_00423480; called before piz open in AI data loader; no visible args |
| S-0429 | 0x00425bf0 | FUN_004262f0 | render | depth-2 | 2026-05-02 | FUN_00425bf0; called before 64-byte record loop; no visible args |
| S-0430 | 0x00425c00 | FUN_004262f0 | render | depth-2 | 2026-05-02 | FUN_00425c00; called with 64-byte record pointer per iteration |
| S-0431 | 0x00425b70 | FUN_004262f0 | render | depth-2 | 2026-05-02 | FUN_00425b70; called with (iteration_index, looked_up_uint32) per iteration |
| S-0400 | 0x00416a30 | FUN_00418560 | ai | depth-1 | 2026-05-02 | FUN_00416a30; AI control step, mode 4/9 variant; args: (puVar9, param_1, EDI, 0x42c80000) |
| S-0401 | 0x00416250 | FUN_00418560 | ai | depth-1 | 2026-05-02 | FUN_00416250; AI control step, normal mode (primary); args: (puVar9, param_1, EDI, 0x42c80000) |
| S-0402 | 0x00417640 | FUN_00418560 | ai | depth-1 | 2026-05-02 | FUN_00417640; post-step processing; args: (param_1, EDI) |
| S-0440 | 0x004c5c00 | 0x0040bb50 FUN_0040bb50 | frontend | passthrough | 2026-05-02 | FUN_004c5c00; asset lookup by name; called as (DAT_0063b8fc, param_1); depth-2 |
| S-0441 | 0x00427680 | 0x00427e00 FUN_00427e00 | frontend | passthrough | 2026-05-02 | FUN_00427680; sprite position/scale setter; args (x, y, flags, scale); depth-2 |
| S-0442 | 0x00427780 | 0x00427e00 FUN_00427e00 | frontend | passthrough | 2026-05-02 | FUN_00427780; sprite selector by ID; arg param_1; depth-2 |
| S-0443 | 0x004277a0 | 0x00427e00 FUN_00427e00 | frontend | passthrough | 2026-05-02 | FUN_004277a0; sprite render preparation (no args); depth-2 |
| S-0444 | 0x00552750 | 0x00427e00 FUN_00427e00 | frontend | passthrough | 2026-05-02 | FUN_00552750; render state pre-setup B (no args); depth-2 |
| S-0445 | 0x00552d10 | 0x00427e00 FUN_00427e00 | frontend | passthrough | 2026-05-02 | FUN_00552d10; render state pre-colour setup (no args); depth-2 |
| S-0446 | 0x00552d70 | 0x00427e00 FUN_00427e00 | frontend | passthrough | 2026-05-02 | FUN_00552d70; render state post-cleanup (no args); depth-2 |
| S-0447 | 0x00556ca0 | 0x00427e00 FUN_00427e00 | frontend | passthrough | 2026-05-02 | FUN_00556ca0; sprite draw call; args (canvas, buf, scaled_size, auStack, canvas2); depth-2 |
| S-0448 | 0x00556e90 | 0x00427e00 FUN_00427e00 | frontend | passthrough | 2026-05-02 | FUN_00556e90; vertex colour setter; args (canvas, color×4); depth-2 |
| S-0452 | 0x0042e590 | 0x0042e5b0 FUN_0042e5b0 | frontend | passthrough | 2026-05-02 | FUN_0042e590; animated logo draw; args (x,y,dim,dim,dim,color,frame_offset,sprite_idx,flag); depth-2 |
| S-0453 | 0x00473c20 | 0x0042e5b0 FUN_0042e5b0 | frontend | passthrough | 2026-05-02 | FUN_00473c20; fullscreen BG draw with two texture sources; depth-2 |
| S-0454 | 0x00473ee0 | 0x0042e5b0 FUN_0042e5b0 | frontend | passthrough | 2026-05-02 | FUN_00473ee0; slide-in panel draw; arg includes fVar1+base_Y_offset; depth-2 |
| S-0455 | 0x00474890 | 0x0042e5b0 FUN_0042e5b0 | frontend | passthrough | 2026-05-02 | FUN_00474890; secondary logo/sprite submit; single int arg (uVar4); depth-2 |
| S-0456 | 0x0042b8b0 | 0x00472c60 FUN_00472c60 | frontend | passthrough | 2026-05-02 | FUN_0042b8b0; returns short; used as X-axis screen dimension/scale; depth-2 |
| S-0457 | 0x0042b8c0 | 0x00472c60 FUN_00472c60 | frontend | passthrough | 2026-05-02 | FUN_0042b8c0; returns short; used as Y-axis screen dimension/scale; depth-2 |

| S-1420 | 0x004a3384 FUN_004a3384 | FUN_00414570 FUN_0046d570 FUN_00415e20 | ai | passthrough | 2026-05-03 | math: likely acos(double); result scaled by _DAT_005cc970 for game-angle units; used in bearing calculations |
| S-1421 | 0x004c3bf0 FUN_004c3bf0 | FUN_00415d00 FUN_00416060 | ai | passthrough | 2026-05-03 | 2D vector length: returns float10 magnitude of (x,0,z) or similar; used for ray-march termination |
| S-1422 | 0x004c39b0 FUN_004c39b0 | FUN_00414570 FUN_00415880 FUN_0046d570 | ai | passthrough | 2026-05-03 | 3-vector normalize: writes unit vector in-place; used throughout AI targeting for direction checks |
| S-1423 | 0x00426bb0 FUN_00426bb0 | FUN_00414570 FUN_00415880 | ai | passthrough | 2026-05-03 | returns int; result 5-iVar3 used as lap-progress threshold; likely total-laps or rank count |
| S-1424 | 0x00534870 FUN_00534870 | FUN_00472650 | ai | passthrough | 2026-05-03 | PRNG state getter: takes rng context param; returns 32-bit integer used for random float generation |
| S-1425 | 0x004671a0 FUN_004671a0 | FUN_00443440 | ai | passthrough | 2026-05-03 | debug draw segment: called when param_5!=0 in FUN_00443440; draws spline lookahead line; returns handle |
| S-1426 | 0x004b55a0 FUN_004b55a0 | FUN_00443440 | ai | passthrough | 2026-05-03 | debug draw release: frees/submits draw object returned by FUN_004671a0; paired with FUN_004671a0 |
| S-0340 | 0x005a9e10 FUN_005a9e10 | 0x005b9f30 LAB_005b9f30 | audio | passthrough | 2026-05-02 | two-call dispatcher; depth-1 callee of FUN_005b9f30; RESOLVED: analyzed in audio_dsound-20260502-1942 |
| S-0341 | 0x005aee20 FUN_005aee20 | 0x005b9f30 LAB_005b9f30 | audio | passthrough | 2026-05-02 | bit-scan-forward loop; depth-1 callee of FUN_005b9f30; RESOLVED: analyzed in audio_dsound-20260502-1942 |
| S-0342 | 0x005ba780 | 0x005b9f30 LAB_005b9f30 | audio | passthrough | 2026-05-02 | LAB_005ba780; fn-ptr at struct+0x34; unrecognized body; depth-1 callee |
| S-0343 | 0x005ba7f0 | 0x005b9f30 LAB_005b9f30 | audio | passthrough | 2026-05-02 | LAB_005ba7f0; fn-ptr at struct+0x30; unrecognized body; depth-1 callee |
| S-0344 | 0x005adfe0 | 0x005a9e10 FUN_005a9e10 | audio | passthrough | 2026-05-02 | FUN_005adfe0; called as FUN_005adfe0(param_1, param_3); depth-2 of entry |
| S-0345 | 0x005ae010 | 0x005a9e10 FUN_005a9e10 | audio | passthrough | 2026-05-02 | FUN_005ae010; called as FUN_005ae010(param_1, param_2); depth-2 of entry |
| S-0346 | 0x005ba720 | 0x005ba1d0 LAB_005ba1d0 | audio | passthrough | 2026-05-02 | LAB_005ba720; early-exit check; depth-2 of entry |
| S-0347 | 0x005bb000 | 0x005ba1d0 LAB_005ba1d0 | audio | passthrough | 2026-05-02 | FUN_005bb000; called before DirectSoundCreate8 with param1; depth-2 of entry |
| S-0348 | 0x005ba760 | 0x005ba1d0 LAB_005ba1d0 | audio | passthrough | 2026-05-02 | LAB_005ba760; error cleanup on DirectSoundCreate8 fail + buffer fail; depth-2 of entry |
| S-0349 | 0x005bbc10 | 0x005ba1d0 LAB_005ba1d0 | audio | passthrough | 2026-05-02 | FUN_005bbc10; format/caps query after vtable+0x18; depth-2 of entry |
| S-0350 | 0x005bbdb0 | 0x005ba1d0 LAB_005ba1d0 + 0x005bad30 LAB_005bad30 | audio | passthrough | 2026-05-02 | FUN_005bbdb0; buffer creation; called in both DirectSound init paths; depth-2 of entry |
| S-0351 | 0x005bac00 | 0x005ba1d0 LAB_005ba1d0 + 0x005bad30 LAB_005bad30 | audio | passthrough | 2026-05-02 | FUN_005bac00; conditional callee when [ESI+0xc] nonzero; depth-2 of entry |
| S-0352 | 0x005bbf30 | 0x005bad30 LAB_005bad30 | audio | passthrough | 2026-05-02 | FUN_005bbf30; unconditional single-arg callee after FUN_005bac00; depth-2 of entry |
| S-0540 | 0x0046d320 | 0x0040b090 FUN_0040b090 | render | passthrough | 2026-05-02 | FUN_0046d320; returns packed color/state int per (slot, index) pair; depth-2 of camera_follow |
| S-0541 | 0x0046d360 | 0x0040b090 FUN_0040b090 | render | passthrough | 2026-05-02 | FUN_0046d360; returns validity flag per (slot, index) pair; depth-2 of camera_follow |
| S-0542 | 0x0040aef0 | 0x0040b090 FUN_0040b090 | render | passthrough | 2026-05-02 | FUN_0040aef0; per-slot update called once per outer loop iteration; depth-2 of camera_follow |
| S-0543 | 0x0055dec0 | 0x0040b090 FUN_0040b090 | render | passthrough | 2026-05-02 | FUN_0055dec0; reads state from DAT_0066d728 struct; returns 0x11 or 0x12 discriminant; depth-2 of camera_follow |
| S-0544 | 0x004756e0 | 0x00426700 FUN_00426700 | render | passthrough | 2026-05-02 | FUN_004756e0; per-node callback called with two table lookups + node ptr + time-delta float + node[4]; depth-2 of camera_follow |
| S-0545 | 0x00475010 | 0x00426780 FUN_00426780 | render | passthrough | 2026-05-02 | FUN_00475010; per-entry time-delta update; called with *piVar2 (handle/ptr) and float time delta; depth-2 of camera_follow |
| S-0620 | 0x005a66d0 | 0x004623e0 FUN_004623e0 + 0x0045da60 + 0x0045dd60 + 0x004631f0 | audio | resolved | 2026-05-06 | FUN_005a66d0; play/stop dispatcher; param_2=0→play, param_2!=0→stop+DAT_007dca50→+0x54; re/analysis/audio_music_d2/005a66d0.md; U-2207 S-2200 |
| S-0621 | 0x005a6dc0 | 0x004623e0 FUN_004623e0 + 0x0045dd60 + 0x004631f0 | audio | resolved | 2026-05-06 | FUN_005a6dc0; null-guard wrapper for FUN_005a6d60; param_4 by address; re/analysis/audio_music_d2/005a6dc0.md; U-2209 S-2201 |
| S-0622 | 0x0045e0f0 | 0x004623e0 FUN_004623e0 + 0x004631f0 | audio | resolved | 2026-05-06 | FUN_0045e0f0; per-channel volume setter; clamp + obj+0x40 + FUN_005a6dc0(5,1,vol) + DAT_0068f644; re/analysis/audio_music_d2/0045e0f0.md; U-2210 |
| S-0623 | 0x00431b20 | 0x0045dd60 FUN_0045dd60 + 0x004631f0 | audio | resolved | 2026-05-06 | FUN_00431b20; fsin(DAT_007f0f00*_DAT_005cd8f0) float10; phase-channel A; re/analysis/audio_music_d2/00431b20.md; U-2211 |
| S-0624 | 0x00432290 | 0x0045dd60 FUN_0045dd60 | audio | resolved | 2026-05-06 | FUN_00432290; already in hooks.csv via timer_d2_cont1; DAT_0067eab0!=0 && DAT_0067eabc in {0xFF210000 0xFF220000}; U-1619 |
| S-0625 | 0x005baf00 | 0x0045dd60 FUN_0045dd60 | audio | resolved | 2026-05-06 | FUN_005baf00; music group volume; +0x38=vol; circular list: node+0x14|=0x40; secondary+0x30=vol; re/analysis/audio_music_d2/005baf00.md; U-2212..U-2214 |
| S-0626 | 0x00431b60 | 0x004631f0 FUN_004631f0 | audio | resolved | 2026-05-06 | FUN_00431b60; already in hooks.csv via timer_d2_cont1; fsin(DAT_007f0f08*_DAT_005cd8f0); phase-channel B |
| S-0627 | 0x0042f760 | 0x004631f0 FUN_004631f0 | audio | resolved | 2026-05-06 | FUN_0042f760; returns DAT_0067f19c; channel-B trigger flag; re/analysis/audio_music_d2/0042f760_0042f770_0042f780.md; U-2215 |
| S-0628 | 0x0042f770 | 0x004631f0 FUN_004631f0 | audio | resolved | 2026-05-06 | FUN_0042f770; returns DAT_0067f1a0; channel-C trigger-A flag; U-2215 |
| S-0629 | 0x0042f780 | 0x004631f0 FUN_004631f0 | audio | resolved | 2026-05-06 | FUN_0042f780; returns DAT_0067f1a4; channel-C trigger-B flag; U-2215 |
| S-0630 | 0x00432230 | 0x004631f0 FUN_004631f0 | audio | resolved | 2026-05-06 | FUN_00432230; state[idx*0x40+0]==0x13 && sub-state==1; channel-D trigger; re/analysis/audio_music_d2/00432230_00432260.md; U-2216..U-2219 |
| S-0631 | 0x00432260 | 0x004631f0 FUN_004631f0 | audio | resolved | 2026-05-06 | FUN_00432260; same state==0x13 but sub-state==2; musicloop1 trigger; U-2216..U-2219 |
| S-0546 | 0x004c4d20 | 0x00426810 FUN_00426810 | render | passthrough | 2026-05-02 | FUN_004c4d20; builds rotation matrix from packed source (param_1+0x1060c); candidate RW math op; depth-2 of camera_follow |
| S-0547 | 0x004c3dc0 | 0x00426810 FUN_00426810 | render | passthrough | 2026-05-02 | FUN_004c3dc0; transforms 3-float vector by matrix local_40; candidate RW vector transform; depth-2 of camera_follow |
| S-0548 | 0x004c39b0 | 0x00426810 FUN_00426810 | render | passthrough | 2026-05-02 | FUN_004c39b0; in-place op on 3-float vector {local_64,0,local_5c}; candidate RW vector normalize; depth-2 of camera_follow |
| S-0549 | 0x0042b930 | 0x004671a0 FUN_004671a0 | render | passthrough | 2026-05-02 | FUN_0042b930; 5-byte getter; returns int state; discriminant 3 routes to FUN_0042f510; depth-2 of camera_follow |
| S-0550 | 0x0042f510 | 0x004671a0 FUN_004671a0 | render | passthrough | 2026-05-02 | FUN_0042f510; 5-byte getter; alternative vehicle ptr when state==3 and param!=-1; depth-2 of camera_follow |
| S-0551 | 0x00471780 | 0x00471ec0 FUN_00471ec0 | render | passthrough | 2026-05-02 | FUN_00471780; per-entry update in outer loop; takes param_1 (cam data) and iVar8 (entry ptr); depth-2 of camera_follow |
| S-0552 | 0x00471ac0 | 0x00471ec0 FUN_00471ec0 | render | passthrough | 2026-05-02 | FUN_00471ac0; called when camera anim triggers; no args; returns; depth-2 of camera_follow |
| S-0553 | 0x0047ce80 | 0x00471ec0 FUN_00471ec0 | render | passthrough | 2026-05-02 | FUN_0047ce80; returns ID from target object uVar4; depth-2 of camera_follow |
| S-0554 | 0x0047ce00 | 0x00471ec0 FUN_00471ec0 | render | passthrough | 2026-05-02 | FUN_0047ce00; returns flags from target object uVar4 (bits 1,2,4 tested); depth-2 of camera_follow |
| S-0555 | 0x0047bb10 | 0x0047c160 FUN_0047c160 | render | passthrough | 2026-05-02 | FUN_0047bb10; per-node computation; args: node ptr, iVar1+0x30, output ptr, validity ptr; depth-2 of camera_follow |
| S-0560 | 0x004671a0 | 0x00403160 FUN_00403160 | hud | passthrough | 2026-05-02 | FUN_004671a0; camera/viewport handle getter; already in hooks.csv (render, camera_follow); depth-2 of FUN_00403160 |
| S-0480 | 0x0042c280 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_0042c280; called case 3 when FUN_0042c220 non-zero; D-1360 |
| S-0481 | 0x0042c2d0 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_0042c2d0; cases 1 and 4; get-value passed to FUN_0042b940; D-1360 |
| S-0482 | 0x0042c2e0 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_0042c2e0; cases 1 and 4; pending-event check; D-1360 |
| S-0483 | 0x0042c2f0 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_0042c2f0; called with arg 0 in cases 1 and 4; D-1360 |
| S-0484 | 0x0042f500 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_0042f500; case 3 player-sort mode selector; D-1360 |
| S-0485 | 0x0042f6a0 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_0042f6a0; case 3 race-end check vs 0xb; D-1360 |
| S-0486 | 0x00432080 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_00432080; case 3 trigger DAT_00771968=5 path; D-1360 |
| S-0487 | 0x004331a0 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_004331a0; case 3 race-end path with FUN_0042b910 result; D-1360 |
| S-0488 | 0x00448700 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_00448700; case 3 final placement call (1, local_10[0]); D-1360 |
| S-0489 | 0x004927c0 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_004927c0; unconditional entry call; purpose not yet analyzed; D-1360 |
| S-0490 | 0x005c9d00 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_005c9d00; case 3 end-of-race trigger input; D-1360 |
| S-0491 | 0x00417740 | 0x0040b540 FUN_0040b540 | util | passthrough | 2026-05-02 | FUN_00417740; called i=0..3 when DAT_007f0fd0==4||7; return written to param_1[i] |
| S-0492 | 0x0040d270 | 0x0040d440 FUN_0040d440 | util | passthrough | 2026-05-02 | FUN_0040d270; called with (PTR_PTR_005f2770, DAT_0063ba7c); course-load orchestrator seen during discovery traversal |
| S-0493 | 0x0040e470 | 0x0042c220 FUN_0042c220 | util | passthrough | 2026-05-02 | FUN_0040e470; called per slot index 0–3; return==1 gates thunk call in FUN_0042c220 |
| S-0494 | 0x00497450 | 0x0042c220 FUN_0042c220 | util | passthrough | 2026-05-02 | thunk_FUN_00497450; called with DAT_007f1a14[i] (0–11); return==0 triggers DAT_0067ea10 flag write |
| S-0680 | 0x004a92de | 0x004af2d4 FUN_004af2d4 | util | passthrough | 2026-05-02 | terminate; CRT terminate(); called when MSVC C++ exception (0xE06D7363/3/0x19930520) detected in top-level SEH filter |
| S-0681 | 0x004af400 | 0x004af2d4 FUN_004af2d4 | util | passthrough | 2026-05-02 | _ValidateExecute; CRT IsBadCodePtr-style validator for function pointer; gates chain-to-old-filter path in FUN_004af2d4 |
| S-0660 | 0x004a4170 | 0x004950b0 FUN_004950b0 | util | passthrough | 2026-05-02 | __alldiv; MSVC 64-bit integer division runtime helper embedded in binary |
| S-0661 | 0x004a4220 | 0x004950b0 FUN_004950b0 | util | passthrough | 2026-05-02 | __allmul; MSVC 64-bit integer multiply runtime helper embedded in binary |
| S-0800 | 0x00492d20 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_00492d20; called each frame, no args, return discarded; tick/advance |
| S-0801 | 0x00493f70 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_00493f70; video completion check; returns 0 when done |
| S-0802 | 0x00493f80 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_00493f80; reads two floats (video dims) into out-params |
| S-0803 | 0x00493fc0 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_00493fc0; takes two floats; returns scale/aspect value |
| S-0804 | 0x00493fd0 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_00493fd0; render draw call; takes (render_target, float4_rect, float4_scale, 0, scale_val) |
| S-0805 | 0x00494460 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_00494460; stops/closes current video stream; arg 0 |
| S-0806 | 0x00494480 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_00494480; gates render/scan paths; arg 0; non-zero=active |
| S-0807 | 0x00494a80 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_00494a80; starts video by index; args (0, index, 0) |
| S-0808 | 0x00499710 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_00499710; arg 0; return passed to FUN_004c1be0 |
| S-0809 | 0x004c19f0 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_004c19f0; takes render target; closes/submits render block |
| S-0810 | 0x004c1a00 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_004c1a00; takes render target; non-zero return gates draw call |
| S-0811 | 0x004c1bb0 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_004c1bb0; takes (render_target, color_bytes_ptr, 1); sets render state |
| S-0812 | 0x004c1be0 | 0x00495350 FUN_00495350 | frontend | passthrough | 2026-05-02 | FUN_004c1be0; takes (render_target, FUN_00499710_result); purpose unknown |
| S-0813 | 0x004967e0 | 0x00492d20 FUN_00492d20 | frontend | passthrough | 2026-05-02 | FUN_004967e0; 283 bytes; sole callee of shim FUN_00492d20; depth-2 from INTRO_FN; D-2320 |
| S-0860 | 0x005507b0 FUN_005507b0 | 0x004cc230 FUN_004cc230 | frontend | passthrough | 2026-05-02 | PIZ archive file-open; called as FUN_005507b0(filename, piz_base); depth-2 of stream-open (localization path) |
| S-0861 | 0x00550bc0 FUN_00550bc0 | 0x004cc230 FUN_004cc230 | frontend | passthrough | 2026-05-02 | File open by name; called as FUN_00550bc0(filename); depth-2 of stream-open (type=1 branch) |
| S-0862 | 0x00550910 FUN_00550910 | 0x004cc160 FUN_004cc160 | frontend | passthrough | 2026-05-02 | File close; called as FUN_00550910(file_handle); depth-2 of stream-close |
| S-0662 | 0x0049d1d0 | 0x0049d270 FUN_0049d270 | util | passthrough | 2026-05-02 | FUN_0049d1d0; no-arg callee at end of FUN_0049d270; purpose unknown |
| S-0814 | 0x004c75e0 | 0x00493fd0 FUN_00493fd0 | frontend | passthrough | 2026-05-02 | FUN_004c75e0; 26 bytes; fills two short[2] (viewport offsets); depth-2; D-2321 |
| S-0826 | 0x004c2f30 FUN_004c2f30 | 0x00499400 FUN_00499400 | save | passthrough | 2026-05-02 | called after save with DAT_00773200 (mode index); applies mode selection; depth-2; DEFERRED D-2386 |
| S-0827 | 0x004c2ed0 FUN_004c2ed0 | 0x00499400 FUN_00499400 | save | passthrough | 2026-05-02 | called after save with &local+DAT_00773200; writes result struct (bStack_50 flags display params); depth-2; DEFERRED D-2387 |
| S-0815 | 0x00494320 | 0x00494460 FUN_00494460 | frontend | passthrough | 2026-05-02 | FUN_00494320; 167 bytes; first cleanup step in video close sequence; depth-2; D-2322 |
| S-0816 | 0x004c7650 | 0x00494460 FUN_00494460 | frontend | passthrough | 2026-05-02 | FUN_004c7650; releases DAT_00771a18 video texture handle; depth-2; D-2323 |
| S-0819 | 0x004c77c0 | 0x00494a80 FUN_00494a80 | frontend | passthrough | 2026-05-02 | FUN_004c77c0; 153 bytes; video texture allocator args(0,0,0,0x84); depth-2; D-2326 |
| S-0667 | 0x004030d0 | 0x00492d30 FUN_00492d30 | util | passthrough | 2026-05-02 | FUN_004030d0; called in state-1 when FUN_0042b930==0x21 |
| S-0668 | 0x004111c0 | 0x00492d30 FUN_00492d30 | util | passthrough | 2026-05-02 | FUN_004111c0; called with arg 50 (0x32) in loop for states 3 and 6 |
| S-0669 | 0x0043d7c0 | 0x00492d30 FUN_00492d30 | util | passthrough | 2026-05-02 | FUN_0043d7c0; called unconditionally after switch post-branch |
| S-0670 | 0x0043dfd0 | 0x00492d30 FUN_00492d30 | util | passthrough | 2026-05-02 | FUN_0043dfd0; called after case-4 branch and after switch exit |
| S-0671 | 0x00496920 | 0x00492d30 FUN_00492d30 | util | passthrough | 2026-05-02 | FUN_00496920; called with arg 5; return gates DAT_007719cc block |
| S-0672 | 0x00498860 | 0x00492d30 FUN_00492d30 | util | passthrough | 2026-05-02 | FUN_00498860; called with FUN_004671a0(0) result when DAT_007719cc==0 |
| S-0820 | 0x004c7730 | 0x004c1be0 FUN_004c1be0 | frontend | passthrough | 2026-05-02 | FUN_004c7730; 46 bytes; receives inner render object and HWND; MCI draw? depth-2; D-2327 |
| S-0640 | 0x004a2b60 FUN_004a2b60 | 0x00496490 FUN_00496490 | boot | passthrough | 2026-05-02 | sprintf variant; called FUN_004a2b60(buf "%.19s\n\n" uVar1); depth-2; also in D-2502 localization-cont1 |
| S-0643 | 0x004a4368 _fclose | 0x00496470 FUN_00496470 | boot | passthrough | 2026-05-02 | FidDB-matched CRT _fclose; __cdecl; 0x004a4368..0x004a43ad; 69 bytes; depth-2; CRT out-of-scope |
| S-1020 | 0x004a9858 FUN_004a9858 | 0x004a43b9 FUN_004a43b9 | boot | passthrough | 2026-05-03 | time-struct ptr → struct tm* converter; body 0x004a9858..0x004a9a57 (512 bytes); depth-3 of WM_CREATE chain; D-2980 |
| S-1021 | 0x004a974d FUN_004a974d | 0x004a43b9 FUN_004a43b9 | boot | passthrough | 2026-05-03 | struct-tm-like int → char* date string (asctime-style 26 chars); body 0x004a974d..0x004a9857 (267 bytes); depth-3 of WM_CREATE chain; D-2981 |
| S-1022 | 0x004a4e40 __aulldiv | 0x004a43d2 FUN_004a43d2 | boot | passthrough | 2026-05-03 | CRT 64-bit unsigned integer division runtime helper; body 0x004a4e40..0x004a4ea7; depth-3; D-2982 |
| S-1023 | 0x004aa00c FUN_004aa00c | 0x004a43d2 FUN_004a43d2 | boot | passthrough | 2026-05-03 | zero-arg call at start of FUN_004a43d2; side effect unknown; body 0x004aa00c..0x004aa039; depth-3; D-2983 |
| S-1024 | 0x004aa060 __aullrem | 0x004a43d2 FUN_004a43d2 | boot | passthrough | 2026-05-03 | CRT 64-bit unsigned integer remainder runtime helper; body 0x004aa060..0x004aa0d4; depth-3; D-2984 |
| S-1040 | 0x0042bf30 FUN_0042bf30 | 0x0042c280 FUN_0042c280 | vehicle | passthrough | 2026-05-03 | called with (0x27f, 0xff210000, 0, 0, 0, 0); 6 hardcoded args; 112 bytes; depth-2; D-3040 |
| S-1041 | 0x0042d3a0 FUN_0042d3a0 | 0x00432080 FUN_00432080 | vehicle | passthrough | 2026-05-03 | called on phase-transition success path; also called by FUN_004331a0; 52 bytes; depth-2; D-3041 |
| S-1042 | 0x004248b0 FUN_004248b0 | 0x004331a0 FUN_004331a0 | vehicle | passthrough | 2026-05-03 | called at end of race-end init; no params; 367 bytes; depth-2; D-3042 |
| S-1043 | 0x00424920 FUN_00424920 | 0x004331a0 FUN_004331a0 | vehicle | passthrough | 2026-05-03 | called after FUN_004248b0 in race-end init; no params; 607 bytes; depth-2; D-3043 |
| S-1044 | 0x004464c0 FUN_004464c0 | 0x00448700 FUN_00448700 | vehicle | passthrough | 2026-05-03 | called 100 times with &DAT_00897fe0; 91 bytes; depth-2; D-3044 |
| S-1000 | 0x0042bf30 FUN_0042bf30 | 0x0042c280 FUN_0042c280 | util | passthrough | 2026-05-03 | called with (0x27f, 0xff210000, 0, 0, 0, 0); pending-action packet setter; body 0x0042bf30..0x0042bfa0; D-2920 |
| S-1001 | 0x0042d3a0 FUN_0042d3a0 | 0x00432080 FUN_00432080 | util | passthrough | 2026-05-03 | zeroes 13-entry 64-byte struct array 0x0067ed78..0x0067f0bc; shared callee with FUN_004331a0; D-2921 |
| S-1002 | 0x004248b0 FUN_004248b0 | 0x004331a0 FUN_004331a0 | util | passthrough | 2026-05-03 | called after zero-clear block in race-end init; body 0x004248b0..0x0042491f (111 bytes); D-2922 |
| S-1003 | 0x00424920 FUN_00424920 | 0x004331a0 FUN_004331a0 | util | passthrough | 2026-05-03 | called after FUN_004248b0 in race-end init; body 0x00424920..0x00424b7f (607 bytes); D-2923 |
| S-1004 | 0x004464c0 FUN_004464c0 | 0x00448700 FUN_00448700 | util | passthrough | 2026-05-03 | array iterator via DAT_00898994 count; stride 0xd8; dispatches on type+4 values 0/1/2; called 100 times; D-2924 |
| S-0920 | 0x00450b10 | 0x00428610 FUN_00428610 | hud | passthrough | 2026-05-03 | FUN_00450b10; primitive rect draw; called from viewport-scaled rect draw with (texture_handle, x, y, w, h, 0xffffffff, uv_ptr); depth-3 of hud_ingame_d2; D-2680 |
| S-0901 | 0x005a6710 FUN_005a6710 | 0x00462950 FUN_00462950 | render | passthrough | 2026-05-03 | RWS audio channel assignment; depth-2; D-2642 |
| S-0902 | 0x005a7aa0 FUN_005a7aa0 | 0x00462950 FUN_00462950 | render | passthrough | 2026-05-03 | RWS audio stream handle getter; (stream, desc+iVar6); depth-2; D-2642 |
| S-0903 | 0x005a7af0 FUN_005a7af0 | 0x00462950 FUN_00462950 | render | passthrough | 2026-05-03 | RWS audio stream count getter; (stream); returns DAT_00690478; depth-2; D-2642 |
| S-1060 | 0x00427f00 FUN_00427f00 | 0x00427f00 TEXT_FN | hud | passthrough | 2026-05-03 | needs FUN_00427840 (byte->UTF16) FUN_00427680 (pos calc) FUN_00556ca0 (dispatch) before TEXT_FN can be implemented |
| S-1061 | 0x00427ca0 FUN_00427ca0 | 0x00427ca0 HUD_text_init | hud | passthrough | 2026-05-03 | needs FUN_00552b60 FUN_0042a6b0 FUN_00556d70 before init can be fully implemented |
| S-1062 | 0x00554940 LAB_00554940 | 0x00554940 per_glyph_render | hud | passthrough | 2026-05-03 | needs RW Im2D vtable call implementations (DAT_007d3ff8 dispatch methods) |
| S-1063 | 0x00555360 FUN_00555360 | 0x00555360 font_sys_init | hud | passthrough | 2026-05-03 | needs thunk_FUN_004cc820 (alloc) FUN_004c5890 (font create) |
| S-1064 | 0x00427ff0 FUN_00427ff0 | 0x00427ff0 DrawText_shadow | hud | passthrough | 2026-05-03 | FUN_00552840(param_7) role unknown; D-3100 |
| S-1065 | 0x00555910 LAB_00555910 | 0x00555910 font_file_loader | hud | passthrough | 2026-05-03 | needs .met file parser internals (FUN_005507b0 + METRICS section handlers) |
| S-1066 | 0x0042c280 FUN_0042c280 | 0x0042c280 FUN_0042c280 | race_state | passthrough | 2026-05-03 | calls FUN_0042bf30(0x27f 0xff210000 0 0 0 0); role of FUN_0042bf30 unknown; D-3113 |
| S-1067 | 0x00432080 FUN_00432080 | 0x00432080 trigger_scan | race_state | passthrough | 2026-05-03 | needs FUN_0040e470 (slot check D-3114) + FUN_0042d3a0 (pre-commit D-3116) + FUN_00496900 (D-3115) |
| S-1068 | 0x004331a0 FUN_004331a0 | 0x004331a0 race_init | race_state | passthrough | 2026-05-03 | needs FUN_0042d3a0 (D-3116) + FUN_004348b0 (D-3117) + FUN_00424920 (S-1003 U-1011) |
| S-1069 | 0x00448700 FUN_00448700 | 0x00448700 FUN_00448700 | race_state | passthrough | 2026-05-03 | depends on FUN_004464c0 (S-1004 still open); 100-loop semantics unclear until FUN_004464c0 mapped |
| S-1070 | 0x004927c0 FUN_004927c0 | 0x004927c0 race_complete | race_state | passthrough | 2026-05-03 | unrecognized DataDB; U-1013 U-1014 need Frida before analysis can proceed |
| S-1160 | 0x004cc820 FUN_004cc820 | 0x004cc7f0 FUN_004cc7f0 | render | passthrough | 2026-05-03 | depth-3 callee; 6-arg call; 4th/5th args are literals 1 and 0; D-3400 |
| S-1161 | 0x004dcaa0 FUN_004dcaa0 | 0x004dc9e0 FUN_004dc9e0 | render | passthrough | 2026-05-03 | depth-3 callee; called as FUN_004dcaa0(0x40000, out_ptr); likely CreateVertexBuffer helper; D-3401 |
| S-1162 | DAT_007d46e0 | 0x004d1d70 FUN_004d1d70 | render | global | 2026-05-03 | D3D resource tracking linked-list head; node layout unknown |
| S-1163 | DAT_00911ae4 | 0x004d1d70 FUN_004d1d70 | render | global | 2026-05-03 | base pointer for D3D resource lookup array; array element layout unknown |
| S-1120 | 0x0042b950 FUN_0042b950 | 0x004030d0 FUN_004030d0 | util | passthrough | 2026-05-03 | called unconditionally after FUN_00472640(0); depth-2 from timer |
| S-1121 | 0x0043df00 FUN_0043df00 | 0x004030d0 FUN_004030d0 | util | passthrough | 2026-05-03 | called when DAT_00636af8==0 and accumulator overflow; depth-2 from timer |
| S-1122 | 0x00472640 FUN_00472640 | 0x004030d0 FUN_004030d0 | util | passthrough | 2026-05-03 | called with arg 0 after accumulator add; depth-2 from timer |
| S-1123 | 0x0040fc00 FUN_0040fc00 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | called at end of cases 3–7 and 10; ~1kb; role in state machine unknown |
| S-1124 | 0x0055dec0 FUN_0055dec0 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | returns mode value; result 5 or 6 gates FUN_004292d0/FUN_004292c0 path |
| S-1125 | 0x0042aab0 FUN_0042aab0 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | called unconditionally in case 2 (in-race tick) |
| S-1126 | 0x00426c30 FUN_00426c30 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | called in shared code path after FUN_0040e590 |
| S-1127 | 0x00426c70 FUN_00426c70 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | called in case 8; role unknown |
| S-1128 | 0x00422b10 FUN_00422b10 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | called in case 1 after wprintf("start of race") |
| S-1129 | 0x00426c10 FUN_00426c10 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | called in case 1 with dereferenced player pointer from PTR_PTR_005f2770 |
| S-1130 | 0x004292c0 FUN_004292c0 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | called with &DAT_0066d728 on stack (mode-5/6 path after FUN_004292d0) |
| S-1131 | 0x004292d0 FUN_004292d0 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | called immediately before FUN_004292c0 in mode-5/6 path |
| S-1132 | 0x0040ddb0 FUN_0040ddb0 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | called in case 9; DAT_0063ba8c set to 10 after return |
| S-1133 | 0x0040e590 FUN_0040e590 | 0x004111c0 FUN_004111c0 | util | passthrough | 2026-05-03 | first call in shared code path; ~3kb function |
| S-1280 | 0x004a2c48 FUN_004a2c48 | 0x00408a70 FUN_00408a70 | frontend | passthrough | 2026-05-03 | called at entry of put-precisepos setter; return value written to 4 per-car struct fields; semantics unknown |
| S-1281 | 0x0040e340 FUN_0040e340 | 0x0040b290 FUN_0040b290 | frontend | passthrough | 2026-05-03 | active-car counter; returns 4 when all 4 cars active; depth-2 from score-updater |
| S-1282 | 0x0040e350 FUN_0040e350 | 0x0040b290 FUN_0040b290 | frontend | passthrough | 2026-05-03 | game-state query; result compared to {1,2,3,4,5} to gate event-buffer append; depth-2 from score-updater |
| S-1283 | 0x0040e370 FUN_0040e370 | 0x0040b290 FUN_0040b290 | frontend | passthrough | 2026-05-03 | per-car active check; non-zero=active; depth-2 from score-updater |
| S-1284 | 0x004189f0 thunk_FUN_00419760 | 0x00422fd0 FUN_00422fd0 | frontend | passthrough | 2026-05-03 | thunk→FUN_00419760; called with (car_idx,1) when FUN_0040e470==1 and DAT_007f0fd0!=7; depth-2 from car-eliminator |
| S-1285 | 0x004215c0 FUN_004215c0 | 0x00422fd0 FUN_00422fd0 | frontend | passthrough | 2026-05-03 | called twice: (car,50.0f,0) and (car,50.0f,1); role unknown; depth-2 from car-eliminator |
| S-1286 | 0x0045ba00 FUN_0045ba00 | 0x00422fd0 FUN_00422fd0 | frontend | passthrough | 2026-05-03 | called with (car_idx,2); constant 2 matches FUN_0040e470 sub-state value; depth-2 from car-eliminator |
| S-1287 | 0x0046c5c0 FUN_0046c5c0 | 0x00422fd0 FUN_00422fd0 | frontend | passthrough | 2026-05-03 | first call in car-eliminate sequence; single car_idx arg; depth-2 from car-eliminator |
| S-1288 | 0x0046c790 FUN_0046c790 | 0x00422fd0 FUN_00422fd0 | frontend | passthrough | 2026-05-03 | called with (car_idx,1); second call in car-eliminate sequence; depth-2 from car-eliminator |
| S-1134 | 0x0043d2a0 FUN_0043d2a0 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | called with 2-arg pairs (0,2),(7,0),(1,0); central transition handler |
| S-1135 | 0x0042ae10 FUN_0042ae10 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | called with arg 0 in inner-switch cases 1,2,9,0xb,0xc; return gates state changes |
| S-1136 | 0x0042aeb0 FUN_0042aeb0 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | called in cases 1,2,9; non-zero result sets DAT_0067eac5 and DAT_0067f1a0 |
| S-1137 | 0x0042bf30 FUN_0042bf30 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | 6-arg call; two sites: DAT_0067ece8..ed04 and DAT_0067e918..e92c |
| S-1138 | 0x004298c0 FUN_004298c0 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | called in transition paths -0xdc0000 and -0xe20000 |
| S-1139 | 0x00413f90 FUN_00413f90 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | returns pointer used with DAT_0067f17c×0x30 stride; -0xd80000 player init |
| ~~S-1380~~ | ~~0x0049dd60 FUN_0049dd60~~ | ~~0x0049ec10 FUN_0049ec10~~ | util | **RESOLVED 2026-05-06** video_mci_d3-20260506-0512 | 2026-05-03 | 4-arg base/ancestor ctor; analyzed in video_mci_d3; see re/analysis/video_mci_d3/0x0049dd60.md |
| S-1560 | 0x00482860 FUN_00482860 | 0x00482930 Replay::New | vehicle | passthrough | 2026-05-03 | called at end of Replay::New and from StartLap on DAT_0063bb14; internal replay reset/rewind |
| S-1561 | 0x00483a30 FUN_00483a30 | 0x00411750 Replay::StartLap | vehicle | passthrough | 2026-05-03 | rewind DAT_0063bb10 to beginning of replay for playback; also called from LapFinish |
| S-1562 | 0x00483a40 FUN_00483a40 | 0x004114e0 Replay::Cleanup | vehicle | passthrough | 2026-05-03 | replay object destructor/free; called on both DAT_0063bb04 and DAT_0063bb08 |
| S-1563 | 0x0046d4a0 FUN_0046d4a0 | 0x00411600 Replay::RecordFrame | vehicle | passthrough | 2026-05-03 | returns vehicle state struct ptr for player index 0; also called from LapFinish |
| S-1564 | 0x00546b10 FUN_00546b10 | 0x004829d0 Replay::WriteFrame | vehicle | passthrough | 2026-05-03 | copies quaternion from vehicle state struct into frame node +0x00..+0x0f |
| S-1565 | 0x00483ca0 FUN_00483ca0 | 0x004117b0 Replay::Save | vehicle | passthrough | 2026-05-03 | serializes replay object DAT_0063bb10 into global buffer &DAT_008a94a8 |
| S-1566 | 0x004cc230 FUN_004cc230 | 0x00483d10 Replay::Load | vehicle | passthrough | 2026-05-03 | open stream for reading (mode 3,1); part of replay file I/O trio |
| S-1567 | 0x004cbd30 FUN_004cbd30 | 0x00483d10 Replay::Load | vehicle | passthrough | 2026-05-03 | read N bytes from stream into buffer; used for both 0x19c header and frame array |
| S-1568 | 0x004cc160 FUN_004cc160 | 0x00483d10 Replay::Load | vehicle | passthrough | 2026-05-03 | close stream and release; called on both success and failure paths |
| S-1569 | 0x0041a9b0 FUN_0041a9b0 | 0x00411ae0 Ghost::PlaybackTick | vehicle | passthrough | 2026-05-03 | setup ghost vehicle transform matrix from interpolated replay frame data |
| S-1570 | 0x0041ad00 FUN_0041ad00 | 0x00411ae0 Ghost::PlaybackTick | vehicle | passthrough | 2026-05-03 | apply ghost vehicle transform matrix to scene/render list |
| S-1571 | 0x0041a960 FUN_0041a960 | 0x00411ce0 Ghost::SetupRender | vehicle | passthrough | 2026-05-03 | ghost renderer initialisation; called when DAT_0063bb10 or DAT_0063bb0c is non-null |
| S-1572 | 0x00546e70 FUN_00546e70 | 0x00482c10 Replay::ReadFrame | vehicle | passthrough | 2026-05-03 | compute slerp interpolation between two adjacent frame node quaternions |
| S-1573 | 0x00482ae0 FUN_00482ae0 | 0x00482c10 Replay::ReadFrame | vehicle | passthrough | 2026-05-03 | quaternion blend helper; called with interpolation factor t and two matrix outputs |
| S-1480 | 0x005c4d30 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | glyph-data block accessor; depth-2 font_text_d2 |
| S-1480 | 0x00552d10 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | render-matrix setup before Im2D quad emission |
| S-1480 | 0x00552df0 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | set draw position (called twice: xy and z) |
| S-1480 | 0x00552da0 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | set draw color/alpha param |
| S-1480 | 0x00552e40 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | begin-render / Im2D batch flush |
| S-1481 | 0x005551d0 | 0x00555910 LAB_00555910 | hud | passthrough | 2026-05-03 | entry guard / current-name getter; U-1490; D-4362 |
| S-1481 | 0x00550a20 | 0x00555910 LAB_00555910 | hud | passthrough | 2026-05-03 | VFS read-line (buf, size, filehandle) |
| S-1481 | 0x00550580 | 0x005507b0 FUN_005507b0 | hud | passthrough | 2026-05-03 | VFS file-open implementation; D-4365 |
| S-1482 | 0x00553f40 | 0x005540d0 FUN_005540d0 | hud | passthrough | 2026-05-03 | glyph-data upload/lock; recursive on node[5] |
| S-1482 | 0x005c4c60 | 0x00553f40 FUN_00553f40 | hud | passthrough | 2026-05-03 | resize/alloc cnt×0x18 Im2D vertex buffer |
| S-1482 | 0x0055deb0 | 0x00553f40 FUN_00553f40 | hud | passthrough | 2026-05-03 | get vertex count from glyph-data block |
| S-1483 | 0x004c4a50 | 0x004c4d20 FUN_004c4d20 | hud | passthrough | 2026-05-03 | angle-axis matrix builder; D-4364 |
| S-1485 | 0x00556e90 | 0x00556d70 FUN_00556d70 | hud | passthrough | 2026-05-03 | set font-style RGBA color |
| S-1485 | 0x00557110 | 0x00556d70 FUN_00556d70 | hud | passthrough | 2026-05-03 | set font-style secondary param to zero |
| S-1360 | 0x005aabe0 | 0x005ba720 FUN_005ba720 + 0x005ba760 FUN_005ba760 | audio | passthrough | 2026-05-03 | FUN_005aabe0; called with *param_1; return value gates CoInitialize/CoUninitialize; audio_dsound_d2 |
| S-1361 | 0x005bc860 | 0x005ba720 FUN_005ba720 | audio | passthrough | 2026-05-03 | FUN_005bc860; called after CoInitialize; failure triggers CoUninitialize+return 0; audio_dsound_d2 |
| S-1362 | 0x005bc880 | 0x005ba760 FUN_005ba760 | audio | passthrough | 2026-05-03 | FUN_005bc880; 11 bytes; called only when FUN_005aabe0(*param_1)<2; audio_dsound_d2 |
| S-1363 | 0x005baf40 | 0x005ba780 LAB_005ba780 | audio | passthrough | 2026-05-03 | FUN_005baf40; called with (EDI, 0x0); role unknown; audio_dsound_d2 |
| S-1364 | 0x005aaa00 | 0x005ba780 LAB_005ba780 | audio | passthrough | 2026-05-03 | FUN_005aaa00; called with (EAX, 0, 0) when [ESI+0xf4]!=0; audio_dsound_d2 |
| S-1365 | 0x005aab70 | 0x005ba7f0 LAB_005ba7f0 | audio | passthrough | 2026-05-03 | FUN_005aab70; called at entry with (EDI, 0x1); audio_dsound_d2 |
| S-1366 | 0x005baa60 | 0x005ba7f0 LAB_005ba7f0 + 0x005bac00 FUN_005bac00 | audio | passthrough | 2026-05-03 | FUN_005baa60; called with ([EDI+0x80], [EDI+0x78], &[EDI+0x44]) or 3-arg variant; audio_dsound_d2 |
| S-1367 | 0x005bc750 | 0x005ba7f0 LAB_005ba7f0 | audio | passthrough | 2026-05-03 | FUN_005bc750; called with ([EDI+0xa8], &[EDI+0x44]) when bit 0x8 set and bits 0x7 clear; audio_dsound_d2 |
| S-1368 | 0x005aea50 | 0x005bb000 FUN_005bb000 | audio | passthrough | 2026-05-03 | FUN_005aea50; called with (1, size, 0x3080c); allocator for streaming buffer; audio_dsound_d2 |
| S-1369 | 0x005aeea0 | 0x005bb000 FUN_005bb000 | audio | passthrough | 2026-05-03 | FUN_005aeea0; called with (param_1+0x3d, 0, 1); likely event/handle init; audio_dsound_d2 |
| S-1370 | 0x005aef00 | 0x005bb000 FUN_005bb000 | audio | passthrough | 2026-05-03 | FUN_005aef00; called with (param_1+0x3e, &LAB_005bb380, 0xf, 0x1000); likely thread creation; audio_dsound_d2 |
| S-1371 | 0x005aef30 | 0x005bb000 FUN_005bb000 | audio | passthrough | 2026-05-03 | FUN_005aef30; called with (param_1+0x3e, param_1+0x27); likely thread start/bind; audio_dsound_d2 |
| S-1372 | 0x005bc470 | 0x005bb000 FUN_005bb000 | audio | passthrough | 2026-05-03 | FUN_005bc470; called with (&DAT_005d098c, format_struct, param_1+0x2a); bit-8 streaming path; audio_dsound_d2 |
| S-1373 | 0x005bc640 | 0x005bb000 FUN_005bb000 | audio | passthrough | 2026-05-03 | FUN_005bc640; called with (param_1+0x2a) on streaming buffer teardown; audio_dsound_d2 |
| S-1374 | 0x005bbd50 | 0x005bbc10 FUN_005bbc10 | audio | passthrough | 2026-05-03 | FUN_005bbd50; called with (param_1, param_5); returns COM interface ptr; audio_dsound_d2 |
| S-1375 | 0x005bbed0 | 0x005bbdb0 FUN_005bbdb0 | audio | passthrough | 2026-05-03 | FUN_005bbed0; called after vtable+0xc succeeds with (param_6, param_3, unaff_retaddr); audio_dsound_d2 |
| S-1440 | 0x004c0e50 FUN_004c0e50 | 0x004b3bf0 FUN_004b3bf0 | vehicle | passthrough | 2026-05-03 | init material/geometry ref after model load; called on *(atomic+4) result of FUN_004e7420 |
| S-1441 | 0x004547c0 FUN_004547c0 | 0x004548a0 FUN_004548a0 | vehicle | passthrough | 2026-05-03 | per-entry activator for DepthCharge struct-A (stride 0x2c, 0x00688240..0x006882f0); ESI-implicit; D-4240 |
| S-1441 | 0x00454170 FUN_00454170 | 0x004548a0 FUN_004548a0 | vehicle | passthrough | 2026-05-03 | per-entry activator for DepthCharge struct-B (stride 0x44, 0x00688020..0x00688240); ESI-implicit; D-4241 |
| S-1442 | 0x004b64e0 FUN_004b64e0 | 0x004b6520 FUN_004b6520 | vehicle | passthrough | 2026-05-03 | underlying 3-arg memset impl: (dst, val=0, size); thin wrapper over CRT memset |
| S-1443 | 0x004e66d0 FUN_004e66d0 | 0x004b3fc0 0x004b3f90 0x004b5320 0x004b5580 0x00474d60 | vehicle | passthrough | 2026-05-03 | RW ForAll dispatcher: (obj, callback, userdata); iterates sub-objects calling callback; core RW ForAll pattern |
| S-1444 | ~~0x00534b60 FUN_00534b60~~ | ~~0x004770c0 FUN_004770c0~~ | vehicle | CLEARED | powerups_d3-20260506-0504 | flags normalizer + allocator call; analyzed in powerups_d3 |
| S-1445 | 0x004c0b70 FUN_004c0b70 | 0x004c0b30 FUN_004c0b30 | vehicle | passthrough | 2026-05-03 | post-alloc init for RW type 0x3000e object; called immediately after vtable alloc |
| S-1446 | 0x004c0ad0 FUN_004c0ad0 | 0x004c1040 FUN_004c1040 | vehicle | passthrough | 2026-05-03 | propagate root-frame reference through children; called per child during RW frame reparent |
| S-1447 | 0x004e67b0 FUN_004e67b0 | 0x004e69a0 FUN_004e69a0 | vehicle | passthrough | 2026-05-03 | pool/heap allocator for RW sub-object (atomic/light) slots; returns ptr or NULL |
| S-1448 | 0x004ce2d0 FUN_004ce2d0 | 0x004781b0 FUN_004781b0 | vehicle | passthrough | 2026-05-03 | pre-seek on existing RW stream object; called when param_2!=0 in FUN_004781b0 |
| S-1449 | 0x004cc230 FUN_004cc230 | 0x004b3bf0 0x004b3e40 0x004781b0 0x004b3b70 | vehicle | passthrough | 2026-05-03 | RW stream open; args (mode, type, ptr); mode 2=file mode 3=memory; returns stream handle or 0 |
| S-1450 | 0x004cc5e0 FUN_004cc5e0 | 0x004b3bf0 0x004b3e40 0x004781b0 | vehicle | passthrough | 2026-05-03 | RW stream chunk seek; args (stream, chunk_type, p3, p4); seeks to chunk of given type |
| S-1451 | 0x004e7420 FUN_004e7420 | 0x004b3bf0 0x004b3e40 0x004781b0 | vehicle | passthrough | 2026-05-03 | RW load atomic/clump from open stream; returns atomic ptr or 0 |
| S-1452 | 0x004cc160 FUN_004cc160 | 0x004b3bf0 0x004b3e40 0x004781b0 0x004b3b70 | vehicle | passthrough | 2026-05-03 | RW stream close; args (stream, save_flag); 0 = close without save |
| S-1453 | 0x004c0740 FUN_004c0740 | 0x004e7e30 0x004e69a0 | vehicle | passthrough | 2026-05-03 | set material/texture reference on RW object; args (obj, mat_or_null); used widely as ref-setter |
| S-1540 | 0x0040dd60 FUN_0040dd60 | 0x00430290 | save | passthrough | 2026-05-03 | guard predicate in FUN_00430290; returns 0 to skip championship handler; semantics unknown (race-over? no-replay?) |
| S-1680 | 0x004d7d70 LAB_004d7d70 | 0x004d7ca0 FUN_004d7ca0 (via FUN_004ccce0 callback) | render | passthrough | 2026-05-03 | Unrecognized fn body (Ghidra label only, no FUN_ entry); 37 bytes; reads param_1+0x38 sub-struct; clears +0x10/+0x14 if non-zero; calls vtable DAT_007d3ff8+0x11c(param_1, param_2); deferred as D-4960 |
| S-1640 | 0x004332a0 FUN_004332a0 | 0x0043c000 FUN_0043c000 | util | passthrough | 2026-05-03 | timer action for slot 0 (DAT_0067e7a8==1); fires when slot-0 timer completes; semantics unknown |
| S-1641 | 0x0042f7b0 FUN_0042f7b0 | 0x0043c000 FUN_0043c000 | util | passthrough | 2026-05-03 | timer action for slot 6 (DAT_0067e7d8==1); fires when slot-6 timer completes; semantics unknown |
| S-1642 | ~~0x0042c960 FUN_0042c960~~ | ~~0x0043c000 FUN_0043c000~~ | render | CLEARED | split_screen-20260505 | CameraTransitionStateMachine; camera/intro-sequence state machine; DAT_0067ed68 transition slider; DAT_0067e9fc mode handler |
| S-1643 | 0x0042fa00 FUN_0042fa00 | 0x0043c000 FUN_0043c000 | util | passthrough | 2026-05-03 | timer action for slot 7 (DAT_0067e7e0==1); fires when slot-7 timer completes; semantics unknown |
| S-1644 | 0x00431f30 FUN_00431f30 | 0x0043d2a0 FUN_0043d2a0 | frontend | passthrough | 2026-05-03 | screen history restore; called with param_1 on push-0, with history index on pop-1; semantics unknown |
| S-1645 | 0x0042d3e0 FUN_0042d3e0 | 0x0043d2a0 FUN_0043d2a0 | frontend | passthrough | 2026-05-03 | screen pre-init; called before stack push/pop in FUN_0043d2a0; semantics unknown |
| S-1646 | 0x0042ad90 FUN_0042ad90 | 0x0043d2a0 FUN_0043d2a0 | frontend | passthrough | 2026-05-03 | player port query; returns short; -1 if no player; called 2x per loop iteration in FUN_0043d2a0 |
| S-1647 | 0x0042ac00 FUN_0042ac00 | 0x0043d2a0 FUN_0043d2a0 | frontend | passthrough | 2026-05-03 | called 2x (push and pop branches) in FUN_0043d2a0; no visible args; role unknown |
| S-1648 | 0x0042ac50 FUN_0042ac50 | 0x0043d2a0 FUN_0043d2a0 0x004325c0 FUN_004325c0 | frontend | passthrough | 2026-05-03 | position calculator for menu slide; returns int used as X/Y in slide table; called per visible entry |
| S-1649 | 0x00432b30 FUN_00432b30 | 0x0043d2a0 FUN_0043d2a0 | frontend | passthrough | 2026-05-03 | transition initiator; called at end of FUN_0043d2a0 with (short)sVar2 arg; starts actual screen transition |
| S-1650 | 0x00492d10 FUN_00492d10 | 0x00432800 FUN_00432800 | frontend | passthrough | 2026-05-03 | multiplayer online check (case 8 in FUN_00432800); returns 1 if online mode active; otherwise clear DAT_0067ed8c |
| S-1651 | 0x0042b930 FUN_0042b930 | 0x004324a0 FUN_004324a0 | frontend | passthrough | 2026-05-03 | current screen type query; returns screen ID int; 0x21 and 1 skip race-start attempt |
| S-1652 | 0x0042c1f0 FUN_0042c1f0 | 0x004324a0 FUN_004324a0 | frontend | passthrough | 2026-05-03 | SP race-start prerequisite check; returns nonzero if race can begin |
| S-1653 | 0x0042c1d0 FUN_0042c1d0 | 0x004324a0 FUN_004324a0 | frontend | passthrough | 2026-05-03 | MP race-start prerequisite check; called before FUN_0042c220 in MP path |
| S-1654 | 0x00432450 FUN_00432450 | 0x004324a0 FUN_004324a0 | frontend | passthrough | 2026-05-03 | MP race transition; same 6-param signature as FUN_0042bf30; used for non-zero eab0 path |
| S-1655 | 0x0042aa00 FUN_0042aa00 | 0x004322c0 FUN_004322c0 | frontend | passthrough | 2026-05-03 | called first in FUN_004322c0 with arg 1; role unknown; may update track-display state |
| S-1656 | 0x00430910 FUN_00430910 | 0x004322c0 FUN_004322c0 | frontend | passthrough | 2026-05-03 | per-option validity check in track-selection loop; returns 0 if option valid; used to skip invalid indices |
| S-1720 | 0x005aa1e0 FUN_005aa1e0 | 0x005aa060 FUN_005aa060 | audio | passthrough | 2026-05-03 | inline callback at LAB_005aa1e0; no Ghidra function body; passed as code ptr to FUN_005aa0c0; D-5080 |
| S-1600 | 0x00409930 FUN_00409930 | 0x0040ab40 FUN_0040ab40 | util | passthrough | 2026-05-03 | called in case 6 of FUN_0040ab40 alongside FUN_0042c1a0 |
| S-1601 | 0x00409970 FUN_00409970 | 0x0040ab40 FUN_0040ab40 | util | passthrough | 2026-05-03 | called on sub-state 7/8 in multiple cases of FUN_0040ab40 |
| S-1602 | 0x0042c1a0 FUN_0042c1a0 | 0x0040ab40 0x0040ac80 | util | passthrough | 2026-05-03 | called on case 6 and case 4 range-guarded paths; shared by two state dispatchers |
| S-1603 | 0x00405420 FUN_00405420 | 0x0040de10 FUN_0040de10 | util | passthrough | 2026-05-03 | called after Replay::StartLap and DAT_0063ba8c=1 in FUN_0040de10 |
| S-1604 | 0x00430820 FUN_00430820 | 0x00429aa0 FUN_00429aa0 | util | passthrough | 2026-05-03 | predicate; non-zero selects table at 0x00614720 over flat arrays |
| S-1605 | 0x00430790 FUN_00430790 | 0x00429aa0 FUN_00429aa0 | util | passthrough | 2026-05-03 | index provider; called 3× per FUN_00429aa0 invocation |
| S-1606 | 0x0045c810 FUN_0045c810 | 0x0045d3a0 FUN_0045d3a0 | util | passthrough | 2026-05-03 | first call in FUN_0045d3a0 before mode-2 guard |
| S-1607 | 0x0045d330 FUN_0045d330 | 0x0045d3a0 FUN_0045d3a0 | util | passthrough | 2026-05-03 | called once before the per-count loop in FUN_0045d3a0 |
| S-1608 | 0x0045d1e0 FUN_0045d1e0 | 0x0045d3a0 FUN_0045d3a0 | util | passthrough | 2026-05-03 | loop body called for each index 0..(DAT_008aa254-1) |
| S-1609 | 0x004657b0 FUN_004657b0 | 0x0045d3a0 FUN_0045d3a0 | util | passthrough | 2026-05-03 | called after per-count loop before FUN_0040e350 store |
| S-1610 | 0x004417e0 FUN_004417e0 | 0x0045d7a0 FUN_0045d7a0 | util | passthrough | 2026-05-03 | fills float triple local_64/60/5c in FUN_0045d7a0 |
| S-1611 | 0x00472820 FUN_00472820 | 0x0045d7a0 FUN_0045d7a0 | util | passthrough | 2026-05-03 | validates float triple; returns 0 to zero it out |
| S-1612 | 0x004728a0 FUN_004728a0 | 0x0045d7a0 FUN_0045d7a0 | util | passthrough | 2026-05-03 | validates 16-uint block local_40; returns 0 to default-init it |
| S-1760 | 0x004492b0 SkyDomeRender | - | render | needs-fn-create | 2026-05-03 | SKY_FN; 378-byte function at 0x004492b0-0x0044942a; Ghidra has no function record; needs function_create in writable session before C2 work |
| S-1761 | 0x00499d90 FUN_00499d90 | 0x004914b0 RainRender | render | passthrough | 2026-05-03 | called as FUN_00499d90(DAT_00771530, 0x380) in RainRender; likely low-level particle vertex-buffer flush; not yet analyzed |
| S-1762 | 0x00491340 FUN_00491340 | 0x00491490 FUN_00491490 | render | passthrough | 2026-05-03 | rain update variant; dispatched by FUN_00491490 based on DAT_007f108b; not analyzed |
| S-1763 | 0x0047a034 FUN_0047a034 | 0x0047a020 FUN_0047a020 | track | passthrough | 2026-05-03 | COURSE.LUA executor; writes DAT_006bf1cc (course description base pointer); 47 sky/fog handlers depend on this pointer |
| S-1700 | 0x004e4320 FUN_004e4320 | 0x00426670 sub_00426670 | render | passthrough | 2026-05-03 | WorldRenderDispatch_Begin callee; args (world_ptr camera); RpWorldRender-family; D-5020 |
| S-1701 | 0x004e4350 FUN_004e4350 | 0x004266b0 sub_004266b0 | render | passthrough | 2026-05-03 | WorldRenderDispatch_End callee; paired end to FUN_004e4320; D-5021 |
| S-1702 | 0x0041ea10 FUN_0041ea10 | 0x00426030 sub_00426030 | render | passthrough | 2026-05-03 | WorldRenderPrePass predicate; no args; gates FUN_0041e8f0; D-5022 |
| S-1703 | 0x0041e8f0 FUN_0041e8f0 | 0x00426030 sub_00426030 | render | passthrough | 2026-05-03 | WorldRenderPrePass setup; called with &DAT_00646e58; D-5023 |
| S-1704 | 0x00401f10 FUN_00401f10 | 0x0040df60 sub_0040df60 | render | passthrough | 2026-05-03 | conditional render sub-pass modes 4/7/8/9/10 + player-count 5-6 + state 3-5; D-5024 |
| S-1705 | 0x00403d30 FUN_00403d30 | 0x00404320 sub_00404320 | render | passthrough | 2026-05-03 | PerModeRenderMachine mode-9 state-3/4/5 renderer; D-5025 |
| S-1706 | 0x00403fa0 FUN_00403fa0 | 0x00404320 sub_00404320 | render | passthrough | 2026-05-03 | mode-10 state-6 per-entry render loop over 0x636b88; D-5028 |
| S-1707 | 0x0042c010 FUN_0042c010 | 0x00433f40 sub_00433f40 | render | passthrough | 2026-05-03 | screen-space rect fill (x1 y1 x2 y2 color); RaceEndFadeOverlay; D-5030 |
| S-1708 | 0x004c7760 FUN_004c7760 | 0x0042f530 sub_0042f530 | render | passthrough | 2026-05-03 | raster copy src camera+0x60/0x64 → dest; ViewportSetup; D-5035 |
| S-1709 | 0x00420050 FUN_00420050 | 0x00410b30 sub_00410b30 | render | passthrough | 2026-05-03 | per-player render pass; 4× loop (i camera); D-5038 |
| S-1710 | 0x0041ebb0 FUN_0041ebb0 | 0x00410b30 sub_00410b30 | render | passthrough | 2026-05-03 | world render with camera arg; InGameRenderDispatcher; D-5039 |
| S-1711 | 0x00413a00 FUN_00413a00 | 0x00410b30 sub_00410b30 | render | passthrough | 2026-05-03 | render callee; called with primary player camera; D-5052 |
| S-1712 | 0x00448730 FUN_00448730 | 0x00410b30 sub_00410b30 | render | passthrough | 2026-05-03 | HUD/frontend layer in InGameRenderDispatcher; D-5057 |
| S-1860 | 0x0046cbb0 FUN_0046cbb0 | 0x00410510 Race::EvaluateResult | save | passthrough | 2026-05-05 | 3-param: reads DAT_00881f90[param_1*0x341] (DNF-flag) and [+1] (lap-count) for player; returns 1 if param_1<16; check local_c==0 follows in callers |
| S-1861 | 0x00405890 FUN_00405890 | 0x00410510 Race::EvaluateResult | save | passthrough | 2026-05-05 | mode-5 all-targets-hit predicate: returns (DAT_0063a5d4 == DAT_0063a5d0); false if DAT_0063a5d0==0 |
| S-1862 | 0x00417730 FUN_00417730 | 0x00410510 Race::EvaluateResult | save | passthrough | 2026-05-05 | player score/time float getter: returns (float10)(float)(&DAT_0089a880)[param_1]; array at 0x0089a880 |
| S-1863 | 0x00417740 FUN_00417740 | 0x00410510 Race::EvaluateResult | save | passthrough | 2026-05-05 | 15B; calls FUN_004a2c48 (x87 ROUND); probable round/rank getter for modes 4/7; Ghidra shows void but caller uses int return; U-1869 |
| S-1864 | 0x0044df80 FUN_0044df80 | 0x00410510 Race::EvaluateResult | save | passthrough | 2026-05-05 | called when FUN_00426c00()==0x26 at race conclusion; 0x50 bytes; purpose unknown |
| S-1865 | 0x00458f80 FUN_00458f80 | 0x00410510 Race::EvaluateResult | save | passthrough | 2026-05-05 | race-end state setter; param_1=0; 0x18 bytes |
| S-1866 | 0x0045bed0 FUN_0045bed0 | 0x00410510 Race::EvaluateResult | save | passthrough | 2026-05-05 | race-end teardown A; no params; also called from FUN_0040cfd0 seq init |
| S-1867 | 0x0045bf30 FUN_0045bf30 | 0x00410510 Race::EvaluateResult | save | passthrough | 2026-05-05 | race-end teardown B; no params; also called from FUN_0040cfd0 seq init |
| S-1868 | 0x00441990 FUN_00441990 | 0x00448220 Frontend::PostRaceResultCamera | frontend | passthrough | 2026-05-05 | alternative result handler when DAT_007f1a50==1; sole caller FUN_00448220 early-out path; D-5500 |
| S-1869 | 0x00446520 FUN_00446520 | 0x00448220 Frontend::PostRaceResultCamera | frontend | passthrough | 2026-05-05 | main result display state machine; called with (&DAT_00897fe0, param_3) on race states 6/7/-1; D-5501 |
| S-1840 | 0x0046cbb0 FUN_0046cbb0 | 0x0040e180 FUN_0040e180 / 0x00410d10 FUN_00410d10 | vehicle | passthrough | 2026-05-05 | per-car state reader; (car,&state_out,&extra_out); state==0 not-destroyed; analyzed this session (see 0x0046cbb0.md) |
| S-1841 | 0x004922e0 FUN_004922e0 | 0x00410d10 FUN_00410d10 | vehicle | passthrough | 2026-05-05 | hit-sound/particle trigger; args (car 3 10 0x80); analyzed this session (see 0x004922e0.md) |
| S-1842 | 0x0040e350 FUN_0040e350 | 0x004922e0 FUN_004922e0 | vehicle | resolved | 2026-05-06 | no-arg getter; RESOLVED vehicle_damage_d3-20260506-1244: returns DAT_0063ba8c; gate ==6 in FUN_004922e0; re/analysis/vehicle_damage_d3/0x0040e350.md; U-2172 |
| U-1947 | 0x004b3c60 FUN_004b3c60 | 0x0042a640 FUN_0042a640 | render | passthrough | 2026-05-06 | BSP/RpWorld stream reader; called after piz lookup succeeds; depth-3 of track_loader_d3; D-5740 |
| U-1948 | 0x00558df0 FUN_00558df0 | 0x0042a740 FUN_0042a740 | render | passthrough | 2026-05-06 | UVAnim chunk loader; (plugin_ptr, stream) → anim handle; depth-3; D-5741 |
| U-1949 | 0x004b3cc0 FUN_004b3cc0 | 0x0042a7f0 FUN_0042a7f0 | render | passthrough | 2026-05-06 | spline stream reader; depth-3 of track_loader_d3; D-5742 |
| U-1950 | 0x004b3de0 FUN_004b3de0 | 0x0042a860 FUN_0042a860 | render | passthrough | 2026-05-06 | animation stream reader; depth-3 of track_loader_d3; D-5743 |
| U-1951 | 0x00479030 FUN_00479030 | 0x004790e0 FUN_004790e0 | render | passthrough | 2026-05-06 | course post-load callback table; depth-4 callee of course post-load thunk; D-5744 |
| U-1952 | 0x00474fb0 FUN_00474fb0 | 0x00474fd0 FUN_00474fd0 | render | passthrough | 2026-05-06 | DFF clump node iterator; (clump, callback_struct); depth-3 of sky dome builder; D-5745 |
| U-1953 | 0x00474f30 FUN_00474f30 | 0x00474fd0 FUN_00474fd0 | render | passthrough | 2026-05-06 | sky dome per-node callback; processes one RpFrame; depth-3; D-5746 |
| U-1954 | 0x0047f4c0 FUN_0047f4c0 | 0x0047f840 FUN_0047f840 | render | passthrough | 2026-05-06 | physics world constructor; (scale:1.0f) → world handle; depth-3 of physics init; D-5747 |
| U-1955 | 0x0047d080 FUN_0047d080 | 0x00480100 FUN_00480100 | render | passthrough | 2026-05-06 | activate physics body slot; (idx, 1); depth-3 of physics post-init; D-5748 |
| U-1956 | 0x0047d100 FUN_0047d100 | 0x00480100 FUN_00480100 | render | passthrough | 2026-05-06 | secondary enable physics body; (idx, 1); depth-3; D-5749 |
| U-1957 | 0x00487280 FUN_00487280 | 0x00480100 FUN_00480100 | render | passthrough | 2026-05-06 | broadphase body registration; (bvh, transform, bounds, out, slot); depth-3; D-5750 |
| U-1958 | 0x0047be80 FUN_0047be80 | 0x0047bf70 FUN_0047bf70 | render | passthrough | 2026-05-06 | triangle mesh init; (tri_count); depth-3 of collision sector builder; D-5751 |
| U-1959 | 0x0047bcc0 FUN_0047bcc0 | 0x0047bf70 FUN_0047bf70 | render | passthrough | 2026-05-06 | collect portal/neighbor list; (&list_ptr, chain_head); depth-3; D-5752 |
| U-1960 | 0x004b53b0 FUN_004b53b0 | 0x0047bf70 FUN_0047bf70 | render | passthrough | 2026-05-06 | bounding sphere builder; (out+0x4e, bounding_data, count); depth-3; D-5753 |
| U-1961 | 0x004c3d90 FUN_004c3d90 | 0x0047bf70 FUN_0047bf70 | render | passthrough | 2026-05-06 | sector bounding geometry builder; (out+0x24, mesh_chunk, vertex_info, flags); depth-3; D-5754 |
| U-1962 | 0x00546380 FUN_00546380 | 0x0045e2a0 FUN_0045e2a0 | render | passthrough | 2026-05-06 | audio waypoint set constructor; (count, type, points_array) → handle → DAT_0068f618; depth-3; D-5755 |
| S-2120 | 0x004cd070 FUN_004cd070 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-06 | Im2D render-primitive flush; called with (base_ptr, ptr, count, 5=TRISTRIP) @ 0x00554afa; D-6288 |
| S-2121 | 0x004cd170 FUN_004cd170 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-06 | Im2D secondary batch call; called with (EBX, 0x912700, 3) @ 0x00554b0e; D-6288 |
| S-2122 | 0x004cd140 FUN_004cd140 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-06 | Im2D begin/reset; no args @ 0x00554b16; D-6288 |
| S-2123 | 0x005c4ad0 FUN_005c4ad0 | 0x005551d0 FontCtx_Alloc | hud | passthrough | 2026-05-06 | alloc glyph data buffer (0x20 bytes, type 0x30190); distinct from 0x005c4d30; D-6289 |
| S-2124 | 0x004c4600 FUN_004c4600 | 0x00552e40 FontCtx_FlushMatrix | render | passthrough | 2026-05-06 | matrix multiply/compose; called as FUN_004c4600(dst, src1, src2, scale); D-6284 |
| S-2125 | 0x004c4dc0 FUN_004c4dc0 | 0x00552e40 FontCtx_FlushMatrix | render | passthrough | 2026-05-06 | matrix invert or copy into DAT_00912b58; D-6285 |
| S-2126 | 0x004c0ed0 FUN_004c0ed0 | 0x00552e40 FontCtx_FlushMatrix | render | passthrough | 2026-05-06 | camera view matrix getter; takes cam+4 field; returns RwMatrix*; D-6286 |
| S-2127 | 0x00552d70 FUN_00552d70 | 0x00427f00 FUN_00427f00 | hud | passthrough | 2026-05-06 | FontMatrix_Pop (counterpart to FontMatrix_Push); called @ 0x00427fc9 after draw; D-6287 |
| S-1920 | 0x004c1210 FUN_004c1210 | 0x00454170 FUN_00454170 | gameplay | passthrough | 2026-05-06 | conditional call; arg = ESI+0x34; called if *(ESI[0xd]+4) non-zero; depth-4 of FUN_00454170; D-5680 |
| S-1921 | 0x004c15c0 FUN_004c15c0 | 0x00454170 FUN_00454170 | gameplay | passthrough | 2026-05-06 | called twice; args ESI+0x34 and ESI+0x38; depth-4 of FUN_00454170; D-5680 |
| S-1922 | 0x004e43b0 FUN_004e43b0 | 0x00454170 FUN_00454170 | gameplay | passthrough | 2026-05-06 | called if FUN_004e4440 returns non-zero; args (return-value ESI+0x30); depth-4; D-5680 |
| S-1923 | 0x004e4800 FUN_004e4800 | 0x004547c0 FUN_004547c0 | gameplay | passthrough | 2026-05-06 | called with *(EDI+0x20); conditional; depth-4 of FUN_004547c0; D-5681 |
| S-1924 | 0x00534d00 FUN_00534d00 | 0x00534b60 FUN_00534b60 | gameplay | passthrough | 2026-05-06 | actual particle allocator; called with (param_1 normalizedFlags param_3 &DAT_00623c78); depth-4; D-5682 |
| S-1925 | 0x004c0910 FUN_004c0910 | 0x004c0870 FUN_004c0870 | gameplay | passthrough | 2026-05-06 | called with (param_1 0); returns object handle used for DLL insert; depth-4; D-5683 |
| S-1926 | 0x004c0d70 FUN_004c0d70 | 0x004c0de0 FUN_004c0de0 | gameplay | passthrough | 2026-05-06 | per-child teardown call in FUN_004c0de0 child-iteration loop; depth-4; D-5684 |
| S-1927 | 0x004e8e90 FUN_004e8e90 | 0x004e68a0 FUN_004e68a0 | gameplay | passthrough | 2026-05-06 | called with new value param_2 when *(param_1+0x18) changes; depth-4 of FUN_004e68a0; D-5685 |
| S-1928 | 0x004e8ea0 FUN_004e8ea0 | 0x004e68a0 FUN_004e68a0 / 0x004e6920 FUN_004e6920 | gameplay | passthrough | 2026-05-06 | called with old *(param_1+0x18) on field change; depth-4; D-5685 |
| S-1929 | 0x004d8bd0 FUN_004d8bd0 | 0x004e6920 FUN_004e6920 | gameplay | passthrough | 2026-05-06 | called with *(param_1+0x14); depth-4 of FUN_004e6920; D-5686 |
| S-1930 | 0x004d8000 FUN_004d8000 | 0x004e6d80 FUN_004e6d80 | gameplay | passthrough | 2026-05-06 | list insert; called with (&DAT_0061867c puVar2); depth-4; also in DEFERRED D-0231 D-0526 |
| S-2000 | 0x004a0ef0 FUN_004a0ef0 | 0x0049dd60 FUN_0049dd60 | video | passthrough | 2026-05-06 | 93b __thiscall; receives (param_3, param_4, &this+0x7c, param_2) from FUN_0049dd60 before vtable writes; depth-4 from video_mci; D-5920 |
| S-2001 | 0x004a1160 FUN_004a1160 | 0x0049dd60 FUN_0049dd60 | video | passthrough | 2026-05-06 | 27b __thiscall; called 3x on &this+0x54/0x58/0x5c with args 0/1/1; result at 0x5c used as HANDLE in SetEvent; depth-4 from video_mci; D-5921 |
| S-1960 | 0x004e5fc0 FUN_004e5fc0 | 0x004e6100 FUN_004e6100 | render | passthrough | 2026-05-06 | called when *(atomic+0x4c) & 2; 0x13D bytes; depth-4 of effects_particle; U-1967 |
| S-1961 | 0x004c0b10 FUN_004c0b10 | 0x004e6100 FUN_004e6100 | render | passthrough | 2026-05-06 | test on secondary object *(atomic+4); 0x10 bytes; returns int; triggers update path; U-1968 |
| S-1962 | 0x004c0ed0 FUN_004c0ed0 | 0x004e6100 FUN_004e6100 | render | passthrough | 2026-05-06 | get float* (matrix) from *(atomic+4); 0x1E bytes; U-1969 |
| S-1963 | 0x004c3d60 FUN_004c3d60 | 0x004e6100 FUN_004e6100 | render | passthrough | 2026-05-06 | matrix concat (3-arg: out ptr+0x2c, in1 ptr+0x1c, in2 frame-matrix); 0x25 bytes; U-1970 |
| S-1964 | 0x00547bf0 FUN_00547bf0 | 0x00539900 FUN_00539900 | render | passthrough | 2026-05-06 | AABB vs triangle pre-test (4 args: query_desc v0 v1 v2); 0x5AE bytes; U-1976 |
| S-1965 | 0x00547450 FUN_00547450 | 0x00539ec0 FUN_00539ec0 | render | passthrough | 2026-05-06 | sphere vs triangle intersection (6 args including out-ptrs for normal+dist); 0x39E bytes; U-1978 |
| S-2087 | 0x005555b0 FUN_005555b0 | 0x00427ad0 FUN_00427ad0 | frontend | passthrough | 2026-05-06 | main sprite draw call; 6 args: (DAT_0067d838, 512B stack buf, scaled param_7, &local_214, 1, DAT_0067d83c); depth-4 of FUN_00427ad0 |
| S-2089 | 0x005554d0 FUN_005554d0 | 0x004282a0 FUN_004282a0 / 0x00428320 FUN_00428320 | frontend | passthrough | 2026-05-06 | text width measurement; (DAT_0067d838, 1024B stack buf, param_2*scale); returns float10; depth-4 |
| S-2090 | 0x00427840 FUN_00427840 | 0x00428320 FUN_00428320 | frontend | passthrough | 2026-05-06 | font setup variant B; no args; replaces FUN_00427780+FUN_004277a0 pair; depth-4 of FUN_00428320 |
| S-2200 | 0x005a7520 | 0x005a66d0 FUN_005a66d0 | audio | passthrough | 2026-05-06 | FUN_005a7520; (ptr, mode); mode 0→FUN_005a7560 (stop?), 1→FUN_005a7460 (play?), 2→FUN_005a75b0 (conditional); depth-3 of audio_music; D-6520 |
| S-2201 | 0x005a6d60 | 0x005a6dc0 FUN_005a6dc0 | audio | passthrough | 2026-05-06 | FUN_005a6d60; (param_1, param_2, param_3, &param_4); actual audio parameter setter; param_4 by address; depth-3 of audio_music; D-6521 |
| S-2220 | 0x00430b00 FUN_00430b00 | 0x00429310 TimeTrial::Tick | util | passthrough | 2026-05-06 | HUD time display updater; args: (split_idx, is_best_flag, time_frac, time_sec, time_min); called 4× per lap-complete in TimeTrial::Tick; leaderboard-20260506-JJJJJ |
| S-2225 | 0x0042f790 FUN_0042f790 | 0x0040d270 Course::Finish | util | passthrough | 2026-05-06 | ghost-mode flag getter; 0=normal race, non-0=ghost; guards full finish path in Course::Finish; leaderboard-20260506-JJJJJ |
| S-2226 | 0x0040d040 FUN_0040d040 | 0x0040d270 Course::Finish | util | passthrough | 2026-05-06 | car finish validator; (param_1, param_2); returns non-0 on valid finish; leaderboard-20260506-JJJJJ |
| S-2360 | 0x004a3f90 __global_unwind2 | 0x005c1ea0 _longjmp | boot | passthrough | 2026-05-06 | SEH global unwind; called at 0x005c1eb3 when saved ExceptionList != FS:[0x0]; depth-3 of memory_pool root; D-7000 |
| S-2361 | 0x004a3fd2 __local_unwind2 | 0x005c1ea0 _longjmp | boot | passthrough | 2026-05-06 | SEH local scope unwind; called at 0x005c1ee8 with (ExceptionList, _Buf[7]); taken when VC20 guard absent; depth-3 of memory_pool root; D-7001 |
| S-2362 | 0x004a4066 FUN_004a4066 | 0x005c1ea0 _longjmp | boot | passthrough | 2026-05-06 | called at 0x005c1ef5 with arg 0 before register restore; thread/signal state reset; depth-3 of memory_pool root; D-7002 |
| S-2363 | 0x005c318c __rt_probe_read4@4 | 0x005c1ea0 _longjmp | boot | passthrough | 2026-05-06 | readable probe for _Buf[0x20]; called at 0x005c1ec4; returns 0 if address unreadable; depth-3 of memory_pool root; D-7003 |
| S-2320 | 0x00550980 FUN_00550980 | 0x004cbe80 FUN_004cbe80 | save | passthrough | 2026-05-06 | 4-arg fwrite-style call; (buf, 1, size, file_handle); case 1/2 file path of stream-write; depth-3 from save_gamesave; DEFERRED D-6880 |
| S-2380 | 0x0040e170 FUN_0040e170 | 0x0043df00 FUN_0043df00 | frontend | passthrough | 2026-05-06 | 9-byte function; called with arg 0; depth-1 callee of frontend game-session initializer; D-7060 |
| S-2400 | 0x004b7a70 FUN_004b7a70 | 0x0047b8a0 FUN_0047b8a0 | input | passthrough | 2026-05-06 | internal; depth-3 from input_lua root; only callee of 0x0047b8a0; not external import; D-7120 |
| S-2401 | 0x004ba1b0 FUN_004ba1b0 | 0x004b7330 FUN_004b7330 | input | passthrough | 2026-05-06 | allocator; args (0,0,0x70); depth-3; also called by 0x004b7480; D-7121 |
| S-2402 | 0x004b7be0 FUN_004b7be0 | 0x004b7330 FUN_004b7330 | input | passthrough | 2026-05-06 | receives (block, &LAB_004b73e0, &stack+4); fail triggers FUN_004b7480; depth-3; D-7122 |
| S-2403 | 0x004c06c0 FUN_004c06c0 | 0x004c0510 FUN_004c0510 | input | passthrough | 2026-05-06 | args (param_1, &PTR_PTR_005d8a70, 0x17); depth-3; D-7123 |
| S-2404 | 0x004b7200 thunk_FUN_004b7fd0 | 0x004c0510 FUN_004c0510 | input | passthrough | 2026-05-06 | thunk to FUN_004b7fd0; args (param_1, &LAB_004c0560, 0); depth-3; D-7124 |
| S-2405 | 0x004b9730 FUN_004b9730 | 0x004c0510 FUN_004c0510 | input | passthrough | 2026-05-06 | args (param_1, 2, &PTR_DAT_00617530); depth-3; D-7125 |
| S-2406 | 0x004b7140 FUN_004b7140 | 0x004c0510 FUN_004c0510 | input | passthrough | 2026-05-06 | args (param_1, 0x54442d18, 0x400921fb); depth-3; D-7126 |
| S-2407 | 0x004b7250 FUN_004b7250 | 0x004c0510 FUN_004c0510 | input | passthrough | 2026-05-06 | args (param_1, &DAT_00617f34); depth-3; D-7127 |
| S-2408 | 0x004ba210 FUN_004ba210 | 0x004b7480 FUN_004b7480 | input | passthrough | 2026-05-06 | args (param_1, 1); depth-3; D-7128 |
| S-2409 | 0x004b9850 FUN_004b9850 | 0x004b7480 FUN_004b7480 | input | passthrough | 2026-05-06 | args (param_1); depth-3; D-7129 |
| S-2410 | 0x004b64e0 FUN_004b64e0 | 0x004b6520 FUN_004b6520 | input | passthrough | 2026-05-06 | receives (param_1, 0, param_2); 57 bytes; depth-3; D-7130 |
| S-2460 | 0x00428450 FUN_00428450 | 0x00428a30 FUN_00428a30 / 0x00428d30 FUN_00428d30 | frontend | passthrough | 2026-05-06 | ticker/overlay; args (0x20, 0xffffffe0) in title context, (0x10, 0x100) in lobby context; depth-2 of title_screen; D-7300 |
| S-2461 | 0x004288a0 FUN_004288a0 | 0x00428a30 FUN_00428a30 | frontend | passthrough | 2026-05-06 | dark/blank screen renderer; called when DAT_0067d84c==0 (assets not ready); depth-2 of title_screen; D-7301 |
| S-2462 | 0x00428320 FUN_00428320 | 0x00428a30 FUN_00428a30 / 0x00428bf0 FUN_00428bf0 | frontend | passthrough | 2026-05-06 | text renderer; renders build date "Jun 14 2004, 11:39:38" and string 0x222; depth-2 of title_screen; D-7302 |
| S-2463 | 0x0042e590 FUN_0042e590 | 0x00428bf0 FUN_00428bf0 | frontend | passthrough | 2026-05-06 | sprite draw; car sprite at (320.0, 260.0) in attract renderer; sprite table structure unknown; depth-2 of title_screen; D-7303 |
| S-2464 | 0x0040d250 FUN_0040d250 | 0x00428d30 FUN_00428d30 | frontend | passthrough | 2026-05-06 | lobby visual element (gradient/line/logo path); depth-2 of title_screen; D-7304 |
| S-2465 | 0x00401ee0 FUN_00401ee0 | 0x00428d30 FUN_00428d30 | frontend | passthrough | 2026-05-06 | lobby visual element; depth-2 of title_screen; D-7305 |
| S-2466 | 0x0042f0b0 FUN_0042f0b0 | 0x00428d30 FUN_00428d30 | frontend | passthrough | 2026-05-06 | lobby UI sub-renderer at (347.5, 168.0); depth-2 of title_screen; D-7306 |
