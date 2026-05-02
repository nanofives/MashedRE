# Analysis Changelog

Append-only log of confidence promotions and demotions, written by the `re-classify` skill. One line per event.

Format: `YYYY-MM-DD  RVA  name  oldC->newC  evidence`

Demotions use `oldC<-newC` (arrow flipped).

```
2026-05-02  --------  PROJECT BOOTSTRAP — first MCP session
2026-05-02  004a4bb7  entry         C0->C1  symbol IMPORTED by Ghidra as PE entry; MSVC EH runtime visible nearby (re/analysis/entry_point.md)
```
