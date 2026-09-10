# Lane 2 — decomp→port transcriber: design + first pilot (2026-09-10)

Companion to `promotion_lanes_assessment_20260910.md` (Lane 2). Goal: make the ~3,900 C2 rows
that have an analysis note but no code portable at machine cost, feeding the shadow lane.
**Generation moves no C-level.** A generated TU is a C2 row's reimplementation candidate; C3
still needs the shadow A/B CLEAN + the caller/callee gate, through `re-classify`.

## Pipeline

```
hooks.csv C2 rows ──reachability──▶ rvas.txt
   (prescreen_result_all.tsv today; drcov/TT-4 later)
      │
      ▼
decomp_pc.py --callees --port --json          (headless Ghidra, N-in-one, read-only slot)
      │  per function: decomp C, DECOMPILER prototype (ret/params/storage), typed globals
      │  (addr, type, size, block, writable), callee list with each callee's own
      │  decompiler prototype (`dproto`)
      ▼
decomp2port.py --emit-dir Lane2 --apply       (pure text transform; refuses what it cannot do)
      │  one TU per function: L2_<rva>.cpp, registered in asi_sources.rsp
      ▼
build.bat  ──▶ decomp2port.py --prune-failed <log>   (COMPILE_FAIL rows dropped, reason kept)
      ▼
shadow_batch.py --rvas ...                    (return-value ports carry a ShadowAB::Run wrapper)
      ▼
re-classify (CLEAN + gate) ──▶ C3
```

### What the `--port` mode adds to `DecompPC.java`
The plain C text is not enough to transcribe mechanically. Three facts were missing and are
now emitted per function:
1. **Decompiler prototype**, not the stored signature. The stored one is `undefined f(void)`
   for most of this project's functions (never committed), while the body uses parameters.
   Parameter *storage* (`Stack[0x4]:4` vs `ECX:4`) is what tells cdecl from register ABI.
2. **Typed globals** from `HighFunction.getGlobalSymbolMap()`: address, Ghidra type, size,
   memory block, writable flag. `_DAT_x` (Ghidra's "overlapping symbol" spelling) is
   resolved by address.
3. **Callee decompiler prototypes**: each callee is decompiled too (30 s budget) so a raw-RVA
   thunk for an unported callee has a signature.

### What the transcriber does (all mechanical, all logged per row)
- Ghidra typedefs (`undefined4`, `byte`, `uint`, ...) as in the hand-written ports.
- Globals become `#define DAT_x (*(T*)0xADDRu)` reads through the absolute address, `const`
  for non-writable blocks. Nothing is folded into literals; `.rdata` constants are read at
  run time exactly like the originals do. `undefined` (size 1) is `unsigned char` so
  `&DAT_x + n` stays byte arithmetic.
- Callees: a port with a live `RH_ScopedInstall` for that RVA is called by its symbol;
  otherwise a `static inline L2T_FUN_x(...)` raw-RVA thunk. **Call-site arity wins** over the
  callee's prototype when they disagree (then the callee is thunked with `unsigned int`
  params even if a port exists — a port's signature is never bent to fit a caller).
- **Every argument is C-cast to the declared parameter type.** Ghidra's C converts int and
  pointer implicitly; MSVC does not; a C cast preserves the bit pattern.
- Float parameters passed as hex literals (`FUN(0x3f800000)`) become `f32bits(0x3f800000u)`,
  a bit reinterpretation, never a numeric conversion.
- The hook symbol is `L2_<name>`; `HookSystem::InstallAll` now treats `L2_*` as **opt-in**
  (installed only through `MASHED_HOOK_ONLY` or `MASHED_HOOK_LANE2=1`). An unverified
  generated port never rides along in a default `.asi` run.
- Return-value functions get the shadow wrapper; void ones are emitted un-shadowed and
  flagged `NEEDS_REGION` (Lane 3 is the planned verifier for them).

### What it refuses (per-row reason, so yield is honest)
| reason | meaning |
|---|---|
| `REGISTER_ABI` | a parameter in a register, or `unaff_ESI`/`in_EAX` in the body; needs naked asm |
| `INDIRECT_CALL` | any call through a `code*` value (vtable, global fn-ptr, callback): target cc unknown |
| `CALLEE_PROTO` | inconsistent call-site arity, register-ABI callee, unknown type |
| `PSEUDO_OP` | `CONCAT`/`SUB4`/`ZEXT`/`in_ST0`/`float10` return: x87 or bit-slice semantics |
| `UNKNOWN_TYPE` | a struct/typedef the mapper does not know |
| `COMPILE_FAIL` | MSVC rejected the TU (set by `--prune-failed`) |

## Pilot: 53 reachable, unported C2 rows

Candidates = `prescreen_result_all.tsv` rows `exercised_inrace`/`race_gated` at C2 with no
`.cpp` in `file` (53). One headless run decompiled all 53 plus every callee.

| outcome | n |
|---|---|
| PORT, return value (shadow-verifiable now) | 5 |
| PORT, void (needs a region / Lane 3) | 21 → 20 after one `COMPILE_FAIL` |
| REFUSE `INDIRECT_CALL` | 22 |
| REFUSE `REGISTER_ABI` | 4 |
| SKIP already ported | 1 |

25 TUs compile; build clean. The one compile failure (`0x005a7e70`) is a global typed
`undefined4` in the symbol map but used as `undefined4 *` in the body — Ghidra's own
inconsistency; the row is reported, not patched by hand.

The first three build attempts each exposed a transcription class that is now handled
(callee arity disagreements; int↔pointer strictness; single-star and global-fn-ptr indirect
calls; one TU sinking the whole batch). Those are recorded in the tool's docstring; the
reviewer should expect the next candidate set to surface one or two more.

Shadow batch of the 5 return-value ports (`log/shadow_ab/batch_lane2.txt`, one boot, 4 cars,
60 s): **2 CLEAN** — `0x005af010` 8/8 and `0x0045d3f0` 2/2, both `A/B-IS-REAL` — and 3
NO_SAMPLES (audio-subsystem functions; the game runs muted in every capture). So the chain
decomp → transcribe → build → install live → sample → compare is closed. **No promotion**: 8
and 2 samples are below the 48 the lane uses elsewhere, and `0x0045d3f0`'s return is
path-independent (caveat below). Re-run with a longer hold before any re-classify.

### Caveat visible in the very first generated port
`L2_0045d3f0` (`if (DAT_006904e8 && !DAT_006904ec) { ...; FUN_005a60b0(); DAT_006904ec = 1; }
return 1;`) is a one-shot init guarded by a flag. Under `ShadowAB::Run` the original runs first
(flag set, callee executed), then the port (flag already set, different path). The RETURN is 1
on both paths, so the row reads CLEAN — and that CLEAN says nothing about the guarded body. Two
consequences the reviewer must apply to Lane 2 output: (1) a CLEAN on a function whose return
is path-independent is weak evidence (`feedback_evidence_discipline`: prove the path ran); (2)
side-effecting callees run twice during a sampled call, which perturbs the game for the rest
of the boot. Lane 3's write tracking is the fix for both (it compares effects, not returns);
until then, `shadow_ab_report.py` verdicts on Lane 2 rows should be read with the body open.

### Live result of the 20 void ports (with the A/B disarmed, `batch_control1/2.txt`)
Installed alone, `0x00421960`, `0x004219c0` and `0x00495fe0` crash the game (two at boot, one
26 s into the race); `0x00421980`, `0x0041f290`, `0x0041f060` run a full race. So 3 of the
first 26 generated ports are wrong in a crashing way, and the `L2_` opt-in guard is what kept
them out of every other session's boots. Two refusal rules came out of it (`CALLEE_REG_ARG`,
`HIDDEN_REG_ARG`); neither catches these three, so at least one more hazard class is open —
the ports' callees have 0-parameter stored prototypes, and the decompiler prototypes we thunk
with may still miss register arguments the disassembly would show.

### Indirect-call idiom table (added later the same day)
`INDIRECT_IDIOMS` in `decomp2port.py`: a global function-pointer table base → calling convention,
each entry citing the verified hand port that establishes it. First entry: the RenderWare device
slot `DAT_007d3ff8` (`(**(code **)(DAT_007d3ff8 + 0x20))(a, b)`), `__cdecl` per `vt20` in
`Frontend/MenuDrawLoopTwin.cpp` and `RwIm2DBridge.cpp`. The rewriter emits a per-slot/arity
helper (`L2_slot_007d3ff8_0x20_2`) with C-cast arguments and leaves every other indirect call
refused. Survey of the 22 refused pilot rows: 8 used only that slot (13 call sites) and are now
generated; the rest are `*DAT_007d4110+off` (4 rows), `*DAT_007d4108+0x28` (1) and object
vtables (2) — table entries to be added only once a hand port pins their convention. Pilot
after the table: **29 TUs compile** (4 return-value, 25 void), 15 `INDIRECT_CALL` refusals left.

Tracked shadow batch of the 7 idiom-recovered ports (`batch_lane2_idiom.txt`, 2 boots):
**7/7 CLEAN, 24/24 samples each** — `0x00421560 0x0048fce0 0x0048fd10 0x0048fd40 0x00457610
0x00486f50 0x00490490`. These are the first decompiler-generated ports verified effect-identical
(touched pages + caller stack window) against the original at their real call sites.

## What Lane 2 needs to scale

1. **Reachability at pool scale.** The pre-screen covers 210 rows; the C2 pool is 3,900. This
   is TT-4 (drcov) or a per-chunk `MASHED_COUNT_RVAS` sweep (~160 boots). Without it, ~1/2 of
   generated ports would be for code no scenario runs.
2. **Indirect calls (22/53 here).** The dominant refusal. Two mechanical fixes: (a) the RW
   device-slot vtable `(*(DAT_007d3ff8+0x20))(a,b)` is a known cdecl idiom already used by
   hand ports (`vt20` in `MenuDrawLoopTwin.cpp`) — a table of known slots → emit the idiom;
   (b) for other fn-ptr calls, read the call-site disassembly (`DisasmPC.java`) to see whether
   ECX is loaded before the CALL (thiscall) and emit the matching cast.
3. **Void ports (21/26 here).** Lane 3 (page-level write tracking) is the verifier; until then
   they sit as installed-but-unverified reimplementations behind the `L2_` opt-in guard.
4. **Register ABI (4/53).** Out of scope for a C++ transcriber; naked-asm generation is a
   separate tool and a separate risk class (memory `feedback_installed_hook_abi_mismatch`).

## Cost per row (measured on the pilot)
Headless decomp: one run for 53 functions + callees (~2 min). Transcription: instantaneous.
Build: 25 TUs in one `build.bat` (~1 min with the object cache). Verification: a share of one
boot (return-value) — the same economics as the shadow lane. No model tokens are spent per
row; the only judgment step is the review of DIVERGENT rows.
