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
