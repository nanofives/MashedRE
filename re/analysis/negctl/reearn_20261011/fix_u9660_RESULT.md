# negctl re-earn fix: U-9660, 0x0042ac90 MenuEntryGet (`menu_entry_get`)

**Verdict: GREEN**

Branch `fix/u9660`. Built with `MASHED_NO_DEPLOY=1`. The port body (`Frontend/MenuNav.cpp:81`) was not changed.
path2 (`run_verify_hook.py`) was not run, and no tracker was edited. Both are left for the parent.

## What the old fixture got wrong

The registry entry (`hooks_registry.py:1757`) ran `arg_type 'none'`: ten bare calls at the quiescent main menu,
with nothing seeded. The body dereferences a table pointer that nothing sets at the main menu
(disasm of `original/MASHED.exe.unpatched`):

| RVA | Instruction | Read |
|---|---|---|
| `0x0042ac90` | `mov ecx,[0x67e9f8]` / `shl ecx,6` | slot index, times 0x40 |
| `0x0042ac99` | `mov edx,[ecx+0x67ed38]` | table pointer of that slot |
| `0x0042ac9f` | `mov ecx,[ecx+0x67ed40]` | cursor of that slot |
| `0x0042acb0`, `0x0042acc0`, `0x0042acd2`, `0x0042acf0`, `0x0042acfd`, `0x0042ad0a` | `[edx+eax*4]`, `[edx+eax*4+4]` | table words |

With slot 0's pointer at 0, the first table read is `[0 + eax*4]`, so both arms AV at 0x0 on every vector. This
matches the Phase A1 audit comment at `MenuNav.cpp:123`. These are the only memory reads. The body writes nothing,
calls nothing, and keeps ESI (`0x0042acad` push / `0x0042acd1` pop).

Control flow, used to predict each vector:
- cursor `-1` (`0x0042aca7` `cmp ecx,-1` / `je 0x0042acd2`) skips the item walk.
- Otherwise there are `cursor+1` passes (`0x0042acac` inc, `0x0042acce` dec / `jne`). Each pass checks the current
  word for `0xff040000` (`0x0042acb0`), then scans forward (`0x0042acc0..0x0042accb`), then steps past it
  (`0x0042accd`).
- The final scan stops on `0xff050000`, `0xff140000` or `0xff060000` (`0x0042acd5`, `0x0042ace0`, `0x0042ace8`,
  loop test `0x0042acf5`). It does not stop on `0xff040000`.
- If the stop word is `0xff060000`, the function returns `0xffffffff` (`0x0042acfd`, `0x0042ad06`). Otherwise it
  returns the next word (`0x0042ad0a`), even when that word is a sentinel.

## Fixture

`re/frida/negctl_fixtures/z_fix_u9660.py`. It overrides `menu_entry_get` with `scalars_to_scattered_globals`,
`fold_ret`, empty `observe` (the body writes nothing), `freeze_main` (the menu code reads and writes
`0x0067e9f8` / `0x0067ed38..` itself), and `scratch_size 0x400` filled with `0xA5`, which matches no sentinel. Each
vector seeds the slot index, points `0x0067ed38 + slot*0x40` at a scratch word array, seeds `0x0067ed40 + slot*0x40`,
and writes the table words. Vectors 13 to 15 make slots 1, 2 or 3 active and seed the other three slots with a decoy
table that has different action codes, so a port reading the wrong slot reads a different value.

16 vectors cover these paths:
- cursor -1 with a stop on each of the three final-scan sentinels
- the final scan passing over `0xff040000`
- a returned word that is itself `0xff060000`
- items 0, 1 and 2 of a three-item table, with stops on T5, T14 and T6
- a T5 placed before the first item tag
- T4 at word 0 (the check at `0x0042acb0` before the loop), and two adjacent T4s
- T6 at word 0
- action code 0
- three active slots other than 0

## Evidence

- `run_diff menu_entry_get`: **GREEN 16/16**, no errors on either arm, 12 distinct fingerprints.
  `evidence/fix_diff_menu_entry_get.csv`
- `run_diff menu_entry_get --self-baseline`: **GREEN 16/16**. `evidence/fix_diff_menu_entry_get_selfbaseline.csv`
- The 16 fingerprints are FNV-1a over the return value. Each one equals the fingerprint of the value predicted from
  the disasm above, 16/16 (checked offline). This includes the collisions 2/7/11 (`0xffffffff`), 5/14
  (`0xAAAA0001`) and 6/13 (`0xAAAA0002`).
- negctl (`evidence/fix_negctl_u9660.tsv`, run `runs/fix_u9660/negctl_20261012_121831.tsv`, handler
  `scalars_to_scattered_globals`, slots 1):

  | Run | Verdict |
  |---|---|
  | real | GREEN 0/16 |
  | baseline | GREEN 0/16 |
  | null | RED 15/16 (idx 12 returns 0, so null is blind there by construction) |
  | ret | RED 16/16 |
  | field_first | NA |
  | field_last | NA |
  | field_none | CTRL-GREEN 0/16 |

  The field classes are NA because the body writes no byte, so there was nothing to flip.
- No port defect was found, so there is no witness.

## Callers and callees

- Callers: one direct call, at `0x0043e4ca` (`call 0x42ac90`). By nearest hooks.csv start, it is inside
  `0x0043dfd0` FUN_0043dfd0, which is **C2**. This attribution comes from the nearest start, not from a Ghidra
  function boundary. A scan of every section for the absolute dword `0x0042ac90` found nothing.
- Callees: none. The function is a leaf.

## For the parent: install is MASS-DISABLED

`MenuNav.cpp:122`:
`// MASS-DISABLED 2026-05-24 needs-canonical-menu-ptr-table: RH_ScopedInstall(MenuEntryGet, 0x0042ac90);`.
The reason given was the missing seeded table. This fixture now supplies one. path2 cannot show a live JMP unless the
install is re-enabled. Re-enabling it is a parent decision (memory `stale-c3-body-behind-disabled-install`).
`path2_tests` in the fixture are vectors 5, 13 and 7.

## Needs a new U-row: the standalone runs a different body for this RVA

`MenuNav.cpp` (`MenuEntryGet`, the body measured here) is in `asi_sources.rsp:56` and `exe_sources.rsp:106`. The
standalone's menu select path calls `ItemActionCode(s.desc_table, s.cursor)` instead (`MenuNavSM.cpp:432`, call site
`:1298`, exe-only via `exe_sources.rsp:107`), and its comment names FUN_0042ac90. It is a different algorithm from
the bytes:

- It returns `0xffffffff` for `item_index < 0`. The original with cursor -1 runs the final scan from word 0, so
  vectors 0, 1, 3, 4 and 15 return a value.
- Inside the target item it steps two words per non-key pair (`idx += 2`). The original scans word by word. For
  example, `[0xff040000, 0x100, 0x33, 0xff050000, 0xAA]` with cursor 0 returns `0xAA` in the original
  (`0x0042acf0..0x0042acfb`). ItemActionCode lands on `0x33`, then `0xAA`, and never sees `0xff050000`.
- It stops on `0xff070000` (`kTermVal`, `MenuNavSM.cpp:344`). The original has no such test.

`[UNCERTAIN]`: whether any real menu descriptor table reaches these differences (a cursor of -1 at select, or an
odd-length run inside an item) is not established. That needs a dump of the live tables at `0x0067ed38 + slot*0x40`
per screen. No run_diff measures ItemActionCode, because it is not exported from the .asi. This is the
`duplicate-rva-implementations-drift` pattern. Proposed U-row: "standalone ItemActionCode diverges from the bytes of
0x0042ac90. Route the standalone through MenuEntryGet, or prove that the real tables never reach the differences."

## Summary

U-9660 was a fixture defect, not a port defect. 0x0042ac90 reads its table through `[0x67e9f8]` → `[slot*0x40 +
0x67ed38]`. At the quiescent main menu that pointer is 0, so the old `arg_type 'none'` vectors AV'd identically on
both arms. The new `z_fix_u9660.py` seeds the slot index, the per-slot table pointer (into scratch), the cursor and
the word table, with decoy tables in the inactive slots. Both arms now reach the real body on 16 vectors: run_diff
GREEN 16/16, self-baseline GREEN 16/16, and every return equals the value predicted from the disassembly. negctl:
null RED 15/16, ret RED 16/16, field_none CTRL-GREEN, field classes NA (the body writes nothing). The row's C3
evidence is re-measured and the port is unchanged. Left for the parent:
- the install at `MenuNav.cpp:122` is still MASS-DISABLED, so path2 needs it re-enabled;
- the standalone's `ItemActionCode` (`MenuNavSM.cpp:432`) is a second, divergent body for this RVA and needs its own
  U-row;
- the only caller, `0x0043e4ca` in FUN_0043dfd0, is C2.
