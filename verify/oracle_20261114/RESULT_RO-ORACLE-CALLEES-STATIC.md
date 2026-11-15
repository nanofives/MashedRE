# RESULT RO-ORACLE-CALLEES-STATIC

Run 47, 2026-11-14. Text/static row: no build, no gates.

## 0. Verdict

GREEN. Neither `0x004a3440` nor `0x004b6770` receives or reads FUN_004b65c0's destination buffer (the 0x20ac-byte
local at `[esp+0..0x20ab]` of the frame at `0x004952f0`). `0x004a3440` runs before the buffer is filled, and
`0x004b6770` takes no argument and makes no stack-relative read of the caller's frame. Only globals feed its callees. This
clears the two [UNCERTAIN] callees in RESULT_RO-ORACLE-GATE section 5 (`verify/replayoracle_20261113/RESULT_RO-ORACLE-GATE.md`).

## 1. Method

- Linear disassembly with capstone through the same tool section 5 used (`verify/replayoracle_20261113/oraclegate/disrange.py`),
  run on `original\MASHED.exe.unpatched` (SHA-256 `BDCAE093A30FBF22...`, the version anchor).
- Output: `verify/oracle_20261114/callees_static_disasm.txt`.
- The ranges `0x004952f0+0x54`, `0x004a3440+0x3d`, `0x004b65c0+0x19`, `0x004b6770+0x30`, `0x004b67a0+0x34` and
  `0x004a4368+0x51` are byte-identical between `MASHED.exe.unpatched` and the patched reference `original\MASHED.exe`
  (pefile `get_data` compare), so these findings hold for the reference binary too.
- I did not use Ghidra. The question is about the instructions, and the disassembly of the anchored bytes answers it.

## 2. `0x004a3440` (called from `0x004952f5` with `eax = 0x20b0`)

| addr | instruction |
|---|---|
| `0x004a3440` | `cmp eax,0x1000` / `0x004a3445 jae 0x4a3455` (taken: 0x20b0 >= 0x1000) |
| `0x004a3455` | `push ecx` |
| `0x004a3456` | `lea ecx,[esp+8]` |
| `0x004a345a..0x004a346c` | loop: `sub ecx,0x1000`, `sub eax,0x1000`, `test [ecx],eax`, `cmp eax,0x1000`, `jae 0x4a345a` |
| `0x004a346e` | `sub ecx,eax` |
| `0x004a3470` | `mov eax,esp` |
| `0x004a3472` | `test [ecx],eax` |
| `0x004a3474` | `mov esp,ecx` |
| `0x004a3476` / `0x004a3478` | `mov ecx,[eax]` / `mov eax,[eax+4]` (restores the saved ecx and loads the return address) |
| `0x004a347b` / `0x004a347c` | `push eax` / `ret` |

- Memory reads: the `test [ecx],eax` probes at `0x004a3465` and `0x004a3472` (one dword per 0x1000-byte step down the new
  frame) and the two loads at `0x004a3476` and `0x004a3478` from its own pushed words. `test` writes no memory.
- Memory writes: `push ecx` at `0x004a3455` and `push eax` at `0x004a347b`. Neither stores a value derived from the buffer.
- Order: the call at `0x004952f5` returns before `0x0049530d lea eax,[esp]` / `0x00495312 call 0x4b65c0`. The buffer
  holds no copied data while `0x004a3440` runs. The probe reads touch the frame's memory before `0x004b65c0` fills it.
- It takes no pointer to the buffer: its only input is `eax` = 0x20b0 (`0x004952f0`).

## 3. `0x004b6770` (called from `0x004b67a0`, reached via `0x00495326 call 0x4b6590` -> `0x004b6590 jmp 0x4b67a0`)

| addr | instruction |
|---|---|
| `0x004b6770` | `test byte ptr [0x7d3e50],1` |
| `0x004b6777` | `je 0x4b6792` |
| `0x004b6779` | `mov eax,[0x7d3e48]` |
| `0x004b677e` / `0x004b677f` | `push eax` / `call 0x4a4368` |
| `0x004b6784` | `add esp,4` |
| `0x004b6787` | `mov dword ptr [0x7d3e48],0` |
| `0x004b6791` | `ret` |
| `0x004b6792` | `mov ecx,[0x7d3e48]` |
| `0x004b6798` / `0x004b6799` | `push ecx` / `call [0x5cc088]` (IAT slot, `KERNEL32.dll!CloseHandle` from the PE import table) |
| `0x004b679f` | `ret` |

- It reads no `[esp+n]` word. Its memory reads are `[0x7d3e50]` and `[0x7d3e48]`, and its only memory write is
  `[0x7d3e48] = 0` at `0x004b6787`.
- `0x004b67a0` calls it with no push before the call (`0x004b67a0 call 0x4b6770` is the first instruction). The frame at
  `0x004952f0` pushes nothing between `0x00495321 call 0x496400` returning and `0x00495326 call 0x4b6590`. So no buffer
  address is passed to `0x004b6770`.
- Each callee gets one argument, the dword at `[0x7d3e48]`: `0x004a4368` at `0x004b677e`, `CloseHandle` at `0x004b6798`.
- `0x004a4368` (first 0x51 bytes read) pushes `0xc` / `0x5d1210` and calls `0x004a5984` (`0x004a4368..0x004a436f`). It
  then reads `[ebp+8]` (its own argument) into `esi` at `0x004a4378`, tests `[esi+0xc]` bit `0x40` at `0x004a437b`, and
  passes `esi` to `0x004a5f1b` (`0x004a438f`), `0x004a431c` (`0x004a439a`) and `0x004a5f6d` (`0x004a43b2`).

## 4. Remaining [UNCERTAIN]

- [UNCERTAIN] Whether `[0x7d3e48]` can ever hold an address inside the `0x004952f0` frame. If it never does, no callee of
  `0x004b6770` can reach the buffer. Missing evidence: a writer census of `0x007d3e48`. Its known writers `0x004b6787` and
  `0x004b67c7` store 0, and `0x004b6618` (`0x004b6610..`) stores into `0x007d3e5c`, not `0x007d3e48`. I did not run a
  full reference scan.
- [UNCERTAIN] The bodies of `0x004a5984`, `0x004a5f1b`, `0x004a431c`, `0x004a5f6d` and `CloseHandle` were not read.
  Each one gets the `[0x7d3e48]` value or constants, not the buffer address. It could reach the buffer only by
  walking ESP up into the caller's frame. Nothing here shows that, and nothing here rules it out.
- Neither point changes the answer to the row question: neither `0x004a3440` nor `0x004b6770` receives or reads the
  destination buffer.

## 5. Effect on RO-ORACLE-GATE section 5

The two callees named as open in section 5 are closed for the row question. The six counter mask rows (`0x008ad868`,
`0x008ad878`, `0x008ad87c`, `0x008ad880`, `0x008ad884`, `0x008ad888`) keep only the residue in section 4 above. No
compared word is written from the stack copy by `0x004a3440` or `0x004b6770`. The only global writes on the path are
`[0x7d3e48]` (`0x004b6787`, `0x004b67c7`), `[0x7d3e4c]` (`0x004b67ab`) and `[0x7d3e58]` (`0x004b67bd`), all constant 0.

## Evidence

- `verify/oracle_20261114/callees_static_disasm.txt`: capstone disassembly of the ranges above, anchored binary.
- `verify/replayoracle_20261113/RESULT_RO-ORACLE-GATE.md` section 5: the question.
