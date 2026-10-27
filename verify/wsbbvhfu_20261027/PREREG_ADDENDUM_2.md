# PREREG D4-WSB-BVH-FU, addendum 2 (committed before the arm it names)

Arm P for `0x00564c80` (`MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`) hung twice before its first sample: no
`shadow_ab.log` was ever created, the game process used 0 CPU over 5 s, and the PID was killed by hand (first attempt
game PID 6204 after 14 min, re-run game PID 6400 after 25 min, both spawned by this step). Under addendum 1 that is
n < 20 after one re-run, so **`0x00564c80` stays C3**. Nothing below changes that.

## Added arm (diagnostic only, no verdict)

PN. `MASHED_SHADOW_PRIVATE=1 SHADOW_TAG=privnostw run_shadow2.py 00564c80`, heap pages tracked, other threads left
running. Without STW the heaps are not locked during the heap walk, and another thread's write to a tracked page is
counted as noise and its sample discarded (`ShadowTrack.h`, `RunTracked`). One boot, no re-run.

Read-out, fixed now:
- The hypothesis of addendum 1 (the original's heap writes survive the restore, so the port meets the primitive once
  more) predicts: with heap tracked, the `+8c8` / `+8cc` values of orig and port are equal, and the samples track
  `o >= 2` pages.
- If orig and port still end 1 apart on both counters with heap tracked, the hypothesis is refuted and the row needs
  a port-side look.
- A hang, crash or 0 counted samples = no read-out. The RESULT says so.
