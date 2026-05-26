# Phase A — Re-validate the 19 suspect frontend C4 rows under the
# post-loader-fix runtime (dinput8 shim autoloads mashed_re_dev.asi).
#
# Per memory project-loader-broken-9d-audit, these C4 rows were promoted
# during 2026-05-15..05-17 while the .asi was NOT being loaded — their
# canonical-observation evidence was structurally meaningless. This sweep
# re-runs run_verify_hook.py against each, which verifies:
#
#   (a) the inline-JMP install actually mutated the first 5 bytes at the
#       target RVA to E9 rel32 -> reimpl, and
#   (b) the per-input test vectors all return without exception.
#
# Output goes to log/sweep_suspect_c4_frontend_<date>.tsv (one row per
# hook) plus the per-hook reports left by run_verify_hook.py.
#
# Run ONLY on main, ONLY when no other Frida session is active, ONLY
# after the dinput8 shim is confirmed live (project-loader-split-dinput8).

import datetime
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
LOG_DIR = ROOT / 'log'

# 19 suspect frontend C4 rows (each has a registered hook). RVAs paired
# with their hooks_registry keys for invocation.
SUSPECTS = [
    ('menu_alpha_get',                 0x0042b930),
    ('sprite_lookup_table_a',          0x0040bb70),
    ('sprite_lookup_table_b',          0x0040bb90),
    ('hotkey_string_base_get',         0x0040b7a0),
    ('frontend_array_get',             0x0040b6c0),
    ('frontend_global_get',            0x0040ad20),
    ('frontend_player_slot_check',     0x0042ebe0),
    ('hud_slot_type_player0',          0x00430a10),
    ('hud_slot_type_player1',          0x00430a60),
    ('hud_slot_type_player2',          0x00430ab0),
    ('sprite_slot_gate',               0x0042ee00),
    ('menu_button_detect_c',           0x0042b310),
    ('menu_button_detect_a',           0x0042aff0),
    ('menu_button_detect_b',           0x0042b180),
    ('menu_button_detect_e',           0x0042b770),
    ('sprite_anim_frame_thunk',        0x0042e590),
    ('race_end_flag_if_end_mode',      0x0042fe30),
    ('race_end_alt_flag_if_end_mode',  0x0042fe50),
    ('get_race_end_flag',              0x0042fe80),
]

# 2 suspect-C4 rows without registry entries — cannot be re-validated.
# Caller must demote these to C3 separately via re-classify.
UNVALIDATABLE = [
    (0x004c5c00, 'LinkedListStringSearch', 'no_registry_entry__arg_type_REFUSED'),
    (0x004cc160, 'FUN_004cc160',           'no_registry_entry'),
]


def main():
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    date = datetime.datetime.now().strftime('%Y%m%d_%H%M')
    out_tsv = LOG_DIR / f'sweep_suspect_c4_frontend_{date}.tsv'

    print(f'=== Phase A — re-validate {len(SUSPECTS)} suspect frontend C4 rows ===')
    print(f'output: {out_tsv}')
    print()

    rows = [['hook_name', 'rva', 'verify_rc', 'result', 'report_path']]
    for name, rva in SUSPECTS:
        rva_hex = f'0x{rva:08x}'
        print(f'--- {name}  {rva_hex} ---')
        rc = subprocess.call(
            ['py', '-3.12', str(ROOT / 're' / 'frida' / 'run_verify_hook.py'), name],
            cwd=str(ROOT),
        )
        report = LOG_DIR / f'verify_hook_install_{name}.txt'
        result = 'PASS' if rc == 0 else 'FAIL'
        rows.append([name, rva_hex, str(rc), result, str(report.relative_to(ROOT))])
        print(f'  rc={rc}  result={result}')
        print()

    print('=== Unvalidatable rows (no registry entry — demote unconditionally) ===')
    for rva, name, reason in UNVALIDATABLE:
        print(f'  0x{rva:08x}  {name:30s}  reason={reason}')

    print()
    out_tsv.write_text('\n'.join('\t'.join(r) for r in rows) + '\n', encoding='utf-8')
    print(f'tsv written: {out_tsv}')

    pass_n = sum(1 for r in rows[1:] if r[3] == 'PASS')
    fail_n = sum(1 for r in rows[1:] if r[3] == 'FAIL')
    print()
    print(f'=== Summary: {pass_n}/{len(SUSPECTS)} PASS, {fail_n}/{len(SUSPECTS)} FAIL ===')
    print(f'  PASS rows: stay C4; update evidence to main_menu_post_loaderfix_{date[:8]}')
    print(f'  FAIL rows: demote C4 -> C3 via re-classify')
    print(f'  Unvalidatable rows ({len(UNVALIDATABLE)}): demote C4 -> C3 via re-classify')

    return 0


if __name__ == '__main__':
    sys.exit(main())
