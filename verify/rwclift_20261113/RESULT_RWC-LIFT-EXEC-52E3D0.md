# RESULT RWC-LIFT-EXEC-52E3D0: matched 0x0052e3d0 replaces the lifted body on the RW core path

Run 42, 2026-11-13. Branch `orch/RWC-LIFT-EXEC-52E3D0`, based on `orch/integration` 145dec4d0. Top-down row
(WORKER_RULES "Row kinds"): both builds, `ddkobj.py check`, knob boots, self-test. G-CTRL / B / 12 captures not run:
the change is ORIGTOP-ONLY (MASHED_ORIG_TOP + MASHED_RWC_CORE), and those hashes are computed with the knob off. No menu
code changed, so no menu walk. No knob added, no default changed. No librw binding added (L-124).

**Verdict: GREEN.**

- L-141: 0x0052e3d0 runs on EngineStart (RWC-PLUGIN-ATTACH RESULT section 3.2, stack
  `RwCore_EngineStart -> 0x004c2fb0 -> 0x004d8000 -> 0x0052e3d0 -> 0x0052ffd0`). Its lifted naked-asm body
  (`RwCoreLift_0052e3d0`, RwCore_Lifted_07.cpp) is gone; the port now compiles `re/match/rwta/0052e3d0.c`
  (MATCH-RWT-A, C4 by match, `re/promotions/applied/MATCH-RWT-A.tsv`) as `RwCore_0052e3d0`.
- The source still scores **100%, BYTE-IDENTICAL under rw9044 and ddk, relocheck OK (15 checked, 0 unchecked)** after the
  C++ edit (section 2).
- `ddkobj.py check`: **0x0052e3d0 `_RwCore_0052e3d0` IDENTICAL** (192 B, 4 relocs masked, 11 abs data refs exact).
  Totals IDENTICAL 1146 / 1176, DIFFERENT 11, not-in-map 19, refusals 4 (wrapper-redefines-globals). All 11 DIFFERENT
  rows were DIFFERENT in earlier baselines (section 3), so none comes from this change.
- Knob boots: every arm ends as in RWC-FIRST-FRAME-IM2D. Self-test **PASS** in every core arm. First-frame Im2D
  compare against the original **IDENTICAL, 18 of 18**, on vs_frame and ddk_frame.

## 1. What changed

| file | change |
|---|---|
| `re/match/rwta/0052e3d0.c` | raw g2s text did not compile as C++ under VS2022 (`typedef bool` / `typedef wchar_t`, `code()` arity, `undefined1 *` to `uint *`, `uint *` to `undefined4`). Rewritten in the MATCH-S11 preamble shape: globals as `MGLOBAL`, g2s typedef block cut to the names the body uses, the RwGlobals +0x108 call typed `RWMALLOCFN`, the thunk result cast to `undefined4`, `(byte)uVar2` explicit, `DAT_007dbcdc` declared `uint` (its address only). Body statements unchanged. Re-scored, section 2 |
| `mashedmod/src/mashed_re/RwCore/RwCore_0052e3d0.cpp` | new, generator output for that source (`FUN_0052e3d0` -> `RwCore_0052e3d0`, callees `RwCore_004c2dc0`, `RwCore_0052ffd0`, `thunk_FUN_004cc820` -> `RwCore_004cc9e0`) |
| `mashedmod/src/mashed_re/RwCore/RwCore_Lifted_07.cpp` | `RwCoreLift_0052e3d0` body and its extern removed |
| `mashedmod/src/mashed_re/RwCore/RwCore_0052e310.cpp` | caller binding `FUN_0052e3d0` -> `RwCore_0052e3d0` |
| `mashedmod/exe_sources.rsp` | + `RwCore\RwCore_0052e3d0.cpp` |
| `verify/rwccorerun_20261111/gen_rwcore_tus.py` | `PCT['0052e3d0'] = 100.0` (no match_ledger row yet); `THUNK_BIND` (below) |
| `verify/rwccorerun_20261111/RwCore_Bodies.tsv` | 0x0052e3d0 `lift` -> `match100 re/match/rwta/0052e3d0.c 100.0 RwCore_0052e3d0` |
| `verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`, `re/match/PROVISIONAL.tsv` | 0x0052e3d0 lifted-scaffolding row removed (the body is a 100% source now, not provisional) |

**Thunk binding.** The original calls the jmp thunk at 0x004cc9e0 from 0x0052e431 (lifted body, same address). The
generator binds a `thunk_FUN_<t>` callee to `<t>` (`rva_of`), which would have called 0x004cc820 directly.
`THUNK_BIND` maps `('re/match/rwta/0052e3d0.c', 'thunk_FUN_004cc820')` to 0x004cc9e0 (re/match/rwl01/004cc9e0.c,
match100). It is scoped to this one source: about 75 other re/match sources name `thunk_FUN_` callees and keep their
current binding until a row checks each one (follow-up proposed below).

**Generator run.** With these two edits a full `gen_rwcore_tus.py` run emits `RwCore_0052e3d0.cpp` byte-for-byte as
committed and the `FUN_0052e3d0` line in `RwCore_0052e310.cpp` as committed (checked, then reverted). The same run also
picks up about 150 sources merged since RWC-PLUGIN-ATTACH (MATCH-RWT-A / -B and others: closure 1589, match100 493,
lift 546) and rebinds many other lifted callees. That is out of scope for this row and was not committed. Only the
0x0052e3d0 swap is on the branch.

## 2. Match re-score after the C++ edit

```
0x0052e3d0 _FUN_0052e3d0: 100.0% (regs 100.0%, ops 100.0%)  53/53 ins  flags=/O2 /Ob1 /G5 tc=rw9044
   bytes: 185 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
0x0052e3d0 _FUN_0052e3d0: 100.0% (regs 100.0%, ops 100.0%)  53/53 ins  flags=/O2 /Ob1 /G5 tc=ddk
   bytes: 185 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
0x0052e3d0	OK	checked=15	unchecked=0	0052e3d0.c
```

`c4_reclass.py batch` on a one-row TSV: `SKIP already C4 (no write)`. No new promotion file: the RVA is already C4 by
match. The C4 status rests on the source text, which changed, and the re-score above holds for the new text.

## 3. Builds and ddkobj check

- Default build (`MASHED_NO_DEPLOY=1 mashedmod\build.bat`): `=== Build OK ===`, rva-lint OK (193 known, NEW=0).
  The first attempt with the raw g2s source failed with the C++ errors listed in section 1.
- Object build (`MASHED_DDKOBJ=1`): `=== Build OK ===`, link rc=0, units linked 1127 / 1394.
- `[ddkobj summary] IDENTICAL 1146/1176  DIFFERENT 11 [0x00401690 0x0042f530 0x004332a0 0x0045a3a0 0x00475a00
  0x004929d0 0x00493710 0x00493900 0x00498810 0x004c0c20 0x004c0f00]  not-in-map 19  units linked 1127/1394
  refusals 4 (wrapper-redefines-globals=4)`.
- The 11 DIFFERENT are earlier rows: 0x0042f530, 0x0045a3a0, 0x00475a00, 0x004929d0, 0x00493710, 0x00493900,
  0x004c0c20, 0x004c0f00 are DIFFERENT in `verify/rwcim2d_20261113/BYTECHECK.tsv` and
  `verify/rwccorerun_20261111/BYTECHECK.tsv`. 0x00401690, 0x004332a0 and 0x00498810 are DIFFERENT in
  `verify/ddkobj_20261112/BYTECHECK_cells.tsv`. None is 0x0052e3d0 or one of its callers or callees.
- Rows touched by this change, all IDENTICAL: `_RwCore_0052e3d0`, `_RwCore_0052e310` (the caller), `_RwCore_004cc9e0`
  (the thunk). Extract: `BYTECHECK_nonidentical.tsv` (the non-IDENTICAL rows plus these three).

## 4. Knob boots (private root D:/MashedWT/RWC-LIFT-EXEC-52E3D0_root, test-slot -> mashed_lock.py run)

`verify/rwccorerun_20261111/run_arms_attach.sh vs_off vs_core ddk_core vs_frame ddk_frame` (RWC_DATA / RWC_ROOT set to
this row's folders).

| arm | exit | expected (RWC-FIRST-FRAME-IM2D section 4) | log |
|---|---|---|---|
| vs_off | 0x00000000 after 12.8 s | knob off, unchanged | RwCore code not entered |
| vs_core | 0xFFFFFFFF (by design) | 0xFFFFFFFF | RwEngineStart on the core, probe, RwEngineStop, failure path |
| ddk_core | 0xFFFFFFFF (by design) | 0xFFFFFFFF | same on the object build |
| vs_frame | **0x5257F001** | 0x5257F001 | `RwEngineStart returns 1`, disc drawn, `RwRasterShowRaster ... first frame presented` |
| ddk_frame | **0x5257F001** | 0x5257F001 | same on the object build |

0x0052e3d0 sits on the RwEngineStart path (stack in the verdict list above). RwEngineStart reaches its end in every core arm, so
the matched body ran there in both builds. The run keeps no per-function execution log [UNCERTAIN: no direct witness
of the call in this run's logs; the stack evidence is RWC-PLUGIN-ATTACH's].

## 5. Self-test

**PASS** in every core arm: `RwTexDictionaryCreate` binding "self-test PASS (0 of 11 checks failed, 0 failing calls so
far)" in vs_core, ddk_core, vs_frame and ddk_frame. RWC-DEVICE "selftest PASS (0 fail)" in vs_frame and ddk_frame.

## 6. First-frame Im2D compare

`compare_im2d.py D:/MashedWT/RWC-FIRST-FRAME-IM2D_data/orig_im2d_1.log <arm log>` (the original-side capture of
RWC-FIRST-FRAME-IM2D, sha256 eef21540aaab95be...): **IDENTICAL, 18 fields compared** for vs_frame and for ddk_frame
(`IM2D_COMPARE_vs_frame.tsv`, `IM2D_COMPARE_ddk_frame.tsv`).

## 7. Not done, follow-ups

- hooks.csv `port` column for 0x0052e3d0: not edited on the branch (trackers through re-classify). The port fact is
  this RESULT: compiled on the knob path, matched source, object-build IDENTICAL.
- Proposed row: check the `thunk_FUN_` bindings across the other ~75 re/match sources the generator includes, and move
  each one whose original call targets the thunk into `THUNK_BIND` (or make the generator read the `// MATCH: 0x<rva>
  thunk_FUN_<t>` labels, as it already does for named labels).
- Proposed row: a full generator regen to take in the sources merged since RWC-PLUGIN-ATTACH (about 150), with its own
  build loop and boots.

## Raw data

`RAW_SHA256_RWC-LIFT-EXEC-52E3D0.txt` lists the arm logs, build logs, the full BYTECHECK.tsv and the match output under
D:/MashedWT/RWC-LIFT-EXEC-52E3D0_data with their SHA-256.
