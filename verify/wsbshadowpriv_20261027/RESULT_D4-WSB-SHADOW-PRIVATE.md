# RESULT D4-WSB-SHADOW-PRIVATE: ShadowTrack private mode (MASHED_SHADOW_PRIVATE) fixed

Verdict: **GREEN.** The freeze is fixed. The heap-tracked write-set leg (PRIVATE + STW) runs 24/24 on all 4 rows, and
3 of the 4 rows now compare heap pages in every sample. `0x00564040` compares 0 pages because its sampled calls write
nothing (2 more arms, section 3). D4-WSB-L38-DEMOTE merged into `orch/integration` (`73bb3ad22`) during this step, so
per the PREREG rule **3 rows are re-promoted C3 -> C4** through re-classify (`0x0055fea0`, `0x0055ff90`, `0x00565260`).
**`0x00564040` stays C3**, and U-9792 stays open for it only. Default gates unchanged on this branch's exe.

Branch `orch/D4-WSB-SHADOW-PRIVATE` off `orch/integration` @ `2d75d00d1`. PREREG `PREREG_D4-WSB-SHADOW-PRIVATE.md`
(`de4ef0d2c`, committed alone), addendum 1 (`b52b7a60b`), addendum 2 (`8663fd0e9`), each committed before the arms it names.
`orch/integration` @ `84a47e645` (contains the L38-DEMOTE merge) merged in at `5f29aa5ae`. That merge touches trackers and
docs only, no source, so the exe the gates ran on is the exe of this branch.

## Per-row page counts (the deliverable)

Heap-tracked leg, `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, census scenario
(`--track 0 --mode 10 --cars 4 --hold 60 --hooks 0x<rva>`), private root, asi `b0ab9507...`. Every sample
`stack=0 noise=0 ovf=0 ret=0`, no NOISY / UNTRACKED / OTHER-EXC line, `PATCHBYTE ... A/B-IS-REAL` on all 4.

| row | n | ndiff | pages o / n (min..max per sample) | heap pages o / n (min..max) | heap pages compared, total over 24 | before (BVH-WRITESET) | PREREG read-out |
|---|---|---|---|---|---|---|---|
| `0x0055fea0` | 24 | 0 | 1..1 / 1..1 | 1..1 / 1..1 | 24 | `o:0,n:0` | **HEAP-CLEAN** |
| `0x0055ff90` | 24 | 0 | 4..4 / 4..4 | 4..4 / 4..4 | 96 | `o:0,n:0` | **HEAP-CLEAN** |
| `0x00565260` | 24 | 0 | 2..4 / 2..4 | 2..4 / 2..4 | 52 | `o:0,n:0` (20 samples) | **HEAP-CLEAN** |
| `0x00564040` | 24 | 0 | 0..0 / 0..0 | 0..0 / 0..0 | 0 | `o:0,n:0` | CLEAN-NO-HEAP, then NO-WRITES-SAMPLED (section 3) |

`heap=` counts tracked pages outside MASHED.exe's image. Every tracked page in these samples is a heap page (pages = heap).

## 1. Root cause of the freeze

Diagnostic boot diag1 (pre-PREREG, unfixed build, `MASHED_SHADOW_TRACE=1`, PRIVATE + STW, `0x0055fea0`):
heaps locked 7/7, 41 threads, 23 heap ranges, 72 regions protected (`raw stage=protected a=00000048`), then **0 faults
reached `Handler`** and the watchdog saw the tracked thread at EIP `0x77b0a08c`, code `c2 08 00` (`ret 8`, a 32-bit
ntdll syscall stub return), stack `[0]=0x77b200db` (return into ntdll), args `0x77bcaa54, 0`, then `0x77bce3c8`.

diag2 (fixed build) logged the VEH node's first dwords: `0x00cc74f0 / 0x77bce3cc / 0x103323a0`. Dword 1 (Blink) is
the list head in ntdll's image at `0x77bce3cc`, so the list's lock word is `0x77bce3c8`, the value on the diag1 hang
stack. Dword 2 (`entry+8` on x86) is a pointer into a separate private block. `RtlpCallVectoredHandlers` writes that
block while it holds the list lock, before it calls the handler.

Mechanism, as the evidence shows it: the refcount block sat on a heap page that private mode had made read-only, the
write faulted inside the dispatcher with the VEH list lock held, the nested dispatch waited on that lock, and the
thread waited on itself. The old code excluded only the VirtualQuery region holding the node, never the refcount block.
Image-only mode (no PRIVATE) never protects heap pages, which is why S / W legs ran and P / PN did not.
[UNCERTAIN] which object `0x77bcaa54` (the wait address) is. It is 0x3974 bytes below the list lock, and the wait
could be on an address the SRW wait path derives. What is missing: ntdll symbols for this build. The fix does not depend
on it: with the holes, the same boot recipe ran 24/24 (diag2) and 4 more boots ran 24/24 each.

## 2. The fix (`Core/ShadowTrack.h`, armed path only)

- `CollectVehHoles`: walk the VEH list from our node by Flink (stops at the image-resident head, max 16 hops). Each
  entry's page(s), plus the page of every dword in its first 0x20 bytes that points into committed private memory,
  become a page hole. 4 holes in diag2 (`0x04d74000`, `0x00cc7000`, ...).
- `Protect()` protects each wanted region in runs around the holes, not as one block. The old rule that skipped the
  whole region holding the node is gone, so that heap segment is now tracked too (minus the hole pages).
- `RunTracked` sample lines carry `heap=o:<n>,n:<n>` and, for non-void functions, `rv=<return>`.
- Opt-in `MASHED_SHADOW_NEED_PAGES=1` (addenda 1 and 2): a sample where neither run wrote a tracked page and stack and
  return agree is discarded, with the first 64, every rv != 0 (up to 256), and every 250th logged as `[..] EMPTY rv=`. At
  most 2000 discards, then sampling stops. `Counter` gained one field (`empty`, `Core/ShadowAB.h`).
- Image-only mode is unchanged: holes are private pages, so no image region is split.

## 3. `0x00564040`: the sampled calls write nothing

Its only stores go to `param_3` (`RwpSolverLeaves1.cpp:201` and the sort block below it). Both callers pass
`*(param_1 + 0x80)` (`RwpSolverBroadphase3.cpp:182`, `:472`). A call that collects 0 entries writes nothing.
- Arm N (addendum 1, NEED_PAGES): 0 counted samples over the 60 s hold, 64 EMPTY lines all `rv=00000000`, 1 NOISY.
- Arm N2 (addendum 2): 0 counted samples, the running total reached 500 (so 500..749 discards), every logged EMPTY
  `rv=00000000`, **no rv != 0 discard**, 0 NOISY.
Read-out: **NO-WRITES-SAMPLED**. In the census scenario the function returns 0 on every sampled call, so there is no
write for any tracker to see. Not a harness gap: a call with rv > 0 and a buffer outside the tracked heap would have
shown up as an `rv != 0` EMPTY line. Getting the leg for this row needs a scenario where it collects entries (car contacts).

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D4-WSB-SHADOW-PRIVATE_root`,
test slot + `mashed_lock.py run --label D4-WSB-SHADOW-PRIVATE`, exe `283ade55`. All four `exit=0 self_exited=True`.
(The outer shell hit its 30 min background limit while queued for the machine lock. The lock wrapper and runner kept
going and finished all four arms on their own; RUNLOG below.)

| arm | sha8 | ref (`merge_train.json`) |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

## Tracker state

L38-DEMOTE went `merge-queued` -> `merged` during this step (`73bb3ad22`). After merging it in, re-classify ran
(`apply_reclass.py`, line-level edits only):

| row | before | after | C4 gate evidence |
|---|---|---|---|
| `0x0055fea0` | C3 impl | **C4 verified** | this heap leg + racediff C4-CLEAN 6/6 (`verify/wsbbvh_20261026/racediff_summary.tsv`) |
| `0x0055ff90` | C3 impl | **C4 verified** | this heap leg + racediff C4-CLEAN 6/6 (`verify/wsbbvh_20261026/racediff_summary.tsv`) |
| `0x00565260` | C3 impl | **C4 verified** | this heap leg + racediff C4-CLEAN 6/6 (`verify/wsbbvh_20261027/racediff_summary.tsv`) |
| `0x00564040` | C3 impl | C3 impl (note added) | heap leg compared 0 pages |

Both legs ran with the inline JMP live (`installed=e9`). No `STUB` in the three bodies (S-3725 / S-3747 list them as
passthrough callees of `0x0047e9c0` / `0x0055ae70`, not stubs in these bodies). Their notes' `[UNCERTAIN]` markers are
all filed already (U-9089, U-9090, U-9121, U-9122), with no new marker. `scenario` column `shadow_tracked` ->
`shadow_tracked_heap` on the 3 rows. U-9792: resolved for the 3, open for `0x00564040`, blocking only its C4.
CHANGELOG: 4 lines below `<!-- ENTRIES -->`. `scripts/lint_rva_bodies.py`: OK, 0 new.

## Files

Committed (`verify/wsbshadowpriv_20261027/`): `PREREG_D4-WSB-SHADOW-PRIVATE.md`, `PREREG_ADDENDUM_1.md`,
`PREREG_ADDENDUM_2.md`, `run_shadow3.py` (copy of `run_shadow2.py`, data path and lock label changed),
`shadow_summary.tsv`, `apply_reclass.py`, this file. Source: `Core/ShadowTrack.h`, `Core/ShadowAB.h`. Trackers:
`hooks.csv` (4 rows), `UNCERTAINTIES.md` (U-9792), `re/analysis/CHANGELOG.md` (4 lines).

Raw (not committed), `D:/MashedWT/D4-WSB-SHADOW-PRIVATE_data/`:

| file | SHA-256 |
|---|---|
| `0055fea0_diag1/shadow_ab.log` (diag1, hang) | `4da2a0a6510b300bdbf070d07456e106c58caf2d01d5b66ab6d9a5e9b105b025` |
| `0055fea0_diag2/shadow_ab.log` (diag2, fixed) | `0016be96f755b29d45ce72b1c2f314199d6416abd620a7d0df41a39697e7a26e` |
| `0055fea0_heap/shadow_ab.log` | `0fbfe22f868a28c9439b3dbc49cf1a00ead762b96fe03571ce9c8c5d6900d233` |
| `0055ff90_heap/shadow_ab.log` | `7a2f316e881b26a6446fa0f08a79994f6ee9521b598b0e5d2cf72e0ee95bcef9` |
| `00564040_heap/shadow_ab.log` | `4f5110997b223eb52a09e65ec01446971a49e0ba53d3490be9b340c3729843b4` |
| `00565260_heap/shadow_ab.log` | `833b0c0b971f98cdcd55a3872b1f0a488219dc47a02a111104a507feee5b844b` |
| `00564040_needpages/shadow_ab.log` (N) | `c6f1997f36ff4b1dc1eda23617ee386b5a96c6589e046e514b92b78bdf06a3ba` |
| `00564040_needpages2/shadow_ab.log` (N2) | `a7da7950551a6073323a41d1a7d338ace6d97cbc90f663f5f4e4324541461992` |
| `diag1.log` / `diag2.log` / `verdict_batch1.log` / `armN.log` / `armN2.log` | `b8d20885...3483` / `024dfd0b...ec` / `e7688a0e...624` / `0cc152eb...1719` / `d6f0fad3...82b4` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates/RUNLOG.txt` | `ee782d796e2fc142afbd34aa1c03a003fe98642a396bf438f17860fdb982d4cc` |

Builds (all `MASHED_NO_DEPLOY=1`, asi copied to the private root only, `original/mashed_re_dev.asi` untouched):
verdict arms asi `b0ab9507242ea568ed0d1c8bdb0c922782157646598090d730a338adbf04237d`; arm N asi `980da0ad...a0a0`;
final (N2 + gates) `mashed_re.exe` `283ade555ea0528a9e59129e3b6bc780510e08724b1e9f1b25f1ff6e9e7108b3`,
`mashed_re_dev.asi` `34ad54687785e91994ce2ba5fe5e71e563abbe79742a2b03ce89ffcc1d908e5f`, `.map`
`44a47a198bb4820c488cad7dba47e65d1efc6d40ffd43cba5ef69f92ee69f714`. The changes after the verdict build are logging
plus the opt-in discard (default off), so they do not change the verdict arms' compare path.
Game PIDs, all spawned by this step: shadow boots 12768, 16356, 5048, 12204, 16720, 4892, 13592, 13860 (killed by the
driver guards only); gate arms 15372, 7624, 10536, 9804 (self-exited). No process of this step is running.

## Next

- `0x00564040`: the heap-tracked leg needs a scenario with car contacts so the function collects entries.
- Arm P can now run for `0x00564c80`, `0x00561390`, `0x00563e70` (BVH-FU's blocked rows) and `0x00561040`.
