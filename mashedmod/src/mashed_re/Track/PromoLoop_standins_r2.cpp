// Mashed RE: stand-in promotion round 2 (plan item #3 step 2, 2026-10-11).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// 0x0047bcc0  TriEdgePairsBuild  (FUN_0047bcc0, 434 B; caller 0x0047bf70)
//   ABI: EAX = n (triangle count, signed: `jle` at 0x0047bd0a), stack cdecl (out, tris); EBX/EBP/ESI/EDI preserved,
//   frame `sub esp, 0x10` + 4 pushes, `add esp, 0x10; ret` at 0x0047be6e. No meaningful return value (EAX is
//   whatever rwfree leaves).
//   tris: 8-byte records, u16 vertex indices at +0 / +2 / +4 (movzx at 0x0047bd10 / 0x0047bd16 / 0x0047bd35).
//   out:  out[0] = block of 0x18-byte edge records, out[1] = record count (0x0047be27 / 0x0047be29).
//
//   0x0047bccd  ebp = 3n; 0x0047bcd1 esi = ebp*3; 0x0047bcd5 shl esi, 3    bytes = 72n (32-bit)
//   0x0047bce3  tmp = rwmalloc(bytes, 0)                                     *(0x007d3ff8)+0x108
//   0x0047bcf4  rep stosd / 0x0047bcfb rep stosb                             zero bytes of tmp
//   0x0047bd10..0x0047bd71  per triangle i, three records (stride 0x18):
//       {+0 tri, +4 0, +8 i, +0xc -1, +0x10 v0, +0x14 v1}, then (v1, v2), then (v2, v0)
//   0x0047bd73..0x0047bde0  for a = 0 .. 3n-2 (k = a+1): if tmp[a].+0 != 0 and k < 3n, for each b in k .. 3n-1 with
//       tmp[b].+0 != 0 and 0x0047bc90(tmp[a], tmp[b]) != 0: tmp[a].+4 = tmp[b].+0, tmp[a].+0xc = tmp[b].+8,
//       tmp[b].+0 = 0, tmp[b].+8 = -1 (0x0047bdaf..0x0047bdbe). The inner loop does not stop at a match.
//   0x0047bde2..0x0047be04  cnt = records with +0 != 0 and +4 != 0
//   0x0047be18  p = rwmalloc(cnt * 0x18, 0); out[0] = p, out[1] = cnt (no fill)
//   0x0047be2c..0x0047be54  copy those records (6 dwords, rep movsd) into p in order
//   0x0047be61  rwfree(tmp)                                                  *(0x007d3ff8)+0x10c
//
// 0x0047bc90 (C3, ESI = a, EDX = b -> EAX) is called through its C body Match47bc90(s, e), now in
// Match/MatchS13_0047bf70.cpp (one body per RVA, rva-lint): 1 when a.v0 is b.v0 or b.v1 AND a.v1 is b.v0 or b.v1.

// MATCH-S13: the body of 0x0047bcc0 (TriEdgePairsBuildBody, entry TriEdgePairsBuild) moved to
// Match/MatchS13_0047bf70.cpp, which compiles the byte-matched source re/match/s13/0047bf70.c.
