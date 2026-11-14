# RESULT IMG-5-ORIGTOP-DEFAULT: image default ON on the MASHED_ORIG_TOP path (L-149 (a) step 5, L-151 (a))

Row `IMG-5-ORIGTOP-DEFAULT` (D4, run 46). Branch `orch/IMG-5-ORIGTOP-DEFAULT` from `orch/integration` @ `d1e5ac586`.

## Verdict: DECISION (the ON logs differ from the OFF logs, so the P1 criterion is not met; the flip is coded on the branch, not enqueued)

L-127 class P1 needs the knob-path captures and logs unchanged ON against OFF on OFF / ON / OFF / ON passes. They are not unchanged:
448 `[RW_BIND]` teardown lines and 3 init/entry lines differ (section "Compare"). The cause is the intended behaviour of the image
(the real teardown bodies run), which is why this goes to Mariano as a decision row and not as an auto P1.

## Caveat (verbatim from the lead)

IMG-5's image-ON boots reach AppDestroy with no AV only under the run-45 null guard, with 130 of 286 teardown destroys called with 0
([UNCERTAIN], cause open in IMG-5-TEARDOWN-NULLS). Do not read "no AV" as "no init or load stand-in left". The null guard is untouched
(separate row RWC-NULL-DESTROY-GUARD).

## What changed (2 files, knob path only)

- `Compat/PortStateImage.cpp` `PortStateImage_On()`: unset now means ON, `MASHED_PORTSTATE_IMAGE=0` opts out. Its only caller is WinMain's
  `origTop` branch (`exe_main.cpp:10649`), so the default (hand) path never consults it.
- `Compat/PortTopCodePtr.cpp` `PortTopCodePtr_Apply()`: `MASHED_OT_CODEPTR` unset now means ON, `=0` opts out. Also called only under `origTop`
  (`exe_main.cpp:10657`). It moves with the image because image-ON boots need the bound slots (RESULT_IMG-5-TEARDOWN-BODIES: without them slot
  0x005f99b0 keeps its original `.text` address).
- Scope: the code-pointer binding now also runs on the image-OFF seed path unless `=0`. The OFF arms set both opt-outs, which equals the previous default.
- Not changed: `MASHED_CODE_TRAP` (stays default OFF, its default is a separate decision), the null guard, the hand path, the gates.

## Boot arms (one default-build exe, 3000 frames, `MASHED_ORIG_TOP=1`, test-slot outside, `mashed_lock.py run` inside, private root)

OFF = `MASHED_PORTSTATE_IMAGE=0 MASHED_OT_CODEPTR=0`. ON = neither set. `hand` = no `MASHED_ORIG_TOP`, 600 frames.

| arm | rc | time | log sha256 |
|---|---|---|---|
| off1 | 0 | 32.5 s | 9778b93a625c854c... |
| on1 | 0 | 148.5 s (includes lock queue time inside the run) | 950e62dcb2a1136c... |
| off2 | 0 | 37.6 s | 830946b18a9eed5f... |
| on2 | 0 | 30.9 s | 8f024c57225d7b84... |
| hand (default path) | 0 | 20.0 s | dab220241e1a9e92... |

Every ON arm ends `[ORIG_TOP] WinMain 0x00492370 returned 0 after 3000 frames`.

ON self-tests (on1 and on2, same lines): `[portstate-image] witness PortStateSeed: 272/272 entries equal, mem=5ecb7d52 want=5ecb7d52 PASS`,
`witness RdataSeed 46/46, DInputSeed 2/2, CP_SEED 72/72 equal; CP_FIX slots holding the original target 188/188`,
`[portstate-image] ... self-test PASS (0 of 8 checks failed)`, `[OT_CODEPTR] applied: 188 slots (11 bound to an exe body, 177 to a stop thunk) ... self-test PASS (0 of 335 checks failed)`.

## Compare (method: `verify/portstate_20261114/img5_origtop_cmp2.py`, raw output `cmp2_full.txt`)

Each log is read line by line. Dropped before comparing: the region-map lines (`COMMIT`, `--- totals`), `cfglog:` timestamps, the `PC_24 after device`
thread id, the `INTRO movie` and `det freeze` timing lines, and the loader/seed self-report lines (`[portstate-*]`, `[OT_CODEPTR]`, `rdata seed`), which
differ by design. 8-digit hex values, `tid=`, and durations are masked. Remaining lines are diffed with `difflib`.

| pair | lines | differing |
|---|---|---|
| off1 vs off2 | 1302 / 1302 | 0 |
| on1 vs on2 | 1749 / 1749 | 0 |
| off1 vs on1 | 1302 / 1749 | 451 |
| off2 vs on2 | 1302 / 1749 | 451 |

So each side is repeatable on its own, and OFF against ON differs the same way both times. The 451 lines are:
- 448 `[RW_BIND]` lines (RpClumpDestroy, RpAtomicDestroy, RwTextureDestroy, RwGlobals +0x10c free) that appear only in ON, in the AppDestroy teardown;
- `type table +0x14 slots: 9 init pointers stored ... 0 already non-zero` (OFF) against `8 ... 1 already non-zero` (ON);
- `type table entry 0: ... matched body returned, first clone` present only in OFF.

Everything before the teardown, and the exit code, is the same. No pixel captures (nav-demo walk or draw-list) were taken: the knob boots here are
log-only, and the row changed no menu code.

## Object build (`MASHED_DDKOBJ=1`, NO_DEPLOY)

- `=== Build OK ===`, log `build_ddkobj.log`. `ddkobj.py check`: IDENTICAL 1241 of 1275, DIFFERENT 19, not-in-map 15, units linked 1222/1501,
  refusals 9 (externalized-static-register-convention 5, wrapper-redefines-globals 4).
- DIFFERENT list: `0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498510 0x00498810 0x004c0c20 0x004c0f00`
  (the 12 of the previous rows) plus `0x00552890 0x005530f0 0x005533d0 0x005538a0 0x00554200 0x00554940 0x005572c0`. This row edits only the two Compat files
  above, so it did not touch those units. [UNCERTAIN] The baseline of `orch/integration` @ d1e5ac586 was not measured here. Test: run `ddkobj.py check`
  on that head. Missing: that baseline.
- No knob boot of the object-build exe was run in this row. Test: the same off/on arms with `KNOB_EXE` pointing at the object build.

## Not run

G-CTRL / B / 12x4 captures (top-down row, merge train runs the default gates); object-build knob boot; menu walk (no menu code changed);
`MASHED_CODE_TRAP` arms (knob untouched).

## Decision for Mariano (filed with `roadmap.ps1 decision`)

- (a) Accept ON as the ORIG_TOP default. The log difference is the teardown now running the real matched bodies, plus the loader lines. Recommended, but it
  rests on the caveat above: the ON teardown reaches AppDestroy only under the null guard, so its "no AV" is not evidence of a complete init.
- (b) Hold the default OFF until IMG-5-TEARDOWN-NULLS explains the 130 null destroys, then re-run these arms.
- (c) Make only the image default and leave `MASHED_OT_CODEPTR` opt-in. Not recommended: image-ON without the bound slots leaves slot 0x005f99b0 at its original `.text` address.

## Raw files (not committed), sha256

```
9778b93a625c854cce77ac356d75f3ee393af50866653050d9daf7285da187de  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots/off1.log
950e62dcb2a1136c1cab1e43befb7fd701b20ee8f70841ae4ad0a00768a5729d  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots/on1.log
830946b18a9eed5f80f8faf61fa86ef29607ee20e079faa49101e1a8c968142f  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots/off2.log
8f024c57225d7b84d8a0a27f5b50ea75352b0892e8bcc28f3901bed801619817  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots/on2.log
dab220241e1a9e92ef70190555ca59bbfb8118261b63b91bc5428d033f19e173  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots/hand.log
dbfd845ba9948b01df86443157baa97aa75884cf53027bfeaa6fd1aa35ca7023  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots.sh
ca43f2824a98e9104e1725017354cb2d535a35867190cabf2226a592395a3696  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots.out
2954d327c7daa1fc70106c5ad6c9b71dba10c206f4e9b6a5df28e997473e6120  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/build_default.log
52587045def77b8b4ee219a3264f2dfdf1dd59b868727fa99ca598f4fb2a17df  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/build_ddkobj.log
e4c2205d5625bfe94c7a6c5926d9eeb65790a9cd8b997e41a511a42d00fa124c  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/ddkobj_check.txt
c7f59dc04bdf88ab645f8de8b0b54946c0c84a9d9ed88f52171e8e32db9e2200  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/cmp2.txt
23747aa719f240a34cc9571a5fedc964628793d0f543728ed5f104e150727f93  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/cmp2_full.txt
fbaaea5e454f3aaf4cc268ba7e24bb2f2d6f302eafaf064b1e1e6622417ef4a8  D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/mashed_re_default.exe
```
