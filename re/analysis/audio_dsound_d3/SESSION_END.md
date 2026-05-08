# SESSION_END — audio_dsound_d3

Session: audio_dsound_d3-20260508  
Pool: Mashed_pool15  
Date: 2026-05-08  
Parent bucket: audio_dsound_d2-cont1 (D-4000..D-4015)

## Summary

Processed all 16 DEFERRED rows from D-4000..D-4015.
- 15 new C1 plates written (D-4000..D-4007, D-4009..D-4015)
- D-4008 (FUN_005aea50): already mapped in audio_rws_loader_d3 — resolved by reference, no new plate

## RVAs analyzed (15 new + 1 existing)
- 0x005aabe0 — ref-count trampoline to FUN_005adef0
- 0x005bc860 — CreateSemaphore-tracker alloc via FUN_005ae250
- 0x005bc880 — tracker release via FUN_005ae330 (11 bytes)
- 0x005baf40 — audio struct field+subfield setter
- 0x005aaa00 — audio object teardown (vtable dispatch + list unlink + FUN_005aacf0 + FUN_005a9f60)
- 0x005aab70 — vtable+0x30 walker over self + child linked list
- 0x005baa60 — IDirectSound3DBuffer position/orient setter (3-way bit dispatch)
- 0x005bc750 — DirectSound buffer format check/reset (decompiler stack warn U-3194)
- 0x005aea50 — already in hooks.csv (audio_rws_loader_d3)
- 0x005aeea0 — CreateSemaphoreA wrapper
- 0x005aef00 — thread descriptor struct init (corrects S-1370 "thread creation" label)
- 0x005aef30 — __beginthread + SetThreadPriority
- 0x005bc470 — COM audio device init (CoCreateInstance + QI + SetFormat)
- 0x005bc640 — COM audio device teardown (Release x2 + helpers)
- 0x005bbd50 — IDirectSound8::CreateSoundBuffer wrapper (DSBUFFERDESC2)
- 0x005bbed0 — QueryInterface ×2 on IDirectSoundBuffer

## Trackers updated
- hooks.csv: +15 rows (C1, mapped)
- STUBS.md: +S-3180..S-3189 (10 depth-4 stubs)
- UNCERTAINTIES.md: +U-3187..U-3196 (10 uncertainties)
- DEFERRED.md: D-4000..D-4015 all struck through

## New stubs (depth-4 callees)
| Stub | RVA | Caller | Notes |
|------|-----|--------|-------|
| S-3180 | 0x005adef0 | 0x005aabe0 | ref-count/observer insert; U-3187 |
| S-3181 | 0x005ae250 | 0x005bc860 | tracker init (0x12,8,1,0,ptr); U-3188 |
| S-3182 | 0x005ae330 | 0x005bc880 | tracker release; U-3189 |
| S-3183 | 0x005aacf0 | 0x005aaa00 | called (obj,1) and (obj,2); U-3190 |
| S-3184 | 0x005a9f60 | 0x005aaa00 | final teardown call; U-3191 |
| S-3185 | 0x005bc190 | 0x005baa60 | param_3→4-float transform; U-3192 |
| S-3186 | 0x005bc450 | 0x005bc470/bc640 | COM device prep/cleanup |
| S-3187 | 0x005be0f0 | 0x005bc470 | slot init helper |
| S-3188 | 0x005be160 | 0x005bc640 | slot cleanup helper |
| S-3189 | 0x005aef70 | 0x005aef30 | _beginthread wrapper proc |

## Uncertainties (U-3187..U-3196)
10 new uncertainties — all structural/semantic gaps in depth-4 callees and COM IIDs. See UNCERTAINTIES.md for resolution paths.

## No cap reached
All 16 rows processed. cap_count=0 at end. No audio_dsound_d3-cont1 bucket needed.
