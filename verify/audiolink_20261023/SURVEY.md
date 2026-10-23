# audiolink Step 0: worker survey (2026-10-23)

Three account2 worker runs (opus, read-only), raw outputs below unedited. Group A = AudioRws + AudioDSound,
B = RwsStream, RwsFmt, MixedC3Sweep, AudioMemory, AudioMusic, AudioCharacterBankPaths, C = the 13 small files.

## Consolidated link order (smallest closure first)

**(a) dead code in the exe, closure = the file itself, no #ifdef needed**
1. RingHeaderInit, AtomicExchangeStore, AudioLeaves_ab2, AudioChainWalk, AudioTableDispatch, AudioList_ag,
   AudioLeaves_ab1, AudioLeaves_ab3, AudioLeaves_ab4, AudioMusic (10 files, ~30 functions)

**(a) dead code, needs host thunks / private storage**
2. AudioMemory (malloc/free thunks for 0x005aea00 / 0x004522d0, RwStreamCore.cpp:51-60 precedent)
3. RwsStream (RwStreamCoreRead/Skip, private tree root for DAT_009146fc). Open: 0x005ab380 arity, 0x005abf80 CALL vs JMP
4. AudioFmtKeyCompare 0x005adf30 split out of AudioRws.cpp, then RwsFmt
5. TimerGetters_ah5 four field getters (after a file split)
6. AudioRws.cpp (pool/list family needs private pools; 0x007dda80 initial value [UNCERTAIN])
7. AudioDSound.cpp leaves (BufFieldSet, Release, SemaphoreCreate, ThreadDescInit, CbDispatch, TwoCallInit)

**(b) job twin with an exe path: decision for Mariano**
- 0x004625b0 AudioCharacterBankPathsBuild vs AudioEngine::CharacterBankPath (case, slashes, prefix differ)
- 0x005aeb90 AudioLinearToLog vs AudioEngine VolToDs (threshold, precision, ceiling differ)
- Noted, not blockers: RwsStream chunk read vs RwWorldStream.cpp; AudioWaveLoad 0x005abfa0 vs RwsBank 0x802 parse

**(c) blocked**
- AudioFmtGlobalScan (0x00633674 table bytes), AudioContextLookup (0x005ac930 no body)
- AudioDSoundQIChain / SecondaryInit (GUID bytes 0x005d09dc / 0x005d09bc, blob 0x005e7100 unread)
- MixedC3Sweep 0x005abfa0 (3 unported C2 callees, 2 globals, key bytes, 3-vs-5 dword header overflow defect)
- AudioListSearch_wfb0f (0x005aaac0 unported, original-owned list), AudioVoiceQueueSet (semaphore has no exe creator),
  AudioPoolFreeGuards (three original pools), AudioRootGet / AudioTimerRateGet (global reads, no exe writer)

## Findings outside the link order
- The two "both rsps" precedents break the ROADMAP gate: AudioQueuePop.cpp:31-32 calls 0x005ad2e0 by address,
  AudioVecLength.cpp:48,66 reads 0x005d757c / 0x00633b48. No MASHED_STANDALONE split. Callers in the exe unchecked.
- ROADMAP.md:238-239 / build.bat GUID-at-file-scope note is stale (AudioDSound.cpp:95-96 is now a comment).
- hooks.csv file-column drift (point at notes, not .cpp): 005ae800, 005ae780, 005aeca0, 005ae0c0, 005be140,
  005ad540, 005a9e40, 005abfa0, 005aefa0, 005aa8a0, 005a8960. Fix via re-classify.
- AudioDSoundSecondaryInit comment (U-0362 "unreachable ==3") contradicts the body at :232-240.


---
## Raw worker output, group A

I've finished the read-only survey of `AudioRws.cpp` (27 functions) and `AudioDSound.cpp` (8 functions). Every function is C3 in `hooks.csv` and every one would be **(a) dead code in the exe**. Nothing in the exe calls any of these 35 exports, by name or by address.

None of the five name-mangling grep checks found a definition of these RVAs or export names in any `exe_sources.rsp` TU. They only appear in asi-only files (MixedC3Sweep, PromoLoop_sessionB, AiTargeting, RwsFmt, AudioPoolFreeGuards). So linking creates **no cross-target dual copy and no STOP-AND-ASK case**.

`AudioEngine.cpp` and `RwsBank.cpp` port no RVA at all. Their only address mentions are comments: `AudioEngine.h:9,61,66` and `AudioEngine.cpp:299,309`.

## Things to know before linking

- **The two "precedent" files are not address-free.** `AudioQueuePop.cpp:31-32` calls `0x005ad2e0`, and `AudioVecLength.cpp:48,66` reads `0x005d757c` and `0x00633b48`. Neither has a `MASHED_STANDALONE` split. The real idiom is `FileWrappers.cpp:36-48`, `GameSaveVFS.cpp:47-59` and `RwStreamCore.cpp:44-60`.
- **The ROADMAP and `build.bat` notes are out of date.** ROADMAP.md:238-239 and `build.bat:139-164` name a file-scope `GUID = *(const GUID*)0x005d09dc` at `AudioDSound.cpp:95-96`. That line is now a comment. The GUID addresses are passed as literals at `:107` and `:112` and are never dereferenced at file scope. `AudioRws.cpp:476-490` (file-scope pointer binds) is still there.
- **There is a precedent for the allocator.** `0x005aea00` and `0x004522d0` are RwGlobals slot trampolines (+0x108 and +0x10c, `hooks.csv:133`, `:121`). `RwStreamCore.cpp:51-57` already maps that alloc slot to `malloc` in the exe as a declared deviation, so `RawAlloc`/`RawFree` ÔåÆ `malloc`/`free` follows it.

## AudioRws.cpp

Each "Lnn" is the line of that function's `hooks.csv` row. All rows are C3. File column: A = `Audio/AudioRws.cpp`, N = an analysis-note path (drift).

| RVA | Name (line) | hooks.csv | Address tunnels | Callees | Exe arm |
|---|---|---|---|---|---|
| 005aca80 | AudioFmtSizeCalc :32 | L778, A | none | none | as-is |
| 005acd10 | AudioFmtTableSearch :82 | L780, A | fn `0x005ac9e0` :84 | AudioFmtEntryMatch, in `RwsFmt.cpp` (asi-only; it calls `0x005adf30` by address at RwsFmt:58,128) | call by symbol and link RwsFmt.cpp |
| 005ac980 | AudioFmtDescCopy :142 | L777, A | none | none | as-is |
| 005acd60 | AudioFmtGlobalScan :205 | L785, A | data `0x00633674` (9-entry pointer table) :209; fn `0x005adf30` :207 | KeyCompare, same file | **(c)**: needs a private copy of the table and the 16-byte keys it points to. [UNCERTAIN] the bytes are not in the tree (U-1734) |
| 005ae080 | AudioSubStructAFree :247 | L769, A | fn `0x005ae920` :253; data pool `0x007dda28` :254 | PoolFree, same file | symbol plus a private pool |
| 005ae050 | AudioSubStructBFree :279 | L770, A | fn `0x004522d0` :286 | RwFree slot (C2, `hooks.csv:121`), original-only | `free` |
| 005ae030 | AudioSubStructCleanup :305 | L1493, A | none (calls by symbol) | AFree, BFree | as-is |
| 005adf30 | AudioFmtKeyCompare :329 | L1494, A | none | none | as-is |
| 005ae010 | AudioSubStructLinkDevice :370 | L1492, A | fn `0x005ae080` :374 | AFree, same file | symbol |
| 005adfe0 | AudioSubStructLinkBuffer :403 | L418, A | fn `0x005ae050` :407 | BFree, same file | symbol |
| 005ae0b0 | AudioSubStructZeroInit :429 | L787, A | none | none | as-is |
| 005ac7b0 | AudioSubStructDualInit :457 | L775, A | fn `0x005ae010` and `0x005adfe0` :462-463 | LinkDevice, LinkBuffer, same file | symbol |
| 005ae800 | AudioPoolBlockAlloc :518 | L792, **N** | `RawAlloc` `0x005aea00` :477 | RwMalloc slot (C2, `:133`) | `malloc` |
| 005ae780 | AudioPoolDestroy :587 | L796, **N** | `RawFree` :481; `PoolFree` `0x005ae920` :485; data `0x007ddab0` :490 | PoolFree, same file; RwFree | symbol, `free`, private null pointer |
| 005aeca0 | AudioFieldEndianPack :637 | L768, **N** | none | none | as-is |
| 005ae0c0 | AudioWaveFmtCopy :688 | L1491, **N** | none | EndianPack (by symbol) | as-is |
| 005ae920 | AudioPoolFree :743 | L425, A | fn `0x004522d0` :750 | RwFree | `free` |
| 005addd0 | AudioListInsertHead :807 | L795, A | data `0x009146c0` :811; fn `0x005ae800` :815 | BlockAlloc, same file | symbol plus a private pool |
| 005ade10 | AudioListRemoveByValue :850 | L132, A | data `0x009146c0` :853 | PoolFree (by symbol) | private pool |
| 005ade90 | AudioListDrain :893 | L417, A | data `0x009146c0` :895 | PoolFree (by symbol) | private pool |
| 005abcb0 | AudioWaveNodeFree :942 | L421, A | fn `0x004522d0` and `0x005ae920` :947-948; data `0x007dd634` :949 | RwFree, PoolFree | `free`, symbol, private pool |
| 005ac740 | AudioSubStructBufCleanup :1001 | L419, A | fn `0x004522d0` :1004 | RwFree | `free` |
| 005ac900 | AudioContextLookup :1039 | L776, A | fn `0x005aa0c0` :1042; code `0x005ac930` (predicate address) :1043 | `0x005aa0c0` is in `RwsStream.cpp` (asi-only, reads `0x009146fc` at RwsStream:255). `0x005ac930` has **no body and no hooks.csv row** | **(c)**: needs a stub in the exe |
| 005ae650 | AudioPoolConstruct :1085 | L791, A | fn `0x005aea00`, `0x005ae800`, `0x005ae780` :1092-1094; data `0x007dda80` (list head) :1095; data `0x007ddab0` :1096 | siblings, RwMalloc | symbol, `malloc`, private storage. [UNCERTAIN] the initial value of `0x007dda80`: :1152 dereferences `*gHead`, so zero storage would AV |
| 005be190 | AudioRwsSubZeroInit :1183 | L1568, A | none | none | as-is |
| 005be140 | AudioSubStructThreeWrite :1216 | L1567, **N**, status `new` | none | none | as-is |
| 005ad540 | AudioSubStructLinkBoth :1249 | L3955, **N**, status `mapped`, name `FUN_005ad540` | fn `0x005ae010` and `0x005adfe0` :1255-1256 | LinkDevice, LinkBuffer | symbol |

**Overlap with AudioEngine/RwsBank:** no RVA is shared.
- No shared pool, list or bank-voice infrastructure. AudioEngine keeps voices in `std::vector` and a `g_sfxRing[8]` array (`AudioEngine.cpp:32-33`, `:52-58`).
- `AudioWaveFmtCopy`, `AudioFmtDescCopy` and `AudioFmtSizeCalc` work on in-memory descriptors. RwsBank parses the 0x803 chunk inline (`RwsBank.cpp:60-68`: rate at +4, name at +112). AudioEngine builds its `WAVEFORMATEX` itself (`AudioEngine.cpp:79-85`).
- [UNCERTAIN] whether the original applies `0x005ae0c0` to the same 0x803 bytes. No caller map was read, so a job-level overlap is unproven. It is not a dual copy as things stand.

**Closure:**
- `RwsFmt.cpp` (for `AudioFmtTableSearch`).
- `RwsStream.cpp` would be needed for `AudioContextLookup`, but that function is blocked by `0x005ac930` anyway, so stub it rather than pull RwsStream in.
- The pool headers `0x007dda28`, `0x009146c0` and `0x007dd634` have no constructor call site in the exe. Zero private headers are safe **only as dead code**: `AudioPoolBlockAlloc` on a zero header dereferences `blk = h[4] = 0` (:524-529). Who constructs them in the original is [UNCERTAIN].

## AudioDSound.cpp

All rows are C3 with file `Audio/AudioDSound.cpp`, except `005a9e40` (see table).

| RVA | Name (line) | hooks.csv | Address tunnels | Callees | Exe arm |
|---|---|---|---|---|---|
| 005baf60 | AudioBufFieldSet :36 | L279 | none | none | as-is |
| 005baf90 | AudioDSoundRelease :66 | L280 | none (vtable slot 8 of `*(p+0x7c)`, U-0361) | COM, runtime | as-is |
| 005bc400 | AudioDSoundQIChain :98 | L282 | .rdata IIDs `0x005d09dc` :107 and `0x005d09bc` :112 | COM QI and Release | **(c)**: needs a private GUID, and the bytes are unread (U-0360). `audio_dsound_d4/0x005bc400.md:41` says to read 16 bytes at that address |
| 005aeea0 | AudioSemaphoreCreate :143 | L1182 | none (`CreateSemaphoreA` import) | Win32 | as-is |
| 005bbfc0 | AudioDSoundSecondaryInit :185 | L281 (status `hooked`) | IID `0x005d09bc` :219; blob `0x005e7100` :228 | COM | **(c)**: same GUID gap. `0x005e7100` is a property-set blob (`bucket_004fcac3/0x005b8c40.md:54`) |
| 005aef00 | AudioThreadDescInit :301 | L1183 | none | none | as-is |
| 005a9e10 | AudioSubStructTwoCallInit :331 | L275 | fn `0x005adfe0` :338 and `0x005ae010` :343 | LinkBuffer, LinkDevice in AudioRws.cpp | symbol |
| 005a9e40 | AudioOutputNodeCbDispatch :390 | L1343, file is an analysis note, status `new`, name `FUN_005a9e40` | none (callback at `+0x3c`) | runtime | as-is |

**Overlap:**
- AudioEngine calls `IDirectSound8`/`IDirectSoundBuffer` directly: Create, Lock, SetVolume, Play (`AudioEngine.cpp:95`, `:139-145`). It has no QI chain and no property-set path.
- It has no streaming worker thread either: `MusicStart` decodes the whole stream up front (`:259-291`). So `AudioSemaphoreCreate` and `AudioThreadDescInit` (the stream-init infrastructure, per the `0x005a8060` note at :270-290) have no AudioEngine counterpart.
- No RVA or job overlap.

**Closure:** `AudioRws.cpp`, for `AudioSubStructTwoCallInit` only.

## Hook-shaped drift
- **Thin wrappers whose only body is two calls by address:** `AudioSubStructTwoCallInit` (`AudioDSound.cpp:331-348`), `AudioSubStructLinkBoth` (`AudioRws.cpp:1249-1261`) and `AudioSubStructDualInit` (`:456-472`). Also `LinkDevice`/`LinkBuffer` (:374, :407). Each calls a sibling defined in the same tree by address instead of by name.
- `AudioSubStructTwoCallInit` is an RWS sub-struct function filed in `AudioDSound.cpp`.
- **The `AudioDSoundSecondaryInit` comments contradict the code.** The comment at :174-176 and `hooks.csv:281` say the `==3` branch is unreachable (U-0362). The body at :232-240 says it is reachable.
- **The `AudioDSound.cpp:2` header says "four functions"; there are 8.**
- **Seven `hooks.csv` file columns point at analysis notes instead of the source file:** 005ae800, 005ae780, 005aeca0, 005ae0c0, 005be140, 005ad540, 005a9e40. Two of those (005be140, 005a9e40) say `new` and one (005ad540) says `mapped`, though the code and its `RH_ScopedInstall` exist. This needs a re-classify pass, not hand edits.

## Proposed link order (smallest closure first)

Every function below is class (a); there are no (b) cases.

1. **`AudioRws.cpp` + `RwsFmt.cpp` (one step).**
   - Exe arm: siblings called by symbol, `malloc`/`free` host thunks, private storage for `0x007dda28`, `0x009146c0`, `0x007dd634`, `0x007ddab0` (null) and `0x007dda80`.
   - Ready now: the zero-closure leaves (SizeCalc, DescCopy, KeyCompare, ZeroInit, EndianPack, WaveFmtCopy, RwsSubZeroInit, ThreeWrite), then the sub-struct family, then the pool and list family, then TableSearch with RwsFmt.
   - Inside this TU, class (c) needs exe stubs, following the FileWrappers "report failure" precedent:
     - `AudioFmtGlobalScan`: table bytes unread.
     - `AudioContextLookup`: `0x005ac930` has no body.
   - [UNCERTAIN] the `0x007dda80` initial value. If it is a self-referencing sentinel, the private storage must be `{&s, &s}`, not zeros.
2. **`AudioDSound.cpp`**, after step 1.
   - Ready now: BufFieldSet, DSoundRelease, SemaphoreCreate, ThreadDescInit, OutputNodeCbDispatch, TwoCallInit (by symbol).
   - Class (c) until the GUID bytes are read: `AudioDSoundQIChain` and `AudioDSoundSecondaryInit` (`0x005d09dc`, `0x005d09bc`, `0x005e7100`). Until then they need exe stubs that return 0.

**Decisions for Mariano:**
- Whether exe stubs for the class (c) functions are acceptable inside a "shared body" link.
- Whether to first spend one execution step reading the 16 bytes at `0x005d09dc` and `0x005d09bc`, the `0x005e7100` blob, the `0x00633674` table and its targets, and the `0x007dda80` initial value from `original/MASHED.exe.unpatched`. I could not do that read-only.

---
## Raw worker output, group B

# Audiolink survey: RwsStream, RwsFmt, MixedC3Sweep, AudioMemory, AudioMusic, AudioCharacterBankPaths

Two cross-cutting points first:

- **None of the 13 RVAs has an `exe_file` entry in `hooks.csv`.** None of them appears in `re/tools/dual_copy_allowlist.txt`. Nothing in `AudioEngine.cpp` or `RwsBank.cpp` calls any function in these six files.
- **The two precedents do not meet the "no MASHED.exe address in the exe arm" rule.** Both are already in `exe_sources.rsp` (lines 238 and 247).
  - `AudioQueuePop.cpp:31-32` calls `0x005ad2e0` by address.
  - `AudioVecLength.cpp:48,66` reads `0x005d757c` and `0x00633b48` in naked asm.
  - Both are dead code in the exe, but they would fault if anything ever called them. Use `Save/FileWrappers.cpp:36-47` and `Render/RwStreamCore.cpp:44-92` as the model instead.
- The exe build compiles `Stubs\HookSystemNoOp.cpp` (`exe_sources.rsp:107`), so `RH_ScopedInstall` lines do nothing there.

---

## 1. RwsStream.cpp

**Functions**

| RVA | Name | Level | hooks.csv file |
|---|---|---|---|
| 0x005ab380 | AudioRwsChunkHeaderRead | C3 | Audio/RwsStream.cpp (hooks.csv:128) |
| 0x005ab410 | AudioRwsChunkTypeSeek | C3 | Audio/RwsStream.cpp (:129) |
| 0x005abf80 | AudioWaveVtableSlot1cDispatch | C3 | Audio/RwsStream.cpp (:424) |
| 0x005aa0c0 | AudioTreeWalkPredicateSearch | C3 | Audio/RwsStream.cpp (:772) |

**Address tunnels**
- `0x004cbd30` RwStreamRead, called by address (`:54-55`).
- `0x005ab380`, its own header reader, called by address (`:137,140`).
- `0x004cc050` RwStreamSkip, called by address (`:138,141`).
- `DAT_009146fc`, the tree-root global, read (`:255,258`).
- `0x005abf80` and the tree walk call no fixed addresses. The vtable and predicate pointers come from arguments (`:209-210`, `:261`).

**Callees**
- `0x004cbd30` and `0x004cc050` already have the shared bodies `RwStreamCoreRead` / `RwStreamCoreSkip` (`Render/RwStreamCore.h:12-13`, in the exe at `exe_sources.rsp:76`).
  - Exe arm: call those directly.
  - `RwStreamCoreSkip` returns `uint32_t*`, where non-NULL means success. That matches the `skipOk == 0` test at `:161-162`.
- `0x005ab380` is in the same file, so it becomes a direct call.
- The tree walk recurses into itself (`:289`).

**Overlap**
- This is the same job, at different RVAs, as code the exe already has in `D3d9Render/RwWorldStream.cpp` (`exe_sources.rsp:75`):
  - `ChunkHeaderRead` for 0x004cc400 (`RwWorldStream.cpp:75-95`) does the identical version decode (`:86-90` there against `RwsStream.cpp:86-97`).
  - `Rwl_StreamFindChunk` for 0x004cc5e0 (`:100-114`) applies the same 0x35000..0x37002 gate.
  - The differences: the audio copies emit no error and write `out[3]` (build) and `out[4] = 0`.
  - Because the RVAs differ, this is not an RVA dual copy, but it is a job twin worth noting to Mariano.
- `RwsBank.cpp:12-82` parses the 12-byte headers inline from a CRT buffer (`u32()`). It does no version decode and no RW stream. Same family of job, no shared RVA.

**Hazards**
- **[UNCERTAIN] Arity of 0x005ab380.** The body here takes 2 arguments (`:51`). The decomp quoted for 0x005ab410 calls it as `FUN_005ab380(stream, &local_14, 0)` (`:113`). `MixedC3Sweep.cpp:28-31` passes a third argument, `streaming_flag` (bit 4 of `param_5`). Missing evidence: the disassembly of 0x005ab380, to see whether `[esp+0xc]` is read, for example as a byte-swap flag.
- **[UNCERTAIN] 0x005abf80 calls `fn()` with no arguments and returns void (`:207,213`).** The Ghidra note says the jump table at 0x005abf8f could not be recovered (`:192-193`). The callers use the result in a loop (`:195-197`, and `MixedC3Sweep.cpp:86-90,310-313`). Missing evidence: the disassembly of 0x005abf80..0x005abf8f, to see whether it is a CALL or a JMP tail call, whether `param_1` is forwarded on the stack, and what lands in EAX.
- **Hook-shaped:** 0x005abf80 is a 3-line pointer chase with no other logic.

**Classification and closure**
- 0x005ab380, 0x005ab410 and 0x005abf80: **(a) dead code**. Closure is `RwStreamCore` only, which is already shared.
- 0x005aa0c0: **(a)** if the exe arm gets private storage for the root (a file-static `g_` pointer, NULL by default, standing in for `DAT_009146fc`).
- No other .asi-only file is needed.

---

## 2. RwsFmt.cpp

**Functions**

| RVA | Name | Level | hooks.csv file |
|---|---|---|---|
| 0x005ac5f0 | AudioFmtDescEqual | C3 | Audio/RwsFmt.cpp (hooks.csv:781) |
| 0x005ac9e0 | AudioFmtEntryMatch | C3 | Audio/RwsFmt.cpp (:786) |

**Address tunnels**
- `0x005adf30` (AudioFmtKeyCompare), called by address (`:58-59`, `:128-129`). This is the only tunnel.

**Callees**
- `0x005adf30`'s body is `AudioRws.cpp:329-352`, which is **asi-only** (C3, hooks.csv:1494). The body itself is a pure leaf with no addresses (`:327`).
- But `AudioRws.cpp` is full of tunnels, for example:
  - `0x005ac9e0` at `:84`
  - `0x005ae920` and `0x007dda28` at `:253-254`
  - `0x004522d0` at `:286`
  - `0x005aea00`, `0x004522d0`, `0x005ae920` and `0x007ddab0` at `:477-490`
- So the whole file cannot be linked. `AudioFmtKeyCompare` would need splitting into a shared TU, the same move savelink2 made when it split `FileWrappers.cpp` out of `SettingsAndIO.cpp`.

**Other notes**
- `AudioFmtTableSearch` (`AudioRws.cpp:84`) calls 0x005ac9e0 by address.
- `Harness/HarnessStubs.cpp:65-69` references 0x005ac5f0 by address. That file is asi-only.

**Overlap:** none. Neither `AudioEngine` nor `RwsBank` has a format-descriptor comparison. `RwsBank` hard-codes 16-bit mono (`RwsBank.h:25-26`).

**Classification:** **(a) dead code**, once 0x005adf30 is linked.

**Closure:** split `AudioFmtKeyCompare` 0x005adf30 out of `AudioRws.cpp` into a shared TU, for example `Audio/AudioFmtKey.cpp`.

---

## 3. MixedC3Sweep.cpp

**Function**
- 0x005abfa0 AudioWaveLoad: **C3**, but `hooks.csv:131` says `status=mapped` with the file column set to `re/analysis/promote_c2_audio_rws/005abfa0.md`, not this `.cpp`. That is tracker drift.
- The C3 came from an "implicit-survival" sweep in which the hook never fired (hooks.csv:131 notes).
- The install is mass-disabled (`:332`).

**Address tunnels**
- Function pointers:
  - `0x005ab380` at `:30`
  - `0x005aea00` at `:37`
  - `0x004cbd30` at `:43`
  - `0x004522d0` at `:49`
  - `0x005ac210` at `:57`
  - `0x005adf30` at `:65`
  - `0x005aea10` at `:71`
  - `0x005aec30` at `:77`
  - `0x005abd30` at `:83`
  - `0x005abf80` at `:89`
  - `0x005aea40` at `:95`
  - `0x005abcf0` at `:101`
- Data:
  - `DAT_007dcaa8`, the global fmt scratch buffer (`:107,168,181`).
  - `DAT_007dcaac`, the stream cap, [UNCERTAIN U-3029] (`:110,280`).
  - `DAT_005e6414` and `DAT_005e6444`, known PCM format keys (`:112-114`). Their byte contents are not in the tree.

**Callees**

| Callee | Existing body | Status |
|---|---|---|
| 0x004cbd30 | shared `RwStreamCore` | already in the exe |
| 0x005ab380, 0x005abf80 | RwsStream | asi-only |
| 0x005adf30 | AudioRws | asi-only |
| 0x005aea10, 0x005aea40, 0x005aec30 | AudioMemory | asi-only |
| 0x005aea00 (C2, hooks.csv:133) | none | original-only |
| 0x004522d0 (C2, hooks.csv:121) | none | original-only |
| 0x005ac210 (C2, deferred: U-3030, U-3031) | none | original-only (`:10-14`, hooks.csv:422) |
| 0x005abd30 (C2) | none | original-only (hooks.csv:423) |
| 0x005abcf0 (C2) | none | original-only (hooks.csv:130) |

**Defect (latent while the hook is disabled)**
- `RwsHdr3` is 3 dwords (`:149-153`), but 0x005ab380 writes 5 dwords (`RwsStream.cpp:83-97`). That is an 8-byte stack overflow at `:172` and `:220`.
- It is the same class as the 2026-07-26 pc=0x44 crash fix (`RwsStream.cpp:57-73`, `:143-151`).
- The field meanings also disagree:
  - MixedC3Sweep uses `out[2]` ("version") as the 0x804 body size (`:226-227`).
  - RwsStream says `out[1]` is the size and `out[2]` is the decoded version (`:83,95`).

**Overlap**
- Same job as `RwsBank.cpp:52-77`, the 0x802 ÔåÆ 0x803/0x804 wave parse, at a different RVA.
- Redirecting `RwsBank` or `AudioEngine` to it would be a behaviour change.

**Hook-shaped:** the 12 `Orig_*` functions (`:28-102`) are pure address-tunnel wrappers.

**Classification:** **(c) blocked**.
- 0x005ac210, 0x005abd30 and 0x005abcf0 have no body.
- `DAT_007dcaa8` and `DAT_007dcaac` need a private-storage story.
- The 3-vs-5 dword header overflow must be fixed first.

**Closure:** RwsStream, AudioMemory and the AudioFmtKeyCompare split, plus three unported C2 functions and their own callees.

---

## 4. AudioMemory.cpp

**Functions**

| RVA | Name | Level | hooks.csv file |
|---|---|---|---|
| 0x005aea10 | AudioAlignedAlloc | C3 | Audio/AudioMemory.cpp (hooks.csv:134) |
| 0x005aea40 | AudioAlignedFree | C3 | Audio/AudioMemory.cpp (:135) |
| 0x005aec00 | AudioByteReverse | C3 | Audio/AudioMemory.cpp (:136) |
| 0x005aee20 | AudioBitScanForward | **C4** | Audio/AudioMemory.cpp (:276) |
| 0x005aec30 | AudioByteSwapBuffer | C3 | Audio/AudioMemory.cpp (:1495) |

**Address tunnels**
- `0x005aea00`, the RW allocator trampoline through `DAT_007d3ff8+0x108` (`:24`, hooks.csv:133).
- `0x004522d0`, the dealloc trampoline through `+0x10c` (`:46`, hooks.csv:121).
- 0x005aec00, 0x005aee20 and 0x005aec30 have no tunnels; they are pure.

**Callees**
- 0x005aea00 and 0x004522d0 have no body.
- Exe arm: use the CRT `malloc` / `free` with the tag dropped. This is a declared deviation with an existing precedent at `RwStreamCore.cpp:51-60`.

**[UNCERTAIN] NULL test in 0x005aea10:** the port's `if (!raw) return nullptr` (`:26`) is not among the listed asm lines. The key skips 0x005aea22..0x005aea2a (`:17-18`). Missing evidence: the disassembly of those bytes.

**Overlap:** none. `AudioEngine` uses `std::vector` and `memcpy`, with no aligned allocator or byte swap.

**Classification:** all five are **(a) dead code**. Alloc and free need the host thunks. Closure: none.

---

## 5. AudioMusic.cpp

**Function:** 0x005baf00 MusicGroupVolumeSet, **C3**, `Audio/AudioMusic.cpp` (hooks.csv:948).

**Address tunnels:** none. It only does offset arithmetic on `param_1` and the list nodes (`:45-80`).

**Callees:** none.

**Overlap:** same domain (music volume) but a different job. `AudioEngine::SetMasterMusicVolume` (`AudioEngine.cpp:386-389`) sets the DSound buffer volume. 0x005baf00 writes the original's group struct (`+0x38`, node `+0x14` bit 0x40, secondary `+0x30`). No RVA is shared, and the exe has no such struct.

**Classification:** **(a) dead code**, linkable as-is. Closure: none.

---

## 6. AudioCharacterBankPaths.cpp

**Function:** 0x004625b0 AudioCharacterBankPathsBuild, **C3**, `Audio/AudioCharacterBankPaths.cpp` (hooks.csv:988).

**Address tunnels**
- `kBankPathBuf` `0x00690158` is written: 6 slots of 0x80 bytes (`:51,72-78`).
- `DAT_006041f0` is replaced by a constant table (`:53-55`), so it is not a tunnel.
- The original's per-slot init `FUN_004a2b60(slot, 0x5cc4de)` (`:14-16`) is not reproduced. `snprintf` leaves the bytes after the NUL untouched.

**Callees:** only the CRT `snprintf`.

**Overlap: STOP-AND-ASK.**
- `AudioEngine::CharacterBankName` / `CharacterBankPath` (`AudioEngine.cpp:298-320`) do the same job: the 0x004625b0 language switch (`:309-316`, commented "FUN_004625b0 language switch") plus the `DAT_006041f0` name table. `AudioEngine.h:58-69` cites both.
- Linking would put two bodies for that job in the exe, and they differ:

| | AudioEngine | 0x004625b0 port |
|---|---|---|
| Names | lowercase (`AudioEngine.cpp:301`) | uppercase (`:54`) |
| Path | `original/` prefix, forward slashes (`:318`) | backslashes, no prefix (`:76`) |
| Shape | on demand, one character | all six into a fixed buffer |

- The only exe caller is `RaceSession.cpp:192`, through `CharacterBankPath`.
- [UNCERTAIN] Whether `scripts/lint_rva_bodies.py` binds the `// FUN_004625b0` comment at `AudioEngine.cpp:309` as a body. I did not read its regex.

**Classification:** **(b)**.
- Linking it as dead code with a private `char[6][0x80]` is technically possible.
- But the job twin means Mariano should decide whether `CharacterBankPath` becomes a thin wrapper over a shared core (the uppercase/backslash output change, and the `original/` prefix moving to the caller) or the twin stays.
- Closure: none.

---

## Proposed link order (smallest closure first)

**(a) Dead code, safe**
1. `AudioMusic.cpp` (0x005baf00): no tunnels, closure 0.
2. `AudioMemory.cpp`: 0x005aec00, 0x005aee20 and 0x005aec30 as-is; 0x005aea10 and 0x005aea40 with `malloc`/`free` host thunks under `#ifdef MASHED_STANDALONE`. Closure 0.
3. `RwsStream.cpp`: 0x005ab380 and 0x005ab410 through `RwStreamCoreRead`/`Skip`, 0x005abf80 as-is, 0x005aa0c0 with a private root pointer. Closure is the already-shared `RwStreamCore`. Resolve the arity and 0x005abf80 [UNCERTAIN] items before relying on any caller.
4. `RwsFmt.cpp`: first split `AudioFmtKeyCompare` 0x005adf30 out of `AudioRws.cpp` into a shared TU. Closure is 1 split.

**(b) Decision for Mariano**
5. `AudioCharacterBankPaths.cpp` (0x004625b0): job twin with `AudioEngine::CharacterBankPath`.
- Also for Mariano, not link blockers: the RwsStream twin with `RwWorldStream.cpp`'s chunk header read and find (item 3), and the job overlap between `RwsBank` and AudioWaveLoad.

**(c) Blocked**
6. `MixedC3Sweep.cpp` (0x005abfa0) needs all of the following first:
   - bodies for 0x005ac210, 0x005abd30 and 0x005abcf0
   - private storage for `DAT_007dcaa8` and `DAT_007dcaac`
   - the `DAT_005e6414` and `DAT_005e6444` key bytes
   - the fix for the 3-vs-5 dword header overflow
   - correcting `hooks.csv:131`, whose status and file column are wrong

---
## Raw worker output, group C

Of the 13 files, 9 can be linked as-is (category a: dead code in the exe). One function needs your decision. Three files are blocked, and one file has to be split first.

**Basis.** I read all 13 files, `Save/FileWrappers.cpp`, both precedent files, `AudioEngine.cpp` (468 lines) and `RwsBank.cpp` (181 lines) in full. I also grepped `hooks.csv`, both rsp files, `Stubs/HookSystemNoOp.cpp`, `build.bat` and `dual_copy_allowlist.txt`.

**What holds for every file:**
- **Hook registration in the exe:** `RH_ScopedInstall` calls `HookSystem::Register`, which does nothing in the exe (`Stubs/HookSystemNoOp.cpp:19-21`, exe rsp line 107). No `#ifdef` is needed just for the hook registrations.
- **No callers:** none of the exported symbols in these files is referenced anywhere in `mashedmod/src` outside its own definition and its `RH_ScopedInstall`. So every link adds a body nothing calls. The one near-caller is `PromoLoop_sessionB.cpp:4577-4586`, which jumps to the *address* `0x005aded0`, not to the symbol, and that file is .asi-only.
- **Float codegen is unchanged:** none of these files is in `x87_tus.txt`. Both targets compile them with the same default SSE2 float model (`build.bat:227` and `:256`), so linking does not change the verified arithmetic.
- **`hooks.csv`:** all 43 rows below have an empty `exe_file` column. Only `0x005aed20` (`hooks.csv:3510`) and `0x005b0f40` (`:3559`) have one filled in.
- **Dual-copy allowlist:** `dual_copy_allowlist.txt` has no entry for any `Audio/` file or any `0x005a`ÔÇô`0x005c` RVA.
- **The precedents break the stated rule.** `AudioQueuePop.cpp:31-32,70` calls `0x005ad2e0` by address. `AudioVecLength.cpp:48,66` reads `0x005d757c` and `0x00633b48`. Both are in the exe rsp (lines 238 and 247) with no `MASHED_STANDALONE` guard. I did not grep for their callers, so whether they are dead in the exe is unchecked. `FileWrappers.cpp:36-48` is the precedent that actually meets the rule.

---

## AtomicExchangeStore.cpp
1. **Functions:** `0x005aefa0` `AudioAtomicExchangeStore`, C3, file column `re/analysis/bucket_audio_005ab710_005af040/0x005aefa0.md` (`hooks.csv:3513`). The file column does not point at this .cpp, which is tracker drift; fix it through re-classify.
2. **Tunnels:** none in code. The IAT slot `0x005cc164` appears only in a comment (`:15`). The body calls `InterlockedExchange` from `<windows.h>` (`:32`).
3. **Callees:** the Win32 `InterlockedExchange` only. It is the exe's own import.
4. **Overlap:** none. AudioEngine and RwsBank have no atomics.
5. **Class:** (a).
6. **Closure:** itself only.

## AudioChainWalk.cpp
1. **Functions:** `0x005af200` `AudioChainWalk`, C3, file `Audio/AudioChainWalk.cpp` (`hooks.csv:3520`).
2. **Tunnels:** none. The callback is an argument (`:47`).
3. **Callees:** the indirect `param_2`. No fixed target.
4. **Overlap:** none. RwsBank's chunk walk (`RwsBank.cpp:38-82`) walks file chunk headers, not a node chain linked at `+0x38`.
5. **Class:** (a).
6. **Closure:** itself only.

## AudioLeaves_ab1.cpp
1. **Functions:** all C3, file `Audio/AudioLeaves_ab1.cpp`:

   | RVA | Name | `hooks.csv` |
   |---|---|---|
   | `0x005ac540` | `AudioByte54Bit3Get` | `:3943` |
   | `0x005ade60` | `AudioListIndexOfKey` | `:767` |
   | `0x005aded0` | `AudioListNodeCount` | `:3971` |
   | `0x005ae590` | `AudioArenaBlockIsFree` | `:3503` |
   | `0x005aeda0` | `AudioShiftAddMul64` | `:3511` |
   | `0x005b0700` | `AudioListMinKeySelect` | `:3541` |

2. **Tunnels:** none. Everything is relative to the arguments (`:23-171`).
3. **Callees:** none.
4. **Overlap:** none. `RwsBankFind` (`RwsBank.cpp:168-178`) is a substring search over a `std::vector`, not a walk of a circular list linked at `+4`.
5. **Class:** (a).
6. **Closure:** itself only.

## AudioLeaves_ab2.cpp
1. **Functions:** `0x005b4060` `AudioFmtConvertByteLength`, C3, file `Audio/AudioLeaves_ab2.cpp` (`hooks.csv:4505`). The `0x005b3580` body was already removed (`:37-41`).
2. **Tunnels:** none.
3. **Callees:** the SSE intrinsics `_mm_cvtss_si32` and `_mm_set_ss` (`:34`).
4. **Overlap:** none. AudioEngine only fills in `nBlockAlign`/`nAvgBytesPerSec` for one fixed format (`AudioEngine.cpp:79-85`). It does no rate, bit-depth or channel conversion sizing.
5. **Class:** (a).
6. **Closure:** itself only.

## AudioLeaves_ab3.cpp
1. **Functions:** all C3, file `Audio/AudioLeaves_ab3.cpp`:

   | RVA | Name | `hooks.csv` |
   |---|---|---|
   | `0x005b73b0` | `AudioFindExtension` | `:4526` |
   | `0x005b9410` | `AudioSourceLoopSet` | `:5909` |
   | `0x005baf40` | `AudioRendererField3cSet` | `:1177` |
   | `0x005bb5b0` | `AudioPcmSaturatedAdd` | `:5912` |
   | `0x005bc450` | `AudioSlotPairZero` | `:1563` |
   | `0x005bcb80` | `AudioMediaSubtypeFromTag` | `:3990` |

2. **Tunnels:** none. `AudioSourceLoopSet` and `AudioRendererField3cSet` dereference the original's object layout (`+0x94`, `+0x11c`, `+0x78`) but only through arguments (`:100-142`).
3. **Callees:** none.
4. **Overlap:** none.
   - The software mixer (`0x005bb5b0`) has no counterpart: AudioEngine leaves mixing to DirectSound, one buffer per voice (`AudioEngine.cpp:94-109`).
   - The loop flag (`0x005b9410`) is a different job from `DSBPLAY_LOOPING` (`AudioEngine.cpp:107`). Nothing would be redirected.
   - AudioEngine has no DirectShow, so the GUID builder (`0x005bcb80`) has no counterpart.
5. **Class:** (a).
6. **Closure:** itself only.

## AudioLeaves_ab4.cpp
1. **Functions:** all C3, file `Audio/AudioLeaves_ab4.cpp`:

   | RVA | Name | `hooks.csv` |
   |---|---|---|
   | `0x005bf660` | `Audio3DwordZero` | `:4039` |
   | `0x005bfcc0` | `AudioCounterPairGet` (`__stdcall`) | `:4050` |
   | `0x005c7500` | `AudioMixerRateCompute` | `:2424` |
   | `0x005c75b0` | `AudioVoiceField8cGet` | `:2426` |
   | `0x005c9380` | `AudioBitBufSizeCalc` | `:2442` |
   | `0x005c9770` | `AudioPcmPackSaturate` | `:2452` |

2. **Tunnels:** none.
3. **Callees:** the original's `__aulldiv` at `0x004a4e40` (C2, `hooks.csv:2424`) is replaced by the compiler's own 64-bit divide (`:115`), so it comes from the exe's own CRT.
4. **Overlap:** none. AudioEngine does no i32-to-i16 pack and no mixer rate calculation.
5. **Class:** (a).
6. **Closure:** itself only.

## AudioListSearch_wfb0f.cpp
1. **Functions:** `0x005aa8a0` `AudioListSearch`, C3, file column `re/analysis/bucket_audio_005a7b60_005ab620/005aa8a0.md` (`hooks.csv:3922`). This is the same file-column drift as above.
2. **Tunnels:**
   - `0x005aaac0`: the original list walker, called by address (`:74-75`).
   - It walks the global list `DAT_007dccf0` (`hooks.csv:773`).
3. **Callees:** `0x005aaac0` is C2 `mapped` (`hooks.csv:773`) and has no body anywhere in the tree, so it exists only in the original. The callback `AudioListSearchCb` is in the same file (`:41-51`).
4. **Overlap:** none. `RwsBankFind` is a different job.
5. **Class:** (c). The walker has no port, and the list is built by the original with no writer in the exe. An exe arm returning 0 would be a hook-shaped stand-in.
6. **Closure:** a port of `0x005aaac0` (it has no file yet) plus private storage for the `DAT_007dccf0` list.

## AudioList_ag.cpp
1. **Functions:** all C3, file `Audio/AudioList_ag.cpp`:

   | RVA | Name | `hooks.csv` |
   |---|---|---|
   | `0x005b3580` | `AudioListInit` | `:4498` |
   | `0x005b35a0` | `AudioListPushBack` | `:4499` |
   | `0x005b3670` | `AudioListFind` | `:4502` |
   | `0x005b36b0` | `AudioListAt` | `:4503` |

2. **Tunnels:** none.
3. **Callees:** none.
4. **Overlap:** none. This is the single body for `0x005b3580`. The duplicate in ab2 was already removed (`AudioLeaves_ab2.cpp:37-41`).
5. **Class:** (a).
6. **Closure:** itself only.

## AudioPoolFreeGuards.cpp
1. **Functions:** all C3, file `Audio/AudioPoolFreeGuards.cpp`:

   | RVA | Name | `hooks.csv` |
   |---|---|---|
   | `0x005ae380` | `AudioPoolFreeGuardA` | `:3497` |
   | `0x005a6c90` | `AudioPoolFreeGuardB` | `:4480` |
   | `0x005ad8b0` | `AudioPoolFreeGuardC` | `:3962` |

2. **Tunnels:**
   - `0x005ae920` is called through a function-pointer literal (`:46`).
   - The pool anchors `0x007dda50`, `0x007dc9e8` and `0x007dd6c4` are passed as immediates (`:53, :61, :69`).
3. **Callees:** `0x005ae920` `AudioPoolFree` is C3 in `Audio/AudioRws.cpp` (`hooks.csv:425`). That file is .asi-only (asi rsp line 39). The guards call the *address*, not the C++ symbol. `AudioPoolFree` itself tunnels `0x004522d0` (RwFree, `AudioRws.cpp:750`).
4. **Overlap:** none. AudioEngine uses `std::vector` and COM `Release`, with no pool allocator.
5. **Class:** (c). The pools exist only in the original. Hook-shaped drift: the whole content is one bit test plus a by-address call with an absolute anchor.
6. **Closure:** `AudioRws.cpp` (large, .asi-only) plus a RwFree host thunk plus private storage for the three pools.

## AudioTableDispatch.cpp
1. **Functions:** both C3, file `Audio/AudioTableDispatch.cpp`: `0x005b10a0` `AudioTableDispatchAt4` (`hooks.csv:3563`) and `0x005b10e0` `AudioTableDispatchAt8` (`:3565`).
2. **Tunnels:** none in code. The table pointer comes from `holder+4` or `holder+8` (`:60, :70`).
3. **Callees:** the indirect `entry->fn`. No fixed target.
4. **Overlap:** none.
5. **Class:** (a).
6. **Closure:** itself only.

## AudioVoiceQueueSet.cpp
1. **Functions:** `0x005a8960` `AudioVoiceQueueSet5a8960`, C3, file column `re/analysis/bucket_audio_005a7b60_005ab620/005a8960.md` (`hooks.csv:3905`). This is the same file-column drift.
2. **Tunnels:** it reads `DAT_007dcae0`, the semaphore `HANDLE` (`:21`). The original creates that semaphore at `0x005a82d2` (`AudioDSound.cpp:272`).
3. **Callees:**
   - `WaitForSingleObject` and `ReleaseSemaphore`: Win32 (`<windows.h>` comes through `HookSystem.h:6`).
   - `0x005b0dc0` is inlined (`:23-27`) instead of called. Its own body, `CmdBuild5b0dc0Set` (C3, `hooks.csv:3554`), is in `Util/PromoLoop_sessionB.cpp:137`, which is .asi-only (asi rsp line 318). The .asi therefore already has two bodies for the `0x005b0dc0` job. In the exe the inlined copy would be the only one. Whether the dual-copy scanner counts inlined callee bodies is [UNCERTAIN]: I did not read the scanner.
   - The caller `0x0045dc80` has no port in the tree.
4. **Overlap:** none. AudioEngine has no threads or semaphore, and its `Voice` struct (`AudioEngine.cpp:52-58`) is a different layout.
5. **Class:** (c). The handle has no exe-side creator. Any exe arm (a private `HANDLE`, or skipping the lock) is behaviour I would be inventing, so it is a decision for you.
6. **Closure:** a private semaphore plus its creator (a port of the `0x005a82d2` owner), and possibly `PromoLoop_sessionB.cpp`'s `0x005b0dc0` if the inlined copy is replaced with a call.

## RingHeaderInit.cpp
1. **Functions:** `0x005b1160` `RingHeaderInit`, C3, file `Audio/RingHeaderInit.cpp` (`hooks.csv:3568`).
2. **Tunnels:** none.
3. **Callees:** none.
4. **Overlap:** none.
5. **Class:** (a).
6. **Closure:** itself only.

## TimerGetters_ah5.cpp
1. **Functions:** all C3, file `Audio/TimerGetters_ah5.cpp`:

   | RVA | Name | `hooks.csv` |
   |---|---|---|
   | `0x005aad40` | `AudioRootGet` | `:1685` |
   | `0x005ab070` | `AudioTimerRateGet` | `:3929` |
   | `0x005aeb90` | `AudioLinearToLog` | `:3509` |
   | `0x005a89a0` | `VoiceFieldD6cGet` | `:1680` |
   | `0x005a89b0` | `VoiceFieldD70Get` | `:1681` |
   | `0x005a89c0` | `VoiceFieldD74Get` | `:1682` |
   | `0x005ab370` | `ChannelField10Get` | `:3937` |

2. **Tunnels:**
   - `0x007dcd20` (`:22`), the audio root pointer.
   - `0x007dcdf8` and `0x007dce00` (`:31-32`), the rate and the mode flag.
   - `0x005cc990` (`:57`) and `0x005cd0b8` (`:69`): float constants loaded into EDX inside inline `__asm`. Other code already records their values: `0x005cc990` = `0x3727c5ac` (`Powerup/PowerupAim.cpp:39`, `Collision/ContactConstants.h:71`) and `0x005cd0b8` = 2000.0f (`Ai/AiStandalone.cpp:704`). That gives a private-constant option for the exe arm. That these are read-only data is [UNCERTAIN]: I did not read the section table.
   - The four field getters (`:100-122`) have no tunnels.
3. **Callees:** none.
4. **Overlap: STOP-AND-ASK for `0x005aeb90`.** It does the same job as `VolToDs` (`AudioEngine.cpp:62-70`): linear volume to DirectSound hundredths of a dB, `2000*log10(v)`, floor ÔêÆ10000, ceiling 0. The two bodies differ:

   | | `VolToDs` | `0x005aeb90` |
   |---|---|---|
   | Floor threshold | `v <= 0.0001f` | `x < [0x005cc990]` |
   | Precision | double | f32 store before a truncating FISTP (`:71-78`) |
   | Ceiling | returns 0 for `v >= 1` | clamps positive results to 0 |
   | Extra input | `g_focusGain` | none |

   Linking would put two bodies for one job in the exe. It matters only if `VolToDs` is redirected to it, which is your call. The other six functions have no overlap.
5. **Class, per function:**
   - The four field getters: (a).
   - `AudioRootGet` and `AudioTimerRateGet`: (c) as written. With `#ifdef` private statics they could link as dead code: the root would return 0, and the rate would return 1000 because the mode flag is never 1. They are hook-shaped drift, though: each is a single global read with no exe-side writer.
   - `AudioLinearToLog`: (b).
6. **Closure:** splitting the file is a precondition, the same way savelink2's option C split FileWrappers out of `SettingsAndIO.cpp`. The four getters move to a shared file. The rest stays .asi-only until you decide.

---

## Proposed link order (smallest closure first)

**(a) Dead code in the exe.** Each closure is the file itself, with no `#ifdef` and no host thunks:
1. `RingHeaderInit.cpp` (1 function)
2. `AtomicExchangeStore.cpp` (1)
3. `AudioLeaves_ab2.cpp` (1)
4. `AudioChainWalk.cpp` (1)
5. `AudioTableDispatch.cpp` (2)
6. `AudioList_ag.cpp` (4)
7. `AudioLeaves_ab1.cpp` (6)
8. `AudioLeaves_ab3.cpp` (6)
9. `AudioLeaves_ab4.cpp` (6)
10. The four field getters from `TimerGetters_ah5.cpp` (`0x005a89a0`/`b0`/`c0`, `0x005ab370`), only after the split into a shared file.

**(b) Would replace an AudioEngine path, your decision (STOP-AND-ASK):**
- `0x005aeb90` `AudioLinearToLog` against `VolToDs`.

**(c) Blocked:**
- `AudioRootGet` and `AudioTimerRateGet`: global-read tunnels with no writer in the exe. They could link as hook-shaped stubs if you accept that.
- `AudioListSearch_wfb0f.cpp`: `0x005aaac0` has no port, and the `DAT_007dccf0` list belongs to the original.
- `AudioVoiceQueueSet.cpp`: the `0x007dcae0` semaphore has no exe-side creator, and `0x005b0dc0` is inlined while its real body is .asi-only.
- `AudioPoolFreeGuards.cpp`: depends on `AudioRws.cpp` (`0x005ae920`, which itself tunnels `0x004522d0`) and on three original pools. This is the largest closure.

**Tracker drift** (fix through re-classify, not by hand): the `hooks.csv` file column for `0x005aefa0` (`:3513`), `0x005aa8a0` (`:3922`) and `0x005a8960` (`:3905`) points at analysis notes instead of the .cpp that holds the body.
