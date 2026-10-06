# PRE-REGISTRATION — drive the save state machine and read the verdict

**Committed UNRUN. 2026-10-05.** Anchor verified (`BDCAE093…3C0E` / `0150…8DA2`).

## 1. What is poked, and that it is contrived

`DAT_008a9588` is the selector of the switch at `0x00409b0e` — **measured**: `0x00409b00` does
`MOV EAX,[0x008a9588]` immediately before the jump, and `0x00409ca8` writes `0xd`, the case
(`switchD_00409b0e::caseD_d`) that calls `SaveLoad` (`0x00404e50`).

**Poke: `008a9588 = d`**, once, 6 s into a `--no-warp` menu hold.

**This is CONTRIVED STATE, C3-grade**, the same class as the existing `pokeLap` / `pokeCtrlSlots`.
It writes a value the game's own code writes, to reach a state this harness otherwise cannot. **A
PASS proves the original CAN read and parse the standalone's file. It does NOT show the game reaches
that state on its own**, and no claim about the normal flow follows from it.

## 2. Arms, each a separate `MASHED_ROOT` install copy

- **arm P (positive control)** — the shipped save with `r2c1` edited `0 → 1` (file `0x24aa4`).
- **arm S (subject)** — the standalone's `mashed_re_gamesave.bin`.

Peeks: `00803358:u` (buffer magic), `007f0aa4:u` (live champ r2c1), `007f0a44:u` (live champ r0c1).

## 3. Gates

**KA-POKE — the control, and it can void everything.** On **arm P**, after the poke,
`0x00803358` must read **`0xDEADBEEF`** on **>= 1** sample. If it never does, the poke did not reach
the load — the state machine may not be ticked at the menu, or `0xd` is not the live case — and the
leg is **VOID**. No statement is then made about arm S.

**KA-PROP — does the loaded data reach the live table?** Still on arm P, after the magic appears,
`0x007f0aa4` must read **1** (the edited cell). This is the discriminator the whole thread has
lacked: it separates "the file was read" from "the file was read AND deserialised into live state".
If the magic appears but `r2c1` stays `0`, report **READ-BUT-NOT-APPLIED** and do not call it
acceptance.

**G-ACCEPT — the verdict, scored only if KA-POKE and KA-PROP both pass.** On **arm S**:

- **ACCEPTED** — `0x00803358` reads `0xDEADBEEF`, i.e. the original read and parsed our file.
  Expected live table values are the standalone's span: `r0c1 = 0`, `r2c1 = 0`.
- **REJECTED** — the magic never appears on arm S while it did on arm P with the same poke.
- **INCONCLUSIVE** — anything else, raw values printed.

**Registered prediction: ACCEPTED on arm S.** The file has the right size, the right magic on disk,
and a structurally valid span with `col4` in the documented `{0,2}` domain. A REJECTED result would
mean the loader validates something none of this session's offline work found.

## 4. Safety

`original/` is never written — both arms are scratch copies launched via `MASHED_ROOT`, and the
repo's `original/gamesave.bin` SHA is re-verified afterwards. `--peek` is a plain read; the only
write is the single declared poke. PID hygiene: only the spawned PID is killed.

## 5. Not in scope

Whether the game reaches state `0xd` unaided; what enters the `0x00409b0e` machine; the profile
block (all-zero in both files); any C-level move.
