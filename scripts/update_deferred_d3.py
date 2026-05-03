#!/usr/bin/env python3
import os, sys

path = 'DEFERRED.md'
with open(path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Lines to remove from Active (D-0100..D-0119)
old_ids = {f'D-{i:04d}' for i in range(100, 120)}

# New active rows to insert where D-0100 was
new_active = [
    '| D-0460 | 0x004ac570 FUN_004ac570 | depth-3 of FUN_004ab8d6; supersedes D-0119; not taken in boot_crt_exit_d3 (cap 18) | pick up as bucket boot_crt_exit_d3-cont1; no further recursion | boot |\n',
    '| D-0461 | 0x004af32d FUN_004af32d | depth-3 of FUN_004a3258; supersedes D-0106; not taken in boot_crt_exit_d3 (cap 18) | pick up as bucket boot_crt_exit_d3-cont1; no further recursion | boot |\n',
    '| D-0462 | 0x004a2bb8 report_failure | depth-4 of 0x004a2be9 __security_check_cookie; not recursed | boot_crt_exit_d3-cont1 or dedicated boot session | boot |\n',
    '| D-0463 | 0x004a31e1 FUN_004a31e1 | depth-4 of 0x004a4126 __onexit; called before __onexit_lk | boot_crt_exit_d3-cont1 | boot |\n',
    '| D-0464 | 0x004a407e __onexit_lk | depth-4 of 0x004a4126 __onexit; core logic of __onexit | boot_crt_exit_d3-cont1 | boot |\n',
    '| D-0465 | 0x004a4158 FUN_004a4158 | depth-4 of 0x004a4126 __onexit; called after __onexit_lk | boot_crt_exit_d3-cont1 | boot |\n',
    '| D-0466 | 0x004ad33b __controlfp | depth-4 of 0x004a5de3 FUN_004a5de3; called with (0x10000,0x30000) | boot_crt_exit_d3-cont1 | boot |\n',
    '| D-0467 | 0x004a5df5 __ms_p5_test_fdiv | depth-4 of 0x004a5e35 __ms_p5_mp_test_fdiv; fallback FPU test | boot_crt_exit_d3-cont1 | boot |\n',
    '| D-0468 | 0x004a9744 __flushall | depth-4 of 0x004a5f07 ___endstdio; unconditional flush | boot_crt_exit_d3-cont1 | boot |\n',
    '| D-0469 | 0x004ad351 __fcloseall | depth-4 of 0x004a5f07 ___endstdio; conditional close via DAT_007739d4 | boot_crt_exit_d3-cont1 | boot |\n',
    '| D-0470 | 0x004a7800 FUN_004a7800 | depth-4 of 0x004a787f __lock; lazy-init of lock slot | boot_crt_exit_d3-cont1 | boot |\n',
    '| D-0471 | 0x004aa7da ___sbh_alloc_new_region | depth-4 of 0x004aac76 ___sbh_alloc_block; allocates new SBH region | boot_crt_exit_d3-cont1 | boot |\n',
    '| D-0472 | 0x004aa891 ___sbh_alloc_new_group | depth-4 of 0x004aac76 ___sbh_alloc_block; initialises new SBH group | boot_crt_exit_d3-cont1 | boot |\n',
]

cleared = [
    '| D-0100 | 0x004a2bf7 FUN_004a2bf7 | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0101 | 0x004a5e35 __ms_p5_mp_test_fdiv | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0102 | 0x004a5de3 FUN_004a5de3 | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0103 | 0x004a5f07 ___endstdio | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0104 | 0x004a77eb FUN_004a77eb | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0105 | 0x004a787f __lock | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0106 | 0x004af32d FUN_004af32d | superseded by D-0461; not taken due to cap | 2026-05-02 |\n',
    '| D-0107 | 0x004a4126 __onexit | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0108 | 0x004a4728 FUN_004a4728 | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0109 | 0x004a5984 __SEH_prolog | analyzed C1 (entry_callees already; D now closed) | 2026-05-02 |\n',
    '| D-0110 | 0x004a59bf __SEH_epilog | analyzed C1 (entry_callees already; D now closed) | 2026-05-02 |\n',
    '| D-0111 | 0x004aac76 ___sbh_alloc_block | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0112 | 0x004aaf72 __callnewh | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0113 | 0x004aaf90 _memset | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0114 | 0x004a7796 __mtdeletelocks | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0115 | 0x004a2be9 __security_check_cookie | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0116 | 0x004a3440 __chkstk | analyzed C1 (entry_callees already; D now closed) | 2026-05-02 |\n',
    '| D-0117 | 0x004a34b0 _strncpy | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0118 | 0x004ac45c ___crtMessageBoxA | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |\n',
    '| D-0119 | 0x004ac570 FUN_004ac570 | superseded by D-0460; not taken due to cap | 2026-05-02 |\n',
]

out = []
inserted_active = False
for line in lines:
    # Detect start of D-0100 block
    if not inserted_active and '| D-0100 |' in line:
        out.extend(new_active)
        inserted_active = True
        # Skip D-0100..D-0119
        continue
    if inserted_active and any(f'| {did} |' in line for did in old_ids):
        continue
    # After D-0039, insert cleared rows
    if 'D-0039 | 0x004ae29f ___crtInitCritSecAndSpinCount' in line and '2026-05-02' in line:
        out.append(line)
        out.extend(cleared)
        continue
    out.append(line)

with open(path, 'w', encoding='utf-8') as f:
    f.writelines(out)

print('Done. Active replacement:', inserted_active)
