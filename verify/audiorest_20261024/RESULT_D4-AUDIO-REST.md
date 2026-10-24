# RESULT: D4-AUDIO-REST, survey of the remaining .asi-only Audio files

Text-only leg. No build, no MASHED boot, no Ghidra. Branch `orch/D4-AUDIO-REST` off `orch/integration` @ `1889fb02d`.
Sources read: the five .cpp files, `mashedmod/asi_sources.rsp`, `mashedmod/exe_sources.rsp`, `hooks.csv`,
`re/analysis/*` plates for 0x005aaac0 / 0x005ac210 / 0x005a8960, `verify/audiolink_20261023/RESULT_rws1.md`,
`Audio/AudioPool.cpp` (the exe-arm precedent).

## Verdict

All five files are **dead code in the exe if linked** (nothing in the exe calls any of their functions: a scan of
`mashedmod/src` finds no caller outside the files themselves). So none of them is a "would replace an AudioEngine
path" case and none needs a decision for Mariano. The cost is the exe arm: each file reads or calls MASHED addresses,
and the ROADMAP gate is "no MASHED address in any exe code path".

**Correction to the task note:** `Audio/AudioCharacterBankPaths.cpp` is no longer .asi-only. It is in
`exe_sources.rsp:114` and `asi_sources.rsp:38` (merged from D4-AUDIO-BANKPATH, `fecd1279b`). Likewise
`AudioLinearToLog` left TimerGetters_ah5 (D4-AUDIO-VOLTODS) and the voice-field getters left it in audiolink slice 5.
Still .asi-only: MixedC3Sweep (asi:179), TimerGetters_ah5 (asi:237), AudioVoiceQueueSet (asi:331),
AudioListSearch_wfb0f (asi:344), AudioPoolFreeGuards (asi:397), AudioRws (asi:39), Rws/RwsStreamRead (asi:404).
The line numbers in the task note (178, 236, 329, 342, 395, 402) are one to two lines early on this branch.
AudioRws.cpp and RwsStreamRead.cpp are outside this survey leg (AudioRws has its own slices, RwsStreamRead is below).

## Ranking (effort low to high)

| Rank | File | Functions | Exe arm needs | Blocker |
|---|---|---|---|---|
| 1 | AudioPoolFreeGuards.cpp | 3 | private pool anchors (3 arrays) | none hard |
| 2 | TimerGetters_ah5.cpp | 2 | private storage for 3 globals | none hard |
| 3 | AudioVoiceQueueSet.cpp | 1 | own semaphore handle, own Win32 calls | none hard |
| 4 | AudioListSearch_wfb0f.cpp | 1 (+1 inline callback) | port of 0x005aaac0 (C2, unported) + private list head | 0x005aaac0 is C2 |
| 5 | MixedC3Sweep.cpp | 1 | 5 callees unported or C2, 2 private globals | 0x005ac210 deferred (U-3030 / U-3031) |

Not in the table: Rws/RwsStreamRead.cpp (see the end).

## Per file

### 1. AudioPoolFreeGuards.cpp (asi_sources.rsp:397)
Functions (`hooks.csv`, all C3, status `impl`, file column = this .cpp):

| RVA | Name | Flag byte | Anchor |
|---|---|---|---|
| 0x005ae380 | AudioPoolFreeGuardA | node+0x00 (`test byte [eax],1`, 0x005ae384) | 0x007dda50 |
| 0x005a6c90 | AudioPoolFreeGuardB | node+0x10 (0x005a6c94) | 0x007dc9e8 |
| 0x005ad8b0 | AudioPoolFreeGuardC | node+0x1c (0x005ad8b4) | 0x007dd6c4 |

Dependencies / tunnels:
- `AudioPoolFree` 0x005ae920, a hard-coded absolute call (`AudioPoolFreeGuards.cpp:46`). It is C3 and already linked
  in both targets as `Audio/AudioPool.cpp` (exe_sources.rsp:113, asi_sources.rsp present). The exe arm can call the
  linked body by name, which is what `AudioPool.cpp` does for its own family ("Calls inside the family go by name").
- Three absolute anchor addresses 0x007dda50 / 0x007dc9e8 / 0x007dd6c4 passed as the pool header. In the exe they must
  become private storage. `AudioPoolFree` reads h[0] (elem), h[1] (cap), h[2] (nbytes), h[4]/h[5] (sentinel at h+4),
  and the byte at h+0x18 (`AudioPool.cpp` body, 0x005ae92b..0x005ae9a4). So the header is at least 7 dwords (0x1c bytes).
  The true allocation size of each anchor is **[UNCERTAIN]**: not read in this leg. `verify/audiolink_20261023/POOL_CONSTRUCTOR.md`
  and `callers_pools.txt` contain no hit for these three addresses. Use an array of 9 dwords like the
  `s_audioNodePool_009146c0[9]` precedent only after the constructor chain (0x005ae650 call sites) confirms the size.
- Callers are the original's C2 functions 0x005a6b70, 0x005ad770, 0x005ae330 (`hooks.csv`: `mapped`, unported), so no
  exe path reaches the guards.

Link blockers: none. Zeroed private storage is the pre-open BSS state (AudioPool.cpp:7-9). A call with zero storage would
walk `blk = h[4] = 0` and fault, exactly as it would in the original before the audio open, which is the same
"nothing calls it" argument slice 2 relied on.

Rsp edits: add `"Audio\AudioPoolFreeGuards.cpp"` to `exe_sources.rsp` after `Audio\AudioPool.cpp` (line 113). Keep it in
`asi_sources.rsp:397`. Link order: after AudioPool.cpp (compile-time order is irrelevant, the dependency is by name).
Hook registration: `RH_ScopedInstall` is an `.asi` registry macro (HookSystem.h:37); the linked files keep it and the
exe ignores the registry (same as AudioPool.cpp:122, :184).

Verification a build worker would need: path1 per-hook A/B for the three exports (the existing registry rows
`log/diff_scenario_batch_audio_pool_free_guard_{a,b,c}.csv` use the `this_reg='stack'` trampoline with the callee stubbed),
re-run on the moved body, plus G-CTRL / B unchanged and lint 0 NEW.

### 2. TimerGetters_ah5.cpp (asi_sources.rsp:237)
After slices 5 and VOLTODS only two functions remain:

| RVA | Name | Reads | hooks.csv |
|---|---|---|---|
| 0x005aad40 | AudioRootGet | `0x007dcd20` (`TimerGetters_ah5.cpp:21`) | C3 impl |
| 0x005ab070 | AudioTimerRateGet | `0x007dcdf8`, gate `0x007dce00 == 1`, else literal 1000 (`:30-34`) | C3 impl |

Tunnels: three absolute data reads, no calls. The file's own footer (`:39-41`) says it stays asi-only because the exe has
no writer for those globals. Nothing in the exe reads them. Exe arm: three private `uint32_t` (zero) behind
`#ifdef MASHED_STANDALONE`, same pattern as `AudioPool.cpp:18-24`. With zero storage `AudioTimerRateGet` returns 1000
(gate 0 != 1), a valid original-reachable value; `AudioRootGet` returns 0.
Related unported neighbours that are NOT in this file and stay out of scope: 0x005ab010 (reads a moving counter, C2) and
0x005aaff0 (C2), both deferred for non-determinism in the file header (`:5-12`).
Rsp edits: add to exe_sources.rsp (suggested next to `Audio\AudioFieldGetters.cpp`, line 110), keep asi line 237.
Verify: path1 on 2 exports (the existing `read_global` / zero-arg scenario rows), gates unchanged.

### 3. AudioVoiceQueueSet.cpp (asi_sources.rsp:331)
One function, 55 bytes: 0x005a8960 `AudioVoiceQueueSet5a8960(base, val)`, C3 (`hooks.csv`: status `mapped`, file column
holds the plate, not the .cpp [UNCERTAIN whether the tracker file column needs a re-classify fix when it moves]).
Body (`AudioVoiceQueueSet.cpp:20-29`): `WaitForSingleObject(*(HANDLE*)0x007dcae0, INFINITE)`; inlined 0x005b0dc0
(`q[1]=val; q[0]|=0x80` at base+0x154); `ReleaseSemaphore(h,1,0)`.
Tunnels:
- `0x007dcae0`, the audio semaphore handle. It is created in the original's stream-cluster init FUN_005a8060 via
  0x005a82d2 (`AudioDSound.cpp:231`). The exe has no writer.
- 0x005b0dc0 (`CmdBuild5b0dc0Set`, C3 in `Util/PromoLoop_sessionB.cpp`) is inlined, so no call. Whether
  PromoLoop_sessionB.cpp is in the exe rsp was not checked and is not needed.
- Win32 imports `WaitForSingleObject` / `ReleaseSemaphore` need `<windows.h>`. The file includes only HookSystem.h and
  relies on it pulling windows.h in [UNCERTAIN, not opened]. Exe links kernel32 already (AudioEngine.cpp uses it).
- Single caller in the original: FUN_0045dc80 (C2, `hooks.csv` status `new`), unported.
Exe arm: private `HANDLE` static (null) behind `MASHED_STANDALONE`. A null handle makes `WaitForSingleObject` return
WAIT_FAILED and then `ReleaseSemaphore` fail, harmless because nothing calls it. Rsp edits as above.
Verify: the existing registry row for this hook plus the usual gates.

### 4. AudioListSearch_wfb0f.cpp (asi_sources.rsp:344)
One exported function plus a file-static callback:

| RVA | Name | hooks.csv |
|---|---|---|
| 0x005aa8a0 | AudioListSearch (42 bytes, cdecl, 1 arg) | C3, status `mapped` |
| 0x005aa8d0 | AudioListSearchCb (data-referenced callback, not a separate hook) | n/a |

Tunnels:
- **0x005aaac0 FUN_005aaac0**, called by absolute address through a function pointer
  (`AudioListSearch_wfb0f.cpp:74`). It is C2, unported, 53 bytes, no callees (indirect call through arg 1 only). The plate
  (`re/analysis/bucket_audio_005a7b60_005ab620/005aaac0.md`) fully describes it: walk `DAT_007dccf0` circular list, call
  `cb(link-4, ctx)` per node, stop on zero return. Porting it is a small leaf, but it must go through `re-classify`
  and needs its own bytes check and path1 before the link.
- **0x007dccf0**, the list head and sentinel. Walked by 0x005aaac0, so the exe needs a private sentinel that is
  self-initialised (`head = &head`), unlike the zero-storage trick: a zero head makes the walk dereference NULL
  (`puVar2 = DAT_007dccf0; if (puVar2 != &DAT_007dccf0)` loop, plate lines above). This is the one place where
  "zero is the pre-open state" is **not** safe if the function is ever called. Dead code, but the build worker should
  initialise it anyway or compile the call out of the exe arm.
- The pushed callback address 0x005aa8d0 (`:74`, the original passes the address of its own inline callback; the port
  passes `&AudioListSearchCb`). In the .asi the port's walker call goes to the ORIGINAL 0x005aaac0, which calls our callback.
Link order: port 0x005aaac0 first (a new leaf in the same file or a sibling), then link this file.
Rsp edits: add to exe_sources.rsp; hold until the 0x005aaac0 port exists, otherwise the exe would contain a call to a
raw MASHED address.

### 5. MixedC3Sweep.cpp (asi_sources.rsp:179)
One function: 0x005abfa0 `AudioWaveLoad` (C3 in hooks.csv, status `mapped`). **Not hooked**: the registration is commented
out (`MixedC3Sweep.cpp:332`, "MASS-DISABLED 2026-05-24 needs-canonical-rws-stream"), so the .asi only exports the symbol.
The deferred 0x005ac210 is a comment in the file header, not code.

Callee map (all from `MixedC3Sweep.cpp:28-102`), with whether the exe can reach a shared body:

| Callee | RVA | Level | Body in exe? |
|---|---|---|---|
| Orig_ReadChunkHdr | 0x005ab380 | C3 AudioRwsChunkHeaderRead | yes, RwsStream.cpp (exe:107) |
| Orig_StreamRead | 0x004cbd30 | C3 RwStreamRead | yes, Render/RwStreamCore.cpp (exe:76) |
| Orig_FmtKeyCmp | 0x005adf30 | C3 AudioFmtKeyCompare | yes, AudioFmtKey.cpp (exe:108) |
| Orig_RawAlloc2 / FreePcmBuf | 0x005aea10 / 0x005aea40 | C3 | yes, AudioMemory.cpp (exe:106) |
| Orig_Bswap | 0x005aec30 | C3 AudioByteSwapBuffer | yes, AudioMemory.cpp |
| Orig_DrainPump | 0x005abf80 | C3 AudioWaveVtableSlot1cDispatch | yes, RwsStream.cpp |
| Orig_RawAlloc | 0x005aea00 | C2 | no body; AudioPool.cpp uses CRT malloc in the exe arm (`:7`, `:29-31`) |
| Orig_Free | 0x004522d0 | C2 | no body; same CRT free |
| Orig_FeedPcm | 0x005abd30 | C2 | **no** |
| Orig_DestroyNode | 0x005abcf0 | C2 | **no** (a C3 sibling `AudioWaveNodeFree` 0x005abcb0 is in AudioRws.cpp, different RVA) |
| Orig_WaveNodeFactory | 0x005ac210 | C2, DEFERRED | **no**, 16 callees, `param_3` = [UNCERTAIN U-3030], inner 0x005aaac0 call = [UNCERTAIN U-3031] |

Globals: `0x007dcaa8` (fmt scratch buffer pointer, `:107`), `0x007dcaac` ([UNCERTAIN U-3029] stream cap, `:110`), format
keys `0x005e6414` / `0x005e6444` (`:112-114`, read-only data in the image; the exe needs its own copy of those constants,
the AudioLinearToLog slice did the same for 0x005cc990 / 0x005cd0b8).
Blockers: three C2 callees with no port (0x005abd30, 0x005abcf0, 0x005ac210) and the last one is blocked on two open
uncertainties. 0x005ac210 also calls 0x005aaac0, so it shares rank 4's leaf. The function has never run under a hook
(registration disabled), so its C3 level rests on the file's synthetic claim, not on a live A/B: before linking, a
bytes check is mandatory (savelink2 found latent defects in 4 of 5 slices, `re/NEXT_SESSION.md`).
Link order: last. Needs 0x005aaac0, 0x005abd30, 0x005abcf0 and 0x005ac210 ported and C2 -> C3 first.
Verify: needs a canonical RWS stream, per the file's own footer; expect the same "needs-canonical-rws-stream" cost.

### Rws/RwsStreamRead.cpp (asi_sources.rsp:404), for completeness
One function, 0x00550950 `RwsStreamRead` (41 bytes): `(size*count)` through a stream vtable slot at `*(stream+0x38)+0x28+8`,
unsigned divide by `size` (`RwsStreamRead.cpp:3-49`). No absolute call, no global: **pure of MASHED addresses**, the
simplest of the whole set. Its blocker is non-audio-engine: it is placed under `Rws/`, nothing in the exe calls it, and the
`div esi` faults on size 0 by design. Rsp edit: one line in exe_sources.rsp. It outranks rank 1 on effort but is
subsystem "audio" by hooks.csv only [UNCERTAIN: the hooks.csv row for 0x00550950 was not opened].

## Recommended slice order

1. **Slice A: RwsStreamRead + AudioPoolFreeGuards** (two tiny files, both no unported callee). One PREREG, shared gates.
2. **Slice B: TimerGetters_ah5 + AudioVoiceQueueSet** (private globals only).
3. **Slice C: port 0x005aaac0** (53-byte leaf, via re-classify, C2 -> C3), then link AudioListSearch_wfb0f.
4. **Slice D: MixedC3Sweep**, only after 0x005abd30, 0x005abcf0 and 0x005ac210 are C3. Probably its own lane (two open
   uncertainties U-3030 / U-3031 and a canonical-RWS-stream test) and should wait on a decision whether to chase it at all,
   since the function is not even hooked.

## Next-step list for the build worker (per slice)

1. Disassemble every function and compare with the port before linking (`py -3.12 re/tools/disasm_fn.py <start> <end>`
   from the main checkout; slice recipe in `re/NEXT_SESSION.md`, "Per slice"). Check both `MASHED.exe` and
   `MASHED.exe.unpatched` for boot patches on the targets (none of the five RVA ranges overlaps the ten patch
   addresses in CLAUDE.md, but that is an address comparison from this survey, not a byte check).
2. Add `#ifdef MASHED_STANDALONE` private storage and leave the `.asi` arm on the original's addresses
   (AudioPool.cpp:18-45 is the template).
3. PREREG committed alone, then edit the two rsp files (`mashedmod/exe_sources.rsp`, keep `mashedmod/asi_sources.rsp`).
4. `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, copy the fresh .asi into the private root, path1 on every moved
   export, then G-CTRL `bacfc747/8dd1c004/6f9932b6` and B `f9deb9b9/d3601fcc/9da9a26a`, lint 0 NEW.
5. Trackers via `re-classify` (file column for the hooks whose `hooks.csv` row currently names a plate, not the .cpp:
   0x005a8960, 0x005aa8a0, 0x005abfa0).

## Not verified in this leg

- No Ghidra reads, so every RVA claim above is from the committed source, `hooks.csv` and the plates. Bytes checks are
  the first build-worker step.
- Pool anchor sizes for 0x007dda50 / 0x007dc9e8 / 0x007dd6c4 and whether the .cpp files include windows.h are
  [UNCERTAIN] as marked.
- `re/tools/dual_copy_allowlist.txt` has no entry for any of these files (grep for Audio / MixedC3 / TimerGetters /
  RwsStreamRead returned nothing), so linking adds no allowlist burn-down item as long as each file keeps one body.
