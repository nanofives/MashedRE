# Retrofit lane — results

Rules as pre-registered in [`PREREG.md`](PREREG.md), commit `e570a7df`, written
before any edit under `mashedmod/src`. Base HEAD `e8140023`, branch
`race/first-frame-parity`. Nothing pushed.

First use of the standing workflow: **a fix that is a real port must also produce
promotion evidence.**

## Version anchor

`original\MASHED.exe.unpatched`
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` — re-checked
at the start of the effort. Every RVA below was read from that file with capstone
and from a **read-only** Ghidra pool clone (`Mashed_pool0`,
`analyzeHeadless -readOnly`). The Ghidra MCP is not wired into this session, so
`re/tools/decomp_pc.py` was used. `Mashed.gpr` was never opened.

## Binaries

| arm | what | exe SHA-256 | asi SHA-256 |
|---|---|---|---|
| A | pre-retrofit HEAD `e8140023` | `3A1BC0FDF58D6DA914F9B1A34C549846BD09F630FE90F8440EF89DFF89AA7401` | `F5972188D96D27C08FDF11DDFE2C55E95D8E1163E694AEBBAE8951BEBF3BAFF1` |
| B | both ports, `60af8ace` | `6626EEDCE04546449EF757ED82BCC827843254AE3A3480F252CC186D64004337` | `8FAEF2C723BD07FA70961969FEA56EE130D5117442B4332717D20A1EF4096BA7` |
| C | B + the comment-only re-classify edits, `46bf1453` | `658AC5579EEA4758E97847340E6D41ABDB4CBE1424797E385B20C9ED6F8DE578` | `EE38321719A64AFC2C87D8024CB64D3689096D1766B24E9F44AB2CB1808CE8F5` |

`mashedmod\build.bat` from PowerShell, clean on every build. One-RVA-one-body
lint: **122 findings, all allowlisted, NEW = 0** on all three, plus a manual
unanchored-duplicate grep (the lint only sees a body whose comment's FIRST token
is the RVA — memory `rva-lint-misses-unanchored-bodies`). No second copy of the
dedupe epsilon, the pool globals or the grid constants survives anywhere under
`mashedmod/src`.

---

## Target A — `0x00458e00` → `PickupPoolSpawn`, gameplay, **C2 → C3**

Body: `mashedmod/src/mashed_re/Gameplay/PickupPoolSpawn.cpp`, in BOTH
`exe_sources.rsp` and `asi_sources.rsp`, so `exe_file == file`.
`RH_ScopedInstall(PickupPoolSpawn, 0x00458e00)`.
`PickupField::InitReal`'s inline copy of the predicates is gone.

### path1 — **GREEN 6/6**, `log/diff_pickup_pool_spawn.csv`, `scenario=race`

**6 of 6 fingerprints distinct** — non-degenerate, and the accept vectors reach
the **body**, not a guard:

| # | vector | ret | observed |
|---|---|---|---|
| 0 | empty pool, type 9 | **0** | count 0→1; `+0x18`=0x42f00000, `+0x1c`=0, `+0x20`=1, `+0x24`=9, `+0x28`=0; position at **both** `+0x2c` and `+0x38` |
| 1 | type 0x15 (BLANK) | -1 | count 0, but `+0x24`=0x15 and `+0x28`=0 **are** written — `0x00458e7e`/`0x00458e85` run BEFORE the type test at `0x00458e96` |
| 2 | dedupe exact hit | -1 | count 1, entry 1 untouched |
| 3 | dedupe 1.02 vs 1.00, d² = 0.00039999924 | -1 | rejects |
| 4 | dedupe 1.05 vs 1.00, d² = 0.0024999952 | **1** | accepts into entry 1 |
| 5 | count seeded 25 | -1 | count 0x19, nothing written |

Vectors 3 and 4 straddle `_DAT_005cc558` = 0.0010000000474974513 by ~2.5× either
way, so no float-model difference can flip them.

### path2 — **PASS**, `log/verify_hook_install_pickup_pool_spawn.txt`

opcode `0xE9`, rel32 matches, interceptor **4/4**, 4/4 calls returned, 0 FAIL.
**Control run FIRST** (memory `verifier-needs-passing-baseline`):
`vehicle_vec3_at_6e4_set`, an existing `cache_setter_observe` row, re-runs PASS
on this build, so the template edit is not what the result is measuring.

### Canonical scenario, hook LIVE

Arctic, Challenge Cup entry 3, unlocked save **copy**; `original/gamesave.bin`
restored and sha-verified `bd18788182b2343e` after every run. Install witness
read **in process** at capture time: `[0x00458e00] = 0xa4da6be9` → `e9 6b da a4`,
and `[0x00448940]` stock, so `MASHED_HOOK_ONLY` isolated the one hook.

Live pool vs **three** independent stock controls, over the 8 fields this
function writes (`+0x18 +0x1c +0x20 +0x24 +0x28` and the anchor `+0x2c..+0x34`):
**0 of 25 records differ**, stable over 6 samples in all four arms. The live
position `+0x38..+0x40` and the angle `+0x04` animate per frame and `+0x48`/
`+0x4c` are heap pointers, so none of those is comparable across processes and
none is compared.

### In-game no-regression (A-P1 / A-P2 / A-P3)

- **A-P1 PASS** — TRAINING **5/5** and ARCTIC **7/7**, bit-identical float32
  positions, types and respawns against the live original reads, in pool order.
  Re-passes on arm C.
- **A-P2 MATCH** — keep set AND drop set on **all 13 tracks**.
- **A-P3** — see the shared pixel result below.

---

## Target B — `0x00448940` → `ArcticTrackNodeSlot0`, render, **C0 → C3**

There was **no `hooks.csv` row** and the function **was not defined in Ghidra**
(`decomp_pc.py --create` inside a `-readOnly` run). It climbed C1 and C2 in this
lane, with the plate `re/analysis/track_nodes_arctic/0x00448940.md` written first.

The **whole 880-byte body** (`0x00448940..0x00448caf`, RET at `0x00448caf`) is
ported, not just the grid. `TrackRenderer`'s inline 5×5 loop now calls the same
`ArcticSeaTileGrid` the node's own translate loop calls.

### Caller gate, read from the bytes

```
0x0041e8b0  8b 0d e4 d7 63 00   mov ecx,dword ptr [0x0063d7e4]
0x0041e8b6  ff 61 14            jmp dword ptr [ecx + 0x14]
```
`+0x14` is `slot[0]`, and the `"arctic"` record's slot 0 at `0x005f349c` is
`0x00448940`. So `TrackNodeDispatch14` (**C3**) is the caller of a function
Ghidra reports with zero callers.

### path1 — **BLOCKED**, exactly as pre-registered (B-B1)

A path1 A/B calls the function **twice** in one process. It clones 24 `RpClump`s
(`0x00448a47`), re-parents frames (`RwFrameRemoveChild` `0x004e45b0`) and pushes
into `0x008962e0` under the global cursor `DAT_0068324c`. An RW allocation cannot
be save/restored, so no safe A/B exists. **No A/B was faked, and the path2-class
result below is not reported as path1.** `hooks.csv`'s `frida_diff` cell reads
`BLOCKED-see-notes`.

### Run instead — canonical scenario, hook LIVE, which is stronger than path1

Install witness read **in process** at capture time:

| arm | `[0x00448940]` | bytes | meaning |
|---|---|---|---|
| hook ON | `0xa5e05be9` | `e9 5b e0 a5` | **JMP installed** |
| hook OFF | `0x5324ec83` | `83 ec 24 53` | stock `sub esp,0x24 ; push ebx` |
| both | `[0x00458e00] = 0x702ae855` | `55 e8 2a 70` | stock — the one hook really was isolated |

Result, hook-ON vs hook-OFF on Arctic:

- **25 live sea-tile clumps**; their frame **MODELLING** (`frame+0x40`) and
  **LTM** (`frame+0x80`) translations are **element-wise identical** between the
  arms and equal to the predicted grid **in order** — tile 0
  `(-150, -4.0999999, -150)`, tile 24 `(90, -4.0999999, 90)` — stable across 6
  samples in each arm.
- `0x008962c0..0x008962cc` identical (`0x38d20000`, `0x3ee76c8b`, `0xb8d20000`,
  `0x4229e016`); cursors `0x0068324c` = 49 and `0x00683248` = 48 identical;
  camera-invariant draw totals identical (**115** calls, **43613** prims,
  **28406** verts).

### The pixel channel is VOID for that capture and is NOT used

The **OFF-vs-OFF control** differs by **204503 of 307200** pixels — this probe
does not pin the pose, so the channel cannot see the fix. The ON-vs-OFF number
(260419) is therefore meaningless and is recorded only so nobody later mistakes
it for a finding (memory `drawstream-ab-channel-gotchas`).

### In-game no-regression (B-P1 / B-P2)

- **B-P1 PASS** — 25 tiles, X and Z each in {-150, -90, -30, 30, 90}, Y =
  `-4.100000` on all 25, **exact and in order**, element-wise equal to the
  pre-retrofit baseline. Both boots, and again on arm C.
- **B-P2** — the `SEA-TILE-OFF` negative prints on the **same 8** non-Arctic
  tracks as the baseline. The other 4 (Neustein, Storm, sands, training) never
  reach that code with a clump 2, on **both** arms — pre-existing, unchanged, and
  reported as the measured 8 rather than the 12 the rule assumed.

### Two Ghidra arities were wrong

Ghidra prints `FUN_004671a0(0,0x44480000)` and `FUN_004c1b10(uVar1)`. The
prologues say otherwise — `0x004671a0` reads ONE stack argument
(`cmp dword ptr [esp+4],-1`), `0x004c1b10` reads TWO (`fld [esp+8]` →
`[esi+0x84]`) — and the call site cleans all 8 pushes at once (`add esp,4` at
`0x00448c7d` plus `add esp,0x1c` at `0x00448ca9` = 32 bytes), which is what let
the decompiler mis-attribute a slot. Porting the decompiler's version would have
pushed 800.0f into the wrong callee.

---

## Shared: pixel no-regression vs the pre-retrofit build

Driver `verify/pickups_fix_20261001/run_race.py` — the standalone's own race-flow
driver, `MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_DETERMINISTIC=1`, muted,
`MASHED_WIN_POS=left-bl`, never `MASHED_NAV_DEMO`, PIDs tracked and only those
killed.

| comparison | pairs | differing |
|---|---:|---:|
| A (pre-retrofit) vs B (ported), 13 tracks × 5 frames | 65 | **0** |
| A vs B, second boot, tracks 0 + 12 | 10 | **0** |
| B boot1 vs B boot2 (self-reproducibility) | 10 | **0** |
| C (final) boot2 vs B boot3 | 10 | **0** |

**One caveat, run down rather than waved away.** `C_r1` differed from `B_r1` on
two **Arctic menu** frames (`00_challengeselect` 45803 px, `02_back_to_menu`
44580 px); the three in-race frames were identical. A second boot of the **same**
build reproduces the same two frames with the **same counts** against the first
(`C_r1` vs `C_r2`), and `C_r2` vs a third B boot is **0**. So the Arctic
challenge-select screen is **bistable between boots** and `C_r1` landed in the
other state. It is not a retrofit effect — one build produced both states.
`A_r1` vs `A_r2` and `B_r1` vs `B_r2`/`B_r3` are all 0, which is why it did not
surface earlier.

---

## Harness changes (additive, defaulted off)

- `cache_setter_observe` gains `CONFIG.fold_ret`, in **both** `diff_template.js`
  and `verify_hook_install_template.js`. Without it the observable is the written
  globals only, so a port that returned the wrong pool **index** on every accept
  would still compare GREEN — and the index is this function's output. The 14
  existing users set neither and are unaffected (control re-run above).
- `run_verify_hook.py`'s config builder now forwards `fold_ret` and
  `obs_globals`. `run_diff.py` already forwarded `fold_ret`; the two builders are
  separate whitelists, the same trap that file's own `arg_layout` / `stub_at`
  comments record.
- `pickup_pos_probe2.py` gains `--hooks NAME[,NAME...]` (it hardcoded
  `MASHED_RE_NO_AUTO_HOOK=1`, so the same capture could not be taken with a hook
  live) and now dumps the 25 sea-tile clumps with their frame translations.
- `re/frida/ARG_TYPES.md` regenerated.

## Trackers (one `re-classify` transaction, `46bf1453`)

Census **C2 3891 → 3890, C3 1011 → 1013**.

- `hooks.csv`: `0x00458e00 PickupPoolSpawn gameplay C3 impl`, `frida_diff =
  log/diff_pickup_pool_spawn.csv:GREEN-6/6-NONDEGEN`; **new row**
  `0x00448940 ArcticTrackNodeSlot0 render C3 impl`, `frida_diff =
  BLOCKED-see-notes`. Both carry `exe_file == file`.
- `UNCERTAINTIES.md`: **U-8325 RESOLVED**; **U-9163..U-9169** filed, each with a
  non-empty `Blocks` cell and a stated reason (D0.3: `Blocks` decides, `Type` is
  a label).
- `STUBS.md`: **S-5713..S-5716**, each with its inline `// STUB S-NNNN` marker in
  the source.
- `re/analysis/CHANGELOG.md`: two entries, inserted by **exact-line** match on
  `<!-- ENTRIES -->` (the header quotes the marker in prose; a substring search
  lands in the header — memory `changelog-marker-exact-match`). Line count
  1018 → 1020, delta exactly +2, nothing rewritten.

## C4 is NOT claimed, for either RVA

`re/CONFIDENCE.md` L37 words C4 as a clean CSV diff produced by the
`diff-original` skill, and `run_diff.py` is **hook-bypassed by construction**, so
that artifact does not exist here. The canonical-scenario evidence above is
written into both plates for a later C4 ruling rather than used to self-grant
one. Two uncertainties block C4 independently of the wording:

- **U-9167** — `0x00448940`'s tail branch runs only when
  `course+0x105f8 == course+0x105fc`. A state comparison cannot tell "ran and
  matched" from "both arms skipped it", and no coverage counter was armed.
- **U-9168 / U-9169** — `0x00458e00`'s rank-2 arm has a **random** callee
  (`FUN_00472690(0,8)` at `0x00458d47`), so it can never be in a bit-identity
  A/B; and the standalone copy stubs `FUN_00458dd0` + `FUN_004c15c0` (S-5714 /
  S-5715), which C4 forbids.

## Open / still owed

- Stage 2 of the pickups defect (the LOOK) is untouched — see `ROADMAP.md` and
  `re/NEXT_SESSION.md`.
- U1b from the sea lane (the dedicated Arctic sea pass `0x00449030` and its eight
  `RwGlobals+0x20` state pairs) is still open. **U2 is now partly answered**: the
  `0x004e45b0` call at `0x00448a1f` is a single
  `RwFrameRemoveChild(course+0x105d4, tiles[0])` — a detach of the base clump
  **before** cloning, not a per-clone world registration.
- The collection radius divergence (`worldR_ * 0.04f` vs the original's 0.5
  sphere at `0x00459228`) is unchanged and still open.
