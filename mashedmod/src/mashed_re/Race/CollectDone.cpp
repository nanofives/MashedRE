// Mashed RE - FUN_00405890 (0x00405890) global reader. In BOTH rsp lists (dualcopy4, 2026-10-16).
//
// The computation is Race/CollectDone.h:CollectDoneBody405890. This TU is the original-address reader of it:
//   - mashed_re_dev.asi: installed at 0x00405890 (registry key pred_405890, arg_type two_global_predicate);
//   - mashed_re.exe:     the pickup tick's naked PK_458920 calls it (Gameplay/PickupCollect_naked.inc:25 was `call PK_405890`,
//                        a verbatim naked copy, now deleted). The exe maps the original's data addresses itself.
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (original\MASHED.exe.unpatched).
#include "../Core/HookSystem.h"
#include "CollectDone.h"

#include <cstdint>

// [moved to Match/MatchS01.cpp: C4-by-match port body] 0x00405890 Pred405890
