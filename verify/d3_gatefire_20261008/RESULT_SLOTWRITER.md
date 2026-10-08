# RESULT — the player slot's writer is `FUN_0042b960`, and the port reproduces everything about it except the four stores

Date 2026-10-08. Follows `RESULT_E470.md` §4 item 1, chosen by **USER DECISION (Mariano,
2026-10-08)**: "Find the original's writer of the player slot cell — the faithful route."

**Static resolution from committed plates. No run, no build, no code change.** `original/`
untouched. No C-level.

## 0. Verdict first

> **`FUN_0042b960`** (`re/analysis/c0_promotion_frontend_a/0x0042b960.md`, 115 bytes, C1, single
> callee `FUN_0040e480`) ends with exactly four stores:
>
> ```
> FUN_0040e480(0, 1)   // slot 0 (player) = 1
> FUN_0040e480(1, 0)   // slot 1 = 0
> FUN_0040e480(2, 0)   // slot 2 = 0
> FUN_0040e480(3, 0)   // slot 3 = 0
> ```
>
> **That is the `1` the port is missing.** It is a race-setup initialiser, not a per-frame writer.

## 1. The two-writer sequence, and which half the port has

| # | writer | what it stores | ported? |
|---|---|---|---|
| 1 | **`FUN_0042b960`** (frontend race setup) | slot 0 = **1**, slots 1-3 = **0** | **NO** |
| 2 | `FUN_00418860` (per-race AI tick) | alive **AI** cars → **2**, gated on `FUN_0046c7b0(v)==1` | **yes** — `AiStandalone.cpp:1759-1761` |

Run in order, the original ends at `{v0: 1, v1/v2/v3: 2}` — **exactly what `o_e470.csv` measured**
(581/672 for v0=1; 455-563/672 for the AI cars at 2). The port runs only step 2, so v0 is never
written and reads the image-pad `0`, while v1-v3 correctly reach `2`. **The measurement and the
static record agree completely**, which is the strongest form this finding could take.

Note the port's step-2 transcription is faithful and was already corrected once against the live
listing (`AiStandalone.cpp:1753-1756`: each of the three calls individually gated, "NOT
unconditional as the older condensed pseudocode implied"). Nothing about step 2 is in question.

## 2. The port already depends on the rest of `FUN_0042b960`'s output

`FUN_0042b960` also writes `DAT_007f1a14 = 0`, walks the 0x4c-stride entry tables at `0x007f1042` /
`0x007f1502`, and sets `0x007f1a24`/`1a34`/`1a44` to `-1` plus `0x007f1a0c = 1`.

**`0x007f1a14` is the port's `kSlotTableBase`** (`Ai/AiState.h:41`, cited at `0x0041856c`) — the
table `VehicleStep` indexes every frame to find a car's ctrl block. So the standalone already
relies on this function's *other* effects being present, by whatever route. It reproduces part of
`FUN_0042b960` and omits the four `FUN_0040e480` stores.

That reframes the fix: this is **not** "seed a global the port has no business writing". It is
**finishing a setup function the port already partially performs** — the same shape as H3's step-6
tail, and it needs no new substrate.

## 3. Scope, and why this is a port rather than a bridge

`FUN_0040e480` is an 18-byte setter, "no branches, no calls", already transcribed in the port as
`CarSlotStateSet` (`AiStandalone.cpp:1404`) and already guarded for the null-base case. So the work
is four calls at the right point in race setup, not a new body.

**What is NOT yet established** and must be before writing it:

- **Where in the port's setup the four stores belong.** `FUN_0042b960`'s own caller chain is not
  traced here; its sibling `FUN_0042b9e0` is called from `FUN_0043dfd0` (a menu-side function), so
  `0042b960`/`0042b9e0` are plausibly the one-player / multi-player variants of the same setup.
  **[UNCERTAIN]** — the port must place the calls where the original's control flow does, not
  merely where it is convenient.
- **Whether `0042b9e0` writes a different pattern.** Only `0042b960` has been read. If the
  standalone's scenario corresponds to the sibling, the values could differ.
- **Whether the rest of `FUN_0042b960` also needs porting.** The port gets `0x007f1a14` set up
  somehow today; if that route already models this function, the four stores belong there rather
  than in a new site.

## 4. What is NOT claimed

- Not that adding the four stores makes branch 2 fire. It should clear **site 105** by letting
  `last` resolve to car 0; whether the chain then reaches **114 (FIRE)** or stops at **126** is
  `RESULT_GF1.md` §4's open question, and site 99 still accounts for **95.10%** of rows regardless.
- Not that `FUN_0042b960` is the *only* writer of slot 0. `FUN_004111c0` case 1 also calls
  `FUN_0040e480(0..2, 2)` + `(3, 0)`, but **only when `FUN_0042f6a0() == 0xb`** — a mode-11 path,
  and it writes `2` to slot 0, not `1`. So it is a different population and does not explain the
  measured `1`.
- Not that the 91/672 samples where the original's v0 reads `0` are explained. Those transitions
  remain uncharacterised (`RESULT_E470.md` §3).
- No C-level. Nothing executed in this leg.

## 5. Next

1. **Trace `FUN_0042b960`'s caller chain** to place the four stores faithfully, and read
   `FUN_0042b9e0` to confirm the one-player/multi-player split. Both are static, cheap, and
   decide the placement.
2. Then port the four `FUN_0040e480` calls, default-OFF, and re-read the `lt_exit` histogram:
   site 105 should drain, and where it drains to is the next real datum.
3. Site 99 stays with the race sub-state-machine row (`RESULT_M5.md`); it is untouched by this.
