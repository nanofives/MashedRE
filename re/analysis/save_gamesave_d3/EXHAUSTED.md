# save_gamesave_d3 — Callee Tree Exhausted

Session: save_gamesave_d3-20260508-1523  
Date: 2026-05-08  
Pool slot: Mashed_pool10

## Result

Fewer than 5 unmapped callees found after drift-skip. Session halted per threshold rule.

## Parent coverage (11 RVAs)

**save_gamesave (d1):** 004099e0, 00404e50, 00404f50, 00404f80, 004b3b70, 004b3bb0  
**save_gamesave_d2:** 004cbe80, 004cbd30, 004cc160, 004cc230, 00550b00

## Depth-3 callees enumerated

| RVA | Called by | Status after drift-skip |
|-----|-----------|------------------------|
| 004d7ff0 | 004cbe80/004cbd30/004cc160/004cc230 | Already in hooks.csv (audio, C1) |
| 004d8480 | 004cbe80/004cbd30/004cc160/004cc230 | Already in hooks.csv (audio, C1) |
| 00550950 | 004cbd30 | Already in hooks.csv (audio, C1) |
| 00550af0 | 004cbd30 | Already in hooks.csv (audio, C1) |
| 005507b0 | 004cc230 | Already in hooks.csv (io, C1) — VFS_Open |
| **00550980** | 004cbe80 | **NOT in hooks.csv — unmapped** |
| **00550910** | 004cc160 | **NOT in hooks.csv — unmapped** |
| **00550bc0** | 004cc230 | **NOT in hooks.csv — unmapped** |

## 3 Uncovered RVAs (below 5-threshold, not analyzed this session)

- `0x00550980` — called by FUN_004cbe80 (RW stream write, file-path branch)
- `0x00550910` — called by FUN_004cc160 (save subsystem function)
- `0x00550bc0` — called by FUN_004cc230 (save subsystem function)

These are in the `0x005507xx–0x00550bxx` utility cluster, likely RWS stream helpers analogous to already-mapped 00550950/00550af0. Suggest folding into an `audio_rws_loader_d3` or `save_stream_helpers` micro-session rather than a full d4 fanout.
