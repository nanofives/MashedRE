# RESULT RO-CTL-OSWORD (run 50, 2026-11-16)

Worker cmwikzzdy1thxp015tw642ijs, lead cmwikyu1t1t3vp015u1ze18rm. Branch `orch/RO-CTL-OSWORD` from `orch/integration`
`9a44eee36`. Row kind: tool and text row (original-side static reads plus a tool change scored on existing captures). No build,
no game run, no G-CTRL, no B, no 12 captures, no tracker or hooks.csv change. No reference race is named (L-167 is open).

## 0. Verdict: GREEN

1. `re/frida/detorig_ticks.py --masks` now drops MASK dwords per dword, not only whole-1 KB-masked blocks. Three routes, each named
   in the report: `full` (the old rule), `raw` (per tick, on blocks the `--tick-raw` ranges cover whole) and `dump` (from the
   checkpoint dumps both runs carry at the same tick). `--block-masks-only` keeps the old rule. On the 10 plain pairs of
   RO-REF-CONTROL-NOISE, `0x00614400` (`0x006147d8`) is no longer counted in any pair and `0x00771800` (`0x00771a00`) in 8 of 10.
2. `0x00771a00` was already `MASK:os` in `replay_masks_manual.tsv` / `replay_masks.tsv` / `warp_normalize.tsv` (RO-MASKS,
   `e116c666b`), with the note "flag written next to 0x006147d8". The disassembly shows it is the same state as `0x006147d8`
   (section 2). The note now cites that. Its class stays `MASK:os`.

## 1. The tool change

`detorig_ticks.py` (header comment lists the routes):

- **raw.** For each block whose 256 dwords all lie in the run's `ticks.raw` ranges, at each tick where the block hash differs the
  differing dwords are read from the raw words with the hash's heap rule (a dword in `[0x00995000, 0x30000000)` with the low two
  bits clear counts as 0, `js/detorig.js` onEnter and `detorig_firstdiv.block_hashes`). A tick whose differing dwords are all MASK
  drops the block at that tick only, so the block can still be NEW at a later tick for an unmasked dword.
- **raw/hash disagreement.** At some ticks the block hash of `0x00614400` differs while no raw dword of that block differs (example
  c1:c2 ticks 531, 587, 591: raw `0x006147d8` 1/1 at 531, hash differs). The hash pass and the raw copy run one after the other in the
  same CModule listener (`re/frida/js/detorig.js`, onEnter: block loop, then `rawCopy`), so a dword written between the two by
  another thread is seen by one and not the other. The tool drops such a tick only when every dword of the block that differs at
  any tick of the pair is MASK (reported as `raw_race`). Otherwise the block is kept and listed under `raw_inconsistent`.
  [UNCERTAIN] which thread writes `0x006147d8`. The evidence is only the disagreement (in all 10 plain pairs, from 3 ticks in c1:c2 to the
  20-entry cap of the report in 6 pairs, `osword_pairs.json` `raw_race`). Missing: a thread-id watch on `0x00493eeb` / `0x0049479a`.
- **dump.** For a NEW block the raw ranges do not cover, the full-image dumps both runs hold at the same tick u (original
  checkpoints k >= 200, exe k != 100, as `detorig_firstdiv.load_run`) are re-hashed against record u - 1 first and used only when
  both match. The block is dropped when at every such checkpoint where its hash differs only MASK dwords differ, and at least one
  of those checkpoints is at or after its NEW tick. A dword that differs only between two checkpoints is not seen, so this route is
  weaker than raw. Kept blocks list their unmasked dwords under `dump_unmasked`.
- **mask files are a union.** `detorig.load_norm` keeps the first file's class per dword. With the usual order
  `warp_normalize.tsv,replay_masks.tsv`, `0x007e95a0..0x007e95bb` is `RULE:WINMSG` (warp_normalize.tsv line 42) and hides the
  `MASK:os` row of `replay_masks_manual.tsv` for the same MSG words. The tick tool now takes a dword as masked when any listed file
  gives it a `MASK:` class. Class names elsewhere (detorig_report) are unchanged.
- New report fields: `raw_masked_blocks`, `raw_inconsistent`, `raw_race`, `dump_ticks`, `dump_unmasked`. `masked_blocks` rows are
  now `[va, tick, route(, checkpoint evidence)]`. No script in the repo reads `masked_blocks` (grep).

## 2. `0x00771a00` and `0x006147d8`: one state (static read, `osword_disasm.txt`)

Binary `original/MASHED.exe.unpatched`, SHA-256 `bdcae093...3c0e` (the anchor). Disassembly by capstone
(`osword_rvadis.py`):

- `0x00493edf` `mov eax, [0x771a00]`, `0x00493ee4` `xor ecx, ecx`, `0x00493ee6` `test eax, eax`, `0x00493ee8` `sete cl`,
  `0x00493eeb` `mov [0x6147d8], eax`, `0x00493ef0` `mov [0x771a00], ecx`. So `0x006147d8` := old `[0x771a00]` and
  `0x00771a00` := 1 if old was 0, else 0.
- `0x004944e7` `xor ebx, ebx` in FUN_004944c0, no other write to ebx/bl/bh/bx up to `0x00494794` (instruction scan of
  `0x004944c0..0x00494794`; calls in between are taken as preserving ebx per the MSVC calling convention [UNCERTAIN], not
  stepped). `0x00494794` `mov [0x771a00], ebx` (0), `0x0049479a` `mov dword [0x6147d8], 1`.
- `0x00493c90` `mov eax, [0x771a00]`, `0x00493c9b` `mov ecx, [eax*4 + 0x771a10]`: `0x00771a00` indexes a dword array at
  `0x00771a10`. `0x00493cb9` and `0x00493eb7` read the same element and call through its first dword (`call [ecx+0xc]`
  `0x00493cb6`, `call [edx+0x50]` `0x00493ec9`). No meaning is assigned to the array.

Measured: in all 154 checkpoint dumps of c1..c5, cw1, cw2, `[0x00771a00] + [0x006147d8] == 1` in 152
(`osword_invariant.txt`: (0,1) 77, (1,0) 75). The two others are (0,0) in c2 k 5 and (1,1) in c4 k 13, both mid-frame checkpoints
(k < 200). [UNCERTAIN] that these two are dumps taken between the stores at `0x00493eeb` and `0x00493ef0`. Only consistent with
it, not shown.

So `0x00771a00` is `1 - 0x006147d8` after either writer pair, and it flips whenever `0x006147d8` does. It stays `MASK:os` with the
same writers. Only the note text changed (`replay_masks_manual.tsv` and the same line in `replay_masks.tsv`).

`replay_masks.tsv` was edited on that one line, not regenerated: `gen_replay_masks.py` on this head also adds
`0x00914700 MASK:audio` (the P2 refile drift RO-REF-CONTROL-NOISE reports, `870f7a5f8`), which is not this row's change.

## 3. Before / after on existing captures

`osword_pairs.py` runs `detorig_ticks.py` with `--masks re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv` on the
RO-REF-CONTROL-NOISE runs (`D:/MashedWT/RO-REF-CONTROL-NOISE_data`, hashes in that row's `raw_sha256_RO-REF-CONTROL-NOISE.txt`),
before = `--block-masks-only`, after = this row. Full rows: `osword_pairs.json`. Log `D:/MashedWT/RO-CTL-OSWORD_pairs.log`
(SHA-256 `866825bdd827339eaa11d66f47e5244e59b5d8a2fa9224d32c34295bca84a6cf`).

IDENTICAL-FOR-N of 600, with N alignment required / with `--n-ok`:

| pair | before | after | before n-ok | after n-ok | still counted after |
|---|---|---|---|---|---|
| c1:c2 | 0 | 0 | 1 | 300 | `0x007f1000` @300 |
| c1:c3 | 0 | 0 | 1 | 300 | `0x007f1000` @300 |
| c1:c4 | 0 | 0 | 1 | 1 | `0x00771800` @1, `0x007f1000` @300 |
| c1:c5 | 1 | **600** | 1 | **600** | none |
| c2:c3 | 0 | 0 | 2 | 300 | `0x007f1000` @300 |
| c2:c4 | 0 | 0 | 2 | 300 | `0x007f1000` @300 |
| c2:c5 | 0 | 0 | 9 | 300 | `0x007f1000` @300 |
| c3:c4 | 2 | **600** | 2 | **600** | none |
| c3:c5 | 0 | 0 | 2 | 2 | `0x00771800` @2, `0x007f1000` @300 |
| c4:c5 | 0 | 0 | 2 | 300 | `0x007f1000` @300 |
| c1:cw1 | 0 | 0 | 1 | 300 | `0x007f1000` @300 |
| c1:cw2 | 1 | 301 | 1 | 301 | `0x007f0c00` @301, `0x00771800` @306 |
| cw1:cw2 | 0 | 0 | 3 | 301 | `0x007f0c00` @301, `0x00771800` @305 |

What moved:

- `0x00614400` is dropped in all 13 pairs (route raw plus `raw_race`, only `0x006147d8` differs).
- `0x00771800` is dropped by route dump in 8 of 10 plain pairs, only `0x00771a00` differing at the checkpoints (example c1:c2:
  checkpoints 17, 18, 299, 301). In c1:c4 and c3:c5 it is kept: the block is NEW at tick 1 / 2 but equal at every checkpoint at or
  after that tick (17, 18, 299, 301, 600), so no checkpoint shows which dword moved. In c1:cw2 and cw1:cw2 it is kept for
  `0x00771998..0x007719a0` at checkpoint 17 (part 4 of RO-REF-CONTROL-NOISE, the +9000 mode, not masked).
- `0x007d7000` @300 is dropped by route dump in the 5 pairs where it was counted (dwords `0x007d70c8`, `0x007d7108`, both MASK).
- c1:c3 `0x007e9400` @8 is dropped by route dump (`0x007e95a0`, `0x007e95b0`, the PeekMessageA MSG words): the union rule. With
  the old first-file rule it stayed counted (checked before the union change: c1:c3 after n-ok was 8).
- Still counted: `0x007f1000` @300 (`0x007f1010` at checkpoint 301, part 3 of RO-REF-CONTROL-NOISE, the N misalignment at the warp)
  and `0x007f0c00` @301 in the watched pairs. Neither involves a MASK dword.

RO-REF-CONTROL-NOISE got 2 of 10 plain pairs at 600 with `--ignore` of both whole blocks plus `--n-ok`. This row gets the same 2
with masks alone (with or without `--n-ok`), and c1:c3 moves from 8 to 300 under `--n-ok`.

## 4. Not done here

- `detorig_firstdiv.py` keeps its own whole-1 KB rule for the record-level first divergence (its header, point 1). Not changed.
- c1:c4 and c3:c5 need `0x00771800` raw-covered to be dropped exactly: adding `0x00771800-0x00771c00` to the reference recipe's
  `--tick-raw` would put that block on route raw. That is a harness recipe change for the lead (og_arms.sh / ctl_arms.sh).

## Files

- `re/frida/detorig_ticks.py`: per-dword routes, union of mask files, `--block-masks-only`.
- `re/frida/replay_masks_manual.tsv`, `re/frida/replay_masks.tsv`: the `0x00771a00` note.
- `verify/ro_20261115/osword_pairs.py`, `osword_pairs.json`: the before / after.
- `verify/ro_20261115/osword_rvadis.py`, `osword_disasm.txt`: the static read.
- `verify/ro_20261115/osword_invariant.txt`: the dump check of section 2.
