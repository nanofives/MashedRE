# HB-MOVE-CONTINUATION-MISS: RESULT (run 44, 2026-11-13)

Verdict: **GREEN** for the fix and its test. Live effect needs a daemon restart (see "Deploy").

## Root cause

The failing move was **not a move**. Run 43's final train `cmwf46yte0wbnlh1nxbgjlp9i` was asked for on claude2
but started on **claude3**: claude2 was at 100% and the daemon's spawn-time usage switch picked claude3 instead.
The train then called `move_self account=claude3` (its brief said it was on claude2). The CLI parked the move
and returned "Scheduled ... your continuation message is delivered after the move". At turn end the relocation
watcher saw that the target was the license the session already ran on, and **dropped the parked move,
continuation included, with no log line, no message and no daemon call**. Nothing ever told the session again
until L0 resent the task.

Code (Happy `C:\Happy\Terminal` main @ `1ff07865`, `packages/happy-cli/src/claude/utils/relocation.ts`):

```
196        if (!move || move.targetProfileId === metadata?.profileId) {
197            pendingSelfMove = null;
198            return;
199        }
```

`move_self` (`startHappyServer.ts:593-601`) never compares the target with the current license, so it reports
"Scheduled" for a same-license move.

## Evidence

Logs on LIVING-PC, Happy home `C:\Happy\Terminal\environments\data\envs\windows-dev\cli\home\logs`
(times local, UTC-3).

Daemon log `2026-11-12-12-42-03-pid-16584-daemon.log`:

- `[12:11:20.535] [CONTROL SERVER] Spawn session request: ... profileId=claude2`
- `[12:11:20.928] [spawnSession] 'claude2' is at 100%, folder switches at 95% (set on C:\): starting on 'claude3' (21%)`
- `[12:11:21.094] [DAEMON RUN] Pre-created session cmwf46yte0wbnlh1nxbgjlp9i` (PID 18312)
- No `Relocate self: cmwf46yte0wbnlh1nxbgjlp9i` line anywhere. PID 18312 keeps reporting the same session until
  `[12:19:04.334]` (L0's resend) and is only stopped at `[12:55:08.852]`. The daemon was never asked to move it.

Session log `2026-11-13-12-11-21-pid-18312.log`:

- `[12:11:22.148] [ClaudeSettings] Successfully read Claude settings from C:\Users\maria\.claude-account3\settings.json`
  (the session runs on claude3 from its first second)
- `[12:11:46.951] [relocation] self-move to claude3 scheduled (move_self)`
- `[12:11:51.191] [claudeRemote] Result received` (turn ends)
- Next line of any kind: `[12:19:04.244] [SOCKET] [UPDATE] Received update` (L0's resend). Between them nothing from
  `[relocation]`, which is what line 196 does: it drops the move and logs nothing.

Working cases, for comparison. All are real cross-license moves:

| Case | Spawned on | move_self target | Daemon line | Continuation |
|---|---|---|---|---|
| run 43 first train `cmwf0q5uo0gvhlh1n4vznf56n` | claude2 (spawn 10:34:17, no usage switch line) | claude3 | `[10:34:44.554] Relocate self: ... -> claude3`, resume PID 5896 at 10:34:45.170 | arrived |
| this worker `cmwfd5n1q27gclh1n5rxpvzy7` (positive control, daemon `2026-11-13-13-24-40-pid-13788-daemon.log`) | claude2 | claude3 | `[16:22:57.272] Relocate self: ... -> claude3`, resumed PID 16312 at 16:22:57.859 | arrived (my first tool call after it ran at 19:23:13Z) |
| run 43 final train `cmwf46yte0wbnlh1nxbgjlp9i` | **claude3** (usage switch) | claude3 | none | **dropped** |

Parent/child relation, mid-turn state and the 5 s after the move do not differ between them. Whether the spawn was
switched does: the same daemon switched three later claude2 spawns to claude3 that day (12:44:53, 13:14:14,
13:23:10 in the daemon log), and any of them that `move_self`s to claude3 hits the same drop.
HB-MOVE-CONTINUATION (8d8b6a19 / 61956bef) fixed the daemon resume path, which this case never reaches.

## Fix

Terminal branch **`fix/move-continuation-same-license`** (worktree `C:\Happy\wt-move-continuation-miss`, from main
`1ff07865`), commit **`7e68acc43dad29e170bd3c7b55dee1bceb32aecb`**. Not pushed, not merged.

`relocation.ts`: when a parked self-move targets the license the session already runs on and carries a
continuation, the watcher now delivers the continuation in place. It posts it to this same session through the
server (`sendToSession`, `source: 'happy-move'`, the same route the usage-limit "continue" uses in
`scheduleLimitWait`), with move_self's model and effort as per-message overrides. It also shows the user
"This conversation already runs on <license>, so nothing moved. Continuing here." It delivers once, only after the
turn ends (same idle gate as a real move). A same-license move without a continuation still resumes idle, and
cross-license moves are unchanged.

Test: `packages/happy-cli/src/claude/utils/relocation.sameLicense.test.ts` (5 cases: delivered in place with
model/effort, delivered once, waits for the turn to end, nothing for no continuation, cross-license still goes to
the daemon).

Test run (LIVING-PC, `test-slot npx vitest run --project unit src/claude/utils/relocation.sameLicense.test.ts src/claude/utils/relocation.test.ts`
in `packages/happy-cli`, `PROYECTOS_TEST_HUB=off`). The vitest global setup ran `tsc --noEmit && pkgroll` first, so the
package typechecks and builds:

```
 OK unit src/claude/utils/relocation.test.ts (22 tests) 9ms
 OK unit src/claude/utils/relocation.sameLicense.test.ts (5 tests) 82ms
 Test Files  2 passed (2)
      Tests  27 passed (27)
```

## Deploy

The fix lives in the happy-cli bundle that each conversation process runs. It is not daemon code. New and resumed
conversations pick it up only from a new bundle (`.happy-bundles/...`), and the daemon pins the bundle it spawns
with (`HAPPY_BUNDLE_PINNED=1`), so in practice it **needs a bundle rebuild plus a daemon restart**. Not done here,
per the brief. Decision for the lead: merge `fix/move-continuation-same-license` to Terminal main and restart the
daemon at a quiet moment.

Workaround until then: a child whose brief says "move_self to claude3 first" should check its account before
moving. Its injected memory path ends in `-account3`, or `HAPPY_PROFILE_ID` is set in its env (it was `claude3` in
this worker after the move). If it already runs on the target, it should skip the move and just work. Otherwise
the lead keeps the 5-minute pickup check and resend.
