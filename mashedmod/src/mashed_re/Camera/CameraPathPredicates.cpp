// Mashed RE — camera-path node predicates (any-active-node tests).
//
// MATCH-S13 (2026-11-02): both bodies moved to Match/MatchS13_0047c270.cpp and Match/MatchS13_0047c2d0.cpp,
// which compile the byte-matched sources re/match/s13/0047c270.c and 0047c2d0.c. Those files also carry the
// inner per-node predicates (0x0047c230, 0x0047c1f0) that the old outer scan here called through their
// original addresses with a register convention (naked InvokeInner shim, now retired with the scan).
//
// Refs: re/analysis/bucket_gameplay_0047ba20_0047f380/{0047c270,0047c2d0,0047c230,0047c1f0}.md
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
