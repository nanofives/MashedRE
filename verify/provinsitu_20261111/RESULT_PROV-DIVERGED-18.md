# RESULT PROV-DIVERGED-18: diagnosis of the 18 DIVERGED bodies from PROV-INSITU

Date 2026-11-11. Worker session cmwcuqp6w4r5csc1bswl8oey1 (claude2, claude-opus-5-5), parent cmwcupe044qjusc1b7642xtrv.
Branch `orch/PROV-DIVERGED-18` from `orch/integration` `bf44aae7d`. Row kind: race-path diagnosis. Changes are
`re/match` sources, `re/tools/match/equiv.py` (+ rules doc), two `re/tools/prov` tools, `re/match/ADMITTED.tsv`,
the new `re/match/PROVISIONAL.tsv` and this folder. No `mashedmod/` file, no tracker, no ROADMAP edit.

## Verdict

**GREEN.** All 18 bodies have a verdict, cited by address and value:
- **7 fixed, each IDENTICAL-FOR-2724 in the replay with only its JMP live:** `0x00448220`, `0x00424eb0`,
  `0x00481e00`, `0x00479100`, `0x00443dc0`, `0x00445aa0`, `0x00442e00`. They go to `PROVISIONAL.tsv` as
  `provisional`, together with `0x00408b00`.
- **2 partly fixed, still DIVERGED, refused:**
  - `0x0046e9e0`: first divergence moved from tick 116 to 321.
  - `0x00468d80`: from tick 408 to 2162, 7 ticks in all.
- **9 not fixed, provisional-refused, each with its lead.**
- **0x00448220 (the equiv.py question): equiv.py was unsound here.**
  - A CRT function cut short by Ghidra's extent (acos `0x004a3384`, size 9) was summarised as no-return. That made
    `0x00408b00` no-return too.
  - So the proof of `0x00448220` stopped at its first call to `0x00408b00` (`0x00448475`). The scale block, where the
    real defect sits, was never compared, and the record showed no rule for it.
  - Both defects are fixed in `equiv.py`. `0x00448220` and `0x00408b00` leave `ADMITTED.tsv`, the other 31 rows hold.
- **Defect classes found:**
  - The DDK reassociating float products or sums written as one expression: 4 bodies.
  - Literals that do not equal the original's constant words: 3 bodies.
  - A `static` file copy of an original global: 1 body, and 32 more sources have one.
  - A stand-in callee bound to the original with another ABI: 1 body.
  - Frame-layout dependence through stale stack: `0x00469aa0`, `0x00441d40`, the rest of `0x0046e9e0`.

| | count | bodies |
|---|---|---|
| fixed, IDENTICAL-FOR-2724 | 7 | 0x00448220, 0x00424eb0, 0x00481e00, 0x00479100, 0x00443dc0, 0x00445aa0, 0x00442e00 |
| partly fixed, still DIVERGED (refused) | 2 | 0x0046e9e0 (@321), 0x00468d80 (@2162) |
| not fixed (refused, lead named) | 9 | 0x00441d40, 0x00443440, 0x00469aa0, 0x004709a0, 0x00469df0, 0x0046ef70, 0x0046f6c0, 0x004c51a0, 0x00487180 |

No promotion, no tracker edit. The 7 IDENTICAL bodies (and `0x00408b00`) are `c4=frida-replay` candidates for the
row that owns L-128 promotions. Each still needs the write-set leg and re-classify's own checks.

## 1. 0x00448220: EQUIV under equiv.py, DIVERGED@1116 in the replay (the soundness question)

**Answer: the EQUIV proof covered about a third of the body.** Two defects in `equiv.py`, both fixed here, plus one
real source defect that the unchecked part hid.

### 1a. What the proof missed

- `equiv.py --trace re/match/s08/00448220.c` (old tool) visits 95 region pairs. The last ones are `+0x21b / 0x0044843b`
  and `+0x221 / 0x00448441`. Each ends at the call `0x00448475 call 0x00408b00` (ours `+0x255`). No pair past that
  call is ever visited: the scale block (`0x0044859a`), the `FUN_00442e00` call and the tail jump to `0x00434360`
  are not compared. `--x87-strict` gives the same EQUIV.
- Why: `summary(0x00408b00)` returned `noreturn: True` (no `ret` reached), so the pair walk ended that path with
  `halt` (`equiv.py`, `do_call`: `if info.get("noreturn"): region.end = ("halt",)`).
- Why `0x00408b00` came out no-return: it calls CRT acos `0x004a3384` at `0x00408bcf`. `match.ghidra_size(0x004a3384)`
  is 9. Its body is `lea edx,[esp+4]; call 0x004a70d5` and it continues at `0x004a338d`, which is a separate entry of
  the function table. `_summary_body` stopped there (`if pc >= next_entry: break  # ... the call before it does not
  return`) with no `ret` seen, so `0x004a3384` was no-return, and the walk of `0x00408b00` stopped at its call to it.
  `0x00408b00` has 6 `ret`s (`0x00408f34`, `0x0040902f`, `0x00409113`, `0x00409184`, `0x004091f5`, `0x0040925a`) and
  the replay shows it returns (416 race-window calls, IDENTICAL-FOR-2724 with its own source live).
- **Second defect, reporting:** the halt was silent. `elif ka == "halt": pass`, and the `no-return` tag was added only
  when no path reached any `ret`. `0x00448220` has two early returns (`DAT_007f1a50 == 1`, and the reset path), so
  rules said `jmp-threading` only. `ADMITTED.tsv` admitted it with nothing showing that 2/3 of the body was unchecked.

### 1b. The fix (`re/tools/match/equiv.py`)

- Falling into the next function-table entry now returns an unknown summary (`_sfail(va, "r11")`), not no-return.
- A path that ends at a no-return call now adds the `no-return` tag even when other paths return.
- `equiv_rules.md` section 4 documents the `no-return` tag and the `r11` case.
- `test_equiv.py`: 2 passed (`test_cases`, `test_references`).
- Re-run over all 33 rows of `ADMITTED.tsv` (verdict per row in `equiv_recheck_admitted.txt` of this folder):
  - 30 unchanged (29 EQUIV, 1 EQUIV-IF-CALLERS-IGNORE(EAX), 0x00555360).
  - `0x00408b00` -> NOT-PROVEN `callee-pop-unknown: callee 0x4a3384` (its admission stopped at `0x00408bcf`).
  - `0x00448220` -> NOT-PROVEN `call-memory ... call 0x442e00` (the swap below, now in view).
  - `0x00499400` NOT-PROVEN `call-regargs ... call 0x498a00` with the **old** tool too, on `orch/integration`
    `bf44aae7d`. Not caused by this change. Not touched here: it is listed in section 5 for the admission owner.
  - Both rows the fix changes were removed from `ADMITTED.tsv` and listed in `PROVISIONAL.tsv` (31 rows remain).

### 1c. The source defect the unchecked part hid

- `relocheck.py` on the old source: BAD at `+0x37c` (`_DAT_005ce1ec`, original `0x005cc320`) and `+0x38e`
  (`_DAT_005cc320`, original `0x005ce1ec`).
- DDK listing (`/FAs`) of `f = DAT_005cc320 / FUN_004c3ac0(&b) * DAT_005cc950 * DAT_005ce1ec;`:
  `fdivr _DAT_005ce1ec`, `fmul _DAT_005cc950`, `fmul _DAT_005cc320`.
- The original: `0x0044859a fdivr [0x005cc320]`, `0x004485a6 fmul [0x005cc950]`, `0x004485ac fmul [0x005ce1ec]`.
- Values (MASHED.exe.unpatched): `0x005cc320` = 0x3f800000 (1.0), `0x005cc950` = 0x3f400000 (0.75),
  `0x005ce1ec` = 0x3f966666 (1.175).
- So the body computed 1.175 / len * 0.75 * 1.0 where the original computes 1.0 / len * 0.75 * 1.175. Equal in
  reals, rounded differently on the x87. The result `f` scales the camera target passed to `FUN_00442e00`. This
  fits the replay: intermittent, from tick 1116, 0 dwords at the checkpoints.
- Fix: the operands written in the other order. cl 13.00.9176 swaps them back (listing: `fdivr _DAT_005cc320`,
  `fmul _DAT_005cc950`, `fmul _DAT_005ce1ec`). Same 97.0%. relocheck: the two sites now OK, the remaining 3 BAD are
  the store offsets of the last block, whose scheduling residual was already in the header (values checked by hand:
  same operands, same x87 order, stores to globals moved across stack-only x87 code).

### 1d. Replay

- Control, the old source in this row's root (`o448220_b`): DIVERGED@1116, 47 diverging ticks, blocks `0x0068f400`,
  `0x0086ac00`, `0x00898000`, `0x00898800`. That is PROV-INSITU's result (47 ticks from 1116).
- Fixed source: **IDENTICAL-FOR-2724** in 2 runs (`f448220_b` with its own blob, `fx_00448220` in the 9-body link).
- So the constant swap is the whole of the 1116 divergence.

## 2. The other 17 bodies

How: `match.py --show` and `relocheck.py` per source, the DDK `/FAs` listing, the original's linear disassembly
(capstone over `MASHED.exe.unpatched` from the entry), and a new check, `re/tools/prov/x87_operand_order.py`: the
order of the absolute x87 memory operands, as values, ours against the original. It shows the constant swap of
`0x00448220` and the folded literal 0x330f2a63 (0.0001 x 1/3000) of `0x0046e9e0` on the old sources, and neither on
the fixed ones. Output for all 11 bodies without an earlier finding: `x87_operand_order_11.txt`.

| rva | best % (old -> new) | first tick (PROV-INSITU) | diagnosis | action |
|---|---|---|---|---|
| `0x00448220` | 97.0 -> 97.0 | 1116 | const operands of the scale swapped by the DDK (section 1) | fixed, IDENTICAL-FOR-2724 x2 |
| `0x00424eb0` | 98.6 -> 98.6 | 1108 | stand-in `FUN_00422fd0` bound to the original: count passed for the index | fixed, IDENTICAL-FOR-2724 |
| `0x00481e00` | 86.5 -> 85.9 | 2 | file-static DDK copy of `0x006ce818` | fixed, IDENTICAL-FOR-2724 |
| `0x00479100` | 69.1 -> 70.9 | 161 | `d[0]` sum regrouped by the DDK | fixed, IDENTICAL-FOR-2724 |
| `0x00443dc0` | 57.5 -> 57.5 | 2 | 1/3 literal 0x3eaaaaab for the original's 0x3eaaaa9f (`0x005ce034`) | fixed, IDENTICAL-FOR-2724 |
| `0x00445aa0` | 67.4 -> 67.4 | 2 | 60 x 1/7 folded; two step literals unequal to `0x005ce184` / `0x005ce17c` | fixed, IDENTICAL-FOR-2724 |
| `0x00442e00` | 65.1 -> 65.1 | 1132 | step literal 1 ULP off `0x005ce02c` | fixed, IDENTICAL-FOR-2724 |
| `0x0046e9e0` | 81.7 -> 81.6 | 116 | thrust products folded / reassociated; then stale stack in the event queue | partly fixed, DIVERGED@321, refused |
| `0x00468d80` | 82.1 -> 82.1 | 408 | spin scale reassociated, contact dot regrouped; residual on 7 ticks | partly fixed, DIVERGED@2162, refused |
| `0x00441d40` | 69.2 | 2 | stack addresses in the tick-2 dwords; frame 0x80 vs 0x84 | refused |
| `0x00443440` | 58.4 | 196 | counts differ (`0x0089a568` 0xb6 / 0xac); no static lead | refused |
| `0x00469aa0` | 60.0 | 385 | stale stack copied into the event queue, shifted 4 B | refused |
| `0x004709a0` | 55.7 | 321 | 17 blocks; register-arg static callees | refused |
| `0x00469df0` | 63.1 (30.1 in the table) | 385 | 10 blocks | refused |
| `0x0046ef70` | 43.9 | 408 | block `0x00881c00`; no value lead from the x87 check | refused |
| `0x0046f6c0` | 50.7 (44.3 in the table) | 1 | writes 0x1900 / 1 into the car record (`0x008815ac` / `b4`) | refused |
| `0x004c51a0` | 14.7 | 1 | raw g2s output | refused |
| `0x00487180` | 11.7 | 0 | raw g2s starting point | refused |

### Per-body notes

**0x00424eb0** (DIVERGED@1108, `0x007f0f64` body 0x447a4000 (1001.0) / orig 0x447a0000 (1000.0)).
- The source held a static stand-in `FUN_00422fd0(int n, int param_1)` with an extra first parameter, kept for the
  98.6% score. Its header already said it "is NOT the real callee shape".
- provlink (and the object build) bind any `FUN_<va8>` name to the original. The original `0x00422fd0` reads its one
  argument from the stack (`0x00422fd1 mov esi,[esp+8]`).
- Our call site pushed the count (`mov edx,[esp+0x18]; push edx`) where the original pushes the index
  (`0x00425275 mov eax,[esp+0x18]; 0x00425279 push esi; 0x0042527a call 0x00422fd0`). So `0x00422fd0` ran on
  index = count.
- Fix: the stand-in renamed `s_StandIn_00422fd0`. It is now linked as the file's own helper and receives the index.
  98.6% and frame 0x38 kept. The 1-arg extern form was tried first and diverged at tick 151 (section 3).

**0x00481e00** (DIVERGED@2, `0x006ce818` body 0 / orig 0x75).
- The C build declared `static unsigned int DAT_006ce818;` (`#ifndef __cplusplus`). Only the C++ port named the
  original word, `MGLOBAL(unsigned int, DAT_006ce818, 0x006ce818)`.
- So the DDK object counted in its own BSS, and the linked blob's one absolute store went to blob `+0x230`
  (`bodies_80.json`: `abs_out 0x00000230@0x108`). The original word stayed 0.
- Fix: the MGLOBAL for both languages. 86.5% -> 85.9% (regs 94.7%, ops 95.9%). The new blob has the store inside the
  image (`w=1/0/1`).
- Same class elsewhere: 32 more `re/match` sources declare a `static` copy of a `DAT_` global (31 functions;
  `0x0041c380` has a pilot copy). Only `0x00498a00`
  (NOT-EXECUTED) is in the PROV-INSITU table. List: `filestatic_sources.txt`. Each is a wrong body under the DDK
  whenever another function reads or writes that word.

**0x0046e9e0** (DIVERGED@116, `0x0063d928` body 0x3d126547 / orig 0x3d126549, 2 ULP).
- The header's residual (2) was a value difference. The thrust products
  `t * dt * DAT_00613108 * g * 0.0001f * 0.00033333333f` compiled to a folded literal (0x330f2a63) applied first.
- The original multiplies left to right through `0x00613108`, `0x005cd03c` (0.0001), `0x005cc948` (1/3000) at
  `0x0046eb95`/`9d`/`a3` and `0x0046ec3a`/`42`/`48`.
- The `== 7` block compiled to `(dt+dt) * +0x9e4 * literal` where the original does `* 0x005cc948`, `* +0x9e4`,
  `* 0x005ce268` (0.0005) (`0x0046edbb`..`0x0046edc7`).
- Fix: one statement per multiply with the constants as `const` globals. The listing now has the original's order
  at all three sites (relocation names checked). 81.7% -> 81.6%.
- Left as is: in the `d` block the original reloads the stored floats where ours keeps them on the x87 stack. These
  are equal values when precision control is 24 bit, as in MASHED.exe (memory note
  `asm-bit-identity-in-asi-not-standalone-pc24`).

**0x00468d80** (DIVERGED@408, `0x00881c64` body 0x3d82ea2a / orig 0x3d82ea29, 1 ULP).
- Two reassociations.
  - Spin scale: `FUN_004c3ac0(&car->spin) * 360.0f * c->f14 * DAT_005cea44` compiled to `len * f14 * 0.2778 * 360`.
    The original computes `len * 360 * f14 * 0.2778` (`0x00469071 fmul [0x005ccac4]`, `0x0046907d fmul [0x005cea44]`).
  - Contact dot `n . vel`: written `x*X + y*Y + z*Z`, it compiled to `(z*Z + y*Y) + x*X`, where the original sums
    `(y*Y + x*X) + z*Z`.
- Fix: one statement per multiply / term, 360 as a `const` global. 82.1% unchanged.

**0x00479100** (DIVERGED@161, block 0x0063dc00).
- `d[0] = m[8]*pos.z + m[4]*pos.y + m[0]*pos.x + m[12]` compiled to `(m0*x + m8*z) + m4*y + m12`. The original sums
  `(m8*z + m4*y) + m0*x + m12` after `0x0047920a call 0x00559040`. `d[1]` and `d[2]` already compiled in the
  original's order.
- Fix: one statement per term. 69.1% -> 70.9%.

**0x00443dc0** (DIVERGED@2, `0x008032ec` body 0x43ac58f7 / orig 0x43accce2).
- The two `* 0.33333334f` literals compile to 0x3eaaaaab. The original multiplies by `0x005ce034` = 0x3eaaaa9f
  (0.33333299) at `0x0044400e` and `0x0044405c`, the value that `0x00443440`'s `0.333333f` gives.
- Fix: `0.333333f`. 57.5% unchanged.
- Other x87-order differences found are not value differences: the original repeats the `a.y`/`b.y` range compares
  (as the source does, `0x004441d7`..`0x004442a5`) and the DDK merges the repeats. Pure compares, no store between.
- The replay settles it: with this one change the body is IDENTICAL-FOR-2724 (`fx_00443dc0`).

**0x00445aa0** (DIVERGED@2, `0x0089650c` body 0xbf719745 / orig 0xbf719777).
- Three value differences.
  - `dt * 60.0f * e * 0.14285715f` compiled to `dt * e * (60/7)` (literal 0x41092493). The original multiplies by
    `0x005cc728` (60, `0x00445d99`), then `e`, then `0x005ce188` (1/7, `0x00445dc0`).
  - The sway step `0.0030679711f` compiles to 0x3b491004. The original's `0x005ce184` is 0x3b490e56
    (0.0030678711, `0x00445f4e`).
  - The roll step `0.0061359424f` compiles to 0x3bc91004. The original's `0x005ce17c` is 0x3bc90e56
    (`0x004463d5`).
- Fix: all three read the original's words, the product one multiply per statement. 67.4% unchanged.
- The new equiv first difference, `call-regargs ... call 0x441c80` (EBP / EDI), is a tool over-approximation, not a
  body defect. `summary(0x00441c80)` gets live-in EDI/EBP from `0x0040e180`, whose `0x0040e187 or edi,0xffffffff` and
  `0x0040e18f or ebp,0xffffffff` write -1 whatever the old value, and are counted as reads (section 5).

**0x00442e00** (DIVERGED@1132, blocks 0x00898000, 0x00912800).
- The sway step `0.004291845f` compiles to 0x3b8ca29b. The original's `0x005ce02c` is 0x3b8ca29c (1 ULP,
  `0x00442fe8`).
- Fix: reads `0x005ce02c`. 65.1% unchanged.

**0x00441d40** (DIVERGED@2): not fixed, provisional-refused.
- Tick 180 dwords: `0x00896620` body 0x3e997982 / orig 3; `0x00896888` body 1 / orig 0x001afb3c;
  `0x00896898` body 0x001afb24 / orig 0x00896748. 0x001afb3c and 0x001afb24 have the form of main-thread stack
  addresses.
- The header lists a frame of 0x80 against the original's 0x84. The fixed equiv's first difference is
  `call 0x441c80 arg 0: (esp0-0x58) | (esp0-0x68)`, a local address 0x10 apart.
- The x87 operand check pairs three `fld 100.0` with the original's `fld [0x005d757c]` (0.0, `0x00442215`,
  `0x0044221f`, `0x00442239`). Decoded from the entry, that is the same `v = 0 / v = s` logic (`s` is reloaded from
  `[esp+0x30]` in the original, folded to the literal in ours, and the zero is `fldz` in ours). Not a value difference.
- [UNCERTAIN] What writes `0x00896620` at tick 2. It needs a `--watch-write` on that dword in a body run.

**0x00469aa0** (DIVERGED@385): not fixed, provisional-refused.
- Tick 543 dwords: `0x007eabdc` body 0x008822a4 / orig 0x00469be2; `0x007eabe0` body 0x00828320 / orig 0x008822a4;
  `0x007eabfc` body 0x008822a4 / orig 0x00469be2.
- 0x00469be2 is the return address of `0x00469bdd call 0x004694e0` in this function. The two pushes before
  `0x00469bd7 call 0x00468d80` are `push 0x828320` and `push esi`.
- So these queue words hold stale stack: the return address and arguments of an earlier call. The body's values are
  the original's shifted by one dword (4 bytes). A callee (original code) copies uninitialised stack into the event
  queue, and our frame is laid out differently.
- Only an identical frame and call-site ESP removes it. [UNCERTAIN] Which instruction copies it: `--watch-write`
  on `0x007eabdc` at tick 385.

**0x00443440, 0x004709a0, 0x00469df0, 0x0046ef70, 0x0046f6c0**: not fixed, provisional-refused.
- Scores 30.1-58.4% in the PROV table. The x87 operand check found no constant or order difference that changes a
  value: `0x0046ef70`'s folded `0x3b360bc0 * 8` is exact, and `0x0046f6c0`'s `0x006146fc` / `0x00614704` pair are both
  0.0. The rest are register-allocation and frame differences.
- `0x0046f6c0` writes 0x1900 and 1 into the car record at tick 1 (`0x008815ac`, `0x008815b4`) where the original has
  0: a structural difference. It needs source work beyond this row.

**0x004c51a0, 0x00487180**: refused. Both are raw `g2s.py` starting points (14.7%, 11.7%). equiv: `call-sequence`,
2 vs 0 and 10 vs 1 calls.

## 3. Replays after the fix

- Harness: PROV-INSITU's `detorig.py` configuration (tick clock, `--q0 0xC0000000`, `warp_normalize.tsv`,
  `--load-sleep-ms 0`, `--warp-clock 2000`, QuickRace track 0, 4 cars, 2,724 ticks).
  - One body live per run, installed by `--prov` / `--prov-rvas`, blobs from `provlink.py`.
  - Every run reports `prov install 1 sites, 0 failed` and `prov rows 1 executed 1`.
- References:
  - `d18_r2` and `d18_r3`: r3 against r2 is IDENTICAL-FOR-2724.
  - `d18_r1` is a FLAKE-T0 harness run (`0x006181a0..a8` = 0x3f000000, 0 in r2 / r3), the signature PROV-INSITU
    documents. It is not used.
  - Compare: `prov_compare.py <run> d18_r2 d18_r3 --pair x_ref,x_ref2` (15 allowed blocks).
  - Every diverging tick, not only the first: `prov_ticks_all.py`, same allowances.
- Table: `replays_d18.tsv`.

| run | body | verdict | diverging ticks |
|---|---|---|---|
| `o448220_b` | 0x00448220 old source (control) | DIVERGED@1116 | 47 |
| `f448220_b`, `fx_00448220` | 0x00448220 | IDENTICAL-FOR-2724 | 0, 0 |
| `fx_00424eb0` | 0x00424eb0, 1-arg extern form (frame 0x34) | DIVERGED@151 | 2100 |
| `sx_00424eb0` | 0x00424eb0, final form (renamed stand-in, frame 0x38) | IDENTICAL-FOR-2724 | 0 |
| `fx_00481e00` | 0x00481e00 | IDENTICAL-FOR-2724 | 0 |
| `fx_00479100` | 0x00479100 | IDENTICAL-FOR-2724 | 0 |
| `fx_00443dc0` | 0x00443dc0 | IDENTICAL-FOR-2724 | 0 |
| `fx_00445aa0` | 0x00445aa0 | IDENTICAL-FOR-2724 | 0 |
| `fx_00442e00` | 0x00442e00 | IDENTICAL-FOR-2724 | 0 |
| `fx_0046e9e0` | 0x0046e9e0 | DIVERGED@321 | 2403 |
| `fx_00468d80` | 0x00468d80 | DIVERGED@2162 | 7 |

- **0x00424eb0 took two attempts.**
  - The real 1-arg call (`int FUN_00422fd0(int)`) is the faithful callee shape. But it drops the dead count, shrinks
    the frame from 0x38 to 0x34, and diverged earlier, at tick 151 (blocks `0x0063d800`, `0x00881c00`..`0x00884400`).
  - The fixed equiv's first difference for that form is `call 0x46c7d0 arg 6: below32(-52) | below32(-56)`.
    `0x0046c7d0` has no summary (`r9`), so every frame slot is compared, and they moved by 4.
  - The final form keeps the 98.6% source and renames the stand-in to `s_StandIn_00422fd0`. provlink then links it as
    the file's own static helper, which receives the index, and the frame stays 0x38. That replays identically.
  - [UNCERTAIN] Which callee reads the caller's frame under the 1-arg form. Not traced; `0x0046c7d0` is the first
    call whose slots differ.
  - Cost: the stand-in is a private copy of `0x00422fd0` (83.3%). Its body calls `thunk_FUN_00419760`, which provlink
    binds to `0x00419760`, the target of the original's thunk `0x004189f0 jmp 0x419760`.
- **0x0046e9e0 remaining:** at the tick-362 checkpoint `0x007eca20` holds 0x3d4ccccd (0.05) in the body run against
  0x41c80000 (25.0) in the original. That word is inside the event-queue range from `0x007e9de0`, as with
  `0x00469aa0`. [UNCERTAIN] Writer not traced; `--watch-write 0x007eca20` at tick 321 would name it.
- **0x00468d80 remaining:** block `0x00881800` on 7 ticks from 2162, 0 dwords at the tick-2179 checkpoint
  (transient). The header residuals (keep-and-pop of the clamped dot, slot order) remain.

## 4. PROVISIONAL.tsv (L-126)

- `re/match/PROVISIONAL.tsv` is new: one row per body here, plus `0x00408b00`, demoted from `ADMITTED.tsv`.
- Columns: source, flags, best_pct now, equiv verdict and first difference (fixed tool), PROV-INSITU verdict, the
  diagnosis, the replay after the fix, and status.
- Status values:
  - `provisional`: the source may run on the knob path in place of a hand stand-in.
  - `refused`: it may not.
  - `pending-replay`: fixed but not yet replayed.
- None of them is C4 by this status (L-126, L-128). A body that replays IDENTICAL-FOR-2724 with its JMP live meets the
  replay form of the frida route and is a candidate for `c4=frida-replay` once the write-set leg is done. Not
  promoted here.
- The row `PROVISIONAL-BODIES` owns the convention (BINDINGS kind, exclusion from the port column). This file gives
  it its first rows, in the columns its title names.

## 5. For other rows

- **equiv.py precision (conservative, not unsound):** `summary()` counts `or r, -1` as a read of `r`
  (`0x0040e187`, `0x0040e18f`). It gives false live-ins (EDI/EBP for `0x00441c80`) and so false NOT-PROVEN
  `call-regargs`.
- **ADMITTED.tsv `0x00499400`** is NOT-PROVEN at `bf44aae7d` with the unchanged tool
  (`call-regargs ... call 0x498a00`). Its source holds a file-static `DAT_007731f8`. The admission owner should
  re-run it.
- **File-static class:** 32 more sources (list `filestatic_sources.txt`).
- **Reassociation class:** a float product or sum written as one expression can compile to another order under
  cl 13.00.9176 /O2 (4 of the 9 fixed bodies). `x87_operand_order.py` finds the constant-operand form of it.

## 6. Builds and gates

- Not run.
  - No exe TU includes any of the 9 changed sources (`grep` of `mashedmod/src` for their paths: 0 hits).
  - None is in `verify/porttop_20261108/BINDINGS.tsv`.
  - `equiv.py` is a tool.
- `ddkobj.py check` not run for the same reason. No G-CTRL / B (top-down rule).
- Game runs: every arm under `test-slot`, inside `mashed_lock.py run --label PROV-DIVERGED-18`, private root
  `D:\MashedWT\PROV-DIVERGED-18_root`. Only PIDs this session spawned were stopped (none needed stopping).
  - The root is a robocopy of `D:\MashedWT\PROV-INSITU_root`, chosen so the binaries are the oracle's:
    `MASHED.exe` `B9977DAB...`, `mashed_re_dev.asi` `906F8601...`, `d3d9.dll` `47901951...`.
  - The original keeps its own exe path at `0x00773c50..0x00773c74` (block 0x00773c00). A run in another root therefore
    differs from PROV-INSITU's references at every tick (run `f448220_a`: tick 0 to 2723, that block only). So this
    row's runs compare against its own two references `d18_r1` / `d18_r2`, with PROV-INSITU's `x_ref` / `x_ref2` as the
    allowed-block pair, as PROV-INSITU did.

## Evidence (this folder)

- `RESULT_PROV-DIVERGED-18.md` (this file).
- `d18_arm.sh`: one arm (private root, lock, compare, all-ticks listing).
- `equiv_recheck_admitted.txt`, `equiv18.txt`, `x87_operand_order_11.txt`, `relocheck17.txt`, `filestatic_sources.txt`.
- `replays_d18.tsv`: the after-fix runs.
- Tools: `re/tools/prov/x87_operand_order.py`, `re/tools/prov/prov_ticks_all.py` (every diverging tick, same
  allowances as `prov_compare.py`), `re/tools/match/equiv.py`.

Raw runs: `D:/MashedWT/PROV-DIVERGED-18_data/` (ticks, checkpoints, logs). Files cited, with SHA-256:

- `D:/MashedWT/PROV-DIVERGED-18_data/d18_r1/ticks.bin` 6b64fe740a8c83f6af68589e82405c73296ef89452b5bbe126d793816b115c33
- `D:/MashedWT/PROV-DIVERGED-18_data/d18_r2/ticks.bin` 6031abcd8a3190e7600aaab72d8325daaacb53adb738f22231062ed0791c5f22
- `D:/MashedWT/PROV-DIVERGED-18_data/d18_r3/ticks.bin` c26dad579afc4cbb725ca00c8f20f1cb7d982b8a92586d237b790e27b9995935
- `D:/MashedWT/PROV-DIVERGED-18_data/o448220_b/ticks.bin` 513c2752ad5e4d174a144dd56aff2223d5d160f67417d0561a786d0d9f127a61
- `D:/MashedWT/PROV-DIVERGED-18_data/f448220_b/ticks.bin` a2c73b6be714ba435ce5625982b2eaed2681e676b7db6294d0d4933e41c2eade
- `D:/MashedWT/PROV-DIVERGED-18_data/fx_00448220/ticks.bin` 1173aef99fda1e4fe6460a98411101254d10a7543d9bfe7521223d95c05a7280
- `D:/MashedWT/PROV-DIVERGED-18_data/fx_00424eb0/ticks.bin` f26cb6c215e4fedd6387cad57e70b2bef61113e98d01196157dbae2fbf69263a
- `D:/MashedWT/PROV-DIVERGED-18_data/sx_00424eb0/ticks.bin` 2adbca797a4e9ebd251ef36fd7821b6094fe5df1a1e108eed9963fae508d5fc8
- `D:/MashedWT/PROV-DIVERGED-18_data/fx_00481e00/ticks.bin` 99d9b8a7dd98d4f91eeeffc83adf7cc1f9393c40846c387d2110d12f794658ab
- `D:/MashedWT/PROV-DIVERGED-18_data/fx_0046e9e0/ticks.bin` 93c887694b0d79ebeebf9f1c27a02175eed17cb387193eb88ca8ce004048a876
- `D:/MashedWT/PROV-DIVERGED-18_data/fx_00468d80/ticks.bin` 3cd907b292389d7b619e8754475b43da13975a7d34b6c45f2a72bc7095d53b17
- `D:/MashedWT/PROV-DIVERGED-18_data/fx_00479100/ticks.bin` 4cde4baa8aeff8a3676da28e6580d4e8104179206fd773ab41502171fc86957e
- `D:/MashedWT/PROV-DIVERGED-18_data/fx_00443dc0/ticks.bin` b334afc9f608333dbe44051c3a10570af6027d9016e538b3884ceef758f9817a
- `D:/MashedWT/PROV-DIVERGED-18_data/fx_00445aa0/ticks.bin` 39038a9cb3c69d1a7b02bb8298725c354b5f03dc54cbd382f7eb77f85d50b6b3
- `D:/MashedWT/PROV-DIVERGED-18_data/fx_00442e00/ticks.bin` eb7faf2944f9b04d6b4fa198c15348adfee36f18cff064a342cbfdec64e2cd59
- `D:/MashedWT/PROV-DIVERGED-18_data/b_448220_fix.json` 83d71c02c8880ba593591e891fa4ce3536b8edc17e561e12f6063c62c90a3036
- `D:/MashedWT/PROV-DIVERGED-18_data/b_fixed9.json` 74279773a0e98bbd601a9a7c266acaa8ef771c27383a74524f8ee7515f17e20f
- `D:/MashedWT/PROV-DIVERGED-18_data/b_424_standin.json` e90d955212bb8e89523f521b964250bd426972eba1b796c3f1bd9a61a47f98b7

