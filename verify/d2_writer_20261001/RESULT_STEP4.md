# RESULT — D2 attempt 15, STEP 4: the collateral review, and the named downstream term

Follows `PREREG_STEP3.md` §3 (the next move, pre-committed before STEP 3's number was seen)
and §4 (the collateral plan). **No fix is authored here**: §3 requires the term to be tested
on the running original first, and that test is a new leg.

---

## 1 Collateral, same-side paired: attempt 14's default against this attempt's default

`collateral.py --mode paired`, A = `s1` (pre-fix default), B = `r1` (post-fix default),
floors `s2` and `r2` (both measured **exactly 0** — the port is bit-deterministic across
boots). 1626 aligned frames, 69 paired fields, **53 divergent, 16 within floor on every
frame**: `fl[0..3]`, `gb498`, `gb49c`, `gnd`, `gt[0..5]`, `in[1]`, `in[3]`, `susp`.

**First divergence: `b14[0]` and `b14[2]` at frame 2**, the two largest by median |A-B|
(9.97e5 and 1.30e5), with everything else following at frame 2-3. That is the fix's own
signature and nothing earlier precedes it.

**No commanded input moved.** `in[0]`, `in[2]`, `steer`, `p15`, `p16` and `reseed` are all
flagged only because the floor is exactly 0 and the two runs differ in length (1629 vs
1626), and every one of them reports **median |A-B| = 0**. The fix changes the drive force,
not the command — which verifies its charter rather than asserting it.

**Outside-scope rows: none readable.** All 53 rows print `scope = outside`, but
`scope_a6a.txt` is keyed to `msd+0xNNN` names and does not apply to a `kv`-vs-`kv` pairing,
exactly as §28.6 recorded. The column was therefore **not read**, and the classification was
done by hand instead: every divergent field is `b14`, a force (`ftot`, `wf`, `w1b`), a
kinematic consequence (`sp`, `horiz`, `velH`, `bodyH`, `slip`, `d[]`, `wax`, `wle4`,
`wld4`), or `b0c` — all downstream of `b14` on a cited chain. **Nothing diverged that is not
downstream of the drive force.**

## 2 Collateral, cross-side banded

`--mode banded`, A = `orig_solo3.msd`, B = `r1`, floors `orig_solo4.msd` and `r2`, banded on
`msd+0x9e4`. Eight fields pair (the ones whose `kv` name fixes its record offset without
guessing: `sp`, `gnd`, `b14[0..2]`, `b0c`, `gb498`, `gb49c`); 5 of 8 exceed their floor in
some band.

**Exactly ONE band is readable.** Six of the seven bands are flagged `!!` — their two median
frame indices differ by more than 50 % — and per the standing rule those rows were **not
read**. The one unflagged band is **70-100** (median frame A 988, B 1014):

| field | nA | nB | med A | med B | gap / floor |
|---|---:|---:|---:|---:|---:|
| **`msd+0xb0c`** | 10 | 136 | **44.305** | **0.69673** | **19355.6x** |
| `msd+0xb14` | 10 | 136 | -5.669e+05 | +6.974e+05 | 3407.6x |
| `msd+0xb1c` | 10 | 136 | -6.426e+05 | +8.285e+05 | 2244.3x |
| `msd+0xb18` | 10 | 136 | **0** | -0.032206 | 925.5x |
| `msd+0x9e4` | 10 | 136 | 89.608 | 82.735 | 55.0x |

`nA = 10` is small and is stated as a caveat. `+0xb0c` is the **top-ranked row by
gap/floor**, which is the term `PREREG_STEP3.md` §3 pre-committed to taking first.

## 3 The frame-aligned first divergence — the measurement the matched launch unlocked

With the launch matched at `L = 0`, the two arms can be compared **at the same `d`** for the
first time in four attempts. Criterion: the first `d >= 0` at which
`|O - P| / max(|O|,|P|) > 1 %`, on the eight cross-comparable fields.

| field | first `d` | ORIG | PORT |
|---|---:|---:|---:|
| `sp` | 0 | 0 | 0.65 |
| **`+0xb0c`** | **1** | **0** | **0.00831162** |
| `+0xb14` | 15 | -265866 | -270030 |
| **`+0xb18`** | **15** | **0** | **0.0218758** |
| `+0xb1c` | 59 | -1.24075e+06 | -1.22802e+06 |
| `gb498`, `gb49c`, `gnd` | never | | |

`+0xb14` and `+0xb1c` first diverge at the **engagement frame itself** and by **1.57 %** and
**1.03 %** — i.e. the drive force is now the right size on the right frame, which is the
same magnitude agreement STEP 1 recorded against the original's signature.

### 3a `+0xb0c` — confirmed as the first diverging term, and NOT clean

`+0xb0c` diverges at `d = 1`, earlier than anything else with a real value, which confirms
attempt 14's step-2A exploratory row. But it diverges **in both directions depending on
regime**: at `d = 1` the ORIGINAL is 0 and the port is 0.0083 (port high), while in the one
readable band the ORIGINAL is 44.305 and the port 0.697 (port **64x low**). A term that is
too large early and too small later is not a single scale error, and no single-constant fix
can be proposed from it. It stays the named first-diverging term and nothing more.

### 3b `+0xb18` — a BINARY divergence, witnessed on four captures

The drive-force accumulator's **Y component is exactly `0.0` on every frame of every
original capture**:

| capture | frames | `+0xb18` nonzero |
|---|---:|---:|
| `orig_solo3` | 2333 | **0** |
| `orig_solo4` | 2333 | **0** |
| `orig_bp1` (this attempt) | 2334 | **0** |
| `orig_bp2` (this attempt) | 2336 | **0** |
| | **9336** | **0 of 9336** |

The PORT writes it **non-zero on 385 of the 400 post-release frames**.

**This is PRE-EXISTING, not introduced here.** §1's paired table reads `b14[1]` median
`0.009007` on the **pre-fix** arm `s1` and `-0.02774` on `r1`: the port was already putting a
vertical component into the drive force before this attempt touched anything.

It is a better-formed lead than `+0xb0c`: binary rather than a magnitude, 9336 of 9336 on
the reference side, and it points at one factor. A6a's `== 1` arm accumulates
`Wf(v, 0xb18, Rp(p,0x20) * ff + Rf(v, 0xb18))` (`Integrate2.cpp`, from
`0x00467d8f..0x00467d9d` / `0x00467cbd..0x00467ccb`), so a `+0xb18` that is identically zero
on the original means the original's `[edi+0x80]` — the Y component of the per-wheel drive
direction — is identically zero in this scenario and the port's is not. **[UNCERTAIN]**
which producer writes that vector on each side; that is what the live test has to name.

---

## 4 What is NOT done, deliberately

`PREREG_STEP3.md` §3 requires the term to be **tested on the running original before any
fix**, and forbids speculative fixes. Neither term has had that test:

- `+0xb18`'s *reference* side is now witnessed four ways, but the **producer** of the
  non-zero Y on the port side is not identified, and no original-side hook has run on it.
- `+0xb0c` is not even a well-posed single defect yet (§3a).

So no fix is authored, no constant is changed, and no new knob exists. The recipe for the
live test is in `re/NEXT_SESSION.md`.
