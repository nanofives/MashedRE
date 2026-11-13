# PREREG DDKOBJ-CELLS (2026-11-12)

Row: recover the 5 units the object build refuses as `fail:wrapper-redefines-globals` (0x004769f0, 0x004282a0,
0x004963e0, 0x00496400, 0x00498910 + 0x00498950 + 0x004989b0). Row kind: top-down (wrappers, the exe bindings they
read, `re/tools/ddkobj/`). Written before any build or arm of this branch.

## Change

1. The 5 wrappers bind the sources' globals to their original addresses with the preamble's own form
   (`static T &n = *(T *)(a)`), no cell indirection.
   - 0x004769f0: its cells already were the original addresses. Text change only.
   - 0x004282a0 (exe): `[0x0067d838]` / `[0x0067d830]` at the original addresses (the exe arena, BSS in the
     original). `MeasureMenuMessage` stores `DeriveMenuMeasureDiv()` at `[0x0067d830]` when it is zero. The lookup slot
     `FUN_00427780` returns the id's `[u16 len][chars]` record in `g_msg_dat` (the shape the original returns), and
     `FUN_004277a0` binds to the S06 body `TextCtrlCodeRemapBody` in both targets. Reason: the DDK object remaps
     through its own in-object copy of 0x004277a0, which needs a real record, not the id.
   - Settings cluster (exe): the log handle `0x00772fbc`, filename `0x007731e8` and buffer `0x00773208` live at their
     original addresses (all BSS in the original). With the port-state seed OFF, the five .rdata strings are written
     to their original addresses at dynamic initialization (before: private copies).
2. `ddkobj.py`: a wrapper `#define MGLOBAL` / `MGLOBALA` in exactly the original-address form is accepted. Any other
   form stays `fail:wrapper-redefines-globals`. Two self-test fixtures (`mgaddr` accepted, `mgcell` refused).

## Arms and pass bars

| # | arm | pass |
|---|---|---|
| 1 | `ddkobj.py selftest` | 0 failures |
| 2 | object build (`MASHED_DDKOBJ=1`) + `ddkobj.py check` | the 5 units link (no `wrapper-redefines-globals` among them); every function of a 100% source IDENTICAL; refusals = the 4 hand-path units only (0x00414180, 0x00404e80, 0x00404e50, 0x00404e00) |
| 3 | boots, `MASHED_DET_FRAMES=3000`: object build OFF and ON, VS2022 ON | rc 0 each, `returned 0 after 3000 frames` on ON |
| 4 | knob-ON log diff, object vs VS2022, `[ORIG_TOP]`/`[RW_BIND]` lines, pointers masked | 0 lines differ |
| 5 | 12-capture walk (menu code changed), OFF/ON/OFF/ON on the object build and on VS2022 | class E 4/4 each |

Expected knob-ON difference against the integration head, recorded, not a bar: the s16 matched readers of
`0x00773208..` compiled into the exe now see what `ConfigLoad` loaded (before, `ConfigLoad` filled a private copy).
Default-path values are unchanged by construction (every moved cell is BSS, so zero as the private copies were; the
lookup gives the same text as `GetMenuMessage` for every record in the 7 language tables: max length 238 < 511, all
offsets aligned and in range). Default gates are not run here (top-down row, WORKER_RULES); the merge train runs them.
