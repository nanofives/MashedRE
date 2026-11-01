# RESULT HB-SHADOW-ALLOC-COMPARE: GREEN, allocator-aware shadow compare, 5/5 allocating rows CLEAN

PREREG `PREREG_HB-SHADOW-ALLOC-COMPARE.md` (commit `035ba2657`, amendment 1 `8039e3941`). Harness change in
`mashedmod/src/mashed_re/Core/ShadowTrack.h` (`e9e70ed0e`, residue pin `5b688e76e`), opt-in `MASHED_SHADOW_ALLOC=1`,
default off. Read-out is on pass 2 (build of `5b688e76e`).

## Verdict

| RVA | before (allocator compare off) | pass 2, `MASHED_SHADOW_ALLOC=1` | allocs/sample | heap pages (A) | masked dwords | pin extra / miss |
|---|---|---|---|---|---|---|
| `0x0055ded0` | DIVERGENT 23/24 (BROAD-2) | **CLEAN 24/24** | 1 | 3-4 | 6 | 12 / 0 |
| `0x0055dca0` | DIVERGENT 1/1 (BROAD-2) | **CLEAN 1/1**, `HOLD=180` re-run CLEAN 1/1 | 7 | 30, 31 | 162, 166 | 2 / 0, 3 / 0 |
| `0x0055dc70` | DIVERGENT 1/1 (BROAD-2) | **CLEAN 1/1**, `HOLD=180` re-run CLEAN 1/1 | 8 | 30, 31 | 162, 162 | 1 / 0, 0 / 0 |
| `0x0057c300` | DIVERGENT 24/24 (NARROW-TAIL) | **CLEAN 24/24** | 2 | 6-14 | 12-206 | 14 / 0 |
| `0x0057c500` | DIVERGENT 22/24 (NARROW-TAIL) | **CLEAN 24/24** (re-run after a launch failure) | 2 | 3-12 | 12-168 | 18 / 0 |

Every sample: `PATCHBYTE installed=e9` with the row's uninstalled byte (`A/B-IS-REAL`), `mm:0` (same allocator calls,
same order, same arguments on both sides), `movf:0`, `blk:0`, heap pages compared above 0 (CONFIDENCE.md heap-writer
leg). `0x0055dca0` and `0x0055dc70` are called once per race load in this scenario: the `HOLD=180` re-run the PREREG
asks for gave 1 sample again, so each has 2 CLEAN samples from 2 boots.

## Harness controls (pass 2)

| arm | row | expected | result |
|---|---|---|---|
| C0 | `0x0055ddd0` (no allocation) | CLEAN, `alloc=n:0` | CLEAN 1/1, `HOLD=180` CLEAN 1/1, `alloc=n:0` |
| K0 | `0x0055ded0`, knob off | DIVERGENT | DIVERGENT 20/24 (sample 0: `+0x4f8` A `0f4df2e8` / B `0f4df378`, the block address) |
| M1 | `0x0055ded0`, `MUT=1` (port's first block dword +1) | DIVERGENT every `mut:1` sample | DIVERGENT 24/24, `blk:1` each (`blk@...:00000000/00000001`) |
| M2 | `0x0055ded0`, `MUT=2` (stored block pointer +4) | DIVERGENT every `mut:2` sample | DIVERGENT 24/24, page diff each (`+0x600` `0f4ebe08` / `0f4ebe0c`) |
| G | default gates, exe `28ad585e` | `merge_train.json` refs | equal, table below |

M1 shows the block-vs-block compare catches a wrong value inside a paired block; M2 shows the offset rule rejects a
pointer into the paired block at the wrong offset. Both mutations are reverted after the compare.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + solo lock,
`A4_ROOT=D:/MashedWT/HB-SHADOW-ALLOC-COMPARE_root`, `mashed_re.exe` `28ad585e` (the header is compiled into the exe):

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv | `8284be46` | `8284be46` |
| H_DEF | gates.csv | `ba407a16` | `ba407a16` |
| H_DEF | ptrace.log | `31fe043e` | `31fe043e` |
| B_DEF_K0 | step.csv | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K1 | step.csv | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K26 | step.csv | `1d5bf4f9` | `1d5bf4f9` |

## What the compare does (and what it does not)

During one compare the RwGlobals (`DAT_007d3ff8`) slots `+0x108` `(size, hint)` and `+0x114` `(n, size, hint)` point
at recording wrappers (`+0x10c` free is read, not swapped). Allocations from the tracked thread inside a window are
logged, and every dword the allocator wrote outside the block it returned is masked. The k-th allocation of the
original's run is paired with the k-th of the port's (count, slot, arguments must agree or the sample counts a
difference). Paired blocks are compared block against block, pointers into paired blocks compare by offset, and a
block dword neither run changed from its pre-allocation value passes. In the port's run the wrapper re-asks the
allocator until the block address mod 16 equals the original's, then frees the misses.

Limits (in the header): a dword the body writes inside the mask is not compared. The masks are small next to the
memory compared (at most 206 dwords against 6-31 tracked pages of 1024 dwords each), and they come only from writes
made while the allocator call was on the stack. A block dword the original writes with the exact stale value of its
own block while the port leaves its own stale value would pass. Allocations by any other route are not paired.

## Pass 1 (superseded, kept as the measurement that found the padding)

Build `.asi fdb0e4d1` (no residue pin): `0x0055ded0` CLEAN 24/24, `0x0055dca0` CLEAN 1/1, `0x0055dc70` CLEAN 1/1,
`0x0057c300` CLEAN 24/24, `0x0057c500` DIVERGENT 7/24. C0 CLEAN 1/1, K0 DIVERGENT 19/24, M1 and M2 DIVERGENT 24/24
(`shadow_summary_pass1.tsv`). Each `0x0057c500` divergence was 16 dwords of the `0x54` block, all pointers into the
`0x70` block at `+0x8` on one side and `+0x10` on the other. That block comes from `0x00564130` (run as the original on
both sides through the port's raw-RVA thunk), which returns `raw + (align - raw % align)`, pad = align when the
remainder is 0 (`L2_00564130.cpp`, `0x00564170..0x00564179`). The offset depends on the block address, so amendment 1
pins the residue.

## Run log

- Audio endpoints (VB-CABLE + Philips FTV) present before and after every launch, 0 MASHED Event-1000 crash events, no
  fault at `0x005baf48`, no U-9677 signature.
- `0x0057c500` pass 2 first launch: `error: could not attach` (Frida), no shadow log, PID 4928 gone afterwards. A launch
  failure, re-run once on the same build: CLEAN 24/24.
- The first G attempt died after H_DEF in `run_arms.py` copying into a missing `A4_OUT` folder (the runner does not
  create it). Folder created, G re-run in full. An earlier G launch was stopped before it started (exe was being
  rebuilt for amendment 1), and one before that failed on shell quoting without launching anything.
- The machine lock and test slots were shared with D4, MATCH and PROMOTE-DETORIG rows. Each shadow row ran in its own
  solo window.

## Files

Committed: PREREG, this RESULT, `shadow_summary.tsv` (pass 2), `shadow_summary_pass1.tsv`, `readout_sa.py`, runner
`run_shadow11.py`, driver `drive_sa.py`, `run_arms_sa.sh`, `run_pass2.sh`, `run_rerun.sh`, `run_gates.sh`.

Raw (`D:/MashedWT/HB-SHADOW-ALLOC-COMPARE_data/`), SHA-256:

| file | sha256 |
|---|---|
| `0055dc70/shadow_ab.log` | `05c7f611d3ca5176337669e793958f614696bdddf36c94194b55817d33138cbe` |
| `0055dc70_h180/shadow_ab.log` | `d71fe2da9df8bb0d6771c49588e79e11f0b50e96d9e4c03a18c7bd2136d73872` |
| `0055dca0/shadow_ab.log` | `3b497a0fbac474ab73e1fcfb6213b76f1d18bc7b741b515938791a9797c4ca31` |
| `0055dca0_h180/shadow_ab.log` | `d074b022cd3885058c5dfefd7e4037f0800c5950575de1015b923336c7585f66` |
| `0055ddd0_c0/shadow_ab.log` | `d29eaaa7ab55878aaa7d15d02eece512fe2456bad89d2eeba8dc69118fbd849d` |
| `0055ddd0_c0_h180/shadow_ab.log` | `1cfd871562ef61f8b4cb69560abe5f3a8ee6b3b18e2e591a83a65f0d97d1d49c` |
| `0055ded0/shadow_ab.log` | `b1ed4f5e2eeb46fab1e7a59502b00cffde95ec3f303b4447ed4afb152917633b` |
| `0055ded0_m1/shadow_ab.log` | `05a064f8f46365e7257c422d821b752abaacdbcb9591b5dd8dfbb6260876d4d9` |
| `0055ded0_m2/shadow_ab.log` | `0de773462cc516cc303df11dc9ee657ac567ccb5887107cb214b384d2a0b915f` |
| `0055ded0_off/shadow_ab.log` | `518e8bf778846aa2281f26fa8eb466b2f4d8a33c52f5b7c7c1771e0e7ca5d8bf` |
| `0057c300/shadow_ab.log` | `5ebc34ae4f607ac3dff47c1f8e1ea58fa03aa4077e8dca52abc4182558fa663e` |
| `0057c500/shadow_ab.log` | `cf13956697b6e4bcf5a91384873f94e110e669973771ed763a03ea87b403583c` |
| `0057c500/shadow_transcript_pass2_launchfail.txt` | `8adcd6f3b571c155c73326a0ec4de36cc06f3a0fe3f71e3c642c53570c1d1ab2` |
| `pass1/0055dc70/shadow_ab.log` | `964fd502b32f9b0cb96ccd165217ef626166d80cfae4f62a6cf41cd1e3b53bab` |
| `pass1/0055dca0/shadow_ab.log` | `0c7f8cd2597c9232f8ed5c4ae2710325dd20eb32199c5d96c4ad2380fcdc71b2` |
| `pass1/0055ddd0_c0/shadow_ab.log` | `3b84138c1161e4a92ba9d654295be8327d8a17c404e868831bfcb896d31c96e0` |
| `pass1/0055ded0/shadow_ab.log` | `0279e859abb18021984557a45e0b9e975c16eaf89a11c766cc38c5d237d232a5` |
| `pass1/0055ded0_m1/shadow_ab.log` | `77aa21e8f81c52d31bebf5da72c52d645d5b82f500bf0e184377777c5fdd9e25` |
| `pass1/0055ded0_m2/shadow_ab.log` | `991a86bf1fc54d3faa216ba1ff59e9db30eb76ffd4bd2ae4902650aec808ddc0` |
| `pass1/0055ded0_off/shadow_ab.log` | `7068b6355e8c4b88c1864febe124bae5df9eb6a586cec2b3f403ac349d21e324` |
| `pass1/0057c300/shadow_ab.log` | `72074cda3de7483d02025cfcc9c924008b7c3b8fc4ad72aec73b727330c955f7` |
| `pass1/0057c500/shadow_ab.log` | `03d8b2384bcc414802d9b0d7df30979a7a6a8b358f5003c7f9bff5044fdb8c21` |
| `gates/H_DEF.step.csv` | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` |
| `gates/H_DEF.gates.csv` | `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` |
| `gates/H_DEF.ptrace.log` | `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| `gates/B_DEF_K0.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/B_DEF_K1.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/B_DEF_K26.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |

Build `MASHED_NO_DEPLOY=1` at `5b688e76e`: `mashed_re.exe`
`28ad585e15ea555237be55a3b1712f6edd9492f7f9baaf23b188142031b05649`, `mashed_re_dev.asi`
`63299e3c7a535dc6190c34b86d3768da57490c40e61a4068ba14736e22dc8f70`, rva-lint 0 NEW. The `.asi` went to the private
root only; `original/mashed_re_dev.asi` was not touched.

## Tracker transaction (re-classify)

- `hooks.csv`: `0x0055ded0`, `0x0055dca0`, `0x0055dc70`, `0x0057c300`, `0x0057c500` C2 -> **C3 impl**, `scenario`
  `shadow_tracked:track0-mode10-cars4`, `frida_diff` `shadow_ab:verify/hb_shadowalloc_20261101/shadow_summary.tsv`,
  dated note appended. No C4: no race diff with the hook installed (CONFIDENCE.md write-set leg, L-38/L-40/L-54).
- C3 gate per row (Lane-2 port under `mashedmod/src/mashed_re/Lane2/`, clean build, `RH_ScopedInstall`, opt-in and
  runtime-toggleable, a caller at C2+, callees at C2+ or run-time slot dispatch):

  | RVA | caller | callees |
  |---|---|---|
  | `0x0055dc70` | `0x0047f4c0` C2 (call at `0x0047f53b`) | `0x0055dca0`, slot `+0x108` |
  | `0x0055dca0` | `0x0055dc70` (call at `0x0055dc92`) | `0x00559b50` C3, `0x0055f450` C3, slots `+0x108` / `+0x114` |
  | `0x0055ded0` | `0x0057c300` (call at `0x0057c320`) | slot `+0x108` only |
  | `0x0057c300` | `0x004218b0` C3 | `0x0055ded0`, slot `+0x108` |
  | `0x0057c500` | `0x004218d0` C3 | `0x00564130` C2, slot `+0x108` |

- `UNCERTAINTIES.md` via `re/tools/uncertainty_resolve.py`: U-9337 (`0x0055dc70`), U-9531 (`0x0055dca0`), U-9534
  (`0x0055ded0`) resolved. Hand-read of the disassembly (`D:/MashedWT/D4-WSB-PORT-BROAD-2_data/dis_<rva>.txt`): every
  listed indirect call loads RwGlobals from `[0x007d3ff8]` right before it (`0x0055dc70`; `0x0055dcd7`, `0x0055dcf3`,
  `0x0055dd11`; `0x0055deff`, `0x0055df65`) and calls slot `+0x108` or `+0x114`, and this step's recorders swapped into
  those slots logged every call with the pushed arguments. Every one is caller-cleans: `add esp, 0x18` at `0x0055dc97`
  (2 slot pushes + 4 for `0x0055dca0`), `add esp, 0x38` at `0x0055dd2b` (the 14 pushes from `0x0055dcaf` on),
  `add esp, 8` at `0x0055df14` and `0x0055df70`. That fixes the esp depth at `0x0055dc82` / `0x0055dc86` /
  `0x0055dc8b` (entry - 8, entry - 0xc) and at `0x0055df43` (the loop `0x0055df28..0x0055df41` pushes nothing).
  U-9341 / U-9342 (`0x0057c500` / `0x0057c300`) were already resolved by D4-WSB-PORT-NARROW-TAIL.
- `STUBS.md`: no stub in the five TUs, nothing filed. `re/analysis/CHANGELOG.md`: 5 promotion lines + 3 U-row lines
  below `<!-- ENTRIES -->`.
- Open after this step: none on these five rows.
