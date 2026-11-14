# RESULT MATCH-RWT-S5 (2026-11-13)

Row: compiler shaping of 0x004dcbb0 (`re/match/rwl01/004dcbb0.c`) toward a byte match. Match-only: no build, no gates.

## Attempt 3 (Fable 5.2 high on claude3, the one Fable attempt): GREEN, byte-identical

**Verdict: GREEN. 83.2% -> 100.0%** (regs 100.0%, ops 100.0%, 119/119 ins) under `ddk` (cl 13.00.9176 /
c2 13.00.9178) and `rw9044`, flags `/O2 /Ob1 /G5`. `relocheck.py`: `0x004dcbb0 OK checked=24 unchecked=0`.
`c4_reclass.py batch --tsv re/promotions/MATCH-RWT-S5.tsv` (dry run): `[1/1] 0x004dcbb0 match PASS byte-identical
369 B, relocheck OK (24/0)`, 1 pass, 0 refused. Not applied on the branch (merge train applies it). Fable gate 1
(two Opus-high attempts failed on this exact problem).

### What closed it

The earlier sources kept the slot index in a local (`idx`) mirrored to `DAT_007d70c8`. The matching source has no such
local. Four source facts, each tied to the original's listing:

1. **`DAT_007d70c8` used directly at every site** (`f1.py`). c2 forms the index web by itself and puts it in ebp:
   load at 0x004dcbbb (`mov ebp,[0x7d70c8]`), loop compare `cmp ecx,ebp` (0x004dcbf5), found path
   `mov ebp,ecx` / `cmp ecx,4` / `mov [0x7d70c8],ebp` (0x004dcc27..0x004dcc2c), reloads after the pn stores
   (0x004dcc7b) and after the call (0x004dcc96). This is the found-path residue of attempt 2 (store from ebp), and it
   needs no store / compare form in the source.
2. **end is a variable** (`end = off + size`, and `end = size` on the two off = 0 paths, 0x004dcc4a `mov ebx,edi`).
   With the expression `off + size` repeated at each site instead (`f1.py` X forms) c2 recomputes it in the tail
   (`lea eax,[esi+edi]`) and has no `mov ebx,edi`.
3. **Loop key local, tail inline** (`f2.py`, `f3.py`). The pn loop key is a local loaded before the loop
   (0x004dcc5f `mov ecx,[ebp*4+0x7d70ec]` precedes the null test at 0x004dcc68; inline key: 99.2). The tail uses
   `DAT_007d70ec[DAT_007d70c8]` inline in each branch (0x004dcc9f `mov ebp,[ebp*4+0x7d70ec]`, then `mov ecx,[ebp]` /
   `mov eax,[ebp]` per branch). A `vb` local before the `if` gives 88.2 (pointer in eax, `*vb` load hoisted above
   the branch).
4. **Entry copy of param_1 through a volatile read** (`a0 = *(volatile uint *)&param_1;`, used for the size product
   and the first round-up, 0x004dcbb0 `mov ecx,[esp+4]`). The loop and the tail read `param_1` itself, which c2
   leaves in its home slot: `mov ebx,[esp+0x14]` per pass (0x004dcc00) and `div dword ptr [esp+0x14]` (0x004dccf2).
   With (1) to (3) in place and param_1 out of the register competition, size takes edi, end ebx and the index ebp
   with no further shaping. This removes the 17-instruction size / end swap of attempt 2.

Score ladder (ddk = rw9044 where both were run): `f1.py` natural source with the global index 44.5 (end variable) /
58.8 (end expression), `f2.py` plus the entry volatile copy 88.2, `f3.py` tail inline 100.0, `f4.py` controls: plain
copy `a0 = param_1` 41.2, param_1 everywhere 41.2, volatile copy only in the size product 70.6, only in the first
round-up 85.7, inline loop key 99.2.

### Disclosures

- **The volatile read is still in the source** (one, in the entry block; attempt 2 had one in the loop). It is plain
  C, emits the one load the original has at 0x004dcbb0, and reads the same value as param_1 (param_1 is never
  written). No inline asm. Without it (plain copy or param_1 everywhere, `f4.py` c / n) c2 gives param_1 a
  callee-saved register and leaves the index in memory, 41.2%. A source form that makes c2 drop param_1 from the
  registers with no volatile was not searched in this attempt (one attempt, stopped at byte identity). If the lead
  reads the match route as excluding a volatile-read cast, this row is 41.2% natural / 100% with the cast and needs a
  decision. `c4_reclass.py` accepts it as is.
- **Provenance of lever 4.** The entry-block placement of the volatile copy first appears in `e1.py` (`t_lp_dp`,
  71.4%), an untracked probe left in the worktree by the stopped wrong-model worker, with `e2.py` (index-site sweep on
  the local-idx source, best 88.3% at 120 instructions). I read both before the move to Fable. Levers 1 to 3 and the
  combination are from this attempt. `e1` / `e2` are committed with the probes for the record.
- **PROVISIONAL.tsv** (`re/match/PROVISIONAL.tsv` line 200) still lists this RVA at 77.5 NOT-PROVEN. That score is void
  (S3) and the row is now byte-matched. Not edited here: no documented tool path for removing a row was found
  (`re/tools/prov/`, `re/tools/provbodies/` read it). The lead or the merge train should drop or refile the row when
  the promotion applies.
- `re/analysis/match_ledger.tsv` line 2615 (77.5, REFUSED) is a generated ledger, not regenerated here.

### Semantics

Byte-identical to 0x004dcbb0 (369 B, relocs checked 24/0), so the body computes the original's function. Against the
S3 / attempt 2 source: same stores in the same order, the local `idx` is replaced by reads of `DAT_007d70c8` at
points where the local held the global's value.

### Checks run

- `py -3.12 re/tools/match/match.py re/match/rwl01/004dcbb0.c` -> 100.0% tc=ddk; `--toolchain rw9044` -> 100.0%.
- `py -3.12 re/tools/match/relocheck.py re/match/rwl01/004dcbb0.c` -> OK checked=24 unchecked=0.
- `py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-S5.tsv` -> PASS (dry run, nothing written).
- No build, no gates (match-only row). The VS2022 port build of this file was not run.

### Evidence files (this attempt)

`verify/match_20261113/MATCH-RWT-S5_probes/`: `f1.py`..`f4.py` with `f1.txt`..`f4.txt` (score tables),
`install_final.py` (writes `f4.py` variant `t` into the source), `e1.py` / `e2.py` and tables (stopped worker).
Promotion: `re/promotions/MATCH-RWT-S5.tsv`.

---

## Attempt 2 (Opus 5.5 high), history

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
