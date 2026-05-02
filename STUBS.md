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
