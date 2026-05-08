# SESSION_END — boot_app_init_d2-cont1_a

**SESSION_SHORT_ID:** boot_app_init_d2-cont1_a-20260508  
**Slot used:** Mashed_pool3  
**Session date:** 2026-05-08  
**Bucket:** re/analysis/boot_app_init_d2-cont1_a/  
**PP6 redirect:** input_lua_d5 aborted (input_lua_d4-cont1 = 0 rows); redirected to boot_app_init_d2-cont1_a using same slot/ID ranges.

## Pre-flight notes

- SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E — verified OK.
- Mashed_pool3 free (no active lock); lock written.
- boot_app_init_d2-cont1 bucket: 38 rows (D-8864 + D-8880..D-8916) confirmed present.

## Drift-skip

| D-ID | RVA | Reason |
|---|---|---|
| D-8864 | 0x00427ca0 | Already in hooks.csv (hud, C1); skip. Scribe: remove D-8864 from DEFERRED as stale. |

## Functions analyzed (19 of 20 cap; 1 drift-skip)

| D-ID | RVA | Confidence | Pattern |
|---|---|---|---|
| D-8880 | 0x00471eb0 | C1 | thunk→FUN_00471df0; bulk-zero two arrays (0x69064c stride 0x8C / 0x86ed20 stride 0x2510); calls FUN_00471530 |
| D-8881 | 0x00425bc0 | C1 | Loads Copters.txd → DAT_00644150; calls FUN_004c5c80(0); zeros 0x260 bytes at 0x899260 |
| D-8882 | 0x00558240 | C1 | D3D render-target/texture create: size-doubling loop; FUN_004c77c0+FUN_004cdca0; map+fill+associate; returns handle or 0 |
| D-8883 | 0x004841d0 | C1 | Zeros 0x400 bytes at 0x6ce840; calls FUN_004840f0 84 times (groups 15+10+7+52); returns 1 |
| D-8884 | 0x00484170 | C1 | COM vtable alloc 0x18 bytes; fills 6-dword struct (param_3[0:1], param_2[0], ratio, 0, 0); returns ptr or NULL |
| D-8885 | 0x004723d0 | C1 | Writes 10×16-byte entries at 0x691500..0x69159c; pattern 0xff0Nffff / 0xff0N3cff / 0 / 0 (N=1..10) |
| D-8886 | 0x00494ef0 | C1 | thunk_FUN_00493f70; returns DAT_00771a04 (RW/DX init flag) |
| D-8887 | 0x00494f20 | C1 | thunk_FUN_00494460 (already mapped at 0x00494460 C1 intro_splash); thunk confirmed redundant |
| D-8888 | 0x00494bc0 | C1 | COM Release ×6: DAT_00771a48 (vtable[9]+[2]) + 0x3c/0x40/0x38/0x44/0x4c (vtable[2]) |
| D-8889 | 0x00496ce0 | C1 | FUN_004d41e0(DAT_00773094) + FUN_004d41e0(DAT_00773088); two RW handle releases |
| D-8890 | 0x0041a3d0 | C1 | FUN_004e6e00(DAT_0063c620); single-call game-obj finalizer |
| D-8891 | 0x0041de70 | C1 | FUN_004e6e00(DAT_0063d5e0); single-call game-obj finalizer |
| D-8892 | 0x0041c2c0 | C1 | FUN_004e6e00(DAT_0063cdb4); single-call game-obj finalizer |
| D-8893 | 0x0041da80 | C1 | FUN_004e6e00(DAT_0063d57c); single-call game-obj finalizer |
| D-8894 | 0x0041e0d0 | C1 | 6×11 entity array teardown at 0x63d610 (stride 44, inner 11): FUN_004e7e30+FUN_004e6920+FUN_004c0c20; then FUN_004e6e00(DAT_0063d7bc) |
| D-8895 | 0x0041d890 | C1 | 2-entry loop at 0x63d298 (stride 0x160): FUN_0041d6d0() per entry |
| D-8896 | 0x0041ccf0 | C1 | 4-entry loop at 0x63ce20 (stride 0x114): FUN_0041cb00(); then FUN_004c5930(DAT_0063cdd0) |
| D-8897 | 0x0041c0e0 | C1 | 2-entry loop at 0x63cab8 (stride 0x16c): FUN_0041beb0() per entry |
| D-8898 | 0x0041b660 | C1 | 4-entry loop at 0x63c8d0 (stride 0x74): FUN_0041b440(); then FUN_004c5930(DAT_0063c888) |

## Cap status

cap_count=0 (19 analyzed ≤ 20 cap). However, 18 DEFERRED rows remain in boot_app_init_d2-cont1 (D-8899..D-8916).

Scribe: add these 18 rows to a new bucket **boot_app_init_d2-cont1_b** using D=10060..10077:

| New D-ID | Old D-ID | RVA | Name | Pickup condition |
|---|---|---|---|---|
| D-10060 | D-8899 | 00489250 | FUN_00489250 | called by FUN_00402a40, no args; decompile and classify |
| D-10061 | D-8900 | 0045b930 | FUN_0045b930 | called by FUN_00402a40, no args; decompile and classify |
| D-10062 | D-8901 | 0041ffb0 | FUN_0041ffb0 | called by FUN_00402a40, no args; decompile and classify |
| D-10063 | D-8902 | 00421590 | thunk_FUN_004210f0 | thunk; called by FUN_00402a40; target FUN_004210f0 |
| D-10064 | D-8903 | 00484130 | FUN_00484130 | called by FUN_00402a40 with DAT_007f0a30; decompile and classify |
| D-10065 | D-8904 | 005581f0 | FUN_005581f0 | called by FUN_00402a40 with DAT_007f09e4; decompile and classify |
| D-10066 | D-8905 | 00467020 | FUN_00467020 | called by FUN_00402a40 with FUN_004671a0 result; decompile and classify |
| D-10067 | D-8906 | 00467070 | FUN_00467070 | called by FUN_00402a40, no args; decompile and classify |
| D-10068 | D-8907 | 004b4880 | FUN_004b4880 | called by FUN_00402a40 twice with DAT_0067f18c/0067f190; decompile and classify |
| D-10069 | D-8908 | 0042c2a0 | FUN_0042c2a0 | called by FUN_00402a40, no args; decompile and classify |
| D-10070 | D-8909 | 00427620 | FUN_00427620 | called by FUN_00402a40, no args; decompile and classify |
| D-10071 | D-8910 | 0047ba10 | thunk_FUN_00496970 | thunk; called by FUN_00402a40; target FUN_00496970 |
| D-10072 | D-8911 | 00425ed0 | FUN_00425ed0 | called by FUN_00402a40, no args; decompile and classify |
| D-10073 | D-8912 | 004b6550 | thunk_FUN_004b6700 | thunk; called by FUN_00402a40; target FUN_004b6700 |
| D-10074 | D-8913 | 00467010 | FUN_00467010 | called by FUN_00402a40, no args; decompile and classify |
| D-10075 | D-8914 | 00428400 | FUN_00428400 | called by FUN_00402a40, no args; decompile and classify |
| D-10076 | D-8915 | 004955c0 | thunk_FUN_00495580 | thunk; target callee of 0x00493560 (cleanup/teardown path) |
| D-10077 | D-8916 | 004963d0 | thunk_FUN_00496370 | thunk; target callee of 0x00493560 (cleanup/teardown path) |

## Trackers to update (scribe)

### hooks.csv — 19 new rows, all C1, subsystem=boot
RVAs: 0x00471eb0, 0x00425bc0, 0x00558240, 0x004841d0, 0x00484170, 0x004723d0, 0x00494ef0, 0x00494f20, 0x00494bc0, 0x00496ce0, 0x0041a3d0, 0x0041de70, 0x0041c2c0, 0x0041da80, 0x0041e0d0, 0x0041d890, 0x0041ccf0, 0x0041c0e0, 0x0041b660

### DEFERRED.md
- Remove D-8864 (stale drift-skip — 0x00427ca0 already mapped hud/C1)
- Remove D-8880..D-8898 (consumed)
- Add D-10060..D-10077 (boot_app_init_d2-cont1_b; 18 rows — from old D-8899..D-8916)
- Remove D-8899..D-8916 (retired by reissue)

### STUBS.md — new stubs (scribe to verify not already registered before assigning S-IDs)

**Already confirmed in hooks.csv — do NOT add:**
- FUN_004c5930 (C1, mapped)
- FUN_004c7650 (C1, mapped)
- FUN_004b3d80 (C1, render, mapped)
- FUN_004e6e00 (S-0166, existing)
- FUN_004e6920 (S-0370..S-0372 range, existing)
- FUN_004c0c20 (S-0370..S-0372 range, existing)
- FUN_00494320 (S-0815, existing from intro_splash)

**New stubs (verify then assign S-3380..S-3399 range):**

| Proposed S-ID | RVA | Called from | Description |
|---|---|---|---|
| S-3380 | 0x00471530 | thunk_FUN_00471df0 (D-8880) | no-arg init before bulk-zero |
| S-3381 | 0x004c5c80 | FUN_00425bc0 (D-8881) | post-TXD-load, single arg 0 |
| S-3382 | 0x004c77c0 | FUN_00558240 (D-8882) | D3D surface create (w, h, 0, 4) |
| S-3383 | 0x004cdca0 | FUN_00558240 (D-8882) | D3D texture create (w, h, 8) |
| S-3384 | 0x004cdd60 | FUN_00558240 (D-8882) | map/lock resource |
| S-3385 | 0x004cdd00 | FUN_00558240 (D-8882) | unmap resource |
| S-3386 | 0x004d5310 | FUN_00558240 (D-8882) | associate primary+secondary |
| S-3387 | 0x00558180 | FUN_00558240 (D-8882) | intermediate builder (local_1c, &param_2) |
| S-3388 | 0x00558400 | FUN_00558240 (D-8882) | render-target writer in loop |
| S-3389 | 0x004d7ff0 | FUN_00558240 (D-8882) | error code formatter |
| S-3390 | 0x004d8480 | FUN_00558240 (D-8882) | error dispatcher |
| S-3391 | 0x004840f0 | FUN_004841d0 (D-8883) | no-arg; called 84 times |
| S-3392 | 0x004d41e0 | FUN_00496ce0 (D-8889) | RW handle free/destroy |
| S-3393 | 0x004e7e30 | FUN_0041e0d0 (D-8894) | detach/remove entity (handle, 0) |
| S-3394 | 0x0041d6d0 | FUN_0041d890 (D-8895) | no-arg per-entry finalizer |
| S-3395 | 0x0041cb00 | FUN_0041ccf0 (D-8896) | no-arg per-entry finalizer |
| S-3396 | 0x0041beb0 | FUN_0041c0e0 (D-8897) | no-arg per-entry finalizer |
| S-3397 | 0x0041b440 | FUN_0041b660 (D-8898) | no-arg per-entry finalizer |

(S-3398..S-3399 remain unallocated in this session.)

### UNCERTAINTIES.md — new rows (U-3387..U-3395)

| U-ID | RVA | Note |
|---|---|---|
| U-3387 | 0x00471eb0 | 0x0069064c array — subsystem owner unknown (stride 0x8C, 8 entries) |
| U-3388 | 0x00471eb0 | 0x0086ed20 array — subsystem owner unknown (stride 0x2510, 8 entries) |
| U-3389 | 0x00425bc0 | 0x00899260 array — purpose unknown (0x260 bytes = 608 bytes) |
| U-3390 | 0x00558240 | size-doubling loop convergence formula meaning unknown |
| U-3391 | 0x004841d0 | significance of call-count groups (15, 10, 7, 26) to FUN_004840f0 |
| U-3392 | 0x004723d0 | table at 0x691500 — byte field semantics (0xff/N/0xff or 0xff/N/0x3c pattern) |
| U-3393 | 0x00494bc0 | identity of 6 D3D interface pointers at 0x771a38..0x771a4c |
| U-3394 | 0x00496ce0 | identity of RW handles at 0x00773094 and 0x00773088 |
| U-3395 | 0x0041ccf0 | identity of objects freed via FUN_004c5930 at 0x63cdd0 and 0x63c888 |

(U-3396..U-3406 unallocated in this session.)
