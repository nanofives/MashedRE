# STATE lane — where the dead globals actually populate (2026-07-29)

> ## ⚠️ TWO CORRECTIONS, IN ORDER — read both
>
> **1. I claimed the game was hung. That was WRONG.** I read
> `verify/capture_20260729/sb_diffpoint.png`, saw a **blank white window**, and concluded every
> diff point this session had been taken against a hung game rather than a race.
>
> **2. The stock control disproved it.** `re/frida/statenav.py` run stock reached a race
> demonstrably — `0x00436810` fired **1961 times**, `0x0045ba00` 30, `0x00408a70` 16,
> `results_hit=1` with `first_results_at=20` — and **its own `sn_round_10.png` is equally white.**
> The menu captures fine; the race does not. The window grab misses the D3D9 backbuffer.
>
> **So a white screenshot is evidence of nothing either way**, and I invalidated six boots of work
> on the strength of one. The runtime findings below are *not* known to be wrong — they are
> *unproven*, which is where they started.
>
> **What the episode actually establishes:** we had no instrument telling us whether the diff point
> was inside a running race, and a screenshot is not one. Added: an **in-race entry counter** at the
> diff point (`--inrace-probe`, default `0x00436810,0x0045ba00,0x00408a70`), counted by RVA so our
> inline JMPs cannot hide it. This is memory `feedback_evidence_discipline` §1 — *prove the path
> ran* — applied to the scenario itself rather than to a function. It should have been there from
> the first batch.
>
> The STATIC half below never touched a running game and is unaffected by any of this.
>
> ---
>
> ## 3. With the counter in place, the answer is unambiguous — and it is bad
>
> | run | in-race probes (1.5s) | conclusion |
> |---|---|---|
> | `statenav.py`, stock | `0x00436810` = **1961**, `0x0045ba00` = 30, `0x00408a70` = 16, results at t+20s | a real race |
> | `run_diff_scenario_batch.py`, passive 25s dwell | **0 / 0 / 0** | not a race |
> | `run_diff_scenario_batch.py`, ACTIVE 45s dwell driving statenav's own control cycle | **0 / 0 / 0** | not a race |
>
> **The batch lane's diff point has never been inside a running race.** It sits at `phase=0`
> indefinitely, while statenav progresses `0 → 4 → 5`. So `phase=0` is the *pre-race / loading*
> state, not the arena, and `--scenario race` returns there.
>
> **This invalidates every live-state verdict the lane has produced**, back to and including the
> first batch on 2026-07-28 — the "48 calls in 1.2s, state reuse confirmed" measurement and every
> "this array is empty" conclusion. What survives is only the hooks that never needed live state:
> `heading_atan2` (seeded float inputs) and `RenderState_GetTexturingOverride` (echoes its own
> argument). The `0x004233e0` C3 promotion rests on seeded inputs and is unaffected.
>
> **Disproved en route:** input-driving is not the difference. Copying statenav's exact control
> cycle into the dwell changed nothing (0/0/0 either way), so the arena is not waiting on a
> keypress.
>
> **Next, and it costs no boots:** diff the two navigation paths. `statenav.main()` reaches a race;
> `run_diff_scenario.drive_to_results(scenario='race')` does not, from the same menu position. One
> candidate worth checking first — `drive_to_results` may leave an input-override installed on
> `FUN_00497310` that the loader then cannot get past, which statenav's own loop would not hit.
>
> Also unexplained and logged, not theorised about: with the active dwell the repeat control went
> **first=GREEN, last=RED 1/12**, and the runner correctly reported DEGRADED.


Follow-up to `c2c3_throughput_session_2026-07-28.md`. Three authored STATE ports all failed
to produce evidence, every one because the global it reads was empty at our attach point. This
note answers *why*, statically, without spending a boot.

**Headline: two of the four "unpopulated" globals were never unpopulated.** The real defect is
the same one three times over — **a pointer parameter described as an int**, driven with
`int_scalar` vectors, producing a both-sides AV that looks like state absence.

---

## Method note: the linear sweep was broken, and it cost a candidate

`capstone.disasm()` **stops at the first undecodable byte**. A linear sweep of `.text`
(0x1c98e0 bytes) yielded **16,236 instructions** and then silently stopped — under 5% of the
section. Every scan built on it under-reports.

- It reported `0x00407600` had **ZERO callers**, and I dropped that candidate for failing the
  caller gate. **Retracted:** a byte-accurate `E8 rel32` scan finds **two callers**, `0x00446520`
  and `0x00464a50`, **both C2**. The gate passes. It is a valid candidate.
- It reported zero references to all five globals below; the byte search finds 26.

**Use a byte search for absolute-address operands, or an `E8` scan for calls.** Never a bare
linear sweep. (The `call_targets.json` artefacts in `scripts/ghidra/` came from Ghidra and are
not affected.)

---

## Who writes each global

| Global | Written at | Inside | Reached from |
|---|---|---|---|
| `0x007dc8d8` | `0x0057c28b` | `FUN_0057c270` | `FUN_00493640` ← `FUN_00493710` ← **SubsystemInit `0x00492270` (C4)** — boot |
| `0x006c2fe8` camera node count | `0x0047c152` | `FUN_0047c0f0` | `FUN_00426e10` ← `FUN_0040d020` ← **`Course::Finish 0x0040d270`** — track load |
| `0x006c2fa8` camera sub-counts | `0x0047c191` | `sub_0047c160` | `sub_00426ab0` ← **`Race::Tick 0x0040fc00`** — per tick |
| `0x0068432c` record array | `0x0044a051` | `FUN_00449bb0` | `FUN_0044f830`, which has **no static callers** — reached indirectly (vtable / fn-ptr table) |
| `0x006e71cc` smplfzx mgr | — | — | **no absolute store anywhere in `.text`** — written through a computed pointer |

## `0x007dc8d8` is a RenderWare plugin offset, not a table base

```
0057c270  push 0x4d7ff0 / push 0x4d7ff0 / push 0x57c2e0   ; dtor / copy / ctor
0057c27f  push 0x901                                       ; plugin id
0057c284  push 4                                           ; plugin data size
0057c286  call 0x004e7d40                                  ; RegisterPlugin
0057c28b  mov  dword ptr [0x007dc8d8], eax                 ; -> DATA OFFSET
```

So `0x0057c210` is `*(obj + pluginOffset)` — **its argument is an `RwObject*`**, and the
hooks.csv name `RwpBodyTableLookup` is misleading. The measured value `0xa4` is **correct and
healthy**, registered at boot.

Which means the observed faults at `0xa4 / 0xa8 / 0xac` for inputs `0 / 4 / 8` are exactly
`arg + 0xa4` — a null object pointer, not a dead table.

**I got this wrong first and committed the wrong fix.** `c9de9f78` added a `min: 0x00400000`
gate on that global on the theory that "a base must look like a pointer"; that would have
permanently skipped a working hook. Reverted. The `min` gate form remains available but the
docstring now warns to confirm what a global *holds* before demanding it look like a pointer.

---

## Standing pattern: pointer parameters described as ints

Three for three in this batch, all producing identical both-sides AVs:

| hook | plate/registry said | actually | fault |
|---|---|---|---|
| `0x0047c270` | `param_1=0 sentinel` | sphere ptr (centre +0x0) | AV `0x0` |
| `0x0047c2d0` | `param_1=0 sentinel` | sphere ptr (radius +0xc) | AV `0xc` |
| `0x00421930` | handle / byte offset | `RwObject*` (plugin at +0xa4) | AV `0xa4,0xa8,0xac` |

**The fault address is the diagnosis.** It equals the argument plus the first field offset the
callee touches, so it names the struct member directly. Read it before concluding "state absent".

---

## Where each candidate actually stands

- **`0x0044b000`** — array populated by `FUN_00449bb0`, reached only indirectly. Attach point
  unknown; the `any_nonzero` gate correctly skips until it fills. Needs a runtime probe to find
  when, since static tracing dead-ends at an indirect call.
- **`0x00421930`** — state is fine; needs a **live object pointer**, i.e. a new arg_type that
  harvests one rather than seeding a scratch buffer. A seeded buffer cannot work: every dword is
  non-zero so `*(buf+0xa4)` is garbage and the second deref faults; a zeroed buffer returns 0 on
  the first null check and is degenerate. **Parked, honestly.**
- **`0x004b4050`** — genuinely degenerate without a real clump pointer. Same missing arg_type.
- **camera pair** — sub-counts are written from `Race::Tick`, so they *should* be live in a race;
  at race-entry+25s they were still 0, so the write is conditional on something further in.
- **`0x006e71cc`** — no absolute store exists, so no static answer. Runtime watchpoint required.

## The actual next lever

Not a later attach point — **an arg_type that harvests a live object pointer from the game**
(walk a known array/handle table, pass a real element). That single addition unblocks
`0x00421930`, `0x004b4050`, the camera pair, and by inspection a large share of the 947-row
STATE backlog, which is dominated by exactly this shape: *resolve a handle/pointer, read a field*.
