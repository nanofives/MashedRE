# Session End — audio_music_d2

**Session ID:** audio_music_d2-20260506
**Pool slot:** Mashed_pool2
**Binary anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓
**Parent:** session GG / audio_music batch 6

## Work completed

Analyzed all 12 depth-2 stubs (S-0620..S-0631) from the parent audio_music session. Two were already resolved in prior sessions; 10 new analysis notes written.

### Functions analyzed (new — C1)

| RVA | Name in Ghidra | Notes file | Key finding |
|---|---|---|---|
| 0x005a66d0 | FUN_005a66d0 | 005a66d0.md | Play/stop dispatcher; param_2=0→play, !=0→stop+write DAT_007dca50 to obj+0x54; tail-call JMP to FUN_005a7520 |
| 0x005a6dc0 | FUN_005a6dc0 | 005a6dc0.md | Null-guard wrapper for FUN_005a6d60; passes param_4 by address |
| 0x0045e0f0 | FUN_0045e0f0 | 0045e0f0.md | Per-channel volume setter; clamps + writes obj+0x40 + FUN_005a6dc0(type=5) + scale array |
| 0x00431b20 | FUN_00431b20 | 00431b20.md | fsin(DAT_007f0f00 × _DAT_005cd8f0) float10; phase-channel A |
| 0x005baf00 | FUN_005baf00 | 005baf00.md | Music group volume; +0x38=vol; circular list nodes: +0x14\|=0x40; secondary +0x30 |
| 0x0042f760 | FUN_0042f760 | 0042f760_0042f770_0042f780.md | returns DAT_0067f19c; channel-B trigger flag |
| 0x0042f770 | FUN_0042f770 | (same file) | returns DAT_0067f1a0; channel-C trigger-A |
| 0x0042f780 | FUN_0042f780 | (same file) | returns DAT_0067f1a4; channel-C trigger-B |
| 0x00432230 | FUN_00432230 | 00432230_00432260.md | state[idx*0x40]==0x13 && sub-state==1; channel-D trigger |
| 0x00432260 | FUN_00432260 | (same file) | same predicate but sub-state==2; musicloop1 trigger |

### Already resolved (cross-session)

| RVA | Resolved in | Notes |
|---|---|---|
| 0x00432290 (S-0624) | timer_d2_cont1 | DAT_0067eab0!=0 && DAT_0067eabc in {0xFF210000, 0xFF220000} |
| 0x00431b60 (S-0626) | timer_d2_cont1 | fsin(DAT_007f0f08 × _DAT_005cd8f0); phase-channel B |

## Stubs introduced (depth-3, deferred)

| ID | RVA | Description | D-range |
|---|---|---|---|
| S-2200 | 0x005a7520 | Audio state dispatcher; mode 0/1/2 → stop/play/conditional | D-6520 |
| S-2201 | 0x005a6d60 | Actual audio parameter setter; 4 args; param_4 by address | D-6521 |

## Uncertainties introduced

U-2207 through U-2219 (13 entries). Key open items:
- **U-2207**: DAT_007dca50 written to obj+0x54 on stop — type unknown
- **U-2209**: FUN_005a6d60 receives param_4 by address — ABI pattern unknown
- **U-2210**: param_type=5 vs =4 in FUN_005a6dc0 calls — field selection unknown (resolves with D-6521)
- **U-2215**: Trigger flags DAT_0067f19c/a0/a4 — writers unknown
- **U-2217/2218**: State enum 0x13 and index DAT_0067e9f8 — semantic unknown

## Depth-3 continuation

Queued as `audio_music_d2-cont1` in re/queue.md.
D-6520: FUN_005a7520 (audio state dispatcher)
D-6521: FUN_005a6d60 (actual parameter setter — also resolves U-2209, U-2210)

## Tracker changes

- **hooks.csv**: +10 rows (005a66d0, 005a6dc0, 0045e0f0, 00431b20, 005baf00, 0042f760, 0042f770, 0042f780, 00432230, 00432260) all C1/audio/mapped
- **STUBS.md**: S-0620..S-0631 → resolved; +S-2200, S-2201 (passthrough)
- **UNCERTAINTIES.md**: +U-2207..U-2219
- **re/queue.md**: +audio_music_d2-cont1 section
