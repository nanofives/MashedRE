# The exe/asi gap, triaged — and the triage's own headline is CORRECTED below

> ## CORRECTION, same day, before this document was used for anything
>
> **The original headline — "280 verified functions are plausibly a build-list change away" — was
> wrong in its implication, and the project had already rejected the strategy it points at.** Two
> things I measured on the wrong field:
>
> 1. **`RH_ScopedInstall` is a NO-OP in the exe.** `HookSystem::Register` resolves to
>    `Stubs/HookSystemNoOp.cpp:19` — a file in `exe_sources.rsp` **only**. So in the standalone a
>    linked reimplementation registers nothing and is a **dead export unless the standalone call
>    graph invokes it by name** (`ROADMAP.md:226-228`, `:235-237`). **Adding a TU to
>    `exe_sources.rsp` makes its body PRESENT, not LIVE.** My `CHEAP` class measured "nothing stops
>    it linking", which is not the same question.
> 2. **`hooks.csv` has an `exe_file` column I did not use.** I keyed on `file` plus the `.rsp`
>    lists; `exe_file` is the authoritative "has an exe-side body" field. Re-measured on it:
>
>    | | C3 | C4 | **C3+C4** |
>    |---|---:|---:|---:|
>    | ported, **has** exe body | 319 | 74 | **393** |
>    | ported, **no** exe body | 695 | 102 | **797** |
>
>    Shipped share of verified work by subsystem: RenderWare-Physics **99 %**, frontend **69 %**,
>    boot 49 %, hud 40 %, vehicle 38 %, util 31 %, ai 29 %, render 23 %, save 17 %, particle **8 %**,
>    gameplay **5 %**, audio **1 %** (1 of 142), input **0 %** (0 of 9).
>
> **And the bulk-link strategy is already settled against**, in writing: *"the real blocker is not
> linkage, it is that this code is hook-shaped … Bulk-adding the class-B files would grow the binary
> and the tracker without shipping one working feature"* (`ROADMAP.md:235-240`).
>
> **So there are THREE states, not two**, and the record already names them: (i) not reversed,
> (ii) reversed and ported but no exe body, (iii) **exe body present with zero call sites**. My
> triage conflated (ii) and (iii). The proof that (iii) is real and not hypothetical:
> `Collision/CarCarContacts.cpp` **is** in `exe_sources.rsp` with a byte-faithful body for
> `0x00469df0`, and that RVA has **zero call sites** (`ROADMAP.md:1876-1877`, `:1931`) — the same
> shape as `BodyOrient_OmegaFromAngVel`, found earlier in this session.
>
> **What survives below:** the `.rsp` counts (221 / 422 / 255), the `/DMASHED_STANDALONE`
> mechanism, the address-range table, the `RH_ScopedInstall` miscount fix, and the
> `NEEDS-STORAGE` / `BLOCKED-*` classes as a map of *linkage* obstacles. **What does not survive:**
> the reading of `CHEAP` as "ready to ship", and the suggestion to bulk-add it.
>
> **The real lever is CALL SITES, not build-list entries.**

## Original document follows, with the headline above superseding its framing.

# The exe/asi gap, triaged — 280 verified functions are plausibly a build-list change away

**MEASURED 2026-10-05.** Tool `re/tools/asi_only_triage.py`, data
`verify/asi_triage_20261005/triage.csv`. **No C-level moved, no band moved, no code changed, no
build run.** Static analysis of committed sources plus `hooks.csv`.

## 1. The gap, which was not tracked in aggregate before

`mashed_re.exe` is the deliverable; `mashed_re_dev.asi` is dev-only and **never shipped**
(`CLAUDE.md`). Of **422** ported TUs, **221** are in `exe_sources.rsp` and **255 are in
`asi_sources.rsp` only**. Of the **888** C3/C4 rows that have a source file, **619 (70 %) build only
into the dev `.asi`.**

This existed in the trackers only as individual instances — `DEFERRED.md` `D-11069` (4 duplicate-RVA
rows), `ROADMAP.md:2521` (one case), U-9187 (three AI TUs). **Nobody had counted it.**

## 2. The discriminator comes from the build, not from taste

The exe is compiled `/DMASHED_STANDALONE` (`mashedmod/build.bat:210`, `:212`), and dual-target TUs
use `#ifdef MASHED_STANDALONE` to swap absolute original-image VAs for private storage —
`Gameplay/PickupPoolSpawn.cpp:79,118,141,149` is the worked example. So what keeps a TU out of the
exe is the set of absolute original-image addresses it still touches **in code**:

| range | state in the standalone | consequence |
|---|---|---|
| `0x00400000..0x004fffff` | **unmapped** (`Compat/StandaloneRvaThunks.h:7`; an access AVs, `Frontend/MenuButtonDetect.cpp:71`) | a call needs an exe-side body or a thunk |
| `0x00500000..0x009fffff` | **VirtualAlloc-mapped blank** (`exe_main.cpp:54`) | reads return 0 — an **inertness** risk, not a crash. U-9187's root cause |

**Comments are stripped before scanning, and that is not a detail:** every ported function cites its
RVA in a `// 0x00xxxxxx` comment, so an unstripped scan flags ~100 % of TUs.

## 3. One correction to this tool, made before its numbers were reported

The first run classed **197 TUs / 415 verified rows** as `BLOCKED-CODEADDR`. That was wrong by ~4x.
Of the **662** original-`.text` code references across them, **471 (71.1 %) are `RH_ScopedInstall`
arguments**, and **163 of the 197 TUs have no other kind**. A hook registration names the *original*
address a body replaces — it is the `.asi` install mechanism and is meaningless in a standalone that
has nothing to hook. Those are now excluded and counted separately as `n_install_only`.

## 4. The triage

| class | TUs | rows | C2 | C3 | C4 | **C3+C4** |
|---|---:|---:|---:|---:|---:|---:|
| **CHEAP** — no original-image code ref outside hook registration, no blank-mapped data ref | 149 | 287 | 7 | 246 | 34 | **280** |
| **NEEDS-STORAGE** — only blank-mapped data refs; the `PickupPoolSpawn` recipe applies | 49 | 137 | 2 | 117 | 18 | **135** |
| **BLOCKED-CODEADDR** — real original-`.text` refs beyond installs | 34 | 92 | 3 | 61 | 28 | 89 |
| **BLOCKED-CALLOUT** — function-pointer casts into unmapped `.text` | 7 | 18 | 7 | 8 | 3 | 11 |
| **DEV-ONLY-BY-DESIGN** — the TU exists to observe the original | 16 | 115 | 11 | 101 | 3 | 104 |
| **TOTAL** | **255** | **649** | 30 | 533 | 86 | **619** |

### CHEAP, by subsystem (C3+C4 rows)

render **75** (37 TUs), gameplay **48** (26), util **36** (22), frontend **23** (9), ai **22** (15),
vehicle **14** (11), boot **12** (8), audio **10** (7), save **10** (5), hud 7 (6), particle 7 (7),
input 4 (4), smplfzx 4 (3), camera 3 (2), track 2 (1), world-objects 1, sky 1, unknown 1.

### NEEDS-STORAGE, by subsystem (C3+C4 rows)

**audio 80** (19 TUs), util 16 (7), render 8 (5), gameplay 8 (6), vehicle 6 (4), save 6 (2),
hud 5 (1), frontend 5 (1), particle 1 (1).

**So audio is the EXPENSIVE class, not the cheap one.** An earlier verbal suggestion in this session
that audio be the pilot "because it cannot regress (b)/(e)" was **wrong on the facts**: 80 of audio's
~109 asi-only verified rows need private storage for blank-mapped globals, and only 10 are CHEAP.

## 5. What "CHEAP" does and does NOT mean — read this before acting on 280

**It is a STATIC verdict**: the TU references no original-image code address outside hook
registration and no blank-mapped data address. It does **not** prove the TU links and runs in the exe.
Three things it cannot see:

1. **Transitive dependencies** — a CHEAP TU may call another port function whose only body is in an
   asi-only TU. Not measured.
2. **Initialisation order** — it may read a global that an asi-only TU populates.
3. **Link-time symbols** — unresolved externals only appear at link.

**The decisive test is therefore a LINK ATTEMPT, not more static analysis.** Adding a CHEAP subset to
`exe_sources.rsp` either links or it does not, and that answer costs one build. That beats refining
this tool.

**And it must be incremental, with the standing no-regression discipline**: after any subset lands,
(e)'s two gated stats must be unchanged on all three cars (`launch` 1426.4 / 2053.0 / 2055.2,
`ft_median_m0` 2550.6 / 2053.0 / 2278.2) and (b) must not regress past 13 of 30 bands, with the band
files proven unedited. 280 functions newly live in the shipping binary is not a change to make in one
step.

## 6. Why this matters beyond the count

Three of the four dead-ends in this session's own work were this gap or its sibling, not missing
reverse-engineering:

- **U-9187** — three AI TUs asi-only; wiring measured inert **and** unsafe.
- **Mode 7 / modes 3/7** — blocked because world-object globals read `.bss` zeros, i.e. the
  blank-mapped region.
- **`BodyOrient_OmegaFromAngVel`** — a written, correct function with zero call sites.

Only `FUN_00446520`'s 8,645 bytes was genuinely missing reversing. **The binding constraint on
visible progress is wiring, not reversing throughput** — which is a different claim from "the big
systems are unported", and the 619 is the evidence for it.

## 7. Caveat on `DEV-ONLY-BY-DESIGN`

That class is **keyword-detected** (`asi-only`, `dev-only`, `reads live globals`, …) and is the
least reliable row in the table. 16 TUs / 104 verified rows is an upper bound on what is deliberately
dev-only among the clearly-marked ones; TUs that are dev-only without saying so land in the other
classes. Treat it as a hint, not a measurement.
