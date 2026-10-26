# RESULT D4-WSH-NOEVID (2026-10-26)

Scope: C4 rows with empty `frida_diff` (4) plus C4 rows whose `file` is an `.md` with no `exe_file` (12).
`0x004c5800` is in both lists, so the set is **15 distinct RVAs**, not 16. Text-only plus tracker edits through
re-classify. No build, no game run, no default touched, gates not run (no source or exe change).

Method: body path from `RH_ScopedInstall` grep over `mashedmod/src`, diff citation checked for file existence,
registry entry in `re/frida/hooks_registry.py` read for the arg_type, install-observe logs grepped for the RVA.
Rubric: `re/CONFIDENCE.md` (C4 = clean diff-original on a canonical scenario).
`log/` is gitignored, so any `log/*` citation is a local file only.

## Kept at C4 (7): body path set in `file`, diff citation exists

| RVA | Body | Diff citation |
|---|---|---|
| 0x004c5820 | Render/TextureLoaderCluster.cpp:101 | negctl reearn_20261011 diff_rw_tex_dictionary_get_current.csv |
| 0x00475a60 | Util/UtilMid.cpp:799 | negctl reearn_20261011 diff_pending_op_queue_flush.csv |
| 0x004cc6e0 | Render/D3D9Helpers_q5.cpp:110 | negctl reearn_20261011 diff_rw_stream_write_chunked.csv |
| 0x004cc770 | Render/RwStreamWriteBytes.cpp:69 | negctl reearn_20261011 diff_rw_stream_write_bytes.csv |
| 0x004241c0 | Frontend/FrontendLeaves_ad1.cpp:111 | negctl reearn_20261011 diff_team_block_one_get.csv |
| 0x0046bce0 | Gameplay/GameplayLeaves_ad3.cpp:46 | log/diff_vehicle_vec3_at_94_get.csv + verify/c4_vehicle_vec3/EVIDENCE.md |
| 0x004464c0 | Util/CameraEntryDispatch.cpp:125 | c4race_compare_4464c0.csv (session_20261012b) |

Caveat 0x004464c0: evidence covers the `.asi` naked copy only. The standalone keeps its own C body
(VehicleSeed.cpp, CameraEntries.h), so its exe evidence belongs to D4-WSH-EXEFILE. 9-field short row, no exe_file cell.

## Demoted C4 -> C3 (8)

| RVA | Body | Why |
|---|---|---|
| 0x004c5800 | Render/TextureLoaderCluster.cpp:73 | no frida_diff. Registry arg_type int_scalar compares the constant return 1 only, the slot write is unobserved. Memory note checked: the tracker name is wrong (U-9729), slot +0x1c is a format-flag word, the dictionary pair is 0x004c5c80/0x004c5ca0. |
| 0x00431ae0 | Boot/BootLowRvaCluster.cpp:322 | no frida_diff, no committed output for the void_write_observe entry (target 0x007f0f04), count=1 boot sighting only |
| 0x00431af0 | Boot/BootLowRvaCluster.cpp:347 | same, target 0x007f0f08 |
| 0x00431b00 | Boot/BootLowRvaCluster.cpp:372 | same, target 0x007f0f00 |
| 0x00439210 | Frontend/SpriteCluster.cpp:377 | registry records it REFUSED (hooks_registry.py:8595), never diffed. Cited log line is an install manifest row, not a firing |
| 0x00428450 | HUD/TextCluster.cpp:285 | cited log/mass_canonical_wave0.txt is absent. Earlier note: Interceptor on the original, broken-loader window. Install-observe is survival only |
| 0x004997b0 | Render/D3D9Helpers_q5.cpp:184 | cited log absent. Registry: no arg_type fit for the 4-arg signature, so no diff exists |
| 0x00408590 | Util/PromoLoop_sessionB.cpp:3306 | frida_diff is the token `green-earlywindow-r191`, no C3->C4 note line. Early-window is a synthetic force-call, same ruling as 0x0046bce0 on 2026-07-31. Exe naked copy PP_408590 (PathProgStep_naked.inc:1004) uncovered |

For the three boot setters `exe_file` was already BootLowRvaCluster.cpp, `file` now matches it (one body).
Re-earn path for every demotion is in that row's `notes` (appended with the 2026-10-26 stamp) and CHANGELOG.

## Tracker transaction
- `hooks.csv`: 15 rows touched (15 lines changed, CRLF preserved). `file` set to a body path on all 15.
- `re/analysis/CHANGELOG.md`: 15 lines prepended below `<!-- ENTRIES -->`.
- STUBS / UNCERTAINTIES: no new rows. No `[UNCERTAIN]` markers added.
- Raw data: none committed, no external raw files cited by this RESULT.
