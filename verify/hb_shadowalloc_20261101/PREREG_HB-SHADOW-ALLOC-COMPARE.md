# PREREG HB-SHADOW-ALLOC-COMPARE: allocator-aware shadow compare, 5 allocating Lane-2 rows

Committed alone, before any arm. Harness step (section HARNESS). Branch `orch/HB-SHADOW-ALLOC-COMPARE`, worktree
`.worktrees/HB-SHADOW-ALLOC-COMPARE`, off `orch/integration` `7a53dc44a` plus `orch/D4-WSB-PORT-BROAD-2` and
`orch/D4-WSB-PORT-NARROW-TAIL` (`6afb1ae11`) merged in, so the five Lane-2 TUs are present.

## Why

Five Lane-2 ports stay C2 only because the shadow lane cannot compare a body that allocates through the RwGlobals
(`DAT_007d3ff8`) slot `+0x108` (`(size, 0x30900)`) or `+0x114` (`(n, size, 0x30900)`):

| RVA | source step | shadow (allocator compare off) |
|---|---|---|
| `0x0055dc70` | D4-WSB-PORT-BROAD-2 | DIVERGENT 1/1, return differs |
| `0x0055dca0` | D4-WSB-PORT-BROAD-2 | DIVERGENT 1/1, 19 dwords |
| `0x0055ded0` | D4-WSB-PORT-BROAD-2 | DIVERGENT 23/24, `param_1+0x10` |
| `0x0057c300` | D4-WSB-PORT-NARROW-TAIL | DIVERGENT 24/24 |
| `0x0057c500` | D4-WSB-PORT-NARROW-TAIL | DIVERGENT 22/24 |

The raw logs show why. The pre-images restore the heap's bookkeeping, and the heap still hands the second run a
different block for the same request: `0x0055ded0` sample 0 stores `0x0f35d6d8` (original) vs `0x0f35d648` (port)
from the same restored state, and `0x0055dca0` differs at `+0x524` by one bit of a 32-bit word (pre `0ee4ffff`, A
`0fe4ffff`, B `0ee5ffff`), the allocator taking a different free slot
(`D:/MashedWT/D4-WSB-PORT-BROAD-2_data/<rva>/shadow_ab.log`).

## What changed (harness only)

`mashedmod/src/mashed_re/Core/ShadowTrack.h`, opt-in `MASHED_SHADOW_ALLOC=1` (default off). For the length of one
compare both slots point at recording wrappers. A call from the tracked thread inside a window is logged (slot, args,
result) and every dword the allocator wrote outside the block it returned goes into a mask. Then:

1. the k-th allocation of the original's run is paired with the k-th of the port's. Count, slot and arguments must
   agree, otherwise the sample counts one difference (`mm:1`);
2. dwords inside a paired block leave the page compare and are compared block against block (original's block k
   after its run vs the port's block k now);
3. any compared dword passes when original = `A_k + d` and port = `B_k + d`, `0 <= d <= size_k` (pages, blocks, the
   stack window, a 4-byte return);
4. a block dword that neither run changed from its pre-allocation value passes (uninitialised memory);
5. masked dwords (allocator writes) are not compared.

With the knob off the page compare is the earlier memcmp verdict, dword by dword, and the stack compare is the earlier
byte loop. A second opt-in, `MASHED_SHADOW_ALLOC_MUT=1|2`, is the negative control: after the port's run it adds 1 to
the first dword of the port's first block (1) or adds 4 to the first dword in the port's pages that holds that block's
address (2), and reverts it right after the compare. The log line carries
`alloc=n:<allocs>,mm:<0|1>,mask:<dwords>,movf:<overflow>,blk:<block dwords differing>,mut:<0|1|2>` and an `[al]` line
with the pairs.

`ShadowTrack.h` is also compiled into `mashed_re.exe` (Collision / Frontend / Render TUs include it), so the exe binary
changes. No exe behaviour changes with the knobs unset: the default gates run on this build.

Build `MASHED_NO_DEPLOY=1`: `mashed_re.exe` `368d2e3b8ff8b5d98c808fae5a16e3585fb19de32be562164ea3476e0e15886a`,
`mashed_re_dev.asi` `fdb0e4d162cda8b3f309b46ab47c3c4b1ffef74d5cfb0e5dbea08e76a50c4944`, rva-lint 0 NEW. The `.asi`
goes to the private root `D:/MashedWT/HB-SHADOW-ALLOC-COMPARE_root/original/` only, never to `original/`.

## Arms

Runner `run_shadow11.py` (copy of NARROW-TAIL's `run_shadow10.py`, paths and label changed, env echoed into the
transcript), driver `drive_sa.py` (copy of `drive_nt.py`): one solo window per row under `mashed_lock.py`, audio
endpoint check before and after, U-9677 piz collisions re-run (max 2). Census scenario as BROAD-2: `TRACK=0`, mode 10,
4 cars, `HOLD=60`, `SAMPLES_DONE=24`, `MASHED_ROOT=D:/MashedWT/HB-SHADOW-ALLOC-COMPARE_root`,
`MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, hook installed alone (`--hooks 0x<rva>`).

| arm | rows | extra env | expected |
|---|---|---|---|
| S | the 5 RVAs | `MASHED_SHADOW_ALLOC=1` | read-out below |
| K0 | `0x0055ded0`, tag `off` | none | DIVERGENT as in BROAD-2 (knob off unchanged) |
| C0 | `0x0055ddd0`, tag `c0` (CLEAN 24/24 in BROAD-2, no allocation) | `MASHED_SHADOW_ALLOC=1` | CLEAN, `alloc=n:0` |
| M1 | `0x0055ded0`, tag `m1` | `MASHED_SHADOW_ALLOC=1 MASHED_SHADOW_ALLOC_MUT=1`, `SAMPLES_DONE=6` | DIVERGENT on every sample with `mut:1` |
| M2 | `0x0055ded0`, tag `m2` | `MASHED_SHADOW_ALLOC=1 MASHED_SHADOW_ALLOC_MUT=2`, `SAMPLES_DONE=6` | DIVERGENT on every sample with `mut:2` |
| G | `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + solo lock, `A4_ROOT` = the private root | none | equal `re/orchestrator/merge_train.json` (G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3) |

Order: S, then C0, K0, M1, M2, then G. A row whose 60 s hold gives fewer than 5 samples is re-run once with
`HOLD=180` (tag `h180`); both runs are reported.

## Read-out

- **Harness GREEN** = C0 CLEAN with `alloc=n:0`, K0 DIVERGENT, M1 and M2 DIVERGENT on every sample whose `mut` is
  non-zero (and at least 1 such sample each), G equal to the references.
- **Row GREEN (C3 candidate)** = arm S CLEAN on every sample, `PATCHBYTE ... A/B-IS-REAL`, every sample `mm:0` and
  `movf:0`, at least one sample with `alloc=n:` above 0 and heap pages compared above 0 (CONFIDENCE.md heap-writer
  leg). Promotion C2 -> C3 only through re-classify, only with harness GREEN. No C4 (no race diff with the hook
  installed).
- **Row DIVERGENT** = stays C2, the RESULT reads out the differing offsets (`blk@`, `@page+off`, `[al]` pairs).
- A sample with `mm:1` means the two runs did not make the same allocator calls: that is a port finding (argument or
  order), not a harness fault.
- Masked dword counts are reported per row. A mask that covers a dword the body also writes hides that write (LIMITS
  in the header); the RESULT says so for any row where `mask` is large against the pages touched.
