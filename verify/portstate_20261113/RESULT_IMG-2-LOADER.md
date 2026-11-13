# RESULT IMG-2-LOADER: whole data image loader on the MASHED_ORIG_TOP path (L-149 a, step 2)

Row `IMG-2-LOADER` (D4-IMG, run 44). Worker cmwfaaild1p6ylh1njiuwr0l1, lead cmwf9zx6d1ljulh1nd88fk6k9. Row kind: top-down.
**Branch `orch/IMG-2-LOADER` was created from `orch/IMG-1-EXTRACT` @ `fc6e8d8f1`** (not from `orch/integration`):
IMG-1 was enqueued but not merged yet, and this row uses its `re/tools/image/` tools. Merge IMG-1 first, or with this.
Spec: `verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md` sections 2, 3 and 5 (IMG-2).

## Verdict: GREEN (loader, self-test, refusal, stage and build hooks), with one knob-path finding for IMG-3 / IMG-4

- Knob `MASHED_PORTSTATE_IMAGE=1`, default OFF, read only on WinMain's `MASHED_ORIG_TOP` branch. Default path unchanged:
  `def_off_img1` (knob set, no ORIG_TOP) logs no `[portstate-image]` line and the seed line as before.
- **Self-test: PASS, 0 of 8 checks failed**, on both builds: the 3 ranges read back equal to the file (IAT bytes 0), the
  PortStateSeed memory witness `mem=5ecb7d52 want=5ecb7d52` (272/272 entries equal, written by the image only), RdataSeed
  46/46, DInputSeed 2/2, CP_SEED 72/72 equal, and all 188 CP_FIX slots hold their original target.
- **ddkobj check: IDENTICAL 1192/1219, DIFFERENT 12, not-in-map 15, units linked 1167/1443, refusals 4
  (wrapper-redefines-globals=4)**. Same tally and same DIFFERENT list as the base (RESULT_BIND-CRT-498510 section 3).
- Finding: with the image ON the knob boot runs its 3000 frames with a log identical to the image-OFF boot (below), then
  ends `0xC0000005` in AppDestroy, at the first code pointer of the original that no list binds. That is spec section 6
  risk 2, the case IMG-3 (trap) reports and IMG-4 (CP list) fixes. Knob default OFF, so nothing ships differently.

## 1. What changed

| File | Change |
|---|---|
| `mashedmod/src/mashed_re/Compat/PortStateImage.{h,cpp}` (new, exe only, `exe_sources.rsp`) | loader + in-process self-test |
| `mashedmod/src/mashed_re/exe_main.cpp` WinMain | reads `MASHED_ORIG_TOP` first; on that branch `PortStateImage_Apply()`, and `PortStateSeed_Apply()` only when the image is off or refused. Default branch: `PortStateSeed_Apply()` as before. `PortStateImage_Log` after the log truncate, next to `PortStateSeed_Log` |
| `Compat/PortStateSeed.cpp` | `PortStateSeed_Log` returns when the seed did not run (only possible when the image ran in its place) |
| `Match/PortTop_00492370.cpp` `PortTop_SeedRdata` | skipped when the image is in (one log line) |
| `Match/PortTop_InputPort.cpp` `PortTop_SeedDInput` | the two byte entries skipped when the image is in. The `c_dfDIKeyboard` copy to 0x005d0974 still runs: the original's DIDATAFORMAT points at 0x0049b5d0 in `.text` |
| `Compat/PortTopCodePtr.cpp` | CP_SEED writes skipped when the image is in (counted, and its self-test still compares them). CP_FIX unchanged: runs only under `MASHED_OT_CODEPTR=1` |
| `Match/PortTop_0045bae0.cpp` | the type-table `+0x14` fill (slot 0x005f9998 + 0x40*i + 0x14, i < 9) also rebinds a slot that holds its own entry's original target. See section 3 |
| `mashedmod/build.bat` | after the exe link: `build\mashed_image.bin` via `extract_image.py` when missing (source `%MASHED_ROOT%\original\MASHED.exe.unpatched`, else `..\original\`); skipped with a note when the extractor (git-archive tree) or the `.unpatched` is absent |
| `scripts/lan_release/stage_lan_release.py` | stages `original\mashed_image.bin` (built in memory by `mashed_image.py` from `<install>\MASHED.exe.unpatched`, anchor checked, hard stop when missing or not the anchor) and lists it in the manifest. Only the stage's own copy is written |

Loader (`PortStateImage.cpp`), per spec section 2:
- lookup: `MASHED_IMAGE` (only candidate when set), the exe's folder, `MASHED_ROOT\original\`, then `original\` under each
  parent of the exe's folder (6 hops);
- checks before any write: magic `MSIM`, version 1, source SHA-256 = anchor `BDCAE093...3C0E`, trailer SHA-256 (own
  FIPS 180-4 implementation, no new library), range count 1..8, each range inside the file, each range FNV-1a, each range
  inside the B17 pad and at or above 0x005cc000. Any failure: one `REFUSED(<reason>)` line, nothing written, and WinMain
  runs the per-address seed instead;
- logs the nonzero bytes already in the ranges before the copy: **2** on every image boot (spec risk 1 measured: 2 bytes
  of 434,252 were written before WinMain; which ones is not recorded, [UNCERTAIN]: no per-address dump was taken);
- copies the payloads, zeroes the IAT 0x005cc000..0x005cc2e8 (spec section 3), runs the self-test before any game code.

## 2. Checks

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, rva-lint OK (237 known, 0 new), asm-lint OK, portstate-seed OK (272, 3231 B, fnv 0x5ecb7d52). `mashed_re.exe` sha256 `ee492f7e5965510c15619268f0c569c55ce425053cdfb9a5f32f5b535f25b58e` |
| build.bat image step | first build wrote `mashedmod\build\mashed_image.bin` 434,400 B sha256 `81CC7775...9AFCD8` (= IMG-1's staged file); later builds leave it (exists) |
| object build (`MASHED_DDKOBJ=1`, NO_DEPLOY) | `=== Build OK ===`, `mashed_re_ddk.exe` sha256 `9967a45d41207861e57622b2f62fa3322ccd7c86cc724f3a00e57de63e96337c` |
| `py -3.12 re/tools/ddkobj/ddkobj.py check` | IDENTICAL 1192/1219, DIFFERENT 12 `[0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498510 0x00498810 0x004c0c20 0x004c0f00]`, not-in-map 15, units linked 1167/1443, refusals 4 (wrapper-redefines-globals=4). Unchanged against the base |
| stage hook (scratch stage, removed after) | `[+] original\mashed_image.bin 434400 bytes sha256 81CC7775...9AFCD8`, 205 files; manifest entry `original/mashed_image.bin`; `.exe/.dll/.asi` in the manifest: `mashed_re.exe` only |
| real `original\` | `original/mashed_image.bin` absent after every step; the private root got the IMG-1 file by copy |
| G-CTRL / B / 12x4 captures / menu walk | not run: top-down row, no menu code changed. The merge train runs the default gates |

## 3. Knob boots (`MASHED_DET_FRAMES=3000`, private root `D:/MashedWT/IMG-2-LOADER_root`, test-slot outside, `mashed_lock.py run --label IMG-2-LOADER` inside)

| arm | exe | env | result | image lines |
|---|---|---|---|---|
| def_on_img0 | default | ORIG_TOP | WinMain returned 0 after 3000 frames, rc 0 | none; seed 272/272, rdata 46/46, dinput 3/3 |
| def_on_img1 | default | ORIG_TOP + IMAGE | 3000 frames, then rc 0xC0000005 in AppDestroy | loaded from `<exe dir>\mashed_image.bin`, self-test PASS 0/8 failed |
| ddk_on_img0 | object | ORIG_TOP | returned 0 after 3000 frames, rc 0 | none |
| ddk_on_img1 | object | ORIG_TOP + IMAGE | 3000 frames, then rc 0xC0000005 at the same line | loaded from `<root>\original\mashed_image.bin` (second lookup leg), self-test PASS |
| def_on_img1_bad | default | ORIG_TOP + IMAGE + `MASHED_IMAGE=bad_image.bin` (one payload byte at file offset 0x200 flipped) | returned 0 after 3000 frames, rc 0 | `REFUSED(trailer-sha256) ... nothing written; the per-address seed runs instead`; seed 272/272 |
| def_off_img1 | default | IMAGE only (no ORIG_TOP) | rc 0 | none (default path ignores the knob) |

**Image ON against image OFF.** The `def_on_img1` log masked for addresses, with the image/seed lines removed, equals
`def_on_img0` line for line except the B17 page-map dump (the copy commits pad pages: totals 12,992 KB vs 9,860 KB), the
`cfglog` date line and an x87 thread id, through the 3000 frames and AppDestroy up to `[PORT-TOP] rva 0x00404820:
stand-in`. Both image-ON logs end on that line (859 lines each, default and object).

**Where it stops.** `re/match/s09/0045b930.c` (powerup teardown, called from AppDestroy 0x00402a40): after
`FUN_00404820()` it calls `PTR_FUN_005f99b0[i].m_fn` for i < `DAT_005f9bd8` (= 9) when nonzero. Image OFF: those words
are 0 (the seed does not cover them) and the calls are skipped. Image ON: they hold the original's `.text` targets.
Entry 0's word 0x005f99b0 = 0x004568a0 is a CP_FIX slot (`PortTopCodePtr.inc:240`, bound to a stop thunk only under
`MASHED_OT_CODEPTR=1`); the slots of entries 1..8 (0x005f99f0 + 0x40*(i-1)) are on no list. A call to 0x004568a0 runs
pad bytes of the original code range, which IMG-3's trap would report. RESULT_PORT-OT-SEED-CODEPTR recorded the same
first stop (0x004568a0 through 0x005f99b0) for its own knob.

**The earlier stop, fixed in this row (Match/PortTop_0045bae0.cpp).** The first image-ON boot stopped at 13 s in the
powerup init: the wrapper filled the type-table `+0x14` slots only when zero, and logged `0 init pointers stored ..., 9
already non-zero` (image OFF: `9 stored, 0 already non-zero`). The image values at 0x005f99ac + 0x40*i are, from
`mashed_image.bin`: `00456760 004548e0 00455400 004583a0 0045a6b0 0045b830 00454e70 004576f0 00452f70`, exactly the
nine rvas the wrapper's own `s_pwiInit` table names. The wrapper now also rebinds a slot equal to its entry's original
target (`s_pwiOrig`). Image OFF the slots are 0, so this is a no-op there (def_on_img0 still logs `9 stored`).

The overlay `mashed.log` of the image-ON arms is empty (the boots end before the config log is closed).

## 4. Raw files (not committed), sha256

```
ee492f7e5965510c15619268f0c569c55ce425053cdfb9a5f32f5b535f25b58e  D:/MashedWT/IMG-2-LOADER_data/default/mashed_re.exe
9967a45d41207861e57622b2f62fa3322ccd7c86cc724f3a00e57de63e96337c  D:/MashedWT/IMG-2-LOADER_data/ddkobj/mashed_re.exe
81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8  D:/MashedWT/IMG-2-LOADER_data/default/mashed_image.bin
ba94b9eab6dd47922364e5c1202a11b86899142da4ef28a6f2fa10c9fbb5141a  D:/MashedWT/IMG-2-LOADER_data/boots/def_on_img0.log
28e04930023bf5474f7d1f785458944ff497b8633f044e7e6c3d878b75822b9c  D:/MashedWT/IMG-2-LOADER_data/boots/def_on_img1.log
3f0d47c78657b0544a1b80d36e9dd29b9665be4a0c09d96db2822232ad0227ac  D:/MashedWT/IMG-2-LOADER_data/boots/ddk_on_img0.log
3f168cc69a5543c1ab9858e4f55e608137cf9cfc703a0ea7231fb77d4e24503b  D:/MashedWT/IMG-2-LOADER_data/boots/ddk_on_img1.log
5a3b318e95bdf9269d49915f53fd2d8b510b537e7e4cca37da1a6624cb441788  D:/MashedWT/IMG-2-LOADER_data/boots/def_on_img1_bad.log
730dfb23927172de9625c2f76d45ac0da36ba85c4dd0d0f1b18e37ded978f957  D:/MashedWT/IMG-2-LOADER_data/boots/def_off_img1.log
caa334f59d2b37a89fb9a12e7428678fc2e2110aea18f7e0676cebf112d3e754  D:/MashedWT/IMG-2-LOADER_data/boots/def_on_img0.mashed.log
f91d9f903510783abb655742afbc5013246d02b61b97c4725b306f69f505d2f2  D:/MashedWT/IMG-2-LOADER_data/boots/ddk_on_img0.mashed.log
bfe14ee879cf4a76ebe864ebb10f5b0d1d4b7f017ec7723a5659a30b7901526c  D:/MashedWT/IMG-2-LOADER_data/boots/def_on_img1_bad.mashed.log
6993ed1432ec1686dc83dfc4ced0521fbb369296024a4df4716e7f8d8550c622  D:/MashedWT/IMG-2-LOADER_data/bad_image.bin
00adcdd7979dac0f64e3ad3f147508f2bcd63f10fcc15a97b3850c57e1b44cfa  D:/MashedWT/IMG-2-LOADER_data/boots.sh
0a58d9365d091ac3e3aee66c2f2c89373d044c2938463ba3ef150c1b3cdf3ea0  D:/MashedWT/IMG-2-LOADER_data/boots.out
fbb772742799d5915d02a06eb78d4cb742cf3eb0770dec67a581bbc76f2dc5b2  D:/MashedWT/IMG-2-LOADER_data/ddkobj_check.txt
cef8e5e1f1c18917dbb084c8d26f9217ead102d176c87a885b2a8c7e1a331d98  D:/MashedWT/IMG-2-LOADER_data/stage_test.txt
34292d2d43cb7dbfb8e6827484c85eafbc4909fbdb4848de9a9f0a89aea53987  D:/MashedWT/IMG-2-LOADER_data/build_default.log
a4d279a3b9aba7a8bc3941e08be59d3131472194b2a64ec8ed5b6a36b0a6cfa9  D:/MashedWT/IMG-2-LOADER_data/build_ddkobj.log
```

No promotions, no tracker edit, no new librw binding, no decision row.

## Next

- IMG-3 (trap, `MASHED_CODE_TRAP=1`): its first report on this knob boot should be the AppDestroy call through
  0x005f99b0 (target 0x004568a0) from the matched 0x0045b930.
- IMG-4 (Ghidra data-reference export, account3): the 9-entry type table at 0x005f9998 (stride 0x40) has code pointers in
  entries 1..8 that `CODEPTR_SLOTS.tsv` does not list (only entry 0 is there). Until IMG-4 lands, an image-ON boot cannot
  pass AppDestroy without binding those slots.
- Merge order: IMG-1-EXTRACT before (or with) this branch.
