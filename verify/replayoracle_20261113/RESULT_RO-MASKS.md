# RESULT RO-MASKS (2026-11-13): per-dword mask file for the exe-vs-original compare

Verdict: GREEN (text/tooling row: no build, no game run, no gates).

## What landed
- `re/frida/replay_masks.tsv` (generated, 402 lines): 388 spans of bound-family dwords at address level (audio 126, directshow 2, crt 143, lua 6, renderware 266, d3dx9 179, qhull 838 dwords) plus the manual rows. Same columns as `warp_normalize.tsv`.
- `re/frida/replay_masks_manual.tsv` (hand source, 9 rows): OS words and stale-stack fields, each with address, writer and reason. New from RO-CONTROL-DRIFT (RESULT on orch/RO-CONTROL-DRIFT @ 22695e174): `0x00773c40` (0x104 B exe path, differs per private root) and `0x00773d60` (kernel32 pointer, differs per boot), both `MASK:os`. Also restated: MSG `0x007e95a0` (28 B), `0x006147d8`, `0x00771a00`; `MASK:stale-stack` for ring `0x007ec9e4` fields +0x28..+0x3c (`0x007eca0c`, 256 x 0x40, 0x14 B) and the RwMatrix pads m[7] m[11] m[15] (`0x006fe7dc` / `ec` / `fc`, 150 x 0x7c).
- `re/tools/gen_replay_masks.py`: regenerates the file. Attribution as in `data_owner_map.py` (absolute-operand scan of .text, function by Ghidra facts, subsystem by hooks.csv) but per address. A dword is masked only when every referrer is of one bound family (crt, renderware, d3dx9, directshow, qhull, lua, audio). 331 dwords are referenced by a bound family AND game code: not masked, listed in `masks_shared.tsv` here. Static scan: pointer-register struct bases are not seen (same limit as data_owner_map).
- Consumers: `detorig.load_norm` takes comma-separated files (earlier file wins per dword, so list `warp_normalize.tsv` first). `detorig_report.py --norm A,B` scores the MASK rows as `MASK:<family>`. `detorig_ticks.py --masks A,B` ignores NEW blocks whose 1 KB is entirely masked (reported as `masked_blocks` in `--json`) and feeds the masks to `--explain`.

## Not masked, by design
- Boot residue: `warp_normalize.tsv` pins it on both sides (PIN rows), so no mask row.
- RenderWare-Physics, libpng, zlib families: not in the design's bound list, left out.
- Dwords reached only through a register base: not seen by the static scan. [UNCERTAIN] how many of the family dwords this misses; evidence needed: a dump diff with a dword outside the file that belongs to a bound family.

## Check (dry run over existing dumps, D:/MashedWT/REPLAY-ORACLE-DESIGN_data)
- `detorig_report.py n_a n_b --norm warp_normalize.tsv,replay_masks.tsv`: parses, normalized 15/15, race-state 15/15, unexplained 0 at every checkpoint, same classes as with `warp_normalize.tsv` alone.
- `detorig_ticks.py n_a n_b --masks ... --explain`: parses; NEW block 0x614400 classes `MASK:os`. `t2_a t2_b --masks ... --json` writes `masked_blocks` (none fully masked on that pair: blocks 0x7dc400 / 0x7dd000 / 0x614400 are partly masked, so the dword-level classification is what applies there).
