# RESULT — D2 attempt 15, STEP 1: **CONFIRMED**. `0x0046d7a2` is the `+0xbf8 = 2` writer.

Measured against [`PREREG_STEP1.md`](PREREG_STEP1.md), committed at `15b31fc4` before the
first run. **No gate was amended.** Two independent boots.

Captures: `orig_bp1.msd` (+ `.boostprobe.csv`, 2334 frames, 113 probe rows),
`orig_bp2.msd` (+ `.boostprobe.csv`, 2336 frames, 113 probe rows). Arm identical to
`verify/d2_reopen_20260929/orig_solo3.msd` plus `--boost-probe`; run 2 adds
`--peek 0063ba8c:i`.

---

## 1 The verdict

| gate | required | boot 1 | boot 2 | |
|---|---|---|---|---|
| **G0** | probe armed, run not VOID | armed, 113 rows, `detached:false`, `err:null` | same | **PASS** |
| **G1** | `0x0046d780` entered exactly once for car 0 | `rel = 1` | `rel = 1` | **PASS** |
| **G2** | at entry `+0xbf8 == 0` and `+0xbf4 == 3000` | `bf8 0`, `bf4 3000` | `bf8 0`, `bf4 3000` | **PASS** |
| **G3** | at return `+0xbf8 == 2`, `+0xbf4` unchanged | `bf8 2`, `bf4 3000` | `bf8 2`, `bf4 3000` | **PASS** |
| **G4** | `>= 100` charge entries, `+0xbf4` non-decreasing to 3000 | 112 entries, non-decreasing, max 3000 | 112, non-decreasing, max 3000 | **PASS** |
| **G5** | `DAT_0063ba8c == 6` on the tick after the release | **NOT MEASURED** — see §4 | partially corroborated | **NOT DECIDING** |
| **G6** | `.msd`: `+0xbf8 == 2` on exactly 14 frames, `+0xbf4` `3000 -> 0` at `-200`/frame | 14 frames (887..900), every delta exactly `200` | 14 frames (882..895), every delta exactly `200` | **PASS** |

> ### VERDICT: **CONFIRMED** (G0 ∧ G1 ∧ G2 ∧ G3), on two boots.
>
> The instruction that sets the vehicle record's `+0xbf8` to `2` at the green light is
> **`0x0046d7a2`**, `mov dword [eax+0x882198], 2`, inside **`FUN_0046d780`**
> (`0x0046d780..0x0046d7ec`). **U-9174 is resolved.**

---

## 2 The mechanism, end to end, with every constant cited

Nothing in this section is inferred; each line names the address it is read from.

| step | where | what |
|---|---|---|
| the charge lives at | `veh[i] + 0xbf4` = absolute `0x00882194 + i*0xd04` | integer, range `[0, 3000]` |
| the state lives at | `veh[i] + 0xbf8` = absolute `0x00882198 + i*0xd04` | integer, `{0, 1, 2}` |
| charge, per state-tick | `FUN_0046d7f0` `0x0046d7f0` | `if (160.0f < accel_byte) charge += delta;` `else charge -= delta;` then clamp `[0, 3000]`. `160.0` is `_DAT_005cea3c` (`0x43200000`); `3000` is the `0xbb8` at `0x0046d869`; accel byte is `0x007f103c + ctrl*0x4c`, `ctrl = (int)[0x007f1a14 + i*0x10]` |
| **delta is a hard constant** | `0x0042c980` and `0x00492d83`, both `push 0x32` | **50** per state-tick. The caller loops `(frameMs-1)/50 + 1` times (`mul 0x51eb851f` / `shr 4` = divide by 50 at `0x0042c973`), so at 60 fps that is **one tick of 50 per frame** |
| release | `FUN_0046d780` `0x0046d780`, called at `0x0041049b` | `charge > 1000` (`0x3e8` at `0x0046d79a`) → **`+0xbf8 = 2`** (`0x0046d7a2`), charge untouched; else `charge > 0` → `+0xbf8 = 1` (`0x0046d7cf`) and `+0xbf4 = charge + 1000` (`0x0046d7d9`); either way `FUN_00422b50(car, signed_amount)` adds to the per-car counter at `0x008995bc + car*0x138` |
| release condition | `0x00410460`, `FUN_0041da90` → `DAT_0063d588` | fires when the pre-race timer reaches `1.86` (`_DAT_005ccdf4` = `0x3fee147b`) |
| then | `0x004104e3` | `DAT_0063ba8c = 6` |
| consumed by | A6a `FUN_00467650` `0x00467d2e` / `0x00467def` | `== 1` adds `5e6` along `[edi+0x7c..0x84]` to `+0xb14/18/1c`; `== 2` **zeroes** `+0xb14/18/1c` (`0x00467e0c`/`0x00467e12`/`0x00467e1f`) and counts `+0xbf4` down |

**So the 15-frame drive-force hold is an OVER-REV BOG.** The charge saturates at 3000, 3000
exceeds the 1000 threshold, the release picks state 2 instead of state 1, and A6a then zeroes
the drive force for `3000 / 200 = 15` frames. It is the losing branch of a start-line launch
mini-game, and the arm the D2 bounds were measured on holds full accel through the countdown,
so the original **always** takes it in this arm.

### The measured timeline, identical in shape on both boots

| | boot 1 | boot 2 |
|---|---|---|
| first charge tick | 776 | 771 |
| charge saturates at 3000 | tick 835 (59 ticks later, `0 -> 3000` at exactly `+50`/tick) | tick 830 |
| release tick | **887** | **882** |
| charge ticks total | 112 (contiguous, one per tick) | 112 |
| accel byte during all 112 | **255** on every tick | 255 |
| `DAT_0063ba8c` inside the tick | **5** on all 113 rows | 5 |
| `+0xbf8 == 2` frames in the `.msd` | 887..900 = **14** | 882..895 = **14** |

112 ticks at 60 Hz = **1.867 s**, which is the `1.86` threshold.

---

## 3 Why two instruments were needed, and what the search space was

`U-9174`'s statement — *every literal-displacement writer of `+0xbf8` writes ZERO* — is
**correct and is not overturned**. The writer folds the base: MSVC turns `&veh[i].+0xbf8`
into `i*0xd04 + 0x00882198`, so the instruction encodes **no `0xbf8` anywhere**. The
`imul eax,eax,0xd04` at `0x0046d78e` is the stride witness, and the record base `0x008815a0`
is independently recorded in the capture provenance (`base_va`, `rec_size`).

- A capstone sweep of `.text` for operands `0x00882194` / `0x00882198` / `0x008815a0`
  returns **26 instructions**, of which **10** touch the two fields. That is what found it.
- A Ghidra decompile of **all 6239 defined functions**, zero decompile failures
  (`re/tools/ghidra_scripts/GrepDecompAll.java`), grepped for `0xbf[048c]`, returns
  **`FUN_00467650` alone**. That is why no second writer is claimed — and it is also why a
  decompiler grep alone would have missed `0x0046d7a2` entirely.

One candidate was rejected on inspection rather than assumed: `0x004254d4`/`0x004254de`/
`0x004254e8` in `FUN_004252c0` writes `+200`, `2`, `3000` to three adjacent dwords — the same
numbers in the same shape — but its base is `&DAT_008995c4` with stride `0x4e` dwords, a
different struct. Coincidence, not the writer.

**[UNCERTAIN], unchanged from the pre-registration:** both instruments see only `.text`, and
a writer inside a region Ghidra left undefined *and* that the linear sweep mis-synchronised
over would be missed by both. No such region is known.

### The complete reference set

Writers of `+0xbf8`: `0x0046d7a2` (=2), `0x0046d7cf` (=1), and A6a's four zeroing stores
`0x00467dd7` `0x00467de5` `0x00467e36` `0x00467e44`. **Six, and no others.**
Readers of `+0xbf8`: `0x0046c742` (getter `FUN_0046c730`), A6a's `0x00467d2e` and `0x00467def`.
Writers of `+0xbf4`: `0x0046d7d9`, `0x0046d845`, `0x0046d864`, `0x0046d869`, `0x0046d874`,
and A6a's `0x00467d08` `0x00467d10` `0x00467d24` `0x00467dcd` `0x00467ddd` `0x00467e2c`
`0x00467e3c`.
Readers of `+0xbf4`: `0x0046c762` (getter `FUN_0046c750`), `0x0046d794`, `0x0046d834`,
`0x0046d84e`, and A6a's `0x00467cf1` `0x00467d1a` `0x00467d3b` `0x00467db9` `0x00467df8`.

**The start state gates exactly one thing beyond `b14`:** nothing else reads `+0xbf8`. The
two getters feed `DAT_007f0a04[i]` / `DAT_007f0a08[i]` at `0x004104a7` / `0x004104b2`, a
readback the release loop performs for display; `+0xbf4` additionally feeds the
`FUN_0040e350() == 6` clamp at `0x00467cf1..0x00467d24`.

---

## 4 Two disclosures

**G5 is NOT MEASURED at the resolution it was written for.** The probe's last row is the
release itself, and `DAT_0063ba8c` is set to 6 one instruction block later at `0x004104e3`,
after the release loop — so this probe can never witness the transition, and both runs
report `state 5 -> 5`. Boot 2's `--peek` samples `0063ba8c` every 3.6 s and shows
`3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6` — i.e. it **is** 6 after the release and was **3** before.
That is corroboration at 3.6 s resolution, not the tick-level gate G5 asked for. G5 was
registered as corroborating and decides nothing; the verdict stands on G0-G3.

> Side observation, recorded and **not acted on**: the global reads **3** outside the tick and
> **5** inside `FUN_004103a0`, then 6 after the release. Memory `no-driving-hud` glosses
> `DAT_0063ba8c` as `3 = driving`; here 3 is the **pre-release** value. **[UNCERTAIN]** — not
> on D2's path, not resolved here.

**The probe's accel column used the wrong stride and has been corrected.** It read
`0x007f103c + ctrl*0x13`; the control-block stride is `0x4c` **bytes** (`0x13` is the
decompiler's DWORD index; `Ai/AiState.h:38` and `Ai/AiController.cpp:170` use `0x4c`). Both
STEP 1 runs used `--poke-ctrl-slots`, giving car 0 → ctrl 0, where both strides yield offset
0 — so **neither run's accel column is wrong**, and no gate read that column. The tool is
fixed for future cars; the fix is disclosed rather than folded in silently.
