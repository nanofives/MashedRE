# RESULT — phase 3 IS a countdown. Confirmed, and the capture's lone substate-4 sample proves the chain.

Date 2026-10-08. Follows `SCOPE_SUBSTATE.md` §5 item 3, chosen by **USER DECISION (Mariano,
2026-10-08)**. **Static, read-only** — one `decomp_pc.py` run against a read-only pool clone.
No build, nothing run. `original/` untouched. No C-level.

Raw: `phase3_decomp.txt`.

## 0. Verdict first

> **`FUN_004102f0` is the pre-race countdown / race-start coordinator**, and it closes
> `SCOPE_SUBSTATE.md` §4's `[UNCERTAIN]`. It decrements a countdown global and hands off to the
> next state when two gates clear.
>
> **The transition out of 3 is to 4, not 6** — and the original capture's **single substate-4
> sample** (682 × `3`, **1 × `4`**, 220 × `6`) is exactly that one-frame transient. The chain is
> **3 → 4 → 6**.

## 1. The body, verbatim

```c
void FUN_004102f0(void) {
  if (DAT_005f29b8 == 100000) {            // one-shot init at countdown start
    FUN_00448700(0,0); FUN_004430a0(1); FUN_0040e590(); FUN_0040d470(1);
  }
  iVar3 = 0; iVar2 = 0x34;                 // 4-slot loop over PTR_PTR_005f2770 + 0x34..0x44
  do {
    if (*(int *)(PTR_PTR_005f2770 + iVar2) != 0) {
      if (FUN_0046c7b0(iVar3) == 1) FUN_0046baa0(iVar3);   // per ALIVE car, each frame
    }
    iVar2 += 4; iVar3 += 1;
  } while (iVar2 < 0x44);
  DAT_005f29b8 = DAT_005f29b8 - (uint)(unaff_EBX * 0x44c) / 0x3c;   // the countdown tick
  if (FUN_00405460() == 0 && FUN_004430b0() == 0) {
    DAT_0063ba8c = 4;                      // -> state 4
    DAT_005f29b8 = 12000;
  }
}
```

Ghidra's own header already read it as *"Pre-race countdown / race-start coordinator. Drives the
countdown global `DAT_005f29b8`"* (C1, 2026-06-01). The body confirms it: a timer decremented per
frame, gated handoff, and per-car work while it runs.

## 2. Two things this connects to directly

**(a) It reads the slot-state table this session has been seeding.** The 4-slot loop indexes
`PTR_PTR_005f2770 + 0x34 + i*4` — the exact cells `MASHED_SLOT_PLAYER` writes and `E470` reads
(`RESULT_E470.md`). So the countdown's per-car work is gated on the same substrate, and a port of
phase 3 would need that table live, which it now is under the seed.

**(b) `DAT_005f29b8` is the countdown global, and it is already known dead standalone.**
`RESULT_A360.md` measured `0x005f29b8` reading **0** in the port — image `.data` the standalone
never loads. `FUN_004111c0` case 1 sets it to `0xff` and the shared tail to `100000`/`12000`. So
**sizing (b) in `SCOPE_SUBSTATE.md` §3 is blocked on exactly this global**, and so is any faithful
port of the countdown: the timer has no live value today.

## 3. What this changes about the sizings

**Sizing (a) is no longer a fabrication.** `SCOPE_SUBSTATE.md` §3 called a timer-driven transition
"a bridge that fabricates a transition the port does not earn". That was right when phase 3 was
unidentified; it is now **measured to be a countdown timer**, so modelling it as one is faithful in
kind. It still needs: the real chain **3 → 4 → 6** (not 3 → 6), a live `DAT_005f29b8`, and the two
exit gates `FUN_00405460` / `FUN_004430b0` — none of which is a constant.

**Sizing (c) shrinks slightly.** `FUN_004102f0` is **172 bytes**, not part of the 13.7 KB
dispatcher's bulk. The state machine's *per-state helpers* are small; `FUN_004111c0`'s size is
mostly case 1 (race init + spawn loop) and the shared tail.

## 4. What is NOT claimed

- Not that porting `FUN_004102f0` alone gives the port a countdown. Its caller
  (`FUN_004111c0` case 3) and the states around it are what sequence it, and `DAT_005f29b8` must
  become live first.
- Not what `FUN_0046baa0`, `FUN_00405460` or `FUN_004430b0` do — unread. The first is per-alive-car
  work during the countdown; the latter two are the exit gates.
- Not that the port's 683-call deficit is *only* the countdown. Phase 3 lasts 683 calls in the
  capture; whether a ported countdown would last the same is unmeasured.
- No C-level. Nothing was executed.

## 5. Next

1. The `DEFERRED.md` row from `SCOPE_SUBSTATE.md` §5 should now record: phase 3 = countdown
   (`FUN_004102f0`, 172 B), chain **3 → 4 → 6**, blocked on `DAT_005f29b8` being dead standalone,
   and sharing the slot-state substrate already seeded.
2. `FUN_00405460` / `FUN_004430b0` are the two exit gates and are the next cheap reads if anyone
   takes the row.
3. Nothing in this leg changes any default or any gate verdict.
