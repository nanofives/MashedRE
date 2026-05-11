# SESSION_END — audio_sfx_dispatch_d3

**Status: COMPLETE**  
**Session ID**: audio_sfx_dispatch_d3-20260511  
**Slot**: Mashed_pool10  

## Pre-session fixes

- D-ID collision resolved: audio_sfx_dispatch-cont1 rows D-7360..D-7375 renumbered to D-7380..D-7395 (track_loader_d4 already held D-7360..D-7367 in the same DEFERRED.md).
- D-7395 (0x005baf60) drift-cleared: already C1 in hooks.csv via audio_sfx_dispatch_d3/audio_dsound_d4 — skipped; 15 of 16 analyzed.
- Prior ABORT (2026-05-07) was stale: audio_sfx_dispatch_d2 was drained by sweep-20260507-2002.

## Functions analyzed (15 C1 plates)

| D-ID | RVA | Name |
|------|-----|------|
| D-7380 | 0x004627b0 | audio device state check (switch DAT_00773920; cases 2/3/4/7→1) |
| D-7381 | 0x005a7f70 | RWS stream node constructor wrapper (→FUN_005aa560 + DAT_007dcae8) |
| D-7382 | 0x005a8890 | audio stream path-ptr attach (mutex + copy to +0xc0/+0xd78 + DLL insert) |
| D-7383 | 0x00462ec0 | DestroyStreams (soundengine.c; 4-stream teardown; clears DAT_006904f0) |
| D-7384 | 0x005a73b0 | audio node system teardown (ctrl-5 to root; N children; FUN_004522d0 free) |
| D-7385 | 0x005b8570 | physical voice node constructor wrapper (→FUN_005aa560 + DAT_007dde70) |
| D-7386 | 0x005a71f0 | voice array registration (alloc tree; FUN_005a6b70 root+N; DAT_007dca34/20/38) |
| D-7387 | 0x005a6280 | virtual voice node constructor wrapper (→FUN_005aa560 + DAT_007dc980) |
| D-7388 | 0x005a6df0 | audio node ctrl-code dispatch null guard (→FUN_005a6d60; 4-arg sig lost) |
| D-7389 | 0x005a5f00 | RWS audio engine creator (FUN_005a5f60 vtable-init; DAT_007dc94c=1) |
| D-7390 | 0x005a6110 | audio engine secondary init (3× FUN_005ad080; ptrs 632c2c/c28/c38) |
| D-7391 | 0x005bbb20 | DirectSound env init (3× FUN_005ad080; ptrs 6351ec/5194/51dc; calls 5bbb70 on fail) |
| D-7392 | 0x004624c0 | audio RtFSManagerRegister (FUN_00551190 + FUN_00550430; logs success/fail) |
| D-7393 | 0x005a9e40 | audio output node vtable attach (null-guards +0x3c fn-ptr; calls (0,param_2)) |
| D-7394 | 0x005a8e70 | output mixing node constructor wrapper (→FUN_005aa560 + DAT_007dcb78; (4,0,0)) |

## Notable findings

- **FUN_005aa560 wrapper family**: four functions (D-7381, D-7385, D-7387, D-7394) are thin 5-arg wrappers over the same RWS audio node constructor `FUN_005aa560`, each binding a different per-type descriptor pointer as arg0.
- **Source path confirmed**: `\toast\Code\src\AppCode\AUDIO\soundengine.c` leaked in D-7383 DestroyStreams.
- **RtFSManagerRegister audio path**: D-7392 (0x004624c0) logs the RtFSManagerRegister call for the audio FS subsystem — callback at 0x005cc4de.
- **D-7388 signature erosion**: Ghidra decompiled `FUN_005a6df0` as single-param; true 4-arg signature recovered from call sites.
- **FUN_005a6d60**: unanalyzed body of the control-code dispatcher — candidate for cont1 if needed.

## Tracker updates

- DEFERRED.md: D-7380..D-7395 removed (all resolved or drift-cleared).
- hooks.csv: 15 new C1 rows appended.
- SCRIBE_QUEUE.md: entry queued (audio_sfx_dispatch_d3-20260511).

## SCRIBE_QUEUE entry

```
2026-05-11  audio_sfx_dispatch_d3-20260511  bucket=audio_sfx_dispatch_d3  rvas=0x004627b0,0x005a7f70,0x005a8890,0x00462ec0,0x005a73b0,0x005b8570,0x005a71f0,0x005a6280,0x005a6df0,0x005a5f00,0x005a6110,0x005bbb20,0x004624c0,0x005a9e40,0x005a8e70  pool=Mashed_pool10  clears=D-7380..D-7394
```
