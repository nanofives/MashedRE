# Movie-skip root-cause investigation — 2026-06-22

Full RE of MASHED's boot-movie subsystem, driven by "why can't I skip the intro movies?"
The investigation **overturned the working premise**: the recurring `eip=0` boot/menu crash was
*not* caused by the movie skip at all — it was a calling-convention bug in
`patch_mashed_skip_movies.py`. The movies themselves skip cleanly.

Binary anchor: pristine `MASHED.exe` (SHA-256 `BDCAE093…`), image base `0x400000`,
`.text` maps 1:1 (file off = RVA − 0x400000). All RVAs below cite the pristine binary.

---

## TL;DR

| Symptom | Root cause | Status |
|---|---|---|
| White screen when binary-NOP'ing the movie *start* | `FUN_00494480` is hardcoded `return 1` → intro loop blits the movie texture **unconditionally**; no texture → white | explained; that approach abandoned |
| Deterministic `eip=0` crash (`esp=0x1afe50`, `ecx=0xff000000`) a few seconds into the menu | `patch_mashed_skip_movies.py` deleted a `push 0` that is **not** an argument — it's a deferred-cleanup stack slot → 4-byte stack imbalance | **FIXED** (NOP only the `call`, keep `push 0`) |
| Latent COM use-after-free in movie teardown | `FUN_00494320` releases `DAT_00771a10`/`a14` ungated + un-nulled (its 6 siblings are gated) | real bug; fix built (`patch_mashed_fix_movie_uaf.py`), **not** the crash above, not applied |

**Stable intro-skip config (joysticks off):** 7 boot patches + `patch_mashed_skip_intro.py` (1-frame
empties). User-confirmed real menu, stable. `skip-movies` is now safe but redundant with the empties.

---

## Movie subsystem map (boot path)

Boot-init `FUN_00402750` has two movie roots:

```
FUN_00402750:
  0x402838  call FUN_00495350     ; INTRO LOOP (4 logos). MUST keep running — it also pumps
                                  ;   the D3D present/render loop later boot code needs.
  0x40283d  push 0                ; <-- deferred-cleanup stack slot (NOT an arg; see crash)
  0x40283f  call FUN_00494c80     ; small.mpg player (separate DirectShow graph)
  0x402844  push 0x5cc41c ; call FUN_00495280   ; opens font36.piz (normal init resumes)
  ...
  0x402888  add esp, 0x24         ; ONE batched cleanup of the accumulated cdecl args
```

Key functions (decompiled this session):

- **`FUN_00495350`** — intro loop. Plays movie indices `{1,2,3,4}`. Each frame: `FUN_004671a0`
  (2D ctx) → `FUN_00494480` → blit → `FUN_004c1be0` (present) → `Sleep(5)`; advances when
  `FUN_00493f70` (movie-active flag `DAT_00771a04`) clears or a key edge is detected (the intro is
  keypress-skippable). The clear color is `{00,80,80,ff}` (teal), **not** white.
- **`FUN_00494480`** — per-frame "movie ready?" gate. Body `0x494480..0x4944a5`. **Hardcoded
  `return 1`** (ticks the player then unconditionally returns 1). ⇒ the blit is *always* run.
- **`FUN_00494a80`** — movie-start: allocs surface `DAT_00771a18 = FUN_004c77c0(…,0x84)`, builds the
  graph (`FUN_004944c0`), sets `DAT_00771a04 = 1`.
- **`FUN_004944c0`** — builds/runs the indexed-movie DirectShow graph (globals `DAT_00771a24`/`a30`/
  `a34`/`a2c`/`a28`/`a20`).
- **`FUN_00494820`** — TextureRenderer `SetMediaType`: creates the two D3D textures
  `DAT_00771a10`/`a14` via `device->CreateTexture` (vtable+0x5c) **only on successful video-format
  negotiation** (requires format `0x16`/`0x19`, else *"Texture is format we can't handle!"*).
- **`FUN_00494320`** — per-movie COM teardown (see UAF below).
- **`FUN_00494c80`** — small.mpg player. Builds a *second* graph (globals `DAT_00771a38`/`a44`/`a48`/
  `a40`/`a3c`). Takes **no arguments** and ends in a **plain `ret`** (`c3` @ `0x494ed8`) — cdecl/void.

---

## Root cause #1 — white screen (the binary movie-START NOP)

`FUN_00494480` always returns 1, so `FUN_00495350` runs `FUN_00493fd0(… DAT_00771a10 …)` (the
movie-texture blit) every frame regardless of whether a movie is active. NOP'ing `FUN_00494a80`
(the movie-start) leaves the texture uninitialized → the blit draws nothing → white. The render is
hard-wired to the movie texture; you cannot skip the movie-start without gutting the render. This
approach (`patch_mashed_skip_intro_logos.py`) was tried and abandoned.

---

## Root cause #2 — the `eip=0` crash = `skip-movies` stack imbalance (THE big finding)

### The crash
Deterministic across every run (identical registers ⇒ **not** a use-after-free / heap corruption):
```
code=0xC0000005 addr=0  eip=0x00000000  esp=0x001afe50  ebp=0x001aff74  eax=1  ecx=0xff000000
```
Stack at fault: `[esp]=0x4a4d38` (return into CRT startup, right after `call WinMain` @ `0x4a4d33`),
`[esp+4..]` = WinMain's args (`hInstance=0x400000, 0, lpCmdLine=0xbeb556, nShowCmd=1`). I.e. a
corrupted `ret`/transfer to 0 at the WinMain↔CRT boundary — a deterministic stack corruption.

### Isolation experiments (joysticks UNPLUGGED throughout)
| Config | Result |
|---|---|
| clean 7-boot baseline + real movies | **stable** +34s |
| clean 7-boot baseline + empties | **stable** +32s |
| baseline + empties + **force-keyboard** | **stable** +24s |
| baseline + empties + **skip-movies (old 7-NOP)** | **crash** `eip=0` ~12s |
| baseline + **skip-movies** + real movies | **crash** `eip=0` ~24s (same signature) |

⇒ the movies are innocent (crash happens with real movies too, just later — independent of intro
length); **force-keyboard is innocent**; **`skip-movies` is the sole culprit.**

### Mechanism
The original `skip-movies` NOP'd **7 bytes** (`push 0` + `call FUN_00494c80`) at `0x40283d`, on the
assumption (its own comment) that `FUN_00494c80` is `__stdcall` so the `push 0` is its argument,
cleaned by the call. **That is wrong:**

- `FUN_00494c80` ends in a **plain `ret`** (`c3` @ `0x494ed8`) — it's cdecl/void and cleans nothing.
- The `push 0` @ `0x40283d` is **not** its argument. It's a **deferred-cleanup stack slot**: MSVC
  accumulates pushed cdecl args + esp-relative locals and pops them in one batch
  `add esp, 0x24` @ `0x402888` (the code even uses `lea [esp+0x24]`/`[esp+0x2c]` locals just before).
- Deleting the `push 0` shifts `esp` by 4 for every subsequent esp-relative access **and** makes
  `add esp, 0x24` over-pop by 4 → **deterministic 4-byte stack imbalance** → corrupted saved return
  → `eip=0` a few seconds into the menu.

### Fix
NOP **only the 5-byte `call`** at `0x40283f`; **keep the `push 0`**. The deferred cleanup then
balances exactly as in the original; small.mpg's graph is never built. Verified stable: real movies +
fixed skip-movies → menu +32s, no crash; empties + fixed skip-movies → +24s, no crash.
(`scripts/patch_mashed_skip_movies.py`, corrected + auto-repairs the old broken form.)

---

## Root cause #3 — latent COM use-after-free in `FUN_00494320` (real bug, NOT the crash above)

`FUN_00494320` (per-movie teardown) releases six COM objects **gated + nulled**
(`if(x){x=0; x->Release();}`) but releases the two D3D textures `DAT_00771a10`/`a14`
**ungated and without nulling**:
```
004943b0  MOV EAX,[0x771a10]   ; no null-check
004943b5  MOV EDX,[EAX]        ; deref — if stale/freed, garbage vtable
004943b8  CALL [EDX+8]         ; Release → jump to garbage
```
A movie that fails format negotiation (`FUN_00494820`) never recreates the textures, so the next
teardown releases a dangling pointer → UAF. Well-formed movies always renegotiate, so the original
never trips it in normal play. A correct fix (gate both like the siblings, in a code cave) is built
in `scripts/patch_mashed_fix_movie_uaf.py` (cave `0x508c95`), **byte-verified but not applied** —
it is a genuine bugfix, but isolation proved it is **not** the `eip=0` crash (that crash predates the
fix and reproduces without it).

---

## Corrections to prior notes

- `CLAUDE.md` skip-movies entry said *"NOPs 7 B incl. the push 0 since FUN_00494c80 is stdcall"* —
  **wrong** (plain ret / cdecl). Corrected.
- `CLAUDE.md` skip-intro entry said the empties *"ALSO crash the same DirectShow path"* — **wrong**;
  that crash was `skip-movies`'s stack bug applied alongside. The empties are stable. Corrected.
- Memory `project-launch-joystick-video-config` attributed the `eip=0` crash to the joystick
  enumeration path and called `force-keyboard` "the fix" — **wrong** for the joysticks-off case;
  corrected (force-keyboard is innocent; the genuine pad-plugged `~0x495883` deref is a *separate*
  issue, untested this session).

## Tooling produced this session
`scripts/patch_mashed_force_keyboard.py` (joystick-ignore, pad-plugged case), `…fix_movie_uaf.py`
(UAF fix, parked), corrected `…skip_movies.py`; `re/frida/force_keyboard.py`. Failed/abandoned
approaches kept for the record: `…skip_intro_logos.py` (white screen), `…clean_exit.py`,
`…skip_teardown.py` (neither stopped the crash — consistent with the corruption being upstream).
Verification screenshots: `verify/_intro_skip_clean_menu*.png`, `verify/_baseline_realmovies_menu.png`.
