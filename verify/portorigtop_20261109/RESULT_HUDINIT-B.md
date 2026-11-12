# RESULT HUDINIT-B (run 38, 2026-11-11): GREEN on the restack onto orch/integration bf44aae7d: 0x00427ca0 runs its matched body under RWMOD and 0x00556e40 runs on four non-zero brush handles

L2 worker `cmwb4f8nh18ymsc1bf03upqca` for lead `cmw7xu1960f37p41557tefpnd`, L-111 step 1, chain 2 of the PORT-OT-HUDINIT split.

Branch `orch/HUDINIT-B` on `orch/L115-HUDINIT-PROOFS` (`be02dffdd`), which sits on `orch/HUDINIT-A` (`d4ee8a938`, on
orch/integration `5fc6a43e4`).
- The code was built and gated once, on the pre-restack stack: HUDINIT-A `f70353697`, proofs `b2e5c796e`, B `8e415cc63`.
  Data `D:/MashedWT/HUDINIT-B_data`, exe `15c08538`.
- The restack onto the rebased HUDINIT-A had no conflict. BINDINGS.tsv was regenerated on it (below).
- Lead instruction: no rebuild or gate of the restacked branch until HUDINIT-A has landed. So the acceptance table below is
  the pre-restack run, and the re-gate is listed under "Left".

## Verdict

With `MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1`, the 0x00427ca0 call of the matched 0x00402750 runs the **matched body of
0x00427ca0** (re/match/s06/00427ca0.c through Match/MatchS06_00427ca0.cpp), whose exe callees now bind by name.
`knob_on_rwmod.log` (pre-restack):
- :245 `rva 0x00427ca0: RWMOD on: matched body (HUDINIT-B)`
- :246-256 its first call, the Rt2d open 0x00552b60(0), as in HUDINIT-A (five inits return 1, two stand-ins)
- :257 `0x004cc230(2, 1, "fgdc20.txd"): 132232 bytes from piz`: 0x0042a6b0 loads the font dictionary
- :259 / :260 the 0x00427880 and 0x00556e90 stand-ins log their first call
- :261 `rva 0x00427ca0 ran: font 0x0067d838=00000000 brushes 0x0067d83c=08EA5070 0x0067d840=08EA4BB0 0x0067d844=08EA4E10 0x0067d848=08EA4CE0, dictionary 0x00912a20=08F4DB98`
- :895 at AppDestroy, `rva 0x00556e40(0x08EA4E10): matched body (RWMOD on, handle set)`
- :896 `RwTexDictionaryDestroy(0x08F4DB98)`: the matched 0x00427620 destroys the fgdc20 dictionary through librw, so it has
  one owner
- :1060 `WinMain 0x00492370 returned 0 after 3000 frames`

**0x00556e40 runs on all four handles.** The selector `PortTop_00427620_Call00556e40` has two outcomes. With RWMOD on and
a non-zero handle it runs the matched body; otherwise it runs the stand-in `PortTop_StandIn04C_00556e40`, which logs its
first call (`[PORT-TOP] rva 0x00556e40: stand-in, not run`). The selector logs only its first call per path, so :895 is
the one line for the matched path. The stand-in line is absent from the whole log, and all four handles are non-zero
(:261). So all four calls took the matched body. Each body run frees its brush through RwGlobals +0x11c onto DAT_00912a00,
because its count at +0x74 is 1 (set by 0x00556d70).

## What changed

| rva | name | exe body | source / kind | evidence |
|---|---|---|---|---|
| 0x00427ca0 | FontText_HudInit | `FUN_00427ca0` (Match/MatchS06_00427ca0.cpp), via the selector | re/match/s06/00427ca0.c | 100%, relocheck OK (both targets already compiled it; the exe branch bound raw addresses before) |
| 0x00556d70 | brush create | `Orig_00556d70` | re/match/rwl05/00556d70.c | 195 B BYTE-IDENTICAL, relocheck OK 5/5 |
| 0x00557110 | brush UVs | `Orig_00557110` | re/match/s24/00557110.c | 169 B BYTE-IDENTICAL, relocheck OK 4/4 (the 96.5% in the source header is stale: match.py gives 100% on this tree) |
| 0x00556cc0 | SetDat00912a20 | `Orig_00556cc0` | re/match/s24/00556cc0.c | 10 B BYTE-IDENTICAL, relocheck OK 1/1 |
| 0x00556e90 | brush colours | `PortTop_StandInHIB_00556e90` | stand-in | 97.5%, below |
| 0x00427880 | font-file loader | `PortTop_StandInHIB_00427880` | stand-in (scope: chain C) | 100% source, but its callee 0x00554390 is 33.5% |

The exe branch of Match/MatchS06_00427ca0.cpp used raw-address forwarders for 0x00552b60 / 0x00556d70 / 0x00556e90. It
now binds by name:
- 0x00552b60 -> `PortTop_FontSysInit_00552b60` (HUDINIT-A)
- 0x00556cc0 -> `Orig_00556cc0`
- 0x00556d70 -> `Orig_00556d70`
- 0x00556e90 and 0x00427880 -> their stand-ins
- 0x0042a6b0 -> the exe body `FUN_0042a6b0` (Match/MatchS06_0042a6b0.cpp, PORT-TOP-04G)

The .asi branch is unchanged. No re/match source was edited.

**0x00556cc0, the matched copy over the hand body.** The exe already has the hand `SetDat00912a20` (Frontend/BatchAA_s3.cpp,
both targets). The matched copy runs on the knob path instead, so no hand body runs there (L0 ruling). It is
`ORIGTOP-ONLY DUP-IN-TARGET`, as POWERUPINIT-A's 0x004b3b70 was.

**The selector** (Match/PortTop_00552b60.cpp, `PortTop_00402750_Call00427ca0`):
- RWMOD on: `FUN_00427ca0()`, then a log of the five handles and 0x00912a20.
- RWMOD off: `PortTop_StandIn04E_00427ca0`.
- The body allocates through RwGlobals +0x118, which is 0 without RWMOD.

### L-115 order for the two 9x% brush rows

1. **0x00557110:** byte match, so it runs.
2. **0x00556e90:** no byte match (35 differing bytes), and the residual is not the SIB class. The differences are integer
   moves (`mov edx, [esp+k]`, `mov esi, [esp+0x18]`, `xor edx, edx`, the `[ecx+0x54]` / `[ecx+0x60]` stores) scheduled
   across x87 instructions at 0x00556f47..0x00556f4d, 0x00556f90..0x00556f9d, 0x0055700a..0x00557010 and
   0x0055701c..0x00557025 (`l115_bytediff.py re/match/s24/00556e90.c`). So it runs a logging stand-in.

   What the stand-in cuts: each brush's RGBAReal colours (+0x00 / +0x18 / +0x30 / +0x48), the RGBA8 copy at +0x60 and
   +0x64 bit 0 stay as 0x00556d70 zeroed them. No reader on the knob path: the font draws are chain C.

### Owner of the shared state

- **0x0067d828..0x0067d848:** the matched 0x00427ca0 is the only exe writer. The reader is the matched 0x00427620
  (AppDestroy). The exe's hand menu measure uses its own cells (`g_menuBa_FontCell` / `g_menuBa_DivCell`,
  `exe_main.cpp:3575`).
- **0x00912a20:** written by `Orig_00556cc0`, destroyed by the matched 0x00427620 through librw (:896).
- **The brush entries:** allocated from the Rt2d brush list DAT_00912a00 (HUDINIT-A) and freed by 0x00556e40.

## Acceptance (pre-restack run, exe `15c08538c19906f783775431a46e4d82b27db79a25d1aa4d862d74c5450ad312`)

| check | result |
|---|---|
| syntax | `cl /Zs ... /DMASHED_STANDALONE` on the 4 new and 3 edited TUs (MatchS06_00427ca0, PortTop_00552b60, PortTop_00402750): no error |
| build | `=== Build OK ===`, gated on the lock files as in HUDINIT-A |
| knob-ON boot, RWMOD off | `exited rc=0 (0x00000000) after 78.2 s`. :245 `RWMOD off: stand-in only` |
| knob-ON boot, RWMOD on | `exited rc=0 (0x00000000) after 52.7 s`, lines in the Verdict |
| 12 captures, RWMOD off | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. ON logs: stand-in only, and 0x00556e40 on the stand-in path |
| 12 captures, RWMOD on | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. Both ON logs carry `rva 0x00427ca0 ran` with four non-zero brushes, `0x00556e40(...): matched body`, no 0x00556e40 stand-in line, `returned 0 after 578 frames` |
| walk_04 | `ba34030d1b7e2fe4` in all 8 runs |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 |
| rva-lint (restacked tree) | `OK -- no NEW duplicate bodies (40 known)`. New ORIGTOP-ONLY lines: 0x00556cc0 (DUP-IN-TARGET vs Frontend/BatchAA_s3.cpp), 0x00557110 (CROSS-TARGET vs Util/PromoLoop_sessionB.cpp) |
| seed (restacked tree) | `--check` OK, 271 entries (no new seed: 0x00427ca0's strings are exe literals, 0x00556d70 reads only DAT_00912a00 and RwGlobals) |

## BINDINGS and the port column

`verify/portorigtop_20261109/update_bindings_hudinit_b.py`:
- Base chain: `update_bindings_hudinit_a.py`.
- On the restacked tree, from HUDINIT-A's BINDINGS.tsv: `--check-base` gives `base chain reproduces BINDINGS.tsv byte for
  byte: True`. It is idempotent (a second run leaves the file byte-identical).
- 6 FIXED rows and 3 APPEND notes: 520 rows (516, +4: 0x00556cc0, 0x00557110, 0x00556e90, 0x00427880).
  - matched 253 -> 257: 0x00427ca0 from stand-in, 0x00556d70 from `rw`, 0x00556cc0, 0x00557110.
  - stand-in 67 -> 68: -0x00427ca0, +0x00556e90, +0x00427880.
  - rw 29 -> 28.

Port column (pre-restack build): 4 rows changed. 0x00427ca0 and 0x00556cc0 go body -> called, and 0x00556d70 and
0x00557110 go none -> called. It is regenerated again with the post-landing build.

## Left

- Re-gate on the restacked tree once HUDINIT-A lands, per the lead: rebuild, both boots, the 12 captures RWMOD off and
  on, G-CTRL / B, then the port column on that build.
- 0x00556e90: stand-in until a byte match or an accepted L-115 class (move scheduling across x87 instructions).
- 0x00427880, the font read and 0x00553e80 are HUDINIT-C.

## Raw files (D:/MashedWT/HUDINIT-B_data, pre-restack, not committed)

| file | sha256 |
|---|---|
| build.log | 76cc0bf81b2c743ab062f4a950f2c830f069b8af03af94e3b75516d0bc2285e8 |
| knob_on_1.log | ccc21796bb04127873c6e8e1203a003a99867c10087832048a695190eceaf186 |
| knob_on_rwmod.log | e71d42ba097b5db7edf76fd346661e0411f75fd6490e55614c4632ae5d61f4d6 |
| runs.tsv | f560646e9825f27b38517ec0c4d851cd17931fc8944763b7fbaa1d1204796438 |
| cap_nav1.out | 398f20373086aff6e15944f215d91f11c60cb295a11b3a13a2ae1178facc7fdf |
| cap_rwmod.out | a8a38ad2934f4861afc973c57a8a6bbb8908f7f49a7c6c1d351db0bc3422b8e4 |
| nav1/r01..r04 | c3207069... / d1db8cc3... / b70e64b7... / f4f41380... |
| nav_rwmod/r01..r04 | e83bb292... / 10887a01... / 8d4aeda2... / 6fd8afe6... |
| walk_04_colourselect.bmp x8 | ba34030d1b7e2fe4... (all equal) |
| gates_run.log | bf3fe6addf27d750f0643270c425524f32d829ea98297d57d4b46586228fe57e |
| arms/H_DEF step / gates / ptrace | 8284be46b3c692f3... / 6793183492e0c082... / 31fe043e261d2df9... |
| arms/B_DEF_K0 / K1 / K26 step | 1d5bf4f980bf7eb4... (all three) |

## Run 38 restack and re-gate (worker on claude3)

Branch `orch/HUDINIT-B` was recreated on `orch/integration` `bf44aae7d` (HUDINIT-A, RWC-BIND-4C5890 and L115-HUDINIT-PROOFS are all merged there). The old branch is kept as `orch/HUDINIT-B-old`. The two HUDINIT-B commits (`cc43f4bd9`, `cda774328`) were cherry-picked. One conflict, `re/tools/dual_copy_allowlist.txt` (both sides append lines): both sets kept. No librw binding for 0x004c5890 is added (L-129).

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`, VS2022) | `=== Build OK ===`. rva-lint OK, no NEW duplicate bodies (95 known). asm-lint OK |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===` |
| `ddkobj.py check` | IDENTICAL 712, DIFFERENT 80, not-in-map 3. The DIFFERENT set is the same 80-row set RWC-BIND-4C5890 reported on the rebase. 0x00427ca0, 0x00556cc0, 0x00556d70, 0x00557110 and 0x00556e40 are all IDENTICAL |
| knob boot, VS2022 exe, RWMOD off | rc 0 after 46.4 s, 3000 frames. `RWMOD off: stand-in only`, no matched-body line |
| knob boot, VS2022 exe, RWMOD on | rc 0 after 79.7 s, 3000 frames. `RWMOD on: matched body`, `rva 0x00427ca0 ran` with brushes 0x0067d83c=08ED55A0 0x0067d840=08ED5800 0x0067d844=08ED62B0 0x0067d848=08ED6180 (all non-zero), `0x00556e40(0x08ED62B0): matched body (RWMOD on, handle set)`, no 0x00556e40 stand-in line, `returned 0 after 3000 frames` |
| knob boot, object exe, RWMOD off / on | rc 0 after 38.2 s / 32.5 s. Same lines as the VS2022 exe. Brushes 090C8AF0 090C7F10 090C8170 090C8A60 non-zero, 0x00556e40 matched body |
| self-test | The knob boot's own checks (seed 39/39, bring-up returns) pass in all four logs. No separate self-test run |
| not run | G-CTRL, B, 12 captures x 4, port column on the restacked build (top-down row, WORKER_RULES). The pre-restack run above carries those, unchanged code |

Remaining stand-ins: 0x00556e90 (97.5%, residual outside L-115) and 0x00427880 (chain C), as in the Verdict.

Raw files (D:/MashedWT/HUDINIT-B_data2, not committed), SHA-256:

| file | sha256 |
|---|---|
| vs_rwmod_off.log | 851F43F793E4EC7CCE828CBFDB8B374910AA307D6FAA62FB4D75E1475EC07D3E |
| vs_rwmod_on.log | 055F7D0ED17B3B294D192FEC9FE17DA4EEC83622C6615D59CDD42D69B1422687 |
| ddk_rwmod_off.log | B666888AE554F5BB5E27517B10808C4979A8D578FA3FD87B3695DF2E12121EF2 |
| ddk_rwmod_on.log | 4308983AD66104E5B63C0C6440B5A9F9150B40E5866E29D2916C6F8A20A03697 |
| mashed_re_vs.exe | FC2E8D32F940B0FC5870C15153A1699288FBDCBF019F21DD447465D394A6D1EC |
| mashed_re_ddk.exe | A208FD18A8F269A8F61C3822A138719849E8C29E7BDDD41D8BD2C6DC251686A4 |
| BYTECHECK.tsv | 2C697F3CAEF07078E01D0440C999798B8FD94F82385C6484D92E5B0421C3001C |
| build.log / build_ddkobj.log | FD2C1D3A... / 6C92F4DB... |