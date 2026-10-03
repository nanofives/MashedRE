# PRE-REGISTRATION — the `FUN_004148b0` knob-took witness. **UNRUN.**

Written and committed **before** any capture in this directory exists and before any line
of game code or any `.rsp` is edited. Branch `race/first-frame-parity`, HEAD at write time
`ec1c1e30`.

## Why this witness exists, and why it is mandatory

`re/NEXT_SESSION.md`'s START-HERE item 1 asks for the `FUN_004148b0` wiring and says it is
cheap: *"64 of 149 calls, and NO new reversing is needed … What is missing: all three TUs
(`AiLeaderTimer.cpp`, `AiTargeting.cpp`, `AiLineOfSight.cpp`) are in `asi_sources.rsp`
only."* The same item then mandates this witness, naming the risk explicitly:
*"`LeaderTimer` reads per-car rank/progress tables that may be `.bss` zeros in the
standalone — the exact trap `verify/d3_modes37_20261002/RESULT_STEP2.md:201-203` already
measured for `FUN_00484c70`."*

**The reversing claim is correct. The "what is missing" claim is not, and this witness is
registered to settle both halves rather than to confirm one.** Reading
`Ai/AiLeaderTimer.cpp` shows the body is **not portable by an `.rsp` line**: it calls three
absolute addresses in `MASHED.exe`'s `.text` and reads four initialised-rodata constants
plus a rodata table, none of which the standalone has. The two legs below test that, and
either can come back against me.

## What `LeaderTimer` actually depends on

From `mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp`, line by line:

| dependency | address | line | kind |
|---|---|---|---|
| mode gate `== 2` → return 0 | `0x0089a368` | `:91` | runtime data |
| float `__ftol`'d → `iVar1` | `0x0089a360` | `:92` | runtime data |
| `idx364`, `!= -1` → `E470` | `0x0089a364` | `:93` | runtime data |
| table bias | `0x0089a374` | `:98` | runtime data |
| **the limit table** | `0x005f2dd8 + (bias + iVar1*5)*4` | `:98` | **initialised rodata** |
| `RankAt(v)` | `0x0089a4c4 + v*0x74` | `:99` | runtime data |
| `TimerAt(v)` | `0x0089a4c8 + v*0x74` | `:108` | runtime data |
| **`Prog(v)`** → `0x008989b0 + v*4` | via `FUN_00442cc0` | `:101`, `:106` | runtime data |
| thresholds `_DAT_005cd0a8 / a4 / a0`, `_DAT_005cc35c` | `0x005cd0a8`, `…a4`, `…a0`, `0x005cc35c` | `:107,116,118,119` | **initialised rodata** |
| frame delta | `0x007f1008` | `:108` | runtime data |
| `E470(i)` | **call `0x0040e470`** | `:94`, `:104` | **`.text` call** |
| `Prog(i)` | **call `0x00442cc0`** | `:101`, `:106` | **`.text` call** |
| `VehRecPtr` | **call `0x0046d4a0`** | `:109` | **`.text` call** |
| `Ftol` | **call `0x004a2c48`** | `:79-85` | **`.text` call** |

## Leg W-SAFE — static, and it does not need the game to run

**Claim under test:** calling `0x0040e470` / `0x00442cc0` / `0x0046d4a0` from
`mashed_re.exe` access-violates, so adding `AiLeaderTimer.cpp` to `exe_sources.rsp` is
unsafe before any question about data.

Evidence to be cited and checked, not assumed:
- `exe_main.cpp:56` — the standalone VirtualAlloc-maps **`0x00500000..0x009fffff`** only.
- `Compat/StandaloneRvaThunks.h:7` — *"the entire `0x00400000..0x004fffff` MASHED .text
  range is unmapped"*; `Frontend/MenuButtonDetect.cpp:71` records the consequence —
  *"calling through this RVA AVs the process"*.
- `hooks.csv`: `0x0040e470`, `0x00442cc0`, `0x0046d4a0` all have an **empty `exe_file`**;
  only `0x004a2c48` has an exe-side body.
- No `StandaloneThunks_Install` call exists for any of the three.

**W-SAFE passes (i.e. the hazard is confirmed) iff all four hold.** If any fails — in
particular if a thunk for one of the three is found — I say so and the leg is reported as
not establishing the hazard.

## Leg W-DATA — live, on BOTH sides, and it is the decisive one

Read the dependency addresses above during a race, on each side, and report every value.

**Port side.** `re/tools/sa_leaderwatch.py` (new), built on `re/tools/sa_boostwatch.py`'s
pattern: spawn `mashed_re.exe` exactly as `sa_capture.py` does and poll with
`ReadProcessMemory`. **No injection, no Frida, no game-code change, no build.**

**Original side.** One Frida **entry** hook on `0x004148b0` — the same hook the
elimination session already ran safely as `o_t3`'s fifth — reading the same addresses at
entry. Entry hooks only; the game is launched muted with `MASHED_TITLE` set; the PID is
tracked and only it is killed.

### W-BASE — a wrong read must not produce a quiet answer

Registered **before** the run, because "all zeros" is the expected-looking answer and is
exactly what a bad base would also print (memory `all-zero-reads-prove-nothing-alone`,
`verify-the-harness-knob-actually-took`):

1. **Independent-channel agreement.** The port reader must also sample
   `0x0089a52c + v*0x74` (`ai_mode`) and `0x007f0ff4` (frame). The same run writes
   `MASHED_AI_STEPDUMP`, which logs `ai_mode` and `clk_0ff4` from inside the game. The
   reader's samples must be consistent with that file's values for the same slots.
2. **Liveness.** `0x007f0ff4` must **advance** across samples. A frozen counter means the
   read is not of live game state.
3. **Non-degenerate control.** At least one sampled address must be **non-zero** on the
   port. `0x0089a52c` and `0x007f0ff4` are known-written by the exe's own AI path, so a
   reader that returns zero for *everything* including those has a bad base.

**If W-BASE fails on any leg, W-DATA is VOID** — not re-thresholded, not interpreted — and
I report that instead of a result.

### W-GATE — the decision rule, registered now

Let `P` = the port's values, `O` = the original's, over the same race phase.

- **INERT** — `O` shows the limit table entry **non-zero** and `Prog` **non-zero** on at
  least one slot, while `P` shows **zero** for both. → The `.rsp` wiring **cannot**
  reproduce the 64 calls even if it were safe to link. **I do not perform the `.rsp`
  wiring.** I report the real scope: the producers that must be ported first.
- **LIVE** — `P`'s inputs are non-zero and of the same shape as `O`'s. → The wiring is
  worth doing; proceed to it as a separately pre-registered step with the full promotion
  leg.
- **MIXED** — some inputs present, some not. → Report exactly which, per address. That
  list **is** the port's scope, and no wiring happens until it is closed.

**No outcome here authorises seeding a global to make the arm fire.**
`verify/d3_modes37_20261002/RESULT_STEP2.md` already set that rule for `FUN_00484c70`:
*"seeding the globals would not be a port."* It applies unchanged.

## Already-known facts this witness must be consistent with, not re-derive

- `AiStandalone.cpp:707` states the Prog array's writer is unported:
  *"`0x008989b0[v]` (FUN_00442cc0; written by the unported `FUN_00442a60`)"*.
- `0x0089a374`, the table bias, is written by `FUN_004177b0`, whose exe copy
  (`Race/RuleEngine.cpp`) was demoted C3→C2 on 2026-09-29 as *"the finish-order fragment
  only"*.
- The committed captures `o_t1` / `o_t2` / `o_t3` carry the **exact 64 calls** the fix must
  change, so any later fix is checkable call-by-call rather than only through a band.

If the live legs contradict any of these three, **the live measurement wins** and I say so.

## Out of scope for this step

No `.rsp` edit. No game-code edit. No build. No tracker promotion. No D2 code. Anything
touching `+0x4a4`, the contact collector / `FUN_00538c80`, grip-clamp #6, the substep/chunk
loop, `ReassertContacts` or player-car speed on Training is reported as a **"D2 REOPEN
CANDIDATE"** row and changed nowhere.

## Rules

Muted launches, `MASHED_TITLE` set, `MASHED_WIN_POS=primary-bl`, `--poke-ctrl-slots` on
race captures, **never** `MASHED_NAV_DEMO`. PIDs tracked, only mine killed. Frida **entry
hooks only**. Ghidra pool clones `-readOnly`; the master project is never written.
`original/MASHED.exe` is the diffing reference and gets no `unlock_*` patch.
