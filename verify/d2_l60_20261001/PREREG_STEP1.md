# D2 attempt 13, STEP 1 — PRE-REGISTRATION (written and committed BEFORE any reduction run)

Resolves **[U-9172]**: measure the ORIGINAL's `l_60` with a **verified** call-site
attribution. Nothing below is amended after a number is seen. If a gate fails I
stop and report it.

---

## 1. The nine `RwV3dLength` sites inside A6a, disassembled

Source: `verify/d2_sink_20261001/a6a_disasm.txt`, 1243 instructions,
`0x00467650..0x00468989`, full capstone coverage, no early stop. ESP-delta walk
by `re/tools/statediff/a12_mult.py::esp_walk`, whose sign convention
(`frame = esp_delta + displacement`) is the one §25.6 paid for. **Ghidra arities
are not used anywhere in this section.**

| call RVA | return RVA | esp_delta | instruction at the return |
|---|---|---:|---|
| `0x0046766e` | `0x00467673` | −244 | `fstp dword ptr [esi + 0x9e4]` |
| `0x00467680` | `0x00467685` | −248 | `fstp dword ptr [esi + 0x9e8]` |
| `0x004680f6` | **`0x004680fb`** | −244 | `fstp dword ptr [esp + 0x10]` → **frame −228** |
| `0x0046820a` | **`0x0046820f`** | −244 | `fst dword ptr [esp + 0x20]` → frame −212 (**kept in ST0**) |
| `0x0046833e` | `0x00468343` | −244 | `fcomp dword ptr [0x5cd03c]` (= 9.99999975e-05) |
| `0x004684bb` | `0x004684c0` | −244 | `fmul dword ptr [0x5cea04]` |
| `0x004684d7` | `0x004684dc` | −248 | `fcom dword ptr [0x5cd03c]` |
| `0x004685b7` | `0x004685bc` | −244 | `fsubr dword ptr [esp + 0x24]` → frame −208 |
| `0x004686a4` | `0x004686a9` | −244 | `fstp dword ptr [esp + 0x24]` → frame −208 |

Instrument self-check, and it passes before any new work: `0x004686a9` resolves
to **frame −208** and `0x004687db` (`fmul [esp+0x20]`, esp_delta −240) resolves
to **frame −208** as well — reproducing §25.3's "same slot" correction, and
`0x0046820f` resolves to **frame −212**, reproducing `a12_mult.py`'s refutation.

## 2. WHICH site feeds `l_60`, proven by RVA

`l_60` is the slot the clamp divides at `0x004686b3 fld dword ptr [esp + 0x94]`,
esp_delta −244 ⇒ **frame −96**. Over the whole of A6a, frame −96 has **exactly
four** accesses:

| RVA | esp_delta | instruction | role |
|---|---:|---|---|
| `0x00467b34` | −240 | `mov dword ptr [esp + 0x90], 0` | **init**, `l_60 = 0` |
| `0x00468220` | −240 | `fadd dword ptr [esp + 0x90]` | accumulate, read |
| `0x0046822b` | −240 | `fstp dword ptr [esp + 0x90]` | accumulate, write |
| `0x004686b3` | −244 | `fld dword ptr [esp + 0x94]` | the clamp's read |

There is **one** accumulate site and the straight-line code into it is:

```
0x004680f6  call 0x4c3ac0            ; RwV3dLength  -> le4
0x004680fb  fstp [frame -228]
   ...
0x0046816c  mov  [frame -228], 0x44800000    ; = 1024.0, on one branch
   ...
0x0046820a  call 0x4c3ac0            ; RwV3dLength  -> ld4, LEFT IN ST0
0x0046820f  fst  [frame -212]        ; (a copy; ST0 survives)
0x00468213  fmul [frame -228]        ; ST0 = ld4 * (frame -228)
0x00468220  fadd [frame -96]
0x0046822b  fstp [frame -96]         ; l_60 += ld4 * (frame -228)
```

> **The site that feeds `l_60` is `0x0046820f`, multiplied by the slot
> `0x004680fb` writes. Those are exactly the two sites §21.9's `a8_l60.py`
> used.** So U-9172's stated suspicion — "the likely error is call-site
> misattribution" — is **refuted by the disassembly before any run**. The nine
> sites are distinguishable and §21.9 picked the right two.

**[UNCERTAIN]** frame −228 is a reused scratch slot with 22 accesses across A6a,
including `0x0046816c` writing the literal `1024.0`. Whether the value at
`0x00468213` is `le4` or `1024` on a given wheel depends on a branch not read
here. §21.9's `min(le4, 1024)` is consistent with both, and the probe measures
`le4` at `0x004680fb` either way, so this does not change the attribution — it
is recorded because it bounds how exactly the ORIGINAL's `l_60` can be
reconstructed from the two magnitudes.

No register is ever aliased to ESP in a way that could hide a fifth access: the
9 `lea reg,[esp+N]` sites are at `+0x14`, `+0xbc`, `+0xc0`, `+0x2c`, `+0x10`,
`+0x44`, `+0x10`, `+0x70`, `+0x78`, none of which is `+0x90`/`+0x94`
(memory `findoffset-blind-to-computed-bases`, `offset-grep-misses-dword-index`).

## 3. CORRECTION, read out of the disassembly before the measurement: clamp #6 is a LATERAL damper, not a speed clamp

`0x00468771..0x004687d7`:

```
D   = fwd.x*vel.x + fwd.y*vel.y + fwd.z*vel.z           ; 0x00468771..0x00468793
frame -224 = vel.x - D*fwd.x                            ; 0x004687b5..0x004687b9
frame -220 = vel.y - D*fwd.y                            ; 0x004687bf..0x004687c9
frame -216 = vel.z - D*fwd.z                            ; 0x004687cd..0x004687d7
```

These three are the **component of the velocity perpendicular to the body
forward axis** — the LATERAL velocity — and both arms then write
`vel -= k * lateral` (`0x00468819..0x00468854`, `0x004688b0..0x004688e8`). The
port's `Integrate2.cpp:657-660` is the same expression, so this is a correction
to how §25.3's own evidence must be read, not to the port.

Constants, read from `original/MASHED.exe.unpatched` through the PE section
table (memory `pe-rva-mapping-field-order`), hex first (memory
`plate-hex-gloss-authoritative`):

| address | bytes | f32 | role |
|---|---|---|---|
| `0x005ce9fc` | `00 00 00 47` | 32768 | arm knee, `fcom` at `0x004687df` |
| `0x005ce9f8` | `80 96 18 4b` | 1e7 | HIGH arm `fsubr` at `0x004687f0` |
| `0x005ce9f4` | `95 bf d6 33` | 1.00000001e-07 | HIGH arm `fmul` at `0x004687f6` |
| `0x005d757c` | `00 00 00 00` | 0 | HIGH arm floor, `0x004687fc` |
| `0x005cc56c` | `cd cc cc 3d` | 0.100000001 | `*0.1` twice = `*0.2`; LOW arm floor |
| `0x005ce9f0` | `00 00 00 38` | 3.05175781e-05 = 2^-15 | LOW arm `fmul` at `0x00468895` |

So, with `G = grip * |vel|` (`0x004687db`):

```
HIGH (G > 32768):  k = max(0, (1e7 - G) * 1e-7) * 0.2        ; k in [0, 0.2]
LOW  (G <= 32768): k = max((32768 - G) * 2^-15, 0.1)         ; k >= 0.1 ALWAYS
```

Gates, both of which must pass for the clamp to run at all:
`0x0046874c` `|vel| != 0`, and `0x00468761` `+0x9e0 == 0x40800000` (all four
wheels grounded).

### The identity that makes §25.3's no-op readable

With `lat ⟂ fwd` and `v·lat = |lat|²`, writing `s = |lat| / |v|`:

> `|v'| / |v| = sqrt(1 - (2k - k²) · s²)`, so for small `k`,
> `1 - |v'|/|v| ≈ k · s²`.

§25.3 measured `+0x9e4 / |v'| ∈ [0.999991, 1.000020]` with coverage 2331/2331,
i.e. **`k · s² ≤ 2.0e-5`**. That bound constrains the PRODUCT. §25.3 read it as
a bound on `k` alone and therefore concluded `l_60 ≥ 79 240`; **it is equally
satisfied by a vanishing lateral `s`**, which was never measured. Deciding
between those two is what this step does.

## 4. The attribution method, and why it needs no new game run

`re/frida/scenario_launch.py --mag-probe` is already an **entry** hook on
`0x004c3ac0` that reads the vector **by pointer** and tags each row with
`this.returnAddress` — exactly the method U-9172 asks for (memory
`frida-interceptor-is-entry-only`, `grep-the-harness-for-the-rva-before-writing-a-probe`).
Its measurement run already exists:

- `verify/d2_magpr_20260930/read1.msd` + `.magprobe.csv` — all nine sites,
  `err = null`, cap auto-detached as designed.
- `verify/d2_bounce_20260930/orig_fp2.msd` + `.fixupprobe.csv` — the reference
  `.msd` §25 used, carrying `velx/vely/velz`, `speed`, `gnd`, **`fwdx`, `fwdz`**
  per probe site.
- `verify/d2_sink_20261001/arm/a6a_dump.log` — the PORT's `act.l60`,
  `act.grip`, `act.kvel`, `act.arm`, `act.clamp` **logged directly**
  (`VehiclePhysicsRun.cpp:1313-1316`, `Integrate2.cpp:653-739`), not
  reconstructed.

**Count-first safety gate.** Satisfied in advance and not re-litigated: §21.8's
count-only run measured `R <= 3949 calls/s` and completed a full 38 s race with
a non-degenerate 2330-frame capture and `err = null`, and §21.9's reading run
then completed with the row cap auto-detaching. **No new game is launched in
step 1.** If a gate below fails, the remedy is a new run, and that run will be
pre-registered separately rather than improvised here.

## 5. GATES. Each must pass or I STOP and report it.

- **G1 — capture integrity, re-verified by me, not inherited.** In
  `read1.msd.magprobe.csv`: all nine sites populated; `n >= 1000` at site
  `004686a9`; and the **known-answer self-check** — site `004686a9`'s vector
  equals the record's own `+0x9b0`/`+0x9b8` on `>= 99%` of its rows. If it
  fails, the return-address tagging is unsound and the section is void.
- **G2 — pairing, with a count not a median.** The reducer delimits frames by
  site `00467673` and reports the full per-frame count distribution for
  `004680fb` and `0046820f`, plus the number of frames where the two counts
  disagree. No frame is silently truncated (memory
  `zero-of-n-needs-a-coverage-check`).
- **G3 — the PORT channel is direct, known-answer.** `act.l60 * act.speed`
  must equal `act.grip` to relative `<= 1e-5` on `>= 99%` of rows where
  `act.clamp == 1`. This checks my reading of `Integrate2.cpp:653`/`:661`/`:721`
  against the log itself. If it fails, the port comparison is void.
- **G4 — the horizontal projection is legitimate.** On the original,
  `fwdx² + fwdz² ∈ [0.99, 1.01]` on `>= 99%` of the rows used, so computing the
  lateral from x/z alone (the `.fixupprobe.csv` carries no `fwdy`) is sound.
  And `n >= 100` in the 100-150 band. If either fails, `s_o` is not measured and
  only D4 below is reported.

## 6. THE MEASUREMENT

Per speed band (`a11_accum.BANDS`), **every row carries `n` and the median
speed**, grounded frames only (`gnd == 4.0`):

- ORIGINAL: median `l_60 = Σ_wheels mag@0046820f × min(mag@004680fb, 1024)`,
  median `l_60 × speed`, and median `s = |v_h − (v_h·f_h) f_h| / |v_h|` taken at
  the first `--fixup-probe` site-1 sample after each site-2 row (the same
  first-after-A6a pairing `a12_clamp.py` implements, with its coverage count).
- PORT: median `act.l60`, `act.grip`, `act.kvel`, the `act.arm` histogram, and
  `s` computed from `snap.vel` / `snap.bf` the same way.

## 7. THE DECISION RULE, fixed in advance

Let `s_o` be the ORIGINAL's median `s` in band **100-150**, measured POST-clamp.
Pre-clamp `s_pre = s_post / (1 − k) ≤ 1.25 · s_post` for any `k ≤ 0.2`.

- **D1 — the no-op is the LATERAL, not `k`.** Fires if `1.25 · s_o ≤ 0.0112`,
  i.e. even the LOW arm's floor `k = 0.1` would keep `k·s² ≤ 2.0e-5` and sit
  inside §25.3's measured interval. Then **§25.3's inference
  "no-op ⇒ k ≈ 0 ⇒ `l_60` ≥ 79 240" is UNFOUNDED**, U-9172's contradiction
  dissolves, §21.9's `285.2` stands on a now-verified attribution, and the
  `l_60` lane closes with a named cause. The next lane is the lateral itself.
- **D2 — `k` really is ≈ 0 on the original.** Fires if `s_o ≥ 0.03`, which
  forces `k ≤ 2.0e-5 / s_o² ≤ 0.0222`. That is below the LOW arm's 0.1 floor, so
  the original must be on the HIGH arm with
  `G ≥ 1e7 − 2.0e-5/(s_o² · 2e-8)`; at `s_o = 0.03` that is `G ≥ 8.889e6`, i.e.
  `grip ≥ 70 440` at speed 126.2 — **≥ 247x** §21.9's `285.2`. Then the conflict
  is in the probe's VALUE, not its site, with the site now verified. **Report it
  and do not fit.**
- **D3 — anything between** (`0.0112/1.25 < s_o < 0.03`): report with `n` and
  medians, author nothing.

Independently of D1/D2/D3, always reported:

- **D4.** The PORT's `act.arm` histogram and median `act.kvel` per band. If the
  port sits on the LOW arm (`k ≥ 0.1`) where the original's `k` is bounded
  above by G4's measurement, that is the cross-side mechanism by which the port
  destroys its lateral, stated as a measurement rather than an inference.

## 8. What this section does NOT change

The §3 bounds (`slip 1500-2000` 0.18855..0.19635, `slip 2000-2600`
0.24488..0.25487, `driving-median` 1904.70..1982.44), the §16.7 arm, the 3-run
precondition. **Step 1 authors no physics change.** Step 0's `+0x1a8` finding is
exploratory and is NOT an input to any rule above.
