# U-9673 RESULT: standalone nav harness R2-2 crash (2026-10-12)

Branch `work/u9673`. Verdict: **fixed in the test harness only. No shared port code changed.** The full harness
now runs to the end: **75 checks OK, 0 FAIL**, 30 of 30 repeat runs identical. The reachability pass runs again.
It reports 12/33. The change from the last full run (11/33) comes from `38a9e0f3` (U-9672 item 1, a user
decision), not from U-9667 (section 4).

## 1. The raw accesses and why they faulted

**Correction to the U-9673 row: the harness first broke at `b83b01e0` (2026-08-29), not `2528c8b3`
(2026-09-05).** Unmapped builds (the original link flags, no arena) from `git archive`:

| Revision | Result |
|---|---|
| `bd92c046` (harness added) | runs to Piece 6, GREEN |
| `b83b01e0^` | runs to Piece 6, GREEN |
| `b83b01e0` | AV (0xC0000005) at R2-2 |
| `2528c8b3^` | AV at R2-2, 5 of 5 runs |
| HEAD before this fix | AV at R2-2 |

R2-2 touches two raw MASHED addresses. Both fault in a bare test exe:

1. **Read, from `b83b01e0`.** `Q_ActivePlayerCount` (`MenuNavSM.cpp`, the
   `reinterpret_cast<const std::int32_t*>(0x007f1a14)` loop) reads 0x007f1a14 + i*0x10 verbatim, as FUN_00430b60
   does (`0x00430b60 mov edx,[0x7f1a14]` .. `0x00430b86`). R2-2 reaches it through `Nav(0x12, kNavPush)`
   (the screen-18 grey-out, PlaceCursor case 0x12).
2. **Write, from `2528c8b3`.** This is the line the U-9673 row names. It runs earlier in R2-2, so at HEAD it is
   the first fault. `2528c8b3` added one line to `Nav_GameStateLoadSave` (`mashedmod/src/mashed_re/Frontend/MenuNavSM.cpp`, the
`std::memcpy(reinterpret_cast<void*>(0x007f0a40u), data + kSpanFile, kSpanBytes)` after the private
`g_save_span` copy). It mirrors the restored span to the live addresses, as the original's REP MOVSD at
`0x00404e91` in FUN_00404e80 does.

- In MASHED.exe, 0x007f0a40 is in `.data` (0x005ea000..0x00914704) past its raw end 0x00637000, so it is
  .bss: present and zero at load (section table of `original/MASHED.exe.unpatched`, memory
  `data-section-is-mostly-bss`).
- In mashed_re.exe the standalone maps it itself in two steps: the `B17_TlsReserve` EXE TLS callback
  (`exe_main.cpp`, `.CRT$XLB`, which reserves 0x007f0000 and 7 other granules), then the granule walk over
  `kMashedDataBase`/`kMashedDataSize` = 0x00500000..0x009fffff (`exe_main.cpp:520-521`).
- `navsm_test.exe` did neither. The write landed on a page it does not own, so it faulted. Two states were
  observed at 0x007f0000 in the test process: not mapped, or covered by a read-only `MEM_MAPPED` view with
  allocation base 0x00720000 that the loader maps before TLS time (state 0x1000, type 0x40000, protect 0x2).
  Both fault on a write.
- Other raw MASHED addresses in the linked files: `FrontendState.cpp` reads 0x007f1a14..0x007f1a44,
  0x0067f184, 0x0067e9fc, 0x0067f17c and 0x007f0a40[], and writes 0x0067e9fc. `MenuScoreSort.cpp` reads
  0x0067e9fc. All of them are inside 0x00500000..0x009fffff, so the fix below covers them. The harness runs to
  the end, so none of them faults any more. `MenuScoreSort.cpp`'s `s_AbortFn = 0x004a332b` is a code pointer
  below the range. It is not called on any harness path (it would fault on call, not on load).
- Semantics on zero: with the four slot globals at 0, `Q_ActivePlayerCount` returns 4. That matches the
  original's measured value at a bare nav push (`b83b01e0` message: globals read [0,0,0,0], Frida call on the
  original at screen 18 returned 4). R2-2's `screen18 av1/av6` checks pass on it.

Memory `naked-asm-rdata-reads-zero-in-exe` does not apply (this is a C++ write, not a naked-asm `.rdata` read).
Memory `zeroed-granule-vs-minus-one-sentinel` does apply: the test arena is zero, like the exe's granules.
For this span that also matches the original, because it is .bss there.

## 2. Fix (test only)

`re/analysis/standalone_menu_sm/harvest/navsm_test.cpp` and `build_navsm_test.bat`:

- `static unsigned char g_mashed_arena[0x00A00000]` (10 MB, .bss). The bat now links with
  `/BASE:0x10000 /FIXED:NO /DYNAMICBASE:NO` (the same flags as mashed_re.exe, `build.bat:218`), so the image
  starts low and the arena spans 0x00036a00..0x00a369ff. That covers all of 0x00500000..0x009fffff. The image
  is mapped before any loader view, so the range is always the test's own memory: writable, zero, and holding
  no other test object.
- `MashedDataRangeOwned()` prints the arena range and FAILs if it does not cover 0x00500000..0x009fffff.
  If that check fails, R2-2 prints `R2-2 SKIPPED ... : FAIL` instead of writing to unowned memory, and the
  later sections still run.

Rejected first: copying the exe's VirtualAlloc approach. A main()-time granule walk owned the span granule in
3 of 5 runs. Adding the exe's TLS-callback reservation made it worse (4 of 20), because the 0x00720000 view
is already there at TLS time. The exe's own comment says such granules "cannot be reclaimed". The image arena
went 30 of 30.

**This fix makes R2-2 stricter.** All four original R2-2 checks are kept, and two are added on the live addresses:
- `blank save leaves live 0x007f0a50 untouched`: a 0xA5A5A5A5 sentinel survives the refused blank save.
- `live span mirrored (a50=2 a58=1 ad4=1 f2c=1)`: after the written save, the live
  0x007f0a50 / 0x007f0a58 / 0x007f0ad4 / 0x007f0f2c hold the file values.
- **Control (RED):** removing only the mirror line from MenuNavSM.cpp gives
  `live span mirrored (a50=2779096485 a58=0 ad4=0 f2c=0): FAIL` and the bat exits 1 ("RED - one or more
  assertions FAILED"). The source was restored afterwards (`git status` clean under `mashedmod/`), then the
  harness was rebuilt GREEN.

The R2-close pass now also prints the reachable set itself, not only the count.

## 3. "4 more files to link"

The bat already had them on this base. `3ee5f26c` added `FrontendNav.cpp`, `FrontendState.cpp`,
`MenuScoreSort.cpp` and `Race/RaceModes.cpp` (ROW_MOVERS_RESULT.md §8). The bat builds with no link errors.
No change was needed beyond the link flags above.

## 4. Full harness result (HEAD + this fix; full log `u9673_navsm_test_out.txt`)

| Section | Checks OK | FAIL | Notes |
|---|---|---|---|
| arena precheck | 1 | 0 | arena 0x00036a00..0x00a369ff |
| Piece 1 SELECT routing | 5 | 0 | |
| Piece 2 grey-out | 4 | 0 | screen 28 line is informational (no verdict) |
| Piece 3 slide tick | 2 | 0 | "on push" line is informational |
| R2-2 save-driven state | 6 | 0 | 4 original checks + 2 new live-span checks |
| R2-close reachability | 1 | 0 | build pass 33/33. Reachability count is printed, not asserted (it never was) |
| Piece 4 WS-G2 | 8 | 0 | |
| Piece 5 continue-cup | 23 | 0 | |
| Piece 6 config wrap-edit | 25 | 0 | |
| **Total** | **75** | **0** | bat: `[navsm_test] GREEN - all assertions passed`, exit 0 |

Stability: 30 consecutive runs of the built exe, all rc=0, 0 FAIL lines, Piece 6 reached in all 30.

### Reachability across revisions (same harness logic, same arena, built from `git archive`; `bd92c046` and `b83b01e0^` also give 11/33 with the same set when built unmapped)

| Revision | Reachable | Set |
|---|---|---|
| `284d706d` (2026-06-10 R2-close, recorded) | 10/33 | 0 1 2 3 4 8 19 30 31 32 |
| `2528c8b3^` | 11/33 | 0 1 2 3 4 **7** 8 19 30 31 32 |
| `a05b9c14` (U-9667 fix) | 11/33 | unchanged |
| `38a9e0f3^` | 11/33 | unchanged |
| `38a9e0f3` (U-9672 item 1: screen 4 confirm pushes 6) | 12/33 | 0 1 2 3 4 **6** 8 **15** 19 30 31 32 |
| `3ee5f26c`, HEAD | 12/33 | same as `38a9e0f3` |

- **U-9667 changed nothing in the reachable set.** This is the question U-9673 was holding open.
- `38a9e0f3` changes it, as its own code describes (decode cited in that commit: `0x0043e557..0x0043e607`):
  - screen 4's confirm now pushes 6 (Challenge Select) when FUN_00430760 returns 1;
  - otherwise it takes the team branch, which pushes 0xf (15) at 0x0043e5e9.
  - The BFS tries every slot_kind, so it now reaches 6 and 15.
  - Screen 7 drops out. Its only remaining push in this BFS comes from screen 5 (Piece 1 `0xff240000 fresh -> push 7`), and 5 is not reachable from roots {0,1} in the push map.
- [UNCERTAIN] Whether the original can reach screen 7 from screen 4 by another path is not established here.
  The harness walks the port's push map, not the original. U-9672 already tracks "screen 4 -> 7 vs 6".
- In the original, the 21 unreachable screens are state- or flow-gated (the 2026-06-10 classification).
  This harness does not re-check that.

## 5. Builds

No file under `mashedmod/` changed, so the `.asi` and `mashed_re.exe` builds are unaffected. They were not rebuilt.

## 6. Proposed tracker changes (orchestrator applies)

- `UNCERTAINTIES.md` U-9673: mark **RESOLVED 2026-10-12**. Proposed text:

  > ~~coverage~~ **RESOLVED 2026-10-12**: the R2-2 crash was raw MASHED-address access on a page the bare
  > test exe does not own. The page was either unmapped or covered by a read-only loader view at 0x00720000.
  > The first break was `b83b01e0` (Q_ActivePlayerCount reads 0x007f1a14, FUN_00430b60), not `2528c8b3`.
  > `2528c8b3`'s live-span mirror (`memcpy` to 0x007f0a40) added a second fault, earlier in R2-2. mashed_re.exe maps that range itself (B17 TLS
  > reservation + kMashedDataBase granule walk). Fix is test-only: navsm_test.exe is linked at /BASE:0x10000
  > with a 10 MB .bss arena covering 0x00500000..0x009fffff (30/30 runs). R2-2 gained two live-span checks.
  > A mirror-removed control reads RED. Full harness 75 OK / 0 FAIL, reachability pass runs again: 12/33.
  > U-9667 left the reachable set unchanged (11/33 before and after). The +6/+15/-7 change comes from
  > `38a9e0f3` (U-9672 item 1). re/analysis/negctl/followup_20261012/u9673_RESULT.md

- No hooks.csv row changes. No C-level moves: this is harness coverage only.
- Optional, user call: pin the reachable set `{0,1,2,3,4,6,8,15,19,30,31,32}` as an assertion in R2-close so
  a future push-map change shows as RED. It is not pinned now because that set comes from the port, not from
  the original. Pinning it would freeze the current state rather than check correctness.

## 7. Evidence-set legs

Not applicable. This is a harness fix with no hook row. Ready for path2: none.
