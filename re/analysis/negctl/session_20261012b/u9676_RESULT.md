# U-9676: who writes the camera entry's RwMatrix pad slots (+0x68/+0x78/+0x88)

Date 2026-10-12, branch `work/crashcam`. Original only (`MASHED_RE_NO_AUTO_HOOK=1`), own PIDs. Disassembly from
`MASHED.exe.unpatched`. The patched exe is byte-identical over `0x00445aa0..0x004464b3`, `0x00441d40..0x00442402`,
`0x00442440..0x004425c7`, `0x004464c0..0x0044651c`, `0x004c4a50..0x004c4d1d`, `0x004c4d20..0x004c4dbb`,
`0x005cb2a0..0x005cb403`.

## Verdict: RESOLVED

None of the three callees stores to the pad slots itself, with one exception that does not carry stack data. The
pads are written by **`rep movsd` at `0x004c4cf1`** inside `FUN_004c4a50`, the helper of `RwMatrixRotate`
(`0x004c4d20`, its only direct caller, `0x004c4dac`). Every callee calls `RwMatrixRotate` on `entry+0x4c`.
`0x004c4cf1` is the **last** write of every pad on every dispatch, for all three callees. It copies a 16-dword
result temp at `[esp+0x70]` into the matrix, and that temp's pads `[esp+0x8c]`, `[esp+0x9c]`, `[esp+0xac]` are
written by nobody. Not by `FUN_004c4a50` (its only store into the temp is the flags at `0x004c4ce2`), and not by
the multiply it calls (`0x005cb2a0` stores only the 12 vector floats). So the pad values are whatever the stack
held at entry ESP `-0x24`, `-0x14` and `-0x04` of `FUN_004c4a50`.

| pad | entry offset | matrix offset | source slot in `FUN_004c4a50` (body ESP / entry ESP) |
|---|---|---|---|
| pad1 | `+0x68` | `+0x1c` | `[esp+0x8c]` = entry ESP `-0x24` |
| pad2 | `+0x78` | `+0x2c` | `[esp+0x9c]` = entry ESP `-0x14` |
| pad3 | `+0x88` | `+0x3c` | `[esp+0xac]` = entry ESP `-0x04` |

This explains the a2-camera result directly. A 4-byte shift in `0x004464c0`'s frame moves `FUN_004c4a50`'s frame by
4, so those three slots read different stale dwords.

## 1. Live write-watch (hardware DR0, `Process.setExceptionHandler`, no Interceptor)

Instrument: `re/frida/cam_frame_writer_watch.py --pads` (extended here). One pad slot per pass, DR0 armed on every
thread. Hits are keyed by trap PC and by the dispatcher return found on the stack (`0x004464e5` = `0x00445aa0`,
`0x004464f5` = `0x00441d40`, `0x00446505` = `0x00442440`). Same race as a2-camera: count `[0x00898994]` = 7,
types `0,1,1,1,2,2,3` (type at `entry+4`, entries at `0x008964c0 + k*0xd8`).

A data breakpoint traps after the write. During `rep movsd` it leaves EIP on the `rep` itself until the last dword,
so pad1/pad2 (dwords 7 and 11, ECX at trap = 8 and 4) report `0x004c4cf1`/`0x004c4c1a`/`0x00442469`, and pad3
(dword 15, ECX = 0) reports the next instruction `0x004c4cf3`/`0x004c4c1c`/`0x0044246b`. For every hit,
`EDI-4` = the watched pad address and `ESI-4` = the source address.

**Writers per dispatch, per pad slot** (pad1, pad2 and pad3 identical, 1.0 to 3.0 s windows at ~60 frames/s, e.g.
e0: 122 + 61 hits in 1.0 s):

| callee | entries | writes per dispatch, in order | last writer |
|---|---|---|---|
| `0x00445aa0` (type 0) | e0 | `0x004c4c1a`, `0x004c4cf1`, `0x004c4cf1` | `0x004c4cf1` |
| `0x00441d40` (type 1) | e1, e2, e3 | `0x004c4c1a`, `0x004c4cf1`, `0x004c4cf1` | `0x004c4cf1` |
| `0x00442440` (type 2) | e4, e5 | `0x00442469`, `0x004c4cf1` | `0x004c4cf1` |
| none (type 3) | e6 | 0 | none (`0x004464c0` calls no callee for type 3) |

Order from a 12-hit ring per pass (`u9676_padwatch5.json`): e0, e1 `c1a, cf1, cf1` repeating, e4 `442469, cf1`
repeating.

The `RwMatrixRotate` call sites match the counts: `0x00445aa0` at `0x00446476`, `0x00446486`, `0x00446496`.
`0x00441d40` at `0x004422d5`, `0x004422e5`, `0x004422f5` (a fourth at `0x00442399` is on a branch with no hit in
this race). `0x00442440` at `0x004425aa`. The first rotate on a just-reset matrix takes the `0x004c4c1a` branch,
later ones the `0x004c4cf1` branch.

Liveness controls. Each `--pad-list` boot ends with an e0+0x68 pass. Boots 3 and 4: control 122 + 61 hits. Boots
2 and 5: control 0. In those two, hits stop about 12 s into the watch (boot 2 at e5 after 34 hits). Every zero
claimed above (e6) comes from boot 4, whose control is live. Boot 1 (all entries in order) has the same cut-off
from e2+0x88 on and is used only for its positive hits. `[UNCERTAIN]` why hits stop after ~12 s (race state or
watchpoint state). It does not affect any positive hit.

## 2. The three copy sites (bytes)

**`0x004c4cf1` (final writer, all callees).** `FUN_004c4a50`: `sub esp,0xa4` + `push ebx/esi/edi`. Combine op at
`[esp+0xc4]` (`0x004c4b1f`). Multiply branch: `0x004c4cd3 lea eax,[esp+0x78]` / `push eax` / `0x004c4cd8 call
[ecx+edx+8]` (ecx = `[0x007d4028]`, edx = `[0x007d3ff8]`, read live: `0x005cb2a0`), then `0x004c4ce2 mov
[esp+0x7c],esi` (flags only), `0x004c4ce6 lea esi,[esp+0x70]`, `0x004c4cea mov edi,ebx` (ebx = matrix arg,
`[esp+0xb4]`), `mov ecx,0x10`, `0x004c4cf1 rep movsd`. Live: `ESI-4` = body ESP `+0x8c`/`+0x9c`/`+0xac` on every
sample, `EDI-4` = the entry pad.
`0x005cb2a0` takes the destination in ECX from arg0 (`0x005cb2ac mov ecx,[ebp+8]`) and stores
`[ecx]`, `+4`, `+8`, `+0x10`, `+0x14`, `+0x18`, `+0x20`, `+0x24`, `+0x28`, `+0x30`, `+0x34`, `+0x38`
(`0x005cb2d0`..`0x005cb3fb`), then `ret` at `0x005cb402`. No store to `+0xc`, `+0x1c`, `+0x2c`, `+0x3c`.

**`0x004c4c1a` (first rotate, types 0 and 1).** Combine-op-2 branch with the matrix's `+0xc` flag test taken
(`0x004c4bfa`): `0x004c4c0d rep movsd` copies the rotation temp `[esp+0x30]` to `[esp+0x70]`, then
`0x004c4c1a rep movsd` copies `[esp+0x70]` to the matrix. The temp's pads `[esp+0x4c]`, `[esp+0x5c]`, `[esp+0x6c]`
(entry ESP `-0x64`, `-0x54`, `-0x44`) are never stored in `FUN_004c4a50`. Every store into the temp:
right `0x004c4b0d/15/39`, flags `0x004c4b49` (`=3`), up `0x004c4b51/5f/6b`, at `0x004c4b75/83/8f`, pos
`0x004c4b26/31/41` (`=0`). Overwritten later in the same dispatch by `0x004c4cf1`.

**`0x00442469` (type 2 only, not stack).** `0x00442452 call 0x46d4a0` fills `[esp+0x18]` with a matrix pointer,
`0x0044245b mov esi,[esp+0x18]`, `0x0044245f lea ebx,[ebp+0x4c]`, `0x00442469 rep movsd`. Live `ESI-4` =
`0x00881ee4`/`0x00881ef4`/`0x00881f04`, i.e. the pads of a matrix at `0x00881ec8` (a global). Overwritten later
in the same dispatch by `0x004c4cf1` via `0x004425aa call RwMatrixRotate`.

## 3. Consequences

- The pad values the original leaves in the camera entries are stale stack from `FUN_004c4a50`'s frame. Any
  caller stack change (frame size, call depth, a Frida thread running the dispatcher) changes them. That is why
  a2-camera's +4 variants moved them, and it fits race.py's first-run orig-vs-orig difference.
- This is a property of `RwMatrixRotate` (hooks.csv `0x004c4d20`, C2, `Math/RwMatrixRotate.cpp`). Any matrix
  `RwMatrixRotate` writes gets stack pads. A run_diff that compares whole 64-byte matrices after
  `RwMatrixRotate` can read RED, or GREEN by luck, on pads alone. `[UNCERTAIN]` whether the existing
  `rw_matrix_rotate` diff masks pads. Not checked in this session.
- No reader of the entry pad slots was identified. Nothing here changes `0x004464c0` (re-ported verbatim, C3).

## 4. Evidence

`crashcam_evidence/u9676_padwatch1.json` (all entries, boot 1), `u9676_padwatch2.{json,txt}` (e4, e5, e2, e3, e6
+ control, boot 2), `u9676_padwatch3.{json,txt}` (e5, e3 + live control), `u9676_padwatch4.{json,txt}` (e2, e6 +
live control), `u9676_padwatch5.{json,txt}` (e0, e4, e1 with hit order). PIDs: boot 2 = 31376, boot 3 = 32076,
boot 4 = 31796, boot 5 = 17600. Two earlier boot-2 attempts are discarded: PID 34316 (its JSON was overwritten by
the re-run) and one aborted by a shell error before printing. Boot 1's PID was not recorded. No MASHED from this
worktree was left running (checked by path).

## 5. Proposed tracker text (the orchestrator applies it via re-classify)

**UNCERTAINTIES.md U-9676 -> RESOLVED:**

> ~~structural~~ **RESOLVED 2026-10-12** (crashcam). No callee writes the pads with its own stores. The last write
> on every dispatch, for all three callees, is `rep movsd` at `0x004c4cf1` in `FUN_004c4a50`, the helper of
> `RwMatrixRotate` `0x004c4d20` (only direct caller `0x004c4dac`), called on `entry+0x4c` from `0x00446476/86/96`
> (`0x00445aa0`), `0x004422d5/e5/f5` (`0x00441d40`) and `0x004425aa` (`0x00442440`). It copies the result temp
> `[esp+0x70]`, whose pads `[esp+0x8c]/[esp+0x9c]/[esp+0xac]` (entry ESP `-0x24/-0x14/-0x04`) nobody writes: the
> multiply `0x005cb2a0` stores only the 12 vector floats. Earlier same-dispatch writes: `0x004c4c1a` (first
> rotate, types 0/1, pads from the never-written rotation-temp slots `[esp+0x4c/0x5c/0x6c]`) and `0x00442469`
> (type 2, copies a global matrix at `0x00881ec8`). Per dispatch per pad: type 0 and type 1 = 1x `0x004c4c1a` +
> 2x `0x004c4cf1`, type 2 = 1x `0x00442469` + 1x `0x004c4cf1`, type 3 = 0. Hardware write-watch on the original,
> live controls. re/analysis/negctl/session_20261012b/u9676_RESULT.md

**New U-row candidate (low):** "Does the `rw_matrix_rotate` (0x004c4d20) diff compare the stack-sourced pad
dwords (+0x1c/+0x2c/+0x3c)? `FUN_004c4a50` copies them from never-written stack slots (`0x004c4cf1`,
`0x004c4c1a`)." Path: read the handler's compare set in `diff_template.js` (via `re/frida/ARG_TYPES.md`).
