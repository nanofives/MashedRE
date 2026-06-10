# SCRIBE_QUEUE fragment — batch_an session 3 (an_s3)

Author-only promote-c2 pass (gameplay campaign 3/~5, third slice 0x0045ae80..0x0045dd50).
Bucket plates are the C2 deliverable; central finalize (ghidra-sweep + re-classify)
writes hooks.csv / trackers and commits.

## Queued

2026-06-03  an_s3  bucket=re/analysis/bucket_gameplay_0045ae80_0045dd50  confidence=C1->C2  rvas=0045ae80,0045b360,0045b390,0045bfd0,0045bff0,0045c030,0045c110,0045c330,0045c4e0,0045c510,0045c550,0045c640,0045c850,0045c860,0045c880,0045ca30,0045cab0,0045caf0,0045cb20,0045cbe0,0045d080,0045d0e0,0045da90,0045daf0,0045dce0,0045dd50  note=subsystem_observed=gameplay (confirmed 23/26); RECOMMEND reclass gameplay->audio for 0045da90 + 0045daf0 + 0045dce0 (all pure RWS-audio emitter param/position sync — 0045da90 two-step setter via FUN_005a89d0 @0x005a8xxx; 0045daf0/0045dce0 push entity xyz to audio source via FUN_005a6d60 @0x005a6xxx; no game-state mutation beyond a cached slot position); 0045c030 borderline render/util (pure RW look-at matrix builder) but KEPT gameplay (sole caller is effect-spawner FUN_00487280); named RW import resolved: FUN_004e45b0=RwFrameRemoveChild (in 0045b360); library_skip=NONE; needs_function_create=NONE; actual pool slot=Mashed_pool3

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. **None drift-skipped** — all
  26 were `gameplay/C1/mapped` in hooks.csv at session start (existing C1 plates in
  `re/analysis/bucket_00449ba0/0x<rva>.md`; deepened, not restarted). All 26 returned
  non-null `function_at` + `decomp_function`. Every plate uses `## Mechanical description`
  and `confidence_target: C2`.

- **POOL SLOT**: pre-assigned **Mashed_pool3** opened read-only
  (`project_program_open_existing`, program `/MASHED.exe`, image_base 0x00400000 so RVAs
  map 1:1). A stale on-disk `Mashed_pool3.lock` (dated May 26) was present but the shared
  MCP reported 0 open sessions and pool3 is NOT in the leaked-in-JVM-lock set
  (pool0/4/5/6); the read-only open succeeded cleanly. `program_close`d at end. Recorded
  in `.pool_slot_an_s3`. (Sibling sessions observed live: an_s1 fell back pool1→pool9,
  an_s2 on pool2 — see their markers.)

- **SUBSYSTEM — bucket is dominantly gameplay; three coherent groups + 1 getter:**

  1. **Impact-particle / detonation effects (gameplay)** —
     - `0045ae80` per-frame hit-spark driver (4 players × 5 tracks × 5 spark slots in
       0x0068bb60..0x0068d76c; raycast + gravity + bounce + immediate-mode billboard
       render via FUN_004893d0/_50/_80/_a0). **C1 correction**: both loops iterate **4**
       entries, not 5 (`(0x68bd04-0x68bb64)/0x68 = (0x68bd00-0x68bb60)/0x68 = 4`).
     - `0045b360` effect-slot detach/reset (zeros +0xc..+0x18; `RwFrameRemoveChild` the
       +0x1c frame).
     - `0045b390` depth-charge/powerup detonation: emit event 0x15, ground decals along
       2 emitter frames, splash damage on up to 4 entities (FUN_00558b40 overlap →
       FUN_00480720/FUN_00481750/FUN_00421630(force 8.0f)/FUN_004922e0(state 2,15,255)).
     - `0045c030` look-at matrix builder (delta@+0 / cross@+0x10 / translation@+0x20;
       FUN_004c39b0 normalize ×2, FUN_004c45f0 orthonormalize, FUN_004c51a0 concat) —
       borderline render/util, kept gameplay (caller = effect spawner FUN_00487280).

  2. **Control-action binding / AI-decision subsystem (gameplay)** — the bulk (18 fns):
     - Predicates/selectors: `0045bff0` slot-empty + `0045c330` its inverter (record
       table 0x0088fc88 stride 0xb4); `0045c640` binding-buffer-pair selector (sets
       DAT_0088fbc4 count-ptr / DAT_0088fbc8 buffer-ptr per player 1/2/3/default);
       `0045c4e0` linear search (40-byte records); `0045c510` direct-binding presence
       (0x0088f680 4-slot table, +0x0c active/+0x28 key); `0045c550` chained-transition
       matcher (0x005fcb74 chain, 0x25 carry marker); `0045caf0` same-team predicate
       (0x007f1a18 id table, gate DAT_008aa254<2); `0045cab0` player-state predicate.
     - Mutators: `0045cb20` register-binding (≤32, copies 0x005fcb50 template + sets
       +0x04 player/+0x08 key); `0045cbe0` unregister-binding (find + shift-down; ignores
       its 3rd arg); `0045d080` clear-all (4 players); `0045c850` per-index flag clear
       (0x0088f09c); `0045c860` 4-dword block clear (0x0088f0a0).
     - Script/AI: `0045c110` condition-opcode evaluator (0xff0N____ tags → player input
       via FUN_00472550, no-arg raw vs (N-1) released-test); `0045c880` 6×6 AI affinity
       matrix update (0x0088f5e0, opcode-keyed deltas, clamp [-5,5]); `0045ca30` its
       tri-band quantizer ({0,-1,-2}); `0045d0e0` per-frame "should-act" decision gate.
       **C1 correction (0045d0e0)**: per-player counter `&DAT_0068d53c + param_1` is
       `int*` → stride **4 bytes** (not 1); slot-conflict loop bound is `DAT_008aa254`
       (player count), not a fixed 4.
     - `0045bfd0` thunk → FUN_00426060 (returns DAT_0065742c).

  3. **3D positional-audio glue (RECOMMEND reclass → audio), 3 fns** — feed the RWS-audio
     engine, no game-state mutation:
     - `0045da90` two-step audio-param setter (slot handle (&DAT_0069045c)[i] → FUN_005a89d0
       twice, selector 0→1). Caller FUN_004657b0.
     - `0045daf0` emitter position sync from a transform (xyz → slot +0x58, then
       FUN_005a6d60 to the linked source). 8 callers all in the 0x0046xxxx audio region.
     - `0045dce0` mode-gated (FUN_00492d10()==3) position sync, flag-bit-0x2 active gate,
       FUN_005a6d60 push. Callers FUN_00406160, FUN_00419a00.

  4. **Misc getter (gameplay)** — `0045dd50` indexed dword getter `g_table[i]` at
     0x008aa300 (sibling of the 0x008aa2e0 active-flag table).

- **RECLASS RECOMMENDATIONS (gameplay → audio), 3 RVAs**: 0045da90, 0045daf0, 0045dce0.
  Each plate carries `subsystem_observed: audio` in frontmatter + an [UNCERTAIN]/note.
  (Counter-argument for keeping gameplay: 0045daf0/0045dce0 also write a gameplay slot's
  cached xyz at +0x58. Sweep's call.)

- **Named RW import resolved**: `0045b360`'s FUN_004e45b0 is the master's
  **`RwFrameRemoveChild`** (recorded in the plate as a call target, not a stub).
  FUN_004e4800 (frame parent-of) remains unnamed — NOT guessed.

- **needs_function_create = NONE** (all 26 have function objects, incl. the Ghidra-marked
  thunk 0045bfd0). **library_skip = NONE** — library-band screen confirmed: every RVA is
  below CRT (0x004a0000) / D3DX9-PSGP (0x004ec000) / qhull (0x0057c5b0) bands; no RVA
  decodes as a named library primitive.

- **Out-of-bucket callees referenced (NOT minted — sweep owns S-IDs)**: FUN_00474d80,
  FUN_004c1480, FUN_0045a950/590/530/ac40, FUN_004671d0, FUN_0045bfe0, FUN_004b4cd0/4650/
  4b20/5080, FUN_00472650, FUN_00486610, FUN_004a2c48, FUN_004893d0/450/480/4a0/3b0/890
  (effect/billboard); FUN_00465ca0, FUN_004c0ed0/39b0/45f0/51a0/5010, FUN_00476860,
  FUN_00475ab0, FUN_0040e370, FUN_0041f030, FUN_00558b40, FUN_00480720/81750/421630/4922e0
  (detonation/damage); FUN_004e4800 (frame parent-of); FUN_00472550 (input query);
  FUN_0046cbb0/0046c7b0 (player-state); FUN_00431b70, FUN_0040e470, FUN_0045dff0 (decision
  gate); FUN_005a89d0, FUN_005a6d60 (RWS-audio); FUN_0046d4a0 (transform fetch),
  FUN_00492d10 (mode gate). Named RW import: RwFrameRemoveChild.

- **Uncertainties carried**: all data-semantic / object-identity / struct-field (non-
  blocking for C2 of these control-flow-complete reads). Filed as bare `[UNCERTAIN]` in
  the plates, NOT minted (author-only). Recurring open items for central minting from the
  reserved U-8300..U-8599 range: the 0xb4-byte record at 0x0088fc88 (+0x00 occupancy);
  the 0x28 binding-record template (0x005fcb50) field layout + the 0x25 carry marker;
  the 6×6 affinity matrix axes (0x0088f5e0) + opcode→delta map; the 0x0088f680 4-slot
  object fields (+0x0c/+0x28/+0x04/+0x14); the 0x007f1077/0x007f1537 0x4c-stride byte
  flags; the 0x0069045c audio-handle table + FUN_005a89d0/FUN_005a6d60 setter semantics;
  flag bit 0x2 at slot+0x34; the 0x008aa300 per-slot value; FUN_00492d10/FUN_00431b70
  mode globals.

- **No U-IDs / S-IDs / arg_types minted. No Frida, no build, no re-classify, no
  hooks.csv write.** Per author-only mission only the bucket dir, this fragment, and
  `.pool_slot_an_s3` (untracked session marker) were created/modified.

> DRAINED by sweep-20260603-1259 — 26 plates + 26 bookmarks to master Ghidra, 0 renames, 0 errors. C1->C2 finalize via re-classify follows.
