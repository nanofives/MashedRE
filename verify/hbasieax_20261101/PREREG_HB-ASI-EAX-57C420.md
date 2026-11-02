# PREREG HB-ASI-EAX-57C420: typed EAX return for 0x0057c420 / 0x0055bd80, then the 0x0057c420 shadow arm

Branch `orch/HB-ASI-EAX-57C420` off `orch/integration` @ `6d9d01a7f`, with `orch/D4-WSB-PORT-NARROW-TAIL` @ `6afb1ae11`
merged in (`83660e8ad`, conflicts in `hooks.csv` / `asi_sources.rsp` / `CHANGELOG.md` resolved by side: the 3
broad-2 rows from integration, the 2 narrow-tail rows from narrow-tail, union for the rest). This PREREG is committed
alone, before any arm.

## Source of the step

`verify/d4wsb_portnarrowtail_20261101/RESULT_D4-WSB-PORT-NARROW-TAIL.md`, "The 4 rows that stay C2": the ports
`FUN_0057c420` (`Collision/RwpVtableKV2.cpp`) and `FUN_0055bd80` (`Collision/RwpSolverGlue5.cpp`) are typed `void`,
while the original's EAX at `0x0057c43d` is `0x0055bd80`'s EAX (`call 0x55bd80` / `add esp,0x10` / `ret` at
`0x0057c435..0x0057c43d`), which is the EAX of the slot call `call dword ptr [edx+0x10]` at `0x0055bdca` (`add
esp,0x10` at `0x0055bdcd`, `ret` at `0x0055bdd4`). The three direct callers `0x0047d213`, `0x0047d30e`, `0x00480263`
`push eax` as the third argument of `0x0055ab30`, which tests it at `0x0055ab69..0x0055ab6f`.

## Static leg S0 (done before this PREREG): what the default `.asi` returns today

Disassembly of the narrow-tail build `mashed_re_dev.asi` `343d8591...` (`mashedmod/build/mashed_re_dev.map` of that
worktree, capstone, script `dis_asi.py` committed here):

| function (.asi VA) | last instructions | EAX at `ret` |
|---|---|---|
| `_FUN_0057c420` `0x100aefb0` | `call _FUN_0055bd80` / `add esp,0x10` / `ret` | `_FUN_0055bd80`'s EAX |
| `_FUN_0055bd80` `0x10068600` (shadow wrapper) | `call RunTracked<void>` / `add esp,0x1c` / `ret` | `RunTracked`'s EAX |
| `RunTracked<void(int,int,uint,uint)>` `0x10067c80`, un-armed path `0x1006815a` | `call [ebp-0x424]` (the impl) / `add esp,0x10` / `mov ecx,[ebp-0xc]` / `mov fs:[0],ecx` / `pop ecx` / `pop edi` / `pop esi` / `mov ecx,[ebp-0x10]` / `xor ecx,ebp` / `call __security_check_cookie` / `mov esp,ebp` / `pop ebp` / `ret` | the impl's EAX (no EAX write; the cookie check reads ECX) |
| `FUN_0055bd80_impl` `0x100682c0` | `call eax` (the slot, `0x10068312`) / `mov ecx,[esp+0x54]` / `add esp,0x10` / `pop esi` / `xor ecx,esp` / `call __security_check_cookie` / `add esp,0x44` / `ret` | the slot's EAX |

So with the shadow lane **un-armed** (every default run), the `.asi` already hands the slot's EAX back through both
ports, by codegen accident. The value is lost only on the **armed** path of `RunTracked` (`MASHED_SHADOW_AB=1` with
`0x0055bd80` sampled), whose compare and log code runs after the impl.

## Change (code commit after this PREREG)

- `FUN_0055bd80` / `FUN_0055bd80_impl` / the `RwpVolFnApply` slot type return `undefined4`: `return` of the slot call
  (`0x0055bdca`). The wrapper returns `RunTracked`'s result.
- `FUN_0057c420` returns `FUN_0055bd80`'s result. On the `.asi` it is split into `FUN_0057c420_impl` plus an
  `.asi`-only `RunTracked` wrapper (`kPhaseAny`), the D4-WSB-PORT-NARROW-TAIL idiom of `0x0057c440`; the exe keeps
  the unwrapped body.
- The `extern` declarations of `FUN_0055bd80` in `RwpSolverCore18/21/22.cpp` and `RwpVtableKV2.cpp` match. None of
  those call sites reads the result.

**No knob.** The narrow-tail RESULT proposed a default-OFF knob on the reading that the fix changes default `.asi`
behaviour. S0 shows the default (un-armed) value is already the slot's EAX, so typing the return keeps the default
value. A knob would choose between two paths that return the same value. Static leg S1 below checks this on the new
build. If S1 shows any default-path EAX other than the slot's, the code is reverted to a default-OFF knob form before
any arm, and that is recorded as an amendment.

## Static leg S1 (new build, before the arms)

Disassemble the new `.asi` `_FUN_0057c420`, `FUN_0057c420_impl`, `_FUN_0055bd80`, `FUN_0055bd80_impl` and both
`RunTracked` instantiations' un-armed paths, and the exe's `_FUN_0057c420` / `_FUN_0055bd80`. PASS = on every
un-armed path the value at `ret` is the slot call's EAX.

## Arms (private root `D:/MashedWT/HB-ASI-EAX-57C420_root`, new `.asi` copied there only, machine lock, audio check)

Runner: `run_shadow11.py` (copy of the narrow-tail `run_shadow10.py`, data path and label changed), driver
`drive_hbe.py` (copy of `drive_nt.py`, runner name, data path and label changed). Scenario = the census one: track 0,
mode 10, 4 cars, 60 s hold, `--hooks 0x<rva>` (that hook installed alone, `MASHED_HOOK_ONLY`), `MASHED_SHADOW_AB=1`,
`SAMPLES_DONE=24`.

| arm | row | why |
|---|---|---|
| S-420 | `0x0057c420` | the held row: return now compared with the write set |
| S-D80 | `0x0055bd80` | C2 since D4-WSB-C3-PORTED (2026-10-27): n=1 CLEAN, then the game exited during race load 2/2 boots. Its armed path dropped the slot EAX before this change, and `0x0057c420`'s callers test that EAX. Report-only re-arm on the new body |

## Verdict rules (fixed now)

- Row CLEAN = every sample `ndiff=0`, `PATCHBYTE installed=e9 ... A/B-IS-REAL` for the row's own hook, n >= 1.
- C2 -> C3 through `re-classify` only when the row is CLEAN and the caller / callee halves hold:
  - `0x0057c420`: callers `0x0047d180` C2, `0x0047d240` C2, `0x00480100` C2 (L23 caller half); callee `0x0055bd80`
    C2 (L23 callee half).
  - `0x0055bd80`: caller `0x0057c420` (C2 or C3 after this step); callees `0x004c4600` and the descriptor `+0x10`
    slot (indirect-dispatch clause covers only an indirect-only callee set, so `0x004c4600` must be C2+ or
    identified). Checked from `hooks.csv` at re-classify time; if it fails, the row stays C2 with the CLEAN result
    recorded.
- DIVERGENT -> the row stays C2, the differing dwords and the return pair (`o=`, `n=`) recorded in the RESULT.
- No sample (game exit before the first sample, no log) -> stays C2, the transcript cited. For `0x0055bd80` a repeat
  of the 2026-10-27 exit is a finding to report (the exit survives the EAX fix), not a re-run.
- Launch failure (`could not attach`) -> one re-run on the same build and env. U-9677 piz signatures -> re-run.
- No C4: no race diff with the hook installed is in scope.

## Default gates (exe)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this step's exe, test slot + machine
lock, private root. Must equal `re/orchestrator/merge_train.json` (G-CTRL `8284be46/ba407a16/31fe043e`, B
`1d5bf4f9` x3). Any mismatch -> RED, nothing promoted.
