# RESULT IMG-5-TEARDOWN-BODIES: real bodies for the 9 powerup-table teardown targets (L-151 (a))

Row `IMG-5-TEARDOWN-BODIES` (D4-image, run 45). Worker cmwflfaca2xsulh1nry13sxjw (claude3, Opus 5.5), lead
cmwfldvnw2xialh1nug92xgbo. Row kind: top-down (bindings + `re/match` sources + promotion TSV). Branch
`orch/IMG-5-TEARDOWN-BODIES` from `orch/integration` @ `ba39eeb6b`.

## Verdict: DECISION (all 9 bodies are matched, bound and byte-identical in the object build. The image-ON boot still AVs in teardown, now at entry 2, because entry 2's INIT 0x00455400 is a stand-in)

- **Master Ghidra:** the 7 missing functions were created (`CreateMissedFunctions.java ... apply`), with sizes 61 / 93 / 39 / 45 /
  55 / 53 / 69 B (`img5_create7_master.tsv`). Pools were synced (14 refreshed, 2 locked slots skipped).
- **Match:** all 9 teardown targets, plus their two particle callees 0x004898d0 and 0x00489c60, are 100% with
  `match.py` (XP DDK) and `relocheck.py` reports OK (`img5_match.txt`). The 7 new ones are raw g2s output.
- **Promotion (branch, not applied):** `re/promotions/IMG-5-TEARDOWN-BODIES.tsv` has 7 rows, route `match`. The
  `c4_reclass.py batch` dry run gives 7/7 PASS (`img5_c4_dryrun.txt`). The C1 rows were added with `re/tools/c1_add.py`.
  0x004568a0 and 0x004540c0 were already C4.
- **Port:** 11 exe bodies (`Match/PortTop_<rva>.cpp`, `Orig_<rva>`) and 11 BINDINGS.tsv rows. The slots are bound by the
  generators, not by hand:
  - `codeptr_inventory.py --write`: slot 0x005f99b0 now holds `Orig_004568a0`, no longer a stop thunk.
  - `codeptr_image.py --write`: slots 1..8, with 75 image slots bound (was 67).
- **Object build:** the 11 bodies are IDENTICAL in `ddkobj.py check` (`img5_bytecheck.tsv`). The DIFFERENT list is unchanged.
- **Boot:** with `MASHED_OT_CODEPTR=1` every image arm passes 0x004568A0 and 0x004540c0. On both builds, all 6 arms then
  end in an access violation (0xC0000005) inside entry 2's teardown 0x004555d0. Entry 2's records are zero because its
  init 0x00455400 never ran on this path: it is the one remaining stand-in in `PortTop_0045bae0.cpp` (94.0%, POWERUPINIT-B2).
  `equiv.py` on it gives NOT-PROVEN (section 4).
- **0x004568A0 verdict (row PORT-TOP-AUDIOTICK-IMGTRAP-4568A0): passed when `MASHED_OT_CODEPTR=1` is set, not without it.**
  - With `MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1` alone (the arms of that row), the slot keeps the image value.
    The trap still reports `execute original 0x004568A0 slot 0x005F99B0` on both builds (def_aud_img1_trap1,
    ddk_aud_img1_trap1).
  - With `MASHED_OT_CODEPTR=1` added, both AUDIOTICK arms run through entries 0 and 1 and then end in the same entry-2 AV.
- **Self-tests (every arm that sets the knobs, both builds):**
  - `[OT_CODEPTR] self-test PASS (0 of 335 checks failed)`, image slots 75 written and 0 refused.
  - `[portstate-image] self-test PASS (0 of 8 checks failed)` and `witness PortStateSeed 272/272 PASS`.
  - `[code-trap] self-test PASS (0 of 6 checks failed)`.

Decision for the lead. 0x004555d0 matches the original byte for byte. Its zero records come from the stand-in init.
- **(a) Recommended:** a body row for 0x00455400, the entry-2 init.
  - It is 94.0% (`re/match/s29/00455400.c`), and `equiv.py` gives NOT-PROVEN `call-memory` at the RpClumpClone call
    0x00455581.
  - Route 1: shape it to 100% (POWERUPINIT-B2 lists the residual as instruction order plus recompute against IV reuse).
  - Route 2: admit it as a provisional body (PROVISIONAL.tsv, PROV-INSITU replay) on the knob path.
  - With entry 2's records built, re-run the arms with no other change.
- (b) Make entry 2's teardown depend on its init: skip 0x004555d0 while entry 2 kept its init stand-in.
  - With the image OFF the slot is 0, so teardown already skips it. (b) is the same behaviour.
  - It is still a stand-in on the T1 path, which L-151 (a) excludes. Not done.

## 1. What changed

| File | Change |
|---|---|
| `re/match/s09/{004555d0,00457b10,0045a790,0045b2d0,00454af0,00456ca0,004530b0}.c` (new) | g2s output, s09 preamble, globals as MGLOBAL, `RpClumpDestroy` renamed `FUN_004e6e00` so relocheck checks the call. `// FLAGS: /O2 /GS`, `// TOOLCHAIN ddk` |
| `re/match/s09/004568a0.c`, `00454af0.c`, `00456ca0.c`, `00457b10.c` | callee prototype `FUN_004768c0(void *p0)` (was `int`): same push under C, valid C++ for the port. Re-matched 100% |
| `re/match/s09/004555d0.c` | callee prototype `FUN_00489c60(int p0)` (was `undefined4 *`), same reason |
| `re/match/s14/00489c60.c` | `typedef int code(...)` under `__cplusplus` only (the rwl03 pattern). C text unchanged, re-matched 100% |
| `mashedmod/src/mashed_re/Match/PortTop_<rva>.cpp` (11 new) | exe-only wrappers. Callees bind by name: `MashedRw_ClumpDestroy_004e6e00`, `MashedRw_FrameDestroy_004c0c20`, `MashedRw_FrameRemoveChild_004c1210`, `MashedRw_AtomicDestroy_004e6920` (existing RW_BIND entries, no new librw binding), `Orig_004768c0`, `Orig_004898d0`, `Orig_00489c60` |
| `mashedmod/exe_sources.rsp` | +11 TUs |
| `verify/porttop_20261108/BINDINGS.tsv` | +11 rows (kind `matched`) |
| `mashedmod/src/mashed_re/Compat/PortTopCodePtr.inc`, `verify/portorigtop_20261109/CODEPTR_SLOTS.tsv` | regenerated by `codeptr_inventory.py --write`: 11 bound / 177 stop (was 10 / 178) |
| `mashedmod/src/mashed_re/Compat/PortTopCodePtrImage.inc`, `re/tools/image/CODEPTR_IMAGE.tsv` | regenerated by `codeptr_image.py --write`: 75 bound / 1012 trap (was 67 / 1020) |
| `re/console/pc_funcs.csv` | +7 extent rows (entry, last byte, size, from the master create) |
| `hooks.csv`, `re/analysis/CHANGELOG.md` | +7 C1 rows / lines, `c1_add.py` (re-classify skill), subsystem `gameplay` |
| `re/promotions/IMG-5-TEARDOWN-BODIES.tsv` | 7 rows route `match` (applied by the merge train) |

No default flipped. The new bodies run only on `MASHED_ORIG_TOP=1` with `MASHED_OT_CODEPTR=1` (slot 0), plus
`MASHED_PORTSTATE_IMAGE=1` (slots 1..8). No hand-path table referenced.

## 2. Ghidra (master write, ghidra-sweep style)

- **Pre-checks:** `ghidra_assert.sh scribe-check` was free, no Java process was running, and there was no `.lock` on
  `Mashed.rep`. A `master.WIP-img5` flag was held during the write (gitignored, removed after).
- **Backup:** `Mashed.rep` + `Mashed.gpr` copied to `D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/master_backup`.
- **Dry run on pool clone Mashed_pool10:** all 7 `WOULD_CREATE`, each "already disassembled: PUSH ESI".
- **Apply on master:** all 7 `CREATED` (`FUN_<rva>`, sizes 61, 93, 39, 45, 55, 53, 69), `Save succeeded for processed
  file: /MASHED.exe`. The sizes equal the IMG-4 transient `--create` decompilations.
- **Pool sync:** `ghidra_pool.sh sync`, 14 refreshed and 2 skipped (locked).

## 3. Match

| rva | entry | source | match.py (DDK) | relocheck |
|---|---|---|---|---|
| 0x004568a0 | 0 | s09/004568a0.c | 100% 14/14, /O2 /GS /Ob1 | OK 4/0 |
| 0x004540c0 | 1 | s09/004540c0.c | 100% 35/35, /O2 /GS /Ob1 | OK 11/0 |
| 0x004555d0 | 2 | s09/004555d0.c | 100% 22/22, /O2 /GS | OK 5/0 |
| 0x00457b10 | 3 | s09/00457b10.c | 100% 28/28 | OK 10/0 |
| 0x0045a790 | 4 | s09/0045a790.c | 100% 14/14 | OK 3/0 |
| 0x0045b2d0 | 5 | s09/0045b2d0.c | 100% 15/15 | OK 4/0 |
| 0x00454af0 | 6 | s09/00454af0.c | 100% 17/17 | OK 6/0 |
| 0x00456ca0 | 7 | s09/00456ca0.c | 100% 16/16 | OK 6/0 |
| 0x004530b0 | 8 | s09/004530b0.c | 100% 22/22 | OK 5/0 |
| 0x004898d0 | callee | s14/004898d0.c | 100% 13/13 | OK 2/0 |
| 0x00489c60 | callee | s14/00489c60.c | 100% 25/25 | OK 5/0 |

`c4_reclass.py batch --tsv re/promotions/IMG-5-TEARDOWN-BODIES.tsv` (dry run): 7/7 PASS, byte-identical 61 / 93 / 39 / 45 /
55 / 53 / 69 B.

## 4. Entry 2's init 0x00455400

`py -3.12 re/tools/match/equiv.py re/match/s29/00455400.c --only 0x00455400` gives:
`NOT-PROVEN call-memory: region ours +0x181 / orig 0x00455581 call 0x4e6ab0: memory differs (...)`.
The proof stops at the RpClumpClone call; the two sides' stored memory terms differ before it. Under L-126 that is not an
admission. A provisional body needs a PROVISIONAL.tsv row with PROV-INSITU replay evidence. This row did neither.

## 5. Knob boots

All boots: private root `D:/MashedWT/IMG-5-TEARDOWN-BODIES_root` (robocopy of IMG-4's root, `original/mashed_image.bin`
sha256 81cc7775... as IMG-4). `PROYECTOS_TEST_HUB=off test-slot` outside, `mashed_lock.py run --label
IMG-5-TEARDOWN-BODIES` inside, `knob_boot.py` (kills only its own PID).

| arm | exe | env | rc | end of log |
|---|---|---|---|---|
| def_off | default | DET_FRAMES=3000 | 0 | knobs off |
| def_on_img1_cp_trap1 | default | ORIG_TOP + IMAGE + OT_CODEPTR + CODE_TRAP=1, 3000 frames | 0xC0000005 (40.6 s) | AppDestroy, entries 0 and 1 run, then `RpClumpDestroy(0)` x2 and the AV |
| def_on_img1_cp_trap2 | default | same, CODE_TRAP=2 | 0xC0000005 | same; 0 trap reports |
| ddk_on_img1_cp_trap1 | object | as def_on_img1_cp_trap1 | 0xC0000005 | same |
| ddk_on_img1_cp_trap2 | object | as def_on_img1_cp_trap2 | 0xC0000005 | same; 0 trap reports |
| def_aud_img1_trap1 | default | ORIG_TOP + OT_TIMER + OT_AUDIOTICK + IMAGE + CODE_TRAP=1, 600 frames | 0x004568A0 | `execute original 0x004568A0 slot 0x005F99B0 ... ret mashed_re.exe+0x17241E` |
| def_aud_img1_cp_trap1 | default | + OT_CODEPTR=1 | 0xC0000005 (7.2 s) | entries 0 and 1 run, entry-2 AV |
| ddk_aud_img1_trap1 | object | as def_aud_img1_trap1 | 0x004568A0 | same trap, ret mashed_re.exe+0x5648 |
| ddk_aud_img1_cp_trap1 | object | + OT_CODEPTR=1 | 0xC0000005 (7.7 s) | entry-2 AV |

**Teardown sequence (def_on_img1_cp_trap1.log, from the `0x00404820` stand-in line on):**
- **Entry 0, 0x004568a0:** `RpClumpDestroy` x4 (0x006886b8 step 0x48 to 0x006887d8). Then 0x004768c0(&0x006887d0): one
  PTank `RpAtomicDestroy`, `RwTextureDestroy` and 4 `RwGlobals +0x10c free`.
- **Entry 1, 0x004540c0:** `RpClumpDestroy` x4 (0F5490B8, 0F549198, 0F5490F0, 0F549160). The frame/atomic loop writes no
  RW_BIND line. Then `RpClumpDestroy(DAT_006882f4 = 0F548F68)` and `RpClumpDestroy(DAT_006882f0 = 0F549828)`.
- **Entry 2, 0x004555d0:** `RpClumpDestroy(0x00000000)` x2 (`*p` at 0x006885e8 and `q[-3]`), then the process ends with
  0xC0000005.
  - The next call in that body is `0x00489c60(*q)` with `q` = 0x00688418, and its first statement is `param_1[3] = 0`.
  - [UNCERTAIN] The faulting instruction address was not captured: no exception logger is armed on this path and no
    dump was written. The attribution rests on the call order of the matched source and on the last two log lines.
    What is missing: the EIP at the fault.
- **Every image arm logs** `type table entry 2 (+0x14 = 0x00455400 in the original): init stand-in, not run`. So records
  0x006883b8 + i*0x6c and 0x006885d4 + i*0x2c are never written on this path.

`0x0045bae0` logs `8 init pointers stored ..., 1 already non-zero` with OT_CODEPTR (slot 0x005f99ac bound to
`Orig_00456760` by the CODEPTR fix-up, as IMG-4 section 5 noted). Without OT_CODEPTR it logs `9 ... stored`.

## 6. Checks

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, rva-lint OK (237 known, 0 new), asm-lint OK, portstate-seed OK (272, 3231 B, fnv 0x5ecb7d52) |
| object build (`MASHED_DDKOBJ=1`, NO_DEPLOY) | `=== Build OK ===`, `[ddkobj] link rc=0`, units 1457, ddk objects 1183 |
| `py.exe -3.12 re/tools/ddkobj/ddkobj.py check` | IDENTICAL 1208/1235, DIFFERENT 12 `[0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498510 0x00498810 0x004c0c20 0x004c0f00]` (same 12 as IMG-4), not-in-map 15, units linked 1183/1457, refusals 4 (wrapper-redefines-globals=4). The 11 IMG-5 bodies are all IDENTICAL |
| self-tests | OT_CODEPTR PASS (0/335), image slots 75 written / 0 refused, portstate-image PASS (0/8), PortStateSeed witness 272/272 PASS, code-trap PASS (0/6), on both builds |
| knobs off | def_off rc 0 after 3000 frames |
| G-CTRL / B / 12x4 captures / menu walk | not run: top-down row, no menu code changed. The merge train runs the default gates |
| `original/mashed_image.bin` | absent in the real `original\` (untouched) |

## 7. Raw files (not committed), sha256

```
523ca8779db10843acb6f342695763596afbbc266467afdcc07516ca7421d157  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/default/mashed_re.exe
e35f8bbc8e6882b8e46bebd1f1019323733531d5f737fc5b7898e14f6ee2c215  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/default/mashed_re.map
1ed9323e7865cd39827d630bc70ce318779b928389bf56dccaaf9cf301e8c445  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/ddkobj/mashed_re.exe
4a154ae44f26874cb9f38b935c5e1ba566b1e337b568e691d013145f0b65b7ae  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/ddkobj/BYTECHECK.tsv
81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8  D:/MashedWT/IMG-5-TEARDOWN-BODIES_root/original/mashed_image.bin
1677a40ede7aa9c0be3d5741bf8e61975bbd11761dcf44b711423e5cf4fbe6f4  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/ddk_aud_img1_cp_trap1.log
7ecf36f3b2af8b6fcbfdfb96469c6b4cfcc802b529a9ff6b75a9f507e9a1e44e  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/ddk_aud_img1_trap1.log
215ccecf1b1974f703982ef819a86de63309b108dcb3668a8f4683d9874da739  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/ddk_on_img1_cp_trap1.log
6ffc2d088290d49571fe036f482b65fccbf7f6df22a9ae480d194fc24dce621a  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/ddk_on_img1_cp_trap2.log
0e977dfb0718eb7dfb76f3cb0e0f3e7d3979fa33dbab54d583f3a945513d34a5  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/def_aud_img1_cp_trap1.log
d16804a1438adf64a5ff811828ad36f1043dcdf03422a6110eda0179873aa6d5  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/def_aud_img1_trap1.log
3ef9fd935c4d12fe8b0aa6d153220baaeec30f9c106a054ea51518e1691439cd  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/def_off.log
c967c827f43acfdba9ff437344f3a71b19f5ed2cc3d6ab534755734ba88425d2  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/def_on_img1_cp_trap1.log
48c50e7804efeccda900f40210eff51d83cfdc5d4a1b392e34ed893b4d44688c  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/def_on_img1_cp_trap2.log
4109695f9580c4364b501119a207d6f4a5f37174208d14435506f670cb3a22c1  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots.sh
5a569663670e1a251a6e14f38cfc27eefed0116245a72cc10abb87a68b8ce589  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots.out
9a0a90062edf7eed5fc664c3cbef6246c36d8b44c92df0bd5a25a7760b6fcf2e  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/build_default.log
af2526657db833ccc1aa32b5f72cfcc9436c14a26b6f4426dadf3157937c6d6e  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/build_ddkobj.log
e4e617bcab4f7e180b9a5b75fcd45400541d652e0a1b390e1f0def5fb3916db5  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/ddkobj_check.txt
a578fe7038ab2a4f388a5d4b38575f5c864ead44bd670810fd0852621011e1ac  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/decomp7.json
432aa107149ceab62e7c8d396af4f17f6fa38e73232467d24c87bfc398371941  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/create7_dryrun_pool.tsv
1a326999f9748b348adf46be5835c6c61edca0205f5ca38e0c81728fb0592c04  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/create7_master_apply.tsv
c757f19d722536fab5402eee55f7710d522df766d81e2ff333f2b7d956df441e  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/create7_master_apply.log
f7b09f1f5f83d18bfa50b2c5da3639dd0d3c3955bf892c2c42cd7a8aeff07170  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/g2s/results.tsv
c4930031b7a008aa8a8969ca5892692ff0d5a5bdcee20a80b2a2730c2c297348  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/tos09.py
d032eff7a949fb0814ead1162995a13639fa2588df101e05097b3370bd4ad514  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/gen_wrappers.py
39e8e5693e79719147f96e76ea35b9910a701813d7e7bd7d9acf4bae4b6842f0  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/addrows.py
```

Master backup (pre-write): `D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/master_backup/` (Mashed.rep + Mashed.gpr).

## Next

- Lead decision (a) or (b) above for entry 2's init 0x00455400.
- With entry 2 built, re-run `boots.sh` arms `def:on_img1_cp_trap1 ddk:on_img1_cp_trap1 def:aud_img1_cp_trap1`. Entries
  3..8 (0x00457b10 ... 0x004530b0) have not run yet on this path: the AV at entry 2 ends the process first.
- Row PORT-TOP-AUDIOTICK-IMGTRAP-4568A0: its trap is cleared with `MASHED_OT_CODEPTR=1`. The AUDIOTICK arms need that
  knob added, and they then hit the same entry-2 AV.
