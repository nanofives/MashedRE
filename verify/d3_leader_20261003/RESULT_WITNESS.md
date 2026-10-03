# RESULT — the `FUN_004148b0` knob-took witness. **W-GATE = INERT. The wiring was NOT performed.**

Pre-registration: `PREREG_WITNESS.md`, committed **unrun** at `3988dbf0`.
**No gate was replaced. No band was moved. No game code and no `.rsp` was edited.**

## Verdict

> **Wiring `FUN_004148b0` into `mashed_re.exe` by adding its TUs to `exe_sources.rsp` would
> be BOTH unsafe AND inert.** It would access-violate on its first callee, and even if that
> were thunked it would `return 0` on the very first table test, before `Prog` is ever read.
> **W-GATE returns INERT exactly as registered, so the wiring was not performed.**
>
> `re/NEXT_SESSION.md`'s START-HERE item 1 is **half right**: the *arm to transcribe* needs
> no new reversing, but its claim that *"what is missing"* is the three TUs being
> `asi_sources.rsp`-only is **wrong**. Six inputs are missing, not zero — and **this witness
> resolved two of them in passing.**

## Gate outcomes

| gate | outcome |
|---|---|
| **W-BASE** (port) | **PASS, all three legs.** |
| **W-BASE** (original, KA) | **PASS.** Live-read thresholds `[6.5, 5.5, 6.0, 4.0]` equal an **independent** PE file-image read to every digit, and `_DAT_005cc35c == 4.0` as `AiLeaderTimer.cpp:61` states. |
| **W-SAFE** | **PASS — the hazard is confirmed**, all four citations hold. |
| **W-DATA** | measured on both sides, below. |
| **W-GATE** | **INERT.** |

### W-BASE, port side — because "all zeros" is also what a bad base prints

1. **Independent-channel agreement.** The reader's `aimode` is `[0]` on all four slots; the
   same run's `MASHED_AI_STEPDUMP` (`p1.csv`, 6979 rows) has `ai_mode` distinct `['0']`.
   The reader's `frame` spans **0..142150**; the stepdump's `clk_0ff4` spans **50..142250**.
   Same channel, same scale, overlapping range.
2. **Liveness.** `0x007f0ff4` advanced 0 → 142150 across 1089 samples.
3. **Non-degenerate.** `mode368` ∈ {0,1}, `flt360` ∈ {0.0, **2.5**}, `bias374` ∈ {0,1,2,3},
   `framedt` ∈ {0, **50**} are all non-zero. A bad base cannot produce those *and* zero
   elsewhere.

## W-SAFE — calling the three callees from the exe access-violates

| evidence | says |
|---|---|
| `exe_main.cpp:56` | the standalone VirtualAlloc-maps **`0x00500000..0x009fffff` only** |
| `Compat/StandaloneRvaThunks.h:7` | *"the entire `0x00400000..0x004fffff` MASHED .text range is unmapped"* |
| `Frontend/MenuButtonDetect.cpp:71` | *"calling through this RVA AVs the process"* |
| `hooks.csv` | `0x0040e470`, `0x00442cc0`, `0x0046d4a0` all have an **empty `exe_file`**; only `0x004a2c48` has an exe-side body |

No `StandaloneThunks_Install` exists for any of the three. `AiLeaderTimer.cpp` calls
`0x0040e470` at `:94` and `:104`, `0x00442cc0` at `:101`/`:106`, `0x0046d4a0` at `:109` —
all in the unmapped `.text` range.

**And the port's own data drives it straight into the first one.** `idx364` is **`0`** in
the standalone, so `idx364 != -1` at `:94` is **true** and `E470(0)` is called. On the
original `idx364` is **`-1` on all 512 calls**, so that call is skipped entirely.

## W-DATA — both sides, same race phase

Port: `re/tools/sa_leaderwatch.py` (new), `ReadProcessMemory`, **no injection, no Frida, no
build**, 1089 samples over 55 s. Original: **one entry hook** on `0x004148b0`
(`scenario_launch.py --leader-probe`, new), **512 calls**, cars 1/2/3.

| input | address | **ORIGINAL** | **PORT** | |
|---|---|---|---|---|
| mode gate | `0x0089a368` | `0` | `{0, 1}` | both live |
| `__ftol` float | `0x0089a360` | `2.5` | `{0.0, 2.5}` | both live |
| `idx364` | `0x0089a364` | **`-1`** | **`0`** | **differs** |
| table bias | `0x0089a374` | `0` | `{0,1,2,3}` | **differs** |
| frame delta | `0x007f1008` | `50` | `{0, 50}` | both live |
| `RankAt` | `0x0089a4c4+v*0x74` | `0` | `0` | same |
| `TimerAt` | `0x0089a4c8+v*0x74` | **`0..1250`, stepping by 50** | **`0`** | **PORT DEAD** |
| **limit table** | `0x005f2dd8` | **14 of 64 non-zero** | **0 of 64** | **PORT ZERO** |
| **thresholds** | `0x005cd0a8/a4/a0`, `0x005cc35c` | **6.5 / 5.5 / 6.0 / 4.0** | **0.0 / 0.0 / 0.0 / 0.0** | **PORT ZERO** |
| **`Prog[0..3]`** | `0x008989b0+v*4` | **non-zero on 464 / 125 / 486 / 461 of 512; max 9.988 / 3.790 / 4.466 / 2.750** | **`0.0` on all 4 slots in all 1089 samples** | **PORT ZERO** |

**The split is perfectly clean and it explains itself:** every **runtime-written** global is
populated in the port; every **initialised-image** value is zero. `exe_main.cpp:56`
VirtualAlloc-maps that address range **blank** — it does not load `MASHED.exe`'s `.rdata` /
`.data` contents — so rodata constants and image tables read as zeros by construction.

## Why it returns 0 — traced on the measured values, not assumed

`AiLeaderTimer.cpp` with the port's own numbers:

1. `:91` `Gi(0x0089a368)` ∈ {0,1}, never `2` → no early return.
2. `:92` `iVar1 = Ftol(flt360)` ∈ {0, 2}.
3. `:93-94` `idx364 == 0 != -1` → **`E470(0)` → ACCESS VIOLATION** (W-SAFE).
4. Were that thunked: `:98` index = `bias374 + iVar1*5` ∈ {0,1,2,3,10,11,12,13}; **every
   one reads `0`** → `limit = 0`.
5. `:99` `limit (0) <= RankAt(v) (0)` → **`return 0`.** `Prog` at `:101` is never reached.

**The original takes the same path and does not stall:** its only index is **10**
(`bias374 = 0`, `iVar1 = 2` on all 512 calls), and the image's `tbl[10] = 1`, so
`1 <= 0` is false and it proceeds — then finds `Prog` non-zero and the timer live.

So the port is blocked **three times over, independently**: the AV, the zero limit table,
and the all-zero `Prog` array.

## The REAL scope — six missing inputs, two of them now resolved

| # | missing input | status | what it needs |
|---|---|---|---|
| 1 | `FUN_0040e470` `E470` | **C3 `impl`**, asi-only, `exe_file` **empty** | an exe-side body |
| 2 | `FUN_00442cc0` `Prog` getter | **C3 `impl`**, asi-only, `exe_file` **empty** | an exe-side body (trivial: reads `0x008989b0[v]`) |
| 3 | `FUN_0046d4a0` `VehRecPtr` | **C3 `impl`**, asi-only, `exe_file` **empty** | an exe-side body |
| 4 | **`FUN_00442a60`** — the **producer** that writes `0x008989b0` | **C2 `new`, NO BODY ANYWHERE** (`hooks.csv`: `Spectator::ComputeDistances`) | **a real port. This is new reversing, and it is the item the handoff's "no new reversing is needed" misses.** `AiStandalone.cpp:707` already named it unported. |
| 5 | limit table `0x005f2dd8` | **RESOLVED HERE** | transcribe as a constant. Only index **10** is ever used and its value is **1**. Head of 64: `[2,1,1,0,0,1,1,0,0,0,1,0,0,0,0,0]`, 14 non-zero. |
| 6 | thresholds `0x005cd0a8/a4/a0`, `0x005cc35c` | **RESOLVED HERE** | **6.5 / 5.5 / 6.0 / 4.0**, read twice independently (live Frida + PE file image) and agreeing to every digit |

**Plus two upstream disagreements that would survive all six**, so they must be settled
before the 64 calls can be reproduced rather than merely made reachable:
`idx364` (**-1** vs **0**) and `bias374` (**0** vs **{0,1,2,3}**; written by `FUN_004177b0`,
whose exe copy `Race/RuleEngine.cpp` was demoted C3→C2 on 2026-09-29 as *"the finish-order
fragment only"*).

**No global was seeded.** `verify/d3_modes37_20261002/RESULT_STEP2.md`'s rule — *"seeding
the globals would not be a port"* — was applied unchanged.

## Consistency with what was already recorded

All three of the pre-registration's "must be consistent with" facts hold, and none had to
be overturned:

- `AiStandalone.cpp:707` *"`0x008989b0[v]` … written by the unported `FUN_00442a60`"` —
  **confirmed by measurement**: the array is `0.0` on all four slots in the port and
  non-zero on 464/512 calls in the original.
- `0x0089a374` is written by `FUN_004177b0`, exe copy demoted — **confirmed**: the two
  sides disagree on exactly that value.
- `o_t1`/`o_t2`/`o_t3` carry the 64 calls — untouched; this session added `o1` with the
  input values beside them, so a future fix is checkable call-by-call.

## D2 WATCH

**No D2 REOPEN CANDIDATE.** Nothing here touches `+0x4a4`, the contact collector /
`FUN_00538c80`, grip-clamp #6, the substep/chunk loop, `ReassertContacts` or player-car
speed on Training. **No D2 code was read or changed.**

## What changed (no game code)

| file | change |
|---|---|
| `re/tools/sa_leaderwatch.py` | **new**, read-only. `ReadProcessMemory` port-side reader with the three W-BASE legs. |
| `re/frida/scenario_launch.py` | **new `--leader-probe`** (+ `--leader-probe-limit`): **one entry hook** on `0x004148b0` logging the inputs, with the threshold KA. Reads nothing back and writes nothing, so it cannot perturb the arm it measures. Existing probes untouched. |

## Hygiene

`mashed_re.exe` spawned and killed **by PID** by `sa_leaderwatch.py`; `MASHED.exe` by
`scenario_launch.py`. No blanket kill by name. Muted, `MASHED_TITLE` set,
`MASHED_WIN_POS=primary-bl`, `--poke-ctrl-slots`, `MASHED_NAV_DEMO` never used. Frida
**entry hooks only**. No Ghidra session. `original/MASHED.exe` read **only** as a file image
for the rodata check; not modified, no `unlock_*` patch. No build. Nothing pushed.
