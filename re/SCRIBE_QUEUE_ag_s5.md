# Scribe Queue fragment — batch_ag Session 5 (util C1→C2 promote-c2, AUTHOR-ONLY)

Generated 2026-06-01. Pool slot: Mashed_pool13 (read-only clone; .pool_slot_ag_s5).
For central finalize: cat this row into re/SCRIBE_QUEUE.md "## Queued" section.

## Queued

2026-06-01  batch-ag-s5  bucket=re/analysis/bucket_util_0052daf0_00582680  rvas=0052daf0,0052ddc0,0052ddf0,0052de60,0052df40,00534920,00550430,00550790,00550be0,0055dec0,0057c5b0,0057c650,0057c670,0057ca30,0057cae0,0057cb30,0057ce90,0057d370,0057d580,0057d990,0057de60,0057e750,0057e820,0057ec70,0057ed80,0057ee00,0057f030,0057f080,0057f0c0,0057f200,0057f4e0,0057f750,0057f860,0057f940,0057f970,00580090,005803a0,005808a0,00580ac0,00580ea0,00580fc0,00581120,00581170,005811e0,00581290,00581320,00581540,005815f0,00581680,005817f0,00581930,00581a40,00581ff0,00582220,005822f0,00582390,00582420,005824a0,00582550,00582680  note=60/60 plated C1->C2; 0 drift-skips (all were C1); 0 LAB_/function_create needed (all had boundaries); 0 stop-and-ask (function_at + decomp resolved every RVA).

## SUBSYSTEM RECLASSIFICATIONS (subsystem_observed != util — central re-classify MUST apply)

### A. Qhull 2002.1 third-party library residue → subsystem `third-party-library[qhull-2002.1]`  (50 RVAs)
The contiguous band 0x0057c5b0..0x00582680 is statically-linked **Qhull 2002.1** (matches
the existing qhull island at 0x0059b460+ promoted via lever3-libs; version literal
"2002.1 2002.8.20" embedded in FUN_0057f970/qh_initbuild @ s_2002_1_2002_8_20_00625e04).
Identified per-function by embedded `qh_*` format strings; several already carry qh_ names
in hooks.csv (qh_fprintf 0057c5b0, qh_check_bestdist 0057cb30) and one calls the already-named
master symbol `qh_printfacet` (from 00581ff0). Treat as subsystem-tag attestation (C2),
do NOT deep-RE, do NOT rename in the sweep beyond optional qh_* labels.
RVAs (with identified qhull function where a string/name pinned it):
  0057c5b0 qh_fprintf | 0057c650 qh_fprintf(arg-swap thunk) | 0057c670 qh output-table builder |
  0057ca30 qh output API (opt "qhull s Pp") | 0057cae0 qh hashtable-insert | 0057cb30 qh_check_bestdist |
  0057ce90 qh_check_maxout | 0057d370 qh_check_output (flip-check driver) | 0057d580 qh_check_points |
  0057d990 qh_checkconvex | 0057de60 qh_checkfacet | 0057e750 qh flipped-facet check |
  0057e820 qh_checkpolygon | 0057ec70 qh_checkvertex | 0057ed80 qh_clearcenters |
  0057ee00 qh_createsimplex | 0057f030 qh_delridge | 0057f080 qh_delfacet | 0057f0c0 qh_facet3vertex |
  0057f200 qh_findgood | 0057f4e0 qh_findgood_all | 0057f750 qh_furthestnext | 0057f860 qh_furthestout |
  0057f940 qh internal-error(infinitevertex) | 0057f970 qh_initbuild(VERSION ANCHOR) | 00580090 qh_initialhull |
  005803a0 qh_initialvertices | 005808a0 qh_makenewfacets | 00580ac0 qh_matchduplicates |
  00580ea0 qh_nearcoplanar(coplanar prune) | 00580fc0 qh_nearvertex | 00581120 qh_newhashtable |
  00581170 qh_nextridge3d | 005811e0 qh_outcoplanar | 00581290 qh point-table writer |
  00581320 qh_pointfacet | 00581540 qh_pointvertex | 005815f0 qh_prependfacet |
  00581680 qh_printhashtable | 005817f0 qh_printlists | 00581930 qh_resetlists | 00581a40 qh_triangulate |
  00581ff0 qh_triangulate_facet | 00582220 qh_triangulate_link | 005822f0 qh_triangulate_mirror |
  00582390 qh_vertexintersect(in-place) | 00582420 qh_vertexintersect_new | 005824a0 qh_vertexneighbors |
  00582550 qh_setaddnth | 00582680 qh_setappend

### B. Image/surface library cluster → subsystem_observed `image-library` (5 RVAs)
0x0052daf0..0x0052df40 is a custom image format-handler registry + surface helpers built over
the RenderWare memory manager (DAT_007d3ff8+0x108, tag 0x3001b) and a format-descriptor table
at DAT_00912160. hooks.csv tags 0052daf0 "image-library (libpng/zlib)" but the mechanical read
shows it is Mashed/RW image-loading GLUE, NOT pure libpng/zlib code. Suggest subsystem=render
or a dedicated `image-library` tag at finalize discretion; mechanically C2-ready either way.
RVAs: 0052daf0 (format_handler_registry dispatch), 0052ddc0 (node-count), 0052ddf0 (surface ctor),
      0052de60 (row-table bind), 0052df40 (payload memcpy).

### C. Genuine util (subsystem stays `util`) (5 RVAs)
00534920 (RW plugin registrar; RNG plugin — hooks.csv tags random_rng), 00550430 (capacity-guarded
registry list prepend — hooks.csv tags input_dinput_d3), 00550790 (setter DAT_007dc76c),
00550be0 (registry list sweep), 0055dec0 (trivial *param_1 getter).

## U-IDs filed (range U-6300..U-6399)
U-6300 (0052daf0 image-descriptor field semantics), U-6301 (0052ddc0 dual-cursor count),
U-6302 (0052ddf0 surface field init meanings), U-6303 (0052de60 two-plane interpretation),
U-6304 (0052df40 width/height field identity), U-6305 (00550430 dedup loop early-return),
U-6306 (00550790 DAT_007dc76c meaning), U-6307 (00550be0 element +0x20 state / +0x3c vtable).
All are data/struct-semantic uncertainties — non-blocking for C2 (mechanical behaviour recorded).

## S-IDs filed (range S-4800..S-4899)
None — every callee resolved to a known RVA or data-driven function pointer; no STUBS.

## Notes for central finalize
- No drift-skips: all 60 were C1 in hooks.csv at author time.
- The 50 qhull rows: central re-classify should flip subsystem util -> third-party-library[qhull-2002.1]
  AND confidence C1 -> C2 (subsystem-tag attestation, consistent with the 0x0059b460 qhull band).
- ghidra-sweep may optionally apply qh_* names per the per-RVA identifications above (master only).
