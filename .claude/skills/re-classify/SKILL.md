---
name: re-classify
description: Classify a Mashed function against the C0..C4 confidence rubric. Use whenever you finish reverse-engineering work on a function — it walks the rubric, updates hooks.csv, drops STUBS/UNCERTAINTIES rows, and refuses promotions that lack evidence. Triggers on "classify", "promote", "what's the confidence of", "mark as done", "C3", "C4", or after any /re or hook-author session.
---

# re-classify — function confidence pipeline

Single source of truth for moving a function up (or down) the confidence ladder. Read `re/CONFIDENCE.md` for the rubric; this skill is the **enforcement**.

## When to invoke

- After `/re` finished decomposing a function — promotes from C0/C1 to whatever the evidence supports.
- After `hook-author` has written a reimplementation — candidate for C3.
- After `diff-original` produced a green CSV — candidate for C4.
- When asked "what's the confidence of …" or "is this done?"
- When demoting after a regression.

## Inputs

The skill needs:
- **RVA** of the function (mandatory).
- **Subsystem** (mandatory; one of: boot, audio, ai, vehicle, track, render, hud, frontend, input, save, net, util, unknown).
- **Target confidence** the user is asking for (C0..C4). The skill validates evidence; never just stamps the request.
- **Evidence pointers** — analysis note path, source file path, Frida diff CSV path, etc.

If anything is missing, the skill **asks before acting**. No silent inference of evidence locations.

## Workflow

For each promotion request, the skill **must** check the corresponding gate from `re/CONFIDENCE.md` and refuse if not satisfied:

| Target | Gate (verbatim from rubric) |
|--------|------------------------------|
| C1 | function has name/strings/xrefs evidence; subsystem assigned |
| C2 | decomp read end-to-end; `re/analysis/<sub>/<func>.md` exists with shape (args/return/reads/writes), every const/offset cited; NO-GUESSING |
| C3 | semantics stated; reimplementation lives at `mashedmod/src/mashed_re/...`; build is clean; hooked via `RH_ScopedInstall`; runtime-toggleable; at least one caller AND one callee at C2+ |
| C4 | `diff-original` CSV produced; clean diff vs original; no stubs in this function's body |

For each refusal, the skill states **exactly which gate criterion failed** and what evidence would unblock it.

## Side effects on success

When the skill promotes a function, it must **all in one transaction**:

1. **Update `hooks.csv`** — find or insert the row for this RVA; update `name`, `subsystem`, `confidence`, `status`, `file`, `scenario`, `frida_diff`, `notes`.
2. **Sweep `[UNCERTAIN]` markers** in the function's analysis note. For each unfiled marker, write a new row in `UNCERTAINTIES.md` (next `U-NNNN`), and replace the inline marker with `[UNCERTAIN U-NNNN]`. Refuse a C3 promotion if any `semantic` or `structural` uncertainty is unresolved.
3. **Sweep stubs** in the function's source. For each callee that resolves to a stub helper, ensure a row exists in `STUBS.md`. Refuse C4 promotion if any stub remains in the function's body.
4. **Append to `re/analysis/CHANGELOG.md`** — one line: `YYYY-MM-DD  RVA  name  oldC->newC  evidence`.
5. **Demotion** is the same flow in reverse: lower the row, append to changelog with `oldC<-newC` arrow.

## CSV row format reminder

```
rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes
00401234,CFoo::Bar,vehicle,C3,impl,mashedmod/src/mashed_re/Vehicle/CFoo.cpp,,pending,calls 00405678 (CFoo::Update)
```

RVA is hex without `0x`. Empty cells stay empty (not `na`).

## Anti-patterns the skill must reject

- "Promote to C3 because the user said so" without evidence pointers.
- "Promote to C4 because the build passes" — that's not the gate.
- Editing `hooks.csv` outside this skill.
- Filing an `UNCERTAINTIES.md` row without a path-to-resolution.
- Filing a `STUBS.md` row without the inline `// STUB S-NNNN` comment in source.

## Output to user

Always finish by printing:
- Old confidence → new confidence
- New tracker row IDs (U-NNNN, S-NNNN) created during the transaction
- Any open uncertainties or stubs still attached to this function (so the user knows what's left)
