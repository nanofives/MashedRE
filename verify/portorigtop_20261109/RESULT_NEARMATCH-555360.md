# RESULT NEARMATCH-555360 (run 38 restack, worker on claude3 then PFB)

Verdict: GREEN (top-down row: both builds, `ddkobj.py check`, four knob boots on my head, two on HUDINIT-B). No G-CTRL, no B, no 12 captures (WORKER_RULES, top-down row).

## What the row is

`0x00555360` (Rt2d open's font-system init) runs on its unedited source `re/match/s24/00555360.c` (94.4%, L-115 / L-117 = (a)) as a near-match body under `MASHED_ORIG_TOP=1` with `MASHED_ORIG_TOP_RWMOD=1`, replacing the logging stand-in from HUDINIT-A. Port axis only, never C4. Files: `Match/PortTop_00555360.cpp`, one forwarder switch in `Match/PortTop_00552b60.cpp`, `Match/PortTop_StandInsHIA.cpp`, `exe_sources.rsp`, BINDINGS rows via `update_bindings_nearmatch_555360.py`.

## Restack

- Old branch kept as `orch/NEARMATCH-555360-old` (4503d7205). New `orch/NEARMATCH-555360` = `orch/HUDINIT-B` a95ec2bd6 + the code commit cherry-picked (clean, auto-merged `exe_sources.rsp` and `BINDINGS.tsv`).
- a95ec2bd6 is on top of `orch/integration` bf44aae7d, which already holds RWC-BIND-4C5890 (0bdbb9f53, c3534b7c2) and HUDINIT-A. Not on top of the later integration head (6a39074cc), which is left to the merge train.
- RWC-BIND-4C5890 follow-up in this row: the old code commit still bound `0x004c5890` to `MashedRw_TexDictionaryCreate_004c5890` (librw `rw::TexDictionary::create`). L-129 (a) dropped that librw binding and L-124 adds none. `PortTop_00555360.cpp` now defines `FUN_004c5890` as `RwCoreBind_TexDictionaryCreate_004c5890` (RwCore/RwCore_Bind_004c5890.cpp, the entry RWC-BIND-4C5890 names for this caller). The BINDINGS row for 0x004c5890 is rewritten to match (`update_bindings_nearmatch_555360.py`, idempotent, 551 rows). No librw binding added.

## 0x004c5890 returns 0 with MASHED_RWC_CORE off: do the readers handle it? Yes

With `MASHED_RWC_CORE` off the binding returns 0 and logs `RwTexDictionaryCreate 0x004c5890: MASHED_RWC_CORE off, no RW core: returns 0` (seen in both RWMOD-on boot logs, lines `[RWC_CORE]`).

- In the near-match body the result is stored to `DAT_00912a1c` and `DAT_00912a20` and tested nowhere (`re/match/s24/00555360.c`, last block). The function still returns 1. The boot logs show `0x00555360=1`, so the 0 changes no return value.
- Readers of `DAT_00912a1c` in the sources: `re/match/s24/00555280.c` lines 60-63 test `!= 0` before `FUN_004c5930` and then store 0. That function is not reached in the knob boots (no `00555280` line in any log).
- Reader of `DAT_00912a20`: the getter `0x00556cd0` (Frontend/BatchAA_s6.cpp) returns it verbatim. Its path-relevant caller `0x00427620` (`re/match/s06/00427620.c` lines 50-53) guards `if (iVar1 != 0) FUN_004c5930(iVar1)`. Also `0x00427ca0` overwrites `DAT_00912a20` through the setter `0x00556cc0` right after the Rt2d open (log: `dictionary 0x00912a20=` non-zero after `rva 0x00427ca0 ran`).
- No other reader of either global exists in `mashedmod/src` or `re/match`.

## Self-test, as L0 asked: BEFORE my work on orch/HUDINIT-B a95ec2bd6, then on my head

"Self-test" here is the knob boot's own checks (WORKER_RULES, top-down row: the knob boot and its self-test). There is no separate self-test command. The checks are the seed `rdata seed: 39/39 strings written`, `dinput seed: 3/3 written`, the bring-up return lines and `WinMain 0x00492370 returned 0 after 3000 frames` with rc 0.

Command (from the worktree, wrapped `test-slot` outside, `mashed_lock.py run` inside, root `D:/MashedWT/HUDINIT-A_root`):
`py.exe -3.12 verify/porttop_20261108/knob_boot.py <label> D:/MashedWT/HUDINIT-A_root 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000 [MASHED_ORIG_TOP_RWMOD=1]`
Scripts: `D:/MashedWT/NEARMATCH-555360_data/run_hb.ps1` and `run_mine.ps1`.

| head | exe | RWMOD | result | log |
|---|---|---|---|---|
| a95ec2bd6 (HUDINIT-B) | `mashed_re_vs.exe` of HUDINIT-B_data2 (sha below, equals the worker's recorded build) | off | PASS: rc 0 after 27.8 s, seed 39/39, 3000 frames, `RWMOD off: stand-in only` | `D:/MashedWT/NEARMATCH-555360_data/hb_vs_rwmod_off.log` |
| a95ec2bd6 (HUDINIT-B) | same | on | PASS: rc 0, seed 39/39, 3000 frames, `RWMOD on: matched body (HUDINIT-B)`, brushes non-zero, `0x00556e40 matched body`, `0x00555360=0` (HUDINIT-A stand-in) | `.../hb_vs_rwmod_on.log` |
| fdffcb7d3+ (this row) | VS2022 exe | off | PASS: rc 0 after 40.3 s, seed 39/39, 3000 frames | `.../nm_vs_rwmod_off.log` |
| this row | VS2022 exe | on | PASS: rc 0 after 30.3 s, seed 39/39, 3000 frames, `0x00555360=1`, brushes non-zero | `.../nm_vs_rwmod_on.log` |
| this row | object exe | off | PASS: rc 0 after 34.6 s | `.../nm_ddk_rwmod_off.log` |
| this row | object exe | on | PASS: rc 0 after 32.9 s, `0x00555360=1` | `.../nm_ddk_rwmod_on.log` |

HUDINIT-B a95ec2bd6 does not fail. a95ec2bd6 only touches `RESULT_HUDINIT-B.md` over 0f9fe3032, so its code is what the HUDINIT-B_data2 exes were built from (their sha256 matches the RESULT_HUDINIT-B.md table: `mashed_re_vs.exe` FC2E8D32..., I booted that file, not a rebuild).

## Builds and object check

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`, VS2022) | `=== Build OK ===`, rva-lint OK (95 known, NEW=0), asm-lint OK |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===` |
| `ddkobj.py check` | IDENTICAL 712, DIFFERENT 81, not-in-map 3. HUDINIT-B had DIFFERENT 80. The one added row is `0x00555360` (`_Orig_00555360`, 94.4% source, not a 100% source, so DIFFERENT is expected). The other 80 DIFFERENT rows are the same set (diffed by rva and result against HUDINIT-B_data2/BYTECHECK.tsv) |

Open item, stated not worked around: `ddkobj` reports `diff_bytes` 79 for 0x00555360 (first diff +0x2d, 368 bytes, 29 exact data refs), while RESULT_L115-HUDINIT-PROOFS.md accounts for 55 differing bytes (50 by return-block layout and its shift, 5 by store order). The two tools count differently (relocation-masked compare against the object versus the byte table). L-117 admits the function by name on the 55-byte proof. Someone should reconcile the 79 against the 55 before any promotion beyond the port axis.

## Not run

G-CTRL, B, 12 captures x 4, port column on this build (top-down row). The merge train runs the default gates once per batch.

## Raw files (D:/MashedWT/NEARMATCH-555360_data, not committed), SHA-256

| file | sha256 |
|---|---|
| nm_ddk_rwmod_off.log | B1DA1247DA4BF7DB120058FF4DBDAC143CEA69C97F4FB95ACA841E58CD041CAD |
| nm_ddk_rwmod_on.log | B82F8C15B82EE0B052D009B7FFDE04789DEAF5C936D71FC124AF06113AB4E585 |
| nm_vs_rwmod_off.log | D03762AE9F9B47F808E3A096D14F0141B3BB7DA5EA178BFEE3F207C932D322B5 |
| nm_vs_rwmod_on.log | C709547DE403D665142820CB4372D32733F4543BE877F4923793C3D88296E0C7 |
| hb_vs_rwmod_off.log | 4A88621AD7D87FEF04759356FD3D9FC8E5BEF785E1B20F1B74838D1D5AA39BA1 |
| hb_vs_rwmod_on.log | A81340D87B56CE9642E48647DB95D3B3274D528A6FBA1CC96201ED4EF7293F63 |
| mashed_re_ddk.exe | 11D8A854224D7AA007AD36320446831DA7DC2A5B86F409B13308F41CBF975B1C |
| mashed_re_vs.exe | 4C13B5ABC2C2C4BA9BC5356405CCA0E0F9EB69B10D6C5574B654199CCED762AB |
| build.log | 4756F1D5B44387803A093DFFA749C5E168990859B63DE03F258EA814C2CAAFBB |
| build_ddkobj.log | 7DA69F1D343E1FF4DBF22619FF14E655E69FD77714AF67119E248097F14BA437 |
| ddkcheck.log | 166FD68411F63E65FE9A52767A8F56D5CF96EA2F9B1D4FCD1872C04E301669AF |
| BYTECHECK.tsv | 88B30124F7C02DBA1290C8E30470C4CD477567497BB99BC4D6FCC8036EA52464 |
