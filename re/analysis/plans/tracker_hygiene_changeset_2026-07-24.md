# Tracker hygiene changeset (2026-07-24)

Generated read-only by account2 Lane A. All quoted lines verified live in the repo at time of analysis. Line numbers cited for navigation only; apply by content-match, not line number, since concurrent sessions may have shifted offsets.

---

## Transactions ÔÇö hooks.csv

### TXN-01 through TXN-13 ÔÇö T1.1: De-duplicate file-offset vs VA row pairs

**Background.** Rows with RVAs in `000xxxxx` file-offset form duplicate the same VA as `004xxxxx`/`005xxxxx` canonical rows. For each pair: keep the row with the richer/semantic name, delete the other. For pairs 1ÔÇô7 the semantic name is in the file-offset row; the surviving row is the VA row but needs its `name` and `subsystem` updated. For pairs 8ÔÇô11 the file-offset row is richer and the VA row has `FUN_` + lower confidence; the surviving row is the file-offset row but needs its `rva` corrected to canonical. For pairs 12ÔÇô13 the VA row is already C3/impl and is the definitive survivor; delete the file-offset row only.

---

**TXN-01** | `hooks.csv` | merge-pair (delete + update) | VA `00495280` Ôåö file-offset `00095280`

DELETE (hooks.csv line 1152):
```
00095280,OpenPizFile,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/00095280.md,,,FUN_00495280; path-normalize via FUN_00402b70; logs Opening piz file %s; calls PizOpen; see re/analysis/piz_fsmanager_handler/REPORT.md
```

REPLACE (hooks.csv line 435) ÔÇö update `name`, `subsystem`, `status`, `file`, merge `notes`:
```
Current:  00495280,FUN_00495280,render,C2,new,re/analysis/promote_c2_piz_loader/00495280.md,promote_c2_piz_loader-20260516,,piz open wrapper: FUN_00402b70 builds path; FUN_004b6570 opens piz; logs OK/FAILED
Replace:  00495280,OpenPizFile,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/00095280.md,,,path-normalize via FUN_00402b70; logs Opening piz file %s; calls PizOpen; see re/analysis/piz_fsmanager_handler/REPORT.md [merged from promote_c2_piz_loader plate 2026-05-16]
```
*Rationale: T1.1; semantic name and subsystem from file-offset row; canonical VA retained; two analysis notes merged.*

---

**TXN-02** | `hooks.csv` | merge-pair | VA `004952f0` Ôåö file-offset `000952f0`

DELETE (hooks.csv line 1153):
```
000952f0,ClosePizFile,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000952f0.md,,,FUN_004952f0; logs Closing piz file %s; calls thunk_FUN_004b67a0; see re/analysis/piz_fsmanager_handler/REPORT.md
```

REPLACE (hooks.csv line 436):
```
Current:  004952f0,FUN_004952f0,render,C2,new,re/analysis/promote_c2_piz_loader/004952f0.md,promote_c2_piz_loader-20260516,,piz close wrapper: FUN_004b65c0 gets name; log; thunk_FUN_004b67a0 closes piz
Replace:  004952f0,ClosePizFile,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000952f0.md,,,logs Closing piz file %s; calls thunk_FUN_004b67a0; see re/analysis/piz_fsmanager_handler/REPORT.md [merged from promote_c2_piz_loader plate 2026-05-16]
```
*Rationale: T1.1; same pattern as TXN-01.*

---

**TXN-03** | `hooks.csv` | merge-pair | VA `004b6570` Ôåö file-offset `000b6570`

DELETE (hooks.csv line 1151):
```
000b6570,PizOpen,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000b6570.md,,,FUN_004b6570; thin bool wrapper for PizOpenAndParse; see re/analysis/piz_fsmanager_handler/REPORT.md
```

REPLACE (hooks.csv line 3666):
```
Current:  004b6570,FUN_004b6570,render,C2,mapped,re/analysis/render_3_c1_to_c2_s5/FUN_004b6570.md,batch-x-s2,,bool wrapper for FUN_004b6940 PizOpenAndParse | batch-render-3-s5 plate at re/analysis/render_3_c1_to_c2_s5/FUN_004b6570.md
Replace:  004b6570,PizOpen,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000b6570.md,,,thin bool wrapper for PizOpenAndParse; see re/analysis/piz_fsmanager_handler/REPORT.md [merged from render_3 plate batch-x-s2]
```
*Rationale: T1.1.*

---

**TXN-04** | `hooks.csv` | merge-pair | VA `004b6710` Ôåö file-offset `000b6710`

DELETE (hooks.csv line 1147):
```
000b6710,PizWin32Open,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000b6710.md,,,FUN_004b6710; BYPASS SITE: CreateFileA(path GENERIC_READ OPEN_EXISTING FILE_FLAG_OVERLAPPED); handle->DAT_007d3e48; alt path via FUN_004a4541 when DAT_007d3e50&1; see re/analysis/piz_fsmanager_handler/REPORT.md
```

REPLACE (hooks.csv line 3671):
```
Current:  004b6710,FUN_004b6710,render,C2,mapped,re/analysis/render_3_c1_to_c2_s5/FUN_004b6710.md,batch-x-s2,,piz file open; EAX=path; CreateFileA GENERIC_READ OPEN_EXISTING FILE_FLAG_OVERLAPPED|NO_BUFFERING; alt stdio FUN_004a4541; handleÔåÆDAT_007d3e48 | batch-render-3-s5 plate at re/analysis/render_3_c1_to_c2_s5/FUN_004b6710.md
Replace:  004b6710,PizWin32Open,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000b6710.md,,,BYPASS SITE: CreateFileA(path GENERIC_READ OPEN_EXISTING FILE_FLAG_OVERLAPPED); handle->DAT_007d3e48; alt path via FUN_004a4541 when DAT_007d3e50&1; see re/analysis/piz_fsmanager_handler/REPORT.md [merged from render_3 plate batch-x-s2]
```
*Rationale: T1.1.*

---

**TXN-05** | `hooks.csv` | merge-pair | VA `004b6770` Ôåö file-offset `000b6770`

DELETE (hooks.csv line 1149):
```
000b6770,PizWin32Close,util,C2,hooked,mashedmod/src/mashed_re/Compat/PizWin32Bypass.cpp,,,"FUN_004b6770; BYPASS SITE: CloseHandle on DAT_007d3e48 / alt path _fclose when DAT_007d3e50&1; original Win32 branch (0x004b6792..0x004b679f) does NOT null DAT_007d3e48 after CloseHandle while stdio branch (0x004b6779..0x004b6791) does at 0x004b6787 ÔÇö see U-42 (re/analysis/cluster_004b4_first_pass/004b6770.md); PizWin32Close_Compat closes the asymmetry defensively (sole caller FUN_004b67a0 nulls explicitly at 0x004b67d1 so bug is latent today); arg_type=harness_limited; C2 ÔÇö boot-test only, no canonical-scenario diff because the bug is unobservable in current callers"
```

REPLACE (hooks.csv line 3672):
```
Current:  004b6770,FUN_004b6770,render,C2,mapped,re/analysis/render_3_c1_to_c2_s5/FUN_004b6770.md,batch-x-s2,,piz file close; CloseHandle (Win32 path no null) or _fclose+null (stdio path); DAT_007d3e48 | batch-render-3-s5 plate at re/analysis/render_3_c1_to_c2_s5/FUN_004b6770.md
Replace:  004b6770,PizWin32Close,util,C2,hooked,mashedmod/src/mashed_re/Compat/PizWin32Bypass.cpp,,,"BYPASS SITE: CloseHandle on DAT_007d3e48 / alt path _fclose when DAT_007d3e50&1; Win32 branch does NOT null DAT_007d3e48 (see U-42); PizWin32Close_Compat closes the asymmetry defensively; caller FUN_004b67a0 nulls at 0x004b67d1 so bug is latent; C2 boot-test only [merged from render_3 plate batch-x-s2]"
```
*Rationale: T1.1; `hooked` status and bypass implementation path preserved from file-offset row.*

---

**TXN-06** | `hooks.csv` | merge-pair | VA `004b67e0` Ôåö file-offset `000b67e0`

DELETE (hooks.csv line 1148):
```
000b67e0,PizWin32Read,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000b67e0.md,,,FUN_004b67e0; BYPASS SITE: ReadFile+GetOverlappedResult async loop on DAT_007d3e48; alt path fseek/fread when DAT_007d3e50&1; see re/analysis/piz_fsmanager_handler/REPORT.md
```

REPLACE (hooks.csv line 3674):
```
Current:  004b67e0,FUN_004b67e0,render,C2,mapped,re/analysis/render_3_c1_to_c2_s5/FUN_004b67e0.md,batch-x-s2,,piz read; offset+dst+size+pBytesRead; Win32 overlapped OVERLAPPED spin-poll loop; alt fseek/fread; progress callback DAT_007d3e5c | batch-render-3-s5 plate at re/analysis/render_3_c1_to_c2_s5/FUN_004b67e0.md
Replace:  004b67e0,PizWin32Read,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000b67e0.md,,,BYPASS SITE: ReadFile+GetOverlappedResult async loop on DAT_007d3e48; alt path fseek/fread when DAT_007d3e50&1; see re/analysis/piz_fsmanager_handler/REPORT.md [merged from render_3 plate batch-x-s2]
```
*Rationale: T1.1.*

---

**TXN-07** | `hooks.csv` | merge-pair | VA `004b6940` Ôåö file-offset `000b6940`

DELETE (hooks.csv line 1150):
```
000b6940,PizOpenAndParse,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000b6940.md,,,FUN_004b6940; reads piz header+directory into DAT_0090dac0; calls PizWin32Open+PizWin32Read; separate from RW VFS; see re/analysis/piz_fsmanager_handler/REPORT.md
```

REPLACE (hooks.csv line 3677):
```
Current:  004b6940,FUN_004b6940,render,C2,mapped,re/analysis/render_3_c1_to_c2_s6/FUN_004b6940.md,batch-x-s2,,PizOpenAndParse: zeros state; strncpy path; calls FUN_004b65e0+FUN_004b6710; reads dir into 0x0090dac0; sets DAT_007d3e4c=1; caller FUN_004b6570; batch-render-3-s6 plate at re/analysis/render_3_c1_to_c2_s6/FUN_004b6940.md
Replace:  004b6940,PizOpenAndParse,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/000b6940.md,,,reads piz header+directory into DAT_0090dac0; calls PizWin32Open+PizWin32Read; separate from RW VFS; see re/analysis/piz_fsmanager_handler/REPORT.md [merged from render_3 plate batch-x-s2]
```
*Rationale: T1.1.*

---

**TXN-08** | `hooks.csv` | merge-pair | VA `005504d0` Ôåö file-offset `001504d0`

DELETE (hooks.csv line 3833) ÔÇö drop the FUN_ / lower-confidence VA row:
```
005504d0,FUN_005504d0,third-party-library[renderware],C1,mapped,re/analysis/bucket_00549580/0x005504d0.md,batch-x-s4,,"batch_x s4 MIXED RW-plugin code; RpPatch + RtFS + Rt2d + anim + image-loader; mostly RenderWare plugin glue | reclass-OUT batch_ar s5 sweep-20260604-0020: vendored renderware primitive (RpPatch 0x123 toolkit + texdict stream readers + RtFS manager + plugin-0x135 registrar) mislabeled render, kept-C1 (not hand-plated); see project_qhull_rwphysics_island"
```

REPLACE (hooks.csv line 1146) ÔÇö fix RVA from file-offset to canonical VA:
```
Current:  001504d0,RtFSManager::FindHandler,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/001504d0.md,,,FUN_005504d0; walks linked list DAT_007dc754 comparing name via RW vtable DAT_007d3ff8+0xe8; see re/analysis/piz_fsmanager_handler/REPORT.md
Replace:  005504d0,RtFSManager::FindHandler,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/001504d0.md,,,walks linked list DAT_007dc754 comparing name via RW vtable DAT_007d3ff8+0xe8; see re/analysis/piz_fsmanager_handler/REPORT.md
```
*Rationale: T1.1; file-offset row has semantic name + C2; VA row has FUN_ + C1 ÔÇö keep C2 semantics, correct RVA.*

---

**TXN-09** | `hooks.csv` | merge-pair | VA `00551090` Ôåö file-offset `00151090`

DELETE (hooks.csv line 3842):
```
00551090,FUN_00551090,third-party-library[renderware],C1,mapped,re/analysis/bucket_00549580/0x00551090.md,batch-x-s4,,"batch_x s4 MIXED RW-plugin code; RpPatch + RtFS + Rt2d + anim + image-loader; mostly RenderWare plugin glue | reclass-OUT batch_ar s5 sweep-20260604-0020: vendored renderware primitive (RpPatch 0x123 toolkit + texdict stream readers + RtFS manager + plugin-0x135 registrar) mislabeled render, kept-C1 (not hand-plated); see project_qhull_rwphysics_island"
```

REPLACE (hooks.csv line 1140) ÔÇö fix RVA:
```
Current:  00151090,RtFSHandler::Poll,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/00151090.md,,,piz_fsmanager_handler vtable+0x3c; GetOverlappedResult; handles 0x3e4 ERROR_IO_INCOMPLETE 0x3e5 ERROR_IO_PENDING; see re/analysis/piz_fsmanager_handler/REPORT.md
Replace:  00551090,RtFSHandler::Poll,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/00151090.md,,,piz_fsmanager_handler vtable+0x3c; GetOverlappedResult; handles 0x3e4 ERROR_IO_INCOMPLETE 0x3e5 ERROR_IO_PENDING; see re/analysis/piz_fsmanager_handler/REPORT.md
```
*Rationale: T1.1.*

---

**TXN-10** | `hooks.csv` | merge-pair | VA `00551190` Ôåö file-offset `00151190`

DELETE (hooks.csv line 3843):
```
00551190,FUN_00551190,third-party-library[renderware],C1,mapped,re/analysis/bucket_00549580/0x00551190.md,batch-x-s4,,"batch_x s4 MIXED RW-plugin code; RpPatch + RtFS + Rt2d + anim + image-loader; mostly RenderWare plugin glue | reclass-OUT batch_ar s5 sweep-20260604-0020: vendored renderware primitive (RpPatch 0x123 toolkit + texdict stream readers + RtFS manager + plugin-0x135 registrar) mislabeled render, kept-C1 (not hand-plated); see project_qhull_rwphysics_island"
```

REPLACE (hooks.csv line 1145) ÔÇö fix RVA:
```
Current:  00151190,RtFSHandler::Install,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/00151190.md,,,FUN_00551190; allocates 0x5c-byte handler struct + 0x14*0x60 slot array; populates 12 vtable fn ptrs; calls FUN_00551330; see re/analysis/piz_fsmanager_handler/REPORT.md
Replace:  00551190,RtFSHandler::Install,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/00151190.md,,,allocates 0x5c-byte handler struct + 0x14*0x60 slot array; populates 12 vtable fn ptrs; calls FUN_00551330; see re/analysis/piz_fsmanager_handler/REPORT.md
```
*Rationale: T1.1.*

---

**TXN-11** | `hooks.csv` | merge-pair | VA `005512b0` Ôåö file-offset `001512b0`

DELETE (hooks.csv line 3844):
```
005512b0,FUN_005512b0,third-party-library[renderware],C1,mapped,re/analysis/bucket_00549580/0x005512b0.md,batch-x-s4,,"batch_x s4 MIXED RW-plugin code; RpPatch + RtFS + Rt2d + anim + image-loader; mostly RenderWare plugin glue | reclass-OUT batch_ar s5 sweep-20260604-0020: vendored renderware primitive (RpPatch 0x123 toolkit + texdict stream readers + RtFS manager + plugin-0x135 registrar) mislabeled render, kept-C1 (not hand-plated); see project_qhull_rwphysics_island"
```

REPLACE (hooks.csv line 1143) ÔÇö fix RVA:
```
Current:  001512b0,RtFSHandler::GetStatus,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/001512b0.md,,,piz_fsmanager_handler vtable+0x48; if slot[+0x20]!=1 call Poll(slot 0); return slot[+0x20]; see re/analysis/piz_fsmanager_handler/REPORT.md
Replace:  005512b0,RtFSHandler::GetStatus,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/001512b0.md,,,piz_fsmanager_handler vtable+0x48; if slot[+0x20]!=1 call Poll(slot 0); return slot[+0x20]; see re/analysis/piz_fsmanager_handler/REPORT.md
```
*Rationale: T1.1.*

---

**TXN-12** | `hooks.csv` | merge-pair | VA `005514e0` Ôåö file-offset `001514e0`

The VA row is already C3/impl with a Frida log. Delete the file-offset row only.

DELETE (hooks.csv line 1142):
```
001514e0,RtFSHandler::IsEOF,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/001514e0.md,,,piz_fsmanager_handler vtable+0x44; compares slot[+0x8] pos vs slot[+0x0/+0x4] bounds; returns 1 at EOF; see re/analysis/piz_fsmanager_handler/REPORT.md
```

KEEP (hooks.csv line 5895, no change needed):
```
005514e0,RtFSHandler_IsEOF,util,C3,impl,mashedmod/src/mashed_re/Util/RtFSHandler.cpp,,log/diff_rtfshandler_is_eof.csv:GREEN-4/4,EOF predicate on piz fsmanager slot struct; cdecl fn(int* slot)->int; 64-bit pos-vs-size compare; C2->C3 frida-sweep-20260626-wf_b0f68acd-r1 integration-diff GREEN; ptr_arg_int_get 4/4 bit-identical
```
*Rationale: T1.1; VA row is the authoritative C3 implementation.*

---

**TXN-13** | `hooks.csv` | merge-pair | VA `00551180` Ôåö file-offset `00151180`

The VA row is already C3/impl. Delete the file-offset row only.

DELETE (hooks.csv line 1141):
```
00151180,RtFSHandler::Cancel,util,C2,mapped,re/analysis/bucket_util_00095280_0040e460/00151180.md,,,piz_fsmanager_handler vtable+0x40; slot[+0x20]=1; no OS call; see re/analysis/piz_fsmanager_handler/REPORT.md
```

KEEP (hooks.csv line 5896, no change needed):
```
00551180,RtFSHandlerCancel,util,C3,impl,mashedmod/src/mashed_re/Util/RtFSHandlerCancel.cpp,,log/diff_rtfs_handler_cancel.csv:GREEN-5/5,RtFSHandler::Cancel vtable+0x40 slot; *(slot+0x20)=1; return 1 (12B); C2->C3 frida-sweep-20260626-wf_b0f68acd-r1 integration-diff GREEN; ptr_arg_int_get 5/5 bit-identical
```
*Rationale: T1.1.*

---

### TXN-14 through TXN-22 ÔÇö T1.2 + T1.3: C4 rows with contradictory status or missing frida_diff evidence

**Note:** These rows require human judgment from re-classify. The changeset below proposes the minimum-plausible correction. If the C4 evidence is sound, update status; if not, demote the confidence level instead.

---

**TXN-14** | `hooks.csv` | replace | `0040b6d0` ÔÇö C4/mapped/empty-frida_diff (T1.2 + T1.3)

Current (line 322):
```
0040b6d0,FUN_0040b6d0,util,C4,mapped,re/analysis/game_state/0x0040b6d0.md,,,"returns (&DAT_008a94e0)[param_1]; getter for same array as FUN_0040b540; U-0491 | C2->C3 c3_batch_ac (s5-redo, Opus author+verify, central classify): Frida bit-identical GREEN via run_diff_warm + re-confirmed in integration build (8/8); arg_types harness-corrected by worker (void_write_observe/int_scalar/struct_three_write ÔÇö suggested ones would false-GREEN)"
```

Proposed replacement ÔÇö change `status` from `mapped` to `analyzed` (no reimplementation file exists; `mapped` is pre-analysis); set `frida_diff` to `pending` (no log pointer recorded despite C4 gate):
```
0040b6d0,FUN_0040b6d0,util,C4,analyzed,re/analysis/game_state/0x0040b6d0.md,,pending,"returns (&DAT_008a94e0)[param_1]; getter for same array as FUN_0040b540; U-0491 | C2->C3 c3_batch_ac (s5-redo, Opus author+verify, central classify): Frida bit-identical GREEN via run_diff_warm + re-confirmed in integration build (8/8); arg_types harness-corrected by worker (void_write_observe/int_scalar/struct_three_write ÔÇö suggested ones would false-GREEN)"
```
*Rationale: T1.2 (status `mapped` contradicts C4) + T1.3 (frida_diff blank). If a reimplementation .cpp was written, point `file` to it and change status to `impl`/`verified`.*

---

**TXN-15** | `hooks.csv` | replace | `00431ae0` ÔÇö C4/new/empty-frida_diff (T1.2 + T1.3)

Current (line 1770):
```
00431ae0,FUN_00431ae0,boot,C4,new,re/analysis/promote_c2_boot_lowrva/0x00431ae0.md,,,"promoted_from=re/analysis/boot_subsystem_d3/0x00431ae0.md; batch_u_s5_2026-05-18; DAT_007f0f04=0x3f333333 (0.7f); 10b setter; 007f0f00..0f10 param-defaults cluster; depth-3 via 004924f0 | C2->C3 via mass-canonical-wave2 (count=1 at boot-to-menu) | C3->C4 2026-05-23 c4-lift: canonical-scenario observation evidence with hook installed (fired during boot-to-menu or per-frame survival) ÔÇö meets C4 rubric per CONFIDENCE.md + feedback_no_overclaiming_c_levels | C4-EVIDENCE-RECONFIRMED(R1-B 2026-06-09, was suspect): canonical evidence in broken-loader window (2026-05-15..24); escaped 6e359fe8 tagging; tagged 2026-06-09 R0 audit. C4 RETAINED pending installed-hook canonical re-validation. | RECONFIRMED C4 (R1-B 2026-06-09): subset-install canonical boot-to-menu observe ÔÇö installed 0xE9 live + manifest installed=1 + 25s survival, no crash; log/install_observe_r1b_20260609.txt"
```

Proposed replacement ÔÇö change `status` from `new` to `impl`; set `frida_diff` to `log/install_observe_r1b_20260609.txt` (referenced in notes):
```
00431ae0,FUN_00431ae0,boot,C4,impl,re/analysis/promote_c2_boot_lowrva/0x00431ae0.md,,log/install_observe_r1b_20260609.txt,"[notes unchanged ÔÇö see current row]"
```
*Rationale: T1.2 (`new` contradicts C4) + T1.3 (frida_diff blank; log path is cited in the notes field). Apply the same pattern to TXN-16 and TXN-17.*

---

**TXN-16** | `hooks.csv` | replace | `00431af0` ÔÇö C4/new/empty-frida_diff (T1.2 + T1.3)

Current (line 1771):
```
00431af0,FUN_00431af0,boot,C4,new,re/analysis/promote_c2_boot_lowrva/0x00431af0.md,,,"promoted_from=re/analysis/boot_subsystem_d3/0x00431af0.md; batch_u_s5_2026-05-18; DAT_007f0f08=0x3f333333 (0.7f); 10b setter; paired with 00431ae0/b00/b10 | C2->C3 via mass-canonical-wave2 (count=1 at boot-to-menu) | C3->C4 2026-05-23 c4-lift: canonical-scenario observation evidence with hook installed (fired during boot-to-menu or per-frame survival) ÔÇö meets C4 rubric per CONFIDENCE.md + feedback_no_overclaiming_c_levels | C4-EVIDENCE-RECONFIRMED(R1-B 2026-06-09, was suspect): canonical evidence in broken-loader window (2026-05-15..24); escaped 6e359fe8 tagging; tagged 2026-06-09 R0 audit. C4 RETAINED pending installed-hook canonical re-validation. | RECONFIRMED C4 (R1-B 2026-06-09): subset-install canonical boot-to-menu observe ÔÇö installed 0xE9 live + manifest installed=1 + 25s survival, no crash; log/install_observe_r1b_20260609.txt"
```

Proposed: change `status` `new`ÔåÆ`impl`, set `frida_diff` to `log/install_observe_r1b_20260609.txt`. Notes column unchanged.
*Rationale: T1.2 + T1.3 ÔÇö identical pattern to TXN-15.*

---

**TXN-17** | `hooks.csv` | replace | `00431b00` ÔÇö C4/new/empty-frida_diff (T1.2 + T1.3)

Current (line 1772):
```
00431b00,FUN_00431b00,boot,C4,new,re/analysis/promote_c2_boot_lowrva/0x00431b00.md,,,"promoted_from=re/analysis/boot_subsystem_d3/0x00431b00.md; batch_u_s5_2026-05-18; DAT_007f0f00=0x3f333333 (0.7f); 10b setter; paired with 00431ae0/af0/b10 | C2->C3 via mass-canonical-wave2 (count=1 at boot-to-menu) | C3->C4 2026-05-23 c4-lift: canonical-scenario observation evidence with hook installed (fired during boot-to-menu or per-frame survival) ÔÇö meets C4 rubric per CONFIDENCE.md + feedback_no_overclaiming_c_levels | C4-EVIDENCE-RECONFIRMED(R1-B 2026-06-09, was suspect): canonical evidence in broken-loader window (2026-05-15..24); escaped 6e359fe8 tagging; tagged 2026-06-09 R0 audit. C4 RETAINED pending installed-hook canonical re-validation. | RECONFIRMED C4 (R1-B 2026-06-09): subset-install canonical boot-to-menu observe ÔÇö installed 0xE9 live + manifest installed=1 + 25s survival, no crash; log/install_observe_r1b_20260609.txt"
```

Proposed: change `status` `new`ÔåÆ`impl`, set `frida_diff` to `log/install_observe_r1b_20260609.txt`.
*Rationale: T1.2 + T1.3.*

---

**TXN-18** | `hooks.csv` | replace | `0046bce0` ÔÇö C4/mapped/empty-frida_diff (T1.2 + T1.3)

Current (line 4124):
```
0046bce0,FUN_0046bce0,gameplay,C4,mapped,re/analysis/bucket_gameplay_0045dff0_0046dd90/0x0046bce0.md,batch-y-s2,,"batch_y s2 MIXED gameplay: vehicle-physics-core (0x00466100..0x0046dd90) + scripted-entity-bytecode (0x00471430..0x00474890) + scenery-actor-pool + WorldDestroy (0x0047b480..0x0047fc33) | C1->C2 batch_an sweep-20260603-1259 | C2->C3 c3_batch_ad (Opus harvest, author+verify, central classify): Frida bit-identical GREEN run_diff_warm + integration build (12/12)"
```

Proposed: change `status` `mapped`ÔåÆ`analyzed`; set `frida_diff` to `pending`. Notes unchanged.
*Rationale: T1.2 + T1.3; analysis note in `file` field confirms C2/C3 work but no .cpp reimplementation file.*

---

**TXN-19** | `hooks.csv` | replace | `00404320` ÔÇö C4/impl/empty-frida_diff (T1.3 only)

Current (line 562, abbreviated ÔÇö notes field is very long):
```
00404320,PerModeRenderMachine,render,C4,impl,mashedmod/src/mashed_re/Render/PerModeRender.cpp,hot-path-behavioral-observation,,"hot-path-behavioral 2026-05-23; per-mode render dispatch 808B modes 5/8/9/10; [... full notes ...]"
```

Proposed: set `frida_diff` to `na` (hot-path behavioral observation cannot use Frida Interceptor; the canonical-scenario survival was the evidence form used). All other fields unchanged.
*Rationale: T1.3; `frida_diff` blank for a C4 row; hot-path behavioral methods use `na` rather than a log path.*

---

**TXN-20** | `hooks.csv` | replace | `00492770` ÔÇö C4/impl/empty-frida_diff (T1.3 only)

Current (line 1768, abbreviated):
```
00492770,MainLoopInit,boot,C4,impl,mashedmod/src/mashed_re/Boot/FrameDispatch.cpp,hot-path-behavioral-observation,,"hot-path-behavioral 2026-05-23; boot-once init; global writes DAT_00828300/007f1000/101c/1020/00771968; GREEN 30s survival run all-4-hooks; [... full notes including RECONFIRMED C4 R1-B ...]"
```

Proposed: set `frida_diff` to `na`. All other fields unchanged.
*Rationale: T1.3; same hot-path behavioral pattern as TXN-19.*

---

**TXN-21** | `hooks.csv` | replace | `004c5800` ÔÇö C4/new/empty-frida_diff (T1.2 + T1.3)

Current (line 1557):
```
004c5800,FUN_004c5800,render,C4,new,re/analysis/texture_loader_d3_cont1/0x004c5800.md,texture_loader_d3_cont1,,"RwTexDictionarySetCurrent: writes param_1 to DAT_007d4054+0x1c+DAT_007d3ff8; returns 1; D-7900 | C2->C3 via mass-canonical-wave2 (count=2) | C3->C4 2026-05-23 c4-lift: canonical-scenario observation evidence with hook installed (fired during boot-to-menu or per-frame survival) ÔÇö meets C4 rubric per CONFIDENCE.md + feedback_no_overclaiming_c_levels | C4-EVIDENCE-RECONFIRMED(R1-B 2026-06-09, was suspect): canonical evidence in broken-loader window (2026-05-15..24); escaped 6e359fe8 tagging; tagged 2026-06-09 R0 audit. C4 RETAINED pending installed-hook canonical re-validation. | RECONFIRMED C4 (R1-B 2026-06-09): subset-install canonical boot-to-menu observe ÔÇö installed 0xE9 live + manifest installed=1 + 25s survival, no crash; log/install_observe_r1b_20260609.txt"
```

Proposed: change `status` `new`ÔåÆ`impl`; set `frida_diff` to `log/install_observe_r1b_20260609.txt`. Notes unchanged.
*Rationale: T1.2 + T1.3.*

---

**TXN-22** | `hooks.csv` | replace | `004c5820` ÔÇö C4/new/empty-frida_diff (T1.2 + T1.3)

Current (line 1558):
```
004c5820,FUN_004c5820,render,C4,new,re/analysis/texture_loader_d3_cont1/0x004c5820.md,texture_loader_d3_cont1,,"RwTexDictionaryGetCurrent: reads DAT_007d4054+0x1c+DAT_007d3ff8; 15 bytes; D-7901 | C2->C3 via mass-canonical-wave2 (count=1) | C3->C4 2026-05-23 c4-lift: canonical-scenario observation evidence with hook installed (fired during boot-to-menu or per-frame survival) ÔÇö meets C4 rubric per CONFIDENCE.md + feedback_no_overclaiming_c_levels | C4-EVIDENCE-RECONFIRMED(R1-B 2026-06-09, was suspect): canonical evidence in broken-loader window (2026-05-15..24); escaped 6e359fe8 tagging; tagged 2026-06-09 R0 audit. C4 RETAINED pending installed-hook canonical re-validation. | RECONFIRMED C4 (R1-B 2026-06-09): subset-install canonical boot-to-menu observe ÔÇö installed 0xE9 live + manifest installed=1 + 25s survival, no crash; log/install_observe_r1b_20260609.txt"
```

Proposed: change `status` `new`ÔåÆ`impl`; set `frida_diff` to `log/install_observe_r1b_20260609.txt`. Notes unchanged.
*Rationale: T1.2 + T1.3.*

---

### TXN-23 ÔÇö T1.4a: hooks.csv header comment ÔÇö vocab drift

Current (hooks.csv line 6):
```
# status: mapped|wip|stubbed|impl|verified|wontfix
```

Replace with:
```
# status: mapped|unmapped|disassembled|analyzed|new|ported|impl|verified|hooked|stub|deferred (actual observed vocab ÔÇö wip/wontfix retired, disasm=disassembled synonym)
```
*Rationale: T1.4; header listed retired/wrong vocab (`wip`, `wontfix`); actual status set is larger.*

---

### TXN-24 ÔÇö T1.4b: hooks.csv vocab normalization ÔÇö `disasm`ÔåÆ`disassembled`

**Scope:** 15 rows in hooks.csv with `status=disasm` should have `status=disassembled`. This is a bulk sed-style replacement: `^([^,]+,[^,]+,[^,]+,[^,]+,)disasm,` ÔåÆ `\1disassembled,`. No content verification of individual lines is required because the transformation is column-exact and `disasm` only appears in the status column (verified by pattern). A grep for `,disasm,` across hooks.csv will enumerate all 15 affected RVAs.

*Rationale: T1.4; `disasm` and `disassembled` are the same status; normalize to the longer canonical form.*

---

### TXN-25 ÔÇö T1.4c: hooks.csv vocab normalization ÔÇö `stub`ÔåÆ`stubbed`

**Scope:** 2 rows with `status=stub` should have `status=stubbed`. Same bulk pattern: `,stub,` ÔåÆ `,stubbed,` in the status column only.

*Rationale: T1.4.*

---

### TXN-26 ÔÇö T1.5a: hooks.csv dead `file` paths ÔÇö 31 qhull rows

**File confirmed non-existent:** `re/analysis/library_residue/qhull.md` (Glob returned no results).

**Affected rows:** hooks.csv lines 2569ÔÇô2599 (31 rows), all with pattern:
```
005aXXXX,<name>,third-party-library[qhull-2002.1],C1,mapped,re/analysis/library_residue/qhull.md,library-drain-qhull,,library_residue qhull-2002.1 (Ghidra-FidDB-attested name); static-linked third-party; sibling of s5 bucket_00583f10
```

First row (line 2569):
```
005a0e00,qh_printcenter,third-party-library[qhull-2002.1],C1,mapped,re/analysis/library_residue/qhull.md,library-drain-qhull,,library_residue qhull-2002.1 (Ghidra-FidDB-attested name); static-linked third-party; sibling of s5 bucket_00583f10
```
Last row (line 2599):
```
005a4ed0,qh_printvertex,third-party-library[qhull-2002.1],C1,mapped,re/analysis/library_residue/qhull.md,library-drain-qhull,,library_residue qhull-2002.1 (Ghidra-FidDB-attested name); static-linked third-party; sibling of s5 bucket_00583f10
```

Proposed action for all 31 rows: clear the `file` column (set to empty string). Pattern replacement:
```
,re/analysis/library_residue/qhull.md,  ÔåÆ  ,,
```
*(Only applies to the 31 rows in the `third-party-library[qhull-2002.1]` subsystem block ÔÇö the pattern is specific enough to be safe.)*

*Rationale: T1.5; note file does not exist; these are library-band C1/mapped rows that do not need a per-function analysis note.*

---

### TXN-27 ÔÇö T1.5b: hooks.csv dead `file` path ÔÇö `Util/StateAccessors.cpp`

**File confirmed non-existent:** `mashedmod/src/mashed_re/Util/StateAccessors.cpp` (Glob returned no results). Sibling rows (lines 419ÔÇô421) use `mashedmod/src/mashed_re/GameState/StateAccessors.cpp`.

Current (hooks.csv line 329):
```
0042b8d0,StatePhaseIsIdle,util,C4,impl,mashedmod/src/mashed_re/Util/StateAccessors.cpp,boot_to_menu_install_observe_2026-06-06,log/c4_verify_result.json,pure-leaf predicate `return DAT_0067eca4==0`; 13 bytes; 10/10 bit-identical Frida force-call A/B via sentinel-write (sentinels exercise both branches); pure-leaf exemption; caller FUN_00492e90 C2; direct callee of 0x00492e90; U-0500 still open (global semantics); analysis re/analysis/game_state/0x0042b8d0.md; ma2-frida-s5 2026-05-16 | C3->C4 (canonical install-observe 2026-06-06): inline-JMP LIVE (0xE9) + exercised 2755x during boot-to-menu (canonical_c4_verify OFF-count) + booted to menu & survived with hook installed + C3 0-arg getter full-domain GREEN.
```

Proposed replacement ÔÇö update `file` from `Util/StateAccessors.cpp` to `GameState/StateAccessors.cpp` (matching sibling rows):
```
0042b8d0,StatePhaseIsIdle,util,C4,impl,mashedmod/src/mashed_re/GameState/StateAccessors.cpp,boot_to_menu_install_observe_2026-06-06,log/c4_verify_result.json,pure-leaf predicate `return DAT_0067eca4==0`; 13 bytes; 10/10 bit-identical Frida force-call A/B via sentinel-write (sentinels exercise both branches); pure-leaf exemption; caller FUN_00492e90 C2; direct callee of 0x00492e90; U-0500 still open (global semantics); analysis re/analysis/game_state/0x0042b8d0.md; ma2-frida-s5 2026-05-16 | C3->C4 (canonical install-observe 2026-06-06): inline-JMP LIVE (0xE9) + exercised 2755x during boot-to-menu (canonical_c4_verify OFF-count) + booted to menu & survived with hook installed + C3 0-arg getter full-domain GREEN.
```
*Rationale: T1.5; TU was moved from Util/ to GameState/; verify that `GameState/StateAccessors.cpp` exists before applying (account3 can Glob to confirm).*

---

## Transactions ÔÇö STUBS.md

### TXN-28 ÔÇö T1.6: Renumber ID-collision rows (7 collision groups)

All rows below are in the **Active stubs** section of STUBS.md (before line 1149 `## Resolved stubs`). Each collision group: one row keeps the original ID, duplicate rows get new IDs. Assigned new IDs start at S-5532 (above current max observed S-5531); use consecutive integers per collision.

> **Rule:** do not renumber rows in the Resolved section ÔÇö they may share IDs by design per the health-doc method note.

---

**S-0340 collision (2 rows, lines 219 and 287):**

Keep (line 219, no change):
```
| S-0340 | 0x00485e10 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_00485e10; no args; depth-3 |
```

Renumber (line 287) ÔÇö additionally note the RESOLVED inline note suggests this row should also move to Resolved (see TXN-35 candidate):
```
Current:  | S-0340 | 0x005a9e10 FUN_005a9e10 | 0x005b9f30 LAB_005b9f30 | audio | passthrough | 2026-05-02 | two-call dispatcher; depth-1 callee of FUN_005b9f30; RESOLVED: analyzed in audio_dsound-20260502-1942 |
Replace:  | S-5532 | 0x005a9e10 FUN_005a9e10 | 0x005b9f30 LAB_005b9f30 | audio | passthrough | 2026-05-02 | two-call dispatcher; depth-1 callee of FUN_005b9f30; RESOLVED: analyzed in audio_dsound-20260502-1942 |
```

---

**S-0341 collision (2 rows, lines 220 and 288):**

Keep (line 220, no change):
```
| S-0341 | 0x00486350 | 0x0040bd00 FUN_0040bd00 | boot | passthrough | 2026-05-02 | FUN_00486350; no args; depth-3 |
```

Renumber (line 288):
```
Current:  | S-0341 | 0x005aee20 FUN_005aee20 | 0x005b9f30 LAB_005b9f30 | audio | passthrough | 2026-05-02 | bit-scan-forward loop; depth-1 callee of FUN_005b9f30; RESOLVED: analyzed in audio_dsound-20260502-1942 |
Replace:  | S-5533 | 0x005aee20 FUN_005aee20 | 0x005b9f30 LAB_005b9f30 | audio | passthrough | 2026-05-02 | bit-scan-forward loop; depth-1 callee of FUN_005b9f30; RESOLVED: analyzed in audio_dsound-20260502-1942 |
```

---

**S-1441 collision (2 rows, lines 437 and 438):**

Keep (line 437, no change):
```
| S-1441 | 0x004547c0 FUN_004547c0 | 0x004548a0 FUN_004548a0 | vehicle | passthrough | 2026-05-03 | per-entry activator for DepthCharge struct-A (stride 0x2c, 0x00688240..0x006882f0); ESI-implicit; D-4240 |
```

Renumber (line 438):
```
Current:  | S-1441 | 0x00454170 FUN_00454170 | 0x004548a0 FUN_004548a0 | vehicle | passthrough | 2026-05-03 | per-entry activator for DepthCharge struct-B (stride 0x44, 0x00688020..0x00688240); ESI-implicit; D-4241 |
Replace:  | S-5534 | 0x00454170 FUN_00454170 | 0x004548a0 FUN_004548a0 | vehicle | passthrough | 2026-05-03 | per-entry activator for DepthCharge struct-B (stride 0x44, 0x00688020..0x00688240); ESI-implicit; D-4241 |
```

---

**S-1480 collision (5 rows, lines 406ÔÇô410):**

Keep (line 406, no change):
```
| S-1480 | 0x005c4d30 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | glyph-data block accessor; depth-2 font_text_d2 |
```

Renumber rows 2ÔÇô5:
```
Line 407 current:  | S-1480 | 0x00552d10 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | render-matrix setup before Im2D quad emission |
Line 407 replace:  | S-5535 | 0x00552d10 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | render-matrix setup before Im2D quad emission |

Line 408 current:  | S-1480 | 0x00552df0 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | set draw position (called twice: xy and z) |
Line 408 replace:  | S-5536 | 0x00552df0 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | set draw position (called twice: xy and z) |

Line 409 current:  | S-1480 | 0x00552da0 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | set draw color/alpha param |
Line 409 replace:  | S-5537 | 0x00552da0 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | set draw color/alpha param |

Line 410 current:  | S-1480 | 0x00552e40 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | begin-render / Im2D batch flush |
Line 410 replace:  | S-5538 | 0x00552e40 | 0x00554940 LAB_00554940 | hud | passthrough | 2026-05-03 | begin-render / Im2D batch flush |
```

---

**S-1481 collision (3 rows, lines 411ÔÇô413):**

Keep (line 411, no change):
```
| S-1481 | 0x005551d0 | 0x00555910 LAB_00555910 | hud | passthrough | 2026-05-03 | entry guard / current-name getter; U-1490; D-4362 |
```

Renumber rows 2ÔÇô3:
```
Line 412 current:  | S-1481 | 0x00550a20 | 0x00555910 LAB_00555910 | hud | passthrough | 2026-05-03 | VFS read-line (buf, size, filehandle) |
Line 412 replace:  | S-5539 | 0x00550a20 | 0x00555910 LAB_00555910 | hud | passthrough | 2026-05-03 | VFS read-line (buf, size, filehandle) |

Line 413 current:  | S-1481 | 0x00550580 | 0x005507b0 FUN_005507b0 | hud | passthrough | 2026-05-03 | VFS file-open implementation; D-4365 |
Line 413 replace:  | S-5540 | 0x00550580 | 0x005507b0 FUN_005507b0 | hud | passthrough | 2026-05-03 | VFS file-open implementation; D-4365 |
```

---

**S-1482 collision (3 rows, lines 414ÔÇô416):**

Keep (line 414, no change):
```
| S-1482 | 0x00553f40 | 0x005540d0 FUN_005540d0 | hud | passthrough | 2026-05-03 | glyph-data upload/lock; recursive on node[5] |
```

Renumber rows 2ÔÇô3:
```
Line 415 current:  | S-1482 | 0x005c4c60 | 0x00553f40 FUN_00553f40 | hud | passthrough | 2026-05-03 | resize/alloc cnt├ù0x18 Im2D vertex buffer |
Line 415 replace:  | S-5541 | 0x005c4c60 | 0x00553f40 FUN_00553f40 | hud | passthrough | 2026-05-03 | resize/alloc cnt├ù0x18 Im2D vertex buffer |

Line 416 current:  | S-1482 | 0x0055deb0 | 0x00553f40 FUN_00553f40 | hud | passthrough | 2026-05-03 | get vertex count from glyph-data block |
Line 416 replace:  | S-5542 | 0x0055deb0 | 0x00553f40 FUN_00553f40 | hud | passthrough | 2026-05-03 | get vertex count from glyph-data block |
```

---

**S-1485 collision (2 rows, lines 418ÔÇô419):**

Keep (line 418, no change):
```
| S-1485 | 0x00556e90 | 0x00556d70 FUN_00556d70 | hud | passthrough | 2026-05-03 | set font-style RGBA color |
```

Renumber (line 419):
```
Current:  | S-1485 | 0x00557110 | 0x00556d70 FUN_00556d70 | hud | passthrough | 2026-05-03 | set font-style secondary param to zero |
Replace:  | S-5543 | 0x00557110 | 0x00556d70 FUN_00556d70 | hud | passthrough | 2026-05-03 | set font-style secondary param to zero |
```

*Rationale: T1.6; 7 collision groups ÔåÆ 12 rows renumbered (S-5532 through S-5543); IDs chosen above current max S-5531.*

---

### TXN-29 ÔÇö T1.7: Relocate 11 struck rows from Active section to Resolved section

Move each row verbatim (with all `~~` formatting intact) from its current position in the Active section to the top of `## Resolved stubs (audit trail ÔÇö do not delete)` (currently at line 1149). No content modification needed ÔÇö move-only.

**Row 1** (STUBS.md line 325) ÔÇö move to Resolved:
```
| ~~S-0801~~ | ~~0x00493f70~~ | ~~0x00495350 FUN_00495350~~ | frontend | resolved | 2026-07-23 | RESOLVED (account2 rewire, build-verified exit 0): OrigVideoDone trampoline replaced with direct call to ported VideoStateFlagGet (impl/C4) at IntroSplash.cpp:~384. Reads same DAT_00771a04, compared ==0 ÔÇö behavior preserved. Struck outside orchestrator's own promotion sweep (00495350 still C2, Frida-gated). |
```

**Row 2** (STUBS.md line 326) ÔÇö move to Resolved:
```
| ~~S-0802~~ | ~~0x00493f80~~ | ~~0x00495350 FUN_00495350~~ | frontend | resolved | 2026-07-23 | RESOLVED (account2 standalone hygiene): 0x00493f80 IntroVideoDimGetter is impl/C4; IntroSplashOrchestrator calls it directly (IntroSplash.cpp:316,394) ÔÇö no passthrough remains. Struck outside the orchestrator's own promotion sweep (00495350 still C2, Frida-gated). |
```

**Row 3** (STUBS.md line 327) ÔÇö move to Resolved:
```
| ~~S-0803~~ | ~~0x00493fc0~~ | ~~0x00495350 FUN_00495350~~ | frontend | resolved | 2026-07-23 | RESOLVED (account2 rewire, build-verified exit 0): OrigAspectHelper trampoline replaced with direct call to ported AspectRatioGlobalGet (impl/C4) at IntroSplash.cpp:~368. Body ignores the two float args (U-0814) so arg-drop is behavior-preserving; ratio computations kept for decomp fidelity. Struck outside orchestrator's own promotion sweep (00495350 still C2, Frida-gated). |
```

**Row 4** (STUBS.md line 332) ÔÇö move to Resolved:
```
| ~~S-0810~~ | ~~0x004c1a00~~ | ~~0x00495350 FUN_00495350~~ | frontend | resolved | 2026-07-23 | RESOLVED (account2 standalone hygiene): 0x004c1a00 IntroSplashVtableSlot6 is impl/C3; IntroSplashOrchestrator calls it directly (IntroSplash.cpp:350) ÔÇö no passthrough remains. Struck outside the orchestrator's own promotion sweep (00495350 still C2, Frida-gated). |
```

**Row 5** (STUBS.md line 333) ÔÇö move to Resolved:
```
| ~~S-0811~~ | ~~0x004c1bb0~~ | ~~0x00495350 FUN_00495350~~ | frontend | resolved | 2026-07-23 | RESOLVED (account2 standalone hygiene): 0x004c1bb0 IntroSplashRenderState is impl/C3; IntroSplashOrchestrator calls it directly (IntroSplash.cpp:347) ÔÇö no passthrough remains. Struck outside the orchestrator's own promotion sweep (00495350 still C2, Frida-gated). |
```

**Row 6** (STUBS.md line 417) ÔÇö move to Resolved:
```
| ~~S-1483~~ | ~~0x004c4a50~~ | ~~0x004c4d20 FUN_004c4d20~~ | hud | resolved | 2026-07-24 | RESOLVED (account2 stale-stub strike, no code change): RwMatrixRotate (0x004c4d20, verified/C4) calls the ported RwMatrixRotateInner (0x004c4a50, verified/C4) as a direct C++ symbol at Math/RwMatrixRotate.cpp:79 ÔÇö no reinterpret_cast/RVA trampoline in the file (file's "verbatim from Ghidra" note = transcription fidelity, not inline-JMP routing). hooks.csv 004c4d20 note already records "supersedes prior S-1483 stub status ... called by C++ symbol". Found via account2 Lane-A global stale scan (open-stub ├ù caller-installed ├ù callee-installed). |
```

**Row 7** (STUBS.md line 439) ÔÇö move to Resolved:
```
| ~~S-1442~~ | ~~0x004b64e0 FUN_004b64e0~~ | ~~0x004b6520 FUN_004b6520~~ | vehicle | resolved | 2026-07-24 | RESOLVED (account2 stale-stub strike, no code change): ZeroFillWrapper (0x004b6520, impl/C3) inlines the FUN_004b64e0 zero-fill as std::memset(p1,0,p2) at Util/TimerSlot.cpp:34 ÔÇö the 0x004b64e0 passthrough is gone (callee also separately ported C3, Input/MemsetInline_ag1.cpp). hooks.csv 004b6520 note records "std::memset(p1,0,p2)==orig FUN_004b64e0(p1,0,p2)" (Frida GREEN at promotion); inlined callee counts as resolved. Twin row S-2410 = same call site. Found via account2 Lane-A global stale scan. |
```

**Row 8** (STUBS.md line 562) ÔÇö move to Resolved:
```
| ~~S-2410~~ | ~~0x004b64e0 FUN_004b64e0~~ | ~~0x004b6520 FUN_004b6520~~ | input | resolved | 2026-07-24 | RESOLVED (account2 stale-stub strike, no code change): same 0x004b6520->0x004b64e0 call site as S-1442 ÔÇö ZeroFillWrapper inlines it as std::memset (Util/TimerSlot.cpp:34); no passthrough remains. Found via account2 Lane-A global stale scan. |
```

**Row 9** (STUBS.md line 772) ÔÇö move to Resolved:
```
| ~~S-3653~~ | ~~0x004117b0 FUN_004117b0~~ | ~~0x0040de00~~ | save | resolved | 2026-07-24 | RESOLVED (account2 stale-stub strike, no code change): ThunkReplaySave (0x0040de00, C4) calls the ported ReplaySave (0x004117b0, impl/C3) directly via C++ linkage at Save/ReplayThunk.cpp:46 (extern decl :40) ÔÇö no RVA trampoline. Target-body stubs S-3654/S-3655/S-3656 (inside ReplaySave, Vehicle/Replay.cpp) remain open and are unaffected. Found via account2 Lane-A global stale scan. |
```

**Row 10** (STUBS.md line 918) ÔÇö move to Resolved:
```
| ~~S-3931~~ | ~~0x00473220 FUN_00473220~~ | ~~0x00473ee0 FUN_00473ee0~~ | frontend | resolved | 2026-07-24 | RESOLVED (account2 inlined-callee stale strike, no code change): the 0x00473ee0 reimpl (LogoOverlayTwin ABI adapter -> LogoOverlayDraw, DrawQuadPrimitives.cpp, hooks.csv 00473ee0 = verified/C4) INLINES FUN_00473220's gradient-quad draw (second color on V1/V3) as the shared body at DrawQuadPrimitives.cpp:573-696 (`second_on_v1v3` branch). 0x00473220 appears ONLY in comments ÔÇö no reinterpret_cast/RVA trampoline anywhere in src. C4 gate ("no stubs in implementation") already implies resolved; inlining verified bit-identical via logo_overlay_diff.py Im2D A/B. Found via account2 Lane-A inlined-callee stale scan. |
```

**Row 11** (STUBS.md line 919) ÔÇö move to Resolved:
```
| ~~S-3932~~ | ~~0x004733b0 FUN_004733b0~~ | ~~0x00473ee0 FUN_00473ee0~~ | frontend | resolved | 2026-07-24 | RESOLVED (account2 inlined-callee stale strike, no code change): same C4-verified LogoOverlayDraw body inlines FUN_004733b0's gradient-quad draw (second color on V0/V2) at DrawQuadPrimitives.cpp:573-696 (`else` branch, DAT_00898a30/_DAT_00898a68). 0x004733b0 appears ONLY in comments ÔÇö no trampoline in src. Twin of S-3931 (same 0x00473ee0 body). Found via account2 Lane-A inlined-callee stale scan. |
```

*Rationale: T1.7; all 11 rows are ~~struck~~ in the Active section but belong in Resolved. After moving them, the census count changes by ÔêÆ11 open / +11 audit-trail.*

---

### TXN-30 ÔÇö T1.7 follow-up: Update STUBS.md census header line

Current census line (STUBS.md line 14):
```
**1,109 open rows / 147 struck.** (2026-07-24 (2nd strike, inlined-callee scan): 2 more ÔåÆ
```

After TXN-29 moves 11 struck rows out of Active: the active-open count drops by 11 (those rows were being counted as open because they lacked `~~` in the census-script sense ÔÇö see health doc ┬ºT1.7 note on census mismatch). Corrected count: **1,098 open / 158 struck** *(or recompute with script ÔÇö this arithmetic assumes none of the 11 moved rows were being included in the `147 struck` tally because they were in the wrong section).*

**Note for account3:** Rather than hard-coding the corrected number here, re-run `tracker_health.py` or a section-aware census after applying TXN-29 to get the exact figure. The current discrepancy documented in T1.7 is that the census header counts the whole file, while the section-aware truth is 1,088 active-open / 173 audit-trail ÔÇö a gap of 21 rows. TXN-29 moves 11 of them; run the census again for the remaining 10 if any.

---

## CONFLICTS

**No CONFLICTS detected.** Every line quoted above was verified by direct Read/Grep of the live file at analysis time. The git status at session start was clean (no uncommitted changes). The only caveat: concurrent account3 sessions may have committed between analysis and application ÔÇö the account3 operator should re-grep each RVA before applying to confirm the line still matches verbatim.

**Specific items to re-verify before applying:**
- TXN-27: Glob `mashedmod/src/mashed_re/GameState/StateAccessors.cpp` before applying ÔÇö if that path also doesn't exist, clear the `file` field entirely rather than changing the path.
- TXN-28 (S-0340 row at line 287, S-0341 row at line 288): these also carry "RESOLVED" notes in the notes field ÔÇö consider whether they should be moved to Resolved (TXN-29 equivalent) rather than just renumbered. Left as renumber-only here because they lack `~~` formatting and T1.7's criterion was specifically struck-formatted rows.
- TXN-30: census arithmetic is approximate; recompute after TXN-29 is applied.

---

## Apply-order note

**Must go through `re-classify`:** TXN-14 through TXN-22 (T1.2/T1.3 C4 confidence corrections) and TXN-29/TXN-30 (STUBS.md struck-row relocation + census update) are substantive tracker state mutations that re-classify validates. Run re-classify for each affected RVA in TXN-14ÔÇô22; use the STUBS.md section-move path in re-classify for TXN-29.

**Pure mechanical text edits (cheaper model, no re-classify gate):** TXN-01ÔÇô13 (hooks.csv de-dup), TXN-23ÔÇô25 (vocab header + normalization), TXN-26ÔÇô27 (dead file paths ÔåÆ blank/repoint), TXN-28 (STUBS.md ID renumber). These are column-value substitutions with no confidence logic ÔÇö a script or cheap-model editor can apply them directly. Recommended order: TXN-28 first (renumber IDs before any row moves to avoid ID confusion in Resolved), then TXN-29, then hooks.csv transactions. TXN-30 (census) must be last.
