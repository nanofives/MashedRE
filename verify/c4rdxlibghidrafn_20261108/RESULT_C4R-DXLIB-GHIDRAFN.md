# RESULT C4R-DXLIB-GHIDRAFN + C4R-DSHOW-NEWROWS (2026-11-08)

**Verdict: GREEN.** 129 functions created in the master Ghidra project (47 d3dx9, 82 DirectShow BaseClasses),
`re/console/pc_funcs.csv` regenerated (+129 entries, nothing else changed), 129 hooks.csv rows added (47 + 82) and all
129 promoted to C4 by route `libid` (L-93). 9 addresses of the 138 given were refused, each with a reason below. No
function was split. 23 d3dx9 rows wait for a sub-tag decision and were not added. Text, tracker and master Ghidra only:
no build, no game run, no `mashedmod/` change.

Branch `orch/C4R-DXLIB-GHIDRAFN` from `orch/integration` @ 9d1034b27, with `orch/C4R-DSHOW-NEWROWS` @ 2c95a5508 merged
(`--no-ff`, autoStash off) for `verify/c4rdshownewrows_20261108/newrows.py`.

## 1. Address list and byte check (`addrs.py`, `addrs.tsv`, `addrs_output.txt`)

Inputs: the C4R-DXLIB-FU refusals in `verify/c4rdxlibfu_20261107/candidates.tsv` with reason `in no pc_funcs body` and
one library function (55), and the C4R-DSHOW-NEWROWS refusals in `verify/c4rdshownewrows_20261108/candidates.tsv`
with reason only `no pc_funcs boundary at rva` (83). The bytes were checked again rather than taken from the old TSVs:
`libmatch.compare_at` against `tools/dxsdk9/sum2003/Lib/d3dx9.lib` (228 objects, 6025 functions) and against a fresh
build of the DX 9.0b BaseClasses source with the rel_ob1 flags `/Gz /MT /GX /O2 /Ob1` (29 objects, 1569 functions,
the C4R-DSHOW-BASECLASSES recipe), image `original/MASHED.exe.unpatched` (anchor checked by `coffpe.Image`). Per
address: identical to the named symbol, the count of library functions identical there, and no pc_funcs body overlapping
`[va, va+size)`.

134 passed. Refused at this step (4, all d3dx9):

| va | symbol | reason |
|---|---|---|
| 0x004eed25 | `init_D3DXQuaternionBaryCentric` | 4 library functions identical at the address |
| 0x005139a0 | `_a_tan` | 3 library functions identical at the address |
| 0x00513780 | `exception` (amdmath.obj label) | bytes differ at +0 |
| 0x00513a9a | `ending` (amdmath.obj label) | bytes differ at +0 |

## 2. Master Ghidra write (`re/tools/ghidra_scripts/CreateLibFunctions.java`)

Same guard family as `CreateMissedFunctions.java` (the 2026-09-11 501-function write), extended to the whole symbol
extent: refuse on non-executable memory, any function overlapping `[va, va+size)`, defined data in it, or `va` inside
an existing instruction. Then disassemble, `createFunction(va)` (body by flow, default `FUN_<va>` name), and compare
the body with the extent: `CREATED_EXACT` when equal, `CREATED_PAD` when the body stops early and every remaining extent
byte is alignment fill (`0x90` / `0xCC` / `mov r,r` / `lea r,[r+0]`), else `REJECT_LONG` / `REJECT_SHORT` and the function is
removed again.

Sequence (multi-session rules): no `java.exe` running, no lock file in `Mashed.rep`, only this worker writes master
(parent brief). Backup `Mashed.rep.bak-20261108-pre-libfn` + `Mashed.gpr.bak-20261108-pre-libfn` (gitignored by
`/Mashed.rep.bak-*/`). Rehearsal: the full apply on master under `analyzeHeadless -readOnly -noanalysis`, so every change
was discarded (`rehearsal_readonly.tsv`). The first rehearsal accepted only `0x90`/`0xCC` as fill and rejected the 11
amdmath bodies whose tail is MASM `align` fill (`8D A4 24 00 00 00 00` = `lea esp,[esp+0]`, `8B FF` = `mov edi,edi`).
The existing amdmath rows follow the same convention (`0x005130c0` `_a_atan2`: pc_funcs body 245 B, symbol 256 B,
already C4 by libid), so the fill rule was widened to those no-op forms and the rehearsal re-run. The write used
only the 129 rehearsal-created addresses (`apply_list.tsv`), `analyzeHeadless ... -noanalysis` without `-readOnly`:
`ok=129 skipped=0 rejected=0`, `Save succeeded` (`apply_master.tsv`, outcomes identical to the rehearsal).
After the write, no lock file and no Java process remained.

Rejected in the rehearsal and not written (5):

| va | symbol | flow body | reason |
|---|---|---|---|
| 0x004a1a10 | `CPosPassThru::NonDelegatingQueryInterface` (ctlutil.obj, 80 B) | 136 B | ends `jmp 0x004a18f0` (`0x004a1a5b`), and 0x004a18f0 has no function: it is one of the 11 ambiguous BaseClasses addresses (7 candidates, all 56 B). With a function at 0x004a18f0 the jmp would be a tail call |
| 0x005136c0 | `_a_ldexp` (64 B) | 74 B | flow continues into 0x00513700 (label `exception`) |
| 0x00513700 | `exception` (28 B) | 30 B | flow leaves the extent at 0x0051371c |
| 0x00513740 | `_a_fmod` (64 B) | 58 B | flow continues into 0x00513780 (label `exception`, bytes differ) |
| 0x00513a20 | `x5` (116 B) | 145 B | flow leaves the extent at 0x00513a94 |

The amdmath ones are MASM routines with shared tails. A function per symbol does not fit their flow, so none was forced.

Created: 118 `CREATED_EXACT` (36 d3dx9 + 82 BaseClasses) and 11 `CREATED_PAD` (amdmath `_a_atan`, `_a_asin`, `_a_log`,
`_a_log10`, `_a_exp`, `_a_sqrt`, `_a_fabs`, `_a_ceil`, `_a_floor`, `_a_frexp`, `_a_modf`; fill 2 to 30 B). Per-address
table at the end.

**Pool clones were not synced** (the brief forbids pool-slot writes). Slots `mashed_pool/Mashed_poolN` still lack these
129 functions until someone allowed to write pool slots runs `scripts/ghidra_pool.sh sync`.

## 3. Labels inside another pc_funcs body: no split (`inside.tsv`)

All 39 d3dx9 labels that sit inside a pc_funcs body (the 17 D rows of C4R-DXLIB-FU plus the 12 `d3dxvec2.obj` and
10 other MASM labels of the same routines) were tested with capstone over the outer body: the outer function splits only
if the last real instruction before the label is `ret`/`jmp` (alignment fill skipped) AND no branch in
`[entry, label)` targets `[label, end)`. **0 of 39 pass.** Every outer body branches into its label (e.g.
`FUN_00508cc0`: `0x00508d37 jmp 0x508d00` into `ScalarLoop`, and `0x00508ecc jmp 0x508dfd`), so each is one assembled
routine. No function was split.

## 4. pc_funcs.csv regeneration (`tools/regen_pcfuncs.py`, `regen_output.txt`, `pcfuncs_added.csv`)

MATCH-PCFUNCS-REGEN recipe (`DumpFunctions.java`, unchanged generator, + the 14 retained stale rows of
`verify/matchpcfuncsregen_20261107/retained_stale_rows.csv`), with one change: the dump comes from master opened
`-readOnly`, not from a pool clone, because the new functions exist only in master (no pool sync allowed).

Control: the dump of master BEFORE the write has sha256 `70630b484652365c1eedc081f837e92f698314e70049ce44a2b2c87885e1c541`,
the same hash MATCH-PCFUNCS-REGEN recorded for its `Mashed_pool0` dump. It equals the committed pc_funcs.csv minus
exactly the 14 retained rows. After the write: 6340 -> 6469 functions, dump sha256
`f032eb0298353a397de3d78cbcb2d6393773092054c18453b7529f4595402043`, identical to the read-only rehearsal dump.

pc_funcs.csv 6354 -> 6483 rows: **added 129, removed 0, changed 0**, 14 stale rows kept. The 129 added entries are
exactly `apply_list.tsv`. Header and LF line endings unchanged. Committed as 4fe26f2cd.

## 5. Rows added and promoted

**d3dx9 (`rows_d3dx9.py`, `candidates_d3dx9.tsv`):** the C4R-DXLIB-FU `newrows.py` checks and `apply.py` row format,
re-run over all 118 `identical-unlisted` d3dx9 addresses without a row, with two extensions, both stated in the script:
(a) a pc_funcs body shorter than the symbol passes only when the bytes past it are alignment fill (same rule as section 2,
amdmath precedent above); (b) two name rules: plain cdecl `_a_atan` -> `a_atan`, and a two-scope symbol
`?Init@CBuffer@D3DXCore@@...` -> `D3DXCore::CBuffer::Init` (MSVC lists the innermost scope first). The FU precedent
was `?compare@D3DXShader@@` -> `D3DXShader::compare`. **47 added:** 31 `[d3dx9-psgp]`, 4 `[d3dx9-shader-compiler]`
(`D3DXCore::CBuffer::Init`, `D3DXCore::CBuffer::Release`, `D3DXShader::CNodeToken::IsEqual`,
`D3DXShader::CNodeType::IsEqual`), and 12 `[d3dx9-backward-extension]` (the amdmath objects: 11 bodies followed by fill, plus
`_a_pow` exact). Sub-tag rule unchanged: the one tag of the object's `identical` d3dx9 rows (`amdmath.obj` -> 4 rows,
all `[d3dx9-backward-extension]`, `cbuffer.obj` 3 and `cnode.obj` 8, all `[d3dx9-shader-compiler]`).

**BaseClasses (`verify/c4rdshownewrows_20261108/newrows.py --apply`, unchanged):** 82 added
(`third-party-library[directshow-baseclasses]`), 14 skipped: 0x004a1a10 no boundary (section 2), 11 ambiguous, 2 rows
present. `newrows.py` writes its outputs into its own folder. Those files were moved here (`candidates_dshow.tsv`,
`added_dshow.tsv`) and the C4R-DSHOW-NEWROWS `candidates.tsv` restored, so that RESULT still matches its evidence.

Rows: confidence C1, status `new`, port `none`, notes = identity + boundary (`function created in master
C4R-DXLIB-GHIDRAFN 2026-11-08`). Line-based append to hooks.csv, no csv round trip. CHANGELOG lines below
`<!-- ENTRIES -->`.

**Promoted:** `c4_reclass.py batch --tsv added_batch.tsv` (129 rows, route libid, evidence
`verify/c4rdxlib_20261107/d3dx_sum2003.tsv` / `verify/c4rdshowbase_20261107/rel_ob1.tsv`): dry run **129 PASS / 0 REFUSED**
(`dryrun_output.txt`, `dryrun_verdicts.tsv`), then `--apply --result <this file> --date 2026-11-08`: 129 rows -> C4,
129 CHANGELOG lines (`c4_apply_output.txt`).

**Not added, waiting on a user decision (23 d3dx9 rows, table at the end):** `d3dxmathx86.obj` maps to both
`[d3dx9-psgp]` and `[d3dx9-shader-compiler]` (5: 0x004fc6f5, 0x004fc75e, 0x004fc793, 0x004fc81e, 0x004fc87d). No d3dx9
row is tagged from `x3d_vec4.obj` (6: 0x0050fc7c..0x0050ffc8) or from `d3dxvec2.obj` (12). These 11 (5 + 6) have a pc_funcs boundary and would pass every
other check. The 12 `d3dxvec2.obj` rows are MASM labels inside `FUN_00508cc0` / `FUN_00508f20` / `FUN_00509180`
(section 3: no split). They would stay unaddable even with a tag.

## 6. Counts

`tracker_lint.py --repo . --base orch/integration`: before `hooks.csv 6015 rows ... pre-existing=3 NEW=0`, after
`hooks.csv 6144 rows ... pre-existing=3 NEW=0` (`lint_before.txt`, `lint_after.txt`).

`ladder_counts.py` (`ladder_before.json`, `ladder_after.json`):

| | before | after |
|---|---|---|
| overall | C1 10, C2 1868, C3 167, **C4 3970**, other 32 (6047) | C1 10, C2 1868, C3 167, **C4 4099**, other 32 (6176) |
| game | C2 718, C3 103, C4 2547, other 32 | unchanged |
| library | C1 10, C2 1150, C3 64, **C4 1423** | C1 10, C2 1150, C3 64, **C4 1552** |

Per tag C4: `[d3dx9-psgp]` 193 -> 224, `[d3dx9-shader-compiler]` 70 -> 74, `[d3dx9-backward-extension]` 6 -> 18,
`[directshow-baseclasses]` 140 -> 222.

`git diff --numstat` of the tracker commit: hooks.csv +129 / -0 (appended rows only), `re/analysis/CHANGELOG.md`
+258 / -0 (129 add + 129 C4 lines). Port column: the added rows carry `port none` (library rows, no `mashedmod/` change),
`backfill_port.py` not run. Gates: not run (text-only).

Follow-ups, not done here: (1) the 11 ambiguous BaseClasses addresses have byte-proven extents where all candidates share a size
(e.g. 0x004a18f0, 7 x 56 B). A function there is a boundary fact, not a naming one, and it would make 0x004a1a10
exact. That needs a brief that allows ambiguous-identity creation. (2) Pool sync. (3) The 23-row sub-tag decision.

## Per-address table (138 addresses)

| va | lib | symbol (object) | sym size | function | Ghidra body | row | C4 |
|---|---|---|---|---|---|---|---|
| 0x0049c790 | dshow | `?GetSampleTimes@CBaseRenderer@@UAEJPAUIMediaSample@@PA_J1@Z` (renbase.obj) | 92 | created exact | 92 B | added | C4 |
| 0x0049c8c0 | dshow | `?Render@CBaseRenderer@@UAEJPAUIMediaSample@@@Z` (renbase.obj) | 63 | created exact | 63 B | added | C4 |
| 0x0049ced0 | dshow | `?CompleteConnect@CRendererInputPin@@UAEJPAUIPin@@@Z` (renbase.obj) | 40 | created exact | 40 B | added | C4 |
| 0x0049cf80 | dshow | `?SetMediaType@CRendererInputPin@@UAEJPBVCMediaType@@@Z` (renbase.obj) | 38 | created exact | 38 B | added | C4 |
| 0x0049d0c0 | dshow | `?RecordFrameLateness@CBaseVideoRenderer@@UAEXHH@Z` (renbase.obj) | 268 | created exact | 268 B | added | C4 |
| 0x0049d2d0 | dshow | `?SetSink@CBaseVideoRenderer@@UAGJPAUIQualityControl@@@Z` (renbase.obj) | 16 | created exact | 16 B | added | C4 |
| 0x0049d2e0 | dshow | `?Notify@CBaseVideoRenderer@@UAGJPAUIBaseFilter@@UtagQuality@` (renbase.obj) | 58 | created exact | 58 B | added | C4 |
| 0x0049d320 | dshow | `?SendQuality@CBaseVideoRenderer@@UAEJ_J0@Z` (renbase.obj) | 359 | created exact | 359 B | added | C4 |
| 0x0049d490 | dshow | `?ShouldDrawSampleNow@CBaseVideoRenderer@@UAEJPAUIMediaSample` (renbase.obj) | 1105 | created exact | 1105 B | added | C4 |
| 0x0049d8f0 | dshow | `?ScheduleSample@CBaseVideoRenderer@@UAEHPAUIMediaSample@@@Z` (renbase.obj) | 38 | created exact | 38 B | added | C4 |
| 0x0049dbc0 | dshow | `?get_DevSyncOffset@CBaseVideoRenderer@@UAGJPAH@Z` (renbase.obj) | 44 | created exact | 44 B | added | C4 |
| 0x0049dbf0 | dshow | `?get_Jitter@CBaseVideoRenderer@@UAGJPAH@Z` (renbase.obj) | 46 | created exact | 46 B | added | C4 |
| 0x0049dc20 | dshow | `?NonDelegatingQueryInterface@CBaseVideoRenderer@@UAGJABU_GUI` (renbase.obj) | 129 | created exact | 129 B | added | C4 |
| 0x0049e0a0 | dshow | `?SourceThreadCanWait@CBaseRenderer@@UAEJH@Z` (renbase.obj) | 37 | created exact | 37 B | added | C4 |
| 0x0049e0d0 | dshow | `?WaitForRenderTime@CBaseRenderer@@UAEJXZ` (renbase.obj) | 99 | created exact | 99 B | added | C4 |
| 0x0049e1f0 | dshow | `?Stop@CBaseRenderer@@UAGJXZ` (renbase.obj) | 199 | created exact | 199 B | added | C4 |
| 0x0049e2c0 | dshow | `?Pause@CBaseRenderer@@UAGJXZ` (renbase.obj) | 228 | created exact | 228 B | added | C4 |
| 0x0049e3b0 | dshow | `?Run@CBaseRenderer@@UAGJ_J@Z` (renbase.obj) | 246 | created exact | 246 B | added | C4 |
| 0x0049e850 | dshow | `?Receive@CBaseRenderer@@UAEJPAUIMediaSample@@@Z` (renbase.obj) | 363 | created exact | 363 B | added | C4 |
| 0x0049e9d0 | dshow | `?SendEndOfStream@CBaseRenderer@@UAEJXZ` (renbase.obj) | 154 | created exact | 154 B | added | C4 |
| 0x0049eae0 | dshow | `?Receive@CRendererInputPin@@UAGJPAUIMediaSample@@@Z` (renbase.obj) | 213 | created exact | 213 B | added | C4 |
| 0x0049ed70 | dshow | `?GetClassID@CBaseFilter@@UAGJPAU_GUID@@@Z` (amfilter.obj) | 50 | created exact | 50 B | added | C4 |
| 0x0049edb0 | dshow | `?SetSyncSource@CBaseFilter@@UAGJPAUIReferenceClock@@@Z` (amfilter.obj) | 109 | created exact | 109 B | added | C4 |
| 0x0049ee20 | dshow | `?GetSyncSource@CBaseFilter@@UAGJPAPAUIReferenceClock@@@Z` (amfilter.obj) | 130 | created exact | 130 B | added | C4 |
| 0x0049eed0 | dshow | `?StreamTime@CBaseFilter@@UAEJAAVCRefTime@@@Z` (amfilter.obj) | 62 | created exact | 62 B | added | C4 |
| 0x0049f160 | dshow | `?AddRef@CEnumPins@@UAGKXZ` (amfilter.obj) | 17 | created exact | 17 B | added | C4 |
| 0x0049f290 | dshow | `?AddRef@CEnumMediaTypes@@UAGKXZ` (amfilter.obj) | 17 | created exact | 17 B | added | C4 |
| 0x0049f360 | dshow | `?NonDelegatingQueryInterface@CBasePin@@UAGJABU_GUID@@PAPAX@Z` (amfilter.obj) | 125 | created exact | 125 B | added | C4 |
| 0x0049f3e0 | dshow | `?NonDelegatingAddRef@CBasePin@@UAGKXZ` (amfilter.obj) | 20 | created exact | 20 B | added | C4 |
| 0x0049f400 | dshow | `?NonDelegatingRelease@CBasePin@@UAGKXZ` (amfilter.obj) | 20 | created exact | 20 B | added | C4 |
| 0x0049f6b0 | dshow | `?CheckConnect@CBasePin@@UAEJPAUIPin@@@Z` (amfilter.obj) | 42 | created exact | 42 B | added | C4 |
| 0x0049f6e0 | dshow | `?ReceiveConnection@CBasePin@@UAGJPAUIPin@@PBU_AMMediaType@@@` (amfilter.obj) | 333 | created exact | 333 B | added | C4 |
| 0x0049f870 | dshow | `?ConnectedTo@CBasePin@@UAGJPAPAUIPin@@@Z` (amfilter.obj) | 48 | created exact | 48 B | added | C4 |
| 0x0049f8a0 | dshow | `?ConnectionMediaType@CBasePin@@UAGJPAU_AMMediaType@@@Z` (amfilter.obj) | 168 | created exact | 168 B | added | C4 |
| 0x0049f9c0 | dshow | `?QueryDirection@CBasePin@@UAGJPAW4_PinDirection@@@Z` (amfilter.obj) | 30 | created exact | 30 B | added | C4 |
| 0x0049f9e0 | dshow | `?QueryAccept@CBasePin@@UAGJPBU_AMMediaType@@@Z` (amfilter.obj) | 42 | created exact | 42 B | added | C4 |
| 0x0049fa50 | dshow | `?SetSink@CBasePin@@UAGJPAUIQualityControl@@@Z` (amfilter.obj) | 37 | created exact | 37 B | added | C4 |
| 0x0049fa80 | dshow | `?NewSegment@CBasePin@@UAGJ_J0N@Z` (amfilter.obj) | 60 | created exact | 60 B | added | C4 |
| 0x0049fb20 | dshow | `?NonDelegatingQueryInterface@CBaseInputPin@@UAGJABU_GUID@@PA` (amfilter.obj) | 78 | created exact | 78 B | added | C4 |
| 0x0049fb70 | dshow | `?GetAllocator@CBaseInputPin@@UAGJPAPAUIMemAllocator@@@Z` (amfilter.obj) | 142 | created exact | 142 B | added | C4 |
| 0x0049fc00 | dshow | `?NotifyAllocator@CBaseInputPin@@UAGJPAUIMemAllocator@@H@Z` (amfilter.obj) | 125 | created exact | 125 B | added | C4 |
| 0x0049fe70 | dshow | `?ReceiveCanBlock@CBaseInputPin@@UAGJXZ` (amfilter.obj) | 182 | created exact | 182 B | added | C4 |
| 0x0049ff90 | dshow | `?Notify@CBaseInputPin@@UAGJPAUIBaseFilter@@UtagQuality@@@Z` (amfilter.obj) | 20 | created exact | 20 B | added | C4 |
| 0x0049ffb0 | dshow | `?CheckStreaming@CBaseInputPin@@UAEJXZ` (amfilter.obj) | 45 | created exact | 45 B | added | C4 |
| 0x004a0460 | dshow | `?Register@CBaseFilter@@UAGJXZ` (amfilter.obj) | 107 | created exact | 107 B | added | C4 |
| 0x004a04d0 | dshow | `?Unregister@CBaseFilter@@UAGJXZ` (amfilter.obj) | 125 | created exact | 125 B | added | C4 |
| 0x004a06e0 | dshow | `?Next@CEnumMediaTypes@@UAGJKPAPAU_AMMediaType@@PAK@Z` (amfilter.obj) | 355 | created exact | 355 B | added | C4 |
| 0x004a0850 | dshow | `?Skip@CEnumMediaTypes@@UAGJK@Z` (amfilter.obj) | 184 | created exact | 184 B | added | C4 |
| 0x004a0910 | dshow | `?Connect@CBasePin@@UAGJPAUIPin@@PBU_AMMediaType@@@Z` (amfilter.obj) | 258 | created exact | 258 B | added | C4 |
| 0x004a0a20 | dshow | `?Disconnect@CBasePin@@UAGJXZ` (amfilter.obj) | 129 | created exact | 129 B | added | C4 |
| 0x004a0b70 | dshow | `?Clone@CEnumPins@@UAGJPAPAUIEnumPins@@@Z` (amfilter.obj) | 156 | created exact | 156 B | added | C4 |
| 0x004a0c70 | dshow | `?Clone@CEnumMediaTypes@@UAGJPAPAUIEnumMediaTypes@@@Z` (amfilter.obj) | 156 | created exact | 156 B | added | C4 |
| 0x004a0e00 | dshow | `?EnumMediaTypes@CBasePin@@UAGJPAPAUIEnumMediaTypes@@@Z` (amfilter.obj) | 136 | created exact | 136 B | added | C4 |
| 0x004a0f50 | dshow | `?EnumPins@CBaseFilter@@UAGJPAPAUIEnumPins@@@Z` (amfilter.obj) | 136 | created exact | 136 B | added | C4 |
| 0x004a1120 | dshow | `?NonDelegatingRelease@CUnknown@@UAGKXZ` (combase.obj) | 60 | created exact | 60 B | added | C4 |
| 0x004a18a0 | dshow | `?GetIDsOfNames@CBaseDispatch@@QAGJABU_GUID@@PAPAGIKPAJ@Z` (ctlutil.obj) | 73 | created exact | 73 B | added | C4 |
| 0x004a1a10 | dshow | `?NonDelegatingQueryInterface@CPosPassThru@@UAGJABU_GUID@@PAP` (ctlutil.obj) | 80 | skipped: flow body 136 B leaves the extent (body outside extent from 0x004a18f0) |  | not added |  |
| 0x004a1b20 | dshow | `?GetCapabilities@CPosPassThru@@UAGJPAK@Z` (ctlutil.obj) | 50 | created exact | 50 B | added | C4 |
| 0x004a1b60 | dshow | `?CheckCapabilities@CPosPassThru@@UAGJPAK@Z` (ctlutil.obj) | 50 | created exact | 50 B | added | C4 |
| 0x004a1ba0 | dshow | `?IsFormatSupported@CPosPassThru@@UAGJPBU_GUID@@@Z` (ctlutil.obj) | 50 | created exact | 50 B | added | C4 |
| 0x004a1be0 | dshow | `?QueryPreferredFormat@CPosPassThru@@UAGJPAU_GUID@@@Z` (ctlutil.obj) | 50 | created exact | 50 B | added | C4 |
| 0x004a1c20 | dshow | `?SetTimeFormat@CPosPassThru@@UAGJPBU_GUID@@@Z` (ctlutil.obj) | 50 | created exact | 50 B | added | C4 |
| 0x004a1c60 | dshow | `?GetTimeFormatA@CPosPassThru@@UAGJPAU_GUID@@@Z` (ctlutil.obj) | 50 | created exact | 50 B | added | C4 |
| 0x004a1ca0 | dshow | `?IsUsingTimeFormat@CPosPassThru@@UAGJPBU_GUID@@@Z` (ctlutil.obj) | 50 | created exact | 50 B | added | C4 |
| 0x004a1ce0 | dshow | `?ConvertTimeFormat@CPosPassThru@@UAGJPA_JPBU_GUID@@_J1@Z` (ctlutil.obj) | 70 | created exact | 70 B | added | C4 |
| 0x004a1d30 | dshow | `?SetPositions@CPosPassThru@@UAGJPA_JK0K@Z` (ctlutil.obj) | 65 | created exact | 65 B | added | C4 |
| 0x004a1d80 | dshow | `?GetPositions@CPosPassThru@@UAGJPA_J0@Z` (ctlutil.obj) | 55 | created exact | 55 B | added | C4 |
| 0x004a1df0 | dshow | `?GetAvailable@CPosPassThru@@UAGJPA_J0@Z` (ctlutil.obj) | 55 | created exact | 55 B | added | C4 |
| 0x004a1e30 | dshow | `?GetRate@CPosPassThru@@UAGJPAN@Z` (ctlutil.obj) | 50 | created exact | 50 B | added | C4 |
| 0x004a1e70 | dshow | `?SetRate@CPosPassThru@@UAGJN@Z` (ctlutil.obj) | 80 | created exact | 80 B | added | C4 |
| 0x004a1ec0 | dshow | `?get_Duration@CPosPassThru@@UAGJPAN@Z` (ctlutil.obj) | 53 | created exact | 53 B | added | C4 |
| 0x004a1f00 | dshow | `?get_CurrentPosition@CPosPassThru@@UAGJPAN@Z` (ctlutil.obj) | 53 | created exact | 53 B | added | C4 |
| 0x004a1f40 | dshow | `?put_CurrentPosition@CPosPassThru@@UAGJN@Z` (ctlutil.obj) | 58 | created exact | 58 B | added | C4 |
| 0x004a1f80 | dshow | `?get_StopTime@CPosPassThru@@UAGJPAN@Z` (ctlutil.obj) | 53 | created exact | 53 B | added | C4 |
| 0x004a1fc0 | dshow | `?put_StopTime@CPosPassThru@@UAGJN@Z` (ctlutil.obj) | 58 | created exact | 58 B | added | C4 |
| 0x004a2000 | dshow | `?get_PrerollTime@CPosPassThru@@UAGJPAN@Z` (ctlutil.obj) | 53 | created exact | 53 B | added | C4 |
| 0x004a2040 | dshow | `?put_PrerollTime@CPosPassThru@@UAGJN@Z` (ctlutil.obj) | 58 | created exact | 58 B | added | C4 |
| 0x004a2080 | dshow | `?get_Rate@CPosPassThru@@UAGJPAN@Z` (ctlutil.obj) | 53 | created exact | 53 B | added | C4 |
| 0x004a20c0 | dshow | `?put_Rate@CPosPassThru@@UAGJN@Z` (ctlutil.obj) | 83 | created exact | 83 B | added | C4 |
| 0x004a2120 | dshow | `?CanSeekForward@CPosPassThru@@UAGJPAJ@Z` (ctlutil.obj) | 53 | created exact | 53 B | added | C4 |
| 0x004a2160 | dshow | `?CanSeekBackward@CPosPassThru@@UAGJPAJ@Z` (ctlutil.obj) | 53 | created exact | 53 B | added | C4 |
| 0x004a2590 | dshow | `?AddRef@CPosPassThru@@UAGKXZ` (ctlutil.obj) | 16 | created exact | 16 B | added | C4 |
| 0x004a25a0 | dshow | `?Release@CPosPassThru@@UAGKXZ` (ctlutil.obj) | 16 | created exact | 16 B | added | C4 |
| 0x004ece75 | d3dx9 | `?c_D3DXVec3TransformArray@@YGPAUD3DXVECTOR4@@PAU1@IPBUD3DXVE` (d3dxmath.obj) | 167 | created exact | 167 B | added | C4 |
| 0x004ecf59 | d3dx9 | `?c_D3DXVec3TransformNormal@@YGPAUD3DXVECTOR3@@PAU1@PBU1@PBUD` (d3dxmath.obj) | 102 | created exact | 102 B | added | C4 |
| 0x004ecfd0 | d3dx9 | `?c_D3DXVec3TransformNormalArray@@YGPAUD3DXVECTOR3@@PAU1@IPBU` (d3dxmath.obj) | 126 | created exact | 126 B | added | C4 |
| 0x004ed09f | d3dx9 | `?c_D3DXVec4Cross@@YGPAUD3DXVECTOR4@@PAU1@PBU1@11@Z` (d3dxmath.obj) | 240 | created exact | 240 B | added | C4 |
| 0x004ed467 | d3dx9 | `?c_D3DXVec4Transform@@YGPAUD3DXVECTOR4@@PAU1@PBU1@PBUD3DXMAT` (d3dxmath.obj) | 159 | created exact | 159 B | added | C4 |
| 0x004ed517 | d3dx9 | `?c_D3DXVec4TransformArray@@YGPAUD3DXVECTOR4@@PAU1@IPBU1@IPBU` (d3dxmath.obj) | 191 | created exact | 191 B | added | C4 |
| 0x004ed5e3 | d3dx9 | `?c_D3DXMatrixDeterminant@@YGMPBUD3DXMATRIX@@@Z` (d3dxmath.obj) | 202 | created exact | 202 B | added | C4 |
| 0x004ed6c0 | d3dx9 | `?c_D3DXMatrixMultiply@@YGPAUD3DXMATRIX@@PAU1@PBU1@1@Z` (d3dxmath.obj) | 566 | created exact | 566 B | added | C4 |
| 0x004ed909 | d3dx9 | `?c_D3DXMatrixTranspose@@YGPAUD3DXMATRIX@@PAU1@PBU1@@Z` (d3dxmath.obj) | 109 | created exact | 109 B | added | C4 |
| 0x004ed989 | d3dx9 | `?c_D3DXMatrixMultiplyTranspose@@YGPAUD3DXMATRIX@@PAU1@PBU1@1` (d3dxmath.obj) | 566 | created exact | 566 B | added | C4 |
| 0x004edbd2 | d3dx9 | `?c_D3DXMatrixInverse@@YGPAUD3DXMATRIX@@PAU1@PAMPBU1@@Z` (d3dxmath.obj) | 927 | created exact | 927 B | added | C4 |
| 0x004edf9f | d3dx9 | `?c_D3DXMatrixScaling@@YGPAUD3DXMATRIX@@PAU1@MMM@Z` (d3dxmath.obj) | 92 | created exact | 92 B | added | C4 |
| 0x004ee029 | d3dx9 | `?c_D3DXMatrixTranslation@@YGPAUD3DXMATRIX@@PAU1@MMM@Z` (d3dxmath.obj) | 92 | created exact | 92 B | added | C4 |
| 0x004ee37f | d3dx9 | `?c_D3DXMatrixRotationQuaternion@@YGPAUD3DXMATRIX@@PAU1@PBUD3` (d3dxmath.obj) | 226 | created exact | 226 B | added | C4 |
| 0x004ee5d4 | d3dx9 | `?c_D3DXQuaternionToAxisAngle@@YGXPBUD3DXQUATERNION@@PAUD3DXV` (d3dxmath.obj) | 53 | created exact | 53 B | added | C4 |
| 0x004ee616 | d3dx9 | `?c_D3DXQuaternionRotationMatrix@@YGPAUD3DXQUATERNION@@PAU1@P` (d3dxmath.obj) | 308 | created exact | 308 B | added | C4 |
| 0x004ee7fa | d3dx9 | `?c_D3DXQuaternionRotationYawPitchRoll@@YGPAUD3DXQUATERNION@@` (d3dxmath.obj) | 217 | created exact | 217 B | added | C4 |
| 0x004ee8e6 | d3dx9 | `?c_D3DXQuaternionMultiply@@YGPAUD3DXQUATERNION@@PAU1@PBU1@1@` (d3dxmath.obj) | 150 | created exact | 150 B | added | C4 |
| 0x004ee99c | d3dx9 | `?c_D3DXQuaternionInverse@@YGPAUD3DXQUATERNION@@PAU1@PBU1@@Z` (d3dxmath.obj) | 177 | created exact | 177 B | added | C4 |
| 0x004eea60 | d3dx9 | `?c_D3DXQuaternionLn@@YGPAUD3DXQUATERNION@@PAU1@PBU1@@Z` (d3dxmath.obj) | 123 | created exact | 123 B | added | C4 |
| 0x004eeaee | d3dx9 | `?c_D3DXQuaternionExp@@YGPAUD3DXQUATERNION@@PAU1@PBU1@@Z` (d3dxmath.obj) | 152 | created exact | 152 B | added | C4 |
| 0x004eebb0 | d3dx9 | `?c_D3DXQuaternionSlerp@@YGPAUD3DXQUATERNION@@PAU1@PBU1@1M@Z` (d3dxmath.obj) | 227 | created exact | 227 B | added | C4 |
| 0x004eed25 | d3dx9 | `?init_D3DXQuaternionBaryCentric@@YGPAUD3DXQUATERNION@@PAU1@P` (d3dxmath.obj) | 47 | skipped: 4 library functions identical |  | not added |  |
| 0x004eedef | d3dx9 | `?c_D3DXPlaneNormalize@@YGPAUD3DXPLANE@@PAU1@PBU1@@Z` (d3dxmath.obj) | 161 | created exact | 161 B | added | C4 |
| 0x004eee9d | d3dx9 | `?c_D3DXPlaneIntersectLine@@YGPAUD3DXVECTOR3@@PAU1@PBUD3DXPLA` (d3dxmath.obj) | 162 | created exact | 162 B | added | C4 |
| 0x004eef4c | d3dx9 | `?c_D3DXPlaneFromPointNormal@@YGPAUD3DXPLANE@@PAU1@PBUD3DXVEC` (d3dxmath.obj) | 56 | created exact | 56 B | added | C4 |
| 0x004eef91 | d3dx9 | `?c_D3DXPlaneFromPoints@@YGPAUD3DXPLANE@@PAU1@PBUD3DXVECTOR3@` (d3dxmath.obj) | 162 | created exact | 162 B | added | C4 |
| 0x004ef040 | d3dx9 | `?c_D3DXPlaneTransform@@YGPAUD3DXPLANE@@PAU1@PBU1@PBUD3DXMATR` (d3dxmath.obj) | 159 | created exact | 159 B | added | C4 |
| 0x004ef0f0 | d3dx9 | `?c_D3DXPlaneTransformArray@@YGPAUD3DXPLANE@@PAU1@IPBU1@IPBUD` (d3dxmath.obj) | 191 | created exact | 191 B | added | C4 |
| 0x004ef1d1 | d3dx9 | `?c_D3DXVec2Normalize@@YGPAUD3DXVECTOR2@@PAU1@PBU1@@Z` (d3dxmath.obj) | 138 | created exact | 138 B | added | C4 |
| 0x004f03c6 | d3dx9 | `?c_D3DXQuaternionSquadSetup@@YGXPAUD3DXQUATERNION@@00PBU1@11` (d3dxmath.obj) | 986 | created exact | 986 B | added | C4 |
| 0x004fba47 | d3dx9 | `?c_D3DXPSGPTessellateNPatch@@YGXPAUD3DXVECTOR3@@PAUD3DXVECTO` (psgpmesh.obj) | 817 | created exact | 817 B | added | C4 |
| 0x00504489 | d3dx9 | `?Init@CBuffer@D3DXCore@@UAEJK@Z` (cbuffer.obj) | 39 | created exact | 39 B | added | C4 |
| 0x005044bd | d3dx9 | `?Release@CBuffer@D3DXCore@@UAGKXZ` (cbuffer.obj) | 32 | created exact | 32 B | added | C4 |
| 0x00510809 | d3dx9 | `?IsEqual@CNodeToken@D3DXShader@@UAEHPAVCNode@2@@Z` (cnode.obj) | 34 | created exact | 34 B | added | C4 |
| 0x005108cd | d3dx9 | `?IsEqual@CNodeType@D3DXShader@@UAEHPAVCNode@2@@Z` (cnode.obj) | 64 | created exact | 64 B | added | C4 |
| 0x00513000 | d3dx9 | `_a_atan` (amdmath.obj) | 191 | created body+fill | 184 B + 7 B fill | added | C4 |
| 0x005132c0 | d3dx9 | `_a_asin` (amdmath.obj) | 256 | created body+fill | 226 B + 30 B fill | added | C4 |
| 0x005133c0 | d3dx9 | `_a_log` (amdmath.obj) | 256 | created body+fill | 233 B + 23 B fill | added | C4 |
| 0x005134c0 | d3dx9 | `_a_log10` (amdmath.obj) | 31 | created body+fill | 17 B + 14 B fill | added | C4 |
| 0x005134e0 | d3dx9 | `_a_exp` (amdmath.obj) | 224 | created body+fill | 213 B + 11 B fill | added | C4 |
| 0x005135c0 | d3dx9 | `_a_sqrt` (amdmath.obj) | 31 | created body+fill | 24 B + 7 B fill | added | C4 |
| 0x005135e0 | d3dx9 | `_a_fabs` (amdmath.obj) | 32 | created body+fill | 8 B + 24 B fill | added | C4 |
| 0x00513600 | d3dx9 | `_a_ceil` (amdmath.obj) | 64 | created body+fill | 50 B + 14 B fill | added | C4 |
| 0x00513640 | d3dx9 | `_a_floor` (amdmath.obj) | 64 | created body+fill | 37 B + 27 B fill | added | C4 |
| 0x00513680 | d3dx9 | `_a_frexp` (amdmath.obj) | 64 | created body+fill | 62 B + 2 B fill | added | C4 |
| 0x005136c0 | d3dx9 | `_a_ldexp` (amdmath.obj) | 64 | skipped: flow body 74 B leaves the extent (body outside extent from 0x00513700) |  | not added |  |
| 0x00513700 | d3dx9 | `exception` (amdmath.obj) | 28 | skipped: flow body 30 B leaves the extent (body outside extent from 0x0051371c) |  | not added |  |
| 0x00513720 | d3dx9 | `_a_modf` (amdmath.obj) | 32 | created body+fill | 16 B + 16 B fill | added | C4 |
| 0x00513740 | d3dx9 | `_a_fmod` (amdmath.obj) | 64 | skipped: flow body 58 B leaves the extent (body outside extent from 0x00513780) |  | not added |  |
| 0x00513780 | d3dx9 | `exception` (amdmath.obj) | 28 | skipped: bytes differ at +0 |  | not added |  |
| 0x005139a0 | d3dx9 | `_a_tan` (amdmath.obj) | 128 | skipped: 3 library functions identical |  | not added |  |
| 0x00513a20 | d3dx9 | `x5` (amdmath.obj) | 116 | skipped: flow body 145 B leaves the extent (body outside extent from 0x00513a94) |  | not added |  |
| 0x00513a9a | d3dx9 | `ending` (amdmath.obj) | 4 | skipped: bytes differ at +0 |  | not added |  |
| 0x00513ac0 | d3dx9 | `_a_pow` (amdmath.obj) | 408 | created exact | 408 B | added | C4 |

## Labels inside another pc_funcs body (39)

| label va | symbol | outer pc_funcs body | last insn before label | branches into [label, end) | verdict |
|---|---|---|---|---|---|
| 0x00508b7a | `TransformLoop` | FUN_00508b40 0x00508b40..0x00508bd5 | `0x00508b76 movups xmm7, xmmword ptr [edx + 0x30]` | 0x00508b61->0x00508bc0 | no-split |
| 0x00508bc0 | `ProcEpilog` | FUN_00508b40 0x00508b40..0x00508bd5 | `0x00508bbe jg 0x508b7a` | 0x00508b61->0x00508bc0 | no-split |
| 0x00508c3a | `TransformLoop` | FUN_00508c00 0x00508c00..0x00508c95 | `0x00508c36 movups xmm7, xmmword ptr [edx + 0x30]` | 0x00508c21->0x00508c80 | no-split |
| 0x00508c80 | `ProcEpilog` | FUN_00508c00 0x00508c00..0x00508c95 | `0x00508c7e jg 0x508c3a` | 0x00508c21->0x00508c80 | no-split |
| 0x00508d00 | `ScalarLoop` | FUN_00508cc0 0x00508cc0..0x00508f1f | `0x00508cfd cmp ecx, 0` | 0x00508cf5->0x00508d39 | no-split |
| 0x00508d39 | `SIMDMatrix` | FUN_00508cc0 0x00508cc0..0x00508f1f | `0x00508d37 jmp 0x508d00` | 0x00508cf5->0x00508d39 0x00508d00->0x00508ef9 | no-split |
| 0x00508dfd | `TransformLoop` | FUN_00508cc0 0x00508cc0..0x00508f1f | `0x00508df7 mov edi, dword ptr [ebp + 0x114]` | 0x00508d00->0x00508ef9 | no-split |
| 0x00508ed1 | `PostTransformLoop` | FUN_00508cc0 0x00508cc0..0x00508f1f | `0x00508ecc jmp 0x508dfd` | 0x00508d00->0x00508ef9 0x00508e00->0x00508ed1 | no-split |
| 0x00508ef9 | `ProcEpilog` | FUN_00508cc0 0x00508cc0..0x00508f1f | `0x00508ef4 jmp 0x508dfd` | 0x00508d00->0x00508ef9 0x00508ed4->0x00508ef9 | no-split |
| 0x00508f60 | `ScalarLoop` | FUN_00508f20 0x00508f20..0x00509165 | `0x00508f5d cmp ecx, 0` | 0x00508f55->0x00508f99 | no-split |
| 0x00508f99 | `SIMDMatrix` | FUN_00508f20 0x00508f20..0x00509165 | `0x00508f97 jmp 0x508f60` | 0x00508f55->0x00508f99 0x00508f60->0x0050913f | no-split |
| 0x0050905d | `TransformLoop` | FUN_00508f20 0x00508f20..0x00509165 | `0x00509057 mov edi, dword ptr [ebp + 0x114]` | 0x00508f60->0x0050913f | no-split |
| 0x00509117 | `PostTransformLoop` | FUN_00508f20 0x00508f20..0x00509165 | `0x00509112 jmp 0x50905d` | 0x00508f60->0x0050913f 0x00509060->0x00509117 | no-split |
| 0x0050913f | `ProcEpilog` | FUN_00508f20 0x00508f20..0x00509165 | `0x0050913a jmp 0x50905d` | 0x00508f60->0x0050913f 0x0050911a->0x0050913f | no-split |
| 0x005091c0 | `ScalarLoop` | FUN_00509180 0x00509180..0x00509336 | `0x005091bd cmp ecx, 0` | 0x005091b5->0x005091f9 | no-split |
| 0x005091f9 | `SIMDMatrix` | FUN_00509180 0x00509180..0x00509336 | `0x005091f7 jmp 0x5091c0` | 0x005091b5->0x005091f9 0x005091c0->0x00509310 | no-split |
| 0x00509279 | `TransformLoop` | FUN_00509180 0x00509180..0x00509336 | `0x00509273 mov edi, dword ptr [ebp + 0x114]` | 0x005091c0->0x00509310 | no-split |
| 0x005092e8 | `PostTransformLoop` | FUN_00509180 0x00509180..0x00509336 | `0x005092e6 jmp 0x509279` | 0x005091c0->0x00509310 0x0050927c->0x005092e8 | no-split |
| 0x00509310 | `ProcEpilog` | FUN_00509180 0x00509180..0x00509336 | `0x0050930b jmp 0x509279` | 0x005091c0->0x00509310 0x005092eb->0x00509310 | no-split |
| 0x0050937a | `TransformLoop` | FUN_00509340 0x00509340..0x005093c9 | `0x00509376 movups xmm7, xmmword ptr [edx + 0x30]` | 0x00509361->0x005093b4 | no-split |
| 0x005093b4 | `ProcEpilog` | FUN_00509340 0x00509340..0x005093c9 | `0x005093b2 jg 0x50937a` | 0x00509361->0x005093b4 | no-split |
| 0x00509420 | `ScalarLoop` | FUN_005093e0 0x005093e0..0x005096fd | `0x0050941d cmp ecx, 0` | 0x00509415->0x00509459 | no-split |
| 0x00509459 | `SIMDMatrix` | FUN_005093e0 0x005093e0..0x005096fd | `0x00509457 jmp 0x509420` | 0x00509415->0x00509459 0x00509420->0x005096d7 | no-split |
| 0x00509561 | `TransformLoop` | FUN_005093e0 0x005093e0..0x005096fd | `0x0050955b mov edi, dword ptr [ebp + 0x114]` | 0x00509420->0x005096d7 | no-split |
| 0x005096af | `PostTransformLoop` | FUN_005093e0 0x005093e0..0x005096fd | `0x005096aa jmp 0x509561` | 0x00509420->0x005096d7 0x00509564->0x005096af | no-split |
| 0x005096d7 | `ProcEpilog` | FUN_005093e0 0x005093e0..0x005096fd | `0x005096d2 jmp 0x509561` | 0x00509420->0x005096d7 0x005096b2->0x005096d7 | no-split |
| 0x00509740 | `ScalarLoop` | FUN_00509700 0x00509700..0x0050997c | `0x0050973d cmp ecx, 0` | 0x00509735->0x00509779 | no-split |
| 0x00509779 | `SIMDMatrix` | FUN_00509700 0x00509700..0x0050997c | `0x00509777 jmp 0x509740` | 0x00509735->0x00509779 0x00509740->0x00509956 | no-split |
| 0x0050983d | `TransformLoop` | FUN_00509700 0x00509700..0x0050997c | `0x00509837 mov edi, dword ptr [ebp + 0x114]` | 0x00509740->0x00509956 | no-split |
| 0x0050992e | `PostTransformLoop` | FUN_00509700 0x00509700..0x0050997c | `0x00509929 jmp 0x50983d` | 0x00509740->0x00509956 0x00509840->0x0050992e | no-split |
| 0x00509956 | `ProcEpilog` | FUN_00509700 0x00509700..0x0050997c | `0x00509951 jmp 0x50983d` | 0x00509740->0x00509956 0x00509931->0x00509956 | no-split |
| 0x0050f73f | `Float16Algo` | FUN_0050f700 0x0050f700..0x0050f82a | `0x0050f73a jmp 0x50f812` | 0x0050f71c->0x0050f73f 0x0050f721->0x0050f812 0x0050f73a->0x0050f812 | no-split |
| 0x0050f7f2 | `PostFloat16Algo` | FUN_0050f700 0x0050f700..0x0050f82a | `0x0050f7ed jmp 0x50f73f` | 0x0050f721->0x0050f812 0x0050f73a->0x0050f812 0x0050f742->0x0050f7f2 | no-split |
| 0x0050f812 | `ProcEpilog` | FUN_0050f700 0x0050f700..0x0050f82a | `0x0050f80d jmp 0x50f73f` | 0x0050f721->0x0050f812 0x0050f73a->0x0050f812 0x0050f7f5->0x0050f812 | no-split |
| 0x0050f883 | `Float32Algo` | FUN_0050f840 0x0050f840..0x0050f9bb | `0x0050f87e jmp 0x50f9a2` | 0x0050f860->0x0050f883 0x0050f865->0x0050f9a2 0x0050f87e->0x0050f9a2 | no-split |
| 0x0050f982 | `PostFloat32Algo` | FUN_0050f840 0x0050f840..0x0050f9bb | `0x0050f97d jmp 0x50f883` | 0x0050f865->0x0050f9a2 0x0050f87e->0x0050f9a2 0x0050f886->0x0050f982 | no-split |
| 0x0050f9a2 | `ProcEpilog` | FUN_0050f840 0x0050f840..0x0050f9bb | `0x0050f99d jmp 0x50f883` | 0x0050f865->0x0050f9a2 0x0050f87e->0x0050f9a2 0x0050f985->0x0050f9a2 | no-split |
| 0x00513820 | `x5` | FUN_005137a0 0x005137a0..0x0051389c | `0x0051381c pfsubr mm1, mm3` | 0x0051381a->0x00513820 | no-split |
| 0x00513920 | `x5` | FUN_005138a0 0x005138a0..0x0051399e | `0x0051391c pfsubr mm1, mm3` | 0x0051391a->0x00513920 | no-split |

## Rows waiting for a sub-tag decision (23)

| rva | symbol | object | reason |
|---|---|---|---|
| 0x004fc6f5 | `?x86_D3DXVec2Transform@@YGPAUD3DXVECTOR4@@PAU1@PBUD3DXVECTOR` | d3dxmathx86.obj | sub-tag ambiguous: d3dx9.lib:obj\i386\d3dxmathx86.obj -> third-party-library[d3dx9-psgp]/third-party-library[d3dx9-shader-compiler] |
| 0x004fc75e | `?x86_D3DXVec2TransformNormal@@YGPAUD3DXVECTOR2@@PAU1@PBU1@PB` | d3dxmathx86.obj | sub-tag ambiguous: d3dx9.lib:obj\i386\d3dxmathx86.obj -> third-party-library[d3dx9-psgp]/third-party-library[d3dx9-shader-compiler] |
| 0x004fc793 | `?x86_D3DXVec3Transform@@YGPAUD3DXVECTOR4@@PAU1@PBUD3DXVECTOR` | d3dxmathx86.obj | sub-tag ambiguous: d3dx9.lib:obj\i386\d3dxmathx86.obj -> third-party-library[d3dx9-psgp]/third-party-library[d3dx9-shader-compiler] |
| 0x004fc81e | `?x86_D3DXVec3TransformNormal@@YGPAUD3DXVECTOR3@@PAU1@PBU1@PB` | d3dxmathx86.obj | sub-tag ambiguous: d3dx9.lib:obj\i386\d3dxmathx86.obj -> third-party-library[d3dx9-psgp]/third-party-library[d3dx9-shader-compiler] |
| 0x004fc87d | `?x86_D3DXVec4Transform@@YGPAUD3DXVECTOR4@@PAU1@PBU1@PBUD3DXM` | d3dxmathx86.obj | sub-tag ambiguous: d3dx9.lib:obj\i386\d3dxmathx86.obj -> third-party-library[d3dx9-psgp]/third-party-library[d3dx9-shader-compiler] |
| 0x00508d00 | `ScalarLoop` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00508cc0 FUN_00508cc0); no name rule for ScalarLoop |
| 0x00508d39 | `SIMDMatrix` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00508cc0 FUN_00508cc0); no name rule for SIMDMatrix |
| 0x00508dfd | `TransformLoop` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00508cc0 FUN_00508cc0); no name rule for TransformLoop |
| 0x00508ed1 | `PostTransformLoop` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00508cc0 FUN_00508cc0); no name rule for PostTransformLoop |
| 0x00508f60 | `ScalarLoop` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00508f20 FUN_00508f20); no name rule for ScalarLoop |
| 0x00508f99 | `SIMDMatrix` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00508f20 FUN_00508f20); no name rule for SIMDMatrix |
| 0x0050905d | `TransformLoop` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00508f20 FUN_00508f20); no name rule for TransformLoop |
| 0x00509117 | `PostTransformLoop` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00508f20 FUN_00508f20); no name rule for PostTransformLoop |
| 0x005091c0 | `ScalarLoop` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00509180 FUN_00509180); no name rule for ScalarLoop |
| 0x005091f9 | `SIMDMatrix` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00509180 FUN_00509180); no name rule for SIMDMatrix |
| 0x00509279 | `TransformLoop` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00509180 FUN_00509180); no name rule for TransformLoop |
| 0x005092e8 | `PostTransformLoop` | d3dxvec2.obj | no sub-tag: object d3dx9.lib:obj\i386\d3dxvec2.obj matched by no d3dx9 row; no pc_funcs entry at rva (inside 0x00509180 FUN_00509180); no name rule for PostTransformLoop |
| 0x0050fc7c | `_x3d_D3DXVec4Transform@12` | x3d_vec4.obj | no sub-tag: object d3dx9.lib:obj\i386\x3d_vec4.obj matched by no d3dx9 row |
| 0x0050fcfe | `?x3d_D3DXVec4Normalize@@YGPAUD3DXVECTOR4@@PAU1@PBU1@@Z` | x3d_vec4.obj | no sub-tag: object d3dx9.lib:obj\i386\x3d_vec4.obj matched by no d3dx9 row |
| 0x0050fd5d | `?x3d_D3DXVec4Cross@@YGPAUD3DXVECTOR4@@PAU1@PBU1@11@Z` | x3d_vec4.obj | no sub-tag: object d3dx9.lib:obj\i386\x3d_vec4.obj matched by no d3dx9 row |
| 0x0050fe8f | `?x3d_D3DXVec4BaryCentric@@YGPAUD3DXVECTOR4@@PAU1@PBU1@11MM@Z` | x3d_vec4.obj | no sub-tag: object d3dx9.lib:obj\i386\x3d_vec4.obj matched by no d3dx9 row |
| 0x0050ff00 | `?x3d_D3DXVec4Hermite@@YGPAUD3DXVECTOR4@@PAU1@PBU1@111M@Z` | x3d_vec4.obj | no sub-tag: object d3dx9.lib:obj\i386\x3d_vec4.obj matched by no d3dx9 row |
| 0x0050ffc8 | `?x3d_D3DXVec4CatmullRom@@YGPAUD3DXVECTOR4@@PAU1@PBU1@111M@Z` | x3d_vec4.obj | no sub-tag: object d3dx9.lib:obj\i386\x3d_vec4.obj matched by no d3dx9 row |

## Files

`addrs.py` / `addrs.tsv` / `addrs_output.txt` / `create.tsv` / `inside.tsv` (list + byte check), `re/tools/ghidra_scripts/CreateLibFunctions.java`, `rehearsal_readonly.tsv`, `apply_list.tsv`, `apply_master.tsv` (master write), `tools/regen_pcfuncs.py`, `regen_output.txt`, `pcfuncs_added.csv` (pc_funcs), `rows_d3dx9.py`, `candidates_d3dx9.tsv`, `rows_d3dx9_dryrun.txt`, `rows_d3dx9_apply.txt`, `added_d3dx9.tsv`, `candidates_dshow.tsv`, `newrows_dshow_dryrun.txt`, `newrows_dshow_apply.txt`, `added_dshow.tsv`, `added_batch.tsv`, `dryrun_output.txt`, `dryrun_verdicts.tsv`, `c4_apply_output.txt` (rows + C4), `lint_before.txt`, `lint_after.txt`, `ladder_before.json`, `ladder_after.json`. Not committed: master dumps before/after (sha256 above), master backup `Mashed.rep.bak-20261108-pre-libfn`.
