# RESULT D5-SHIM-DOCS (GREEN)

Text-only. Source: `verify/d5shimconsumers_20261030/RESULT_D5-SHIM-CONSUMERS.md` section 6 items 2-5.
Base: e23f260b7. Branch: orch/D5-SHIM-DOCS.

## Edits (4 files, 6 insertions, 2 deletions)
- `.claude/skills/frida-sweep/SKILL.md:36`: appended "Dev-only and it stays after D5" note to the d3d9 shim precondition.
- `.claude/skills/promote-c3-batch/SKILL.md:381`: appended the same note to the `mashed_re_dev.asi` rebuild bullet.
- `scripts/patch_mashed_fix_camera_res.py`: 2 docstring lines after the COUPLING paragraph (coupling survives D5).
- `scripts/lan_release/stage_lan_release.py`: 2 docstring lines after the embedded-texture paragraph (dev harness stays, never shipped).
Line endings of each touched line kept as found.

## Checks
- `git diff --stat` lists only those 4 files.
- `py -3.12 -m py_compile` passes on both .py files. Changes are inside docstrings only.
- No exe, build input or behaviour touched. Gates not run (no source change).

## Proposed text for files this row may not edit
ROADMAP.md D5 (around lines 3006-3007), replace the dev-harness bullet with:
`- mashed_re.exe runs without the dev d3d9 shim, d3d9_real.dll, the dinput8 ASI loader, or any original/MASHED.exe boot patch. Those are dev harness only: they stay in the repo for the original-side lane (diff-original, parity harness, BBDUMP) and are not shipped.`
and on the "Dev .asi out of the shipping matrix" line add: `(not removed, diff-original loads it)`.

CLAUDE.md "Runtime state", add after item 4 (d3d9 shim): `Dev harness only: items 1, 3 and 4 stay after D5 for the original-side lane and are never shipped. fix_camera_res and the d3d9 shim retire together or not at all.`
