# RESULT: audiolink slice 4, AudioFmtKeyCompare split into Audio/AudioFmtKey.cpp, RwsFmt linked into the exe

PREREG: `PREREG_slice4.md` (+ amendment 1). Branch `work/audiolink`. Final build: exe `4004ebb1`, dev .asi `2f5b419d`
(first build exe `a4f9ed2f` / .asi `31a08f99`). Evidence: `D:/MashedWT/audiolink_data/path1/S4*_*`,
`D:/MashedWT/audiolink_data/arms_slice4.txt`, `arms_slice4b.txt`, `disasm_slice4.txt`.

**Verdict: P failed on the first run and holds after the amendment. G holds on both builds.**
- 0x005adf30 AudioFmtKeyCompare moved from Audio/AudioRws.cpp (asi-only) into Audio/AudioFmtKey.cpp + .h, in both rsp
  files.
- RwsFmt.cpp (0x005ac5f0 AudioFmtDescEqual, 0x005ac9e0 AudioFmtEntryMatch) is in both rsp files and calls the key
  compare by name. No address is left in the exe arm.
- In the exe these are dead code. Lint 0 NEW.

## Step 1: bytes vs port

All three bodies matched the bytes (`disasm_slice4.txt`). Argument order to the key compare: (p1[1], p2[1]) at
0x005ac600 / 0x005ac601 and (cand[1], entry[0]) at 0x005aca11 / 0x005aca12.

## First run (exe `a4f9ed2f`, .asi `31a08f99`): P RED

| Hook | Result |
|---|---|
| audio_fmt_key_compare | 15 / 15 GREEN |
| audio_fmt_entry_match | 10 / 10 GREEN |
| **audio_fmt_desc_equal** | **9 / 10, RED idx 2** |

Idx 2 passes the key pointers 0xaaaaaaaa / 0xbbbbbbbb. Both sides fault, at different addresses: the original at
0xbbbbbbbb, the shared body at 0xaaaaaaaa. Control: the slice-3 .asi (RwsFmt calling the original 0x005adf30 by
address) is 10 / 10 GREEN, both faulting at 0xbbbbbbbb.

So `repe cmpsb` (0x005adf4a) faults on the ES:[EDI] byte (param_2) first. The port's loop read param_1 first. Splitting
out the key compare is what exposed this: before, RwsFmt ran the original's compare on both sides. The key-compare
harness never passes an invalid pointer, so its 15 / 15 could not see it. G was unchanged on this build.

## Fix and re-run (exe `4004ebb1`, .asi `2f5b419d`)

The shared body loads param_2's byte before param_1's through volatile reads. The compiled loop is `mov cl,[edx]`
(param_2) then `mov al,[edi+edx]`.
- **P:** 3 / 3 GREEN (15, 10, 10). desc_equal idx 2 faults at 0xbbbbbbbb on both sides.
- **G:** H_DEF-5 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged.

## Trackers (`apply_trackers_s4.py`)

- 0x005adf30: file + exe_file -> Audio/AudioFmtKey.cpp, with a note on the load order.
- exe_file for the two RwsFmt rows.
- Levels kept.

## Not covered

AudioRws.cpp's own AudioFmtGlobalScan still calls 0x005adf30 by address (asi-only, a later slice).

## Next in the survey order

The four TimerGetters_ah5 field getters (0x005a89a0 / b0 / c0, 0x005ab370) after a file split. Then AudioRws (pools /
lists need private storage, 0x007dda80 initial value [UNCERTAIN]) and the AudioDSound leaves.
