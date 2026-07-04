# D-11057 — Fable Ghidra confirmations (2026-07-03)

The Ghidra-MCP confirmations the claude2 draft handed off to Fable. Read-only pool13.

## ecdc-flag write site (FUN_0043dfd0)
- **Action `0xff300000` → `DAT_0067ecdc = 1`; action `0xff310000` → `DAT_0067ecdc = 2`.** These two arms
  then fill `DAT_0067eaf0..0067eb7f` with `0xffffffff` (stride 0xc) and `goto LAB_0043f203`. They do
  **NOT** write `DAT_0067ed6c` or `DAT_0067ea64`.
  - [UNCERTAIN] the `0xff300000/0xff310000` compares are not literal-immediate CMPs (no `81ff0000030ff`
    byte hit); the value is dispatched via a shifted/jump-table form. The `ecdc=1/=2` results are certain.
- Shared tail **LAB_0043f203**: `FUN_0043d2a0(4,0); DAT_0067ea68 = 1; JMP 0x0043f6d4`.
- The physically-adjacent **`0xff2e0000` arm** (0x0043f18c–0x0043f218, occupies the f1a2–f203 bytes) is
  what writes **`DAT_0067ea64 = 1`** (at 0x0043f1eb), plus `ecdc`, `DAT_0067f17c/f180/f184`,
  `DAT_007f0fe8=0`. Summary: **ea64 is written by the 0xff2e0000 arm, not by the ecdc arms; ed6c by
  neither.**

## Continue-cup modal chain (DAT_0067ed6c = tri-state)
`if (DAT_0067ecdc == 0)`:
- `ed6c == 0` → no modal: `DAT_0067f1a8=0; FUN_0043d2a0(7,0); DAT_0067ea6c=5`.
- `ed6c != 0` → choose variant via `FUN_004307a0()`:
  - `==0` → `FUN_0042bf30(0x136, 0xff270000, 2, 0x2e, 0x2f, 0); ed6c=2; ea6c=5` — **two-choice screen 0x136**.
  - `else` → `FUN_0042bf30(0x135, 0xff280000, 1, 0x2d, 0, 0); ed6c=1; ea6c=5` — **single-confirm screen 0x135**.
- A separate confirm site: `FUN_0043d2a0(7,0); if (ed6c==2) DAT_0067e9fc = 3` (two-choice → SM state 3).

So **`DAT_0067ed6c` gates the continue-cup confirmation modal only** (0=none / 1=0x135 / 2=0x136); it
does NOT hold a length value.

## Game-length / option editing (0x00440283+, the decrement/left handler)
Per-item selector `EDI = DAT_0067ed40[item]`; values edited with fixed wrap ranges:
- `EDI==1` → `DAT_0067ea74` (wrap 0..2)
- `EDI==2` → `DAT_0067ea90` (wrap 1..4)
- `EDI==3` → `DAT_0067ea94` (wrap)
Cursor/key-repeat via `DAT_0067ea88` (re-arms to 2). The length/config values live in
**`DAT_0067ea74 / ea90 / ea94`**, selected by `DAT_0067ed40[item]`. (The symmetric increment/right
handler within 0x00440283..0x00440820 was not separately dumped.)

## kT10 descriptor table — MATCH (31/31, verbatim)
MenuNavSM.cpp cites base `PTR_DAT_005f7638`; index 10 → `0x005f7058`. All 31 LE dwords at 0x005f7058
equal `kT10[0..30]` exactly, including screen ids `0x13f` [1] and `0x153` [9], action codes `0xff300000`
[16] / `0xff310000` [21], the `0xffffffff` sentinel [11], and tail `0xff070000` [30]. **No diffs — the
MenuNavSM.cpp kT10 table is the verbatim original; no port change needed.**
