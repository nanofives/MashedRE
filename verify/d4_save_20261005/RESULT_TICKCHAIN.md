# RESULT — the save flow is a SIX-VARIABLE state cluster, not one selector. That explains the void.

**RAN 2026-10-05.** Static reads via `re/tools/decomp_pc.py` (read-only pool slot). **No C-level
moved, no band moved, no code changed, `original/` untouched.** Follow-on to `RESULT_POKE.md`.

---

## 1. The tick chain, traced

```
FUN_00492d30
   └─> FUN_0043d7c0            frontend tick, 1791 bytes
         │                     keyed on DAT_0067eab0 (transition-state enum);
         │                     reads DAT_0067eca4 — the SAME global orig_nav_hold.py
         │                     calls "phase"
         ├─> FUN_0043d2a0      the nav push/pop I was driving
         └─> FUN_0040ac80      "companion state machine", keyed on DAT_008a9584
```

So the frontend tick **does** reach save-adjacent machinery every frame — `FUN_0040ac80` is live at
the menu. The subsystem is wired into the menu loop.

## 2. Why the poke was never going to work

`FUN_0040ac80` switches on **`DAT_008a9584`**, and in `case 1` it *reads* `DAT_008a9588` only as a
**condition**:

```c
case 1:
  if ((5 < DAT_008a9588) && ((DAT_008a9588 < 9 || (DAT_008a9588 == 0xb)))) {
      DAT_008a9584 = 0;
  }
  return;
case 2:
  bVar1 = DAT_008a958c != 7;
  DAT_008a9584 = 0;  DAT_008a958c = 6;
  if (bVar1) { DAT_008a9598 = 8;  DAT_008a9590 = 0xb; }
  return;
case 4:
  if ((5 < DAT_008a9594) && (DAT_008a9594 < 0xb)) { FUN_0042c1a0(); DAT_008a9584 = 0; }
  return;
```

**That is six state variables in one cluster** — `DAT_008a9584`, `008a9588`, `008a958c`, `008a9590`,
`008a9594`, `008a9598` — reading and writing each other.

**I poked one of six.** `DAT_008a9588` is a *consumed* value here, not the driver; the driver in this
function is `DAT_008a9584`. And note `case 1`'s guard — `5 < DAT_008a9588 < 9` or `== 0xb` — would
be **false** for the `0xd` I wrote, so even the one machine that is ticking would have ignored it.

**That fully explains `RESULT_POKE.md`'s void**, and it explains it better than "the dispatcher never
runs": a dispatcher somewhere may well run, but nothing in the live path consumes `0xd`, and the
value I chose was outside every guard that reads it.

## 3. What is still not located

**The dispatcher at `0x00409b0e` and its caller.** Its containing region is one of the Ghidra-missed
areas (`--callers` returns `(none)`; `--datarefs` shows the calls). It reads `DAT_008a9588` at
`0x00409b00` and jumps through `switchdataD_00409e40`, whose jumptable Ghidra could not recover
("Too many branches"). Whoever continues needs that jumptable resolved or the enclosing function
created.

The third reader, at `0x0040aa60`, is also in a Ghidra-missed region and was not traced.

## 4. The honest close on this question

**Does the original accept the standalone's `gamesave.bin`? Still unanswered**, after eight steps in
one session. Every step was measured; none was guessed; and the sequence is a fair record of a
question that kept turning out to be one level deeper than it looked:

1. `0x00803358` — wrong address class. VOID.
2. `*0x008a94a8` — superseded, never run.
3. `0x007F0A40`, race-warped — right address, wrong harness. VOID on a degenerate control.
4. `0x007F0A40`, menu hold — right harness, file still unread.
5. nav scan — **a phantom**: push works; my reading lacked a baseline.
6. screens are the wrong dimension — the trigger is a state machine.
7. the state machine looked dormant; poking its selector did nothing.
8. **it is a six-variable cluster; I poked a consumed value, with a number outside every guard.**

**The productive output of the save thread is unaffected and is all committed:** the four
`GameSave` functions are live in `mashed_re.exe` with 5/5 and 3/3 gates, the file layout is fully
resolved (`RESULT_U3559.md`), `gamesave_parse.py`'s live constant is fixed, and `--no-warp` /
`--poke-u32` are reusable harness additions documented as contrived where they are.

**Recommendation: stop this thread here.** It is bounded to one named unknown — the `0x00409b0e`
jumptable and its caller — and that is a Ghidra job for a fresh session, not another runtime probe.
