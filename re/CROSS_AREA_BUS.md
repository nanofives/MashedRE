<!-- CROSS-AREA FINDING BUS -->
<!-- Star topology: children report cross-area findings to the PARENT orchestrator.     -->
<!-- The parent (and ONLY the parent) appends rows below the ENTRIES marker and pings    -->
<!-- the affected child via mcp__happy__send_to_session. A child NEVER edits this file    -->
<!-- directly; it messages the parent, which arbitrates and records.                     -->
<!--                                                                                      -->
<!-- This is the DURABLE channel: a finding survives here whether or not the affected     -->
<!-- session is live. The send_to_session ping is the LIVE channel (steering only); if    -->
<!-- the peer is idle/closed it will read the row on its next wake instead.               -->
<!--                                                                                      -->
<!-- Row schema (one row per finding, pipe-delimited):                                    -->
<!--   id | date | from-area | affects-area | kind | anchor | claim | status | resolution -->
<!--     id          B-#### monotonic, never reused                                       -->
<!--     from-area   canonical subsystem that discovered it                               -->
<!--     affects-area canonical subsystem(s) that must react (comma-sep)                  -->
<!--     kind        struct-offset | global | shared-rva | dispatcher | format | assumption-->
<!--     anchor      RVA / struct+offset / global addr the claim is about (cite, per NO-GUESSING)-->
<!--     claim       one line, mechanical, no intent words                                -->
<!--     status      OPEN | PINGED | ACK | LANDED | REJECTED                              -->
<!--     resolution  filled when status leaves OPEN: what the affected area did           -->
<!--                                                                                      -->
<!-- Rule: a struct/global/shared-rva finding is ALSO a merge fact. Once the scribe (or   -->
<!-- the header edit that documents it) lands it, both areas read the same value; the bus -->
<!-- row moves to LANDED and cites where it landed. Do not carry a LANDED fact as OPEN.   -->

# Cross-area finding bus

Next id: **B-0006**

## Active

<!-- ENTRIES -->
B-0005 | 2026-09-01 | frontend | hud | shared-global-block | 0x0067ea94-0x0067ecf4 | The frontend-state global block. area/frontend r5 characterised its remaining ~59 doc-only C2 rows and found EVERY one gated on reads/writes into this dword window (enumerated for 0x00430670 as DAT_0067e9fc plus 0x0067ea94..0x0067eaa8), which is why the child-synthetic frontier there is exhausted: hook-bypassed path1 on these is a degenerate green. FILED FOR HUD because hud is highly likely to read the same window and would otherwise re-derive it from scratch; hud is currently MINED-OUT (2 dry rounds, B-0001 refuted) so this is a note for whenever it is restaffed, NOT a live blocker. NOT ESTABLISHED, and deliberately not asserted: which specific offsets hud reads, and whether any are written by both areas. Evidence re/analysis/frontend_round5_frontier.md. Parent is authorising frontend to author a seed_globals+fold_ret arg_type to seed this window per-test, which is the harness both areas would share.
B-0004 | 2026-09-01 | render | audio | harness | re/frida/verify_hook_install_template.js:callFn | fmt_desc_pair_compare path2 call-through now wired (2 self-alloc 0x40 bufs, sparse fNN u32 writes, dual-buffer fingerprint); audio fmt-desc hooks 0x005ac5f0/0x005ac9e0/0x005ad540 get working path2 call-through once this lands on main. Same class as GAP-5. | OPEN | SWEEP-CRITICAL: carry the callFn change into the frida-sweep before any audio path2. On branch area/render.
B-0003 | 2026-09-01 | hud | gameplay | signature-correction | 0x004726f0 | FUN_004726f0 (C3, plated void(float*,float*) in the port) actually RETURNS a float on ST0 — evidence: in 00412cf0 the FPU is empty after Vec3Magnitude's FSTP, then CALL 004726f0, then FCOM reads ST0 = its return; result feeds record byte+0x27. A void-typed port breaks every ST0-dependent caller. | OPEN | PARENT-OWNED (no gameplay child): verify via headless decomp + fix the port signature; highest severity (shipped C3 correctness).
B-0002 | 2026-09-01 | hud | util | doc-correctness | 0x004a2c48 | FUN_004a2c48 is __ftol (x87 float->int64, C3, Math/FPURound.cpp), NOT "QPC tick" as the util_c0_promote / 0x00412cf0.md plate labels it. | OPEN | PARENT-OWNED (no util child): plate/doc fix, defer to a re-classify pass. Low severity.
B-0001 | 2026-09-01 | hud | render | module-reclass | 0x00553000-0x00557fff | The "font-vector 2D" band (00554010/150/200/390, 00555830, 00556780, 00556e40, 005571c0/e0, FontSys_*) is suspected vendored RenderWare Rt2d (module-vendor-doubt in ~15 plates); FlushMatrix's callers 00552890/00552920 are ALREADY reclassed third-party-library[renderware] Rt2d (corroborates). A confirmed Rt2d calibration reclasses-OUT ~15 rows to library-skip C1, shrinking hud residue. Was mislabeled render (batch_ao). | RESOLVED=REFUTED | hud ran the calibration (re/analysis/area_hud_round2_rt2d_calibration.md): the band is FIRST-PARTY FGDC20.RWF font subsystem (a Mashed asset, original/TOASTART/Common/Font36.piz), NOT vendored RW — its callees are first-party render/boot (004cd070/004cd140/004cd170, 005c4c60/4d30/4da0), only 2 are genuinely 3rd-party, and the port already reimplements it (D3d9Render/MashedFont.cpp etc). NO reclass-OUT; hud residue does NOT shrink (real first-party work). Group A (named/hand-plated C3) never eligible; Group B (9 anon rows) refute too. Action: resolve the module-vendor-doubt on Group B plates to REFUTED so it can't resurface. Good honest catch — closed REFUTED not LANDED.

## Resolved

<!-- RESOLVED -->
