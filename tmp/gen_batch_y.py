"""Generate batch_y.txt — Mashed C1 brute-force discovery batch."""
import csv

EXCLUDE = [
    (0x004a0000, 0x004b3fff),
    (0x0049dcb0, 0x004a0000),
    (0x004b4a80, 0x004c4000),
    (0x004fcb51, 0x005112c9),
    (0x00516bb0, 0x0052df40),
    (0x0057c5b0, 0x005913c0),
    (0x005a0e00, 0x005a5017),
    (0x005c0000, 0x005d0000),
]
BATCH_X_RANGES = [
    (0x00452ec0, 0x0045a191),
    (0x004b4a80, 0x004b82a1),
    (0x004c4270, 0x004cff21),
    (0x00549580, 0x00553ef1),
    (0x005a6f30, 0x005adf61),
    (0x005bba60, 0x005bfed1),
]

def is_excluded(rva):
    for lo, hi in EXCLUDE:
        if lo <= rva < hi:
            return True
    for lo, hi in BATCH_X_RANGES:
        if lo <= rva < hi:
            return True
    return False

mapped = set()
with open('hooks.csv', encoding='utf-8', errors='replace') as f:
    r = csv.DictReader(f)
    for row in r:
        rva_str = row['rva'].strip().lower()
        if rva_str.startswith('0x'):
            rva_str = rva_str[2:]
        try:
            mapped.add(int(rva_str, 16))
        except ValueError:
            pass

unmapped = []
with open('re/analysis/function_inventory.csv', encoding='utf-8', errors='replace') as f:
    r = csv.DictReader(f)
    for row in r:
        rva = int(row['rva'], 16)
        size = int(row['body_size'])
        if rva in mapped:
            continue
        if is_excluded(rva):
            continue
        unmapped.append((rva, row['name'], size))
unmapped.sort()

bands = [
    {
        'lo_seek': 0x00449ba0, 'hi_seek': 0x00500000,
        'label': 'gameplay-powerup-bezier-adjacent',
        'slot': 1,
        'hypothesis': (
            'Cluster anchored at 0x00449ba0 adjacent to SkyDomeRender, '
            'DepthChargePathSafetyCheck, PowerupSlotDeactivate, '
            'PowerupRoundCleanup, Bezier::GetLocate. Working hypothesis: '
            'powerup pickup/spawn + sky/depthcharge edge cases + bezier-path '
            'helpers. Confirm or reclassify during decomp.'
        ),
    },
    {
        'lo_seek': 0x00466100, 'hi_seek': 0x00500000,
        'label': 'vehicle-physics-core',
        'slot': 2,
        'hypothesis': (
            'Cluster anchored at 0x00466100 deep in the vehicle subsystem; '
            'adjacent to VehicleWheelDrivetrainUpdate, '
            'VehicleContactScanUpdate, VehicleVehicleCollisionImpulse, '
            'VehicleAeroStabilizer, VehicleDampVec3, VehicleEntitySlotRead. '
            'Working hypothesis: vehicle physics step (wheel / drivetrain / '
            'collision / contact) + vehicle slot helpers. Confirm or '
            'reclassify during decomp.'
        ),
    },
    {
        'lo_seek': 0x004d7ac0, 'hi_seek': 0x00500000,
        'label': 'rw-frame-light-world',
        'slot': 3,
        'hypothesis': (
            'Cluster anchored at 0x004d7ac0 between the Lua source band '
            '(extended exclusion 0x004b4a80..0x004c4000) and the start of '
            'the D3DX9 HLSL band; adjacent to RwFrameAddChild, '
            'RwFrameDestroyHierarchy, RpLightCreate, RpLightStreamRead, '
            'RpWorldInstance, RwEngineForAllPlugins. Working hypothesis: '
            'RenderWare frame hierarchy + light/world plugin glue. Confirm '
            'or reclassify during decomp.'
        ),
    },
    {
        'lo_seek': 0x00554010, 'hi_seek': 0x00580000,
        'label': 'font-canvas-context',
        'slot': 4,
        'hypothesis': (
            'Cluster anchored at 0x00554010 directly above '
            'FontGlyph_UploadData, FontCanvas_Init, FontCtx_Alloc, '
            'FontCtx_LoadMetrics_Met, FontCtx_BuildExtTable, '
            'FontStyle_Alloc. Working hypothesis: font/canvas init + '
            'context allocator + glyph metrics loader + style table. '
            'Confirm or reclassify during decomp.'
        ),
    },
    {
        'lo_seek': 0x00565d50, 'hi_seek': 0x0057c000,
        'label': 'unexplored-large-fns-565xx',
        'slot': 5,
        'hypothesis': (
            'Cluster anchored at 0x00565d50 in a previously-unexplored span '
            'between the libpng+zlib band (excluded 0x00516bb0..0x0052df40) '
            'and the qhull band (excluded 0x0057c5b0..0x005913c0). NO '
            'game-named neighbors mapped yet (zero anchors within +/-0x1000). '
            'Avg function size 631B is HIGH (well above the library-residue '
            'heuristic threshold of 150B), strongly suggesting real game '
            'code rather than library residue. Working hypothesis: unknown '
            'game subsystem (heavy/large functions imply non-trivial logic). '
            'Confirm subsystem during decomp; this session carries elevated '
            'STOP-AND-ASK risk - report immediately if first 20 decomps '
            'reveal library FidDB names or pure CRT helpers.'
        ),
    },
    {
        'lo_seek': 0x005a5020, 'hi_seek': 0x005bba00,
        'label': 'audio-fmt-rws-tree',
        'slot': 6,
        'hypothesis': (
            'Cluster anchored at 0x005a5020 immediately above the '
            'audio-rws-tree bucket (batch-x-s5); adjacent to '
            'AudioSubStructTwoCallInit, AudioTreeWalkPredicateSearch, '
            'AudioRwsChunkHeaderRead, AudioWaveVtableSlot1cDispatch, '
            'AudioFmtDescCopy, AudioFmtTableSearch, AudioContextLookup. '
            'Working hypothesis: audio format descriptor + RWS chunk-tree + '
            'wave vtable dispatch. Confirm or reclassify during decomp.'
        ),
    },
]

# Take 80 from each band
for band in bands:
    cands = [u for u in unmapped if band['lo_seek'] <= u[0] < band['hi_seek']][:80]
    assert len(cands) == 80, f"Band {band['label']}: only {len(cands)} candidates"
    band['cands'] = cands
    band['lo'] = cands[0][0]
    band['hi'] = cands[-1][0] + cands[-1][2]
    band['span'] = band['hi'] - band['lo']
    band['avg'] = sum(c[2] for c in cands) / 80

header = """==============================================================================
batch_y.txt  -  Mashed C1 brute-force  -  2026-05-19
==============================================================================

Mode:           Opus 1M first-pass C0/unmapped -> C1 discovery
Sessions:       6
RVAs per sess:  80  (Opus 1M hard cap 100; default 80)
Total RVAs:     480
Unmapped pool:  1118 (post-exclusion; full-inventory unmapped before
                exclusions = 1518. The 9 library-residue bands + batch-x
                ranges remove 400 candidates.)
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
  0x004b4a80..0x004c4000  - Lua 5.0 source EXTENDED (batch-x-s2 finding
                            pushed the lower edge down from 0x004ba000 to
                            0x004b4a80; the source band now includes ~20
                            additional Lua VM internals - lstate/lgc/lvm/
                            lapi/ltable/lstring/lmem/llex/lobject/lopcodes/
                            lzio/ldebug plus lparser/lundump/lfunc/lcode)
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

Library-vs-game-code heuristic (batch_w post-mortem 2026-05-19, refined
by batch_x post-mortem 2026-05-19):
- Game code: avg function size >= 150 B/fn AND at least 5 game-named
  (non-FUN_/sub_/thunk_/library-prefixed) mapped neighbors within +/-0x1000
  of the bucket span.
- Library residue: dense small functions (avg < 100 B/fn) AND zero
  game-named neighbors within +/-0x2000.
- batch_x s2 caught Lua leakage below 0x004b4a80 by applying this heuristic
  - that is why this batch_y extends the Lua exclusion down from
  0x004ba000 to 0x004b4a80.
- Session 5 of batch_y (bucket_00565d50) has ZERO game-named anchors but
  avg 631 B/fn, which is the inverse signal - large functions but
  unexplored neighborhood. Treat as elevated STOP-AND-ASK risk per the
  per-session note.

"""

blocks = []
for i, band in enumerate(bands, 1):
    lo = band['lo']
    hi = band['hi']
    span = band['span']
    avg = band['avg']
    label = band['label']
    slot = band['slot']
    bucket_name = f"bucket_{lo:08x}"
    hypothesis = band['hypothesis']

    block = f"""==============================================================================
Session {i} - {bucket_name}  (FIRST-PASS, 80-RVA discovery)
==============================================================================

Pool slot:    Mashed_pool{slot} (pre-assigned)
Model:        Opus 4.7 (claude-opus-4-7[1m]) - hard cap 100 RVAs (this session: 80)
Skill:        discover-c1-batch (first-pass; Opus 1M mode)
Bucket dir:   re/analysis/{bucket_name}/
Drains to:    re/SCRIBE_QUEUE.md (Queued); ghidra-sweep picks up later
Address span: 0x{lo:08x} .. 0x{hi:08x}  (~{span:,} bytes of code, avg ~{avg:.0f}B/fn)
Bucket label: {label}

# Mission
Discover and write C0->C1 plates for 80 unmapped functions in the
contiguous-RVA bucket anchored at 0x{lo:08x}. {hypothesis}
These functions have no row in hooks.csv and no analysis note. Report
final subsystem in the queue row's `subsystem_observed=` field.

# Pre-flight
```bash
export POOL_SLOT_FILE=".pool_slot_batch_y_s{i}"
bash scripts/ghidra_pool.sh acquire   # should return Mashed_pool{slot} if free
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
  6. Write re/analysis/{bucket_name}/0x<rva>.md with:
     - frontmatter (rva, name_in_ghidra, size_bytes, confidence=C1,
       callees_depth1, callers_noted, opened_in_slot=Mashed_pool{slot},
       session_date=2026-05-19)
     - ## Mechanical description (bullets; no speculation; cite addresses)
     - ## Constants (table; cite RVA for each)
     - ## Uncertainties (bare [UNCERTAIN] markers; sweep assigns U-IDs)
     - ## Stubs encountered (bare [STUB] markers; sweep assigns S-IDs)

# Candidates
| # | RVA | Ghidra name | size |
|---|-----|-------------|------|
"""
    for j, (rva, gname, size) in enumerate(band['cands'], 1):
        block += f"| {j} | 0x{rva:08x} | {gname} | {size} |\n"

    block += f"""
# Output (this session ONLY produces these - does NOT touch hooks.csv)
- 80 per-RVA .md plates in re/analysis/{bucket_name}/
- ONE queue row appended to re/SCRIBE_QUEUE.md (Queued section):
    2026-05-19  batch-y-s{i}  rvas=0x...,0x...  bucket=re/analysis/{bucket_name}
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
  per batch_w post-mortem; batch_x-s2 caught Lua leakage this way).
- RW catalog match found but signature mismatches Ghidra decomp -> flag, do
  NOT auto-name; defer to a re-classify pass.
- More than 5 candidates have catalogued [UNCERTAIN] markers above C2 -> halt
- Adjacent functions form a known struct shape (>= 3 share offsets) ->
    optionally produce re/analysis/structs/<name>.md alongside the plates

"""
    blocks.append(block)

content = header + "".join(blocks)
with open('batch_y.txt', 'w', encoding='utf-8', newline='\n') as f:
    f.write(content)

print(f"Wrote batch_y.txt ({len(content)} bytes)")
for i, band in enumerate(bands, 1):
    print(f"  s{i}: bucket_{band['lo']:08x}  span={band['span']}B  avg={band['avg']:.0f}B  {band['label']}")
