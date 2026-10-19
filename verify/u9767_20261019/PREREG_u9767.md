# PREREG u9767: car 1's frame-start forward at call 67 (2026-10-19)

Branch `work/u9767` from `work/u9766` `b34dbbcfa`. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, private root
`D:\MashedWT\u9756_root`. Written before any knob arm. Stack under test: `RLOLCP` (`MASHED_ROTATE_LUT` + `MASHED_ORTHO_LUT` +
`MASHED_A6A_LC0` + `MASHED_ROTATE_PC24`), on which car 1 equals o_wobj2 in every physics field through call 66.

## 0. Measured before this PREREG

- At call 67 both sides start A5 Phase 0 from a forward that is not the end-of-66 `+0x948` (`-0.00666628964` on both): the
  original from `-0.010067312`, the port from `-0.00835652556`. Call 67 is the first steer-0 frame (front steer `-1.3724` at
  66, `0` at 67), so `FUN_0046c7d0` (the steer-0 heading assist, `Race/HeadingAssist.cpp`, default ON) rotated the pose on both
  sides, by different angles.
- The assist reads its heading from `+0x9d4 / +0x9dc` (`[edi + 0x881f74]`). Write watch o_w9d4 (`--watch-write 9d4`; two runs,
  both froze car 1 at about 13 fps, traps at `0x004dc481` inside the SSE vectors slot, so the values are not used, only the
  writer sites): the writers are A5 Phase 0 (`0x0046ddc9`, once per frame) and the getter `FUN_0046d510` (call `0x0046d53a`,
  return `0x0046d53f`, five per frame), which sets `+0x9d4..+0x9dc` = `(0,0,1)` (`0x00614708`) through the vectors slot with the
  slot-0 matrix `rec + 0x928` (`0x00881ec8`), i.e. that matrix's `at` row. Its callers: 12 in the AI step (`0x004143d5..
  0x0041849f`), `0x0042536f / 0x00425467 / 0x00425624 / 0x0042562f`, `0x0044833f`, `0x0047ed35`. The AI tick runs before the
  assist call at `0x00424f1e`.
- The port writes `+0x9d4` only at A5's frame start (`FwdAt`), so its assist reads the previous frame's forward: the
  "read-time" `+0x9d4` lag excluded from earlier diffs is what the assist consumes.

## 1. Hypothesis, knob, arms, gates

H1: with the assist reading (and leaving in `+0x9d4..+0x9dc`) the current `+0x928` `at` row, as `FUN_0046d510` provides,
car 1's call-67 frame-start forward equals the original's. Knob `MASHED_HEADING_FWD_LIVE=1` (default OFF), applied at
`AssistCar`'s entry for every slot the assist processes. [UNCERTAIN] the knob assumes the getter ran for that car earlier in the
frame; the per-frame call set of `FUN_0046d510` is not measured.

Arms: `B_DEF_K0/K1/K26` (G-NOOP); `B_XW0RLOLCPF_K1` (record); `B_RLOLCPF_K0/K1/K26`; `B_F_K0/K1/K26` (the knob on the default);
D2 / 13 tracks / G-CTRL only for a default proposal.

| gate | rule |
|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` |
| G-REC | `XW0RLOLCPF`: car 1's physics fields equal o_wobj2 past call 66 (`firstdiff.py`) |
| G-B | (b) on `RLOLCPF` and `F`: report against the default (A 2/3, B 3/3, C 0/3) |
| G-D2 / G-13 | for a default proposal: D2 3/3, 13 tracks 0 RED, G-CTRL reported |
