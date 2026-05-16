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
| ~~S-0040~~ | ~~0x004a45cf~~ | ~~0x004a45fb _malloc~~ | boot | resolved | 2026-05-12 | RESOLVED boot_crt_env_cont1: __nh_malloc C1 at re/analysis/boot_crt_env_cont1/0x004a45cf.md |
| ~~S-0041~~ | ~~0x004a4660~~ | ~~0x004a460d _free~~ | boot | resolved | 2026-05-12 | RESOLVED boot_crt_env_cont1: FUN_004a4660 C1 at re/analysis/boot_crt_env_cont1/0x004a4660.md |
| ~~S-0042~~ | ~~0x004a787f~~ | ~~0x004a460d _free~~ | boot | resolved | 2026-05-12 | RESOLVED boot_crt_env_cont1: drift-skip; __lock already C1 in boot_crt_exit_d3 |
| ~~S-0043~~ | ~~0x004aa497~~ | ~~0x004a460d _free~~ | boot | resolved | 2026-05-12 | RESOLVED boot_crt_env_cont1: ___sbh_find_block C1 at re/analysis/boot_crt_env_cont1/0x004aa497.md |
| ~~S-0044~~ | ~~0x004aa4c2~~ | ~~0x004a460d _free~~ | boot | resolved | 2026-05-12 | RESOLVED boot_crt_env_cont1: ___sbh_free_block C1 at re/analysis/boot_crt_env_cont1/0x004aa4c2.md |
| ~~S-0045~~ | ~~0x004ae28f~~ | ~~0x004ae29f ___crtInitCritSecAndSpinCount~~ | boot | resolved | 2026-05-12 | RESOLVED boot_crt_env_cont1: ___crtInitCritSecNoSpinCount@8 C1 at re/analysis/boot_crt_env_cont1/0x004ae28f.md |
| ~~S-0046~~ | ~~0x004af166~~ | ~~0x004af2b6 ___initmbctable~~ | boot | resolved | 2026-05-12 | RESOLVED boot_crt_env_cont1: __setmbcp C1 at re/analysis/boot_crt_env_cont1/0x004af166.md |
| ~~S-0047~~ | ~~0x004affaf~~ | ~~0x004affe0 FUN_004affe0~~ | boot | resolved | 2026-05-12 | RESOLVED boot_crt_env_cont1: FUN_004affaf C1 at re/analysis/boot_crt_env_cont1/0x004affaf.md |
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
| S-0080 | 0x00551510 FUN_00551510 | 0x00550390 FUN_00550390 | 2026-05-12 | C1 analyzed rw_engine_teardown_d2; RW plugin dispatch; dispatches fn ptrs at param_1+0x20/+0x24 with (param_1+0x50); see re/analysis/rw_engine_teardown_d2/0x00551510.md |
| S-0081 | 0x004c2c90 FUN_004c2c90 | 0x004c2f60 + 0x004c3040 | 2026-05-12 | C1 analyzed rw_engine_teardown_d2; RW engine state-transition dispatcher; switch on param_2 (0xd..0x12); default calls FUN_004d7ff0+FUN_004d8480; see re/analysis/rw_engine_teardown_d2/0x004c2c90.md |
| S-0082 | 0x004d7ca0 FUN_004d7ca0 | 0x004c3270 FUN_004c3270 | 2026-05-12 | C1 analyzed rw_engine_teardown_d2; render context teardown; iterates DAT_007d6c54 array, unlinks nodes, calls FUN_004ccce0+FUN_004cc9f0; see re/analysis/rw_engine_teardown_d2/0x004d7ca0.md |
| S-0083 | 0x004ccf20 FUN_004ccf20 | 0x004c3270 FUN_004c3270 | 2026-05-12 | C1 analyzed rw_engine_teardown_d2; free-list/pool block teardown; walks DAT_007d45cc doubly-linked list, frees all blocks via vtable+0x10c/+0x11c, zeros DAT_007d45fc; see re/analysis/rw_engine_teardown_d2/0x004ccf20.md |
| S-0220 | 0x004d7ff0 FUN_004d7ff0 | 0x004c2c90 FUN_004c2c90 | 2026-05-12 | C1 analyzed audio_rws_loader_d2; 4-byte identity function; returns param_1 unchanged (MOV EAX,[ESP+4]; RET); see re/analysis/audio_rws_loader_d2/004d7ff0.md |
| S-0221 | 0x004d8480 FUN_004d8480 | 0x004c2c90 FUN_004c2c90 | 2026-05-12 | C1 analyzed audio_rws_loader_d2; first-error recorder; stores {uint,uint} at DAT_007d3ff8[DAT_007d6c5c] if slot empty (first=0, second=0x80000000); see re/analysis/audio_rws_loader_d2/004d8480.md |
| S-0222 | 0x004ccce0 FUN_004ccce0 | 0x004d7ca0 FUN_004d7ca0 | 2026-05-12 | C1 analyzed rw_engine_teardown_d3; RwFreeListForAllUsed equivalent; iterates pool block bitmap, calls param_2(element_addr, param_3) per set bit; D-0580 resolved; see re/analysis/rw_engine_teardown_d3/0x004ccce0.md |
| S-0223 | 0x004cc9f0 FUN_004cc9f0 | 0x004d7ca0 FUN_004d7ca0 | 2026-05-12 | C1 analyzed render_d3d9_device; RwFreeListDestroy equivalent; unlinks from global chain at DAT_007d45cc, walks+frees internal free list, frees header; D-0581 resolved; see re/analysis/render_d3d9_device/0x004cc9f0.md |

## Conventions

- ID format: `S-NNNN`, monotonic, never reused.
- Every stub must have a corresponding `// STUB S-NNNN` comment in source.
- `re-classify` skill writes new rows; `hook-author` skill enforces that no new C3 row in `hooks.csv` lands while the function still has unresolved stubs.
| S-0100 | 0x004522d6 | 0x004522d0 FUN_004522d0 | audio | passthrough | 2026-05-02 | Indirect dispatch target through DAT_007d3ff8+0x10c; not statically traceable |
| S-0180 | 0x004a2bb8 | 0x004a2be9 __security_check_cookie | boot | passthrough | 2026-05-02 | report_failure; depth-4 of boot_crt_exit_d3; D-0462 |
| S-0181 | 0x004a31e1 | 0x004a4126 __onexit | boot | passthrough | 2026-05-02 | FUN_004a31e1; called before __onexit_lk; role unknown; depth-4; D-0463 |
| S-0182 | 0x004a407e | 0x004a4126 __onexit | boot | passthrough | 2026-05-02 | __onexit_lk; core logic of __onexit; depth-4; D-0464 |
| S-0183 | 0x004a4158 | 0x004a4126 __onexit | boot | passthrough | 2026-05-02 | FUN_004a4158; called after __onexit_lk; role unknown; depth-4; D-0465 |
| S-0184 | 0x004ad33b | 0x004a5de3 FUN_004a5de3 | boot | passthrough | 2026-05-02 | __controlfp; called with (0x10000,0x30000); depth-4; D-0466 |
| S-0185 | 0x004a5df5 | 0x004a5e35 __ms_p5_mp_test_fdiv | boot | passthrough | 2026-05-02 | __ms_p5_test_fdiv; fallback FPU test; depth-4; D-0467 |
| S-0186 | 0x004a9744 | 0x004a5f07 ___endstdio | boot | passthrough | 2026-05-02 | __flushall; unconditional flush; depth-4; D-0468 |
| S-0187 | 0x004ad351 | 0x004a5f07 ___endstdio | boot | passthrough | 2026-05-02 | __fcloseall; conditional close; depth-4; D-0469 |
| ~~S-0188~~ | ~~0x004a7800~~ | ~~0x004a787f __lock~~ | boot | resolved | 2026-05-12 | RESOLVED boot_crt_env_cont1: FUN_004a7800 C1 at re/analysis/boot_crt_env_cont1/0x004a7800.md |
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
| ~~S-0342~~ | ~~0x005ba780~~ | ~~0x005b9f30 LAB_005b9f30~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~LAB_005ba780; RESOLVED audio_dsound_d2; C1 re/analysis/audio_dsound_d2/0x005ba780.md~~ |
| ~~S-0343~~ | ~~0x005ba7f0~~ | ~~0x005b9f30 LAB_005b9f30~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~LAB_005ba7f0; RESOLVED audio_dsound_d2; C1 re/analysis/audio_dsound_d2/0x005ba7f0.md~~ |
| ~~S-0344~~ | ~~0x005adfe0~~ | ~~0x005a9e10 FUN_005a9e10~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~FUN_005adfe0; RESOLVED audio_rws_loader_cont1; C1 re/analysis/audio_rws_loader_cont1/0x005adfe0.md~~ |
| ~~S-0345~~ | ~~0x005ae010~~ | ~~0x005a9e10 FUN_005a9e10~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~FUN_005ae010; RESOLVED audio_rws_loader_cont1; C2 re/analysis/audio_rws_loader_cont1/0x005ae010.md~~ |
| ~~S-0346~~ | ~~0x005ba720~~ | ~~0x005ba1d0 LAB_005ba1d0~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~LAB_005ba720; RESOLVED audio_dsound_d2; C1 re/analysis/audio_dsound_d2/0x005ba720.md~~ |
| ~~S-0347~~ | ~~0x005bb000~~ | ~~0x005ba1d0 LAB_005ba1d0~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~FUN_005bb000; RESOLVED audio_dsound_d2; C1 re/analysis/audio_dsound_d2/0x005bb000.md~~ |
| ~~S-0348~~ | ~~0x005ba760~~ | ~~0x005ba1d0 LAB_005ba1d0~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~LAB_005ba760; RESOLVED audio_dsound_d2; C1 re/analysis/audio_dsound_d2/0x005ba760.md~~ |
| ~~S-0349~~ | ~~0x005bbc10~~ | ~~0x005ba1d0 LAB_005ba1d0~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~FUN_005bbc10; RESOLVED audio_dsound_d2; C1 re/analysis/audio_dsound_d2/0x005bbc10.md~~ |
| ~~S-0350~~ | ~~0x005bbdb0~~ | ~~0x005ba1d0 LAB_005ba1d0 + 0x005bad30 LAB_005bad30~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~FUN_005bbdb0; RESOLVED audio_dsound_d2; C1 re/analysis/audio_dsound_d2/0x005bbdb0.md~~ |
| ~~S-0351~~ | ~~0x005bac00~~ | ~~0x005ba1d0 LAB_005ba1d0 + 0x005bad30 LAB_005bad30~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~FUN_005bac00; RESOLVED audio_dsound_d2; C1 re/analysis/audio_dsound_d2/0x005bac00.md~~ |
| ~~S-0352~~ | ~~0x005bbf30~~ | ~~0x005bad30 LAB_005bad30~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-02~~ | ~~FUN_005bbf30; RESOLVED audio_dsound_d2; C1 re/analysis/audio_dsound_d2/0x005bbf30.md~~ |
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
| S-0481 | 0x0042c2d0 | 0x004929d0 FUN_004929d0 | util | RESOLVED 2026-05-12 | 2026-05-02 | GetDat0067ecb4 promoted C2→C3 (game_state_sentinel_diff-20260512); reimpl mashed_re/GameState/StateAccessors.cpp |
| S-0482 | 0x0042c2e0 | 0x004929d0 FUN_004929d0 | util | RESOLVED 2026-05-12 | 2026-05-02 | GetDat0067ecb8 promoted C2→C3 (game_state_sentinel_diff-20260512); reimpl mashed_re/GameState/StateAccessors.cpp |
| S-0483 | 0x0042c2f0 | 0x004929d0 FUN_004929d0 | util | passthrough | 2026-05-02 | FUN_0042c2f0; called with arg 0 in cases 1 and 4; D-1360 |
| S-0484 | 0x0042f500 | 0x004929d0 FUN_004929d0 | util | RESOLVED 2026-05-12 | 2026-05-02 | GetDat0067ea64 promoted C2→C3 (game_state_sentinel_diff-20260512); reimpl mashed_re/GameState/StateAccessors.cpp |
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
| ~~S-1284~~ | ~~0x004189f0 thunk_FUN_00419760~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/004189f0.md |
| ~~S-1285~~ | ~~0x004215c0 FUN_004215c0~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/004215c0.md |
| ~~S-1286~~ | ~~0x0045ba00 FUN_0045ba00~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/0045ba00.md |
| ~~S-1287~~ | ~~0x0046c5c0 FUN_0046c5c0~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/0046c5c0.md |
| ~~S-1288~~ | ~~0x0046c790 FUN_0046c790~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/0046c790.md |
| S-1134 | 0x0043d2a0 FUN_0043d2a0 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | called with 2-arg pairs (0,2),(7,0),(1,0); central transition handler |
| S-1135 | 0x0042ae10 FUN_0042ae10 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | called with arg 0 in inner-switch cases 1,2,9,0xb,0xc; return gates state changes |
| S-1136 | 0x0042aeb0 FUN_0042aeb0 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | called in cases 1,2,9; non-zero result sets DAT_0067eac5 and DAT_0067f1a0 |
| S-1137 | 0x0042bf30 FUN_0042bf30 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | 6-arg call; two sites: DAT_0067ece8..ed04 and DAT_0067e918..e92c |
| S-1138 | 0x004298c0 FUN_004298c0 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | called in transition paths -0xdc0000 and -0xe20000 |
| S-1139 | 0x00413f90 FUN_00413f90 | 0x0043d7c0 FUN_0043d7c0 | util | passthrough | 2026-05-03 | returns pointer used with DAT_0067f17c×0x30 stride; -0xd80000 player init |
| ~~S-1380~~ | ~~0x0049dd60 FUN_0049dd60~~ | ~~0x0049ec10 FUN_0049ec10~~ | util | **RESOLVED 2026-05-06** video_mci_d3-20260506-0512 | 2026-05-03 | 4-arg base/ancestor ctor; analyzed in video_mci_d3; see re/analysis/video_mci_d3/0x0049dd60.md |
| ~~S-1560~~ | ~~0x00482860 FUN_00482860~~ | ~~0x00482930 Replay::New~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | Replay::Reset; C1 analyzed; re/analysis/replay_record_cont1/0x00482860.md |
| ~~S-1561~~ | ~~0x00483a30 FUN_00483a30~~ | ~~0x00411750 Replay::StartLap~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | Replay::Rewind; C1 analyzed; re/analysis/replay_record_cont1/0x00483a30.md |
| ~~S-1562~~ | ~~0x00483a40 FUN_00483a40~~ | ~~0x004114e0 Replay::Cleanup~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | Replay::Free; C1 analyzed; re/analysis/replay_record_cont1/0x00483a40.md |
| ~~S-1563~~ | ~~0x0046d4a0 FUN_0046d4a0~~ | ~~0x00411600 Replay::RecordFrame~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 (drift: already C1 in ai_update_d2) | 2026-05-03 | vehicle struct ptr getter; drift-clear D-4603 |
| ~~S-1564~~ | ~~0x00546b10 FUN_00546b10~~ | ~~0x004829d0 Replay::WriteFrame~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | RwMatrix→Quaternion (Shepperd method); C1 analyzed; re/analysis/replay_record_cont1/0x00546b10.md |
| ~~S-1565~~ | ~~0x00483ca0 FUN_00483ca0~~ | ~~0x004117b0 Replay::Save~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | Replay::Save; C1 analyzed; re/analysis/replay_record_cont1/0x00483ca0.md |
| ~~S-1566~~ | ~~0x004cc230 FUN_004cc230~~ | ~~0x00483d10 Replay::Load~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | RwStreamOpen; C1 analyzed; re/analysis/replay_record_cont1/0x004cc230.md |
| ~~S-1567~~ | ~~0x004cbd30 FUN_004cbd30~~ | ~~0x00483d10 Replay::Load~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 (drift: already C2 in texture_loader_d3) | 2026-05-03 | RwStreamRead; drift-clear D-4606 |
| ~~S-1568~~ | ~~0x004cc160 FUN_004cc160~~ | ~~0x00483d10 Replay::Load~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | RwStreamClose; C1 analyzed; re/analysis/replay_record_cont1/0x004cc160.md |
| ~~S-1569~~ | ~~0x0041a9b0 FUN_0041a9b0~~ | ~~0x00411ae0 Ghost::PlaybackTick~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | Ghost::SetupTransforms; C1 analyzed; re/analysis/replay_record_cont1/0x0041a9b0.md |
| ~~S-1570~~ | ~~0x0041ad00 FUN_0041ad00~~ | ~~0x00411ae0 Ghost::PlaybackTick~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | Ghost::ApplyTransforms; C1 analyzed; re/analysis/replay_record_cont1/0x0041ad00.md |
| ~~S-1571~~ | ~~0x0041a960 FUN_0041a960~~ | ~~0x00411ce0 Ghost::SetupRender~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | Ghost::Init; C1 analyzed; re/analysis/replay_record_cont1/0x0041a960.md |
| ~~S-1572~~ | ~~0x00546e70 FUN_00546e70~~ | ~~0x00482c10 Replay::ReadFrame~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | SLERP prep; C1 analyzed; re/analysis/replay_record_cont1/0x00546e70.md |
| ~~S-1573~~ | ~~0x00482ae0 FUN_00482ae0~~ | ~~0x00482c10 Replay::ReadFrame~~ | vehicle | **RESOLVED 2026-05-13** replay_record_cont1 | 2026-05-03 | Catmull-Rom 3D spline; C1 analyzed; re/analysis/replay_record_cont1/0x00482ae0.md |
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
| ~~S-2124~~ | ~~0x004c4600 FUN_004c4600~~ | ~~0x00552e40 FontCtx_FlushMatrix~~ | ~~render~~ | ~~resolved vehicle_dynamics_d3-20260512~~ | ~~2026-05-06~~ | ~~RwMatrixMultiply; C1 new; render_pipeline_d3/004c4600.md~~ |
| ~~S-2125~~ | ~~0x004c4dc0 FUN_004c4dc0~~ | ~~0x00552e40 FontCtx_FlushMatrix~~ | ~~render~~ | ~~resolved vehicle_dynamics_d3-20260512~~ | ~~2026-05-06~~ | ~~RwMatrixInvert; C1 new; render_pipeline_d3/004c4dc0.md~~ |
| S-2126 | 0x004c0ed0 FUN_004c0ed0 | 0x00552e40 FontCtx_FlushMatrix | render | passthrough | 2026-05-06 | camera view matrix getter; takes cam+4 field; returns RwMatrix*; D-6286 |
| S-2127 | 0x00552d70 FUN_00552d70 | 0x00427f00 FUN_00427f00 | hud | passthrough | 2026-05-06 | FontMatrix_Pop (counterpart to FontMatrix_Push); called @ 0x00427fc9 after draw; D-6287 |
| ~~S-1920~~ | ~~0x004c1210 FUN_004c1210~~ | ~~0x00454170 FUN_00454170~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; detach-from-parent + root propagation~~ |
| ~~S-1921~~ | ~~0x004c15c0 FUN_004c15c0~~ | ~~0x00454170 FUN_00454170~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; identity-matrix frame reset~~ |
| ~~S-1922~~ | ~~0x004e43b0 FUN_004e43b0~~ | ~~0x00454170 FUN_00454170~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; detach geometry + release list refs~~ |
| ~~S-1923~~ | ~~0x004e4800 FUN_004e4800~~ | ~~0x004547c0 FUN_004547c0~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; table lookup DAT_007d7174+param_1~~ |
| ~~S-1924~~ | ~~0x00534d00 FUN_00534d00~~ | ~~0x00534b60 FUN_00534b60~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; particle system constructor; depth-5 D-10480~~ |
| ~~S-1925~~ | ~~0x004c0910 FUN_004c0910~~ | ~~0x004c0870 FUN_004c0870~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; deep-clone frame node tree; depth-5 D-10481~~ |
| ~~S-1926~~ | ~~0x004c0d70 FUN_004c0d70~~ | ~~0x004c0de0 FUN_004c0de0~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; recursive frame node destructor~~ |
| ~~S-1927~~ | ~~0x004e8e90 FUN_004e8e90~~ | ~~0x004e68a0 FUN_004e68a0~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; AddRef increment~~ |
| ~~S-1928~~ | ~~0x004e8ea0 FUN_004e8ea0~~ | ~~0x004e68a0 FUN_004e68a0 / 0x004e6920 FUN_004e6920~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; Release/decref+destructor; depth-5 D-10482~~ |
| ~~S-1929~~ | ~~0x004d8bd0 FUN_004d8bd0~~ | ~~0x004e6920 FUN_004e6920~~ | ~~gameplay~~ | ~~resolved powerups_d4-20260508-1823~~ | ~~2026-05-06~~ | ~~analyzed C1; particle descriptor destructor; depth-5 D-10483~~ |
| ~~S-1930~~ | ~~0x004d8000 FUN_004d8000~~ | ~~0x004e6d80 FUN_004e6d80~~ | ~~gameplay~~ | ~~resolved intro_splash_d3 (drift skip)~~ | ~~2026-05-06~~ | ~~already C1 in hooks.csv before powerups_d4 ran; drift-cleared~~ |
| S-3520 | 0x004e8090 FUN_004e8090 | 0x00534d00 FUN_00534d00 | gameplay | passthrough | 2026-05-08 | frame secondary init; called with (**(iVar4+0x18+0x20), 0) after particle spawn succeeds; depth-5; D-10480 |
| S-3521 | 0x00535330 FUN_00535330 | 0x00534d00 FUN_00534d00 | gameplay | passthrough | 2026-05-08 | alternate particle type setup (path B: pcVar2==0 no 0x20000000 flag); called (piVar5 param_2 param_3); depth-5; D-10480 |
| S-3522 | 0x004d8090 FUN_004d8090 | 0x004c0910 FUN_004c0910 | gameplay | passthrough | 2026-05-08 | list notify on clone; called (&DAT_00617f78 new_node original_node); depth-5; D-10481 |
| S-3523 | 0x004f0c10 FUN_004f0c10 | 0x004e8ea0 FUN_004e8ea0 | gameplay | passthrough | 2026-05-08 | releases resource at param_1+0x54; called with (*(param_1+0x54)); depth-5; D-10482 |
| S-3524 | 0x004f3b60 FUN_004f3b60 | 0x004e8ea0 FUN_004e8ea0 | gameplay | passthrough | 2026-05-08 | teardown of sub-object at param_1+0x20; called with (param_1+0x20); depth-5; D-10482 |
| S-3525 | 0x004e2ff0 FUN_004e2ff0 | 0x004d8bd0 FUN_004d8bd0 | gameplay | passthrough | 2026-05-08 | particle descriptor allocator-free; called with (param_1); depth-5; D-10483 |
| S-2340 | 0x004d8000 FUN_004d8000 | 0x004c77c0 FUN_004c77c0 | frontend | passthrough | 2026-05-06 | find-in-list + secondary dispatch; called with (&DAT_00618180 alloc_result) on video texture alloc; see also S-1930; resolved intro_splash_d3 2026-05-06; D-6940 |
| S-2341 | 0x004d8c40 FUN_004d8c40 | 0x004c7730 FUN_004c7730 | frontend | passthrough | 2026-05-06 | doubly-linked list splice + swap + counter clear; called before vtable slot 38 dispatch; single caller; resolved intro_splash_d3 2026-05-06; D-6941 |
| S-2000 | 0x004a0ef0 FUN_004a0ef0 | 0x0049dd60 FUN_0049dd60 | video | passthrough | 2026-05-06 | 93b __thiscall; receives (param_3, param_4, &this+0x7c, param_2) from FUN_0049dd60 before vtable writes; depth-4 from video_mci; D-5920 |
| S-2001 | 0x004a1160 FUN_004a1160 | 0x0049dd60 FUN_0049dd60 | video | passthrough | 2026-05-06 | 27b __thiscall; called 3x on &this+0x54/0x58/0x5c with args 0/1/1; result at 0x5c used as HANDLE in SetEvent; depth-4 from video_mci; D-5921 |
| S-1962 | 0x004c0ed0 FUN_004c0ed0 | 0x004e6100 FUN_004e6100 | render | passthrough | 2026-05-06 | get float* (matrix) from *(atomic+4); 0x1E bytes; U-1969 |
| S-1964 | 0x00547bf0 FUN_00547bf0 | 0x00539900 FUN_00539900 | render | passthrough | 2026-05-06 | AABB vs triangle pre-test (4 args: query_desc v0 v1 v2); 0x5AE bytes; U-1976 |
| S-1965 | 0x00547450 FUN_00547450 | 0x00539ec0 FUN_00539ec0 | render | passthrough | 2026-05-06 | sphere vs triangle intersection (6 args including out-ptrs for normal+dist); 0x39E bytes; U-1978 |
| S-2087 | 0x005555b0 FUN_005555b0 | 0x00427ad0 FUN_00427ad0 | frontend | passthrough | 2026-05-06 | main sprite draw call; 6 args: (DAT_0067d838, 512B stack buf, scaled param_7, &local_214, 1, DAT_0067d83c); depth-4 of FUN_00427ad0 |
| S-2089 | 0x005554d0 FUN_005554d0 | 0x004282a0 FUN_004282a0 / 0x00428320 FUN_00428320 | frontend | passthrough | 2026-05-06 | text width measurement; (DAT_0067d838, 1024B stack buf, param_2*scale); returns float10; depth-4 |
| S-2090 | 0x00427840 FUN_00427840 | 0x00428320 FUN_00428320 | frontend | passthrough | 2026-05-06 | font setup variant B; no args; replaces FUN_00427780+FUN_004277a0 pair; depth-4 of FUN_00428320 |
| S-2200 | 0x005a7520 | 0x005a66d0 FUN_005a66d0 | audio | resolved | 2026-05-06 | FUN_005a7520; 3-way mode dispatcher; re/analysis/audio_music_d3/005a7520.md; D-7600 D-7601 D-7602 |
| S-2201 | 0x005a6d60 | 0x005a6dc0 FUN_005a6dc0 | audio | resolved | 2026-05-06 | FUN_005a6d60; indirect dispatch via 8-byte fn-ptr table; re/analysis/audio_music_d3/005a6d60.md; U-2567 |
| S-2560 | 0x005a7460 FUN_005a7460 | 0x005a7520 FUN_005a7520 | audio | resolved | 2026-05-12 | audio_music_cont1; priority-queue dequeue+sub-obj stop; re/analysis/audio_music_cont1/0x005a7460.md; U-3674 U-3675 U-3676 |
| S-2561 | 0x005a7560 FUN_005a7560 | 0x005a7520 FUN_005a7520 | audio | resolved | 2026-05-12 | audio_music_cont1; optional dequeue+priority recompute+sorted insert; re/analysis/audio_music_cont1/0x005a7560.md; U-3676 |
| S-2562 | 0x005a75b0 FUN_005a75b0 | 0x005a7520 FUN_005a7520 | audio | resolved | 2026-05-12 | audio_music_cont1; priority score compute+sorted list insert/reposition; re/analysis/audio_music_cont1/0x005a75b0.md; U-3677 U-3678 |
| S-2220 | 0x00430b00 TimeDisplay::SetEntry | 0x00429310 TimeTrial::Tick | util | resolved | 2026-05-06 | HUD time display entry setter (47b); array 0x008989e0 stride 0xc; index=(param_2+param_1*2)*0xc; writes (frac sec min); leaderboard_d2-20260506 |
| S-2225 | 0x0042f790 GhostMode::IsActive | 0x0040d270 Course::Finish | util | resolved | 2026-05-06 | ghost mode flag getter (5b); returns DAT_0067ea70; leaderboard_d2-20260506 |
| S-2226 | 0x0040d040 Course::ValidateCarsFinished | 0x0040d270 Course::Finish | util | resolved | 2026-05-06 | car finish validator (200b); loop 4 cars; FUN_0041f320(state)+FUN_0041efc0(lap); modes 10+6 use FUN_00431d70; U-2607; leaderboard_d2-20260506 |
| S-2600 | 0x00429840 FUN_00429840 | 0x0040e560 FUN_0040e560 | util | passthrough | 2026-05-06 | unknown; called from D-6580 with arg 0xb; also from TimeTrial::LapFinishProcessor with arg 1 on race-complete; leaderboard_d2-20260506 |
| S-2601 | 0x00411870 FUN_00411870 | 0x0040e560 FUN_0040e560 | util | passthrough | 2026-05-06 | unknown init; arg 0; mode-2 lap-complete transition; leaderboard_d2-20260506 |
| S-2602 | 0x0041e130 FUN_0041e130 | 0x0040e560 FUN_0040e560 | util | passthrough | 2026-05-06 | unknown init; arg 0; mode-2 lap-complete transition; leaderboard_d2-20260506 |
| S-2603 | 0x00429860 FUN_00429860 | 0x004103a0 TimeTrial::LapFinishProcessor | util | passthrough | 2026-05-06 | lap-tick condition check; non-0=proceed; first gate in LapFinishProcessor; leaderboard_d2-20260506 |
| S-2604 | 0x00430820 FUN_00430820 | 0x004103a0 TimeTrial::LapFinishProcessor | util | passthrough | 2026-05-06 | already-processed guard; 0=not yet done; second gate in LapFinishProcessor; leaderboard_d2-20260506 |
| S-2605 | 0x0041da90 FUN_0041da90 | 0x004103a0 TimeTrial::LapFinishProcessor | util | passthrough | 2026-05-06 | lap elapsed time output; writes float to addr param; triggers race-complete when result>=DAT_005ccdf4; leaderboard_d2-20260506 |
| S-2606 | 0x00411d60 FUN_00411d60 | 0x004103a0 TimeTrial::LapFinishProcessor | util | passthrough | 2026-05-06 | post-race-complete action in mode 2; called after FUN_00429840(1); leaderboard_d2-20260506 |
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
| ~~S-2460~~ | ~~0x00428450 FUN_00428450~~ | ~~0x00428a30/0x00428d30~~ | frontend | **cleared** 2026-05-06 | already C1 in hooks.csv (hud_ingame_d3); title_screen_d2 confirmed; D-7300 resolved |
| ~~S-2461~~ | ~~0x004288a0 FUN_004288a0~~ | ~~0x00428a30~~ | frontend | **cleared** 2026-05-06 | C1 mapped title_screen_d2; menu layout renderer (image+7 sprites); re/analysis/title_screen_d2/0x004288a0.md |
| ~~S-2462~~ | ~~0x00428320 FUN_00428320~~ | ~~0x00428a30/0x00428bf0~~ | frontend | **cleared** 2026-05-06 | already C1 in hooks.csv (hud_frontend_d3 text-width variant B); D-7302 resolved |
| ~~S-2463~~ | ~~0x0042e590 FUN_0042e590~~ | ~~0x00428bf0~~ | frontend | **cleared** 2026-05-06 | C1 mapped title_screen_d2; wrapper→FUN_0040bb70→FUN_004c5c00 string search; [UNCERTAIN U-2547] key arg untracked; re/analysis/title_screen_d2/0x0042e590.md |
| ~~S-2464~~ | ~~0x0040d250 FUN_0040d250~~ | ~~0x00428d30~~ | frontend | **cleared** 2026-05-06 | C1 mapped title_screen_d2; indexed ptr dereference getter; re/analysis/title_screen_d2/0x0040d250.md |
| ~~S-2465~~ | ~~0x00401ee0 FUN_00401ee0~~ | ~~0x00428d30~~ | frontend | **cleared** 2026-05-06 | C1 mapped title_screen_d2; object-select+RW-matrix+RpClumpRender; re/analysis/title_screen_d2/0x00401ee0.md |
| ~~S-2466~~ | ~~0x0042f0b0 FUN_0042f0b0~~ | ~~0x00428d30~~ | frontend | **cleared** 2026-05-06 | C1 mapped title_screen_d2; int getter DAT_0067f17c+73; re/analysis/title_screen_d2/0x0042f0b0.md |
| S-2540 | 0x004c5c00 FUN_004c5c00 | 0x0040bb70 FUN_0040bb70 / 0x0042e590 FUN_0042e590 | frontend | passthrough | 2026-05-06 | case-insensitive linked-list string search; iterates list@param_1+8; returns node-8 on match; 114b; depth-3 of title_screen_d2; D-7303 chain |
| S-2541 | 0x00401570 FUN_00401570 | 0x00401ee0 FUN_00401ee0 | frontend | passthrough | 2026-05-06 | table scan: iterates DAT_00636578 stride 0x68 (13 entries) matching entry[0x38]==param_1; result→DAT_00636ac0; 36b; depth-3 of title_screen_d2; D-7305 |
| S-2542 | 0x00401da0 FUN_00401da0 | 0x00401ee0 FUN_00401ee0 | frontend | passthrough | 2026-05-06 | RW matrix setup+dirty for DAT_00636ac0 object; identity+translate+rotAxisAngle(-30+accum)+scale+translate; calls FUN_004c1480; 308b; depth-3 of title_screen_d2; D-7305 |
| S-2543 | 0x004c1480 FUN_004c1480 | 0x00401da0 FUN_00401da0 | render | passthrough | 2026-05-06 | calls FUN_004c52f0(param_1+0x10); links param_1+0xa0 into dirty list@DAT_007d3ff8+0xbc; sets flag bits 3 and 0xc; 145b; depth-4; D-7540 |
| S-2544 | 0x00426cf0 FUN_00426cf0 | 0x00401da0 FUN_00401da0 | frontend | passthrough | 2026-05-06 | trivial addr getter: returns &DAT_0066d6e4; 5b; depth-3 of title_screen_d2 |
| S-2500 | 0x00441c80 FUN_00441c80 | 0x00445aa0 FUN_00445aa0 / 0x00441d40 FUN_00441d40 | util | passthrough | 2026-05-06 | interpolated XYZ getter; reads two vehicle slot positions via FUN_0046d4a0; lerp by _DAT_005cc32c; also calls FUN_0040e180 for default slots; body 0xb2 bytes; D-7420 |
| S-2501 | 0x004430a0 FUN_004430a0 | 0x00445aa0 FUN_00445aa0 | util | passthrough | 2026-05-06 | 9B setter: DAT_00897fe0 = param_1; called with 0 in 40-entry player-slot loop (stride 0x4c; DAT_007f1042..0x7f12a2); D-7421 |
| S-2620 | 0x0046f6c0 FUN_0046f6c0 | 0x004709a0 FUN_004709a0 | vehicle | passthrough | 2026-05-06 | called as FUN_0046f6c0(1) after each rigid-body substep; 3580 bytes; purpose unknown; D-7780 |
| S-2621 | 0x00469aa0 FUN_00469aa0 | 0x004709a0 FUN_004709a0 | vehicle | passthrough | 2026-05-06 | returns int; non-zero breaks substep loop (collision settled?); 847 bytes; D-7781 |
| S-2622 | 0x00469df0 FUN_00469df0 | 0x004709a0 FUN_004709a0 | vehicle | passthrough | 2026-05-06 | collision pair detection/resolution; args (veh_b_idx, substep_count); 5062 bytes; D-7782 |
| S-2623 | 0x004c39b0 FUN_004c39b0 | 0x0046ef70 FUN_0046ef70 | vehicle | passthrough | 2026-05-06 | vector normalize; args (&dst_float3, &src_float3); 270 bytes; D-7783 |
| S-2624 | 0x00413c70 FUN_00413c70 | 0x00467300 FUN_00467300 | vehicle | passthrough | 2026-05-06 | 4-channel effect trigger; args (channel=0..3, code=3, 0); 58 bytes; D-7784 |
| S-2480 | 0x004cc230 FUN_004cc230 | 0x004b3c60 FUN_004b3c60 | render | passthrough | 2026-05-06 | stream open/setup; args (2, 1, param_1); depth-5 of track_loader_d4; D-5740 D-5742 D-5743 |
| S-2481 | 0x004cc5e0 FUN_004cc5e0 | 0x004b3c60 FUN_004b3c60 | render | passthrough | 2026-05-06 | chunk reader; args (stream, chunk_id, out_data, out_size); depth-5 of track_loader_d4; D-5740 D-5741 D-5742 D-5743 |
| S-2482 | 0x004e99b0 FUN_004e99b0 | 0x004b3c60 FUN_004b3c60 | render | passthrough | 2026-05-06 | data processor called on successful chunk read; arg (stream); depth-5 of track_loader_d4; D-5740 |
| S-2483 | 0x004cc160 FUN_004cc160 | 0x004b3c60 FUN_004b3c60 | render | passthrough | 2026-05-06 | stream close/cleanup; args (stream, 0); depth-5 of track_loader_d4; D-5740 D-5742 D-5743 |
| S-2484 | 0x004cbd30 FUN_004cbd30 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | called (stream, &param_2, local_c) after struct chunk read; depth-5 of track_loader_d4; D-5741 |
| S-2485 | 0x004d7ff0 FUN_004d7ff0 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | error code builder; args (0x80000013, size) or (1); depth-5 of track_loader_d4; D-5741 D-5755 |
| S-2486 | 0x004d8480 FUN_004d8480 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | error frame dispatcher; arg (&error_frame); depth-5 of track_loader_d4; D-5741 D-5755 |
| S-2487 | 0x0055deb0 FUN_0055deb0 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | list size/count getter; depth-5 of track_loader_d4; D-5741 |
| S-2488 | 0x0055dec0 FUN_0055dec0 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | list begin iterator; depth-5 of track_loader_d4; D-5741 D-5747 D-5748 |
| S-2489 | 0x005c4ad0 FUN_005c4ad0 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | secondary alloc (4 bytes) with flags; depth-5 of track_loader_d4; D-5741 |
| S-2490 | 0x005c4bb0 FUN_005c4bb0 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | list element accessor by index; depth-5 of track_loader_d4; D-5741 |
| S-2491 | 0x005c4d20 FUN_005c4d20 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | list shrink by count; depth-5 of track_loader_d4; D-5741 |
| S-2492 | 0x005c4d50 FUN_005c4d50 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | list free/clear; depth-5 of track_loader_d4; D-5741 |
| S-2493 | 0x005c4df0 FUN_005c4df0 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | list element by index (end-relative); depth-5 of track_loader_d4; D-5741 |
| S-2494 | 0x005c4e10 FUN_005c4e10 | 0x00558df0 FUN_00558df0 | render | passthrough | 2026-05-06 | list end iterator; depth-5 of track_loader_d4; D-5741 |
| S-2495 | 0x00545260 FUN_00545260 | 0x004b3cc0 FUN_004b3cc0 | render | passthrough | 2026-05-06 | spline data processor; arg (stream); depth-5 of track_loader_d4; D-5742 |
| S-2496 | 0x0052daf0 FUN_0052daf0 | 0x004b3de0 FUN_004b3de0 | render | passthrough | 2026-05-06 | animation data processor; arg (stream); depth-5 of track_loader_d4; D-5743 |
| S-2497 | 0x00543dc0 FUN_00543dc0 | 0x00479030 LAB_00479030 | render | passthrough | 2026-05-06 | post-load query/init; args (0x0, ESI); depth-5 of track_loader_d4; D-5744 |
| S-2498 | 0x004b4550 FUN_004b4550 | 0x00479030 LAB_00479030 | render | passthrough | 2026-05-06 | sphere center computation; also callee of D-5753 FUN_004b53b0; depth-5 of track_loader_d4; D-5744 D-5753 |
| S-2499 | 0x00543da0 FUN_00543da0 | 0x00479030 LAB_00479030 | render | passthrough | 2026-05-06 | attribute setter; args (ESI, 0x5ceb8c, 2, 4, EDI); depth-5 of track_loader_d4; D-5744 |
| S-2640 | 0x0047b860 FUN_0047b860 | 0x0047b9b0 FUN_0047b9b0 | physics | passthrough | 2026-05-07 | pre-setup call before (*param_3)(); 21 bytes; depth-2 callee of 0047b9b0; D-7840 physics_collision_d2 |
| S-2641 | 0x0047b8d0 FUN_0047b8d0 | 0x0047b9b0 FUN_0047b9b0 | physics | passthrough | 2026-05-07 | main executor body (script, buf); 167 bytes; depth-2 callee of 0047b9b0; D-7841 physics_collision_d2 |
| S-2642 | 0x0047b880 FUN_0047b880 | 0x0047b9b0 FUN_0047b9b0 | physics | passthrough | 2026-05-07 | post-call teardown; 24 bytes; depth-2 callee of 0047b9b0; D-7842 physics_collision_d2 |
| S-2643 | 0x0047ce40 FUN_0047ce40 | 0x004715a0 FUN_004715a0 | physics | passthrough | 2026-05-07 | vol-index → handle mapper; 36 bytes; depth-2 callee of 004715a0; D-7843 physics_collision_d2 |
| ~~S-2625~~ | ~~0x0046c5f0 FUN_0046c5f0~~ | ~~0x0046ddb0 VehicleWheelForceIntegrator / 0x0046f6c0 VehicleWheelContactSolver~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: drift-skip; C1 in physics_collision_d4_breadth as triangle-face-normal-helper |
| ~~S-2626~~ | ~~0x0047d3c0 FUN_0047d3c0~~ | ~~0x0047eb30 VehiclePhysicsWorldStep~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: C1 VehiclePhysicsWorldCreate at re/analysis/vehicle_update_d3_cont/0047d3c0.md |
| ~~S-2627~~ | ~~0x0047ea40 FUN_0047ea40~~ | ~~0x0047eb30 VehiclePhysicsWorldStep~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: C1 PhysicsSceneStepWrapper at re/analysis/vehicle_update_d3_cont/0047ea40.md |
| ~~S-2628~~ | ~~0x0046cb30 FUN_0046cb30~~ | ~~0x0047eb30 VehiclePhysicsWorldStep~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: drift-skip; C1 in profile_career_d4 as Player::GetOffset3D |
| ~~S-2629~~ | ~~0x004c52f0 FUN_004c52f0~~ | ~~0x0046f6c0 VehicleWheelContactSolver~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: C1 RwMatrixCombine at re/analysis/vehicle_update_d3_cont/004c52f0.md |
| ~~S-2630~~ | ~~0x0046d4d0 FUN_0046d4d0~~ | ~~0x0047eb30 VehiclePhysicsWorldStep~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: C1 VehiclePhysicsMatrixSet at re/analysis/vehicle_update_d3_cont/0046d4d0.md |
| ~~S-2631~~ | ~~0x00442ce0 FUN_00442ce0~~ | ~~0x0046ddb0 VehicleWheelForceIntegrator~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: C1 VehicleRubberBandSpeedModifier at re/analysis/vehicle_update_d3_cont/00442ce0.md |
| ~~S-2632~~ | ~~0x00442c80 FUN_00442c80~~ | ~~0x0046ddb0 VehicleWheelForceIntegrator~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: drift-skip; C2 in util_c0_promote |
| ~~S-2633~~ | ~~0x004a3384 FUN_004a3384~~ | ~~0x00468980 VehicleAeroStabilizer / 0x0046f6c0 VehicleWheelContactSolver~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: drift-skip; C1 in profile_career_d4 as CRT::acos |
| ~~S-2634~~ | ~~0x0046cc40 FUN_0046cc40~~ | ~~0x0046f6c0 VehicleWheelContactSolver~~ | vehicle | resolved | 2026-05-13 | RESOLVED vehicle_update_d3_cont-20260513: drift-skip; C1 in physics_collision_d4_breadth as WheelTerrainContactClassifier |
| S-2660 | 0x004c5800 FUN_004c5800 | 0x00550130 FUN_00550130 | render | passthrough | 2026-05-06 | RwTexDictionarySetCurrent candidate: setter for current TXD context; called with NULL to clear then with saved ptr to restore; D-7900 |
| S-2661 | 0x004c5820 FUN_004c5820 | 0x00550130 FUN_00550130 | render | passthrough | 2026-05-06 | RwTexDictionaryGetCurrent candidate: getter returning saved TXD context ptr; result stored for later restore; D-7901 |
| S-2662 | 0x004c5830 FUN_004c5830 | 0x00550130 FUN_00550130 | render | passthrough | 2026-05-06 | RwTextureSetCurrent candidate (or similar): setter for current texture context; D-7902 |
| S-2663 | 0x004c5850 FUN_004c5850 | 0x00550130 FUN_00550130 | render | passthrough | 2026-05-06 | RwTextureGetCurrent candidate: getter returning saved texture ptr; D-7903 |
| S-2664 | 0x004c5a00 FUN_004c5a00 | 0x00550130 FUN_00550130 | render | passthrough | 2026-05-06 | RwTexture create-from-stream (main allocate+init step); 90 bytes; D-7904 |
| S-2665 | 0x004c5ae0 FUN_004c5ae0 | 0x00550130 FUN_00550130 | render | passthrough | 2026-05-06 | RwTexture post-process step 1; 109 bytes; purpose unknown; D-7905 |
| S-2666 | 0x004c5b50 FUN_004c5b50 | 0x00550130 FUN_00550130 | render | passthrough | 2026-05-06 | RwTexture post-process step 2; 109 bytes; purpose unknown; D-7906 |
| S-2667 | 0x004d8810 FUN_004d8810 | 0x00550130 FUN_00550130 | render | passthrough | 2026-05-06 | state check called twice in texture stream reader; non-zero result required to proceed; 608 bytes; D-7907 |
| S-2668 | 0x004e1df0 FUN_004e1df0 | 0x00550130 FUN_00550130 | render | passthrough | 2026-05-06 | error cleanup path in texture stream reader; called on FUN_004c5a00 failure; 112 bytes; D-7908 |
| S-2669 | 0x004cdd60 FUN_004cdd60 | 0x004cee90 FUN_004cee90 | render | passthrough | 2026-05-06 | RwImageAllocatePixels candidate: alloc pixel+palette buffer; stride computation; sets img+0x10/+0x14/+0x18; D-7909 |
| S-2670 | 0x004cc4f0 FUN_004cc4f0 | 0x004cc400 FUN_004cc400 | render | passthrough | 2026-05-06 | RW chunk type validator: returns 1 for known types {5..0xb,0xe..0x10,0x12,0x14,0x1a}; now in hooks.csv C1 — clear stub |
| S-2860 | 0x00419760 FUN_00419760 | 0x004189f0 thunk_FUN_00419760 | frontend | passthrough | 2026-05-07 | thunkee of 004189f0; 112 bytes; not yet in hooks.csv; D-8500 |
| S-2861 | 0x00420de0 FUN_00420de0 | 0x004215c0 FUN_004215c0 | frontend | passthrough | 2026-05-07 | sole callee of 004215c0; receives param_2 (50.0f or 0/1 from parent caller); 30 bytes; D-8501 |
| ~~S-2929~~ | ~~0x00405460 FUN_00405460~~ | ~~0x004102f0 FUN_004102f0~~ | util | ~~passthrough~~ | 2026-05-07 | RESOLVED 2026-05-08 game_state_d5-cont1: analyzed C1; re/analysis/game_state_d5-cont1/0x00405460.md |
| ~~S-2930~~ | ~~0x0040e590 FUN_0040e590~~ | ~~0x004102f0 FUN_004102f0~~ | util | ~~passthrough~~ | 2026-05-07 | RESOLVED 2026-05-08 game_state_d5-cont1: analyzed C1; re/analysis/game_state_d5-cont1/0x0040e590.md |
| ~~S-2960~~ | ~~0x00404fa0 FUN_00404fa0~~ | ~~0x00405460 FUN_00405460~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x00404fa0.md |
| ~~S-2961~~ | ~~0x00408b00 FUN_00408b00~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x00408b00.md |
| ~~S-2962~~ | ~~0x00409290 FUN_00409290~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x00409290.md |
| ~~S-2963~~ | ~~0x0040b250 FUN_0040b250~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x0040b250.md |
| ~~S-2964~~ | ~~0x0040b410 FUN_0040b410~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x0040b410.md |
| ~~S-2965~~ | ~~0x0041ede0 FUN_0041ede0~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x0041ede0.md |
| ~~S-2966~~ | ~~0x0041ee50 FUN_0041ee50~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x0041ee50.md |
| ~~S-2967~~ | ~~0x0041ef80 FUN_0041ef80~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x0041ef80.md |
| ~~S-2968~~ | ~~0x0041f000 FUN_0041f000~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: timer_d3_cont1_b-20260512; C1 re/analysis/timer_d3_cont1_b/0x0041f000.md (drift) |
| ~~S-2969~~ | ~~0x00429820 FUN_00429820~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x00429820.md |
| ~~S-2970~~ | ~~0x0046b1c0 FUN_0046b1c0~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: timer_d3_cont1_b-20260512; C1 re/analysis/timer_d3_cont1_b/0x0046b1c0.md (drift) |
| ~~S-2971~~ | ~~0x0046b540 FUN_0046b540~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: timer_d3_cont1_b-20260512; C1 re/analysis/timer_d3_cont1_b/0x0046b540.md (drift) |
| ~~S-2972~~ | ~~0x0046c6d0 FUN_0046c6d0~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x0046c6d0.md |
| ~~S-2973~~ | ~~0x004704c0 FUN_004704c0~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x004704c0.md |
| ~~S-2974~~ | ~~0x0048f680 FUN_0048f680~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x0048f680.md |
| ~~S-2975~~ | ~~0x0048f740 FUN_0048f740~~ | ~~0x0040e590 FUN_0040e590~~ | util | ~~passthrough~~ | 2026-05-08 | RESOLVED: game_state_d5_cont2-20260512; C1 re/analysis/game_state_d5_cont2/0x0048f740.md |
| S-3180 | 0x005adef0 FUN_005adef0 | 0x005aabe0 FUN_005aabe0 | audio | passthrough | 2026-05-08 | called as FUN_005adef0(&DAT_007dccf0,&LAB_005aac00,param_1); return value used as ref-count by callers; audio_dsound_d3 |
| S-3181 | 0x005ae250 FUN_005ae250 | 0x005bc860 FUN_005bc860 | audio | passthrough | 2026-05-08 | called as FUN_005ae250(0x12,8,1,0,&DAT_007ddfec); tracker/pool init; audio_dsound_d3 |
| S-3182 | 0x005ae330 FUN_005ae330 | 0x005bc880 FUN_005bc880 | audio | passthrough | 2026-05-08 | called as FUN_005ae330(&DAT_007ddfec); counterpart to FUN_005ae250; audio_dsound_d3 |
| ~~S-3183~~ | ~~0x005aacf0 FUN_005aacf0~~ | ~~0x005aaa00 FUN_005aaa00~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-08~~ | ~~called twice: (param_1,1) and (param_1,2) when +0x1c bit0x1 set; audio_dsound_d3~~ | RESOLVED: audio_dsound_cont1-20260512; C1 re/analysis/audio_dsound_cont1/0x005aacf0.md |
| ~~S-3184~~ | ~~0x005a9f60 FUN_005a9f60~~ | ~~0x005aaa00 FUN_005aaa00~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-08~~ | ~~final teardown call (param_1,param_2,param_3); audio_dsound_d3~~ | RESOLVED: audio_dsound_cont1-20260512; C1 re/analysis/audio_dsound_cont1/0x005a9f60.md |
| ~~S-3185~~ | ~~0x005bc190 FUN_005bc190~~ | ~~0x005baa60 FUN_005baa60~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-08~~ | ~~called as FUN_005bc190(param_3,local_48); transforms param_3 → 4-float buffer; audio_dsound_d3~~ | RESOLVED: audio_dsound_cont1-20260512; C1 re/analysis/audio_dsound_cont1/0x005bc190.md |
| ~~S-3186~~ | ~~0x005bc450 FUN_005bc450~~ | ~~0x005bc470 FUN_005bc470 + 0x005bc640 FUN_005bc640~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-08~~ | ~~prep/cleanup call; no-arg in FUN_005bc470; with param_1 in FUN_005bc640; audio_dsound_d3~~ | RESOLVED: audio_dsound_cont1-20260512; C1 re/analysis/audio_dsound_cont1/0x005bc450.md |
| ~~S-3187~~ | ~~0x005be0f0 FUN_005be0f0~~ | ~~0x005bc470 FUN_005bc470~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-08~~ | ~~called as FUN_005be0f0(0,param_3+2) and FUN_005be0f0(0,param_3+8,0); slot init; audio_dsound_d3~~ | RESOLVED: audio_dsound_cont1-20260512; C1 re/analysis/audio_dsound_cont1/0x005be0f0.md |
| ~~S-3188~~ | ~~0x005be160 FUN_005be160~~ | ~~0x005bc640 FUN_005bc640~~ | ~~audio~~ | ~~passthrough~~ | ~~2026-05-08~~ | ~~called as FUN_005be160(param_1+2) and FUN_005be160(param_1+8); slot cleanup; audio_dsound_d3~~ | RESOLVED: audio_dsound_cont1-20260512; C1 re/analysis/audio_dsound_cont1/0x005be160.md |
| S-3189 | 0x005aef70 LAB_005aef70 | 0x005aef30 FUN_005aef30 | audio | passthrough | 2026-05-08 | _beginthread target; thread wrapper proc; receives thread-descriptor ptr as arg; dispatches to [1] proc; audio_dsound_d3 |
| S-3190 | 0x005aa560 FUN_005aa560 | 0x005c7990 FUN_005c7990 | audio | passthrough | 2026-05-08 | called as FUN_005aa560(&DAT_007de1c0,0,p2,p1,p3,p4); 567 bytes (005aa560–005aa797); depth-2 from 005ba1d0 tail; D-0952; audio_dsound_d4 | RESOLVED: analyzed in audio_dsound_d5-20260511; C1 plate at re/analysis/audio_dsound_d5/0x005aa560.md |
| S-3565 | 0x005aad40 FUN_005aad40 | 0x005aa560 FUN_005aa560 | audio | passthrough | 2026-05-11 | called at 0x005aa593 with no args when param_2 is null; returns default context ptr; audio_dsound_d5 |
| S-3566 | 0x005aa030 FUN_005aa030 | 0x005aa560 FUN_005aa560 | audio | passthrough | 2026-05-11 | called at 0x005aa5aa as FUN_005aa030(param_1, context[0]); validation check; returns 0=fail; audio_dsound_d5 |
| S-3567 | fn-ptr@[param_1+0x3c] | 0x005aa560 FUN_005aa560 | audio | passthrough | 2026-05-11 | indirect call at 0x005aa614 with (0, puVar4) when param_4 is null; target RVA runtime-variable from audio struct field; U-3427; audio_dsound_d5 |
| S-3568 | 0x005aa7e0 FUN_005aa7e0 | 0x005aa560 FUN_005aa560 | audio | passthrough | 2026-05-11 | called at 0x005aa62b as FUN_005aa7e0(param_1, param_3, &local_204); returns format/state value; out-param at &local_204; audio_dsound_d5 |
| S-3569 | 0x005aa7a0 FUN_005aa7a0 | 0x005aa560 FUN_005aa560 | audio | passthrough | 2026-05-11 | called at 0x005aa72a as FUN_005aa7a0(param_1, &param_3, puVar4, uVar5); default audio node allocator; returns node ptr or null; audio_dsound_d5 |
| S-3570 | 0x005aa900 FUN_005aa900 | 0x005aa560 FUN_005aa560 | audio | passthrough | 2026-05-11 | called at 0x005aa687 as FUN_005aa900(node, param_1, param_3, local_204); initializes node fields after alloc; audio_dsound_d5 |
| S-3120 | 0x00412130 FUN_00412130 | 0x00401f10 FUN_00401f10 | render | passthrough | 2026-05-08 | called as count getter; return used as loop bound for second sub-array at DAT_00636ac0+0x50; render_frame_d4 |
| S-3121 | 0x004e6680 FUN_004e6680 | 0x00401f10 FUN_00401f10; 0x004725c0 FUN_004725c0 | render | passthrough | 2026-05-08 | per-element op on pointer arrays at DAT_00636ac0+0x40/+0x50 and DAT_0069150c; render_frame_d4 |
| S-3122 | 0x00427e00 FUN_00427e00 | 0x00403fa0 FUN_00403fa0; 0x004041c0 FUN_004041c0 | render | passthrough | 2026-05-08 | 6-arg sprite/text draw (id, x, y, color, scale, mode); render_frame_d4 |
| S-3123 | 0x00472b10 FUN_00472b10 | 0x0042c010 FUN_0042c010 | render | passthrough | 2026-05-08 | 6-arg rect/quad draw (0, x1_scaled, y1_scaled, x2_scaled, y2_scaled, color); render_frame_d4 |
| S-3124 | 0x004273e0 FUN_004273e0 | 0x0042c090 FUN_0042c090 | render | passthrough | 2026-05-08 | line draw 5-arg (x1, y1, x2, y2, color_ptr); renders outline segments; render_frame_d4 |
| S-3125 | 0x00427780 FUN_00427780 | 0x004278d0 FUN_004278d0; 0x00427990 FUN_00427990 | render | passthrough | 2026-05-08 | 1-arg; receives track/level id (param_1); font/context setup step 1; render_frame_d4 |
| S-3126 | 0x004277a0 FUN_004277a0 | 0x004278d0 FUN_004278d0; 0x00427990 FUN_00427990 | render | passthrough | 2026-05-08 | no args; font/context setup step 2; paired with S-3125; render_frame_d4 |
| S-3127 | 0x00556e90 FUN_00556e90 | 0x004278d0 FUN_004278d0; 0x00427990 FUN_00427990; 0x00427be0 FUN_00427be0 | render | passthrough | 2026-05-08 | 5 args (handle, ptr×4 repeated); font color/parameter setter; render_frame_d4 |
| S-3128 | 0x005555b0 FUN_005555b0 | 0x004278d0 FUN_004278d0; 0x00427990 FUN_00427990; 0x00427be0 FUN_00427be0 | render | passthrough | 2026-05-08 | 6 args (handle, buf, float_scale, data_ptr, align, handle2); string format+render; render_frame_d4 |
| S-3129 | 0x00427840 FUN_00427840 | 0x00427be0 FUN_00427be0 | render | passthrough | 2026-05-08 | no args; combined font/context setup (replaces S-3125+S-3126 pair); render_frame_d4 |
| S-3130 | 0x004770a0 FUN_004770a0 | 0x0041ebb0 FUN_0041ebb0 | render | passthrough | 2026-05-08 | 1-arg; object handle; paired complement to FUN_00476df0 (C1 effects_particle); called after render-state-8 disable; render_frame_d4 |
| S-3131 | 0x00421720 FUN_00421720 | 0x004219c0 FUN_004219c0 | render | passthrough | 2026-05-08 | no args; called per 0x208-byte entry in array 0x0063fb90..0x006403b0; 4 iterations; render_frame_d4 |
| S-3132 | 0x004c1b40 FUN_004c1b40 | 0x00425e40 FUN_00425e40 | render | passthrough | 2026-05-08 | 2 args (RW_device_ptr, uVar1 from FUN_004e6100); result gates 3-vtable dispatch; render_frame_d4 |
| S-0380 | 0x0046c5c0 FUN_0046c5c0 | 0x00422fd0 FUN_00422fd0 | vehicle | passthrough | 2026-05-08 | vehicle state clear; called first in vehicle reset sequence |
| S-0381 | 0x0046c790 FUN_0046c790 | 0x00422fd0 FUN_00422fd0 | vehicle | passthrough | 2026-05-08 | vehicle activate with flag 1 |
| S-0382 | 0x004215c0 FUN_004215c0 | 0x00422fd0 FUN_00422fd0 | vehicle | passthrough | 2026-05-08 | vehicle speed/parameter setter; called twice with 0x42480000 (50.0f) for axes 0 and 1 |
| S-0383 | 0x0045ba00 FUN_0045ba00 | 0x00422fd0 FUN_00422fd0 | vehicle | passthrough | 2026-05-08 | powerup state reset with mode 2 |
| S-0384 | 0x00419760 thunk_FUN_00419760 | 0x00422fd0 FUN_00422fd0 | vehicle | passthrough | 2026-05-08 | player control enable thunk; called when FUN_0040e470(param_1)==1 and mode!=7 |
| S-3200 | 0x004095a0 FUN_004095a0 | 0x00409710 FUN_00409710 | render | passthrough | 2026-05-08 | called as FUN_004095a0(1) before opening led.piz; 1-arg setup; track_loader |
| S-3201 | 0x004cc230 FUN_004cc230 | 0x00409710 FUN_00409710; 0x004235b0 FUN_004235b0 | render | passthrough | 2026-05-08 | called as FUN_004cc230(2,1,&DAT_0063b7f0/00644110); returns handle; track_loader |
| S-3202 | 0x004cc5e0 FUN_004cc5e0 | 0x00409710 FUN_00409710; 0x004235b0 FUN_004235b0 | render | passthrough | 2026-05-08 | called as FUN_004cc5e0(handle,id,&out1,out2); returns int; id=0x13269901/0x13269902; track_loader |
| S-3203 | 0x004cbd30 FUN_004cbd30 | 0x00409710 FUN_00409710; 0x004235b0 FUN_004235b0 | render | passthrough | 2026-05-08 | called as FUN_004cbd30(handle,&buf,size); reads data into float array; track_loader |
| S-3204 | 0x004cc160 FUN_004cc160 | 0x00409710 FUN_00409710; 0x004235b0 FUN_004235b0 | render | passthrough | 2026-05-08 | called as FUN_004cc160(handle,0); close/release handle; track_loader |
| S-3205 | 0x004b3f90 FUN_004b3f90 | 0x00412050 FUN_00412050 | render | passthrough | 2026-05-08 | 7-arg call (handle,ptr,0,0.1f,~1.0f,3.0f,index); returns object; track_loader |
| S-3206 | 0x0047fad0 FUN_0047fad0 | 0x00412050 FUN_00412050 | render | passthrough | 2026-05-08 | 1-arg; wraps FUN_004b3f90 result; track_loader |
| S-3207 | 0x0047cdc0 FUN_0047cdc0 | 0x00412050 FUN_00412050 | render | passthrough | 2026-05-08 | 2-arg (result_ptr, 6.0f); called twice per iteration; track_loader |
| S-3208 | 0x00423480 FUN_00423480 | 0x004235b0 FUN_004235b0 | render | passthrough | 2026-05-08 | no-arg setup call before opening ai.piz; track_loader |
| S-3209 | 0x00425bf0 FUN_00425bf0 | 0x004262f0 FUN_004262f0 | render | passthrough | 2026-05-08 | no-arg call before loop in FUN_004262f0; track_loader |
| S-3210 | 0x00425c00 FUN_00425c00 | 0x004262f0 FUN_004262f0 | render | passthrough | 2026-05-08 | 1-arg (ptr to 64-byte entry); per-element op; track_loader |
| S-3211 | 0x00425b70 FUN_00425b70 | 0x004262f0 FUN_004262f0 | render | passthrough | 2026-05-08 | 2-arg (index, lookup_result); per-element op; track_loader |
| S-3212 | 0x00550430 FUN_00550430 | 0x004b6640 FUN_004b6640 | boot | passthrough | 2026-05-08 | no-arg call after FUN_00551330 in FUN_004b6640; boot_app_init_d4 |
| S-3213 | 0x005504d0 FUN_005504d0 | 0x004b6640 FUN_004b6640 | boot | passthrough | 2026-05-08 | called with PTR_DAT_006172f0; first call in FUN_004b6640; boot_app_init_d4 |
| S-3214 | 0x00551330 FUN_00551330 | 0x004b6640 FUN_004b6640 | boot | passthrough | 2026-05-08 | called with (puVar1, 2, PTR_DAT_006172f0, PTR_DAT_006172f4); boot_app_init_d4 |
| S-3215 | 0x00427580 FUN_00427580 | 0x004275d0 FUN_004275d0 | boot | passthrough | 2026-05-08 | no-arg tail call in FUN_004275d0; boot_app_init_d4 |
| S-3216 | 0x00552920 FUN_00552920 | 0x004275d0 FUN_004275d0 | boot | passthrough | 2026-05-08 | called with FUN_00553ef0() result; boot_app_init_d4 |
| S-3217 | 0x00553cf0 FUN_00553cf0 | 0x004275d0 FUN_004275d0 | boot | passthrough | 2026-05-08 | called with (FUN_00553ef0 result, &DAT_0067d828); boot_app_init_d4 |
| S-3218 | 0x00553e80 FUN_00553e80 | 0x004275d0 FUN_004275d0 | boot | passthrough | 2026-05-08 | called with FUN_00553ef0() result; boot_app_init_d4 |
| S-3219 | 0x00553ef0 FUN_00553ef0 | 0x004275d0 FUN_004275d0 | boot | passthrough | 2026-05-08 | no-arg handle getter; return used as arg to S-3216/S-3217/S-3218; boot_app_init_d4 |
| S-3260 | 0x004b9890 FUN_004b9890 | 0x004ba3a0 FUN_004ba3a0 | input | passthrough | 2026-05-08 | string table resize to sizestrt/2 when >75% buckets empty; called from string-shrink-check (FUN_004ba3a0); Lua 5.0; input_lua_d4 [SCRIBE GAP: missing from sweep-20260508-1737] |
| S-3261 | 0x004b9630 FUN_004b9630 | 0x004ba4f0 FUN_004ba4f0 | input | passthrough | 2026-05-08 | get type/tag index from TValue; called from callgcTM (FUN_004ba4f0) to index GC tag-method table; Lua 5.0; input_lua_d4 [SCRIBE GAP: missing from sweep-20260508-1737] |
| S-3262 | 0x004beef0 FUN_004beef0 | 0x004bef20 FUN_004bef20 | input | passthrough | 2026-05-08 | compute instruction array byte size (sizecode×sizeof(Instruction)); called from luaF_freeproto (FUN_004bef20) to subtract from totalbytes; Lua 5.0; input_lua_d4 [SCRIBE GAP: missing from sweep-20260508-1737] |
| S-3420 | 0x0042bfb0 FUN_0042bfb0 | 0x0040acd0 FUN_0040acd0 | frontend | passthrough | 2026-05-08 | called with IDs 0x1b5-0x2a3 and 0xff460000 as args; c0_promotion_frontend_a |
| S-3421 | 0x005c9d00 FUN_005c9d00 | 0x0040acd0 FUN_0040acd0 | frontend | passthrough | 2026-05-08 | return used as bool; c0_promotion_frontend_a |
| S-3422 | 0x004d8560 FUN_004d8560 | 0x0040acd0 FUN_0040acd0 | frontend | passthrough | 2026-05-08 | called with (0); queried up to 3x in sequence; c0_promotion_frontend_a |
| S-3423 | 0x00409a80 FUN_00409a80 | 0x0040acd0 FUN_0040acd0 | frontend | passthrough | 2026-05-08 | called with (param_1); c0_promotion_frontend_a |
| S-3424 | 0x0042c1a0 FUN_0042c1a0 | 0x0040acd0 FUN_0040acd0 | frontend | passthrough | 2026-05-08 | called with (param_1); c0_promotion_frontend_a |
| S-3425 | 0x0045b350 FUN_0045b350 | 0x0040acd0 FUN_0040acd0 | frontend | passthrough | 2026-05-08 | called with (param_1); c0_promotion_frontend_a |
| S-3426 | 0x004099e0 FUN_004099e0 | 0x0040acd0 FUN_0040acd0 | frontend | passthrough | 2026-05-08 | called with (param_1); c0_promotion_frontend_a |
| S-3427 | 0x0040ce80 FUN_0040ce80 | 0x0042a940 FUN_0042a940 | frontend | passthrough | 2026-05-08 | takes param_1, return used as int key in table search; c0_promotion_frontend_a |
| S-3428 | 0x0040e470 FUN_0040e470 | 0x0042ae10 FUN_0042ae10 | frontend | passthrough | 2026-05-08 | takes int index, return compared to 2; c0_promotion_frontend_a |
| S-3429 | 0x004c5c00 FUN_004c5c00 | 0x0042bcb0 FUN_0042bcb0 | frontend | passthrough | 2026-05-08 | FUN_004c5c00(DAT_00636ac8, local_14); returns pointer; c0_promotion_frontend_a |
| S-3430 | 0x004b5750 FUN_004b5750 | 0x0042bcb0 FUN_0042bcb0 | frontend | passthrough | 2026-05-08 | drawing call; coordinate order and uVar4 meaning unknown; c0_promotion_frontend_a |
| S-3431 | 0x004a2b60 FUN_004a2b60 | 0x0042d290 FUN_0042d290 | frontend | passthrough | 2026-05-08 | takes 3 args; semantics unknown; c0_promotion_frontend_a |
| S-3432 | 0x004a2c48 FUN_004a2c48 | 0x0042d290 FUN_0042d290 | frontend | passthrough | 2026-05-08 | no args; returns int; semantics unknown; c0_promotion_frontend_a |
| S-3433 | 0x0040bb50 FUN_0040bb50 | 0x0042ee00 FUN_0042ee00 | frontend | passthrough | 2026-05-08 | no args; return passed through; semantics unknown; c0_promotion_frontend_a |
| ~~S-3440~~ | ~~0x00468d80 FUN_00468d80~~ | ~~0x00469aa0 FUN_00469aa0~~ | ~~vehicle~~ | ~~resolved vehicle_dynamics_d3-20260512~~ | ~~2026-05-08~~ | ~~VehicleTerrainContactSolver; C1 new; vehicle_dynamics_d3/00468d80.md~~ |
| ~~S-3441~~ | ~~0x004694e0 FUN_004694e0~~ | ~~0x00469aa0 FUN_00469aa0~~ | ~~vehicle~~ | ~~resolved vehicle_dynamics_d3-20260512~~ | ~~2026-05-08~~ | ~~VehicleObjectContactSolver; C1 new; vehicle_dynamics_d3/004694e0.md~~ |
| ~~S-3442~~ | ~~0x004c4dc0 FUN_004c4dc0~~ | ~~0x00469df0 FUN_00469df0~~ | ~~vehicle~~ | ~~resolved vehicle_dynamics_d3-20260512~~ | ~~2026-05-08~~ | ~~RwMatrixInvert; C1 new; render_pipeline_d3/004c4dc0.md~~ |
| S-3540 | 0x004938e0 FUN_004938e0 | 0x00403910 FUN_00403910 | render | passthrough | 2026-05-08 | no-arg callee; called with (DAT_00636b78, DAT_00636b70); c0_promotion_render_a |
| S-3541 | 0x004036a0 FUN_004036a0 | 0x00403910 FUN_00403910 | render | passthrough | 2026-05-08 | called with (0, DAT_00636b78); c0_promotion_render_a |
| S-3542 | 0x0041f290 FUN_0041f290 | 0x00404650 FUN_00404650 | render | passthrough | 2026-05-08 | called with (0, 0x16); result used as arg to FUN_004c1480; c0_promotion_render_a |
| S-3543 | 0x0048ad50 FUN_0048ad50 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 1st callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3544 | 0x00487de0 FUN_00487de0 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 2nd callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3545 | 0x00406130 FUN_00406130 | 0x004072e0 FUN_004072e0 | render | passthrough | 2026-05-08 | no-arg; called unconditionally at top of FUN_004072e0; c0_promotion_render_a |
| S-3546 | 0x004b59c0 FUN_004b59c0 | 0x004072e0 FUN_004072e0 | render | passthrough | 2026-05-08 | called with (puVar2, puVar2+0xc, &local_4) inside conditional branch; c0_promotion_render_a |
| S-3547 | 0x0048e7b0 FUN_0048e7b0 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 3rd callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3548 | 0x00457610 FUN_00457610 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 4th callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3549 | 0x00475d30 FUN_00475d30 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 5th callee in 18-callee render dispatch; also promoted to C1 in c0_promotion_render_a |
| S-3550 | 0x00486f50 FUN_00486f50 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 6th callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3551 | 0x00486220 FUN_00486220 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 7th callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3552 | 0x0048fce0 FUN_0048fce0 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 8th callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3553 | 0x0048fd40 FUN_0048fd40 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 9th callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3554 | 0x0048fd10 FUN_0048fd10 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 10th callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3555 | 0x00475e50 FUN_00475e50 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 11th callee in 18-callee render dispatch; also promoted to C1 in c0_promotion_render_a |
| S-3556 | 0x00477810 FUN_00477810 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 12th callee in 18-callee render dispatch; also promoted to C1 in c0_promotion_render_a |
| S-3557 | 0x0048fd70 FUN_0048fd70 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 13th callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3558 | 0x00490490 FUN_00490490 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 14th callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3559 | 0x0048baf0 FUN_0048baf0 | 0x0040bde0 FUN_0040bde0 | render | passthrough | 2026-05-08 | no-arg; 15th callee in 18-callee render dispatch; c0_promotion_render_a |
| S-3560 | 0x004c1a40 FUN_004c1a40 | 0x0042d560 FUN_0042d560 / 0x0042f660 FUN_0042f660 | render | passthrough | 2026-05-11 | 2-arg: sets float field on object; near-clip or min-range; frontend_unmapped_a |
| S-3561 | 0x004c1d30 FUN_004c1d30 | 0x0042d420 FUN_0042d420 | render | passthrough | 2026-05-11 | no-arg alloc; returns object handle or 0; frontend_unmapped_a |
| S-3562 | 0x004c0740 FUN_004c0740 | 0x0042d420 FUN_0042d420 | render | passthrough | 2026-05-11 | 2-arg: (object, uVar2); sets field on object; frontend_unmapped_a |
| S-3563 | 0x00474890 FUN_00474890 | 0x0042e8b0 FUN_0042e8b0 | frontend | passthrough | 2026-05-11 | 1-arg: takes return value of FUN_0042e590; role not read; frontend_unmapped_a |
| S-3564 | 0x00423b40..0x00424070 (14 fns) | 0x0042c510 FUN_0042c510 | vehicle | passthrough | 2026-05-11 | 14-way AI/track slot dispatch table cases 0..13; per-node-type slot handlers; frontend_unmapped_a |


| S-3571 | 0x004b6fc0 FUN_004b6fc0 | 0x0047a1b0..0x0047aa50 (18 Lua C handlers) | track | passthrough | 2026-05-11 | Lua C arg-count / arg-index getter; shared callee of all COURSE.LUA course-config handlers; track_collision_geometry_s14 |
| S-3572 | 0x004b70d0 FUN_004b70d0 | 0x0047a1b0..0x0047aa50 (18 Lua C handlers) | track | passthrough | 2026-05-11 | Lua C string arg getter; shared callee of all COURSE.LUA course-config handlers; track_collision_geometry_s14 |
| S-3573 | 0x004b7090 FUN_004b7090 | 0x0047a280..0x0047aa20 (multi-arg handlers) | track | passthrough | 2026-05-11 | Lua C number arg getter; used in two-arg path of counter-indexed filename handlers; track_collision_geometry_s14 |
| S-3574 | 0x0042f6a0 FUN_0042f6a0 | 0x0040d040 0x0040d110 | track | passthrough | 2026-05-12 | game mode getter; called to detect modes 10/6 (tournament) and 0xb; track_world_initial_sweep |
| S-3575 | 0x0041f320 FUN_0041f320 | 0x0040d040 0x0040cf80 | track | passthrough | 2026-05-12 | player active check; returns nonzero if player slot in use; track_world_initial_sweep |
| S-3576 | 0x0041efc0 FUN_0041efc0 | 0x0040d040 | track | passthrough | 2026-05-12 | player current track group getter; compared against candidate group in cache check; track_world_initial_sweep |
| S-3577 | 0x0045b350 FUN_0045b350 | 0x0040d110 0x0040cf80 | track | passthrough | 2026-05-12 | audio reinit; called at start of post-load setup and during player teardown; track_world_initial_sweep |
| S-3578 | 0x0041a8d0 FUN_0041a8d0 | 0x0040d110 | track | passthrough | 2026-05-12 | race start init; takes start-params struct (RGB+alpha+yaw) and track base; track_world_initial_sweep |
| S-3579 | 0x004220d0 FUN_004220d0 | 0x0040d110 | track | passthrough | 2026-05-12 | track assignments array setter; receives local_10[16] per-player absolute track IDs; track_world_initial_sweep |
| S-3580 | 0x00404e00 FUN_00404e00 | 0x0040cea0 | track | passthrough | 2026-05-12 | track-name to handle A lookup; used for surface TXD; track_world_initial_sweep |
| S-3581 | 0x00404e20 FUN_00404e20 | 0x0040cea0 | track | passthrough | 2026-05-12 | track-name+group to handle B lookup; used for surface TXD variant; track_world_initial_sweep |
| S-3582 | 0x004c5cb0 FUN_004c5cb0 | 0x0040cea0 | track | passthrough | 2026-05-12 | TXD/model handle lookup from piz; called with handle+0; track_world_initial_sweep |
| S-3583 | 0x0041ecc0 FUN_0041ecc0 | 0x0040cea0 | track | passthrough | 2026-05-12 | per-player surface handle assignment; args: player, handleA, handleB; track_world_initial_sweep |
| S-3584 | 0x0041a980 FUN_0041a980 | 0x0040cf80 | track | passthrough | 2026-05-12 | vehicle reset; called when any player active during pre-reload teardown; track_world_initial_sweep |
| S-3585 | 0x00422140 FUN_00422140 | 0x0040cf80 | track | passthrough | 2026-05-12 | player state clear; called during pre-reload teardown; track_world_initial_sweep |
| S-3586 | 0x0041ec00 FUN_0041ec00 | 0x0040cf80 | track | passthrough | 2026-05-12 | per-player teardown; called for each of 4 players during pre-reload; track_world_initial_sweep |
| S-3587 | 0x0040cf40 FUN_0040cf40 | 0x0040cf80 | track | passthrough | 2026-05-12 | teardown completion finalizer; called after all 4 players torn down; track_world_initial_sweep |
| S-3588 | 0x00490000 FUN_00490000 | 0x0040cfd0 | track | passthrough | 2026-05-12 | physics world teardown step 1; first call in track-world teardown; track_world_initial_sweep |
| S-3589 | 0x0045bed0 FUN_0045bed0 | 0x0040cfd0 | track | passthrough | 2026-05-12 | audio stop step 1; called in track-world teardown; track_world_initial_sweep |
| S-3590 | 0x0045bf30 FUN_0045bf30 | 0x0040cfd0 | track | passthrough | 2026-05-12 | audio stop step 2; called in track-world teardown; track_world_initial_sweep |
| S-3591 | 0x00426b40 FUN_00426b40 | 0x0040cfd0 | track | passthrough | 2026-05-12 | track state clear by ID; receives current track ID from FUN_00426c00; track_world_initial_sweep |
| S-3592 | 0x004725f0 FUN_004725f0 | 0x0040cfd0 | track | passthrough | 2026-05-12 | physics teardown step 2; called in track-world teardown; track_world_initial_sweep |
| S-3593 | 0x00405400 FUN_00405400 | 0x0040cfd0 | track | passthrough | 2026-05-12 | subsystem clear called after DAT_0080332c cleared; track_world_initial_sweep |
| S-3594 | 0x00484c90 FUN_00484c90 | 0x0040cfd0 | track | passthrough | 2026-05-12 | subsystem teardown step 4; called in track-world teardown; track_world_initial_sweep |
| S-3595 | 0x00471df0 FUN_00471df0 | 0x0040cfd0 | track | passthrough | 2026-05-12 | subsystem teardown step 5; called in track-world teardown; track_world_initial_sweep |
| S-3596 | 0x00458c70 FUN_00458c70 | 0x004264d0 0x00426340 | track | passthrough | 2026-05-12 | cleanup before Lua load; called before FUN_0042fe30 and FUN_004074a0; track_world_initial_sweep |
| S-3597 | 0x00426460 FUN_00426460 | 0x004264d0 | track | passthrough | 2026-05-12 | powerups_gold.dff post-load hook; called only when DFF exists; track_world_initial_sweep |
| S-3598 | 0x004e6e00 FUN_004e6e00 | 0x004264d0 | track | passthrough | 2026-05-12 | DFF load completion handler; called with medal rank; track_world_initial_sweep |
| S-3599 | 0x0047a130 FUN_0047a130 | 0x004264d0 | track | passthrough | 2026-05-12 | Lua script execute; called with resolved lua filename; track_world_initial_sweep |
| S-3600 | 0x0042a470 FUN_0042a470 | 0x004264d0 0x00426340 | track | passthrough | 2026-05-12 | Lua file load; 3 args (0, filename, 1); returns handle or 0; track_world_initial_sweep |
| S-3601 | 0x004074a0 FUN_004074a0 | 0x00426340 0x0040d270 | track | passthrough | 2026-05-12 | pre-Lua init step; called before piz open in FUN_00426340 and in FUN_0040d270 non-ghost path; track_world_initial_sweep |
| S-3602 | 0x004073b0 FUN_004073b0 | 0x00426340 | track | passthrough | 2026-05-12 | Lua environment execute; called after KTCScript.lua registered; track_world_initial_sweep |
| S-3603 | 0x0041f330 FUN_0041f330 | 0x00420420 0x00420230 | track | passthrough | 2026-05-12 | vehicle name getter; returns current vehicle name string; track_world_initial_sweep |
| S-3604 | 0x0042a6b0 FUN_0042a6b0 | 0x00420420 | track | passthrough | 2026-05-12 | vehicle piz load; takes path+name+0; stores to DAT_0063d7e8; track_world_initial_sweep |
| S-3605 | 0x004c5c80 FUN_004c5c80 | 0x00420420 | track | passthrough | 2026-05-12 | set active piz context; called with vehicle handle; track_world_initial_sweep |
| S-3606 | 0x004b3fc0 FUN_004b3fc0 | 0x00420420 | track | passthrough | 2026-05-12 | BSP sector list builder; args: sector clump, output array; returns count; track_world_initial_sweep |
| S-3607 | 0x004c15c0 FUN_004c15c0 | 0x00420420 | track | passthrough | 2026-05-12 | vehicle slot handle getter; reads DAT_0063d840[playerIdx]; track_world_initial_sweep |
| S-3608 | 0x00420290 FUN_00420290 | 0x00420420 | track | passthrough | 2026-05-12 | player speed/position init; args: playerIdx, 0x3f800000 (1.0f); track_world_initial_sweep |
| S-3609 | 0x0041fe10 FUN_0041fe10 | 0x00420420 | track | passthrough | 2026-05-12 | vehicle context finalize; called at end of sector loop; track_world_initial_sweep |
| S-3610 | 0x004260b0 FUN_004260b0 | 0x00420420 | track | passthrough | 2026-05-12 | physics world handle getter A; result stored in DAT_0063d850; track_world_initial_sweep |
| S-3611 | 0x004260a0 FUN_004260a0 | 0x00420420 | track | passthrough | 2026-05-12 | physics world handle getter B; result stored in _DAT_0063d7ec; track_world_initial_sweep |
| S-3614 | 0x0041ea70 FUN_0041ea70 | 0x00426640 | render | passthrough | 2026-05-12 | callee of FUN_00426640 (DAT_0066d704 guard path); called when FUN_0041ea70()==0; c0_promotion_render_a S-ID gap resolved |
| S-3615 | 0x0041e950 FUN_0041e950 | 0x00426640 | render | passthrough | 2026-05-12 | callee of FUN_00426640; called unconditionally after FUN_0041e9b0; c0_promotion_render_a S-ID gap resolved |
| S-3616 | 0x00432ad0 FUN_00432ad0 | 0x00448730 | render | passthrough | 2026-05-12 | first callee of FUN_00448730 (HUD/frontend layer); no args; c0_promotion_render_a S-ID gap resolved |
| S-3617 | 0x004430c0 FUN_004430c0 | 0x00448730 | render | passthrough | 2026-05-12 | second callee of FUN_00448730; 1 arg (DAT_00896498); c0_promotion_render_a S-ID gap resolved |
| S-3618 | 0x00477500 FUN_00477500 | 0x00477810 | render | passthrough | 2026-05-12 | callee of FUN_00477810; called when element+0x58 != 0; no args; c0_promotion_render_a S-ID gap resolved |
| S-3619 | 0x0041e140 FUN_0041e140 | 0x00429e10 | render | passthrough | 2026-05-12 | no-arg gate in FUN_00429e10 Path-A; result checked == 0; not in hooks.csv; render_c0_promote_b |
| S-3620 | 0x00429bd0 FUN_00429bd0 | 0x00429e10 | render | passthrough | 2026-05-12 | C0/deferred; called with uVar4 (0 or 1) in FUN_00429e10 Path-B race-mode sub-cases; render_c0_promote_b |
| S-3621 | 0x004295a0 FUN_004295a0 | 0x00429e10 | render | passthrough | 2026-05-12 | C0/deferred; called in FUN_00429e10 Path-B cases 5/9/10; render_c0_promote_b |
| S-3622 | 0x004778e0 FUN_004778e0 | 0x00477a10 | render | passthrough | 2026-05-12 | callee of FUN_00477a10; called when visibility check==0 AND piVar3[1]!=0; not in hooks.csv; render_c0_promote_b |
| S-3623 | 0x004cd2d0 FUN_004cd2d0 | 0x00477a10 | render | passthrough | 2026-05-12 | C0/deferred; called with arg=4 after FUN_004cd070 returns nonzero in FUN_00477a10 loop; render_c0_promote_b |
| S-3624 | 0x00496c10 FUN_00496c10 | 0x0047b9e0 | render | passthrough | 2026-05-12 | 2-arg passthrough target of FUN_0047b9e0; not in hooks.csv; render_c0_promote_b |
| S-3625 | 0x00494fd0 FUN_00494fd0 | 0x00495080 FUN_00495080 | frontend | passthrough | 2026-05-12 | called from FUN_00495080 with transformed-float arg and param_2; not yet reversed; game_mode_cont2 |
| S-3626 | 0x004173a0 FUN_004173a0 | 0x004046a0 | util | passthrough | 2026-05-12 | float10 result; used as multiplier in FUN_004046a0 axis-calc; timer_d3_cont1_a |
| S-3627 | 0x00426c00 FUN_00426c00 | 0x004046a0 | util | passthrough | 2026-05-12 | int discriminant; switch cases 0xb/0x1a/0x27 in FUN_004046a0; also used at FUN_004117b0; timer_d3_cont1_a |
| S-3628 | 0x00426cf0 FUN_00426cf0 | 0x004046a0 | util | passthrough | 2026-05-12 | int pointer/handle; indexed via (iVar3+iVar6*4-4) in FUN_004046a0; timer_d3_cont1_a |
| S-3629 | 0x00406370 FUN_00406370 | 0x004073b0 | util | passthrough | 2026-05-12 | no-arg sub-init called before record sweep; timer_d3_cont1_a |
| S-3630 | 0x004b47e0 FUN_004b47e0 | 0x004073b0 | util | passthrough | 2026-05-12 | (local_40, puVar1[-1], 0); writes into local_40 scratch; timer_d3_cont1_a |
| S-3631 | 0x004c1480 FUN_004c1480 | 0x004073b0 | util | passthrough | 2026-05-12 | (*(puVar1[-0x30]+4), local_40, 0); consumes scratch from FUN_004b47e0; timer_d3_cont1_a |
| S-3632 | 0x0047d180 FUN_0047d180 | 0x004073b0 | util | passthrough | 2026-05-12 | (puVar1[-0x23], local_40); third in record-process chain; timer_d3_cont1_a |
| S-3633 | 0x0040e340 FUN_0040e340 | 0x0040b180 | util | passthrough | 2026-05-12 | no-arg int getter; consumed in FUN_0040b180 (==2,==3) and FUN_0041b4d0 (return stored to DAT_0063c89c); timer_d3_cont1_a |
| S-3634 | 0x00412190 FUN_00412190 | 0x0040bd80 | util | passthrough | 2026-05-12 | via thunk_FUN_00412190 @ 0x00412880; no-arg in dispatcher; timer_d3_cont1_a |
| S-3635 | 0x004904d0 FUN_004904d0 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3636 | 0x0048f260 FUN_0048f260 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3637 | 0x0048f680 FUN_0048f680 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3638 | 0x0048f740 FUN_0048f740 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3639 | 0x00475a60 FUN_00475a60 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3640 | 0x00475ef0 FUN_00475ef0 | 0x0040bd80 | util | passthrough | 2026-05-12 | via thunk_FUN_00475ef0 @ 0x00476430; no-arg in dispatcher; timer_d3_cont1_a |
| S-3641 | 0x00477730 FUN_00477730 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3642 | 0x0048ade0 FUN_0048ade0 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3643 | 0x00487df0 FUN_00487df0 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3644 | 0x00486f90 FUN_00486f90 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3645 | 0x00486270 FUN_00486270 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3646 | 0x0048a5d0 FUN_0048a5d0 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3647 | 0x0048bb70 FUN_0048bb70 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; timer_d3_cont1_a |
| S-3648 | 0x00421080 FUN_00421080 | 0x0040bd80 | util | passthrough | 2026-05-12 | no-arg sub-init in 17-call dispatcher; last in chain; timer_d3_cont1_a |
| S-3649 | 0x0041d910 FUN_0041d910 | 0x0040dbd0 | util | passthrough | 2026-05-12 | pre-dispatch action; no args; timer_d3_cont1_a |
| S-3650 | 0x0042f6a0 FUN_0042f6a0 | 0x0040dbd0 | util | passthrough | 2026-05-12 | switch discriminant; also used in FUN_0040b180/FUN_00410860/FUN_00414060; not in hooks.csv yet; timer_d3_cont1_a |
| S-3651 | 0x0041e080 FUN_0041e080 | 0x0040dbd0 | util | passthrough | 2026-05-12 | case-2 action in FUN_0040dbd0; timer_d3_cont1_a |
| S-3652 | 0x0041b520 FUN_0041b520 | 0x0040dbd0 | util | passthrough | 2026-05-12 | default-tail action in FUN_0040dbd0; timer_d3_cont1_a |
| S-3653 | 0x004117b0 FUN_004117b0 | 0x0040de00 | save | passthrough | 2026-05-12 | thunk target body of thunk_FUN_004117b0 @ 0x0040de00; one-shot replay save; timer_d3_cont1_a |
| S-3654 | 0x00430820 FUN_00430820 | 0x004117b0 | save | passthrough | 2026-05-12 | pre-write gate in replay save body; result checked == 0; timer_d3_cont1_a |
| S-3655 | 0x00483ca0 FUN_00483ca0 | 0x004117b0 | save | passthrough | 2026-05-12 | replay_write(handle, DAT_008a94a8) in replay save body; timer_d3_cont1_a |
| S-3656 | 0x004099a0 FUN_004099a0 | 0x004117b0 | save | passthrough | 2026-05-12 | post-save action in replay save body; no args; timer_d3_cont1_a |
| S-3657 | 0x00429b30 FUN_00429b30 | 0x00410860 | util | passthrough | 2026-05-12 | no-arg called at FUN_00410860 entry after DAT_007f1014 bool; timer_d3_cont1_a |
| S-3658 | 0x004a2c48 FUN_004a2c48 | 0x00410860 | util | passthrough | 2026-05-12 | clock/tick getter; result stored to DAT_0063ba80 in FUN_00410860 and used as index in FUN_00414180; timer_d3_cont1_a |
| S-3659 | 0x0042aab0 FUN_0042aab0 | 0x00410860 | util | passthrough | 2026-05-12 | first call inside trigger block (DAT_005f29b8>199 AND bVar1); timer_d3_cont1_a |
| S-3660 | 0x00429860 FUN_00429860 | 0x00410860 | util | passthrough | 2026-05-12 | called when FUN_0042f6a0==2; result compared with 0xb; timer_d3_cont1_a |
| S-3661 | 0x0043aee0 FUN_0043aee0 | 0x00410860 | util | passthrough | 2026-05-12 | called on FUN_00429860==0xb path; timer_d3_cont1_a |
| S-3662 | 0x00448700 FUN_00448700 | 0x00410860 | util | passthrough | 2026-05-12 | called with (1,iVar3-1) on trigger path or (0,0) on else path; timer_d3_cont1_a |
| S-3663 | 0x0040e460 FUN_0040e460 | 0x00410860 | util | passthrough | 2026-05-12 | called with (1) on trigger path; timer_d3_cont1_a |
| S-3664 | 0x00424b80 FUN_00424b80 | 0x00410860 | util | passthrough | 2026-05-12 | called in else path of trigger block; no args; timer_d3_cont1_a |
| S-3665 | 0x0041eda0 FUN_0041eda0 | 0x00410860 | util | passthrough | 2026-05-12 | called with (loop_idx,1) for non-sentinel slots in loop D; timer_d3_cont1_a |
| S-3666 | 0x00413fa0 FUN_00413fa0 | 0x00414060 | util | passthrough | 2026-05-12 | int return; uses param_1 of FUN_00414060; timer_d3_cont1_a |
| S-3667 | 0x00431d80 FUN_00431d80 | 0x00414060 | util | passthrough | 2026-05-12 | int return on case 6 path; multiplied by _DAT_005cd088; timer_d3_cont1_a |
| S-3668 | 0x0042fe80 FUN_0042fe80 | 0x00414060 | util | passthrough | 2026-05-12 | int return on case 10 path; multiplied by _DAT_005cd088; timer_d3_cont1_a |
| S-3669 | 0x00472650 FUN_00472650 | 0x00414180 | util | passthrough | 2026-05-12 | (float,float) writer; called per-record with pair from DAT_005f2d80[iVar2*8..iVar2*8+4]; timer_d3_cont1_a |
| S-3670 | 0x0040e4b0 FUN_0040e4b0 | 0x00414220 | util | passthrough | 2026-05-12 | int return; -1 sentinel = early-out; timer_d3_cont1_a |
| S-3671 | 0x0040e470 FUN_0040e470 | 0x00414220 | util | passthrough | 2026-05-12 | int return; (i) returning 1 marks winner index; timer_d3_cont1_a |
| S-3672 | 0x004194f0 FUN_004194f0 | 0x00418990 | util | passthrough | 2026-05-12 | thunk target body of thunk_FUN_004194f0 @ 0x00418990; record-sweep over 0x6c and 0x54 stride tables; timer_d3_cont1_a |
| S-3673 | 0x00418a30 FUN_00418a30 | 0x00418990 | util | passthrough | 2026-05-12 | loop-1 iterator; pointer return; record stride 0x6c bytes; timer_d3_cont1_a |
| S-3674 | 0x00418e30 FUN_00418e30 | 0x00418990 | util | passthrough | 2026-05-12 | between-loops action 1 (post-loop-1 cleanup); timer_d3_cont1_a |
| S-3675 | 0x00418e50 FUN_00418e50 | 0x00418990 | util | passthrough | 2026-05-12 | between-loops action 2 (pre-loop-2 setup); timer_d3_cont1_a |
| S-3676 | 0x004190f0 FUN_004190f0 | 0x00418990 | util | passthrough | 2026-05-12 | loop-2 iterator; pointer return; record stride 0x54 bytes; timer_d3_cont1_a |
| S-3677 | 0x0040b890 FUN_0040b890 | 0x0041b4d0 | util | passthrough | 2026-05-12 | no-arg getter; result stored to DAT_0063c8c4; timer_d3_cont1_a |
| S-3678 | 0x0041adb0 FUN_0041adb0 | 0x0041b4d0 | util | passthrough | 2026-05-12 | invoked once per 0x74-byte record; uses global cursor; timer_d3_cont1_a |
| S-3679 | 0x004b6520 FUN_004b6520 | 0x0041bf20 | util | passthrough | 2026-05-12 | (ptr,0x10) — likely zero-init/memset of first 16 bytes of 0x16c-byte record; also used by input_lua_d2; timer_d3_cont1_a |
| S-3680 | 0x0041b720 FUN_0041b720 | 0x0041bf20 | util | passthrough | 2026-05-12 | iterator returning pointer; loop terminates when iVar1+0x16c >= 0x63cd90; record stride 0x16c; timer_d3_cont1_a |
| S-3681 | 0x0041b770 FUN_0041b770 | 0x0041c010 | util | passthrough | 2026-05-12 | invoked once per 0x16c-byte record without args; uses global cursor (same table as FUN_0041bf20 / FUN_0041b720); timer_d3_cont1_a |
| S-3682 | (id-rename-padding) | n/a | util | id-rename-padding | 2026-05-12 | placeholder reserved after S-3625 collision-shift (concurrent session took S-3625); timer_d3_cont1_a |
| S-3683 | 0x005aac20 FUN_005aac20 | 0x005aacf0 FUN_005aacf0 | audio | resolved | 2026-05-16 | resolved by promote_c2_rws_audio_loader (session promote_c2_rws_audio_loader-20260516-0309); promoted C1->C2; see re/analysis/promote_c2_rws_audio_loader/5aac20.md |
| S-3684 | 0x005aa7e0 FUN_005aa7e0 | 0x005a9f60 FUN_005a9f60 | audio | resolved | 2026-05-16 | resolved by promote_c2_rws_audio_loader (session promote_c2_rws_audio_loader-20260516-0309); promoted C1->C2; see re/analysis/promote_c2_rws_audio_loader/5aa7e0.md |
| S-3685 | 0x005ae400 FUN_005ae400 | 0x005a9f60 FUN_005a9f60 | audio | resolved | 2026-05-16 | resolved by promote_c2_rws_audio_loader (session promote_c2_rws_audio_loader-20260516-0309); promoted C1->C2; see re/analysis/promote_c2_rws_audio_loader/5ae400.md |
| S-3686 | 0x005bf610 FUN_005bf610 | 0x005be0f0 FUN_005be0f0 | audio | resolved | 2026-05-16 | resolved by promote_c2_rws_audio_loader (session promote_c2_rws_audio_loader-20260516-0309); promoted C1->C2; see re/analysis/promote_c2_rws_audio_loader/5bf610.md |
| S-3687 | 0x005be140 FUN_005be140 | 0x005be160 FUN_005be160 | audio | resolved | 2026-05-16 | resolved by promote_c2_rws_audio_loader (session promote_c2_rws_audio_loader-20260516-0309); promoted C1->C2; see re/analysis/promote_c2_rws_audio_loader/5be140.md |
| S-3688 | 0x005be190 FUN_005be190 | 0x005be160 FUN_005be160 | audio | resolved | 2026-05-16 | resolved by promote_c2_rws_audio_loader (session promote_c2_rws_audio_loader-20260516-0309); promoted C1->C2; see re/analysis/promote_c2_rws_audio_loader/5be190.md |
| S-3689 | 0x005bf690 FUN_005bf690 | 0x005be160 FUN_005be160 | audio | resolved | 2026-05-16 | resolved by promote_c2_rws_audio_loader (session promote_c2_rws_audio_loader-20260516-0309); promoted C1->C2; see re/analysis/promote_c2_rws_audio_loader/5bf690.md |
| S-3690 | 0x00550580 FUN_00550580 | 0x005507b0 VFS_Open | io | passthrough | 2026-05-12 | VFS file-open implementation; called as FUN_00550580(handler, path, flags, 0, 0); counterpart to VFS_FileExists (FUN_00550b00); font_text_cont1 |
| S-3691 | 0x004c5ca0 FUN_004c5ca0 | 0x00555af0 FontCtx_LoadMetrics_Met | hud | passthrough | 2026-05-12 | RwTexDictionaryGetCurrent(); returns current active TXD; save half of TXD save/restore pair with FUN_004c5c80; font_text_cont1 |
| S-3692 | 0x004c5cb0 FUN_004c5cb0 | 0x00555af0 FontCtx_LoadMetrics_Met | hud | passthrough | 2026-05-12 | RwTexDictionaryFindNamedTexture(name, buf); looks up texture by name in current TXD; returns tex dict entry ptr or NULL; font_text_cont1 |
| S-3693 | 0x004ce2d0 FUN_004ce2d0 | 0x00555af0 FontCtx_LoadMetrics_Met | hud | passthrough | 2026-05-12 | called as FUN_004ce2d0(DAT_00912a0c); purpose uncertain — RW stream seek or reset; appears in audio/boot contexts as "optional pre-seek"; font_text_cont1 |
| S-3694 | 0x00550a20 FUN_00550a20 | 0x00555af0 FontCtx_LoadMetrics_Met | hud | passthrough | 2026-05-12 | VFS_ReadLine(buf, size, ctx); reads next line from open file into buf; returns 0 at EOF; called in .met parse loop; font_text_cont1 |
| S-3695 | 0x005c4c60 FUN_005c4c60 | 0x00555af0 FontCtx_LoadMetrics_Met | hud | passthrough | 2026-05-12 | GlyphBuf_Resize(handle, count, type); resizes glyph data block in font ctx; font_text_cont1 |
| S-3696 | 0x005c4d30 FUN_005c4d30 | 0x00555af0 FontCtx_LoadMetrics_Met | hud | passthrough | 2026-05-12 | GlyphBuf_GetBase(handle) → ptr; returns base pointer to resized glyph data buffer; font_text_cont1 |
| S-3697 | 0x004cdca0 FUN_004cdca0 | 0x00555ff0 FontCtx_LoadMetrics_Atlas | hud | passthrough | 2026-05-12 | RwImageCreate(width, height, depth); allocates RwImage struct; called with atlas tex dims + 0x20; font_text_cont1 |
| S-3698 | 0x004cdd00 FUN_004cdd00 | 0x00555ff0 FontCtx_LoadMetrics_Atlas | hud | passthrough | 2026-05-12 | RwImageDestroy(img*); frees scan image after atlas glyph extraction complete; font_text_cont1 |
| S-3699 | 0x004d52d0 FUN_004d52d0 | 0x00555ff0 FontCtx_LoadMetrics_Atlas | hud | passthrough | 2026-05-12 | called as FUN_004d52d0(img, *piVar8); binds texture pixels to scan image (RwTextureGetRaster or similar); font_text_cont1 |
| S-3700 | 0x005c4d50 FUN_005c4d50 | 0x00554050 FontCanvas_Shutdown | hud | passthrough | 2026-05-12 | GlyphBuf_SpecialFree(); no-arg free; called when font_canvas_node[1]==-1 (special case); font_text_cont1 |
| S-3701 | 0x00553e80 FUN_00553e80 | 0x00554050 FontCanvas_Shutdown | hud | passthrough | 2026-05-12 | FontChain_FreeList(node*); recursive free of remaining glyph node linked list; font_text_cont1 |
| S-3702 | 0x004c5770 FUN_004c5770 | 0x00552b90 FontSys_Shutdown | hud | passthrough | 2026-05-12 | FontCtx_Free(ctx*); frees individual font context object from 32-slot stack; font_text_cont1 |
| S-3703 | 0x00555830 FUN_00555830 | 0x00555280 FontSys_ShutdownContextPool | hud | passthrough | 2026-05-12 | FontObj_Free(obj*); frees font object node[1] during context pool teardown; also called in FontText_HudShutdown (FUN_00427620); font_text_cont1 |
| S-3704 | 0x0055dca0 FUN_0055dca0 | 0x0055dc70 FUN_0055dc70 | physics | passthrough | 2026-05-12 | BVH/physics struct initializer; 8-field 0x20-byte init: [0]=FUN_00559b50 result [1]=FUN_0055f450 result [2][5]=two arrays of (capacity*4) bytes [6]=param+4 [7]=calloc'd param+4 * 8 bytes (zero 8-byte pairs); track_loader_d3_cont1 |
| S-3705 | 0x004bc440 FUN_004bc440 | 0x004b7aa0 FUN_004b7aa0 | input | passthrough | 2026-05-12 | Lua chunk loader prep; receives (buf param_2 param_3 param_4); loads chunk into 276-byte buffer; input_lua_d2_cont1 |
| S-3706 | 0x004b79c0 FUN_004b79c0 | 0x004b7aa0 FUN_004b7aa0 | input | passthrough | 2026-05-12 | Lua chunk executor; receives (L buf binary_flag); executes loaded chunk; binary_flag=1 if first byte=0x1b; input_lua_d2_cont1 |
| S-3707 | 0x004bee20 FUN_004bee20 | 0x004b7ff0 FUN_004b7ff0 | input | passthrough | 2026-05-12 | TObject capture allocator; receives (L count); allocates area for count×16-byte TObjects; returns base ptr; input_lua_d2_cont1 |
| S-3708 | 0x004b7740 FUN_004b7740 | 0x004b8340 FUN_004b8340 | input | passthrough | 2026-05-12 | tag method call dispatcher; receives (L fn_slot_ptr 0); invokes tag-method function on stack; input_lua_d2_cont1 |
| S-3709 | 0x004b96c0 FUN_004b96c0 | 0x004b9650 FUN_004b9650 | input | passthrough | 2026-05-12 | event name→integer index; receives (event_name_string); returns 0..14 or -1; input_lua_d2_cont1 |
| S-3710 | 0x004b9950 FUN_004b9950 | 0x004b9aa0 FUN_004b9aa0 | input | passthrough | 2026-05-12 | string intern by pointer+length; receives (L str len); interns/hashes string; input_lua_d2_cont1 |
| S-3711 | 0x004b9bb0 FUN_004b9bb0 | 0x004b8340 FUN_004b8340 | input | passthrough | 2026-05-12 | table key probe; receives (table key); returns value slot ptr or DAT_005d8808 if absent; input_lua_d2_cont1 |
| S-3712 | 0x004b9630 FUN_004b9630 | 0x004b8340 FUN_004b8340 | input | passthrough | 2026-05-12 | value type reader; receives (slot_ptr); returns type tag integer; input_lua_d2_cont1 |
| S-3713 | 0x004b9ef0 FUN_004b9ef0 | 0x004b8340 FUN_004b8340 | input | passthrough | 2026-05-12 | table key inserter; receives (L table key_desc[{type=3 key}]); inserts new entry; returns new slot ptr; input_lua_d2_cont1 |
| S-3714 | 0x00546bf0 FUN_00546bf0 | 0x00546b10 FUN_00546b10 | vehicle | passthrough | 2026-05-13 | matrix→quat degenerate case (m00-dominant row); called when m00 > m11 and m00 > m22; replay_record_cont1 |
| S-3715 | 0x00546c50 FUN_00546c50 | 0x00546b10 FUN_00546b10 | vehicle | passthrough | 2026-05-13 | matrix→quat degenerate case (m11-dominant row); called when m11 > m22 and trace <= epsilon; replay_record_cont1 |
| S-3716 | 0x00546cb0 FUN_00546cb0 | 0x00546b10 FUN_00546b10 | vehicle | passthrough | 2026-05-13 | matrix→quat degenerate case (m22-dominant row); called when m22 is largest diagonal; replay_record_cont1 |
| S-3717 | 0x0041a8b0 FUN_0041a8b0 | 0x0041a9b0 FUN_0041a9b0 Ghost::SetupTransforms | vehicle | passthrough | 2026-05-13 | per-slot ghost transform setup; takes (slot_idx, ghost_obj_ptr); replay_record_cont1 |
| S-3718 | 0x0041aac0 FUN_0041aac0 | 0x0041ad00 FUN_0041ad00 Ghost::ApplyTransforms | vehicle | passthrough | 2026-05-13 | ghost per-frame update step A; takes (ghost_array_ptr); replay_record_cont1 |
| S-3719 | 0x0041ac60 FUN_0041ac60 | 0x0041ad00 FUN_0041ad00 Ghost::ApplyTransforms | vehicle | passthrough | 2026-05-13 | ghost global step; no args; replay_record_cont1 |
| S-3720 | 0x0041a9d0 FUN_0041a9d0 | 0x0041ad00 FUN_0041ad00 Ghost::ApplyTransforms | vehicle | passthrough | 2026-05-13 | ghost per-frame update step B; takes (ghost_array_ptr); replay_record_cont1 |
| S-3721 | 0x0041a4a0 FUN_0041a4a0 | 0x0041ad00 FUN_0041ad00 Ghost::ApplyTransforms | vehicle | passthrough | 2026-05-13 | per-slot ghost render prepare; takes (slot_idx, ghost_obj_ptr); replay_record_cont1 |
| S-3722 | 0x0041a5d0 FUN_0041a5d0 | 0x0041ad00 FUN_0041ad00 Ghost::ApplyTransforms | vehicle | passthrough | 2026-05-13 | per-slot ghost render apply; takes (slot_idx); replay_record_cont1 |
| S-3723 | 0x0041a840 FUN_0041a840 | 0x0041a960 FUN_0041a960 Ghost::Init | vehicle | passthrough | 2026-05-13 | per-slot ghost initialiser; takes (slot_idx); replay_record_cont1 |
| S-3724 | 0x0055e200 FUN_0055e200 | 0x0047e9c0 PhysicsSceneInitSequence | vehicle | passthrough | 2026-05-13 | first scene init call; args (scene, param); vehicle_update_d3_cont |
| S-3725 | 0x0055fe50 FUN_0055fe50 + 0x0055fea0 FUN_0055fea0 + 0x0055ff90 FUN_0055ff90 + 0x0055ff70 FUN_0055ff70 | 0x0047e9c0 PhysicsSceneInitSequence | vehicle | passthrough | 2026-05-13 | physics scene setup cluster (calls 2..5 of init sequence); args (scene) each; vehicle_update_d3_cont |
| S-3726 | 0x0047d640 FUN_0047d640 | 0x0047e9c0 PhysicsSceneInitSequence | vehicle | passthrough | 2026-05-13 | uses scene sub-objects at scene+0x8c and scene+0x90; call 6 of init; vehicle_update_d3_cont |
| S-3727 | 0x00561e60 FUN_00561e60 + 0x00561040 FUN_00561040 + 0x00561e80 FUN_00561e80 | 0x0047e9c0 PhysicsSceneInitSequence | vehicle | passthrough | 2026-05-13 | physics scene setup cluster (calls 7..9 of init sequence); args (scene) each; vehicle_update_d3_cont |
| S-3728 | 0x0047def0 FUN_0047def0 | 0x0047e9c0 PhysicsSceneInitSequence | vehicle | passthrough | 2026-05-13 | uses scene sub-objects at scene+0x8c and scene+0x90; call 10 of init; vehicle_update_d3_cont |
| S-3729 | 0x00561390 FUN_00561390 + 0x00561c50 FUN_00561c50 | 0x0047e9c0 PhysicsSceneInitSequence | vehicle | passthrough | 2026-05-13 | physics scene finalize cluster (calls 11..12 of init sequence); args (scene) each; vehicle_update_d3_cont |
| S-3730 | 0x0055ade0 FUN_0055ade0 | 0x0047d240 VehiclePhysicsBodyPlace | vehicle | passthrough | 2026-05-13 | deregister body from parent scene; args (parent, body); vehicle_update_d3_cont |
| S-3731 | 0x004c4d20 FUN_004c4d20 | 0x0047d240 VehiclePhysicsBodyPlace | vehicle | passthrough | 2026-05-13 | set transform on node; args (node, &local_2c, float, 0); vehicle_update_d3_cont |
| S-3732 | 0x004c45f0 FUN_004c45f0 | 0x0047d240 VehiclePhysicsBodyPlace | vehicle | passthrough | 2026-05-13 | update/dirty transform node; args (node); vehicle_update_d3_cont |
| S-3733 | 0x0055dff0 FUN_0055dff0 | 0x0047d240 VehiclePhysicsBodyPlace / 0x0057c220 RWP37WorldSlotAssign | vehicle | passthrough | 2026-05-13 | reset physics body state; args (body, 0); vehicle_update_d3_cont |
| S-3734 | 0x0057c420 FUN_0057c420 | 0x0047d240 VehiclePhysicsBodyPlace | vehicle | passthrough | 2026-05-13 | get physics body AABB; args (body, &local_20); returns transform; vehicle_update_d3_cont |
| S-3735 | 0x0055ab30 FUN_0055ab30 | 0x0047d240 VehiclePhysicsBodyPlace | vehicle | passthrough | 2026-05-13 | set physics body matrix from AABB; args (parent, body, transform); vehicle_update_d3_cont |
| S-3736 | 0x0055c380 FUN_0055c380 | 0x0055c810 PhysicsBodySetLinearVelocity | vehicle | passthrough | 2026-05-13 | physics body prepare/validate; args (body, &DAT_005e4fe0); vehicle_update_d3_cont |
| S-3737 | 0x0055bbf0 FUN_0055bbf0 | 0x0055b940 PhysicsBodySetMass | vehicle | passthrough | 2026-05-13 | compute inertia tensor from shape; args (*param_1, &Ix, &tensor); vehicle_update_d3_cont |
| S-3738 | 0x0055b860 FUN_0055b860 | 0x0057c500 RWP37WorldCreate | vehicle | passthrough | 2026-05-13 | link body list into world; args (world, body_list_ptr); vehicle_update_d3_cont |
| S-3739 | 0x0057c1b0 FUN_0057c1b0 | 0x0057c500 RWP37WorldCreate | vehicle | passthrough | 2026-05-13 | init body list; args (body_list_ptr); vehicle_update_d3_cont |
| S-3740 | 0x0055ded0 FUN_0055ded0 | 0x0057c300 RWP37BodyCreate | vehicle | passthrough | 2026-05-13 | init body slot entry; args (body, 1, 0); vehicle_update_d3_cont |
| S-3741 | 0x00564130 FUN_00564130 | 0x0057c500 RWP37WorldCreate | vehicle | passthrough | 2026-05-13 | allocate pool: args (elem_size=0x60, count=0x10, flags=0x30900); returns pool ptr; vehicle_update_d3_cont |
| S-3742 | 0x00481e00 FUN_00481e00 | 0x004826d0 PhysicsCollisionBodyCreate | vehicle | passthrough | 2026-05-13 | create physics body from scene; args (scene, mass=1.0f, linked_scene_or_0); vehicle_update_d3_cont |
| S-3743 | 0x00482140 FUN_00482140 | 0x004826d0 PhysicsCollisionBodyCreate | vehicle | passthrough | 2026-05-13 | set gravity enable flag on body; args (body, bool); vehicle_update_d3_cont |
| S-3744 | 0x00563810 FUN_00563810 | 0x004826d0 PhysicsCollisionBodyCreate | vehicle | passthrough | 2026-05-13 | activate/wake physics body; args (body); vehicle_update_d3_cont |
| S-3745 | 0x00564c80 FUN_00564c80 | 0x00559c40 PhysicsSceneBodyRegister / 0x0055ae70 PhysicsSceneBodySetShapeFlags | vehicle | passthrough | 2026-05-13 | scene shape error/update callback; args (scene, slot_idx, shape_data); vehicle_update_d3_cont |
| S-3746 | 0x00565570 FUN_00565570 | 0x0055ae70 PhysicsSceneBodySetShapeFlags | vehicle | passthrough | 2026-05-13 | apply shape flags to scene slot; args (scene, slot_idx, flags); vehicle_update_d3_cont |
| S-3747 | 0x00565200 FUN_00565200 + 0x00565260 FUN_00565260 | 0x0055ae70 PhysicsSceneBodySetShapeFlags | vehicle | passthrough | 2026-05-13 | get/clear old shape data pair; args (scene, slot, data_out) / (scene, slot); vehicle_update_d3_cont |
| S-3748 | 0x004d7ff0 FUN_004d7ff0 | 0x004c3c60 Vec2Normalize | render | passthrough | 2026-05-14 | RW error code passthrough (C1); called with error code 0x19 when magnitude < epsilon; identity-like fn (returns param_1); c3_render_math |
| S-3749 | 0x004d8480 FUN_004d8480 | 0x004c3c60 Vec2Normalize | render | passthrough | 2026-05-14 | RW first-error recorder (C1); called with &local_min_val when magnitude < epsilon; stores {uint,uint} at DAT_007d3ff8[DAT_007d6c5c] if slot empty; c3_render_math |
| S-3770 | 0x004d40d0 FUN_004d40d0 | 0x004cd070 FUN_004cd070 | render | passthrough | 2026-05-16 | RW plugin dispatcher (3-arg: ptr header flag); C1; called with flag=1 from 004cd070 and flag=0 from 004cd2d0; promote_c2_rw_render_submit |
| S-3771 | 0x004d40c0 FUN_004d40c0 | 0x004cd2d0 FUN_004cd2d0 | render | passthrough | 2026-05-16 | 5-byte getter: returns DAT_007d4710 (current RW pipeline ptr); C1; promote_c2_rw_render_submit |
| S-3780 | 0x00558180 FUN_00558180 | 0x00558240 FUN_00558240 | render | passthrough | 2026-05-16 | layout/fill-init; called as FUN_00558180(local_1c, &param_2); result used as fill source uVar3; promote_c2_rw_state |
| S-3781 | 0x00558400 FUN_00558400 | 0x00558240 FUN_00558240 | render | passthrough | 2026-05-16 | fill operation; called as FUN_00558400(uVar3, local_1c, offset, pitch) per block; promote_c2_rw_state |
| S-3820 | 0x004ac869 FUN_004ac869 | 0x004b302f __stricmp | util | passthrough | 2026-05-16 | unmatched callee inside __stricmp body; CRT internal not resolved by FidDB; depth-2 of promote_c2_txd_loader |
| S-3900 | 0x00496ec0 FUN_00496ec0 | 0x00497060 FUN_00497060 | render | passthrough | 2026-05-16 | initialises 13-entry local_80 array; passed to FUN_00497000 for projection matrix; promote_c2_input_lua |
| S-3901 | 0x004ed6ba thunk_FUN_004ed6ad | 0x00497060 FUN_00497060 | render | passthrough | 2026-05-16 | thunk to FUN_004ed6ad; matrix operation on two 64-byte local buffers; promote_c2_input_lua |
| S-3890 | 0x00558470 FUN_00558470 | 0x004938c0 FUN_004938c0 | render | new | 2026-05-16 | void call #1 in teardown sequence; not reversed; promote_c2_launch_handshake |
| S-3891 | 0x00550390 FUN_00550390 | 0x004938c0 FUN_004938c0 | render | new | 2026-05-16 | void call #2 in teardown sequence; not reversed; promote_c2_launch_handshake |
| S-3892 | 0x004c2f60 FUN_004c2f60 | 0x004938c0 FUN_004938c0 | render | new | 2026-05-16 | void call #3 in teardown sequence; not reversed; promote_c2_launch_handshake |
| S-3893 | 0x004c3040 FUN_004c3040 | 0x004938c0 FUN_004938c0 | render | new | 2026-05-16 | void call #4 in teardown sequence; not reversed; promote_c2_launch_handshake |
| S-3894 | 0x004c3270 FUN_004c3270 | 0x004938c0 FUN_004938c0 | render | new | 2026-05-16 | void call #5 in teardown sequence; not reversed; promote_c2_launch_handshake |