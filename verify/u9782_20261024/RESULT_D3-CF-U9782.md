# RESULT D3-CF-U9782: FUN_004709a0 retry counter vs StepSubstep

Step: D3-CF-U9782 (ROADMAP.md §D3 CLOSED block, carried item U-9782; UNCERTAINTIES.md:63).
Branch: `orch/D3-CF-U9782` from `orch/integration` @ `1889fb02d`. No source change.

## Verdict

**GREEN, no divergence. No knob.** The case U-9782 names (two ret-1 car-car contacts in one
pass 0, so `ebp` reaches 2) cannot occur in the original. `0x00469df0` returns `arg2 == 0`, and
`arg2` is the live `ebp`. After the first ret-1 call, `ebp` is 1, so every later call in the same
inner loop returns 0 and `0x00470bde inc ebp` does not run again. `ebp` therefore advances by
exactly 1 per retry, on the world path and on the car-car path, the same as the port's `pass`.
The port's `ccPass` is the same value the original passes. G-CTRL count: 0 pass-0 loops with
two ret-1 contacts out of 1524 pass-0 loops (section 3). Gates unchanged (section 4).

## 1. Static reading (MASHED.exe.unpatched, capstone, `disasm_004709a0_00469df0.txt`)

FUN_004709a0 retry body:

| RVA | instruction | effect |
|---|---|---|
| `0x00470ab0` | `cmp ebp,2` / `jge 0x470c47` | at most 2 executions, exit with no swap |
| `0x00470acb` | `mov [esp+0x1c],1` (written as `[esp+0x24]` under 2 pushes) | the car-car "settled" flag is set at every pass |
| `0x00470b0a` | `inc ebp` / `jmp 0x470ab0` | world retry: +1 |
| `0x00470bc3` | `push ebp` | **2nd stack arg of `0x00469df0` = live `ebp`** |
| `0x00470bc4` | `push ebx` | 1st stack arg = j |
| `0x00470bcd` | `call 0x469df0` | ECX = self (`esi`), EAX = other (`edi + 0x8815a0`) |
| `0x00470bd5` | `test eax,eax` / `je 0x470c12` | ret 0: next j |
| `0x00470bde` | `inc ebp` | ret 1: +1 |
| `0x00470be2` | `mov [esp+0x1c],0` | ret 1: the pass is not settled |
| `0x00470c25` / `0x00470c2b` | flag == 0 -> `je 0x470ab0` | retry |
| `0x00470c31..0x00470c41` | `+0x9ac = +0x9a8`, `+0x9a8 = (+0x9a8 - 1) & 1` | selector swap (committed pass only) |

`0x00469df0` returns:

| RVA | path |
|---|---|
| `0x00469df0` / `0x00469dfc` / `0x00469e03` / `0x00469e0a` / `0x00469e0d` | `sub esp,0x164` then push ebx, ebp, esi, edi: frame 0x174, return address at `[esp+0x174]`, arg1 at `+0x178`, arg2 at `+0x17c` |
| `0x0046a603` / `0x0046a605` | after the 2-pass hull loop (`cmp ecx,2` at `0x0046a5fa`): `test edx,edx` / `je 0x46b1a4` |
| `0x0046b1a4..0x0046b1b6` | `xor eax,eax` / `ret`: **returns 0** (no contact) |
| `0x0046ae9f..0x0046aec3`, `0x0046b054..0x0046b078`, `0x0046b17f` | the mode tests and the ring-head wrap all reach `0x0046b18b` |
| `0x0046b18b` / `0x0046b194` / `0x0046b196` / `0x0046b199` | `mov ecx,[esp+0x17c]` (arg2) / `xor eax,eax` / `test ecx,ecx` / `sete al`: **returns arg2 == 0** |

These are the only two `ret` instructions between `0x00469df0` and `0x0046b1b8`.

So per pass, with `e` = `ebp` at `0x00470ab0`:

- `e = 0`: the first contact call that gets past the hull test returns 1, `ebp` becomes 1. Every
  later call in the same j loop gets arg2 = 1 and returns 0. The pass leaves with `ebp = 1` and
  the flag 0, so it retries once. Two ret-1 calls in one pass 0 do not exist.
- `e = 1`: every call returns 0, the flag stays 1, the pass commits (swap at `0x00470c31`).
- A world retry at `0x00470b0a` adds 1 and jumps to the top before the j loop runs.

`ebp` at `0x00470ab0` therefore equals the number of earlier retries in this substep, which is
the port's `pass` (`VehiclePhysicsRun.cpp:1886`, `continue` at `:2034` and `:2124`). The port's
`ccPass` (`:2061`, `++` at `:2116`) starts at `pass` and passes the same value the original pushes
at `0x00470bc3`. `Collision::VehicleCarCarContact` returns `false` on no hull hit
(`CarCarContacts.cpp:161` / `:373`) and `pass == 0` otherwise (`:310` / `:523`), the same two
paths. U-9782's premise ("two car-car contacts in one pass 0 reach ebp 2") is refuted statically.

## 2. Counter validation on the logs U-9782 cites

`count_cc.py` on `D:/MashedWT/substep_data/arms3/B_DEFL_K{0,1,26}.cc.txt` (the substep lane's knob-off B arms):

| arm | calls | ret 1 | ret 1 with pass != 0 | pass-0 loops | pass-0 loops with two ret 1 |
|---|---|---|---|---|---|
| K0 | 225 | 4 | 0 | 199 | 0 |
| K1 | 402 | 28 | 0 | 352 | 0 |
| K26 | 233 | 4 | 0 | 207 | 0 |

Total 860 calls, 36 ret 1: the numbers in the U-9782 row.

## 3. G-CTRL count

Arm `H_DEFL` = the G-CTRL recipe (`run_arms.py`, 14400 frames) on the default exe plus
`MASHED_CARCAR_LOG` (stack `DEFL`). Log: `H_DEFL.cc.txt`, 1586 lines (under the 8000-line cap, so
the whole run is in it). First call frame 17, last call frame 12565. Counter output: `count_cc.out.txt`.

| quantity | G-CTRL |
|---|---|
| contact calls | 1586 |
| ret 1 | 33 |
| ret 1 with pass != 0 | 0 |
| pass-0 loops | 1524 |
| pass-0 loops with two ret 1 | **0** |
| pass-0 loops with any call after the first ret 1 (merged-run upper bound) | 0 |

The count the row marked `[UNCERTAIN: not counted on G-CTRL]` is 0. The second count is 0 as well,
so on G-CTRL no pass-0 loop has a second candidate pair after its first ret-1 contact. Section 1
says such a call would return 0 in both binaries anyway. The step's knob was conditional on a
non-zero count, so no knob and no PREREG.

## 4. Gates

Exe `04dd5187` (`orch/D3-CF-U9782` build, `MASHED_NO_DEPLOY=1`, no source change). Root
`D:/MashedWT/D3-CF-U9782_root`, lock label `D3-CF-U9782`. `RUNLOG.txt`, `run1.console.txt`.

| arm | step | gates | ptrace |
|---|---|---|---|
| H_DEF | bacfc747 | 8dd1c004 | 6f9932b6 |
| H_DEFL (+ log) | bacfc747 | 8dd1c004 | 6f9932b6 |
| B_DEF_K0 / K1 / K26 | f9deb9b9 / d3601fcc / 9da9a26a | | |

G-CTRL and B match the default trios. The log arm matches too, so the log does not perturb the run.

Harness note: `mashed_lock.py run` printed `breaking STALE 'mashed_machine' (pid=None alive=False
age=1792873612s)` right after `WAITING ... held by pid=14560 (D4-AUDIO-VOLTODS-FLIP)`. pid 14560 was
not alive when checked, so the break itself was correct. The `pid=None` / epoch-sized age reads as
a lock file read between the holder's release and the next write [UNCERTAIN: not reproduced].
Possible follow-up for the HB-LOCKPRIO owner.

## 5. Tracker effect (for re-classify, not applied here)

U-9782 resolves as "no divergence": the retry counter of FUN_004709a0 and `StepSubstep`'s `pass`
are the same value at every `0x00470ab0`, by the return law of `0x00469df0` (`0x0046b18b..0x0046b199`).
No knob, no PREREG (the step's knob was conditional on a non-zero count).
