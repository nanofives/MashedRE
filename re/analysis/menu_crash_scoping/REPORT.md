# Runtime self-exit scoping — it is a CRASH, not a timeout/clean-exit

Session date: 2026-05-26/27. Purpose: diagnose the ~60-75s "self-exit" that gates
the full-race C4 fire-counting observation. Mandate was scope-and-stop (no fix).

## Verdict

The self-exit is a **genuine access violation (0xC0000005)**, NOT a clean quit, idle
timeout, or watchdog. It reproduces with our hooks **disabled** (different faulting
site), so it is **not caused by hook installation** — the ~94 s instability is upstream
of our patcher. With hooks ON the fault is **`strlen(NULL)`** reached through the
RenderWare "strlen-wrapper" vtable slot invoked by our (faithful) `FontText_UTF16WidenCopy`
hook; with hooks OFF the same ~94 s state surfaces as a null/wild-pointer call instead.
Root cause = a **NULL/corrupt pointer state that appears at ~94 s**; the hooks only
relocate where it is first dereferenced. See "Frida caller-recovery" below.

## Evidence (3 plain launches, no Frida; exit code + Windows WER event log)

Launched via `Start-Process MASHED.exe -WorkingDirectory original -PassThru`, polled
window-truth on a timeline, captured `Process.ExitCode` on exit. Tool:
`scripts/observe_runtime.ps1` (repeatable). WER = Application-Error event log
(`Get-WinEvent ... ProviderName='Application Error'`).

| Run | Hooks (`MASHED_RE_NO_AUTO_HOOK`) | Crash t | ExitCode | WER faulting module | WER offset | Faulting EIP |
|-----|----------------------------------|---------|----------|---------------------|-----------|--------------|
| 1   | ON  (unset)  | 93.6 s | 0xC0000005 | `MASHED.exe`  | 0x000a9440 | 0x004a9440 |
| 2   | OFF (=1)     | 95.9 s | 0xC0000005 | `unknown`     | 0x00000000 | 0x00000000 |
| 3   | ON  (unset)  | 94.7 s | 0xC0000005 | `MASHED.exe`  | 0x000a9440 | 0x004a9440 |

PE ImageBase = 0x00400000 (verified from the optional header), so WER offset
0xa9440 → EIP 0x004a9440.

## What the faulting EIP is

`0x004a9440` is inside **`_strlen`** (MSVC VS2003 CRT, function entry 0x004a9410,
size 139 B). Per the existing analysis plate `re/analysis/boot_crt_env/004a9410.md`:

> Dword-at-a-time loop (LAB_004a9440, 0x004a9440): At 0x004a9440: `MOV EAX, dword ptr [ECX]` — load 4 bytes.

So the hooks-ON crash is a **read access violation while `strlen` scans for the NUL
terminator** — i.e., MASHED handed `strlen` a `char*` (ECX) that walks into unmapped
memory (bad/garbage pointer, or a string with no NUL inside its committed page).
`_strlen` itself is correct CRT code; the bad pointer originates upstream in the caller.

The hooks-OFF crash faults at **EIP 0x00000000** (WER module `unknown`, offset 0) —
a transfer of control to a null/wild pointer. Different faulting *instruction*, same
exception and same ~94 s timing.

## Determinism

- Faulting site is **stable per configuration**: both hooks-ON runs (1 and 3) fault at
  the identical `_strlen` instruction 0x004a9440.
- It **differs between configs**: hooks-ON → `_strlen` read-AV; hooks-OFF → null-exec AV.
- Crash *timing* is **config-independent**: 93.6 / 95.9 / 94.7 s across both configs.

Most parsimonious reading (marked inference, not a decompilation fact): a single
underlying defect terminates the process at ~94 s; toggling the 250 inline-JMP hooks
changes process memory layout/contents, which moves *where* the wild pointer first
gets dereferenced (a string ptr vs a code ptr) without changing *that* it crashes or
*when*. This is the classic signature of upstream memory corruption / use of
uninitialised-or-freed state. [UNCERTAIN] — not proven; alternative is two independent
bugs that coincidentally both fire at ~94 s.

## Timer vs stage-transition (rules out a fixed watchdog)

All 3 runs above had **full-size intros** (intro-skip OFF), so they all reach the
intro→menu region at ~the same wall-time → can't distinguish a launch-relative timer
from a transition crash *from these 3 runs alone*.

Prior-session observation (memory `project_runtime_blocked.md`, 2026-05-26): with
**intro-skip ON** (5 empty MPGs) MASHED dies **before t=15 s** — much earlier, tracking
the (now near-zero) intro length. That death-time-scales-with-intro-length behaviour
argues **against** a fixed wall-clock watchdog/idle-timeout (H1) and **for** a crash at
the intro→menu stage transition (H2). [UNCERTAIN] — could NOT independently re-confirm
this session: the empty-video resource `re/prior_art/MashedRunner/tmp/SciLorsEmptyVideo.mpg`
is missing, so `scripts/patch_mashed_skip_intro.py` cannot run without first regenerating it.

## Frida caller-recovery (the real AV + caller chain)

`re/frida/poll_attach_catch_crash.py` (exception handler + ESP stack-dump + module
table), attached by PID to a normal launch. Raw capture:
`re/analysis/menu_crash_scoping/crash_eip_real_av.json`.

NOTE: MASHED raises `OutputDebugStringA` ("Replay size is N", N growing) **every frame**
— a shipped debug-build leftover in MASHED.exe itself (our source contains no
`OutputDebugString`; verified). Under a debugger this is `DBG_PRINTEXCEPTION_C`
(0x40010006, Frida type `system`) and floods the handler. The catcher was fixed this
session to ignore non-fatal types and only report real faults. It then caught the AV:

```
type        access-violation         eip 0x004a9440  (= _strlen; bytes 8b 01 = MOV EAX,[ECX])
ecx         0x0                       mem_address 0x0   mem_op read     → strlen(NULL)
stack[0]    0x717580d4                → return addr INSIDE mashed_re_dev.asi (base 0x71750000 → asi+0x80d4)
stack[+16]  0x00428349                → MASHED.exe return address (one frame up the chain)
```

Caller chain (resolved from the on-disk PE, no guessing):
- `0x00428344` in MASHED.exe is `E8 f7 f4 ff ff` = `CALL rel32` → **target 0x00427840**
  (return addr 0x00428349 = the stack[+16] value).
- `0x00427840` = **`FontText_UTF16WidenCopy`**, an ACTIVE hook
  (`mashedmod/src/mashed_re/HUD/HudBatch.cpp:642`). Its reimpl does
  `call [*(0x007d3ff8) + 0xf4]` to get the string length.
- `*(rw_base + 0xf4)` is documented in our own source as the RenderWare **"strlen
  wrapper"** (`Render/TextureLoaderCluster.cpp:205`, cited 0x004c5af6). That wrapper
  calls the original `_strlen` (0x004a9410) — whose return address is the `.asi`
  value at stack[0].

So a **NULL string/charset text pointer** reaches `strlen` via the RW strlen-wrapper,
invoked from our `FontText_UTF16WidenCopy` hook. Our reimpl mirrors the original's
vtable call faithfully (the original 0x00427840 would issue the same call) — the bug is
the **NULL string in the RW object at ~94 s**, not the reimpl's structure. This is
consistent with the hooks-OFF run dying at a different null-pointer site at the same time.

[UNCERTAIN] mechanism that produces the NULL at ~94 s. One lead: the every-frame
"Replay size is N" grows monotonically (frame loop is recording replay even at this
stage); a fixed-cap replay buffer overflowing could corrupt adjacent state. NOT proven —
needs a watchpoint / buffer-cap check before any claim.

## Screenshot status (goal: verified main-menu shot) — NOT achieved

- `empire_splash_t007.png` (this dir; copied from `verify/scene_t007.png`, run 3) shows
  the **Empire Interactive splash (`empire.mpg` intro)** rendering in the MASHED window →
  renderer + intro pipeline work; the window is live, not frozen-black. (`verify/` is
  overwritten each `observe_runtime.ps1` run; this copy is the frozen evidence.)
- Later shots (t≈90 s) captured an **occluding terminal**, not MASHED: GDI
  `CopyFromScreen` grabs whatever is top-of-Z-order at the window rect, and MASHED was
  not foreground. No menu content was ever visually confirmed.
- A clean main-menu screenshot is **blocked** on this build: the menu (if reached) only
  appears ~90 s in, ~4 s before the crash, and is occluded. Foregrounding MASHED to
  de-occlude risks the documented focus-restore-during-intro freeze.

## Bottom line for C4

- C4 is UNBLOCKED at the loader+hook level (live inline-JMP confirmed last session).
- C4 full-race fire-counting is **still gated**: the menu/intro scene crashes (AV) at
  ~94 s, so there is no sustained stable observation window, and the crash is **not**
  something we can clear by disabling/bisecting our hooks (it reproduces hooks-off).

## Recommended next steps (NOT executed — for user decision; involves a fix or a
## control that touches `original/`)

1. **No-`.asi` control** (decisive: stock-MASHED vs our-DLL-present). Temporarily
   rename `original/mashed_re_dev.asi` aside (it is OUR build artifact, restorable from
   `mashedmod/build/`), launch, and check whether the ~94 s AV persists with NO loaded
   DLL at all. This is the one remaining question the env-var A/B can't answer (the
   `.asi` is still mapped, just unhooked, in run 2). NOTE: touches `original/`, so it is
   a stop-and-ask item per project rules — not done here.
2. **DONE this session — strlen caller recovered** (see "Frida caller-recovery"): the
   NULL string reaches `strlen` via the RW strlen-wrapper invoked by `FontText_UTF16WidenCopy`
   (0x00427840). Next, find *what nulls the RW charset/string object at ~94 s*: set a
   write-watchpoint on the `[rw_base+0xf4]`-target's string field, and check the
   "Replay size is N" path (`ReplayRecordFrame` 0x00411600 etc.) for a fixed-cap buffer
   that overflows ~94 s. Trace the value of `*(0x007d3ff8)` and its `+0xf4` string field
   over time.
3. **Regenerate the empty-MPG resource** and re-run intro-skip to lock down H1-vs-H2.
4. If the no-`.asi` control still crashes → this is a stock-MASHED-on-Win11 defect;
   investigate what populates/clears the RW charset object, and the audio-COM path we
   NOP'd, as candidates. Whether the original 0x00427840 also `strlen(NULL)`-crashes given
   the same state (i.e. is this purely a state bug?) is answerable by the no-`.asi` control.
