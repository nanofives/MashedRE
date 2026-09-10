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

### `0x00421960` — NOT this class, but a THIRD generator gap: shared stack arguments

```c
uVar1 = UtilDeref55dec0(DAT_006ce274);
L2T_FUN_00559c40(uVar1);
```

No loop, no advanced local — the repaired heuristic correctly **accepts** it. The real cause is
that the original **shares one pushed argument block between two consecutive calls**:

```
0x00421966  push 7 / push -1 / push eax / push ecx
0x0042196c  call 0x55dec0        ; FUN_0055dec0 -- 4 args
0x00421971  add esp,4            ; pops ONE dword -- three args stay live
0x00421974  push eax             ; the return value
0x00421975  call 0x559c40        ; FUN_00559c40(eax_ret, eax_old, -1, 7)
0x0042197a  add esp,0x10         ; now pops all four
0x0042197d  ret
```

Ghidra models that as two independent **one-argument** calls, and the generated port duly calls
`FUN_00559c40` with one argument. The callee reads arg1 and arg3:

```
0x00559c40  sub esp,0x20
0x00559c43  push ebx
0x00559c44  mov ebx,[esp+0x30]   ; = E+0xc  -> arg3, which the original supplies as -1
0x00559c48  push esi
0x00559c49  mov esi,[esp+0x2c]   ; = E+4    -> arg1
0x00559c4d  cmp ebx,-1
0x00559c51  jne 0x559ca2         ; <-- with garbage arg3 this branch is taken
...
0x00559ca6  mov ebp,ebx          ; garbage
0x00559caa  shr ebp,5
0x00559cad  shl ebp,2
0x00559cb3  mov edi,[eax+ebp]    ; <-- FAULT, unmapped 0x1d9644f4
```

So arg3 is stack garbage instead of `-1`, the `cmp ebx,-1 / jne` takes the wrong branch, and the
bitset index derived from that garbage faults — **exactly** the caught AV
(`log/crash_eip_00421960.txt`, `mem_address 0x1d9644f4`, after `shr ebp,5 / shl ebp,2 /
and ecx,0x1f`).

**Why the generator missed it.** `decomp2port.py` already refuses `CALLEE_REG_ARG` when the
callee's prototype shows **fewer** stack params than the site passes. The **opposite** mismatch —
prototype shows *more* — only `log.append("CALLEE_ARITY … -> raw thunk")`ed and fell through,
emitting a thunk built with the **site's** arity, i.e. a call that passes too few arguments. That
is never safe. Now a refusal (`CALLEE_ARITY:… (site passes FEWER)`).

The tell is the mismatched `add esp,N` after the call, and that is **not visible in the
decompilation text** — which is why the refusal is on the arity disagreement itself rather than
on detecting the idiom.

**Caveat, stated because it cuts against the change:** this makes the generator stricter, so
Lane 2 yield drops, and **any TU generated under the permissive rule is suspect** — not just
this one. `0x00421960` is the proven instance; how many others took that path is not known,
because the `CALLEE_ARITY` log lines were not retained.

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
