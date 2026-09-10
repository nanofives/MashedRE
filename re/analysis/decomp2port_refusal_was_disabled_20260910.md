# 13 stray control bytes silently disabled `decomp2port.py`'s refusals (2026-09-10)

Two of the three Lane 2 crashers are one bug, and the bug is not in the ports — it is that the
generator's own guard against generating them had been dead code since it was written.

## The corruption

`re/tools/decomp2port.py` contained **13 raw control bytes across 7 lines**, every one inside an
`r"..."` regex literal, where a backslash escape was meant:

| byte | was written as | count |
|---|---|---:|
| `0x08` (BS) | `\b` — word boundary | 12 |
| `0x01` (SOH) | `\1` — backreference | 1 |

One corruption event, from a writer that interpreted backslash escapes instead of keeping them
literal. Affected lines and what each regex became:

```
71   INDIRECT = ... |<BS>code<BS>| ...              (meant |\bcode\b|)
269  re.finditer(r"<BS>FUN_[0-9a-fA-F]{8}\s*\(\s*\)")     (meant \bFUN_…)
272  re.findall(r"<BS>((?:p[a-z]*Var|iVar|uVar)\d+)\s*=\s*<SOH>\s*\+")
                                                    (meant \b(…)\s*=\s*\1\s*\+)
273  re.sub(r"^[ \t]*[A-Za-z_][\w \*]*<BS>%s;…")          (meant \b%s)
274  re.findall(r"<BS>%s<BS>")                            (meant \b%s\b)
275  re.findall(r"<BS>%s\s*=")                            (meant \b%s\s*=)
276  re.findall(r"<BS>%s<BS>[^;]*[<>]") + (r"[<>][^;]*<BS>%s<BS>")
```

Consequence: **the entire `HIDDEN_REG_ARG` refusal could never fire.** Line 272 is its entry
condition, and with `\1` replaced by `0x01` the pattern demands a literal SOH byte between `=`
and `+` — which no decompilation contains. One alternative of `INDIRECT` was also dead.

Repaired by byte-level replacement (`0x08` → `\b`, `0x01` → `\1`), 0 control bytes remaining,
`ast.parse` clean.

## What it let through

### `0x00495fe0` — the crash inside the `fix_joypad` boot-patch cave, fully explained

The original:

```
0x00495fe0  mov eax,[0x772fac]
0x00495fe5  push edi
0x00495fe6  xor edi,edi
0x00495fea  jle 0x496008
0x00495fed  mov esi,0x771e88     ; <-- ESI = base of the device array
0x00495ff2  call 0x495870        ; <-- FUN_00495870 reads its argument FROM ESI
0x00495ffd  add esi,0x448        ; <-- advance one device per iteration
0x00496005  jl 0x495ff2
```

**`FUN_00495870` takes its device pointer in ESI.** Ghidra did not model that, so the
decompilation reads `FUN_00495870()` with no arguments and an otherwise-unused counter, and the
generated port faithfully reproduces a call with *no* register set up:

```c
iVar1 = 0;
if (0 < DAT_00772fac) {
  do { L2T_FUN_00495870(); iVar1 = iVar1 + 1; } while (iVar1 < DAT_00772fac);
}
```

With ESI undefined, `fix_joypad`'s cave at `0x00508bde` executes `mov eax,[esi]` with `ESI = 0`
— **the exact access violation caught earlier** (`log/crash_eip_00495fe0.txt`,
`mem_address 0x0`, `mem_op read`). The boot patch is not at fault; it never receives a valid
pointer.

### `0x004219c0` — same class, and the refusal's own comment cites this RVA

```c
puVar1 = &DAT_0063fb90;
do { L2T_FUN_00421720(); puVar1 = puVar1 + 0x208; } while ((int)puVar1 < 0x6403b0);
```

A pointer advanced 0x208 per iteration, never passed to a zero-argument call. The refusal's
comment describes this shape *verbatim, with this stride, citing this RVA* — it was written from
this very failure and then never fired.

### `0x00421960` — NOT this class

```c
uVar1 = UtilDeref55dec0(DAT_006ce274);
L2T_FUN_00559c40(uVar1);
```

No loop, no advanced local. The repaired heuristic correctly **accepts** it, and its crash is
elsewhere: EIP `0x00559cb3`, inside `FUN_00559c40` itself, faulting on `mov edi,[eax+ebp]` at an
unmapped address right after a bitset index computation (`shr ebp,5` / `shl ebp,2` /
`and ecx,0x1f`). Still open, needs its own read.

## Verification

Replayed the repaired heuristic against real bodies plus negative controls:

| body | verdict |
|---|---|
| `0x004219c0` real | **REFUSE** `HIDDEN_REG_ARG:puVar1 → FUN_00421720` (reads=4 writes=2 conds=1) |
| `0x00495fe0` real | **REFUSE** `HIDDEN_REG_ARG:iVar1 → FUN_00495870` (reads=4 writes=2 conds=1) |
| `0x00421960` real | accept (correctly — different shape) |
| control: the local IS passed to the call | accept |
| control: zero-arg call, no loop local | accept |

## Actions taken

- 13 control bytes repaired; all 7 regexes now read as intended.
- `0x004219c0` and `0x00495fe0` manifest rows → `SKIP:hidden-reg-arg`; their results rows →
  `INVALID_PORT` with the mechanism inline. Both TUs must be regenerated (the generator will now
  refuse) or removed — they are not defects to fix, they are ports that should never have existed.
- `0x00421960` left as `CRASH`; it is a genuine open defect of a different kind.

## The lesson worth keeping

A refusal that never fires is indistinguishable from a refusal that always passes. This one had
a comment, a cited RVA and a named failure mode, and it was dead. **Any guard added to a
generator needs a positive test that it refuses a known-bad input** — the negative controls here
would all have passed against the broken regex too.
