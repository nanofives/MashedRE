# RESULT: audiolink slice 1, the 10 zero-closure Audio files linked into the exe

PREREG: `PREREG_slice1.md` (+ amendment 1). Branch `work/audiolink`. Build: exe `71587212`, dev .asi `dc146391`
(baseline exe `75448b8b` / .asi `316e6eb5`). Evidence: `D:/MashedWT/audiolink_data/path1/*.txt`,
`D:/MashedWT/audiolink_data/arms_slice1.txt`, `disasm_slice1.txt`.

**Verdict: C, C2, P and G hold.** RingHeaderInit, AtomicExchangeStore, AudioLeaves_ab1..ab4, AudioChainWalk,
AudioTableDispatch, AudioList_ag and AudioMusic (29 functions) are in both rsp files with one body each. In the exe they
are dead code with no address. Lint 8 findings, all allowlisted, 0 NEW.

## Step 1: bytes vs port (29 functions, `disasm_slice1.txt`)

22 identical. Fixed in the shared body:

| RVA | was | bytes | fix |
|---|---|---|---|
| 0x005b4060 | round-to-nearest, order rate*bits*ch | `or ah,0xc` truncation at 0x005b40e7 / 0x005b4125, order (ch*bits)*rate*size | truncate, byte order, each step double -> float (U-9789 above 2^24) |
| 0x005baf00 | raw-bit copies | FLD/FSTP m32 quiets an SNaN | `X87CopyF32` |
| 0x005b10a0 / 0x005b10e0 | void | target EDX:EAX passes through | u64 forward |
| 0x005ae590 | bool | `sete cl / mov eax,ecx` | int 0 / 1 |
| 0x005bb5b0 | per-sample load/store | 8 loads, then 4 stores per block | block order |
| 0x005bfcc0 | counters read once | out1 stored, then re-read | byte order |

## Controls (old .asi `270e036f`, main's deployed copy)

- **C** `audio_fmt_convert_byte_length` (15 vectors, 5 new): RED idx 10 (original 0x442 = 1090, old 0x444 = 1092) and
  idx 11 (1 vs 2). The other three new vectors agree under both rounding modes.
- **C2** `music_group_volume_set` (early_window_leaf_diff, node+0x1c bit table): RED t=1 (7fc00001 vs 7f800001),
  t=4 (ffc00001 vs ff800001), t=6 (7fffffff vs 7fbfffff). GREEN t=0, 2, 3, 5 (zero, QNaN, denormal, 1.0).

## P: path1 on the shared body (.asi `dc146391`)

29 / 29 GREEN, 0 mismatches (`run_diff.py` for 28, `early_window_leaf_diff.py` for `music_group_volume_set`, which
has no `diff_template.js` handler). `audio_fmt_convert_byte_length` 15/15, `music_group_volume_set` 7/7.

## G: gates (exe `71587212`, root D:/MashedWT/savelink_root)

H_DEF-5 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged.

## Trackers (`apply_trackers.py`, re-classify)

exe_file set for the 29 rows, 0x005aefa0 file column fixed, notes on the 7 fixed bodies, levels kept. U-9789 added.

## Not covered

- An SNaN volume argument at struct+0x38 (the harness passes a JS number).
- The overlap orders of 0x005bb5b0 / 0x005bfcc0 (handlers use distinct buffers).
- 0x005b4060 above 2^24 (U-9789).

## Next in the survey order

AudioMemory (malloc / free thunks), RwsStream (0x005ab380 arity and 0x005abf80 CALL / JMP first), the 0x005adf30 split
+ RwsFmt, the TimerGetters_ah5 getter split, then AudioRws and the AudioDSound leaves. The two job twins (0x004625b0,
0x005aeb90) wait for Mariano.
