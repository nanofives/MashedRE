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

## Resolved stubs (audit trail — do not delete)

| ID | RVA | Caller | Resolved date | Resolution |
|----|-----|--------|---------------|------------|
|    |     |        |               |            |

## Conventions

- ID format: `S-NNNN`, monotonic, never reused.
- Every stub must have a corresponding `// STUB S-NNNN` comment in source.
- `re-classify` skill writes new rows; `hook-author` skill enforces that no new C3 row in `hooks.csv` lands while the function still has unresolved stubs.
| S-0080 | 0x00551510 | 0x00550390 sub_00550390 | render | passthrough | 2026-05-02 | FUN_00551510; called as FUN_00551510(node_ptr, 2) per list-node iteration; depth-2 of RW_TEAR_FN |
| S-0081 | 0x004c2c90 | 0x004c2f60 sub_004c2f60 + 0x004c3040 sub_004c3040 | render | passthrough | 2026-05-02 | FUN_004c2c90; called with (DAT_007d3ff8+0x10,0x12/3,0,0,0) and (DAT_007d3ff8+4,1,0,0,0); likely core RW engine state-transition function; depth-2 of RW_TEAR_FN |
| S-0082 | 0x004d7ca0 | 0x004c3270 sub_004c3270 | render | passthrough | 2026-05-02 | FUN_004d7ca0; called when DAT_007d3ff4==0 in final teardown step; depth-2 of RW_TEAR_FN |
| S-0083 | 0x004ccf20 | 0x004c3270 sub_004c3270 | render | passthrough | 2026-05-02 | FUN_004ccf20; called when DAT_007d3ff4==0 in final teardown step; depth-2 of RW_TEAR_FN |
| S-0260 | 0x004a42c5 | 0x004987b0 FUN_004987b0 | input | passthrough | 2026-05-02 | FUN_004a42c5; varargs string formatter (3-arg: output_buf, format_ptr, va_list_ptr); depth-2 callee of FUN_004987b0; filed as D-0700 |
| S-0100 | 0x004522d6 | 0x004522d0 FUN_004522d0 | audio | passthrough | 2026-05-02 | Indirect dispatch target through DAT_007d3ff8+0x10c; not statically traceable |
| S-0101 | 0x004d7ff0 FUN_004d7ff0 | 0x004cbd30 FUN_004cbd30 | audio | passthrough | 2026-05-02 | Error-code constructor; called on short read and EOF; depth-2 of FUN_004cbd30 |
| S-0102 | 0x004d8480 FUN_004d8480 | 0x004cbd30 FUN_004cbd30 | audio | passthrough | 2026-05-02 | Error dispatcher/logger; called after FUN_004d7ff0; depth-2 of FUN_004cbd30 |
| S-0103 | 0x00550950 FUN_00550950 | 0x004cbd30 FUN_004cbd30 | audio | passthrough | 2026-05-02 | File-read wrapper (fread-level); cases 1+2 of stream dispatch; depth-2 of FUN_004cbd30 |
| S-0104 | 0x00550af0 FUN_00550af0 | 0x004cbd30 FUN_004cbd30 | audio | passthrough | 2026-05-02 | EOF checker; called after short read or seek failure; also called from FUN_004cc050; depth-2 |
| S-0105 | 0x005509b0 FUN_005509b0 | 0x004cc050 FUN_004cc050 | audio | passthrough | 2026-05-02 | File-seek wrapper (fseek-level); seek mode 1; depth-2 of FUN_004cc050 |
| S-0106 | 0x005a7a40 FUN_005a7a40 | 0x005a79a0 FUN_005a79a0 | audio | passthrough | 2026-05-02 | Searches pool list for wave node match; depth-2 of FUN_005a79a0 |
| S-0107 | 0x005ade90 FUN_005ade90 | 0x005a79a0 FUN_005a79a0 | audio | passthrough | 2026-05-02 | Resets embedded list head at param_1+0xc; depth-2 of FUN_005a79a0 |
| S-0108 | 0x005a7ea0 FUN_005a7ea0 | 0x005a79a0 FUN_005a79a0 | audio | passthrough | 2026-05-02 | Final deallocation of audio object; depth-2 of FUN_005a79a0 |
| S-0109 | 0x005ae0c0 FUN_005ae0c0 | 0x005a7ee0 FUN_005a7ee0 | audio | passthrough | 2026-05-02 | Initialises sub-structure at audio_obj+0x24; called conditionally on param_1[0] != 0; depth-2 |
| S-0110 | 0x005ae010 FUN_005ae010 | 0x005a7ee0 FUN_005a7ee0 | audio | passthrough | 2026-05-02 | Links audio_obj with sub-structure at +0x24; depth-2 of FUN_005a7ee0 |
| S-0111 | 0x005adfe0 FUN_005adfe0 | 0x005a7ee0 FUN_005a7ee0 | audio | passthrough | 2026-05-02 | Initialises sub-structure at audio_obj+0x34; depth-2 of FUN_005a7ee0 |
| S-0112 | 0x005ac740 FUN_005ac740 | 0x005abcf0 FUN_005abcf0 | audio | passthrough | 2026-05-02 | Cleans sub-structure at wave_node+0x10 or +0x2c; called twice; depth-2 of FUN_005abcf0 |
| S-0113 | 0x005a7e70 FUN_005a7e70 | 0x005abcf0 FUN_005abcf0 | audio | passthrough | 2026-05-02 | Unknown wave_node operation; depth-2 of FUN_005abcf0 |
| S-0114 | 0x005ae030 FUN_005ae030 | 0x005abcf0 FUN_005abcf0 | audio | passthrough | 2026-05-02 | Unknown wave_node operation; depth-2 of FUN_005abcf0 |
| S-0115 | 0x005abcb0 FUN_005abcb0 | 0x005abcf0 FUN_005abcf0 | audio | passthrough | 2026-05-02 | Final deallocation of wave_node; depth-2 of FUN_005abcf0 |
| S-0116 | 0x005ac210 FUN_005ac210 | 0x005abfa0 FUN_005abfa0 | audio | passthrough | 2026-05-02 | Creates wave object from 0x803 format data; depth-2 of FUN_005abfa0 |
| S-0117 | 0x005adf30 FUN_005adf30 | 0x005abfa0 FUN_005abfa0 | audio | passthrough | 2026-05-02 | Compares two format descriptors; used against DAT_005e6414 and DAT_005e6444; depth-2 |
| S-0118 | 0x005aec30 FUN_005aec30 | 0x005abfa0 FUN_005abfa0 | audio | passthrough | 2026-05-02 | Byte-swaps audio sample buffer; called when endian flag set and bit-depth > 8; depth-2 |
| S-0119 | 0x005abd30 FUN_005abd30 | 0x005abfa0 FUN_005abfa0 | audio | passthrough | 2026-05-02 | Feeds PCM chunk to audio system; depth-2 of FUN_005abfa0 |
| S-0120 | 0x005abf80 FUN_005abf80 | 0x005abfa0 FUN_005abfa0 | audio | passthrough | 2026-05-02 | Drain/flush audio system; called in loop while returns 1; depth-2 of FUN_005abfa0 |
| S-0121 | 0x005ae920 FUN_005ae920 | 0x005ade10 FUN_005ade10 | audio | passthrough | 2026-05-02 | Returns list node to free pool at DAT_009146c0; depth-2 of FUN_005ade10 |
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
| S-0200 | 0x004c2d90 | 0x00472380 FUN_00472380 | render | passthrough | 2026-05-02 | FUN_004c2d90; called FUN_004c2d90(0,4,FUN_004d7ff0,FUN_004d7ff0); result gating further execution; depth-3 of rw_engine_init_d2 |
| S-0201 | 0x004e7d40 | 0x00472380 FUN_00472380 | render | passthrough | 2026-05-02 | FUN_004e7d40; called FUN_004e7d40(0x10,0x26990004,&LAB_00472330,FUN_004d7ff0,&LAB_00472360); return→DAT_0086ecd4; depth-3 of rw_engine_init_d2 |
| S-0280 | 0x00550b00 FUN_00550b00 | 0x00404f80 sub_00404f80 | save | passthrough | 2026-05-02 | file-exists check; single filename arg; depth-2 of gamesave.bin exists-predicate; DEFERRED D-0760 |
| S-0281 | 0x004cc230 FUN_004cc230 | 0x004b3b70 sub_004b3b70 + 0x004b3bb0 sub_004b3bb0 | save | passthrough | 2026-05-02 | file-open; args (2, mode, filename); mode=1 read / mode=2 write; depth-2 of both file wrappers; DEFERRED D-0761 |
| S-0282 | 0x004cbd30 FUN_004cbd30 | 0x004b3b70 sub_004b3b70 | save | passthrough | 2026-05-02 | read operation; args (handle, buffer, size); depth-2 of file-read wrapper; DEFERRED D-0762 |
| S-0283 | 0x004cc160 FUN_004cc160 | 0x004b3b70 sub_004b3b70 + 0x004b3bb0 sub_004b3bb0 | save | passthrough | 2026-05-02 | close; args (handle, 0) in read path; args (write_result, 0) in write path U-0288; depth-2; DEFERRED D-0763 |
| S-0284 | 0x004cbe80 FUN_004cbe80 | 0x004b3bb0 sub_004b3bb0 | save | passthrough | 2026-05-02 | write operation; args (handle, buffer, size, ESI_caller); 4th arg uncertain U-0287; depth-2 of file-write wrapper; DEFERRED D-0764 |
| S-0202 | 0x00498950 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_00498950; called for saved-subsystem check; return non-zero copies DAT_0077338c/DAT_00773390; depth-3 of rw_engine_init_d2 |
| S-0203 | 0x004989b0 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_004989b0; called after FUN_00498ea0 before LAB_00499590; no args; depth-3 of rw_engine_init_d2 |
| S-0204 | 0x00498a00 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_00498a00; called with &64-byte-buf; fills buffer; used in mode-name logging inner loop; depth-3 of rw_engine_init_d2 |
| S-0205 | 0x00498c00 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_00498c00; called first in FUN_00499400; no args; depth-3 of rw_engine_init_d2 |
| S-0206 | 0x00498e40 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_00498e40; called when param_1==0 and FUN_00498950==0; return stored to DAT_00773200; depth-3 of rw_engine_init_d2 |
| S-0207 | 0x00498ea0 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_00498ea0; called after dialog path before LAB_00499590; no args; depth-3 of rw_engine_init_d2 |
| S-0208 | 0x004c2e70 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_004c2e70; called FUN_004c2e70(DAT_0077340c); depth-3 of rw_engine_init_d2; not in rw_engine_init session D |
| S-0209 | 0x004c2f30 | 0x00499400 FUN_00499400 | render | passthrough | 2026-05-02 | FUN_004c2f30; called FUN_004c2f30(DAT_00773200); depth-3 of rw_engine_init_d2; not in rw_engine_init session D |
| S-0215 | 0x004cfa00 | 0x004c7a60 thunk_FUN_004cfa00 | render | passthrough | 2026-05-02 | FUN_004cfa00; thunk target; zeros 512B at DAT_00911b00 then initializes D3D/RW format-cap table 0x911b50..0x911cd5; depth-3 |
| S-0216 | 0x004c7690 | 0x004c7a60 thunk_FUN_004cfa00 | render | passthrough | 2026-05-02 | FUN_004c7690; called by FUN_004cfa00 with (0x24,0x40c,&LAB_004cfdd0,&LAB_004cfdf0,0); depth-3 |
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
| S-0373 | 0x00493ac0 | 0x00493b50 FUN_00493b50 | util | passthrough | 2026-05-02 | LAB_00493ac0; code label (not a named function); indirect call target via PTR_FUN_006147dc (pre-NT5/non-NT code page path); returns UINT; depth-2 from FUN_004944c0 |
| S-0374 | 0x00493b40 | 0x00493b50 FUN_00493b50 | util | passthrough | 2026-05-02 | LAB_00493b40; code label (not a named function); indirect call target via PTR_FUN_006147dc (NT5+ code page path); returns UINT; depth-2 from FUN_004944c0 |
| S-0375 | 0x0049ec10 | 0x00493c00 FUN_00493c00 | util | passthrough | 2026-05-02 | FUN_0049ec10; called with (&DAT_005cfaac, 0, param_2, param_3) from FUN_00493c00 and FUN_00494ac0; depth-2 from FUN_004944c0 |
| S-0376 | 0x004a3b84 | 0x00493f00 FUN_00493f00 | util | passthrough | 2026-05-02 | FUN_004a3b84; called as vsnprintf-like formatter (buf, 0x3ff, fmt, va_args); depth-2 from FUN_004944c0 via FUN_00493f00 |
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
| S-0421 | 0x004cc230 | FUN_00409710/FUN_004235b0 | render | depth-2 | 2026-05-02 | FUN_004cc230; called with (2,1,ptr); returns handle-like int; shared by LED+AI piz loaders |
| S-0422 | 0x004cc5e0 | FUN_00409710/FUN_004235b0 | render | depth-2 | 2026-05-02 | FUN_004cc5e0; called with (handle, chunk_id, &size, flags); chunk IDs 0x13269901/0x13269902 |
| S-0423 | 0x004cbd30 | FUN_00409710/FUN_004235b0 | render | depth-2 | 2026-05-02 | FUN_004cbd30; called with (handle, dest_buf, size); reads chunk into float array |
| S-0424 | 0x004cc160 | FUN_00409710/FUN_004235b0 | render | depth-2 | 2026-05-02 | FUN_004cc160; called with (handle, 0); closes chunk |
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
| S-0449 | 0x0042d5a0 | 0x0042e3a0 FUN_0042e3a0 | frontend | passthrough | 2026-05-02 | FUN_0042d5a0; scroll/animation updater; arg=(accumulator-15000); depth-2 |
| S-0450 | 0x00472f40 | 0x0042e3a0 FUN_0042e3a0 | frontend | passthrough | 2026-05-02 | FUN_00472f40; filled rect draw (top band?); args (x,y,w,h,argb); depth-2 |
| S-0451 | 0x004730b0 | 0x0042e3a0 FUN_0042e3a0 | frontend | passthrough | 2026-05-02 | FUN_004730b0; filled rect draw (bottom band?); args (x,y,w,h,argb); depth-2 |
| S-0452 | 0x0042e590 | 0x0042e5b0 FUN_0042e5b0 | frontend | passthrough | 2026-05-02 | FUN_0042e590; animated logo draw; args (x,y,dim,dim,dim,color,frame_offset,sprite_idx,flag); depth-2 |
| S-0453 | 0x00473c20 | 0x0042e5b0 FUN_0042e5b0 | frontend | passthrough | 2026-05-02 | FUN_00473c20; fullscreen BG draw with two texture sources; depth-2 |
| S-0454 | 0x00473ee0 | 0x0042e5b0 FUN_0042e5b0 | frontend | passthrough | 2026-05-02 | FUN_00473ee0; slide-in panel draw; arg includes fVar1+base_Y_offset; depth-2 |
| S-0455 | 0x00474890 | 0x0042e5b0 FUN_0042e5b0 | frontend | passthrough | 2026-05-02 | FUN_00474890; secondary logo/sprite submit; single int arg (uVar4); depth-2 |
| S-0456 | 0x0042b8b0 | 0x00472c60 FUN_00472c60 | frontend | passthrough | 2026-05-02 | FUN_0042b8b0; returns short; used as X-axis screen dimension/scale; depth-2 |
| S-0457 | 0x0042b8c0 | 0x00472c60 FUN_00472c60 | frontend | passthrough | 2026-05-02 | FUN_0042b8c0; returns short; used as Y-axis screen dimension/scale; depth-2 |

| S-0403 | 0x00414030 FUN_00414030 | called with 0xffffffff on AI line-bank switches in FUN_00417180; return via ECX/EDX |
| S-0404 | 0x00472650 FUN_00472650 | called as FUN_00472650(0, 0x3f800000) in FUN_00417180; returns float10; likely random [0,1.0] |

| S-0405 | 0x00452160 FUN_00452160 | called as FUN_00452160() in FUN_00417640; returns float array ptr (target vehicle position?) |
| S-0406 | 0x00452ea0 FUN_00452ea0 | called as FUN_00452ea0(param_1) in FUN_00417640; per-vehicle powerup predicate |
| S-0407 | 0x00452eb0 FUN_00452eb0 | called as FUN_00452eb0() in FUN_00417640; returns float10; threshold gate |
| S-0408 | 0x0046d570 FUN_0046d570 | called as FUN_0046d570(&local_10,param_1) in FUN_00417640; float result |
| S-0409 | 0x004c3ac0 FUN_004c3ac0 | called as FUN_004c3ac0(&local_c) in FUN_00417640; in-place op on 3-float displacement |
| S-0410 | 0x00417cf0 FUN_00417cf0 | called as FUN_00417cf0(param_1,&local_24,&local_2c,param_2) in FUN_00417da0 mode-6/local_48==0 branch; replaces FUN_004148b0 in mode-8 variant |
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
| S-0600 | 0x0042a470 | 0x0042a530 FUN_0042a530 | render | passthrough | 2026-05-02 | FUN_0042a470; piz lookup; called 5x with type codes 1–5; depth-2 of FUN_0042a6b0; DEFERRED D-1720 |
| S-0601 | 0x00496400 | 0x0042a530 FUN_0042a530 | render | passthrough | 2026-05-02 | FUN_00496400; printf-style debug logger; depth-2 of FUN_0042a6b0; DEFERRED D-1721 |
| S-0602 | 0x004cf7d0 | 0x004b3d20 FUN_004b3d20 | render | passthrough | 2026-05-02 | FUN_004cf7d0; TXD stream reader called after RwStreamFindChunk(0x16); 513 bytes; depth-2 of FUN_0042a6b0; DEFERRED D-1722 |
| S-0603 | 0x0054f8d0 | 0x004b3d80 FUN_004b3d80 | render | passthrough | 2026-05-02 | FUN_0054f8d0; DFF/clump stream reader called after RwStreamFindChunk(0x23); 1153 bytes; depth-2 of FUN_0042a6b0; DEFERRED D-1723 |
| S-0540 | 0x0046d320 | 0x0040b090 FUN_0040b090 | render | passthrough | 2026-05-02 | FUN_0046d320; returns packed color/state int per (slot, index) pair; depth-2 of camera_follow |
| S-0541 | 0x0046d360 | 0x0040b090 FUN_0040b090 | render | passthrough | 2026-05-02 | FUN_0046d360; returns validity flag per (slot, index) pair; depth-2 of camera_follow |
| S-0542 | 0x0040aef0 | 0x0040b090 FUN_0040b090 | render | passthrough | 2026-05-02 | FUN_0040aef0; per-slot update called once per outer loop iteration; depth-2 of camera_follow |
| S-0543 | 0x0055dec0 | 0x0040b090 FUN_0040b090 | render | passthrough | 2026-05-02 | FUN_0055dec0; reads state from DAT_0066d728 struct; returns 0x11 or 0x12 discriminant; depth-2 of camera_follow |
| S-0544 | 0x004756e0 | 0x00426700 FUN_00426700 | render | passthrough | 2026-05-02 | FUN_004756e0; per-node callback called with two table lookups + node ptr + time-delta float + node[4]; depth-2 of camera_follow |
| S-0545 | 0x00475010 | 0x00426780 FUN_00426780 | render | passthrough | 2026-05-02 | FUN_00475010; per-entry time-delta update; called with *piVar2 (handle/ptr) and float time delta; depth-2 of camera_follow |
| S-0620 | 0x005a66d0 | 0x004623e0 FUN_004623e0 + 0x0045da60 + 0x0045dd60 + 0x004631f0 | audio | passthrough | 2026-05-02 | FUN_005a66d0; (audio_obj_ptr, 0_or_1); stop/start on audio object; D-1780 |
| S-0621 | 0x005a6dc0 | 0x004623e0 FUN_004623e0 + 0x0045dd60 + 0x004631f0 | audio | passthrough | 2026-05-02 | FUN_005a6dc0; (sub_obj_ptr, param_type, flag, value); parameter setter on audio sub-object; D-1781 |
| S-0622 | 0x0045e0f0 | 0x004623e0 FUN_004623e0 + 0x004631f0 | audio | passthrough | 2026-05-02 | FUN_0045e0f0; (channel_index, volume_float); sets volume for channel in DAT_0068f640 array; D-1782 |
| S-0623 | 0x00431b20 | 0x0045dd60 FUN_0045dd60 + 0x004631f0 | audio | passthrough | 2026-05-02 | FUN_00431b20; zero-arg, returns float10; timing/delta or audio-frame getter; D-1783 |
| S-0624 | 0x00432290 | 0x0045dd60 FUN_0045dd60 | audio | passthrough | 2026-05-02 | FUN_00432290; zero-arg, returns int; boolean condition governing music mute; D-1784 |
| S-0625 | 0x005baf00 | 0x0045dd60 FUN_0045dd60 | audio | passthrough | 2026-05-02 | FUN_005baf00; (ptr, float); called with music global DAT_0069049c and 1.0f or 0.0f; D-1785 |
| S-0626 | 0x00431b60 | 0x004631f0 FUN_004631f0 | audio | passthrough | 2026-05-02 | FUN_00431b60; zero-arg, returns float10; similar pattern to FUN_00431b20; D-1786 |
| S-0627 | 0x0042f760 | 0x004631f0 FUN_004631f0 | audio | passthrough | 2026-05-02 | FUN_0042f760; zero-arg, returns int; boolean trigger check for channel DAT_00604eb0; D-1787 |
| S-0628 | 0x0042f770 | 0x004631f0 FUN_004631f0 | audio | passthrough | 2026-05-02 | FUN_0042f770; zero-arg, returns int; boolean trigger check A for channel DAT_00604e1c; D-1788 |
| S-0629 | 0x0042f780 | 0x004631f0 FUN_004631f0 | audio | passthrough | 2026-05-02 | FUN_0042f780; zero-arg, returns int; boolean trigger check B for channel DAT_00604e1c; D-1789 |
| S-0630 | 0x00432230 | 0x004631f0 FUN_004631f0 | audio | passthrough | 2026-05-02 | FUN_00432230; zero-arg, returns int; boolean trigger check for channel DAT_00605918; D-1790 |
| S-0631 | 0x00432260 | 0x004631f0 FUN_004631f0 | audio | passthrough | 2026-05-02 | FUN_00432260; zero-arg, returns int; boolean trigger check for musicloop1 (DAT_00605d24); D-1791 |
| S-0546 | 0x004c4d20 | 0x00426810 FUN_00426810 | render | passthrough | 2026-05-02 | FUN_004c4d20; builds rotation matrix from packed source (param_1+0x1060c); candidate RW math op; depth-2 of camera_follow |
| S-0547 | 0x004c3dc0 | 0x00426810 FUN_00426810 | render | passthrough | 2026-05-02 | FUN_004c3dc0; transforms 3-float vector by matrix local_40; candidate RW vector transform; depth-2 of camera_follow |
| S-0548 | 0x004c39b0 | 0x00426810 FUN_00426810 | render | passthrough | 2026-05-02 | FUN_004c39b0; in-place op on 3-float vector {local_64,0,local_5c}; candidate RW vector normalize; depth-2 of camera_follow |
| S-0595 | 0x00534870 | 0x00472650 FUN_00472650 + 0x00472690 FUN_00472690 | render | passthrough | 2026-05-02 | FUN_00534870; RNG get-value; called as this-call from FUN_00472650 and no-arg from FUN_00472690; D-1660 |
| S-0596 | 0x00535700 | 0x00476df0 FUN_00476df0 | render | passthrough | 2026-05-02 | FUN_00535700; particle vertex buffer map/lock; (emitter_ptr, &out_ptr, channel_id, flags); D-1661 |
| S-0597 | 0x00535910 | 0x00476df0 FUN_00476df0 | render | passthrough | 2026-05-02 | FUN_00535910; particle vertex buffer unlock/draw-call; (emitter_ptr); D-1662 |
| S-0598 | 0x00538c80 | 0x0048fe70 FUN_0048fe70 | render | passthrough | 2026-05-02 | FUN_00538c80; world sector query with callback; (world_ptr, pos3, callback, out_ptr); D-1663 |
| S-0549 | 0x0042b930 | 0x004671a0 FUN_004671a0 | render | passthrough | 2026-05-02 | FUN_0042b930; 5-byte getter; returns int state; discriminant 3 routes to FUN_0042f510; depth-2 of camera_follow |
| S-0550 | 0x0042f510 | 0x004671a0 FUN_004671a0 | render | passthrough | 2026-05-02 | FUN_0042f510; 5-byte getter; alternative vehicle ptr when state==3 and param!=-1; depth-2 of camera_follow |
| S-0551 | 0x00471780 | 0x00471ec0 FUN_00471ec0 | render | passthrough | 2026-05-02 | FUN_00471780; per-entry update in outer loop; takes param_1 (cam data) and iVar8 (entry ptr); depth-2 of camera_follow |
| S-0552 | 0x00471ac0 | 0x00471ec0 FUN_00471ec0 | render | passthrough | 2026-05-02 | FUN_00471ac0; called when camera anim triggers; no args; returns; depth-2 of camera_follow |
| S-0553 | 0x0047ce80 | 0x00471ec0 FUN_00471ec0 | render | passthrough | 2026-05-02 | FUN_0047ce80; returns ID from target object uVar4; depth-2 of camera_follow |
| S-0554 | 0x0047ce00 | 0x00471ec0 FUN_00471ec0 | render | passthrough | 2026-05-02 | FUN_0047ce00; returns flags from target object uVar4 (bits 1,2,4 tested); depth-2 of camera_follow |
| S-0555 | 0x0047bb10 | 0x0047c160 FUN_0047c160 | render | passthrough | 2026-05-02 | FUN_0047bb10; per-node computation; args: node ptr, iVar1+0x30, output ptr, validity ptr; depth-2 of camera_follow |
| S-0560 | 0x004671a0 | 0x00403160 FUN_00403160 | hud | passthrough | 2026-05-02 | FUN_004671a0; camera/viewport handle getter; already in hooks.csv (render, camera_follow); depth-2 of FUN_00403160 |
| S-0561 | 0x004c1a00 | 0x00403160 FUN_00403160 | hud | passthrough | 2026-05-02 | FUN_004c1a00; camera begin-update; returns non-zero on success; depth-2 of FUN_00403160; DEFERRED D-1600 |
| S-0562 | 0x004c1c80 | 0x00403160 FUN_00403160 | hud | passthrough | 2026-05-02 | FUN_004c1c80; sets/restores viewport rectangle on camera handle; depth-2 of FUN_00403160; DEFERRED D-1600 |
| S-0563 | 0x004c19f0 | 0x00403160 FUN_00403160 | hud | passthrough | 2026-05-02 | FUN_004c19f0; camera end-update/teardown; called with camera handle arg; depth-2 of FUN_00403160; DEFERRED D-1600 |
| S-0564 | 0x00402fb0 | 0x00403160 FUN_00403160 | hud | passthrough | 2026-05-02 | FUN_00402fb0; HUD draw body for sub-mode 0xb; no visible args; depth-2 of FUN_00403160; DEFERRED D-1600 |
| S-0565 | 0x00428760 | 0x00403160 FUN_00403160 | hud | passthrough | 2026-05-02 | FUN_00428760; 2D draw with 4 float args (50.0,30.0,240.0,120.0); called only when DAT_00771964 != 0; depth-2 of FUN_00403160; DEFERRED D-1600 |
| S-0566 | 0x0041c2d0 | 0x0041a3e0 FUN_0041a3e0 + 0x0041c300 FUN_0041c300 | hud | passthrough | 2026-05-02 | FUN_0041c2d0; shared HUD draw body for game-mode 10 (DAT_0063c628 guard) and game-mode 5 (DAT_0063cdbc guard); depth-2; DEFERRED D-1600 |
| S-0567 | 0x0041b340 | 0x0041b630 FUN_0041b630 | hud | passthrough | 2026-05-02 | FUN_0041b340; per-entry HUD slot draw; called when entry+0x6c != 0; 4-entry loop base 0x0063c8d0; depth-2; DEFERRED D-1600 |
| S-0568 | 0x0041bc50 | 0x0041c0c0 FUN_0041c0c0 | hud | passthrough | 2026-05-02 | FUN_0041bc50; per-entry HUD draw; always called (no guard); 2-entry loop base 0x0063cab8; depth-2; DEFERRED D-1600 |
| S-0569 | 0x0041c9a0 | 0x0041ccc0 FUN_0041ccc0 | hud | passthrough | 2026-05-02 | FUN_0041c9a0; per-entry HUD draw; called when entry+0x110 != 0; 4-entry loop base 0x0063ce20; depth-2; DEFERRED D-1600 |
| S-0570 | 0x0041d410 | 0x0041d870 FUN_0041d870 | hud | passthrough | 2026-05-02 | FUN_0041d410; per-entry HUD draw; always called (no guard); 2-entry loop base 0x0063d298; depth-2; DEFERRED D-1600 |
| S-0571 | 0x0041de80 | 0x0041ded0 FUN_0041ded0 | hud | passthrough | 2026-05-02 | FUN_0041de80; HUD draw body for game-modes 4/7/8/9; receives param_1 (0 or 1); depth-2; DEFERRED D-1600 |
| S-0572 | 0x0041e630 | 0x0041e850 FUN_0041e850 | hud | passthrough | 2026-05-02 | FUN_0041e630; HUD draw body for sub-mode 2; no args; depth-2; DEFERRED D-1600 |
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
