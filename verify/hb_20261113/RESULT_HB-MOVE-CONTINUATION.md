# RESULT HB-MOVE-CONTINUATION (run 42)

Verdict: GREEN (text row, no build, no gates).

## Deliverable
`re/orchestrator/WORKER_RULES.md`, Sessions section: new bullet "After move_self". Rule: check `latestSeq` 5 minutes
after the move, resend once with `yolo: true` if it did not climb, then stop_child and respawn.

## Evidence (Terminal repo read-only, C:\Happy\Terminal, happy-cli)
- `src/daemon/relocateSessions.ts:153-162` (performRelocation doc) records this exact incident: run 38, a rebase worker's
  resumed CLI exited with code 1 two seconds in, the move still logged "moved" and nothing resumed it.
- `relocateSessions.ts:240-260`: the daemon already retries the resume up to RESUME_START_RETRIES when the resumed process
  exits before its start report (`awaitStarted`), and `:261-263` posts the message once so it waits on the server for the next
  process. So the early-exit cause is already fixed in Terminal.
- `src/daemon/resumeContinuation.ts:158-174` (relocationMessage): returns null when the caller gave no continuation and the
  old process held no live work, so no message is posted and the session idles by design. A move_self without
  `continuation` can therefore look like a dropped continuation.
- Not verified: whether the daemon running on the PCs had that fix at run 38 time, and the cmwcylbz9 case (yolo resend
  helped), which fits a permission holding prompt rather than a missing message. No runtime reproduction was done.

## Recommendation
Keep the resend rule as the standing backstop. When calling move_self, always pass `continuation` with the task text.
No Terminal edit made.
