# Dual-install audit — 39 refusals in one boot (2026-09-09)

Arming the full hook set logged **39 DUAL-INSTALL REFUSED** lines. `HookSystem` refuses to
install a hook over an RVA that already carries an `E9`, because saving our own JMP as the
"original prologue" would permanently corrupt the restore path. Every refusal means **the
refused hook is not installed for that run** — it is dead code.

Tool: `re/tools/dualinstall_audit.py --tail 39` (classifies, names both copies, and checks
whether `hooks.csv` points at the winner or the corpse).

The 39 split into three very different causes, and lumping them together is what made my
first summary wrong ("39 RVAs carrying two competing implementations" — it is 28):

| cause | count | meaning |
|---|---:|---|
| **E9-thunk false positive** | **10** | the ORIGINAL function begins with `E9` and our guard misreads it |
| real boot-patch collision | 1 | `FsopenSafe` @ `0x004a4541` vs the `fix_fopen` patch |
| DUP-REGISTRY | 28 | two of OUR implementations claim one RVA |

## 1. The guard has a false-positive class: original jump thunks (10 hooks NEVER install)

The names were the tell — `TimerInitThunk`, `ThunkVideoStateGet`, `ThunkReplaySave`,
`thunk_LaunchLangGate`, `SlotActiveThunk`, `Global7d3e4cGetThunk`. Read from the anchored
**unpatched** binary:

```
0x004222c0 TimerInitThunk           e9 5b fe ff ff
0x00494ef0 ThunkVideoStateGet       e9 7b f0 ff ff
0x0040de00 ThunkReplaySave          e9 ab 39 00 00
0x0040cd90 CountNonZeroPairs        e9 5b 91 01 00
...  10 of 11 begin with E9 in the PRISTINE binary
```

These are compiler-generated jump thunks. `HookSystem` sees `E9`, concludes "already hooked",
and refuses — **every run, permanently**. It cannot distinguish *our* JMP from the original's
own JMP.

**Evidence consequence:** these 10 rows can never carry installed-hook evidence. **4 are C4**
— `0x00494ef0 ThunkVideoStateGet`, `0x0040de00 ThunkReplaySave`, `0x0040cd90 CountNonZeroPairs`,
`0x00496900 SlotActiveThunk` — plus 6 C3. Filed as **U-9087**.

**Candidate fix, NOT applied** (it changes core install behaviour for every hook and would
re-enable 10 hooks that have never once run — an architecture-level change that wants an
explicit decision): distinguish the two cases by *where the E9 points*. If the jump target
lands inside `mashed_re_dev.asi`, it is ours and the refusal is right; if it lands inside
`MASHED.exe`, it is the original's own thunk and hooking is safe. `HookSystem` already saves
the prologue bytes, so it can also compare against the pristine expected bytes.

## 2. One genuine collision

`FsopenSafe` @ `0x004a4541` is refused because the **`fix_fopen` boot patch** already put a
detour there (CLAUDE.md "Runtime state", one of the ten patches). A correct refusal, but it
means this C4 hook never installs while the boot recipe is applied. Worth a tracker row of its
own — the hook and the patch are two implementations of the same fix.

## 3. 28 duplicate-RVA implementations — and 25 trackers name the dead copy

Two of our own registered symbols claim the same RVA; whichever registers first wins, the
other is dead code. **25 of the 28 have `hooks.csv`'s `file` column pointing at the LOSER**,
so the tracker documents the copy that does not run. Six are C4 rows.

```
0x0042af50 C3   WINS MenuReadinessCheck      (TimerReset.cpp)
               DEAD MenuReadinessCheckC     (MenuStateMachine.cpp)   <-- tracker points here
0x0046cbb0 C4   WINS VehicleCarStateRead     (VehicleState.cpp)
               DEAD CarStatePairGet         (PromoLoop_round25.cpp)  <-- tracker points here
0x004f8660 C3   WINS PluginFieldReadA8       (PluginFields_ah4.cpp)
               DEAD PluginDataDwordA        (StateBatchGetters.cpp)  <-- tracker points here
```

This sizes U-9065 / `duplicate-rva-implementations-drift` properly: not the 2–3 known cases,
**28 in a single boot**, most of them mis-tracked. Note the winner is decided by static-init
order, i.e. link order — so it can flip silently when the source list changes.

## Scope note

39 refusals is what **one** full-set boot produced. The accumulated log holds 1,252 distinct
RVAs across months of differing hook configurations; that larger number is not a count of
today's duplicates and should not be quoted as one.
