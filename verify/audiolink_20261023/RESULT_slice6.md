# RESULT: audiolink slice 6, the byte-read step and Audio/AudioDSound.cpp linked into the exe

PREREG: `PREREG_slice6.md`. Branch `work/audiolink`. Build: exe `36811066`, dev .asi `98bacf4b`. Evidence:
`D:/MashedWT/audiolink_data/path1/S6*`, `D:/MashedWT/audiolink_data/arms_slice6.txt`, `bytes_step.txt`,
`disasm_slice6.txt`.

**Verdict: C, P and G hold.** These seven are in both rsp files with one body each:

| RVA | Function |
|---|---|
| 0x005baf60 | AudioBufFieldSet |
| 0x005baf90 | AudioDSoundRelease |
| 0x005bc400 | AudioDSoundQIChain |
| 0x005aeea0 | AudioSemaphoreCreate |
| 0x005bbfc0 | AudioDSoundSecondaryInit |
| 0x005aef00 | AudioThreadDescInit |
| 0x005a9e40 | AudioOutputNodeCbDispatch |

Neither arm has a MASHED.exe address: the three GUIDs the original passes by address are file constants with the
image's bytes. AudioSubStructTwoCallInit 0x005a9e10 moved to AudioRws.cpp, which stays asi-only. Lint 0 NEW.

## Byte-read step (`read_bytes.py`, MASHED.exe.unpatched)

| Address | Content |
|---|---|
| 0x005d09dc | IID_IDirectSound3DListener (byte-equal to dsound.h:861). **U-0360 resolved.** |
| 0x005d09bc | IID_IKsPropertySet (dsound.h:1212) |
| 0x005e7100 | GUID {CD5368E0-3450-11D3-8B6E-00105A9B7BBC}, named in no SDK header, then floats. It is the REFGUID of QuerySupport (IKsPropertySet slot 5), so 16 bytes are read. |
| 0x00633674 | 9 pointers into .rdata 16-byte keys 0x005e6414..0x005e64b4 (dumped). Unblocks AudioFmtGlobalScan. |
| 0x007dda80, 0x007dda28, 0x009146c0, 0x007dd634, 0x007ddab0 | BSS, zero at load. The values are runtime-only. Their constructor is still unknown. |
| 0x005bafdc (Release jump table) | 005bafbe 005bafb8 005bafd6 005bafd6 005bafc4 005bafca 005bafd0. Matches the port. |

## Step 1: bytes vs port

- Six bodies matched. The SemaphoreCreate comment is corrected (`neg / sbb` is a full 32-bit mask).
- **0x005bbfc0 had two defects:**
  - A failed QI returns 0 with no Release (`jl 0x005bc014` at 0x005bbfe0).
  - On a failed or non-3 QuerySupport, the bytes release the QI output ks (`[esp]` at 0x005bc00a).

  The port released pUnk in both cases. It is re-ported from the bytes.
- **U-0362 refuted:** the `== 3` test reads the support word that QuerySupport writes into the caller's param_1 slot
  (0x005bbfe6 / 0x005bbffc).
- Harness `dsound_secondary_init`: per-test modes (success / QI fails / QuerySupport fails / writes 2), separate outer
  and inner Release counters, and the 16 GUID bytes each call receives.

## C control (old .asi `2f5b419d`)

RED on t = 1, 2, 3, 5, 6, 7, 9 exactly as predicted:
- Mode 1 (QI fails): original `r0 n1 ro0 ri0`, old port `r0 n2 ro1 ri0`.
- Modes 2 / 3: original `ri1`, old port `ro1`.

GREEN on 0, 4, 8. The GUID bytes seen by the stubs are equal on both sides.

## P: path1 (.asi `98bacf4b`)

7 / 7 GREEN, 0 mismatches:

| Hook | Cases |
|---|---|
| buf_field_set | 10 |
| dsound_release | 16 |
| qi_chain | 8 (8 distinct outputs) |
| semaphore_create | 10 |
| secondary_init | 10 |
| thread_desc_init | 10 |
| output_node_cb_dispatch | 6 |

## G: gates (exe `36811066`)

H_DEF-5 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged.

## Trackers (`apply_trackers_s6.py`)

- exe_file for the seven rows.
- File move 0x005a9e10 -> Audio/AudioRws.cpp.
- 0x005a9e40 file-column drift fixed.
- U-0360 and U-0362 resolved.
- Levels kept.

## Lane total after slices 1-6

16 TUs, 52 functions, one body each in both targets.

Left:
- **AudioRws.cpp** (26 functions, asi-only):
  - The table bytes for AudioFmtGlobalScan are now in hand.
  - The pool / list family needs private storage, and those globals are BSS with no exe writer.
  - AudioContextLookup needs 0x005ac930.
- The job twins and the two precedents (Mariano).
- The blocked files listed in `RESULT_slice5.md`.
