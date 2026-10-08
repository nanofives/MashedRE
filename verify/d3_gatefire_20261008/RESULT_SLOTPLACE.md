# RESULT — both slot-state writers are MENU-side. The port has no faithful placement; it has a stand-in.

Date 2026-10-08. Follows `RESULT_SLOTWRITER.md` §5 item 1, chosen by **USER DECISION (Mariano,
2026-10-08)**. **Static, read-only** — `decomp_pc.py` against a read-only pool clone
(`-readOnly -noanalysis`, never the master project). No build, no code change, no run.
`original/` untouched. No C-level.

Raw: `slotwriter_decomp.txt` (both bodies + callers), `slotwriter_caller.txt` (`FUN_0043dfd0`).

> **Account-2 note:** Ghidra MCP is blocked on this licence, but `re/tools/decomp_pc.py` drives
> `analyzeHeadless` directly, so read-only decompilation is **not** gated by that. Memory
> `ghidra-mcp-down-use-analyzeheadless` applies and worked unmodified.

## 0. Verdict first

> **Both writers live inside `FUN_0043dfd0`, a MENU MESSAGE HANDLER** (its only caller is
> `FUN_00492d30`; its body switches on message codes like `0xff1d0000` / `0xff390001`). The
> standalone **never runs the original's menu**, so there is no control-flow site to place the four
> stores at faithfully.
>
> **The port already has a stand-in for this frontend initialisation** —
> `TrackRenderer.cpp:371` writes the slot-INDEX table in its race-setup block, which is
> `FUN_0042b960`'s other half. Adding the four stores there is consistent with what the port
> already does, but it is a **BRIDGE placement, not faithful control flow**, and must be labelled
> that way.

## 1. The selection logic, verbatim

In `FUN_0043dfd0`:

```
// multi-player: message 0xff1d0000, and only if FUN_00430760() == 0
iVar8 = FUN_0042b9e0();
if (iVar8 == 0x1000) { ... &DAT_0067e850[i] = 1; &DAT_0067e938[i] = 0; ... }

// single-player: LAB_0043fc37
if ((*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) == 1) &&
    (*(int *)(&DAT_0067ed40 + DAT_0067e9f8 * 0x40) == 0)) {
  FUN_0042b960();
}
```

Both guards are menu-entry-table conditions. Neither is reachable from race setup.

## 2. The two bodies, and a CORRECTION to `RESULT_SLOTWRITER.md`

**`FUN_0042b960`** (109 B, single-player): finds the player's ctrl-block index by scanning the
0x4c-stride table (`DAT_007f1a14` ends as that index), sets `DAT_007f1a24/1a34/1a44 = -1`,
`DAT_007f1a0c = 1`, then `FUN_0040e480(0,1); (1,0); (2,0); (3,0)`.

**`FUN_0042b9e0`** (364 B, multi-player): **starts** with `for i in 0..3: FUN_0040e480(i, 0)`,
requires ≥2 controllers (`if (iVar5 < 2) return 1`), bails on duplicate assignment
(`return 0`), sets the **index** table `0x007f1a14..0x7f1a54` to `-1`, then writes
`FUN_0040e480(local_4, 1)` per human found and `DAT_007f1a0c = 2`.

**Correction.** I previously wrote that `0042b9e0` "sets the whole `0x007f1a14..0x7f1a54` range to
`0xffffffff` rather than `0`". That **conflated two different tables**: it zeroes the four
slot-STATE cells via `FUN_0040e480(i, 0)` (line 53) *and separately* sets the ctrl-block INDEX
table to `-1` (line 104). `FUN_0040e480` writes the state table behind `0x005f2770 + 0x34`; the
`0x007f1a14` range is the index table. They are not the same memory and the earlier sentence was
wrong about which got `-1`.

**`DAT_007f1a0c` is the mode selector** — `1` from the single-player path, `2` from the
multi-player path.

## 3. Which path the measured race took — settled by the measurement itself

`o_e470.csv` measured the original at `{v0: 1, v1/v2/v3: 2}`. That is `FUN_0042b960` (slot 0 = 1,
slots 1-3 = 0) followed by `FUN_00418860` (alive AI → 2). The multi-player path would have
produced `1` on **two or more** slots. **So the scenario ran the single-player path**, and
`FUN_0042b960` is the correct model. That was an open `[UNCERTAIN]` in `RESULT_SLOTWRITER.md` §3
and it is now closed by data already in hand.

## 4. Placement, and why it is a bridge

The port's race-setup block (`TrackRenderer.cpp` ~:368-386) already:

- writes the slot-INDEX table — `Ai::I32(kSlotTableBase + v*stride) = v` for all four cars (`:371`)
- seeds `0x005f2770 = 0x005f2728` behind `MASHED_SLOTSTATE_SEED` (`:386`)
- calls `Ai_ResetRace()`

That is the standalone's stand-in for the frontend initialisation `FUN_0042b960` performs. The four
`FUN_0040e480` stores belong beside those lines — **not because the original calls them there**
(it does not; it calls them from a menu message handler) but because that block is where the port
already models this setup.

**Pre-existing deviation, noted not fixed:** the port writes the index table as **identity**
(`v -> v` for all four), whereas single-player `FUN_0042b960` writes
`[playerBlockIdx, -1, -1, -1]`. So the port already gives all four cars valid indices where the
original gives one. That is a separate, older divergence and this leg does not touch it.

## 5. What is NOT claimed

- Not that adding the stores is faithful control flow. It is a bridge placement, §4.
- Not that the four stores suffice. `FUN_0042b960` also sets `DAT_007f1a0c = 1` and the index
  pattern; whether any consumer of those matters standalone is unmeasured.
- Not that branch 2 then fires — site 105 should drain, but site 99 still accounts for **95.10%**
  of rows (`RESULT_GF1.md`).
- Nothing about `FUN_00492d30` or the rest of `FUN_0043dfd0`, which were not read beyond the two
  call sites.

## 6. Next

1. Add the four stores to the port's race-setup block, **default-OFF**, labelled a bridge, and
   re-read the `lt_exit` histogram: does site 105 drain, and into **114 (FIRE)** or **126**?
2. The index-table identity deviation (§4) deserves its own row — it is older than this lane.
3. Site 99 stays with the race sub-state-machine row (`RESULT_M5.md`).
