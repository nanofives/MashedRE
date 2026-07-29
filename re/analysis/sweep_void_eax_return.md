# Sweep: installed hooks that DROP an implicit EAX return (2026-07-28)

Motivated by U-9025 (`f1855ad9`): `AudioThreadDescInit` was declared `void` while the original
leaves `param_1` in EAX and its callers `TEST EAX,EAX` before spawning a thread. Eight earlier
instances of the register-ABI class were all crashers; this one was a **hang**, so the class
needed its own detector.

Tool: `scripts/sweep_void_eax_return.py`. Rule — all three must hold:
1. the port is installed via `RH_ScopedInstall` and its C++ return type is `void`
2. the ORIGINAL body writes EAX before its first `RET`
3. a direct caller READS EAX after the `CALL` before redefining it

**`scripts/ghidra/sweep_reg_abi.py` could not be reused: it MISSED `0x005aef00`** (absent from
both `confirmed` and `candidates` in `classa_findings.json`). It hunts registers the original
*preserves* and our body *clobbers*; this class is the opposite shape — the original *defines*
EAX and our body fails to define it the same way. An all-clear there is not an all-clear here.

## Self-test (mandatory; the tool refuses to report without it)

Forcing `0x005aef00` back to `void` makes the detector fire, naming both real call sites:

```
fired: 0x005aef00  original writes EAX at 0x005aef00 (mov eax, dword ptr [esp + 4])
  caller call site 0x005a8315 -> test eax, eax @ 0x005a831d
  caller call site 0x005be3db -> test eax, eax @ 0x005be3e3
```

**A passing self-test proves the detector fires on a true positive. It says nothing about false
positives** — and the first run had 20 of them, every one of the form `caller -> xor eax, eax`,
because capstone reports EAX in the *read* set of `xor eax,eax` (the ALU op reads both operands)
when it is really the idiomatic "EAX = 0", a pure definition. Fixed; 31 flags fell to 11.

## Funnel

| stage | count |
|---|---|
| installed hooks parsed | 1195 |
| returns a value | 604 |
| no caller consumes EAX | 407 |
| `__declspec(naked)` (EAX written by hand) | 115 |
| original never writes EAX | 45 |
| no definition found (parser miss) | 13 |
| **FLAGGED** | **11** |

## Triage — flags are candidates, not findings

Each flag was compared against the **emitted** body in the deployed `.asi`. Per the standing
rule, none of these has been fixed: an earlier audit tested 8 comment-flagged candidates and all
8 were false positives.

### Refuted — our emitted body leaves EAX equivalent to the original's

| RVA | why |
|---|---|
| `0x005abf80` `AudioWaveVtableSlot1cDispatch` | both load `EAX = *(*(p+0xc)+0x1c)`, test it, tail-dispatch; the fall-through RET carries the same value |
| `0x00426dc0` `FrontendRaycastForward` | original tail-calls `0x479100`; ours `call eax / add esp,0x10 / ret` — EAX is the callee's return in both |
| `0x0042fab0` `SpriteSlotDispatch` | original's jump-table arms rewrite `[esp+4]` and tail-`jmp 0x40bb90`; ours calls it and returns — EAX is the callee's return in both (36 call sites, so this one mattered) |

These are correct **by accident of codegen**, not by declaration. A `void` port whose EAX happens
to be right is one optimiser decision away from the U-9025 bug.

### Live candidates — emitted EAX differs from the original's

| RVA | original's EAX at RET | ours | caller consumes |
|---|---|---|---|
| `0x00485a00` `Init485a00` | `mov eax,[esp+0xc]` (param_3) | `mov eax,[edx+0x60]` | `mov [esi],eax` @ `0x00485211` |
| `0x005b1180` `Tbl5b1180` | `lea eax,[ecx+eax*4]` | `mov eax,[esi+4]` | `mov esi,eax`; `push eax` |
| `0x005ab980` `Ring5ab980` | `mov eax,ecx` | `add eax,esi` | `add ebp,eax` @ `0x005ab7f7` |
| `0x005b6a40` `SuccApproxQuantize5b6a40` | `mov eax,[edi]` | `or eax,4` | `and eax,0xff` |
| `0x004c5010` `RwMatrixScale` | scratch; RW convention returns the matrix | `mov eax,[ecx+8]` | `mov esi,eax` @ `0x004863e2` |
| `0x0042e590` `SpriteAnimFrameThunk` | small arg-rewriting tail-jmp to `0x40bb70` | emitted body looks **structurally unrelated** (`movss xmm0`, `sub esp,0x10`, many pushes) — inspect before assuming it is even the right symbol | `mov esi,eax`; `push eax` |

**Caveat on all six:** the "EAX at RET" column comes from a bounded *linear* disassembly to the
first `RET`. Functions with several RETs or forward branches can be misread, so each needs a
proper per-function pass before any change. `0x0042e590` is the one to look at first — a body
that does not resemble its original at all is a different and possibly larger problem.

### Previously unassessed — now checked via the `.map`, both REFUTED

`0x0055bd80` and `0x0057c420` are `extern "C"` but not `dllexport`, so the export table cannot
see them — the blind spot that hid `0x0055deb0` from an earlier audit. `sweep_void_eax_return.py
--triage` now falls back to `mashedmod/build/mashed_re_dev.map` (8028 symbols vs 1131 exports,
so the fallback is worth far more than these two rows).

**`0x0057c420` — refuted.** Both bodies end by calling `FUN_0055bd80` and returning its EAX:
the original `call 0x55bd80`, ours `call 0x37410`, which is exactly `_FUN_0055bd80`'s `.map`
RVA. The three callers' `push eax` forwards the same value either way.

**`0x0055bd80` — refuted.** Both end on the same indirect vtable call and leave its return in
EAX:

```
original                                  ours (.map rva 0x37410)
0055bdca  call dword ptr [edx + 0x10]     00037462  call eax
0055bdcd  add esp, 0x10                   00037464  mov ecx, [esp + 0x54]
0055bdd0  pop esi                         00037468  add esp, 0x10
0055bdd1  add esp, 0x40                   0003746b  pop esi
0055bdd4  ret                             0003746c  xor ecx, esp
                                          0003746e  call 0x7a0ca   ; __security_check_cookie
                                          00037473  add esp, 0x44
                                          00037476  ret
```

`__security_check_cookie` takes the cookie in ECX and preserves EAX, so EAX at the `RET` is the
vtable call's return on both sides. The three-way selection feeding that call also matches: the
original carries the chosen value in EAX (`[esp+0x4c]` on the flag path, `esi` when param_2 is
zero, else the return of `0x4c4600`); ours carries the identical selection in EDX and pushes it.

**Worth noting for other work, though not a defect here:** our build emits **/GS stack cookies**
in some hook bodies — a `mov eax,[__security_cookie] / xor eax,esp` prologue and a
`call __security_check_cookie` epilogue, which also grew the frame from `0x40` to `0x44`.
Harmless for a full-replacement hook. It would NOT be harmless for anything trampoline-shaped or
depending on exact stack layout, so check for it before converting a hook body to naked asm.

## Final tally

**5 refuted** (`0x005abf80`, `0x00426dc0`, `0x0042fab0`, `0x0055bd80`, `0x0057c420`),
**6 live candidates** (`0x00485a00`, `0x005b1180`, `0x005ab980`, `0x005b6a40`, `0x004c5010`,
`0x0042e590`), **0 unassessed**. Nothing fixed — each candidate needs a per-function pass, since
the "EAX at RET" column comes from a bounded linear scan that can misread multi-RET bodies.
Start with `0x0042e590`, whose emitted body does not structurally resemble its original.
