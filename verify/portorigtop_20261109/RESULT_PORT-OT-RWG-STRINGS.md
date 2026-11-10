# RESULT PORT-OT-RWG-STRINGS: RwGlobals string slots on MASHED_ORIG_TOP_RWMOD, 4 more rows released

Branch `orch/PORT-OT-RWG-STRINGS`, stacked on orch/PORT-OT-RWG-MODULES `07a9a01c3`. PREREG `b0a870d3a` (committed
alone), code `6b2e3919e`. Worker cmwa9vtqk1tftle1czfhnvys9.

## Verdict: GREEN

- With `MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1`, RwGlobals rows: **OK 65, needs binding 7**. PORT-OT-RWG-MODULES had
  61 / 11. This matches the PREREG prediction.
- Released (`RWG_STRINGS_RELEASED.tsv`): 0x004113b0 (+0xc4), 0x00427840 (+0xf4), 0x00555910 (+0xe8 +0xf4),
  0x005adf60 (+0xe8).
- No row went from OK to needs binding.
- 0x004ce790 and 0x004ce8e0 now have every call slot non-null. Each stays at needs binding on one data field only:
  image module +0x21c, the format list, is 0 in the exe and a heap list in the original.
- Default path unchanged: G-CTRL `8284be46 / 67931834 / 31fe043e`, B `1d5bf4f9` x3.

## The change (`LibRw/RwApiBind.cpp`, `RegisterRwModules`, same knob)

MASHED.exe values are from the original-side probe of PORT-OT-RWG-MODULES: RwGlobals block at 33.02 s,
`D:/MashedWT/PORT-OT-RWG-MODULES_data/orig/orig_probe.json`, SHA-256 `ae6d8a62...f2c2`.

| slot | original | evidence for the original | binding |
|---|---|---|---|
| +0xc4 | 0x004a2b60 | hooks.csv: libid identical to libcmt `_sprintf` | `RwGlobalsSprintf` (vsprintf). The one call is `re/match/s02/004113b0.c:64` `"%sAll%d.rep"` |
| +0xd0 | 0x004a34b0 | hooks.csv `_strncpy` (CRT, C2) | `strncpy` |
| +0xdc | 0x004d8750 | hooks.csv RwStrrchr C4 | exe body `RwStrrchr` (Render/RwStrSearch.cpp), `int` argument narrowed to `char` as that body takes it |
| +0xe8 | 0x004af340 | disassembly, below | `RwGlobalsStrcmp` |
| +0xf4 | 0x004a9410 | hooks.csv `_strlen` (CRT, C2) | `strlen` |
| +0xf8 | 0x004d86d0 | hooks.csv RwStrupr C4 | exe body `RwStrupr` (Render/RwStrCase.cpp), wrapper returns s |
| +0xfc | 0x004d8700 | hooks.csv RwStrlwr C4 | exe body `RwStrlwr`, wrapper returns s |

**0x004af340** (capstone over `MASHED.exe.unpatched`):
- 0x004af340..0x004af37c: loads the two arguments, then compares 4 bytes per pass, `cmp al,[ecx]` / `cmp ah,[ecx+1]` /
  ... with a NUL test after each byte.
- 0x004af380: a common NUL returns 0.
- 0x004af384..0x004af38b: on a difference it runs `sbb eax,eax`, `shl eax,1`, `add eax,1`. That gives -1 when the
  first string's byte is below the second's (unsigned compare) and +1 when it is above.
- `RwGlobalsStrcmp` returns exactly these three values, comparing unsigned bytes.
- [UNCERTAIN] The unaligned entry path at 0x004af38c (taken when `edx & 3`) was not read. Evidence missing: its
  disassembly. Its result for aligned and unaligned strings is taken to be the same function. No row depends on the
  alignment of its arguments.

**RwStrupr / RwStrlwr return s.** 0x004d86d0 loads eax from `[esp+4]` at entry and never writes it again
(0x004d86d0..0x004d86f7). The exe bodies return void, so the wrappers return s.

**Slots left at 0**, with no ORIG_TOP row calling them (`ot_measure.py lists`). Original values:

| slot | original |
|---|---|
| +0xc8 | 0x004a42c5 |
| +0xd8 | 0x005c2300 |
| +0xe0 | 0x004d8730 (RwStrchr, exe body exists) |
| +0xe4 | 0x004a3860 |
| +0xec | 0x004a3a90 |
| +0xf0 | 0x004d8680 |
| +0x100 | 0x004a3ac9 |
| +0x104 | 0x005c22bd |

**Not done: the format registrations of 0x00493600** (`re/match/s15/00493600.c`).
- It calls 0x004ce790 twice: `(&PTR_DAT_005cf74c, 0x004935a0, 0x0052d530)` and `(&PTR_DAT_005cf748, 0x004935d0,
  0x00513f90)`.
- None of those four functions has an exe body (`Match/PortTop_00493710.cpp:50`).
- Registering the two formats with null functions would make +0x21c non-null and release 0x004ce790 / 0x004ce8e0 in
  the verdict. But 0x004ce8e0 calls the format's function at +0x2c, so the list would exist without the behaviour
  behind it. The release needs those four functions ported, or bound to the exe's image decode.

## Arms (PREREG)

| arm | result |
|---|---|
| A1 knob boot | PASS. rc 0, 7 samples: the exe finished 3000 frames before the 30 s mark. The exe log has `... string slots +0xc4 +0xd0 +0xdc +0xe8 +0xf4 +0xf8 +0xfc, record 00FFA360 size 0x440`. |
| A2 measure | PASS. OK 65 / needs binding 7. Seed columns unchanged (111 / 13). |
| A0 gates | PASS, exe sha8 `24317ed3`. H_DEF step `8284be46`, gates `67931834`, ptrace `31fe043e`. B_DEF_K0 / K1 / K26 `1d5bf4f9`. |
| A3 captures | PASS. OFF@0 ON@0 OFF@0 ON@0 with RWMOD=1: rc 0 x4, freeze `pinned pos=55 hash=28538c74`. All 12 hashes are equal across the four runs and equal to the PORT-OT-RWG-MODULES runs (walk_04 `4d53d93f87f0332d`, the BAR_Y default). |
| A4 lint | PASS. `[rva-lint] OK -- no NEW duplicate bodies (2 known)`. |

## Remaining 7 needs-binding rows

| row | blocker | original |
|---|---|---|
| 0x004ce790, 0x004ce8e0 | image +0x21c empty | heap list, filled through 0x00493600 |
| 0x00413a40 | call slots +0x24 +0x34 | `0x004d6910` D3d9State_Get (C3), `0x004dbc90` (no hooks row) |
| 0x004430e0 | +0x34 | |
| 0x004d52d0 | +0x60 | `0x004de1b0` |
| 0x0047ade0 | +0x0 | the original holds 0 here too |
| 0x004c1680 | data +0xbc | heap `0x02b49a64` |

The B15 bridge already binds +0x20 / +0x30, so device slots +0x24 / +0x34 are the next device-slot step.

## Evidence (raw data in `D:/MashedWT/PORT-OT-RWG-STRINGS_data`, not committed)

| file | SHA-256 |
|---|---|
| probe.json (exe, ORIG_TOP + RWMOD, rc 0) | 86923b064b7e9c022e7d37c0755982c26cc50fb4b84d71ba0dd97fb19a09fe20 |
| probe_mashed_re.log (registration line) | e2c6aa71666e28234b7ddca5c90cedfa87ae2680528970e7f9ae80e2884665f0 |
| OT_MEASURE_before.tsv (the MODULES table) | 7f7f9522ab6afa3b777b745e138637651e43728d19ca4f3591341ba065d57084 |
| arms_run.log (A1, A0, A3) | bc33277a6ab825d089796d3a8ec5ca3594e95074901409ba4693649ef0c0898b |
| cap/runs.tsv (A3) | 77a00af53e96b4bbd2d0962fed1a92103bbec4766f47401102ff93900b496da0 |
| mashed_re.exe (built from 6b2e3919e) | 24317ed368509c03e0a0774e041cdab04d262b6282f6e7a70cd96810f14cf3f9 |
| ../PORT-OT-RWG-MODULES_data/orig/orig_probe.json (original values, --orig) | ae6d8a62f6ec97dc8f530f15796a8d4649864ab07cf7fa3ad6275a968b1cf2c2 |

## REPORT

```
REPORT PORT-OT-RWG-STRINGS
verdict: GREEN
branch: orch/PORT-OT-RWG-STRINGS @ <RESULT commit>
evidence: verify/portorigtop_20261109/RESULT_PORT-OT-RWG-STRINGS.md
gates: G-CTRL 8284be46/67931834/31fe043e  B 1d5bf4f9/1d5bf4f9/1d5bf4f9
decision: none
next: device slots +0x24/+0x34/+0x60 (0x00413a40, 0x004430e0, 0x004d52d0); 0x004ce790/0x004ce8e0 need 0x00493600's four image functions ported first
```
