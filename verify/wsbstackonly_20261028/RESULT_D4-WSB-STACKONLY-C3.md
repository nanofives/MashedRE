# RESULT D4-WSB-STACKONLY-C3: GREEN (0x0056b7a0 C3->C4, 0x00568990 DIVERGENT stays C3, 0x0055bae0 / 0x0055c000 stay C3)

Run 20 re-pickup (2026-10-29 UTC), after the run-19 BLOCKED (audio endpoint, section "Run 19" below).
PREREG `PREREG_D4-WSB-STACKONLY-C3.md` committed alone at `fa8ab1fbb`, followed as written. No source change.
Build unchanged from run 19: `mashed_re.exe` `779bc9c3...c447`, `mashed_re_dev.asi` `c6a264a3...3566`
(private root `D:/MashedWT/D4-WSB-STACKONLY-C3_root` only, `original/mashed_re_dev.asi` not touched).

## Per-row outcome

| row | before | after | read-out (PREREG) |
|---|---|---|---|
| `0x0055bae0` | C3 impl | C3 impl | no arm (L-40 a, every return is caller stack, BROAD-WS leg 2). Dated note already on the row, none added. |
| `0x0055c000` | C3 ported | C3 ported | same as above |
| `0x0056b7a0` | C3 impl | **C4 verified** | arm S PASS write-set on Warzone (track 11) + arm R `C4-CLEAN` |
| `0x00568990` | C3 impl | C3 impl | arm H **DIVERGENT** on Warzone, 24 of 24 samples. Arm R not run (PREREG: only rows whose S/H passed). |

## Arm S, `0x0056b7a0` (scenario search, `run_shadow5.py`, one boot per track, hook installed alone)

Env per PREREG: `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1 MASHED_SHADOW_NEED_PAGES=1`,
`HOLD=150`, mode 10, 4 cars, `SAMPLES_DONE=24`. Every boot `PATCHBYTE installed=e9 uninstalled=8b A/B-IS-REAL`.

| track | counted | ndiff | discards (EMPTY rv 0) | heap pages compared | read-out |
|---|---|---|---|---|---|
| 12 City | 0 | 0 | 750..999 | 0 | none, next track |
| 3 Arctic | 0 | 0 | 1500..1749 | 0 | none, next track |
| 11 Warzone | 24 | 0 | 750..999 | 48 (2 per sample, o:2 n:2) | **PASS write-set**, stop |

Warzone samples: `pages=o:2,n:2,diff:0 stack=0 noise=0 ovf=0 ret=0 heap=o:2,n:2 rv=00000001` on all 24,
0 NOISY / UNTRACKED / OTHER-EXC lines (`heap_pages_t11.tsv`). Track 10 (Dump) was not needed. The NEED_PAGES
2000-discard budget was not reached on any track (highest logged mark 1500, Arctic).

## Arm H, `0x00568990` on Warzone (track 11)

`PATCHBYTE installed=e9 uninstalled=83 A/B-IS-REAL`, 24 counted samples, **24 divergent**, heap pages
o:3..9 / n:3..10, total 147 (`heap_pages_t11.tsv`). Original return values (`rv=`) 0, 1, 2, 3. The return
also differs on 4 samples (`ret=1` = `ro.diff(rn)`, `Core/ShadowTrack.h:764`): samples 1, 2, 12, 19, original
rv 1, 3, 2, 2. Discards before/among the counted samples: 1000..1249 (all rv 0). Every page divergence starts on
the same heap page `0x0f50c000`.
Format below is `offset:pre/orig/port` (`Core/ShadowTrack.h:798-801`):

- sample 0: `0ec:00000000/00000000/77ad1ccf`, `0fc:00000000/00000000/00020000`, `16c:00000000/77bce3c8/00000000`,
  `17c:00000000/00566000/ffffffff`.
- samples 1..23: `0ec:77ad1ccf/00000000/77ad1ccf`, `0fc:00020000/00000000/00020000` on every sample, plus on the
  samples with 4 dwords one more pair 0x10 apart at `10c/11c`, `14c/15c` or `16c/17c`:
  `xxc:00000000/77bce3c8/00000000`, `xxc+0x10:{00000000|ffffffff}/00566000/ffffffff`.

So on page `0x0f50c000` the original body stores `00000000` at `+0x0ec` / `+0x0fc` and `77bce3c8` / `00566000`
at a pair 0x10 apart in `+0x10c..+0x17c`, and the port body leaves those dwords at their pre-state or writes
`77ad1ccf` / `00020000` / `00000000` / `ffffffff`. [UNCERTAIN U-9798] What page `0x0f50c000` is (the tracker counts it as
32-bit heap; the stored values have the shape of return addresses and frame markers) and which instruction in
either body writes it are not established: no watch-write run, no Ghidra on this account. The row stays C3 with
the divergence on the row note. Resolution path: `--watch-write` on `0x0f50c0ec` in one boot of each side on
Warzone (memory `find-record-writers-with-watch-write`), then a fix brief if the port is the side that differs.

## Arm R, `0x0056b7a0` (`run_bvh5.py` = copy of RGN `run_bvh6.py`, tool `c4_racediff_v7.py` unchanged)

Quick Battle, A4 window 300..1200, ON boot PID 17812 then OFF boot PID 21072, installed `0xE9` (`installed: true`
= byte compare to `0xe9`, `c4_racediff_v7.py:401`). **C4-CLEAN**: 6 calls each side, 1 signature each side, equal:
`[c55bd80,h0,0,0,Sloc] [c55ae50,h1,h2] eax=0`.

## Default gates on this build

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on `mashed_re.exe` `779bc9c3`, under the
machine lock and a test slot, `A4_ROOT=D:/MashedWT/D4-WSB-STACKONLY-C3_root`. All six equal `merge_train.json`:

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv | `5943c5f1` | `5943c5f1` |
| H_DEF | gates.csv | `364350ae` | `364350ae` |
| H_DEF | ptrace.log | `52052890` | `52052890` |
| B_DEF_K0 | step.csv | `3a03670c` | `3a03670c` |
| B_DEF_K1 | step.csv | `9fcdbd3a` | `9fcdbd3a` |
| B_DEF_K26 | step.csv | `53778664` | `53778664` |

The first gate attempt stopped before any hash with `FileNotFoundError` (the `A4_OUT` folder did not exist), so it
was re-run once with the folder created. No source change on this branch, so the gates only confirm the build.

## Tracker transaction (re-classify)

- `hooks.csv` `0056b7a0`: C3 impl -> **C4 verified**, scenario
  `c4_racediff_bvh:QuickBattle-f300-1200+shadow_tracked_heap:track11-mode10-cars4`, frida_diff
  `verify/wsbstackonly_20261028/racediff_summary.tsv`, dated note. C4 gate: clean diff with the inline JMP live on both
  legs, no stub in the body (`STUBS.md` has no row for it). U-9115 (data, bitmap encoding name only) stays open and does not gate.
- `hooks.csv` `00568990`: stays C3 impl, dated DIVERGENT note, `U-9798` (structural) filed with the page / offsets
  above and a watch-write resolution path.
- `0055bae0`, `0055c000`: unchanged (their L-40 (a) note is already on the rows from BROAD-WS).
- `re/analysis/CHANGELOG.md`: 3 lines below `<!-- ENTRIES -->`. Plate frontmatter not touched (earlier WS-B C4 rows such
  as `0x0055fea0` keep `confidence: C1` there too).

## Harness (committed)

- `drive_so.py`: copy of `verify/wsbsolvrgn_20261028/drive_rgn.py` (runner names, data path, label changed). One
  solo window per row with the lock wait outside every runner timer, `Get-PnpDevice -Class AudioEndpoint -PresentOnly`
  before and after each window, Application-log Event 1000 scan for `0x005baf48` and the U-9677 offsets.
  Every window: endpoint `OK Philips FTV (NVIDIA High Definition Audio)` before and after, crash events none.
- `run_bvh5.py`: copy of `verify/wsbsolvrgn_20261028/run_bvh6.py` (data path, label changed).
- `shadow_summary.tsv`, `heap_pages_t11.tsv`, `racediff_summary.tsv`: scored tables.

## Raw (not committed), `D:/MashedWT/D4-WSB-STACKONLY-C3_data/`

| file | SHA-256 |
|---|---|
| `0056b7a0_t12/shadow_ab.log` | `a08d3efb8140c6ed639fcd5c26f2a590658820835c92b72f8d5364607ed61c3b` |
| `0056b7a0_t3/shadow_ab.log` | `4f57cd5742427de4f25f3d46bf49da4461a29fddbb22dccb4939140179ca2e03` |
| `0056b7a0_t11/shadow_ab.log` | `78200c3e04aa30e62439d9ec06d0197cec333b88348db9a7c9f188243ccdb76e` |
| `00568990_t11/shadow_ab.log` | `2f2d2d0c77e572c98677eb2ae18a10894c5afe1316a9d0d596d80d679de0a41d` |
| `0056b7a0/c4_racediff_samples.json` | `9cb59450f064bba7731575cf50315f1c8e978e6e4ff032d0c2821ce46647adfe` |
| `0056b7a0/c4_racediff_result.json` | `d2a03965ee0fdadd2d0781e81574e335809f568a9923cae6733d902c4510b1b8` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |

Game PIDs spawned and killed by the runners (only these): 6080 (t12), 17292 (t3), 16380 (t11 S), 7868 (t11 H),
17812 / 21072 (arm R), standalone gate PIDs 18604 (first attempt), 6960, 17300, 3536, 3848 (all self-exited). No watcher left running.

## Run 19 (BLOCKED, kept for the record)

Pre-launch check 2026-10-28T19:07Z: `Get-PnpDevice -Class AudioEndpoint -PresentOnly` returned no object; the only
endpoints were 2 x `Philips FTV (NVIDIA High Definition Audio)`, `Present=False`. No original was launched. Build,
private root, PREREG (`fa8ab1fbb`) and the copied tools `run_shadow5.py` / `heap_pages.py` date from that run.
Build log `D:/MashedWT/D4-WSB-STACKONLY-C3_data/build.log` (SHA-256 `0af1900181b36bf6b08e8de859821220b7fc75fba07dae0b7bd8981854319145`).
