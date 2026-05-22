import os

buckets = [
    (1, 0x00452ec0, 'depthcharge-mp-vehicle-adjacent',
     'Cluster anchored at 0x00452ec0 adjacent to mapped DepthChargePerFrameTeardown and similar gameplay/vehicle helpers. Working hypothesis based on adjacent xrefs: multiplayer/vehicle update logic. Confirm or reclassify during decomp.'),
    (2, 0x004b4a80, 'camera-io-wrappers',
     'Cluster anchored at 0x004b4a80 just past the MSVC CRT band; adjacent to FileReadWrapper_i3, FileWriteWrapper_i3, Camera::BuildFrame. Working hypothesis: low-level IO + camera glue. Confirm or reclassify during decomp.'),
    (3, 0x004c4270, 'rw-math-core',
     'Cluster anchored at 0x004c4270 just past the Lua-5.0 source band; adjacent to RwV3dTransformPoint, RwV3dTransformVector, Vec3Magnitude (C3). Working hypothesis: RenderWare math primitives + transform helpers. Confirm or reclassify during decomp.'),
    (4, 0x00549580, 'vfs-piz-streaming',
     'Cluster anchored at 0x00549580; adjacent to VFS_Open, VfsStreamRead and other PIZ archive plumbing. Working hypothesis: VFS / .piz streaming pipeline. Confirm or reclassify during decomp.'),
    (5, 0x005a6f30, 'audio-rws-tree',
     'Cluster anchored at 0x005a6f30; adjacent to AudioSubStructTwoCallInit, AudioTreeWalkPredicateSearch, AudioRwsChunkHeaderRead. Working hypothesis: audio RWS chunk-tree walker / sub-struct allocator. Confirm or reclassify during decomp.'),
    (6, 0x005bba60, 'audio-music-mixer',
     'Cluster anchored at 0x005bba60; adjacent to MusicGroupVolumeSet, AudioBufFieldSet. Working hypothesis: audio music/mixer / channel field setters. Confirm or reclassify during decomp.'),
]

POOL_SLOTS = [1, 2, 3, 4, 5, 6]

header = '''==============================================================================
batch_x.txt  -  Mashed C1 brute-force  -  2026-05-19
==============================================================================

Mode:           Opus 1M first-pass C0/unmapped -> C1 discovery
Sessions:       6
RVAs per sess:  80  (Opus 1M hard cap 100; default 80)
Total RVAs:     480
Unmapped pool:  1628 (post-exclusion: batch-w drained 480 + 8 library bands;
                fully-inventoried unmapped before exclusions = 2815)
Pool slots:     Mashed_pool1..Mashed_pool6 (verify free at launch)
Anchor binary:  original/MASHED.exe.unpatched SHA-256 (patched MASHED.exe
                differs by design; anchor lives on .unpatched per CLAUDE.md)
                BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
Pool dir:       C:/Users/maria/Desktop/Proyectos/Mashed/mashed_pool

Read this BEFORE starting any session:
- .claude/skills/discover-c1-batch/SKILL.md  (Opus 1M caps section)
- .claude/skills/ghidra-pool/SKILL.md         (acquire/release flow)
- re/CONFIDENCE.md                            (C0 -> C1 evidence rubric)
- re/SESSION_RULES.md (search for scribe-queue) (queue-row format)

RW reference catalog (876 RW names by module - use during decomp):
  re/analysis/plans/rw_function_catalog.tsv

Already-named library-residue ranges (DON'T re-discover):
  0x004a0000..0x004b3fff  - MSVC CRT (FidDB-attested, pre-excluded)
  0x0049dcb0..0x004a0000  - DirectShow strmbase static-link (batch-v-s2 finding)
  0x004ba000..0x004c4000  - Lua 5.0 source (batch-w-s3 finding: lparser+lundump+
                            lfunc+lcode at 0x004bdcc0..0x004bfa80; full band
                            extends down to cover lstate/lgc/lvm/lapi/ltable/
                            lstring/lmem/llex/lobject/lopcodes/lzio/ldebug)
  0x004fcb51..0x005112c9  - D3DX9 HLSL shader compiler frontend (batch-w-s4
                            preprocessor+byacc-parser+lexer+diagnostics+symbol
                            hashtable + breadth_unmapped_005xx-20260514
                            assembler+math-primitives band; ~94KB combined)
  0x00516bb0..0x0052df40  - libpng + zlib + image-loader (batch-v-s4; batch-t-s4)
  0x0057c5b0..0x005913c0  - qhull 2002.1 extended (batch-v-s5; batch-t-s5)
  0x005a0e00..0x005a5017  - qhull print* (Ghidra FidDB)
  0x005c0000..0x005d0000  - MSVC CRT tail (batch-v-s6; 33 FidDB + 23 helpers)

If your bucket lands inside one of these ranges, halt per the STOP-AND-ASK
library-residue clause.

Library-vs-game-code heuristic (batch_w post-mortem 2026-05-19):
The dense-RVA window heuristic that drove batch_w-s3 (Lua) and batch_w-s4
(D3DX9 HLSL) selected library code because small-contiguous functions with
no game-named neighbors looked leaf-heavy. batch_x candidates were filtered
to require at least 5 game-named (non-FUN_/sub_/thunk_) mapped neighbors
within +/-0x1000 of the bucket. All 6 picks below satisfy this; sessions
should still STOP-AND-ASK if their bucket turns out to be library residue.

'''


def gen_session(n, start, label, mission_extra, pool_slot, candidates):
    lo = candidates[0][0]
    hi = candidates[-1][0]
    span = int(hi, 16) - int(lo, 16)
    bucket = f'bucket_{start:08x}'
    body = f'''==============================================================================
Session {n} - {bucket}  (FIRST-PASS, 80-RVA discovery)
==============================================================================

Pool slot:    Mashed_pool{pool_slot} (pre-assigned)
Model:        Opus 4.7 (claude-opus-4-7[1m]) - hard cap 100 RVAs (this session: 80)
Skill:        discover-c1-batch (first-pass; Opus 1M mode)
Bucket dir:   re/analysis/{bucket}/
Drains to:    re/SCRIBE_QUEUE.md (Queued); ghidra-sweep picks up later
Address span: {lo} .. {hi}  (~{span:,} bytes of code, avg ~{span // 80}B/fn)
Bucket label: {label}

# Mission
Discover and write C0->C1 plates for 80 unmapped functions in the
contiguous-RVA bucket anchored at 0x{start:08x}. {mission_extra}
These functions have no row in hooks.csv and no analysis note. Report
final subsystem in the queue row's `subsystem_observed=` field.

# Pre-flight
```bash
export POOL_SLOT_FILE=".pool_slot_batch_x_s{n}"
bash scripts/ghidra_pool.sh acquire   # should return Mashed_pool{pool_slot} if free
sha256sum original/MASHED.exe.unpatched  # MUST equal BDCAE0...EFD3C0E
```

# Workflow (Opus 1M - bulk-coherent per session)
Read all candidate RVAs first; then decomp in clusters of ~10-15 at a time.
Opus 1M context keeps all 80 functions' decomp + cross-refs in scope
simultaneously - exploit this for cross-function naming (recurring callees,
shared globals, repeated string patterns).

Per RVA:
  1. mcp__ghidra__function_at <rva>     (or listing_code_unit_at if no fn)
  2. mcp__ghidra__decomp_function       (the body)
  3. mcp__ghidra__function_callees      (depth-1 callees)
  4. mcp__ghidra__function_callers      (subsystem inference)
  5. Cross-ref the RW catalog: signature/calls/constants matching an RW
     API -> name and cite catalog row
  6. Write re/analysis/{bucket}/0x<rva>.md with:
     - frontmatter (rva, name_in_ghidra, size_bytes, confidence=C1,
       callees_depth1, callers_noted, opened_in_slot=Mashed_pool{pool_slot},
       session_date=2026-05-19)
     - ## Mechanical description (bullets; no speculation; cite addresses)
     - ## Constants (table; cite RVA for each)
     - ## Uncertainties (bare [UNCERTAIN] markers; sweep assigns U-IDs)
     - ## Stubs encountered (bare [STUB] markers; sweep assigns S-IDs)

# Candidates
| # | RVA | Ghidra name | size |
|---|-----|-------------|------|
'''
    for i, (r, n, s) in enumerate(candidates, 1):
        body += f'| {i} | {r} | {n} | {s} |\n'

    body += f'''
# Output (this session ONLY produces these - does NOT touch hooks.csv)
- 80 per-RVA .md plates in re/analysis/{bucket}/
- ONE queue row appended to re/SCRIBE_QUEUE.md (Queued section):
    2026-05-19  batch-x-s{n}  rvas=0x...,0x...  bucket=re/analysis/{bucket}
                  subsystem_observed=<X>  notes=<one-line>

# NO COMMITS, NO hooks.csv WRITES
- Do NOT mutate hooks.csv, STUBS.md, UNCERTAINTIES.md, or DEFERRED.md.
  Those are the sweep's / orchestrator's responsibility.
- Do NOT git commit. The orchestrating session commits after all 6
  sessions return.

# STOP-AND-ASK
- function_at returns null AND listing_code_unit_at also null -> skip + note
- Bucket-range turns out to be library residue (any of the 8 already-known
  ranges above OR newly-discovered FidDB-attested cluster) -> halt + report.
  Sessions should NOT promote library residue.
- Average function size in bucket < 150B AND zero game-string anchors after
  first 20 decomps -> halt + report (library-vs-game-code heuristic miss
  per batch_w post-mortem).
- RW catalog match found but signature mismatches Ghidra decomp -> flag, do
  NOT auto-name; defer to a re-classify pass.
- More than 5 candidates have catalogued [UNCERTAIN] markers above C2 -> halt
- Adjacent functions form a known struct shape (>= 3 share offsets) ->
    optionally produce re/analysis/structs/<name>.md alongside the plates

'''
    return body


def load(start):
    rows = []
    with open(f'tmp/batch_x_buckets/{start:08x}.tsv') as f:
        for line in f:
            r, n, s = line.strip().split('\t')
            rows.append((r, n, int(s)))
    return rows


out = header
for (n_idx, start, label, mission_extra), pool in zip(buckets, POOL_SLOTS):
    out += gen_session(n_idx, start, label, mission_extra, pool, load(start))

with open('batch_x.txt', 'w', newline='\n', encoding='utf-8') as f:
    f.write(out)

print('bytes written:', os.path.getsize('batch_x.txt'))
