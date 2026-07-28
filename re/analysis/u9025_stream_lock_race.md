# U-9025 — the wedge is an unsynchronised gate around the stream lock (ORIGINAL-CODE defect)

Status: **mechanism proposed from disassembly; awaiting the runtime trace that confirms it.**
Session 2026-07-28 (orchestrator, account3). Anchored to
`MASHED.exe` SHA-256 `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`
(disassembly taken from `original/MASHED.exe.unpatched`, and from `Mashed_pool5`).

## Prior state

The previous session pinned the wedge to one instruction: the GUI thread parks in
`WaitForSingleObject([0x007dcae0], INFINITE)` at `0x005a8406`, inside `FUN_005a8390`.
`0x007dcae0` is `CreateSemaphoreA(NULL, init=1, max=1, NULL)` — a binary semaphore that
starts SIGNALLED — and it probes `WAIT_TIMEOUT` at wedge time, i.e. **count 0: an unpaired
acquire**. `[0x007dcb68]` measured `1` at wedge time. The leading hypothesis carried into
this session was self-deadlock by re-entrancy.

## New measurement 1 — the lock is DEAD on a healthy run

`re/frida/statenav.py` gained an `semtrace` rpc export (this session): it attaches to the
IAT-bound `WaitForSingleObject` / `ReleaseSemaphore` (`[0x005cc090]` / `[0x005cc094]`) and
filters on `handle == *[0x007dcae0]`, read live on every call because the handle does not
exist at spawn time. `this.returnAddress` names the call site as `site+6`.

Two complete menu-navigated race runs (`log/wedge_run0.log`, hook set = manifest indices
[75,150), the wedging configuration) recorded:

- `*[0x007dcae0] = 2804` — the semaphore **is** created.
- **ZERO** waits and **ZERO** releases on that handle across an entire completing run.
- The tracer is not degenerate: with the handle filter off it logged 40 events in seconds
  (`handle 2952`, sites `0x005c702e` / `0x005c75d7`) — a *different* semaphore.

So on the healthy path the whole `0x005a8280..0x005a8c8x` locking cluster is inert.

## New measurement 2 — why it is inert, and where it breaks

`FUN_005a8390` (Ghidra `Mashed_pool5`, decompiled + disassembled) gates **both** halves of
its critical section on the same global, read **twice, independently**:

```
005a83f4  MOV EAX,[0x007dcb68]          <- acquire gate  (read #1)
005a83f9  TEST EAX,EAX
005a83fb  JZ  0x005a840c                    ... skip the acquire when 0
005a83fd  MOV EDX,[0x007dcae0]
005a8406  CALL [0x005cc090]             WaitForSingleObject(sem, INFINITE)
005a840c  ... doubly-linked-list insert into [0x007dcad8] ...
005a8423  MOV EAX,[0x007dcb68]          <- release gate  (read #2, INDEPENDENT)
005a8428  TEST EAX,EAX
005a8430  JZ  0x005a8443                    ... skip the release when 0
005a843d  CALL [0x005cc094]             ReleaseSemaphore(sem, 1, NULL)
005a8443  MOV EAX,[0x007dcb68]          <- non-atomic read-inc-write
005a8449  INC EAX
005a844a  MOV [0x007dcb68],EAX
```

`[0x007dcb68]` is a count of registered objects, and it is **0 until the first registration
completes** — which is exactly why a healthy run records no semaphore traffic at all.

The matching unregister at `0x005a8460` decrements it, non-atomically, and **before** taking
the lock:

```
005a8460  MOV EAX,[0x007dcb68]
005a846a  DEC EAX
005a846b  MOV [0x007dcb68],EAX          <- decrement is NOT under the lock
005a8470  MOV EAX,[0x007dcae0]
005a8478  CALL [0x005cc090]             WaitForSingleObject(sem, INFINITE)   (UNGATED)
   ...
005a84ba  CALL [0x005cc094]             ReleaseSemaphore(sem, 1, NULL)       (UNGATED)
005a84c1  RET
```

`0x005a8460` is self-consistent: both of its calls are ungated, so it is always paired.
`FUN_005a8390` is not. `[0x007dcb68]` is written at only six operand sites, all inside these
two functions (`0x005a83f5, 0x005a8424, 0x005a8444, 0x005a844b, 0x005a8461, 0x005a846c`),
so the whole search space is these two bodies.

### The predicted failure interleaving

Thread A in `FUN_005a8390`, thread B in `FUN_005a8460`:

1. A reads the gate at `0x005a83f4`, sees `1`, **acquires** — semaphore count 1 → 0.
2. B decrements the gate at `0x005a846b`, `1 → 0`, then blocks at `0x005a8478` (A holds it).
3. A reads the gate again at `0x005a8423`, now sees `0`, so it **skips the release** at
   `0x005a843d`. The semaphore is left at 0 **forever**.
4. A increments the gate at `0x005a844a`, `0 → 1`.
5. Every later entrant reads gate `= 1` at `0x005a83f4` and blocks at `0x005a8406`.

This reproduces all three measured facts simultaneously: wedge frame `0x005a840c`
(the instruction after `0x005a8406`), semaphore count `0`, and **gate `== 1`**.

**Relation to the refuted hypothesis.** The previous session pre-registered and refuted a
"gate-zero" reading which predicted `[0x007dcb68] == 0` *at wedge time*. That prediction is
refuted and stays refuted. The mechanism above is a different one and predicts `== 1`,
because step 4 restores the gate after the release was already skipped. It is not a
re-opening of the refuted claim.

**This is a defect in the original game code, not in our port.** Neither call site is under
an interlocked operation, and the port installs no hook in this cluster. Hooks change
*timing* only, which is consistent with stock 6/6 vs hooked-pre-fix 2/6.

## The confirming test — RUN, AND IT REFUTED THE MECHANISM ABOVE

Two wedges were caught with the tracer live (`log/wedge/wedge_a2_pid39092.txt` +
`log/wedge_run2.log`; `log/wedge/wedge_a1_pid23412.txt` + `log/wedge_run1.log`). Both agree,
and both **falsify the interleaving proposed above**:

| quantity | predicted | measured (both wedges) |
|---|---|---|
| gated `wait-leave rv=0` before the wedge | >= 1 | **0** |
| `release` events on the handle | one fewer than acquires | **0** |
| the blocking `wait-enter` | a later one | **the first and only one** |

The wedging wait at `MASHED.exe+0x1a840c` is the **first** wait ever issued on that handle in
the process. `FUN_005a8390` never completed a critical section, so it cannot have skipped a
release. The asymmetry documented above is real in the code, but it is **not** what fires.

### What the extended trace shows instead

`semtrace` was widened to `CreateSemaphoreA`, `WaitForSingleObjectEx`,
`WaitForMultipleObjects{,Ex}` and `MsgWaitForMultipleObjects{,Ex}`. In the wedged run
(`log/wedge_run1.log`), with `[0x007dcae0] = 0xa88 = 2696`:

- `seq 3`: `CreateSemaphoreA -> handle 2696, init=1, max=1` on the main thread — the runtime
  initial count is **measured**, not inferred; the semaphore does start signalled.
- `seq 5..16`: a **different thread (tid 32248)** waits on handle 2696 three times via
  `WaitForMultipleObjects(nCount=1)`, from return addresses **outside `MASHED.exe`**
  (`0x72932f20`, `0x7293308a`, and `0x75d5e3b8` inside the `...Ex` inner call). Results:
  `WAIT_TIMEOUT`, then **`WAIT_OBJECT_0`**, then **`WAIT_OBJECT_0`**.
- No `ReleaseSemaphore` on that handle, ever.
- `seq 58`: the GUI thread's `MASHED.exe+0x1a840c` wait — blocks forever.

Two `WAIT_OBJECT_0` results on a `max=1` semaphore with no observed release are not
self-consistent, so **one of the two premises is wrong**: either those return addresses do
not belong to a caller waiting on *this* object, or a release happens through a path that
does not cross the `kernel32!ReleaseSemaphore` export (e.g. `ntdll` directly). Resolving
that needs the **owning module** of those addresses, which the tracer did not record. It now
emits every site as `module+0xoffset`; a third hunt is running to get it.

**Do not report a mechanism for U-9025 until that attribution lands.** What IS newly
established is negative and solid: *the wedge is not an unpaired acquire by `FUN_005a8390`,
because `FUN_005a8390` never acquires successfully at all.*

## The pre-registered test as originally stated (kept for the record)

In a trace of a wedged run, `semtrace` must show:

- a nonzero number of gated events at all (proving `[0x007dcb68]` became nonzero), and
- **exactly one more `wait-leave rv=0` than `release`**, with the imbalance attributable to
  a missing release at site `0x005a8443`, and
- a final `wait-enter` with **no** `wait-leave`.

If instead the counts balance, or the blocked wait is the first event on the handle, this
mechanism is wrong and re-entrancy / a third party must be reconsidered.

## Open

- `FUN_005a8390` has **no static callers** (`function_callers` = 0) — it is reached
  indirectly, consistent with the RenderWare stream vtables noted in U-6700.
- The region `0x005a8a1c..0x005a8c8x` holds further sites touching `0x007dcae0` that Ghidra
  has not resolved into functions; they are not needed for this mechanism but are unaudited.

---

## STOCK vs HOOKED control (2026-07-28) — the decisive experiment

Pre-registered before the runs: *if DSOUND's consumption of the count is hook-independent,
every stock run shows at least one `DSOUND.dll` wait on `[0x007dcae0]` returning
`WAIT_OBJECT_0`; zero DSOUND events across all stock runs means the hooked build is what
puts DSOUND on that handle.*

Three stock runs (`MASHED_RE_NO_AUTO_HOOK=1`, tracer live, all three completed the round:
`log/semstock_run{0,1,2}.log`) against the three hooked runs (`log/wedge_run{0,1,2}.log`):

| | STOCK (3 runs) | HOOKED (2 wedges) |
|---|---|---|
| `CreateSemaphoreA` caller | `MASHED.exe+0x1aeeb4` (`0x005aeeb4`, inside `FUN_005aeea0`) | `mashed_re_dev.asi+0xdf32` (our `AudioSemaphoreCreate` port of `0x005aeea0`) |
| acquires / releases | 5767/5767, 5719/5719, 5659/5659 — **net 0 every run** | 5/12 and 1001/1007 — **net −7 and −6** |
| `DSOUND.dll` waits on the handle | **NONE, 0/3 runs** | `DSOUND.dll+0x42f20` ×2, `+0x4308a` ×1, **4 × `WAIT_OBJECT_0`** |
| thread doing the lock work | a **worker** thread (`tid 4428`; two workers in runs 1–2), never the GUI thread | the **main/GUI thread** — the same tid that created the semaphores and the one that wedges |

**Conclusion (measured, 3/3 vs 2/2):** DSOUND does **not** touch this semaphore in the stock
game. The consumption is introduced by the hooked build.

Two differences appear together and either could be the cause:

1. **Ownership.** Only in the hooked build does `DSOUND.dll` wait on the object stored at
   `[0x007dcae0]` and take counts it never returns (4 × `WAIT_OBJECT_0`, 0 releases).
2. **Threading.** Stock runs the stream lock on a dedicated worker thread; the hooked build
   runs it on the **GUI thread**, so an `INFINITE` wait there is a self-deadlock rather than
   a background stall. The prior session's re-entrancy intuition was pointed at the right
   phenomenon by the wrong route.

Also note stock's acquire/release balance is **exactly** 0 in all three runs, whereas both
wedges are net-negative — i.e. the over-release branch of the `FUN_005a8390` gate asymmetry
(release-without-acquire) only fires under the hooked build. The asymmetry is real original
code, but stock never hits its window.

### Next measurement (targeted, not a bisect)

The suspect set is now small and named, not the 75-hook range. `Audio/AudioDSound.cpp`
installs `AudioThreadDescInit` at **`0x005aef00`** and `AudioSemaphoreCreate` at
**`0x005aeea0`**, plus `AudioDSoundSecondaryInit` `0x005bbfc0` and `AudioOutputNodeCbDispatch`
`0x005a9e40`. Trace `CreateThread` (caller module + start address) stock vs hooked: if the
hooked build fails to spawn the stream worker, that is the defect, and `AudioThreadDescInit`
is the first thing to check.

---

## CreateThread census (2026-07-28) — the hooked build never spawns the stream worker

Pre-registered: *stock must show a `CreateThread` whose returned tid is the tid doing the
lock work; the hooked build must lack it. The census must log `CreateThread` events in BOTH
configs, or an absence proves nothing.* Census is unfiltered by design.

Non-degeneracy: 96–101 `CreateThread` events logged in **every** run, both configs.

| run | CreateThread with a game/`.asi` start routine | tid doing the lock work | spawned? |
|---|---|---|---|
| STOCK `thr_stock0` | 1 — `start=MASHED.exe+0x1c2dd9`, `site=MASHED.exe+0x1c2ed0` | 38264 (5716 events) | **yes** |
| STOCK `thr_stock1` | 1 — same start/site | 16912 (9726 events) | **yes** |
| HOOKED `wedge_run0` | **0** | none | — |
| HOOKED `wedge_run1` | **0** | 32324 (1976 events) | **no** |
| HOOKED `wedge_run2` | **0** | 15972 (2024 events) | **no** |

`MASHED.exe+0x1c2ed0` = `0x005c2ed0`, the `CreateThread` call inside the statically-linked
CRT `__beginthread` (`0x005c2e79`; consistent with the known CRT band `0x5c0000..0x5c8000`).
`Mashed_pool5` gives `__beginthread` exactly **one** caller: **`FUN_005aef30`**:

```c
undefined4 * FUN_005aef30(undefined4 *param_1, undefined4 param_2) {
  param_1[2] = param_2;
  hThread = (HANDLE)__beginthread((_StartAddress *)&LAB_005aef70, param_1[4], param_1);
  if (hThread == (HANDLE)0xffffffff) return 0;
  *param_1 = hThread;  SetThreadPriority(hThread, param_1[3]);  return param_1;
}
```

and its descriptor comes from `FUN_005aef00` — which **we hook**
(`Audio/AudioDSound.cpp:305`, `AudioThreadDescInit`). The spawn chain in `FUN_005bb000` is:

```c
iVar6 = FUN_005aeea0(puVar4 + 0x3d, 0, 1);        // CreateSemaphoreA(NULL, init=0, max=1)
if (iVar6 != 0) {
  FUN_005aef00(puVar4 + 0x3e, &LAB_005bb380, 0xf, 0x1000);   // <- HOOKED (AudioThreadDescInit)
  iVar6 = FUN_005aef30(puVar4 + 0x3e, puVar1);               // <- spawns the worker
  if (iVar6 != 0) return;                                    //    success
  CloseHandle((HANDLE)puVar4[0x3d]);                         //    FAILURE PATH closes the semaphore
  ... teardown ...
}
```

The `init=0, max=1` create in that chain is exactly the `create ... "init": 0` event present
at `seq 4` of **every** trace, stock and hooked — so in the hooked build execution *does*
reach `FUN_005aeea0` and therefore enters the `if`. The break is therefore between that
create and `__beginthread`, i.e. inside `FUN_005aef00` (**hooked**) or `FUN_005aef30`
(not hooked).

The failure path also **`CloseHandle`s the semaphore**, which supplies a mechanism for the
otherwise-inexplicable `DSOUND.dll` waits: a closed handle's value is recycled by the next
kernel object, so a later wait on a stale stored handle can land on a DirectSound object.

### Verified NOT the defect

`FUN_005aef00` is plain `__cdecl` with four stack arguments
(`0x005aef00 MOV EAX,[ESP+4]` … `0x005aef23 MOV [EAX+0x10],ECX`), and our
`AudioThreadDescInit` writes the same five fields in the same order with the same values.
The port body matches the original field-for-field.

**Unaudited:** the original leaves `EAX = param_1` on return (loaded at `0x005aef00`, never
clobbered). Our `void` C++ port returns whatever `EAX` happens to hold — the implicit-EAX-
return defect class of `feedback_installed_hook_abi_mismatch`. Whether any caller consumes
it is not yet established.

### Next measurement

Count entries to `0x005aef00`, `0x005aef30` and `0x005c2e79` in one hooked run. Whichever is
the first with count 0 localises the break to a single call. Cheap: three `countthese` RVAs,
one run, no bisect.

---

## ROOT CAUSE — `AudioThreadDescInit` (`0x005aef00`) drops the original's implicit EAX return

### Entry counts (milestone snapshots, so a wedged run still reports)

`MASHED_COUNT_RVAS` was added to `statenav.py`, with `dump_counts()` at title /
track-confirm / start-attempt — the end-of-run dump never happens on a wedge, which is
exactly when the numbers matter.

| RVA | STOCK (2 runs) | HOOKED (2 runs) |
|---|---|---|
| `0x005bb000` | 2 | 2 |
| `0x005aeea0` `AudioSemaphoreCreate` | 6 | 6 |
| `0x005aef00` `AudioThreadDescInit` | **1** | **1** |
| `0x005aef30` (spawner) | **1** | **0** |
| `0x005c2e79` `__beginthread` | **1** | **0** |

Control reaches the descriptor init in both, and only stock proceeds to the spawner.

A follow-up run probing the call sites `0x005bb2a0 / 0x005bb30b / 0x005bb312` read **0 in
both configs**, including stock — so `FUN_005bb000` is *not* the caller that spawns the
worker, and the earlier attribution to it was wrong. `function_callers` gives three callers
for both `0x005aef00` and `0x005aef30`: `FUN_005a8060`, `FUN_005bb000`, `FUN_005be260`.
**`FUN_005a8060` is the stream-cluster init itself** — the function that creates
`[0x007dcae0]` at `0x005a82d2`. The semaphore and the worker are born in the same function.

### The defect

```
005a8315  CALL FUN_005aef00
005a831a  ADD  ESP,0x10
005a831d  TEST EAX,EAX          <- the caller TESTS the return value
005a831f  JZ   +0x13            <- skips the spawn when EAX == 0
005a8321  PUSH 0 / PUSH ...
005a8328  CALL FUN_005aef30     <- __beginthread happens here
```

`FUN_005be260` does the same at `0x005be3db` (`ADD ESP,0x10 / TEST EAX,EAX / JZ +0x1f`).

Ghidra types `FUN_005aef00` as `void`, but the original's body never clobbers `EAX` after
its first instruction:

```
005aef00  MOV EAX,[ESP+4]        <- EAX = param_1, and it survives to the RET
005aef04  MOV ECX,[ESP+8]
005aef08  MOV EDX,[ESP+0xc]
005aef0c  MOV [EAX+4],ECX
005aef0f  MOV ECX,[ESP+0x10]
005aef13  MOV dword ptr [EAX],0
005aef19  MOV dword ptr [EAX+8],0
005aef20  MOV [EAX+0xc],EDX
005aef23  MOV [EAX+0x10],ECX
005aef26  RET                    (C3 — plain __cdecl, verified in the image)
```

So the original **always** returns the (non-null) descriptor pointer and the branch is
always taken. Our port is declared `void`:

```cpp
extern "C" __declspec(dllexport) void __cdecl AudioThreadDescInit(...)
```

and therefore returns whatever `EAX` happens to hold after a C++ body that ends in stores.
When that is 0, `JZ` is taken and **the audio/stream worker thread is never created.**

This is the implicit-EAX-return case of `feedback_installed_hook_abi_mismatch`. The port's
five field writes are correct; only the return is wrong.

### Everything it accounts for

- `0x005aef00` reached, `0x005aef30` and `__beginthread` not (measured, 2/2 vs 2/2).
- No game `CreateThread` in any hooked run; exactly one in every stock run.
- `[0x007dcae0]` still exists in hooked runs — it is created at `0x005a82d2`, *before* the
  spawn branch.
- The stream lock is either idle or driven from the GUI thread, so an `INFINITE` wait there
  is a self-deadlock rather than a background stall.
- Non-determinism: whether the game later waits on the never-serviced semaphore depends on
  timing, which is why completion is ~2/6 rather than 0/6.

### Fix shape (not yet applied)

Return the descriptor pointer:

```cpp
extern "C" __declspec(dllexport) unsigned int __cdecl AudioThreadDescInit(
        std::uint32_t* param_1, ...) { ...; return (unsigned int)param_1; }
```

Then disassemble the `.obj` and confirm `EAX` holds `param_1` at the `RET` — per
`feedback_msvc_inline_asm_needs_ds_override`, "it compiled" is not evidence.

### Also found (documentation defect, not runtime)

The plate above `AudioSemaphoreCreate` in `Audio/AudioDSound.cpp` carries a fabricated ASM
key: it lists `SETNZ AL / NEG AL / MOVZX EAX,AL` at addresses `0x005aeaab..0x005aeac1`
which are not even inside the function. The real bytes at `0x005aeea0` are
`8b 44 24 0c | 8b 4c 24 08 | 6a 00 | 50 | 51 | 6a 00 | ff 15 a4c05c00 | 8b 4c 24 04 |
89 01 | f7 d8 (NEG EAX) | 1b c0 (SBB EAX,EAX) | 23 c1 (AND EAX,ECX) | c3` — a full-width
mask, so the C++ port's behaviour is correct. The comment is wrong, the code is not; the
`feedback_wrong_plate_propagates_into_ports` class, caught before it propagated.

---

## FIX APPLIED AND VERIFIED (2026-07-28)

`AudioThreadDescInit` now returns `std::uint32_t` = `param_1`. Emitted body dumped from the
deployed `original/mashed_re_dev.asi` export (RVA `0x0000df80`) — "it compiled" is not
evidence:

```
8b 44 24 04        MOV EAX,[ESP+0x4]        ; EAX = param_1  ... and survives to the RET
8b 4c 24 08        MOV ECX,[ESP+0x8]
89 48 04           MOV [EAX+0x4],ECX
8b 4c 24 0c        MOV ECX,[ESP+0xc]
89 48 0c           MOV [EAX+0xc],ECX
8b 4c 24 10        MOV ECX,[ESP+0x10]
c7 00 00000000     MOV dword ptr [EAX],0
c7 40 08 00000000  MOV dword ptr [EAX+0x8],0
89 48 10           MOV [EAX+0x10],ECX
c3                 RET
```

Register footprint vs the original: EAX = `param_1` at the RET (the property the callers
test); ECX used as scratch, as in the original. Ours does not touch EDX where the original
does — preserving *more* than the original is safe, since no caller may rely on EDX being
destroyed. Store order to the five distinct offsets differs from the original's and is
unobservable: this runs before the thread it describes exists.

### Behavioural verification, hooked build, 2/2

| | before the fix | after the fix | STOCK |
|---|---|---|---|
| `0x005aef00` / `0x005aef30` / `0x005c2e79` | 1 / **0** / **0** | **1 / 1 / 1** | 1 / 1 / 1 |
| game `CreateThread` | **0** | **1** (`start=MASHED.exe+0x1c2dd9`) | 1 |
| stream-lock acquire / release | 5/12 and 1001/1007 (**net −7, −6**) | **2018/2018, 4010/4010** | net 0 every run |
| round completed | wedge | 2/2 | 3/3 |

The lock's acquire/release balance returning to exactly 0 is the sharpest single indicator:
the over-release branch of the `FUN_005a8390` gate asymmetry stops firing once the worker is
back, which also explains why stock never reaches that window.

A wedge-rate test (`catch_wedge.py --attempts 8`) is running; prior to the fix wedges were
caught on attempts 3, 2 and 3 (≈1 in 3), so 8 consecutive completions would be p≈0.04.

### Tracker consequence — NOT YET APPLIED (needs sign-off)

`hooks.csv` row `005aef00` is **C3** citing `log/diff_audio_thread_desc_init.csv` "Frida GREEN
10/10". That evidence is structurally blind to the defect: `arg_type thread_desc_init`
allocates a 20-byte scratch buffer, calls, and fingerprints the **five written fields** — it
never reads the return value, and `hooks_registry.py` declares `'ret': 'void'`. The row's
evidence should be cleared and re-earned, exactly as `0x00430760` / `0x004c1a00` /
`0x004148b0` were earlier this session.

Better still, `arg_type thread_desc_init` should fold the return value into its fingerprint,
so this class cannot pass again on this handler. That is an edit to `diff_template.js`.

### Wedge-rate result

`catch_wedge.py --attempts 8` after the fix: **8/8 completed, no wedge.** With the two census
runs that is **10/10**. Today's pre-fix hooked rate was 3 wedges in 8 runs (hunts caught one
on attempts 3, 2 and 3 respectively).

- Fisher exact, one-tailed, 3/8 vs 0/10: **p = 0.069** — suggestive, short of 0.05.
- P(0 wedges in 10 runs | pre-fix rate 3/8) = **0.009**.

**The statistics are no longer the load-bearing evidence, and should not be quoted as if they
were.** The prior session's mistake was resting a fix claim on a rate comparison; here the
rate is corroboration for a mechanism that was measured directly: the emitted `EAX`, the
entry counts matching stock 1/1/1, the game `CreateThread` reappearing, and the lock's
acquire/release balance returning to exactly 0. The defect is fixed as a matter of observed
behaviour, not inference from completions.

**What 10/10 does NOT establish:** that no other wedge source exists. It bounds the residual
rate loosely (95% upper bound ≈ 26% per run), so a rarer second cause would survive this
test. U-9025's *mechanism* is resolved; keep the harness for the next unexplained hang.
