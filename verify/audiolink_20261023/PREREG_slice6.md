# PREREG: audiolink slice 6, the byte-read step and Audio/AudioDSound.cpp linked into the exe

Written before any arm. Branch `work/audiolink` on `731680ae3`. Build: exe `36811066`, dev .asi `98bacf4b`
(slice 5: exe `439495bc`, .asi `8467b4aa`). Lint 8 findings, all allowlisted, 0 NEW.
Bytes: `bytes_step.txt` (`read_bytes.py` on MASHED.exe.unpatched), `disasm_slice6.txt`.

## Byte-read step (all from the anchored image)

- 0x005d09dc = IID_IDirectSound3DListener and 0x005d09bc = IID_IKsPropertySet. Byte-equal to the Windows SDK
  `dsound.h:861` / `:1212`. This resolves U-0360.
- 0x005e7100: 16 bytes of GUID {CD5368E0-3450-11D3-8B6E-00105A9B7BBC}, which no SDK header names, followed by floats.
  It is the REFGUID argument of IKsPropertySet slot 5 = QuerySupport (dsound.h vtable order), so 16 bytes are read.
- 0x00633674: 9 pointers 0x005e6414 / 6424 / 6434 / 6444 / 6454 / 64a4 / 6464 / 6474 / 64b4 into .rdata 16-byte keys
  (0x005e6414 + 0xb0 dumped).
- 0x007dda80, 0x007dda28, 0x009146c0, 0x007dd634, 0x007ddab0: all BSS (zero at load). Their values come only from
  runtime writers, so the survey's "0x007dda80 initial value" is 0 in the image. The runtime constructor is still
  unknown.
- 0x005bafdc (Release's jump table): 005bafbe 005bafb8 005bafd6 005bafd6 005bafc4 005bafca 005bafd0. Matches the port.

## Change

- AudioDSound.cpp joins `exe_sources.rsp`: BufFieldSet 0x005baf60, Release 0x005baf90, QIChain 0x005bc400,
  SemaphoreCreate 0x005aeea0, SecondaryInit 0x005bbfc0, ThreadDescInit 0x005aef00 and OutputNodeCbDispatch 0x005a9e40.
- The three GUIDs are file constants with the image's bytes, used in both arms (QI / QuerySupport read the bytes, not
  the address).
- AudioSubStructTwoCallInit 0x005a9e10 moves to AudioRws.cpp (asi-only). It calls that file's LinkBuffer / LinkDevice,
  which need the pool family.
- Step 1: six bodies match the bytes (SemaphoreCreate's comment corrected: `neg / sbb` is a full 32-bit mask).
  **0x005bbfc0 has two defects:** a failed QI returns 0 with no Release (`jl 0x005bc014` at 0x005bbfe0, the port
  released pUnk). On a failed or non-3 QuerySupport the bytes release the QI output ks (`[esp]` at 0x005bc00a), the
  port released pUnk. Re-ported from the bytes.
- Harness: `dsound_secondary_init` gains per-test modes (t % 4: success / QI fails / QuerySupport fails / writes 2),
  separate outer and inner Release counters, and the GUID bytes each call receives. The old handler had only the
  success path.

## Arms (sandbox disabled; explicit PIDs only)

- **C control.** `run_diff.py audio_dsound_secondary_init` with the pre-change .asi `2f5b419d` (old body).
- **P path1.** `run_diff.py` for audio_buf_field_set, audio_dsound_release, audio_dsound_qi_chain,
  audio_semaphore_create, audio_dsound_secondary_init, audio_thread_desc_init, audio_output_node_cb_dispatch, with
  .asi `98bacf4b`.
- **G gates.** Exe `36811066`, `run_arms.py H_DEF-5 B_DEF_K0 B_DEF_K1 B_DEF_K26`, root D:/MashedWT/savelink_root.

## Predictions

- **C.** RED on t = 1, 2, 3, 5, 6, 7, 9 (the old port releases the outer object: ro1 where the original has ro0, and
  ri0 where it has ri1). GREEN on t = 0, 4, 8 (success path, no Release).
- **P.** 7 / 7 GREEN, 0 mismatches. secondary_init 10 / 10 with the original's ri1 on modes 2 / 3 and no Release on
  mode 1. The GUID bytes seen by the stubs are equal on both sides.
- **G.** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a, unchanged.

## Decision rule

C, P, G hold: commit. hooks.csv: exe_file for the seven rows, file move 0x005a9e10 -> Audio/AudioRws.cpp, U-0360
resolved, 0x005bbfc0 defect note, levels kept (path1 re-earned on the fixed body), via re-classify. C fails: the
handler does not bite, no commit. P fails: report, no commit. G fails: stop for Mariano.
