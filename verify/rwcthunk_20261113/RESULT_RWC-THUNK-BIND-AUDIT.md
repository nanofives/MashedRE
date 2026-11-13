# RESULT RWC-THUNK-BIND-AUDIT

Row kind: top-down / tool row (WORKER_RULES "Row kinds"). Run: both builds, `ddkobj.py check`, the knob boots, self-test.
G-CTRL, B and the 12 captures were not run (hashes are computed with the knob off). No librw binding added (L-124).
Worker: claude2, Sonnet medium.

## 1. Audit (`audit_thunk_bind.py`, output `THUNK_AUDIT.tsv`)

For each of the 76 re/match sources that name a `thunk_FUN_<t>` callee (91 source/name pairs), the script reads the
original bytes (`original/MASHED.exe.unpatched`) of the source's own `// MATCH:` function(s), sweeps it linearly with
capstone to its last ret, and classifies each direct `call` (E8) whose target is `<t>` or a `jmp rel32` stub of `<t>`.

| verdict | pairs | meaning |
|---|---|---|
| STUB | 62 | every direct call to `<t>` in the function lands on a jmp stub of `<t>`; default binding (`rva_of` = `<t>`) calls a different address than the original |
| NOCALL | 29 | no call to `<t>` or a stub in the source's own extent (the source defines the thunk itself, or the name is only declared) |
| DIRECT / MIXED | 0 / 0 | none |

All 62 STUB rows name exactly one stub per `<t>` (no multi-stub). `THUNK_BIND.tsv` lists them. Examples:
`thunk_FUN_004cc820` -> stub 0x004cc9e0 (20 sources), `thunk_FUN_004c4680` -> 0x004c4670 (re/match/rwl01/004cf2b0.c),
`thunk_FUN_004cfa00` -> 0x004c7a60 (re/match/s19/004c32b0.c, call at 0x004c3590).

Scope found: only 10 STUB sources are in the generated RW-core closure (0x004c1980, 0x004c5f60, 0x004c78e0, 0x004cbca0,
0x004cd900, 0x004d3db0, 0x0052e3d0, s19/0x004c32b0, s21/0x004d7c60, s21/0x004d8fa0). The other 52 STUB pairs have stubs outside
the RW band (`inrw`) in sources the core does not compile, so the generator leaves them as they were.

## 2. Generator change (`gen_rwcore_tus.py`)

- `THUNK_BIND` now also loads `verify/rwcthunk_20261113/THUNK_BIND.tsv`, keeping a row only when its stub is in the RW band
  (`setdefault`, so the existing 0x0052e3d0 entry is unchanged).
- The closure walk (was `NAMED.get(n) or rva_of(n)`) now consults `THUNK_BIND` first, so a bound stub joins the closure.
- Full regeneration run: `closure 1652: match100 526, provisional 277, hand 16, lift 679, exe 14, missing 103, stop 37`;
  803 TUs, 679 lifted, 140 missing stubs. Diff of generated files: nine `RwCore_<rva>.cpp` change one line each
  (`#define thunk_FUN_004cc820 RwCore_004cc820` -> `RwCore_004cc9e0`, and in `RwCore_004c32b0.cpp`
  `thunk_FUN_004cfa00` -> `RwCore_004c7a60`), `RwCore_Bodies.tsv` gains `0x004c7a60 match100 re/match/s20/004c7a60.c`,
  `exe_sources.rsp` reorders (closure walk order), `RwCore_Lifted_07..10.cpp` redistribute lifted bodies between chunk files
  (same lifted set, chunked in closure order). The ~150 newer sources were part of the full regeneration, not hand-picked.

## 3. Builds and ddkobj

- Default build (`MASHED_NO_DEPLOY=1`): Build OK. `mashed_re.exe` sha256 `ba86dcdc2adde262...` (kept at `D:/MashedWT/RWC-THUNK-BIND-AUDIT_data/mashed_re_default.exe`).
- Object build (`MASHED_DDKOBJ=1`): Build OK. `mashed_re_ddk.exe` sha256 `0e019d9fcfd0834c...`.
- `ddkobj.py check`: **IDENTICAL 1189/1215, DIFFERENT 11, not-in-map 15**, units linked 1165/1441, refusals 4
  (wrapper-redefines-globals=4). DIFFERENT list `0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0
  0x00493710 0x00493900 0x00498810 0x004c0c20 0x004c0f00`: 10 of the 11 addresses of RESULT_RWC-LIFT-EXEC-52E3D0 section 3
  (that run lists 0x0042f530, this run lists 0x0043c5b0 instead. [UNCERTAIN] not investigated: whether 0x0043c5b0 / 0x0042f530 is touched by this change; neither is
  a function in the touched set below). Prior baseline was IDENTICAL 1146/1176, so 43 more units now check.
- Touched rows, all IDENTICAL (`BYTECHECK_touched.tsv`): `_RwCore_004c1980`, `_RwCore_004c78e0`, `_RwCore_004cbca0`,
  `_RwCore_004d3db0`, `_RwCore_004d7c60`, `_RwCore_004d8fa0`, `_RwCore_004c7a60` (new, 16 B), `_Orig_004cc9e0`.
  0x004c5f60, 0x004cd900 and 0x004c32b0 have no row in `BYTECHECK.tsv` (not substituted or not in map in this build) so the
  call-target change in them is not byte-checked by ddkobj. [UNCERTAIN] why: not looked up.
- No ddkobj refusal involved the touched units.

## 4. Knob boots (private root `D:/MashedWT/RWC-THUNK-BIND-AUDIT_root`, test-slot -> mashed_re lock `mashed_lock.py run`)

`run_arms_attach.sh`, vs arms with `KNOB_EXE` = the saved default exe, ddk arms on `mashed_re_ddk.exe`.

| arm | exit | same as RESULT_RWC-LIFT-EXEC-52E3D0 |
|---|---|---|
| vs_off | 0x00000000 after 12.4 s | yes |
| vs_core | 0xFFFFFFFF after 0.5 s | yes (by design) |
| ddk_core | 0xFFFFFFFF after 1.9 s | yes (by design) |
| vs_frame | 0x5257F001 after 0.6 s | yes |
| ddk_frame | 0x5257F001 after 0.5 s | yes |

The core arms are a probe whose stop codes are the expected ones. vs_frame / ddk_frame reach `first frame presented`.

## 5. Self-test: **PASS**

`RwTexDictionaryCreate` binding "self-test PASS" in the core arms (vs_core, ddk_core, vs_frame, ddk_frame) and RWC-DEVICE
"selftest PASS (0 fail)" in vs_frame and ddk_frame.

## 6. Not shown

- No run witnesses that the rebound callers execute in the boot path (no per-function log): only that the boot is unchanged.
- First-frame Im2D compare not re-run (no menu code changed).
- G-CTRL / B not run: default gates untouched by construction (generated files are exe-only, `RVA-LINT: ORIGTOP-ONLY`).

## Raw files (kept out of git, `D:/MashedWT/RWC-THUNK-BIND-AUDIT_data`)

sha256 of each arm log in `RAW_SHA256.txt`.
