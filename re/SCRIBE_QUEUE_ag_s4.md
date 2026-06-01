# SCRIBE_QUEUE fragment — batch_ag Session 4 (util C1->C2 author-only)

One row for central finalize to cat into `re/SCRIBE_QUEUE.md` (Queued section).
Format follows `re/SESSION_RULES.md` § "Parallel-fanout scribe-queue pattern".

AUTHOR-ONLY: 60 per-RVA C2 plates written to the bucket dir below. NO hooks.csv /
re-classify / ghidra-sweep / build / Frida / commit performed (central finalize does that).

## Queued

2026-06-01  batch-ag-s4  bucket=re/analysis/bucket_util_004764f0_0052da20  rvas=004764f0,00476880,004777d0,00477920,00477b40,004785e0,00484c90,00485070,00488e70,0048f260,0048f680,0048f6b0,0048f6e0,0048f710,0048f740,00490380,004904d0,00492d10,00493390,00493570,00493580,00493590,004938e0,00493ac0,00493b40,00493b50,00493bc0,00493c00,00493f00,004944b0,004944c0,00494ac0,00494ee0,00494f00,00494f10,00495110,00496930,00496d00,00496d20,00496d60,00496db0,00498810,0049cfb0,0049d080,0049d240,0049d270,0049ec10,004b4430,004b44f0,004b4510,004b4550,004b4cd0,004c4dc0,004ccde0,004d8560,0052cb00,0052d170,0052d7e0,0052d980,0052da20  level=c1->c2-authored  pool=Mashed_pool9  pool-assigned=Mashed_pool9  mcp_session=f2d6d9229dbf4fbfadd6db1b85f247af  count=60/60  note=All 60 were C1 in hooks.csv at start (no drift-skips). Each plate = frontmatter (rva,name,size_bytes,confidence_target=C2,callees_depth1,callers_noted,opened_in_slot,session_date) + mechanical description + constants table + uncertainties + stubs. Genuine engine/game code — this range is BELOW the qhull-2002.1 library island (~0x0057c5b0..0x005a5820) that HALTED batch-ag-s6; none of these 60 are third-party library residue.

### function_create (LAB_ -> function boundary) — done on the Mashed_pool9 writable clone
- 0x00493ac0 (was LAB_00493ac0) -> created FUN_00493ac0; U-6200. Pre-NT5 ANSI codepage resolver (GetThreadLocale/GetLocaleInfoA 0x1004/atoi/GetACP), 126 bytes.
- 0x00493b40 (was LAB_00493b40) -> created FUN_00493b40; U-6201. NT5+ codepage resolver (returns 3 = CP_THREAD_ACP), 5 bytes.
  (Both are the resolver targets installed by FUN_00493b50 via InterlockedExchange into PTR_FUN_006147dc. Boundaries created only because the pre-assigned read-only session could not; reopened pool9 writable, update_analysis=false, NOT saved. Master Ghidra UNTOUCHED — sweep should make these boundaries authoritative in Mashed.gpr.)

### subsystem_observed reclass candidates (hooks.csv has all as util)
- 0x00496d20 -> **render**: thin wrapper over RenderWare `RpWorldForAllWorldSectors(world, &LAB_00496d10, DAT_00773088|0)`. Flag for central reclass review.
- 0x00496d00 (sets DAT_00773090) is a sibling global-setter of the same render-traversal cluster (DAT_00773088) — borderline; left util, noted.
- 0x004c4dc0 RwMatrixInvert and 0x004b4430/0x004b4cd0 (RW matrix/bezier) confirmed correct as util (general RenderWare math primitives; hooks.csv already refined 004c4dc0 vehicle->util).
- 0x0052cb00 / 0x0052d170 / 0x0052d7e0 / 0x0052d980 / 0x0052da20 = the image library (BMP load / scanline convert / scanline write / format-handler registry add / instantiate). Engine image lib, util is correct (NOT vendored-third-party like qhull).

### tracker reconciliations to apply during sweep (reported per NO-GUESSING, decomp-literal)
- 0x0048f710: element count is **10**, not the "9" in the prior hooks.csv note. Bound (0x76a0b8 - 0x769f50)/0x24 = 0x168/0x24 = 10 (last write @0x76a094).
- 0x00490380: **30** entries (matches its zero-init FUN_004904d0 over 0x86a4a0..0x86ae00 stride 0x50), not the "38" in the prior note.
- 0x0049cfb0: sentinel at +0x104 is 0xfffb6c20 = **-300000** (prior note said "-418784"); +0x138=0xfffffc18=-1000, +0x13c/+0x10c=0xffffffff=-1.

### U-IDs minted this session (range U-6200..U-6299)
- U-6200: FUN_00493ac0 boundary created (LAB_-> function). Resolved (behavior fully transcribed).
- U-6201: FUN_00493b40 boundary created (LAB_-> function). Resolved (returns const 3).
(No S-IDs minted from S-4700..S-4799; plates only reference pre-existing S-0373/S-0374/S-0375/S-0376/S-0662/S-1380. Pre-existing U-IDs referenced, not re-minted: U-0367/0368/0369/0670/0671/0672/0673/0676/0677/0678/0679/0680/1144/1387/3472/3473/3474/3475.)
