# C4 evidence — 0x00482900 ReplayGetSize (orch-iter21, 2026-07-31)

## What iter20 refused, and why it was right to

iter20 measured `0x00482900` firing 13x per canonical race and nearly called it C4.
An install probe then reported `armed[orig]`. Root cause found this session and it is
NOT what iter20 hypothesised (arming order): `scenario_launch.py:623` sets
`MASHED_RE_NO_AUTO_HOOK=1` whenever `--hooks` is empty. Every one of those races ran the
**stock original with no hooks installed at all**. The 13 entries were entries into the
original function, exactly as the refusal said.

A second, independent defect masked the fix: `Module.findExportByName(name, sym)` — the
STATIC form — was removed in Frida 17. It threw `TypeError`, the surrounding `catch`
swallowed it to `null`, and that is indistinguishable from "the .asi is not loaded yet".
Two boots were spent on a load-order theory before a module enumeration showed
`mashed_re_dev.asi` present the whole time. Both fixed in `scenario_launch.py`.

## The measurement

Canonical race, `--track 0 --cars 6 --hold 25 --hooks 0x00482900`, counters on the
**.asi exports** (reachable only through the installed inline JMP):

| run | asi:ReplayGetSize | asi:ReplayRecordFrame (control) |
|-----|-------------------|---------------------------------|
| 1   | 13                | 0                               |
| 2   | 13                | 0                               |

`ReplayRecordFrame` is the non-degeneracy control: its export resolves and its counter
arms (`armed@0x739cce40`), but it is not in `MASHED_HOOK_ONLY`, so no JMP routes to it.
0 there while 13 lands on the row under test rules out "the counter counts anything".
Race completed normally both runs (`spawn fired: 12`, matching the stock-original run).

## Install state + returns (path2, same build)

`log/verify_hook_install_replay_get_size.txt`, copied to `path2_install.txt`. Zero FAILs:
site holds `E9 7b c9 58 73` → `0x73a0f280` (the reimpl), reimpl interceptor fired 2/2, and
the returns match the registry's asserted non-degeneracy spec:

- `(1.0, 1)` -> 2572
- `(2.5, 4)` -> 1744  — the discriminator: 37.5 TRUNCATES to 37; a round-to-nearest port
  returns 1780 and fails only here.

## Scope of the claim

This establishes the port executes in the canonical scenario with the JMP live, and
returns the specified values through that JMP. It does not re-derive the C2/C3
transcription evidence, which stands unchanged.
