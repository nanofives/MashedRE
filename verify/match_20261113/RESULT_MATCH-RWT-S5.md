# RESULT MATCH-RWT-S5 (2026-11-13)

Row: compiler shaping of 0x004dcbb0 (`re/match/rwl01/004dcbb0.c`) toward a byte match. Match-only: no build, no gates.
Model: Opus 5.5, high effort (second Opus-high attempt on the S3-corrected source). Branch `orch/MATCH-RWT-S5` from
`orch/integration`.

**Verdict: RED (not matched). 62.0% -> 83.2%** (regs 96.6%, ops 97.5%, 119/119 ins), identical under `ddk`
(cl 13.00.9176 / c2 13.00.9178) and `rw9044`, flags `/O2 /Ob1 /G5`. No C4 claim, no promotion TSV. The committed source
computes the same function as the S3 source (each change reads the same value, see "Semantics").

## Levers that worked

1. **Per-pass reload of param_1 in the search loop** (62.0 -> 82.4). In the loop body:
   `a = *(volatile uint *)&param_1; uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;`.
   c2 allocates per web, so this takes param_1's web out of the loop. Result, matching the original:
   param_1 loaded into ecx before the pushes (`mov ecx,[esp+4]` at 0x004dcbb0), used for the size multiply and the first
   round-up, the loop counter takes ecx (`xor ecx,ecx`), the loop reloads `[esp+0x14]` each pass (0x004dcc00), and the
   tail divides by memory (`div dword ptr [esp+0x14]` at 0x004dccf2) with plain `param_1` in the source. A volatile read
   in the tail as well is not needed (same score with and without).
2. **Hoisted inner-loop key** (82.4 -> 83.2): `key = DAT_007d70ec[idx];` before `for (pn = DAT_007d70c0; ...)`, compare
   `pn[2] == key`, together with `DAT_007d70dc[DAT_007d70c8]` in the first check (c2 forwards the global to idx). The
   original loads the key before the null test (`mov ecx,[ebp*4+0x7d70ec]`, after `mov eax,[0x7d70c0]`).

## Exact missing lever

**size / end register priority.** All 20 remaining wrong instructions but 3 are one swap: ours size (uVar7) = ebx and
end (uVar5) = edi, the original size = edi (0x004dcbcb `mov edi,ecx`, `imul edi,[esp+0x18]`) and end = ebx
(`lea ebx,[esi+edi]`, store `mov [edx*4+0x7d70cc],ebx`). The reload temp follows end (original `mov ebx,[esp+0x14]`).

Mechanism, measured with synthetic probes (`syn.py`, `syn1.py`, `syn2.py`): c2 colours webs in priority order (weighted
references, ties by first definition) on esi, edi, ebx, ebp. In our source the reload temp `a` is a web with four
references inside the loop, so it outranks size and takes edi. end does not interfere with `a`, so end lands in edi too,
and size drops to ebx. In the original the reload is not a web (a spill reload placed just before its first use, after
`mov edx,[ecx*4+0x7d70cc]`) and lands in end's dead ebx.

Diagnostic (`space13.py`, changes code, not a candidate): two extra stores of size after the tail move size to edi and
`a`/end to ebx (90.9% with the two extra instructions). One extra store is not enough (81.7%). So size needs about two
more weighted references than it has, or `a` about two fewer, with no code change. Nothing found does that:

- recomputing `param_1 * param_2` at any one or two use sites of size (8 sites, 36 combinations, `space14.py`): c2's CSE
  canonicalizes to the same IL, 83.2 every time;
- moving the end compare out of its variable (`if (uVar6 + uVar7 <= ...) { uVar5 = ...`, `space4.py`, `space15.py`) is
  canonicalized, or (y1 form) drops end below idx (size edi, but end and `a` in ebp);
- the reload temp merged with other variables (Lock result, key, piVar2, the first-check copy, `space7.py`): webs are
  split, no change; labels / gotos around the reload (`space12.py`): no change;
- idx-reference reductions (global reads in the first check, key, FUN argument, loop compare, increment path,
  `space15.py`): no gain.

Second open spot (3 instructions): the found path. Original `mov ebp,ecx; cmp ecx,4; mov [0x7d70c8],ebp`
(0x004dcc27..0x004dcc2c) stores from idx; ours `cmp ecx,4; mov ebp,ecx; mov [0x7d70c8],ecx`. Six store / compare forms
(`space5.py` d0..d5) give the same listing.

Next step for a third attempt: the spill-reload form means the original param_1 was the lowest-priority web. Find the
source form that makes c2 itself split param_1 (no volatile temp). Every plain-param_1 form tried keeps param_1 in a
register through the loop and spills end instead (62.0 / 62.8), including address-taken forms that emit no code
(`space8.py`: `&param_1 == 0`, `(void)&param_1`, dead pointers, self-assign: param_1 stays enregistered),
struct / union / array-by-value param (`space10.py`: scalarized), inline round-up helpers (`space9.py`),
signedness of params and locals (`space11.py`, 80 combinations), goto-built loop (`space6.py`: same as do/while).
Fable gate 1 now applies to this row (two Opus-high attempts on the corrected source, S3 and this one).

## Other levers tried (no gain)

- Global `DAT_007d70c8` used everywhere in place of idx (`v1.py`): 37.8 / 44.5, the loop compare goes to memory.
- Declaration order of uVar4..uVar7 (24 orders), `register`, block vs function scope of the reload temp, the reload
  value assigned into uVar5 or into param_1 itself: all 82.4 / 83.2 or lower.
- Volatile reads at every loop use (`tvolall` 57.9), only the first loop use (`lv4` 50.8), param_2 as the size variable
  (`v8.py`), `(&param_2)[-1]` address forms (30.3, c2 materializes the address).
- Bounded permuter (`re/tools/match/match_permute.py`): 733 compiles from the 82.4 source (4 rounds, walk 80) and 1003
  compiles from the 83.2 source (6 rounds, walk 150): no improvement (`perm1_log.txt`, `perm2_log.txt`).

## Semantics

Changes against the S3 source (`MATCH-RWT-S5_probes/s3_004dcbb0.c`, frozen copy):
- loop reload `a = *(volatile uint *)&param_1`: the same value as param_1 (param_1 is never written);
- `DAT_007d70dc[DAT_007d70c8]` in the first check: idx was loaded from `DAT_007d70c8` with no store in between;
- `key = DAT_007d70ec[idx]` before the pn loop: the loop body stores only on the path that breaks out (pn[1], pn[3]),
  so every compare reads the same key. The original hoists the same load.

The store order fixed by S3 (new index to `DAT_007d70c8`, tail store before the div) is unchanged. PROVISIONAL.tsv still
lists 77.5 for this row (the void S3-era score); not touched here.

## Checks

- `py -3.12 re/tools/match/match.py re/match/rwl01/004dcbb0.c` -> 83.2% tc=ddk; `--toolchain rw9044` -> 83.2%.
- No build or gates (match-only row). The source is plain C (one volatile read, no asm), valid as C++ for the port
  include; the VS2022 port build was not run here.
- relocheck / c4_reclass not run: the row is not byte-identical.

## Evidence files

`verify/match_20261113/MATCH-RWT-S5_probes/`: `drv.py` (variant driver), `v1.py`..`v15.py`, `space1.py`..`space15.py`
(variant sets) with `space*.txt` score tables, `syn*.py` (synthetic allocation probes), `lv1.c` (82.4 source),
`best832.c` (83.2 body), `s3_004dcbb0.c` (S3 source, the base of `v2.py`..`v6.py`), `perm1_log.txt` / `perm2_log.txt`.
The compiled variant sources (`out/`, 5.4 MB) are not committed.
