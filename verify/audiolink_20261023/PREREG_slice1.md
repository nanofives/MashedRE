# PREREG: audiolink slice 1, the 10 zero-closure Audio files linked into the exe

Written before any arm. Branch `work/audiolink` from `88887a23f`. Baseline build (no change): exe `75448b8b`, dev .asi
`316e6eb5`. Change build: exe `71587212`, dev .asi `dc146391`. Lint 8 findings, all allowlisted, 0 NEW.
Survey: `SURVEY.md`. Byte evidence: `disasm_slice1.txt` (`disasm_slice1.py`, from `MASHED.exe.unpatched`; no boot
patch overlaps any of the 29 ranges, checked against `MASHED.exe` in the same script).

## Change

- `exe_sources.rsp` gains RingHeaderInit, AtomicExchangeStore, AudioLeaves_ab1..ab4, AudioChainWalk,
  AudioTableDispatch, AudioList_ag, AudioMusic (29 functions). None has an address tunnel, nothing in the exe calls
  them: dead code in the exe, one body shared with the .asi. RH_ScopedInstall is a no-op in the exe
  (Stubs/HookSystemNoOp.cpp).
- Step 1 (bytes vs port, all 29) found two defects and four order / width gaps, fixed in the shared body:
  - **0x005b4060 DEFECT**: both fistp TRUNCATE (`or ah,0xc` at 0x005b40e7 / 0x005b4125), the port rounded to
    nearest; the product order is (ch * bits) * rate * size (0x005b4087..0x005b40d2), the port had rate * bits * ch.
    Path1's 10 vectors were all integer products or an even tie (1498.5), so they never saw it.
  - **0x005baf00 DEFECT (signaling NaN only)**: the two FLD/FSTP m32 copies quiet an SNaN; the port copied raw bits.
  - 0x005b10a0 / 0x005b10e0: the target's EDX:EAX passes through (nothing touches EAX/EDX after `call [ecx]`), the
    port was void. Now forwards a u64.
  - 0x005ae590: `sete cl / mov eax,ecx` returns a full dword, the port returned bool. Now int.
  - 0x005bb5b0: 8 loads before 4 stores per block. 0x005bfcc0: out1 stored, then the counters re-read. Both now in
    byte order (they only differed on overlapping buffers).
- hooks_registry: 5 new path1 vectors for `audio_fmt_convert_byte_length` (non-integer products).

## Arms (sandbox disabled; explicit PIDs only)

- **C control.** `run_diff.py audio_fmt_convert_byte_length` with the pre-change .asi (main's deployed copy, the
  old body) copied over `mashedmod/build/mashed_re_dev.asi`, then the new .asi put back.
- **P path1.** `run_diff.py <name>` for all 29 registry entries with the change .asi `dc146391`.
- **G gates.** Exe `71587212`, `run_arms.py H_DEF-5 B_DEF_K0 B_DEF_K1 B_DEF_K26`, A4_ROOT D:/MashedWT/savelink_root,
  A4_OUT D:/MashedWT/audiolink_data/arms.

## Predictions

- **C.** RED: at least the 1636-byte vector (old 1092, original 1090) and the 1.5 vector (old 2, original 1).
  Shows the new vectors bite. Other new vectors are reported, not predicted.
- **P.** All 29 GREEN, 0 mismatches, `audio_fmt_convert_byte_length` 15/15.
- **G.** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a, unchanged (exe adds dead code only).

## Not covered

- The SNaN quieting in 0x005baf00 (the `struct_list_float_set` handler seeds only 0.5 + t*0.25). Static evidence only.
- The aliasing orders in 0x005bb5b0 / 0x005bfcc0 (the handlers use distinct buffers).
- U-9789 (new): 0x005b4060 with a rate or size of 2^24 or more, where the double-step argument does not hold.

## Decision rule

- C, P, G hold: commit. hooks.csv: exe_file column for the 29 RVAs, file-column drift fixed for 0x005aefa0, levels
  kept (the two defect fixes keep C3: path1 re-earned on the fixed body), U-9789 added, via re-classify.
- C fails (no RED): the vectors do not bite, report, no commit.
- P fails: report the first failing hook, no commit.
- G fails: stop for Mariano.
