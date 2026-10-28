# RESULT D5-RFX-EMITTERS (rfx particle emitters, unblocks D5-ICONCUBE-GLOW)

Branch `orch/D5-RFX-EMITTERS`. PREREG `verify/rfx_emitters_20261028/PREREG_D5-RFX-EMITTERS.md` (`eaeae62fd`,
committed alone). Tested `mashed_re.exe` SHA-256 `3437568a883fb4e356108bffd10392224577421d78706320027ae70768868079`
(built with `MASHED_NO_DEPLOY=1`, `.asi` not deployed).

**Verdict: GREEN.** A1: the original's bytes in Unicorn and the port's core agree on 93,472 of 93,472 dwords
(722 cases, 2,459 instanced particles), and a rounding-level mutant fails 26 of 74 instancing cases, so the
arm can fail. A2: G-CTRL and B equal to the merge_train.json references on all four arms.

## 1. What changed

- `mashedmod/src/mashed_re/Render/RfxEmitter.{h,cpp}` (new, exe-only, `exe_sources.rsp`, default SSE2 TU):
  the pure core. `Create` = `FUN_004770c0` (flag map, `FUN_00534b60` normalisation, plugin `+0`, `+4`, `+0x40`,
  `+0xa0`, `+0xa4..+0xac`, the matrix template `0x00692558`, the optional batch setters, per-flag arrays);
  the seven staging setters with their NULL defaults; the seven batch setters (`0x00476ae0` .. `0x00476cb0`);
  `Submit` = `FUN_00476d00`; `Flush` = `FUN_00476df0` (+ `FUN_00535700` / `FUN_00535910` lock bits);
  `SelectCallbacks` = `0x00536780`; `Instance` = the position instancers `0x00536e90` (size) and `0x00536fe0`
  (size + 2D rotate), colour `0x00537b20` / `0x00537d20`, uv `0x00537d70` / `0x00537e60`. Arrays are raw
  dwords, so every copy is a bit copy. `0x00536fe0`'s `fsincos` runs as inline x87 under PC_24 (CW saved and
  restored), the rest is float arithmetic in the disassembly's operation order.
- `LibRw/RwRaceFx.{h,cpp}`: `RaceFx_EmittersEnabled()` (`MASHED_RFX_EMITTERS=1`, default OFF, not in
  FaithDefaults) and `RaceFx_DrawEmitter(tank, camera)` = `FUN_004770a0` (draw only when plugin `+4` != 0),
  camera vectors `r = -LTM right`, `u = LTM up` (`0x005381c0..0x005381e2`), indices (0,1,2)(0,2,3), identity
  world matrix (fresh frame, `FUN_004c0b30` / `FUN_004e7e30`), the batch's texture, blend `+0xa4/+0xa8`
  (5/6 default from `[0x006132c4]`/`[0x006132c8]`) and vertex alpha `+0xac`; the caller's Z / cull / fog stand.
- **No caller.** Nothing in the build calls `Rfx::Create` or `RaceFx_DrawEmitter`; the first consumer is the
  pickup glow (D5-ICONCUBE-GLOW: `FUN_004587a0` create `0x817`/25/`puglow`, `FUN_00459000` submits in its state-1
  branch (colour call at `0x004591f8`) and its state-2 branch, one flush `FUN_00476df0(&DAT_0068b968)` after the
  pool loop, `FUN_00458b10` ZWRITE off / ZTEST on render).
- `re/tools/dual_copy_allowlist.txt`: 15 `CROSS-TARGET` rows (section 4).

## 2. Static facts (all from `original/MASHED.exe.unpatched`, decomp_pc.py + capstone)

| fact | where |
|---|---|
| game flag -> PTank flag 1->1, 2->4, 4->2, 8->8, 0x10->0x20, 0x20->0x80, 0x40->0x100, 0x200->0x8000, 0x400->0x20000, 0x800->0x80000, 0x1000->0x100000; base 0x10000000; flags 0 -> 0x807 | `FUN_004770c0` (decomp; 0x004770cc base 0x10000000, 0x004770d3 default 0x807) |
| puglow `0x817` -> PTank `0x10080027` (no normalisation change) | `FUN_00534b60` |
| `+0xa0` = the normalised flags for the ARRAY layout; the rebuild at 0x00534dce..0x00534fd5 runs only under 0x20000000, which `FUN_004770c0` never sets | `FUN_00534d00` 0x00534d6c, 0x00534dd0 |
| plugin `+0x40` = 0xffffffff at create | 0x00534d79 |
| data descriptor handed to every callback = plugin + 0x48 | 0x00534db8; lock offsets `FUN_00535700` |
| `0x00536fe0` args: a1 vertex out {ptr, stride 0xc}, a2 = desc +0x30 (-right), a3 = desc +0x3c (up), a4 count, a5 plugin+0x48 | call site 0x005366e2..0x005366f3; `FUN_00537fb0` 0x005381c0..0x005381e2 |
| rotated quad: s,c = fsincos(rot); R0 = (-s u) + (c r); U0 = (s r) + (c u); X = (R0 sx) 0.5; Y = (U0 sy) 0.5; v0 = (-X - Y) + p, v1 = (X - Y) + p, v2 = (Y + X) + p, v3 = (Y - X) + p | 0x00537042..0x005371d4 |
| unrotated quad (`0x00536e90`): X = (sx 0.5) r, Y = (sy 0.5) u, same four vertices; 0.5 = `[0x005cc32c]` | 0x00536ed0..0x00536fc2 |
| colour: RGBA bytes b0..b3 -> b3<<24 \| b0<<16 \| b1<<8 \| b2 on all four vertices | 0x00537b43..0x00537b68 |
| constant uv (`0x00537e60`): (u0,v1)(u1,v1)(u1,v0)(u0,v0) from plugin +0xd8; default (0,0,1,1) `[0x00613294]` | 0x00537e77..0x00537ebe; `FUN_00476c10` 0x00476c18 |
| setter NULL defaults: pos `0x006925a8` (BSS, 0 writers, so 0), size `0x00613288` (1, 1), colour `0x00613290` (0xffffffff), matrix `0x00692558` (template), uv2 `0x00613294`, uv4 `0x006132a4` (0,1,1,1,1,0,0,0) | 0x004769a8, 0x004769d8, 0x004769f8, 0x00476a1a, 0x00476a48, 0x00476a88 |
| puglow rotation input = entry angle * `[0x005cd7a8]` (0x3c8efa35 = 0.017453292) | `FUN_00459000` state-1 branch (decomp line `FUN_00476a30(pfVar4[-0x1a] * _DAT_005cd7a8)`; instruction address not read) |

Colour byte order: the D3DCOLOR `FUN_00537b20` packs is drawn as is (R = byte 0, B = byte 2). Evidence: the
pickup type-19 glow, dword `0xffff0000`, is blue on the original's capture
(`re/analysis/PICKUPS_LOOK_PLACEMENT_2026-09-29.md` section 1.1). The packed value goes into the array
`FUN_00537fb0` locks with `FUN_004e89e0(geo, 8)` (0x005382a5); how the D3D9 path consumes it is not
transcribed, so the blue capture is the evidence, not the lock.

## 3. Arms

### A1: original bytes in Unicorn vs the port core (`emit_unicorn.py`, `emit_port.cpp`)

Run: `py -3.12 verify/rfx_emitters_20261028/emit_unicorn.py D:/MashedWT/D5-RFX-EMITTERS_data/emit_port.exe D:/MashedWT/D5-RFX-EMITTERS_data/a1`
(port exe built by `build_port.bat`, `cl /O2` x86). Log `A1_run.txt`.

| set | cases | content | mismatching cases |
|---|---|---|---|
| C1 create | 262 | the 6 caller flag words (0x807, 0x80c, 0x817, 0x427, 0x895, 0) + 256 random 13-bit words, cap 1..300: 72 dwords each (struct, array sizes, plugin fields, material colour, template) | 0 |
| C2 sequences | 240 | 10..60 random setter / NULL-default / submit / flush ops, cap 1..8, submits past cap; emitter + PTank arrays after | 0 |
| C3 selector | 146 | every flag word C1 produced + 64 random dwords; 6 callback addresses each, 8 distinct position callbacks reached | 0 |
| C4 instancing | 74 | 2,459 particles: puglow `0x10080027` x40, `0x10080007` x10, `0x10000007`, `0x10000087`, `0x10080005` (constant colour), `0x10080025` x6 each; random p, size, rot (0 every 7th, else [-12.6, 12.6] rad or deg * 0.017453292), colour, uv, camera vectors | 0 |

`orig.txt` and `port.txt` are byte-identical (same SHA-256). Raw files (not committed):

| file | SHA-256 |
|---|---|
| `D:/MashedWT/D5-RFX-EMITTERS_data/a1/cases.txt` | `5b20dc6d2f01edf6262782f161609622f53b7a7a103a71fab4f0bc1033aebd6b` |
| `D:/MashedWT/D5-RFX-EMITTERS_data/a1/orig.txt` | `fdaf8b6a2db7d51804ea7a0c76d5c3acd12a1e3197308c95408d5843d9b1bce5` |
| `D:/MashedWT/D5-RFX-EMITTERS_data/a1/port.txt` | `fdaf8b6a2db7d51804ea7a0c76d5c3acd12a1e3197308c95408d5843d9b1bce5` |
| `D:/MashedWT/D5-RFX-EMITTERS_data/emit_port.exe` | `403c9f260e9fddf6b34298f93d08882ff5d611496c8e1af81cd6462cfe0a1b71` |

**Negative controls** (not in the PREREG, added after the clean first run):
- libm `std::sin` / `std::cos` in place of the PC_24 `fsincos` (`negctl_mutate_libm.py`): **0 mismatches**
  (`A1_negctl_libm.txt`). On these ~2,100 non-zero angles the correctly rounded float sin / cos equal the
  x87 result, so this control cannot fail and proves nothing. Recorded, not counted.
- the six rotated-axis sums in double (`negctl_mutate_double.py`): **26 of 74 instancing cases fail**, first
  difference 1 ulp (`43213476` vs `43213477`, case 649) (`A1_negctl_double.txt`, port.txt SHA-256
  `a348f37beda70dd4a7e47053c61554c4c5b5b2f60e3c40cd5653f4aa199a812f`). A1 sees a one-ulp error.

What A1 does not cover: `FUN_00534d00` is stubbed (its effect is the six fields cited above), RwMalloc,
`FUN_004e8090`, `FUN_004c0b30`, `FUN_004e7e30` are stubbed, and `FUN_00537fb0` / the librw draw wrapper are
not executed. The emulator copies of `FUN_00534b60`, the setters, submit, flush, lock / unlock, selector
and the six callbacks run verbatim.

### A2: default gates (`verify/savelink_20261023/run_arms.py`)

| arm | file | lines | sha8 | reference | |
|---|---|---|---|---|---|
| H_DEF | step.csv | 14659 | 5943c5f1 | 5943c5f1 | equal |
| H_DEF | gates.csv | 56873 | 364350ae | 364350ae | equal |
| H_DEF | ptrace.log | 28436 | 52052890 | 52052890 | equal |
| B_DEF_K0 | step.csv | 1459 | 3a03670c | 3a03670c | equal |
| B_DEF_K1 | step.csv | 1825 | 9fcdbd3a | 9fcdbd3a | equal |
| B_DEF_K26 | step.csv | 1411 | 53778664 | 53778664 | equal |

All four self-exited (exit 0), exe `3437568a`, private root `D:/MashedWT/D5-RFX-EMITTERS_root`, under
`mashed_lock.py run --label D5-RFX-EMITTERS` and test-slot. Log `RUNLOG_gates.txt`; arm files in
`D:/MashedWT/D5-RFX-EMITTERS_data/arms`. After the build only comments changed in `Render/RfxEmitter.{h,cpp}`
(three address citations reworded), so the committed source builds the same code.

## 4. The CROSS-TARGET allowlist rows (lead decision)

`lint_rva_bodies.py` failed the first build: the `.asi` already has C3 hook bodies for 15 of these RVAs, on
the original's raw globals and batch struct (`Render/FrameHelpers_q2.cpp`: `0x004769a0` `0x004769d0`
`0x004769f0` `0x00476a30` `0x00476a40`; `Util/PromoLoop_sessionB.cpp`: `0x00476a10` `0x00476a80` `0x00476ae0`
`0x00476b30` `0x00476b90` `0x00476be0` `0x00476c10` `0x00476c60` `0x00476cb0` `0x00476d00`). The exe core
keeps its own storage (Staging struct, vectors, PTank by value), so it is a second body per RVA. The lint's
documented resolution for a deliberate divergence judged against the original is an allowlist row with a
citation, so the branch adds 15 rows citing A1. The allowlist is the D4 burn-down list, so this grows it.
Options for the lead:
- (a) accept the 15 rows (recommended for now: the exe body is judged dword-exact against the original's
  bytes, and nothing calls it yet), with a follow-up row to consolidate;
- (b) consolidate before merge: rewrite the core onto the original's layouts (the 0x40-byte batch struct with
  array pointers, the plugin block by offset, the staging globals as one struct at `0x006924d8`), compile it
  into both targets, rebase the 15 `.asi` hooks onto it, and re-run their Frida diffs (original boot needed).

## 5. Not claimed, follow-ups

- No C-level change and no tracker edit. Candidate re-classify transaction for the lead: set `exe_file` =
  `mashedmod/src/mashed_re/Render/RfxEmitter.cpp` on the 15 rows above plus `004770c0`, `00476df0`, `004770a0`;
  new rows for `00534b60`, `00536780`, `00536e90`, `00536fe0`, `00537b20`, `00537d20`, `00537d70`, `00537e60`
  (the last six are Ghidra-missed functions, reached only through the selector).
- Not ported (Instance returns false, nothing drawn, one stderr line): position `0x00536940` `0x00536ad0`
  `0x00536c90` `0x005371f0` `0x00537460` `0x00537610` `0x00537890` (matrix, rfx2's DrawBatch carries it ad hoc)
  `0x005379c0`; colour `0x00537b80` `0x00537c40`; uv `0x00537de0` `0x00537ed0`; normal `0x00537f40`; the
  bounding block of `FUN_00537fb0` (0x00537fd4..0x0053807b); `FUN_00534d00`'s ARRAY allocation (0x0053510c..).
  The constant-colour path needs `FUN_00476b30` to have run (its initial `+0xc4` is not transcribed).
- No original boot was needed. An audio endpoint was present today (`Philips FTV (NVIDIA High Definition
  Audio)`, `Get-PnpDevice -Class AudioEndpoint -PresentOnly`), so D5-ICONCUBE-GLOW can try an original capture.
- D5-ICONCUBE-GLOW: wire `FUN_004587a0` / `FUN_00459000` / `FUN_00458b10` through `Rfx::` behind
  `MASHED_RFX_EMITTERS`, compare to `training_orig_row_00884.draws.json` event 225 (PUGlow 128x128, n
  [20, 10], rs [1,0,4,1,0,5,1,5,6,2,0,0], tss [4,4,1], fvf 322), retire the PickupField orbs.
