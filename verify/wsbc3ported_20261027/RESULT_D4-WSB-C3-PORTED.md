# RESULT D4-WSB-C3-PORTED: RW-Physics C2/ported rows, shadow A/B to C3

Verdict: **GREEN**. 9 rows promoted C2 -> C3 through re-classify. 10 armed rows stay C2 with a
measured reason. 1 EXEC row is not shadow-able, and the 29 NOT-EXEC rows were not armed.
No C4 is claimed.

Branch `orch/D4-WSB-C3-PORTED` off `orch/integration` @ `37383a4bb`. PREREG
`PREREG_D4-WSB-C3-PORTED.md`, committed alone (`d793ef5de`). Addendum 1 (`8d8f1dadb`) and
addendum 2 (`a330e3117`) were each committed before the arm they govern.

## Scope (49 rows)

The census (`verify/wsbcensus_20261026/wsb_census.tsv`, D4-WSB-CENSUS merged) gives 20 EXEC / 29 NOT-EXEC.

- 19 EXEC rows armed. `0x0055bd70` (EXEC) is a `__declspec(naked)` argument-forwarding tail JMP with
  0 stores (`scan_stores.txt`) and no ShadowAB wrapper. Not armed, stays C2.
- 29 NOT-EXEC rows were not armed: no samples in the census scenario, and no samples is not evidence.
  They stay C2 with no tracker edit. List: `0055a9a0 00561280 00565160 00565ef0 00565fa0 00566200
  00566830 0056b9d0 0056bb80 0056be80 0056d350 0056d3f0 0056fb90 0056fea0 00570090 005729a0 005735f0
  00573670 00575b60 00578d90 00579c00 00579d50 00579e50 00579ee0 0057a250 0057a660 0057a9a0 0057adb0
  0057ae20`. Re-pickup: a scenario that reaches them (another track or mode, player driving, race end).

## Source change

- 14 shadow wrappers `Run` / `RunRegion` -> `ShadowAB::RunTracked` (`638c026c9`). The original-body
  store scan (`scan_stores.py`) found pointer stores in 13 return-only rows, and `0055b750`'s declared
  span covered only part of the write set. Unarmed, `RunTracked` calls only `_impl` (`Core/ShadowTrack.h:739`).
- `00578bd0` / `00578cb0`: the wrapper evaluates `NarrowDotOrder()` before `SHADOW_AB_COUNTER`, under
  `#ifndef MASHED_STANDALONE` (.asi only, `a330e3117`, PREREG addendum 2).
- No `_impl` body, knob or default changed.

## Method

The census scenario (`--track 0 --mode 10 --cars 4 --hold 60`) with one hook installed per boot
(`--hooks 0x<rva>`), `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`. Private root
`D:\MashedWT\D4-WSB-C3-PORTED_root` (`MASHED.exe.unpatched` SHA-256 = anchor `BDCAE093...3C0E`),
`mashed_lock.py run --label D4-WSB-C3-PORTED`. Runner `run_shadow.py`, report `re/tools/shadow_ab_report.py`.
Only the PID each launcher printed was killed.

- `.asi` batch 1 (10 rows): SHA-256 `502a72e91c03fee5dc70ec225e3013c30a7e82a8deba7654d3f4b07d679deea6`
  (`638c026c9`).
- `.asi` batch 2 and the re-runs: `3e77c5ee1a38181f4245d2c84f201d9ea6dc032082198b0e5962eabefbbb8e0d`
  (`a330e3117`).
- The shared `original\mashed_re_dev.asi` was not touched.

## Results (`shadow_summary.tsv`, heap counts `heapcount.txt`)

| rva | verdict | n | heap pages (orig, 24 samples) | outcome |
|---|---|---|---|---|
| `0055b750` | CLEAN | 24 | 0 (3 stores land in the caller stack window) | **C3** |
| `0055c2d0` | CLEAN | 24 | 0 | **C3** |
| `00564310` | CLEAN | 24 | 24 | **C3** |
| `005667c0` | CLEAN | 24 | 2 | **C3** (default x87 body) |
| `0056f0a0` | CLEAN | 24 | 160 | **C3** |
| `00575120` | CLEAN | 24 | 24 | **C3** |
| `00578b20` | CLEAN | 24 | 0 | **C3** |
| `00578bd0` | arm 1 DIVERGENT 1/24, arm 2 CLEAN | 24 | 0 | **C3** |
| `00578cb0` | arm 1 DIVERGENT 1/24, arm 2 CLEAN | 24 | 0 | **C3** |
| `005751f0` | DIVERGENT_FLOAT10 | 48/48 | 0 | C2: precision |
| `00577be0` | DIVERGENT | 1/24 | 0 | C2: stack byte |
| `0056f350` | DIVERGENT | 24/24 | 360 | C2: real |
| `00574ad0` | DIVERGENT | 8/24 | 24 | C2: ULP |
| `005752b0` | DIVERGENT | 22/24 | 25 | C2: ULP, inherits |
| `00575560` | DIVERGENT | 21/24 | 25 | C2: ULP, inherits |
| `00576880` | DIVERGENT | 10/24 | 0 | C2: stack |
| `00577cb0` | DIVERGENT | 10/24 | 0 | C2: real (ret) |
| `00578ff0` | DIVERGENT | 2/24 | 24 | C2: real (ret) |
| `0055bd80` | CLEAN | 1 (2 boots) | 0 | C2: game exits |

Every armed row printed `PATCHBYTE ... A/B-IS-REAL` (installed `e9`).

### Divergence detail (rows that stay C2)

- **`005751f0`, precision.** It runs through `Run` (0 stores, the return is the only output). The
  original's float10 return has a 24-bit-precision mantissa (low dword `80000000` / `e0000000` /
  `c0000000`), and the port's is full double precision, 48/48. This is not only a lane limit:
  `00578b20`, also float10, read CLEAN through the same memcmp. Fix shape: an x87/float body like
  HB-6's `FUN_005667c0_x87`, as a default-OFF knob + PREREG.
- **`00574ad0`, ULP class.** One heap float dword per diverging sample differs in the low bits
  (`3bccab90` vs `3bccab70`, `bd187f12 -> bd44248c / bd44248e`), return equal. Arithmetic order or
  precision, the NARROW_DOT_ORDER class. `005752b0` (22/24) and `00575560` (21/24) read the same class
  and call `00574ad0` (directly / through `005752b0`), so fix `00574ad0` first.
- **`0056f350`, real.** On heap pages the original writes nonzero floats at `+0x9a0/+0x9ac/+0x9b8/+0x9c4`
  (e.g. `3d833922`) where the port leaves `0`, and `+0xda4..+0xdb4` differ, 24/24. Composite: the port
  path includes ported callees `0055b750` (C3 here), `0056f0a0` (C3 here), `0056f1f0`, `0056fad0`, and the
  owner is not localized.
- **`00577cb0`, real.** Caller-stack out-param bytes differ (`+0x044`, `+0x080`), and the return
  differs on sample 13. Callee `00577be0` diverges.
- **`00577be0`.** 1/24: 1 byte at stack-window `+0x0c4` (its `out_pt` lives in the caller frame),
  return equal. The stack compare logs no dword values, so ULP vs logic is open.
- **`00576880`.** 10/24. Sample 0 is the TLS init-epoch artifact below. The rest are stack out-param
  bytes (`+0x058`, `+0x09c`). Callees `00577be0` / `00577cb0` diverge.
- **`00578ff0`, real.** 2/24. Sample 1 is the TLS init-epoch artifact. Sample 23: the return differs,
  and the original writes a 23-dword record at `+0x728..` (`+0x728 = 1`, floats) where the port leaves
  the pre-state.
- **`0055bd80`.** 1 CLEAN sample, then the game exited during race load, on 2/2 boots (launcher: "game
  exited while waiting for race running", then "script has been destroyed"). No crash dump was written
  by these PIDs. Next: `re/frida/poll_attach_catch_crash.py` on an armed boot.

### TLS init-epoch artifact (00578bd0, 00578cb0, also seen in 00576880 and 00578ff0)

Arm 1 of `00578bd0` / `00578cb0` diverged on sample 0 only: one port-only heap dword
`80000009 -> 8000000a`, return equal. `NarrowDotOrder()` (`Collision/NarrowFixKnobs.h:16`) is a
function-local static with a lambda initializer. Its first evaluation ran inside the port side of
the tracked window, and MSVC's thread-safe static init bumps the thread's init epoch in TLS, a heap
page. `005667c0` already evaluates its knob in the wrapper and read CLEAN. With the same warm-up added
under `#ifndef MASHED_STANDALONE`, both rows re-ran CLEAN 24/24. `00576880` / `00578ff0` show the same
dword on one sample, but they also carry other diffs, so the warm-up would not change their outcome.

## C3 gate per promoted row (re-classify)

- Reimplementation in `mashedmod/src/mashed_re/Collision/`, `RH_ScopedInstall`, runtime-toggleable
  (hook-only install). Build clean (`=== Build OK ===`, both targets).
- Anti-island: every promoted row has a caller and a callee at C2+, or is a leaf / indirect-dispatch
  (`antiisland.py` -> `antiisland.txt`, from the anchored binary). Leaves (`0055b750 005667c0 00575120`)
  use the leaf exemption: bit-identical on live race inputs, 24/24.
- Uncertainty sweep: 4 unfiled plate markers were filed as U-9793 (`0055b750`), U-9794 (`0055c2d0`),
  U-9795 (`00575120`) and U-9796 (`00578bd0`). Each has `Blocks` = nothing, with the reason. No other
  open row names these RVAs. Stub sweep: 0 `STUB` in the 9 `_impl` bodies.
- `file == exe_file` for all 9 (one TU), so the evidence covers the shipped copy's source. The
  measurement itself is the `.asi` build.
- Island-wide gap, not new here: no RW-Physics C3/C4 plate (0 of 78) carries a `## Purpose` section.
  The plates hold the mechanical description, and the island note is
  `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md`. These 9 follow that precedent.

C4 is owed to the C4 band rows. L-38: `0055b750 0055c2d0 00578b20 00578bd0 00578cb0` compared 0 heap
pages here (their stores land in the caller frame), so they need a scenario that writes heap, or they
stay C3 under L-38. `00564310 005667c0 0056f0a0 00575120` compared more than 0 heap pages. All 9 still
need a canonical racediff.

## Gates

GATES_PLACEHOLDER

## Files

Committed: `PREREG_D4-WSB-C3-PORTED.md`, `scan_stores.py/.txt`, `antiisland.py/.txt`, `run_shadow.py`,
`shadow_summary.tsv`, `heapcount.txt`, this RESULT.

Raw, `D:/MashedWT/D4-WSB-C3-PORTED_data/<rva>[_r2]/` (`shadow_ab.log`, `shadow_report.tsv`,
`shadow_transcript.txt`). SHA-256 of each `shadow_ab.log`:

| file | SHA-256 |
|---|---|
| `0055b750/shadow_ab.log` | `948926c9a3f1fe395abb3459233efc4f52bb102691968f6fa2370fbf0c17ced4` |
| `0055bd80/shadow_ab.log` | `e904402d48fbe443539330206aafe078c207513e658304504f7189354526afe3` |
| `0055bd80_r2/shadow_ab.log` | `e904402d48fbe443539330206aafe078c207513e658304504f7189354526afe3` |
| `0055c2d0/shadow_ab.log` | `d3a91739d7fd1fed8bd2988d203c787204f5eca52496f79c72222455f9a9b28e` |
| `00564310/shadow_ab.log` | `0cba424908e05c99981b22e686cb9a159094a8dbd678705651e32a892d1a5eb1` |
| `005667c0/shadow_ab.log` | `6dae891403ce153af25f499c7504ba55bb7009712aa5cfeebeae979ac0393fbd` |
| `0056f0a0/shadow_ab.log` | `c14e76acb1d980ea08cb2cb8e4d91b2bcaf012478ebe05985f372b79b920ce51` |
| `0056f350/shadow_ab.log` | `7f992bbb73991d27e504905591bb02495600ec8610da6ef45611ece1ed1ee104` |
| `00574ad0/shadow_ab.log` | `f7fa5520d5e93e14779b43b4f3d5396e7df351cd42d1be4dfee4a4c6e078e18f` |
| `00575120/shadow_ab.log` | `02b63fb9f4f1f5ba6e2a511c30c2fb5789bc72c10d5b0157c0620e34528d26a1` |
| `005751f0/shadow_ab.log` | `d80d40d6b96e05ae38b230083dd85cf9b545fc068929ff06042b43f0ed1719e4` |
| `005752b0/shadow_ab.log` | `e28daeaead177159a3ac82f1f7f3aa80929316bb14f378eb8243ef97ff22c1c4` |
| `00575560/shadow_ab.log` | `51af020ff9ae7cb94dd36fd69c3764b5f53531692436df8f430f7d95f95dfa88` |
| `00576880/shadow_ab.log` | `ac81ff3566e88d7fc0e09c0baa6e68852199cce4e1401cfe3f2e42fe0628688a` |
| `00577be0/shadow_ab.log` | `8d1605232f103c3077d3b2686efe90b630ccc6d46846d7588b95d32332625a03` |
| `00577cb0/shadow_ab.log` | `fffcaca14360de35acedf9056b043027b772135990838b4d4fc6b732f7aea470` |
| `00578b20/shadow_ab.log` | `f6f671e3173b103e1a3c9ce98b41eeef5e2c0e21ba994a07e970278952cde97a` |
| `00578bd0/shadow_ab.log` | `3e04a707f860f9e07c9925aca013fc68bed95db68d9d52334cbbe2790ce8f9e6` |
| `00578bd0_r2/shadow_ab.log` | `c0e23214f42cb0db0ed36af9750bddcdf2227162c622a5aa8d764655c7d94c6b` |
| `00578cb0/shadow_ab.log` | `15f9066a5b401828a521268af4e30d6cb33c753413a4f3b0a1d7c813a15f33a6` |
| `00578cb0_r2/shadow_ab.log` | `8e46c495f66f537d2542407d62a50b6b03812d3132538f58e2262ec9aebb3868` |
| `00578ff0/shadow_ab.log` | `d952a870b96cd92c64f7e2f1f10cd6cf4f0f80c594268013ea941b348a72ea12` |

## Next

- Knob + PREREG fix rows: `00574ad0` (ULP, then re-arm `005752b0` / `00575560`) and `005751f0` (x87 body).
- Localize `0056f350`, `00577cb0`, `00578ff0` (real divergences). Add stack dword logging to RunTracked
  for `00577be0` / `00576880`.
- `0055bd80`: crash catch on an armed boot.
- Hand the 9 C3 rows to the C4 band rows.
