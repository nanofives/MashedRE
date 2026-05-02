# Session Ownership

When multiple Claude sessions / worktrees are active, each owns a set of subsystems. While a session "owns" a subsystem, it is the only one allowed to:
- Promote functions in that subsystem to higher confidence.
- File `STUBS` / `UNCERTAINTIES` rows attributed to that subsystem.
- Edit code under `mashedmod/src/mashed_re/<Subsystem>/`.

Ownership prevents tracker conflicts on `hooks.csv` merges. Other sessions may *read* and *cite* an owned subsystem, but cannot change its rows.

## Current allocation

| Subsystem | Owner (worktree / session) | Branch | Status |
|-----------|----------------------------|--------|--------|
| boot      | main                       | main   | unowned (default; main session takes it on startup) |
| util      | main                       | main   | unowned (default) |
| audio     | (unassigned)               | —      | will become `phase5/audio` worktree when work begins |
| ai        | (unassigned)               | —      | will become `phase5/ai` |
| vehicle   | (unassigned)               | —      | will become `phase5/vehicle` |
| track     | (unassigned)               | —      | will become `phase5/track` |
| render    | (unassigned)               | —      | will become `phase5/render` (last; touches everything) |
| hud       | (unassigned)               | —      | will become `phase5/hud` |
| frontend  | (unassigned)               | —      | will become `phase5/frontend` (recommended first sweep — most isolated) |
| input     | (unassigned)               | —      | will become `phase5/input` |
| save      | (unassigned)               | —      | will become `phase5/save` |
| net       | (unassigned)               | —      | TBD whether Mashed has multiplayer code paths |

## Conventions

- **Default ownership** when no explicit allocation: the **main** worktree owns `boot`, `util`, and any subsystem with zero open functions.
- A worktree claims a subsystem by editing this file in its branch and committing. The merge of that commit into `main` is the formal claim.
- A worktree releases ownership by setting Owner back to `(unassigned)` in the same way.
- Cross-subsystem changes (e.g., a global struct that touches both `vehicle` and `physics`) require coordination: the session making the change updates *every* affected subsystem's row in `hooks.csv` and announces the cross-cut in `re/analysis/CHANGELOG.md`.

## Anti-patterns

- A worktree advancing functions in a subsystem it doesn't own. Refused at merge time.
- Two worktrees claiming the same subsystem. The second to commit must `git pull --rebase` and either coordinate or pick a different subsystem.
- "Temporary" ownership for one-off edits. If you're touching it, claim it; if you're done, release it. No grey zone.
