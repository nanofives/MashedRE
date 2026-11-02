# RESULT D4-WSB-55B080-ULP: GREEN, 0x00546b10 port compiled x87, 0x0055b080 CLEAN 24/24 with the port bound, SSE2 control DIVERGENT 8/24

PREREG `PREREG_D4-WSB-55B080-ULP.md` committed alone at `a8f035b49`, before every arm. Amendment 1 at `f6c5c841b` (with
its one-file harness fix), after S1 / S2 / S3 and before S1a / S2a / Ca / G. Branch `orch/D4-WSB-55B080-ULP` off
`orch/integration` @ `6d9d01a7f`. Fix commit `e56e0cb3d`. `orch/integration` merged back in at `ab71bb207` (no conflict). Second re-merge at `13ae4bac3` (integration `066dc991a`, no conflict, no build input changed: empty diff under `mashedmod/` and `re/tools/dual_copy_allowlist.txt`).

## Cause

`0x00546b10` (and its X / Z siblings `0x00546bf0` / `0x00546cb0`) lived in `Collision/RwpSolverMath2.cpp`, compiled
`/arch:SSE2`. Their `float10` temporaries (= double on MSVC) stayed 53-bit. The original keeps the same chain on the x87
stack (capstone over `MASHED.exe.unpatched`: trace `0x00546b23..0x00546b2d`, `fcom [0x5d757c]` `0x00546b30`, `fadd 1.0`
`0x00546b3f`, `fstp [esp]` `0x00546b46`, `fmul st(1)` / `fstp [edi+0xc]`, `fdivr [0x5cc32c]` `0x00546b5e`, three
`fld; fsub; fmul st(1); fstp` rows to `0x00546b81`), and the race thread runs CW `0x007f` (PC_24), so each op rounds to
24 bits. `0.5/s` and the `m[i]-m[j]` terms are where the two diverge. Same mechanism as U-9220 (`0x00546c50`).

## Fix (`e56e0cb3d`)

- The three bodies move unchanged to `Collision/RwpSolverMath2Quat.cpp`, listed in `mashedmod/x87_tus.txt` and in both
  source lists. The other four K2 bodies stay SSE2.
- `Lane2/L2_0055b080.cpp` binds the `0x00546b10` port again (the PORT-BROAD-2 amendment-1 raw thunk is removed).
- Codegen: the three `_impl` bodies carry 41 / 24 / 24 x87 ops and 0 SSE ops on both targets. The `0x00546b10` body
  follows the original's op tree (same sum order, same `fdivr`, same three rows). Its one extra `fstp/fld [esp+0x10]`
  before the FastSqrt push is a float32 store and reload of a value already rounded to float32, so it changes nothing.
- Classification: faithful correction (U-9220 precedent), no knob. The standalone runs PC_53, where x87 and SSE2
  compute the same double chain. Gate G confirms this (below).

## Amendment 1: two harness defects (S1 and S2 void)

1. S1 ran without `MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1` (the PORT-BROAD-2 env), so no heap page was tracked:
   `pages=o:0,n:0` on all 24 samples (pass 1 had 9, pass 2 had 7). The quaternion table is heap, so S1 compared nothing.
2. The generated `0x00546b10` shadow wrapper was `ShadowAB::Run`, which compares only the return (`Core/ShadowAB.h:38`),
   and the return is 1 on every non-null pair (`mov ebx,1` `0x00546b28`, `mov eax,ebx` `0x00546b59` / `0x00546bba` /
   `0x00546be0`). So S2, and the row's 2026-09-10 C3 evidence (48/48), never compared the quaternion. That is how the
   1-ulp difference reached PORT-BROAD-2 unseen. Fix: the wrapper uses `RunRegion` over the 0x10 bytes at `param_1`
   (inert unless `MASHED_SHADOW_AB` is armed).

## Arms

Census scenario: `scenario_launch.py --track 0 --mode 10 --cars 4 --hold 60 --hooks 0x<rva>`, hook installed alone,
private root `D:/MashedWT/D4-WSB-55B080-ULP_root`, `SAMPLES_DONE=24`. Verdicts from `re/tools/shadow_ab_report.py`
(`shadow_summary.tsv`).

| arm | `.asi` | hook | env | verdict |
|---|---|---|---|---|
| S1 | fix `64be2a52` | `0x0055b080` | no PRIVATE | CLEAN 24/24, **void** (0 pages tracked) |
| S2 | fix `64be2a52` | `0x00546b10` | no PRIVATE | CLEAN 48/48, **void** (`Run`, return only) |
| S3 | fix `64be2a52` | `0x00546bf0` | no PRIVATE | NO_LOG: full 60 s race ran, no call (branch not reached) |
| S4 | fix `64be2a52` | `0x00546cb0` | no PRIVATE | NO_LOG: full 60 s race ran, no call (branch not reached) |
| **S1a** | fix `7ab16153` | `0x0055b080` | PRIVATE + STW | **CLEAN 24/24**, ndiff 0, A/B-IS-REAL `e9/83`, pages 7 (x11) / 8 (x13), diff 0 |
| S2a | fix `7ab16153` | `0x00546b10` (RunRegion) | PRIVATE + STW | CLEAN 48/48, `fields=0/4` x48, A/B-IS-REAL `e9/53` |
| Ca1 | SSE2 control `c854fa54` | `0x00546b10` (RunRegion) | PRIVATE + STW | CLEAN 48/48, `fields=0/4` x48 |
| **Ca2** | SSE2 control `c854fa54` | `0x0055b080` | PRIVATE + STW | **DIVERGENT 8/24** (samples 16-23), 1 dword each, 1 ulp |

Ca2 is the pass-1 signature: one dword per sample on page `0x0f4f4000`, at `+0xe34 / +0xe38 / +0xe48 / +0xe58 / +0xe68 /
+0xe78 / +0xe88 / +0xe68`, 16-byte stride, e.g. `bf340d2e` vs `bf340d2d`, `baf6ceec` vs `baf6ceeb`, `bad815f2` vs
`bad815f3`. Pass 1 had 4/24 (samples 16-19) on the same layout. S1a and Ca2 differ only in the `RwpSolverMath2Quat`
line of `x87_tus.txt` (same source, same commit), so the x87 codegen removes the difference.

**Nested `0x00546b10` samples at `0x0055b080`'s call site.** `L2_0055b080` calls the port wrapper directly, so the
`0x00546b10` counter samples there too (`PATCHBYTE installed=53 uninstalled=53`, the hook-not-installed shape of
`Core/ShadowAB.h:44-48`: the code at the RVA is the untouched original, so the compare is real). S1a: 4/4 `fields=0/4`.
Ca2: 8/8 DIVERGENT, 5 at `@+08`, 3 at `@+00,+08`.

**Ca1 is CLEAN**, so S2a alone does not separate the two builds. The first 48 calls in race phase (the counter samples
the first `kDefaultSamples` = 48, `Core/ShadowAB.h:132`) do not reach an input where PC_24 and PC_53 round differently.
The verdict rests on S1a against Ca2 and on the nested samples, as PREREG amendment 1's read-out says.

## Read-out against the PREREG

GREEN: S1a CLEAN 24/24 with the quaternion pages tracked, the paired control Ca2 DIVERGENT 8/24 with the pass-1
signature, nested `0x00546b10` samples 4/4 equal vs 8/8 differing, gates G unchanged. S2a CLEAN but uninformative (Ca1
CLEAN). S3 / S4 not exercised. No other differing dword in any arm.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + solo machine lock,
`A4_ROOT=D:/MashedWT/D4-WSB-55B080-ULP_root`.

| arm | file | ref (`merge_train.json`) | G, fix exe `871df233` (pre-merge, `f6c5c841b`) | G2, merged exe `6edd4754` (`ab71bb207`) |
|---|---|---|---|---|
| H_DEF | step.csv | `8284be46` | `8284be46` | `8284be46` |
| H_DEF | gates.csv | `ba407a16` | `ba407a16` | `ba407a16` |
| H_DEF | ptrace.log | `31fe043e` | `31fe043e` | `31fe043e` |
| B_DEF_K0 | step.csv | `1d5bf4f9` | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K1 | step.csv | `1d5bf4f9` | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K26 | step.csv | `1d5bf4f9` | `1d5bf4f9` | `1d5bf4f9` |

The first gate launch failed before any arm ran (`test-slot` is a bash script and `mashed_lock.py run` starts its
command with CreateProcess, WinError 2). Re-run through `proyectos_test_slot.py`.

## Run log

- Audio endpoints (VB-CABLE x3 + Philips FTV) present before and after every shadow launch, 0 MASHED Event-1000 events,
  no fault at `0x005baf48`, no U-9677 signature, no re-run for a crash.
- The driver yielded to a PRIORITY-PENDING merge train twice (S1 queue, S3 queue), as the L0 rule says. No lock broken,
  only spawned PIDs killed.

## Tracker transaction (re-classify, no level change)

- `hooks.csv`: `0x00546b10` C3 kept, `frida_diff` -> this `shadow_summary.tsv`, `exe_file` ->
  `Collision/RwpSolverMath2Quat.cpp`, dated note (the old 48/48 was return-only). `0x00546bf0` / `0x00546cb0` C2 kept,
  `exe_file` moved, dated note (not exercised). `0x0055b080` C3 kept, `frida_diff` -> this summary, dated note (port bound
  again, S1a / Ca2).
- `re/analysis/CHANGELOG.md`: four lines below `<!-- ENTRIES -->`.
- No C4: no race diff with the hook installed (`re/CONFIDENCE.md` write-set leg).
- Not touched, for a follow-up: `STUBS.md` S-3714 / S-3715 / S-3716 list the three siblings as passthrough stubs of
  `0x00546b10`, but the port has called the ported siblings since B5e-K2 (2026-07-16). `UNCERTAINTIES.md` U-7561
  (subsystem boundary of `0x00546b10`) is unrelated to this fix.

## Files

Committed: PREREG, this RESULT, `shadow_summary.tsv`, `run_shadow10.py` (copy of
`verify/d4wsb_portbroad2_20261101/run_shadow9.py`, data path, label and summary changed), `drive_ulp.py` (copy of
`drive_pb2.py`, runner, log path and label changed, plus the merge-train yield rule from `verify/hbasieax_20261101/drive_hbe.py`).

Raw (not committed), `D:/MashedWT/D4-WSB-55B080-ULP_data/`:

| file | SHA-256 |
|---|---|
| `0055b080/shadow_ab.log` (S1) | `49b148f888e4dfa1dde74d7a19ce9b5af3759efcc57aff8cf983e381ee18284c` |
| `00546b10/shadow_ab.log` (S2) | `c52280bd0779b0ce932304b9e45c9694db6d21e3cac71fc7c92b79695856f7b5` |
| `0055b080_a1/shadow_ab.log` (S1a) | `c52701999019901476f960cebfafb39575ed152e7725765a03918a80996aa191` |
| `00546b10_a1/shadow_ab.log` (S2a) | `b3d947a36183af836f0c019861cbf3dd5eddfb433a6b109c04fb4af51a10c106` |
| `00546b10_ctl/shadow_ab.log` (Ca1) | `b3d947a36183af836f0c019861cbf3dd5eddfb433a6b109c04fb4af51a10c106` (same bytes as S2a: lines carry only counts) |
| `0055b080_ctl/shadow_ab.log` (Ca2) | `21015d759fe535656390a65edf1573bf88fc0084e7a86904ff8384b8d1141623` |
| `drive_ulp.log` | `82d0d471d5abe7a0a5471eeb87315d058b653c18c489444e61c0603c9a154732` |
| `seq_a1.log` | `e072caf5dd7b900a0370d1fa5d0ce67e9cd50a3edb7afebd015344f9674d4959` |
| `build1.log` / `build2.log` (fix `e56e0cb3d`) | `fc5e8ad94f83f1f837b491dda2223d362786a21778354b37ae2b468fe6e2168e` / `070f535ea8f93623ffc0fcee3dff08052137a43e6b5363af3fa986482436684b` |
| `build_control.log` / `build_control_a1.log` | `b529c76af3b8e761ce72c8c395592d9e8fdea7b6b85d6e475ea83c7a0f0809e3` / `ed8dd3ea415a067c95a108ce3d2adc0f48ae6c9c6a0aea95d28e4514e7992bac` |
| `build_fix_a1.log` | `21e43f9f2aab60dbe1dafe804e68f179eead90f84784f74dff3d1a6aae1053c3` |
| `quat_asi.disasm.txt` / `quat_exe.disasm.txt` / `control/quat_asi_control.disasm.txt` | `3f54aa6639652ba359716a9ea5f56cb6dc89f199c9bb742cb62e0842601ccacb` / `d2ae320549c300c65310b6293da3a7938cb4c97797006282f755fd4ad0636dde` / `334d179853b7f1d050da2258d0aa7c9c60b2d8431f3f82192027c71c9e18aca4` |
| `fix_a1/mashed_re_dev.asi` / `fix_a1/mashed_re.exe` | `7ab161535c7293927d2d12893bef9ea4f8e6fffa2cd24297a097a72954f920d2` / `871df233929a887845c035cec95a494c572cd27a0cb6dde8a1d0e7e9151562e2` |
| `control_a1/mashed_re_dev.asi` | `c854fa5471e508595caee015a4a8752de06b2d9e8f44cb63b7d0494341cfe4a0` |
| `gates/RUNLOG.txt` (G) | `aff58392b6122b6160dc735ddb2b889fd75447a389a68d4d5faf176369b563f0` |
| `gates/H_DEF.step.csv` / `.gates.csv` / `.ptrace.log` (G) | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` / `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` / `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| `gates/B_DEF_K{0,1,26}.step.csv` (G) | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `merged/mashed_re.exe` / `build_merged.log` | `6edd4754d09979f58617cadcdf4c0a77cc5531f141067183dd2a75fd0c5fcb00` / `467c922ef3f1929f1122cb067c44c2a15e82c9813af0e0ec0b1b5c2043a984e7` |
| `gates_merged/RUNLOG.txt` (G2) | `a717ca94ae72e6c66020cc1fe03d6ff9167b8ee8e69b94a39e58fff8b7b49ae4` (4 arms, exe `6edd4754`; outputs hash-equal to G) |
