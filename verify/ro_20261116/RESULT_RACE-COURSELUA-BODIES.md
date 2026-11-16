# RESULT RACE-COURSELUA-BODIES (run 52, section MATCH, match-only row)

Branch `orch/RACE-COURSELUA-BODIES` from `orch/integration` @ `215d5bbb9`. Worker session cmwjev3903u5wp015x1l344h8,
lead cmwjaf65a3d7fp0153fg1nefc. Opus 5.5 medium. No build, no game run, no gates (match-only row kind).

Goal: give the four stops of PORT-OT-RACE checkpoint 2 a body (`verify/ro_20261116/RESULT_PORT-OT-RACE.md` section 6,
on branch orch/PORT-OT-RACE @ 4952d4280): 0x0047a020 (the stop that ends the course load at COURSE.LUA), then
0x00479330, 0x00478660, 0x00462950.

## 1. Verdict: all four listed PROVISIONAL (L-126); none byte-matched, none admitted

| rva | source | match.py --bytes (DDK, `/O2 /GS /Ob1`) | equiv.py (tool 1ed7ceb1a) | outcome |
|---|---|---|---|---|
| 0x0047a020 | `re/match/s12/0047a020.c` | 199 B, 13 differing bytes, first at +0x86 (92.2% on Ghidra's 196 B extent) | no verdict: the tool does not end (section 3) | provisional |
| 0x00479330 | `re/match/s12/00479330.c` | 2894 B, 72 differing bytes, first at +0x932 (97.2%) | `NOT-PROVEN callee-pop-unknown: callee 0x478200 (fallback: the exposed frame differs between the paths into a join)` | provisional |
| 0x00478660 | `re/match/s12/00478660.c` | 1602 B, 972 differing bytes, first at +0x3b (79.1%) | `NOT-PROVEN call-memory: region ours +0x111 / orig 0x00478771 call 0x4c39b0: memory differs` (x87 terms, fsub / fsubr / fsubp operand order of the floats stored at esp0-0x104 / esp0-0x100) | provisional |
| 0x00462950 | `re/match/s10/00462950.c` | 1138 B, 163 differing bytes, first at +0x251 (88.8%) | `NOT-PROVEN indirect-call-esp-reset: orig: ESP restored from a register in a function with a call through ...` (call chain 0x004a2cbd / 0x004a2b60) | provisional |

Each row is appended to `re/match/PROVISIONAL.tsv` (status `provisional`, `replay_prov_insitu` not run, evidence =
this file) with its verdict line. Provisional is not C4 and not an admission (L-126, L-128). No `re/promotions/*.tsv`:
nothing reached a byte match. No decision row: no verdict was conditional (no EQUIV-IF-CALLERS-IGNORE).

Consequence for PORT-OT-RACE: the four sources may now run on the knob path as listed provisional bodies, so that row can
bind them in place of the stops. Their object-build bytes stay DIFFERENT; a provisional unit with DIFFERENT bytes is
what the replay oracle then measures.

## 2. Raise attempts (one attempt per RVA, sources unchanged)

- **0x0047a020.** The only residue is the store schedule inside the 32-entry init loop 0x0047a0a0..0x0047a0c8: the
  original stores `ecx` (0x3f000000) to `[eax+0x10]` (entry +0xf4) second, right after `[eax-0xe4]` (entry +0x0);
  ours stores it sixth. Variants scored in this row (scratch copies, `match.py --bytes`), on top of the 312 permuter
  compiles and 6 orders the source header records:
  - the order of the eight per-entry stores follows the source order, and the loop pointer bias is the field of the
    **second** store (bias 0xe4 when fe4 is second, 0xf4 when ff4 is second). The original has bias 0xe4 with the
    +0xf4 store second, which no source order tried here gives.
  - orders with ff4 at positions 1..8 (12 orders): 149 / 127 / **9** / 11 / 12 / 13 / 14 / 17 / 149 / 14 / 120 / 124
    differing bytes. Best 9 (f0, fe4, ff4, fe8[0..2], ff8, ffc), not 0.
  - ff4 / ff8 / fe4 / f0 retyped int or undefined4 with ff4 second: 127 each. Chained `ff4 = ff8 = 0.5f` (second or
    third), chained zeros, a `do/while` loop, a `float *q = &entry.fe4` loop: 15 to 131. `0.5` as double, a local
    `h = 0.5f`, int zero stores: 13 (no change).
  The source is left as it is (13 bytes) because the equiv attempt and the provisional row cite it.
- **0x00479330, 0x00478660, 0x00462950.** The source headers already record the hand work (TL-MATCH-479330,
  TL-MATCH-LUA, MATCH-S10 h6) and their residues: the instance-loop register form (0x00479330), the x87 schedule of the
  cross products plus the commuted retest (0x00478660), the exit-path block layout from +0x251 (0x00462950). x87
  instruction order is never admitted, so 0x00478660 cannot be admitted even with a better proof. No new lever was
  tried on these three in this row.

## 3. equiv.py on 0x0047a020 does not end [tool limit]

- default budget (`--stats`): killed by this worker after about 60 min with no output.
- `--max-steps 20000 --max-terms 100000`: killed by `timeout 1500` (exit 124) with no output. The budget counters
  (steps, terms) do not stop it, so the time is spent outside the instruction loop they count.
- `--trace` (90 s): the pairing reaches `pair +0xaa / 0x0047a0ca` (the `call 0x47b9b0` after the loop) and prints
  nothing after it. [UNCERTAIN] Where the tool spends the time after that pair is not measured. The function's Ghidra
  extent (196 B) ends 3 bytes before the real `mov eax, ebp / pop ebp / ret` (199 B, source header); whether that
  extent is the cause is not tested. Register line.

## 4. relocheck (via `prov_drop.py` dry run)

`prov_drop.py` lists all four as `KEEP not-included` (no exe TU on this branch includes them; PORT-OT-RACE adds the
TUs on its branch). Its relocheck column: 0x0047a020 `OK checked=12`; the other three report BAD at an offset past
their first differing byte (0x00462950 +0x252 `_DAT_0088e670` vs orig 0x00690478, first difference +0x251;
0x00479330 +0x95f, first difference +0x932; 0x00478660 +0x1b3, first difference +0x3b). Both 0x0088e670 and
0x00690478 occur in `re/match/s10/00462950.c` (lines 47 and 53). [UNCERTAIN] These BAD rows are the code shift after
the first difference, not a wrong target: read from the offsets and the source only. Register line.

## 5. Files

- `re/match/PROVISIONAL.tsv`: +4 rows (0x0047a020, 0x00479330, 0x00478660, 0x00462950).
- `re/orchestrator/UNCERTAIN_REGISTER.md`: +2 lines (sections 3 and 4).
- this RESULT.
Scratch variants and raw equiv output stay in the session scratchpad (not committed).
