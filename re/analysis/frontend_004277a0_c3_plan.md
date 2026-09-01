# 0x004277a0 — C3-readiness plan (control-code remap, pure leaf)

Area-loop frontend round 2, 2026-09-01. Decode refresh + harness spec. Not yet landed.

## What it is (decomp, pool Mashed_pool3, NO-GUESSING)

`FUN_004277a0` — Ghidra signature `void FUN_004277a0(void)`, but it takes **two
register arguments**:
- `EAX` = source pointer (`in_EAX`, `ushort*`): a length-prefixed short array
  `[count, ch0, ch1, ...]`.
- `EBX` = destination pointer (`unaff_EBX`, `short*`): output short array.

Body (all constants cited from the decomp):
- `uVar1 = *in_EAX` — read `count` from `src[0]`.
- If `(short)count > 0`: loop `count` times. Read `src[1+i]` (address
  `in_EAX + 2 + i*2`), remap control codes, write to `dst[i]`:
  - `8 -> 0x81`, `9 -> 0x7f`, `10 -> 0x81`, `0xb -> 0x8d`, `0xc -> 0x80`,
    `0xd -> 0x87`, `0xe -> 0x8f`; any other value passes through unchanged.
- Always: `dst[(short)count] = 0` (null-terminate at index `count`).

It is a deterministic pure transform: reads `src`, writes `dst`, no globals, no
callees (**pure leaf**, 0 callees).

## C3 gate status

- Reimplementation: **not yet authored** (this is the remaining work).
- Callers at C2+: **YES** — MenuMenusBA (`0x004282a0`, C3) and MenuMenusBB
  (`0x00427ad0`, C3) both call it.
- Callee at C2+: **exempt** — pure leaf (0 callees), same exemption used for
  `0x004c19f0` etc.
- The only real blocker is the Frida harness: no existing `arg_type` delivers
  EAX=src-ptr + EBX=dst-ptr and observes the dst buffer.

## Harness spec — new arg_type `eax_ptr_ebx_outbuf`

Adapt `diff_template.js` `esi_idx_ecx_outbuf4` (line ~2456): same trampoline shape,
different registers. Per side:

```
push ebx
mov eax, <srcbuf imm32>   ; 0xB8, patched — per-test seeded src
mov ebx, <dstbuf imm32>   ; 0xBB, patched — per-side output buffer (constant per side)
call <target>             ; 0xE8 rel32
pop ebx
ret
```

- Allocate a src buffer and a dst buffer PER SIDE (separate — the fn only reads src
  and writes dst, never stores src ptr into dst, so no shared-buffer aliasing is
  needed, unlike `eax_ecx_insert`).
- Per test: write the length-prefixed short array into src (count at `[0]`, chars at
  `[1..]`), zero dst, call, fingerprint dst as `count+1` u16 words.
- Add the mirror case to `verify_hook_install_template.js` `callFn` (same seeding),
  so path2 call-through fires (same completeness pattern as the round-1
  `draw_quad_observe` fix).

Reimpl: naked function reading EAX/EBX (preserve EBX per the original's
`push/pop ebx`), transcribing the 7 remap constants above; register via
`RH_ScopedInstall(..., 0x004277a0)`; link the `.cpp` into the exe/asi build.

## Test vectors (exercise every remap arm + passthrough + count edge)

- count=0 (only the terminator write).
- count=1 with each of `{8,9,10,0xb,0xc,0xd,0xe}` (7 vectors) -> the 7 mapped outputs.
- a passthrough char (e.g. `'A'`=0x41) -> unchanged.
- a mixed multi-char string with control codes interleaved.
- a longer string (count ~16) to catch loop/stride bugs.

Expected: dst fingerprints bit-identical between original and reimpl (`crash_equal_ok`
not needed — no pointer deref that can AV given valid seeded buffers).
