# RESULT RO-MASKS-REGEN + RO-CTL-PAD-68BC (run 50, 2026-11-15)

Branch `orch/RO-MASKS-REGEN` from `f55abfff3` (orch/RO-CTL-OSWORD). Row kind: tool + text (mask table). No build, no game run, no G-CTRL, no B.
No reference race is named (L-167 open). Files changed: `re/frida/replay_masks_manual.tsv` (+3 rows), `re/frida/replay_masks.tsv` (regenerated).
The OSWORD `0x00771a00 MASK:os` note is present once in each file and is not in the diff (`grep -c 771a00` = 1 / 1).

## 1. RO-MASKS-REGEN

Command: `py -3.12 re/tools/gen_replay_masks.py --facts C:/Users/maria/Desktop/Proyectos/Mashed/log/ghidra_funcfacts.jsonl`
(facts SHA-256 `8ee4c0d8c032565325553332556abb2358bb06ec384f75900382fa135377f057`, the same as RO-REF-CONTROL-NOISE section 6).
Output `gen_masks.out`: audio 127, directshow 2, crt 143, lua 6, renderware 266, d3dx9 179, qhull 838 dwords; 389 spans.

Mask diff before the manual pad rows (regenerate alone), line by line:

| line | change |
|---|---|
| header count | `1560 dwords in 388 spans` -> `1561 dwords in 389 spans` |
| `0x00914700 1 0 4 MASK:audio` | NEW |

Nothing else differs, so the OSWORD row and every other row regenerate unchanged.

Reader evidence for `0x00914700` (absolute-operand referrers, RESULT_RO-REF-CONTROL-NOISE section 6, `mask_referrers_00914700.txt` on
orch/RO-REF-CONTROL-NOISE): FUN_005a6190 `0x005a6199` and `0x005a61a9` (writes), FUN_005ac210 `0x005ac2ec` (read), FUN_005b8200
`0x005b823d`, `0x005b82e3`, `0x005b83e2`, `0x005b8466`, `0x005b853d` (reads). All three functions are `audio` in `hooks.csv` at this base
(FUN_005b8200 was `third-party-library[d3dx9-fx]` at e116c666b, refiled by 870f7a5f8 / 3a86c8469). Generator rule: `gen_replay_masks.py` lines
90-93 mask a dword when every referrer maps to one bound family; line 30 maps prefix `audio` to family `audio`. Value `0x00000010` (16) in
every control run at ticks 1 and 600, so the mask changes no verdict on the existing runs.

**Does a refile that moves mask membership need a regenerate step: yes.** Reason: the file is a pure function of `hooks.csv` subsystem
plus the Ghidra facts (gen_replay_masks.py lines 47-51, 86-93), so any change to a function's subsystem prefix across the `BOUND` list
(lines 27-30), in either direction, changes it, and the refile had no step that reruns the generator (the P2 refile 870f7a5f8 left the file
stale for 2 days until RO-REF-CONTROL-NOISE noticed). Where it belongs: the P2 refile class (L-127) in the refile row's checklist, as one
command, `gen_replay_masks.py --facts ... ` and a commit of the diff when non-empty, in the same commit as the `hooks.csv` subsystem change
(the same train key-merge applies to `hooks.csv`). Narrower trigger: only refiles with `audio`, `crt`, `renderware`, `d3dx9`, `directshow`,
`qhull`, `lua` on either side. No automation added. [UNCERTAIN] whether a mask drop (membership removed) also needs a human look: not
observed here.

## 2. RO-CTL-PAD-68BC: decision, mask the pad family (12 words)

Mask: three rows in `replay_masks_manual.tsv`, `count 4, stride 0x68, width 4, MASK:stale-stack`, at `0x0068bba4`, `0x0068bbb4`, `0x0068bbc4`
(regenerated into `replay_masks.tsv`). Class `stale-stack` follows the existing row `0x006fe7dc` ("RwMatrix pad m[7], stale stack", writer
`0x004863e4`, a `rep movsd` of an RwMatrix).

Addresses (RESULT_RO-CTL-68BC0C section 1): record array `DAT_0068bb60[4]`, record size 0x68, `mat` (16 dwords) at record +0x28, written
by the `rep movsd` at `0x0045a736` in FUN_0045a6b0 (`lea edi,[ebx+0x28]` `0x0045a729`, `mov ecx,0x10` `0x0045a72c`, loop `0x0045a6dd`
(`mov ebx,0x68bd00`) .. `0x0045a768` (`cmp ebx,0x68bb60`)). Record n mat base = 0x0068bb88 + n*0x68. Dwords 7, 11, 15 of `mat`:

| record | dword 7 | dword 11 | dword 15 |
|---|---|---|---|
| 0 | 0x0068bba4 | 0x0068bbb4 | 0x0068bbc4 |
| 1 | 0x0068bc0c | 0x0068bc1c | 0x0068bc2c |
| 2 | 0x0068bc74 | 0x0068bc84 | 0x0068bc94 |
| 3 | 0x0068bcdc | 0x0068bcec | 0x0068bcfc |

Observed values: og1 holds `0x0068bc2c` = 4 (signed 4) at every record, 0 in 8 other runs (RESULT_RO-REF-CONTROL-NOISE section 5). Record 1
dwords 7 and 11 held 4 / a pointer-range value in 160 of 411 runs, all in the 2026-11-11 PROV-INSITU window (RESULT_RO-CTL-68BC0C section 3).
The three pad dwords of `mat` hold other data in no run: dword 0 / 5 / 10 = 0x3f800000 (1.0f), dword 3 = 3, same in every run, and are
not masked.

Reader evidence:
- Live: `brw2` (read-or-write watch on `0x0068bc0c` and `0x0068bc1c`, PE entry to race end): no pc inside MASHED.exe read either word,
  only the write at `0x0045a736` (RESULT_RO-CTL-68BC0C section 6).
- Static (`pad68bc_scan.py`, `pad68bc_scan.out`, MASHED.exe.unpatched): the only memory operand with a displacement in `0x0068bb60..0x0068bcff`
  is `0x0045a89d mov edx,[ecx+0x68bb60]` (offset +0, record start). Immediates in that range: `0x00459311`, `0x004593cd` (0x68bb90),
  `0x0045a140` (0x68bbac), `0x0045a768`, `0x0045a791` (0x68bb70), `0x0045a7d2`, `0x0045a85d`, `0x0045a8bf`, `0x0045a91d`, `0x0045abd8`
  (0x68bb70), `0x0045ac0b` (0x68bb84), `0x0045ac32`, `0x0045af07` (0x68bb64), `0x0045af18`. None names a pad address.

Limits, stated as missing evidence:
- [UNCERTAIN] pad words of records 0, 2, 3 and `0x0068bc2c`: never watched for reads. Static scan sees no direct pad address, but a
  register-based `[reg+0x44]`, `[reg+0x54]`, `[reg+0x64]` access with `reg` = record pointer would not show in it. Test row: one `rw` watch run
  (`detorig.py --watch-mode rw`) on the 9 other pad addresses plus `0x0068bc2c`, then drop the unproven rows if a reader runs.
- [UNCERTAIN] the source of the nonzero value (frame `+0x50 + 0x1c / 0x2c / 0x3c` of the cloned atom's frame, via FUN_004c0ed0 return
  param_1 + 0x50), unchanged from RESULT_RO-CTL-68BC0C section 3.
- Reason to mask all 12 and not only record 1: the one loop at `0x0045a6dd..0x0045a768` writes all four records with the same instruction
  `0x0045a736`, so the pad words share one writer, and the existing `0x006fe7dc` row takes the same class for the same field.
- The class label `stale-stack` is taken from that precedent. This RESULT does not show that the source pad holds stack contents.
- Masks apply in `detorig_firstdiv` / `detorig_ticks` via `replay_masks.tsv` (verified: `detorig.load_norm` returns MASK for
  `0x0068bba4`, `0x0068bc0c`, `0x0068bc2c`, `0x0068bcdc`, `0x0068bcfc` and `0x00914700`, and None for `0x0068bc10`, `0x0068bc28`).
  `prov_compare.py` keeps its own two-word mask, unchanged.
- No oracle re-score was run (needs captures). Predicted effect, not measured: og1 no longer reports block `0x0068bc00`.

## Files

Committed in `verify/ro_20261115/`: this RESULT, `pad68bc_scan.py`, `pad68bc_scan.out`, `gen_masks.out`.
Raw data not used by this row. Facts file `log/ghidra_funcfacts.jsonl` (main checkout) SHA-256 above.
