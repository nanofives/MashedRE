# Matching-compiler spike — 2026-09-09

**Question asked:** can we compile the port with a period MSVC and get machine code
byte-identical to `MASHED.exe`, the way `reccmp`-style decomp projects do? That would be
static, offline, per-function evidence needing no Frida, no arg_type, no live game — and it
would bypass every harness-safety class that blocks 52 of the 88 ranked C2 frontier rows.

**Answer, in one line:** not with a modern toolchain (measured below, 3/3 functions), and
**not yet settled for a period toolchain** — acquiring one is blocked, see §4. The spike
produced a working instrument anyway (§3), which is the part worth keeping.

---

## 1. Toolchain hunt — BLOCKED

Every `cl.exe` on this machine is modern: `19.29.30159`, `19.30.30715`, `19.44.35223`,
`19.50.35721` (VS 2019 / 18 / 2022 BuildTools). No 13.x anywhere.

`MASHED.exe` is linker 7.0 = **MSVC .NET 2002, i.e. `cl` 13.00.x** (CLAUDE.md, build stamp
`Jun 14 2004 11:39:38`).

Acquisition attempts:

| route | result |
|---|---|
| `VCToolkitSetup.exe` from archive.org (31.4 MB, SHA-256 `03aad135c22e953e0928b118705338afdbd08abf8e4039038ef77945504e65fa`) | downloads fine; **`IDS_ERROR26` from the InstallShield wizard on Win11** — the 2003 InstallShield 6 stub does not run on modern Windows (same wall virtuallyfun hit on Win8/10) |
| carve the installer payload instead of running it | overlay `[0]` is 32.7 MB, header `InstallShield`; **no OLE/MSI, CAB, ZIP or zlib signature anywhere** — IS6-compressed, not carvable with 7z/`unshield` |
| `vc2003toolkit.7z` pre-extracted mirror (vpsland/superglobalmegacorp) | **404** (returns a 5 KB HTML error page) |

Note the Toolkit is 7.1 (`13.10.3052`) anyway, one release *after* the target.

**Untried, and the most promising route:** the DDKs ship the same compilers as standalone
files, no InstallShield.
- **Windows XP DDK (2600)** → `cl` **13.00.9466** — the matching major version.
- Windows Server 2003 DDK (3790) → `cl` 13.10.3077 (= Toolkit-era).

`[UNCERTAIN]` whether the DDK `cl` 13.00.9466 is codegen-identical to the retail VS .NET
2002 compiler Supersonic used. Nothing in this spike tests that.

## 2. Baseline measured: modern MSVC vs the original

Since the period compiler was unavailable, the useful fallback measurement is *how far
modern MSVC is*, which is the bar a period compiler has to beat.

Three **C4** functions (already verified-verbatim, so any divergence is the compiler, not
the port), transcribed to C++98 in `spike.cpp`, built
`cl /O2 /Gy /GS- /arch:IA32` (x86, `19.44.35223`):

| RVA | function | ours | orig | instrs | aligned | verdict |
|---|---|---:|---:|---|---:|---|
| `0x0046dbe0` | `VehicleRacePositionGet` | 15 B | 17 B | 3 vs 4 | 50% | PARTIAL |
| `0x00424920` | `EndOfRoundAccumulator` | 513 B | 608 B | 97 vs 129 | 18% | PARTIAL |
| `0x00423e60` | `PlayerScoreTeamAccCumB` | 131 B | 127 B | 43 vs 34 | 7% | PARTIAL |

`/O1` was also tried on the smallest function: identical output to `/O2`.

**Every divergence found is codegen-era, none is logic.** Three distinct mechanisms:

1. **Instruction fusion.** Ours `imul eax, [esp+4], 0xd04`; the original
   `mov eax,[esp+4]` then `imul eax, eax, 0xd04`. Same result, 2 bytes shorter.
2. **Scheduling / register pressure.** `EndOfRoundAccumulator` is 32 straight
   `*dst += *src` statements. Modern MSVC emits a tight 3-instruction pattern per add,
   reusing `eax`. MSVC 7.0 **software-pipelines** across `eax`/`ecx`/`edx`, hoisting loads
   ahead of stores — 4 instructions per add and 95 bytes *longer*. That is Pentium-4-era
   scheduling, and it is not reachable by editing our source.
3. **Register allocation and CSE.** In `0x00423e60` ours spills to `esi`/`edi` (so it pays
   a `push`/`pop` prologue); the original stays entirely in the volatile `eax`/`ecx`/`edx`.
   Ours hoists `imul …,0x138` above the branch (computed once); the original computes it
   in both arms.

The original is *larger and less tight* than modern output in 2 of 3 cases. This is not a
case of our source being wrong or unoptimised.

## 3. The dividend: operand correspondence (works TODAY, no period compiler)

The three mechanisms above all move registers and scheduling around. What they never touch
is **which absolute addresses the function references and which immediates it uses** —
those come from the source. So comparing those two sets across a 20-year compiler gap is
sound, and it catches exactly the transcription-defect class that has bitten this project
(U-9085: a C3 body that did not match its RVA; `[[wrong-plate-propagates-into-ports]]`).

Implemented in **`re/tools/matchdiff.py`** (COFF parse + capstone + PE read; no build-system
coupling). Results:

```
0x0046dbe0  addresses  1/1   immediates 1/1   PASS
0x00424920  addresses 64/64  immediates 0/0   PASS
0x00423e60  addresses  8/8   immediates 5/5   PASS  (multiplicity differs — CSE, advisory)
```

**Non-degeneracy proven, not assumed** (per `[[feedback_evidence_discipline]]`). Two
deliberate single-token defects were injected and both were caught:

```
stride 0x341 -> 0x340    immediates 0/1 MISMATCH  only OURS 0xd00 / only ORIG 0xd04     FAIL
base 0x899f7c -> 0x899f80  addresses 4/8 MISMATCH  4 addresses shifted by 4              FAIL
```

The second is the interesting one: one wrong base propagated into 4 distinct wrong
addresses, all flagged.

**Calibration finding.** The gate must compare the **distinct set**, not the multiset. On
`0x00423e60` the multiset differs (original computes `0x138` twice, one per branch; MSVC
2022 hoists it) — a multiset gate calls a known-good C4 function defective. Multiplicity is
reported as advisory only. This is calibrated against known-good code, which is why the
false positive was caught rather than shipped.

## 4. Verdict and disposition

- **Matching decompilation with the current toolchain: NO.** Do not pursue byte-matching
  against `cl` 19.x; the divergence is structural to the compiler generation.
- **With a period `cl` 13.00: UNTESTED.** The evidence is *encouraging* — all three
  divergence mechanisms are compiler-version artifacts and the logic already corresponds
  exactly — but that is a hypothesis, not a result. Filed as **TT-1** in
  `re/TOOLING_TODO.md`, gated on acquiring a DDK-sourced `cl` 13.00.9466.
- **Operand correspondence: ADOPT NOW.** Filed as **TT-2**. It is a static check runnable
  over all 918 C3 + 180 C4 rows with no game, no Frida and no period compiler. It does not
  replace a `diff-original` Frida diff and **moves no C-level on its own** — it is a
  transcription-defect *detector*, i.e. it can demote, and it can pre-screen a batch before
  spending harness time.

## Artifacts

- `re/tools/matchdiff.py` — the comparison tool.
- Spike sources / objects: scratchpad `…/scratchpad/vc71/` (`spike.cpp`, `spike_o2.obj`,
  `spike_bad.cpp` negative control). Not committed — regenerable from this note in minutes.
- Reproduce: `py -3.12 re/tools/matchdiff.py <obj> <symbol> <rva> <size>`
  (defaults to `original/MASHED.exe.unpatched`, the SHA-256-anchored reference).
