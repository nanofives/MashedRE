# RESULT: audiolink slice 5, the four field getters split into Audio/AudioFieldGetters.cpp

PREREG: `PREREG_slice5.md`. Branch `work/audiolink`. Build: exe `439495bc`, dev .asi `8467b4aa`. Evidence:
`D:/MashedWT/audiolink_data/path1/S5_*`, `D:/MashedWT/audiolink_data/arms_slice5.txt`, `disasm_slice5.txt`.

**Verdict: P and G hold.**
- VoiceFieldD6cGet 0x005a89a0, VoiceFieldD70Get 0x005a89b0, VoiceFieldD74Get 0x005a89c0 and ChannelField10Get
  0x005ab370 moved unchanged into Audio/AudioFieldGetters.cpp, which is in both rsp files. In the exe they are dead code
  with no address.
- TimerGetters_ah5.cpp keeps AudioRootGet, AudioTimerRateGet and AudioLinearToLog and stays asi-only.
- Lint 0 NEW.

## Step 1: bytes vs port

Each getter is `mov eax,[esp+4] / mov eax,[eax+off] / ret` (off 0xd6c / 0xd70 / 0xd74 / 0x10). Identical. Despite
its name, the registry's `thiscall_field_get` passes the pointer as a __cdecl stack argument, which matches.

## P / G

- **P:** path1 4 / 4 GREEN, 12 cases each, 0 mismatches.
- **G:** exe `439495bc`. H_DEF-5 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged.

## Trackers (`apply_trackers_s5.py`)

file + exe_file -> Audio/AudioFieldGetters.cpp for the four rows, levels kept.

## Where the lane stands after slices 1-5

15 TUs, 45 functions, one body each in both targets.

The survey's zero-closure items are done. Everything left needs either bytes not yet read or a decision:
- **AudioRws.cpp** (asi-only, 25 functions after the 0x005adf30 move):
  - The pool and list family needs private pools for 0x007dda28 / 0x009146c0 / 0x007dd634 / 0x007ddab0 / 0x007dda80.
    The 0x007dda80 initial value is [UNCERTAIN]: it has to be read from the image.
  - AudioFmtGlobalScan needs the 0x00633674 table bytes.
  - AudioContextLookup needs a body for 0x005ac930.
- **AudioDSound.cpp:**
  - Six leaves are ready.
  - QIChain and SecondaryInit need the GUID bytes at 0x005d09dc / 0x005d09bc and the 0x005e7100 blob.
- **Decisions for Mariano:**
  - The job twins 0x004625b0 / 0x005aeb90.
  - The address-reaching precedents AudioQueuePop / AudioVecLength.
- **Blocked on unported callees:** MixedC3Sweep, AudioListSearch_wfb0f, AudioVoiceQueueSet, AudioPoolFreeGuards and
  AudioRootGet / AudioTimerRateGet.
