# RESULT — the cheap `bias374` test is INERT on its own; the limit table is the unlock

Date 2026-10-08. Follows `RESULT_M5.md` §5 item 1, chosen by **USER DECISION (Mariano,
2026-10-08)**: "Run the cheap test — pin bias374 at 0 and see if branch 2 is clear."

**No run was needed and none was made.** The answer is derivable from data already committed, and
the one new fact was read statically from `MASHED.exe.unpatched`. No build, no code change,
nothing seeded. `original/` untouched. No C-level.

## 0. Verdict first

> **Pinning `bias374` at 0 would change nothing, and committed data already proves it.** The gate
> it feeds reads `0x005f2dd8[bias374 + iVar1*5]`, and **every one of that table's 64 entries reads
> `0` in the standalone** — `GF0-CTL` measured `tbl10 = 0` on 53,992/53,992 rows, and the table is
> image `.data` the standalone never loads. Changing *which* blank entry is read is not a change.
>
> **The table transcription is the actual unlock, and `bias374` only matters once it exists.**
> The two are coupled; neither alone does anything.

## 1. The table, read from the image, cross-checked against the live read

`0x005f2dd8`, file offset `0x1f2dd8`, 64 ints:

```
[ 0..15] 2 1 1 0 0 1 1 0 0 0 1 0 0 0 0 0
[16..31] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
[32..47] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
[48..63] 0 0 0 0 0 0 0 0 1 2 3 4 5 6 7 8
```

**Two independent channels agree to every digit.** This static read of the unpatched image
reproduces the 2026-10-03 witness's **live Frida read** (`RESULT_WITNESS.md:106`): same head of 16,
same **14 of 64** non-zero, same `tbl[10] == 1`. That closes the table as a transcription source —
it needs no further measurement.

## 2. Why `bias374` matters only after the table lands

The gate, `Ai/AiLeaderTimer.cpp:98-99`:

```
limit = tbl[bias374 + iVar1*5]
if (limit <= RankAt) return 0
```

`iVar1 = Ftol(flt360)`; the port's `flt360` is `2.5` (seeded) → `iVar1 = 2`, matching the original.
`RankAt` is `0` on both sides. So:

| state | index | `limit` | gate |
|---|---|---|---|
| standalone today, any `bias374` | 10 or 14 | **0** (table blank) | `0 <= 0` → **return 0** |
| table transcribed, `bias374 = 0` | **10** | **1** | `1 <= 0` false → **PASSES** |
| table transcribed, `bias374 = 4` (the port's terminal band) | **14** | **0** | `0 <= 0` → **return 0** |

So the two fixes are strictly coupled. The table alone is not enough while `bias374` sits at 4; a
pinned `bias374` alone is not enough while the table is blank. **Together they open the `:99` gate
for the first time** — and what lies behind it becomes the next question.

## 3. What is NOT claimed

- **Not that branch 2 then fires.** Opening `:99` only reaches the *next* check in
  `FUN_004148b0`'s chain (`:105` `last == -1`, `:121` `iVar1c != 2`, `:123`/`:126` timer gates).
  Nothing downstream of `:99` has ever been measured on the port, because `FUN_004148b0` has **no
  exe body at all** — that is `PREREG_GATEFIRE.md` §4, the leg still not run.
- Not that pinning `bias374` is the right fix. `RESULT_M5.md` showed the ramp is downstream of the
  standalone having no race sub-state transitions; a pin would be a measurement aid, not a port,
  and the standing rule (`verify/d3_modes37_20261002`) is that seeding globals is not porting.
- No C-level. Nothing executed in this leg.

## 4. Next

The honest sequence is now unambiguous and it is `PREREG_GATEFIRE.md` §4 unchanged:

1. **Branch 2's body** — an exe-side `FUN_004148b0` with the table transcribed as a cited constant
   (now fully resolved, §1) and the four thresholds `6.5 / 5.5 / 6.0 / 4.0`, plus an exe
   `FUN_00442cc0` and the LOS lift. Without a body there is nothing to count, with or without
   `bias374`.
2. `bias374` is then a **measurement knob** for that leg, not a blocker in its own right — hold it
   at `0` to match the original and see whether `GF1-FIRE` approaches 64.

What this leg bought: the table is closed as a source, and the coupling is understood, so neither
item above can be mis-scoped as "one knob away".
