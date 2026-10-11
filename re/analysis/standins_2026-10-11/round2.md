# Stand-in promotion round 2 (2026-10-11): 0x0047bcc0

Port: `mashedmod/src/mashed_re/Track/PromoLoop_standins_r2.cpp` (.asi only, exe_file empty); full disassembly cited in
the port header. Harness addition: `reg_args` (register-argument thunk, `re/frida/js/standins.js` regThunk), used to
deliver EAX on both arms.

## 0x0047bcc0 TriEdgePairsBuild (was FUN_0047bcc0)

**Purpose.** Given `n` triangles in EAX (8-byte records, u16 vertex indices at +0/+2/+4) it allocates a 72n-byte temp
block through rwmalloc, zeroes it and writes three 0x18-byte edge records per triangle ({tri, 0, i, -1, va, vb} for
(v0,v1), (v1,v2), (v2,v0)). For each record with a non-NULL +0 it scans every later record and, for each one that still
has a non-NULL +0 and matches under 0x0047bc90, copies that record's +0/+8 into its own +4/+0xc and clears the other
record (+0 = 0, +8 = -1); the scan does not stop at the first match. It counts the records with +0 and +4 both non-NULL,
allocates `count * 0x18` through rwmalloc into out[0] with the count in out[1], copies those records in order and frees
the temp block through rwfree.

- Caller: 0x0047bf70 (C2). Callees: 0x0047bc90 (C3, called through its existing body Match47bc90), rwmalloc / rwfree
  (RwGlobals memoryFuncs slots, identified RW API).
- path1 GREEN 7/7 (self-baseline GREEN 7/7): one triangle (0 pairs), two sharing an edge (1), a 4-triangle fan (4),
  degenerate repeats (5,5,6)/(5,6,5) (1), three identical triangles (3, exercising the no-break scan), n = 0, and a
  5-triangle strip (4). The arena digest covers the temp block after pairing and the copied output records.
  path2 PASS 2/2 (inline JMP live, EAX delivered by the same thunk).
