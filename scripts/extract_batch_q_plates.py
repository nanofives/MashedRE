import os, re

def find_file(candidates):
    for p in candidates:
        if os.path.exists(p):
            return p
    return None

def extract_plate(filepath):
    if not filepath or not os.path.exists(filepath):
        return None
    with open(filepath, encoding='utf-8', errors='replace') as f:
        content = f.read()
    for header in ['Mechanical description', 'Body', 'Overview', 'Interpretation']:
        m = re.search(r'##\s+' + header + r'\s*\n([\s\S]*?)(?=\n##|\Z)', content)
        if m:
            lines = [l.strip() for l in m.group(1).split('\n') if l.strip()]
            if lines:
                text = lines[0].lstrip('- *1.2.3.').strip()
                if len(text) > 120:
                    text = text[:120].rsplit(' ', 1)[0] + '…'
                return text
    # fallback: first non-frontmatter line
    lines = [l.strip() for l in content.split('\n')
             if l.strip() and not l.startswith('#') and not l.startswith('---')
             and not re.match(r'\w+:', l)]
    if lines:
        text = lines[0].lstrip('- *').strip()
        if len(text) > 120:
            text = text[:120].rsplit(' ', 1)[0] + '…'
        return text
    return None

# s (session label) -> [(rva_hex, [file_candidates])]
SESSIONS = [
    ('s1', [
        ('0x0040e470', ['.worktrees/batch-q-s1/re/analysis/race_results/0040e470.md']),
        ('0x00401570', ['.worktrees/batch-q-s1/re/analysis/title_screen_d2/0x00401ee0.md', 're/analysis/title_screen_d2/0x00401ee0.md']),
        ('0x00401da0', ['.worktrees/batch-q-s1/re/analysis/title_screen_d2/0x00401ee0.md', 're/analysis/title_screen_d2/0x00401ee0.md']),
        ('0x00401ee0', ['.worktrees/batch-q-s1/re/analysis/title_screen_d2/0x00401ee0.md', 're/analysis/title_screen_d2/0x00401ee0.md']),
    ]),
    ('s2', [
        ('0x0040e480', ['.worktrees/batch-q-s2/re/analysis/c0_promotion_frontend_a/0x0040e480.md', 're/analysis/c0_promotion_frontend_a/0x0040e480.md']),
        ('0x00403050', ['.worktrees/batch-q-s2/re/analysis/loading_screen/0x00403050.md', 're/analysis/loading_screen/0x00403050.md']),
        ('0x00408a50', ['.worktrees/batch-q-s2/re/analysis/race_results/00408a50.md', 're/analysis/race_results/00408a50.md']),
        ('0x00408a70', ['.worktrees/batch-q-s2/re/analysis/race_results/00408a70.md', 're/analysis/race_results/00408a70.md']),
    ]),
    ('s3', [
        ('0x004c5c00', ['.worktrees/batch-q-s3/re/analysis/sprite_gate_c3/0x004c5c00.md', 're/analysis/sprite_gate_c3/0x004c5c00.md']),
        ('0x00408ad0', ['.worktrees/batch-q-s3/re/analysis/race_results/00408ad0.md', 're/analysis/race_results/00408ad0.md']),
        ('0x0040acd0', ['.worktrees/batch-q-s3/re/analysis/c0_promotion_frontend_a/0x0040acd0.md', 're/analysis/c0_promotion_frontend_a/0x0040acd0.md']),
        ('0x0040b290', ['.worktrees/batch-q-s3/re/analysis/race_results/0040b290.md', 're/analysis/race_results/0040b290.md']),
    ]),
    ('s4', [
        ('0x00427e00', ['.worktrees/batch-q-s4/re/analysis/hud_frontend/0x00427e00.md', 're/analysis/hud_frontend/0x00427e00.md']),
        ('0x0040b460', ['.worktrees/batch-q-s4/re/analysis/hud_frontend_d3/0x0040b460.md', 're/analysis/hud_frontend_d3/0x0040b460.md']),
        ('0x0040b620', ['.worktrees/batch-q-s4/re/analysis/hud_frontend_d3/0x0040b620.md', 're/analysis/hud_frontend_d3/0x0040b620.md']),
        ('0x0040b6b0', ['.worktrees/batch-q-s4/re/analysis/hud_frontend_d3/0x0040b6b0.md', 're/analysis/hud_frontend_d3/0x0040b6b0.md']),
    ]),
    ('s5', [
        ('0x00472f40', ['.worktrees/batch-q-s5/re/analysis/credits_screen/0x00472f40.md', 're/analysis/credits_screen/0x00472f40.md']),
        ('0x0040b7a0', ['.worktrees/batch-q-s5/re/analysis/hud_frontend_d3/0x0040b7a0.md', 're/analysis/hud_frontend_d3/0x0040b7a0.md']),
        ('0x0040b7b0', ['.worktrees/batch-q-s5/re/analysis/hud_frontend_d3/0x0040b7b0.md', 're/analysis/hud_frontend_d3/0x0040b7b0.md']),
        ('0x0040bb70', ['.worktrees/batch-q-s5/re/analysis/hud_frontend_d3/0x0040bb70.md', 're/analysis/hud_frontend_d3/0x0040bb70.md']),
    ]),
    ('s6', [
        ('0x004730b0', ['.worktrees/batch-q-s6/re/analysis/credits_screen/0x004730b0.md', 're/analysis/credits_screen/0x004730b0.md']),
        ('0x0040bb90', None),
        ('0x0040d250', ['.worktrees/batch-q-s6/re/analysis/title_screen_d2/0x0040d250.md', 're/analysis/title_screen_d2/0x0040d250.md']),
        ('0x0040d590', ['.worktrees/batch-q-s6/re/analysis/race_results/0040d590.md', 're/analysis/race_results/0040d590.md']),
    ]),
    ('s7', [
        ('0x00472c60', ['.worktrees/batch-q-s7/re/analysis/hud_frontend/0x00472c60.md', 're/analysis/hud_frontend/0x00472c60.md']),
        ('0x0040e3a0', ['.worktrees/batch-q-s7/re/analysis/hud_frontend_d3/0x0040e3a0.md', 're/analysis/hud_frontend_d3/0x0040e3a0.md']),
        ('0x0040eee0', ['.worktrees/batch-q-s7/re/analysis/race_results/0040eee0.md', 're/analysis/race_results/0040eee0.md']),
        ('0x00414120', ['.worktrees/batch-q-s7/re/analysis/c0_promotion_frontend_a/0x00414120.md', 're/analysis/c0_promotion_frontend_a/0x00414120.md']),
    ]),
    ('s8', [
        ('0x004a2c48', ['.worktrees/batch-q-s8/re/analysis/hud_frontend_d3/004a2c48.md', 're/analysis/hud_frontend_d3/004a2c48.md']),
        ('0x004189f0', ['.worktrees/batch-q-s8/re/analysis/race_results_d2/004189f0.md', 're/analysis/race_results_d2/004189f0.md']),
        ('0x004215c0', ['.worktrees/batch-q-s8/re/analysis/race_results_d2/004215c0.md', 're/analysis/race_results_d2/004215c0.md']),
        ('0x00422fd0', ['.worktrees/batch-q-s8/re/analysis/race_results/00422fd0.md', 're/analysis/race_results/00422fd0.md']),
    ]),
    ('s9', [
        ('0x004739f0', ['.worktrees/batch-q-s9/re/analysis/hud_frontend/0x004739f0.md', 're/analysis/hud_frontend/0x004739f0.md']),
        ('0x00426cf0', ['.worktrees/batch-q-s9/re/analysis/title_screen_d2/0x00426cf0.md', 're/analysis/title_screen_d2/0x00426cf0.md']),
        ('0x004274d0', ['.worktrees/batch-q-s9/re/analysis/localization/004274d0.md', 're/analysis/localization/004274d0.md']),
        ('0x004274e0', ['.worktrees/batch-q-s9/re/analysis/localization/004274e0.md', 're/analysis/localization/004274e0.md']),
    ]),
    ('s10', [
        ('0x00473870', ['.worktrees/batch-q-s10/re/analysis/hud_frontend_d5/0x00473870.md', 're/analysis/hud_frontend_d5/0x00473870.md']),
        ('0x00427ad0', ['.worktrees/batch-q-s10/re/analysis/hud_frontend_d3/0x00427ad0.md', 're/analysis/hud_frontend_d3/0x00427ad0.md']),
        ('0x00427c90', ['.worktrees/batch-q-s10/re/analysis/title_screen/0x00427c90.md', 're/analysis/title_screen/0x00427c90.md']),
        ('0x00427f00', ['.worktrees/batch-q-s10/re/analysis/hud_frontend_d5/0x00427f00.md', 're/analysis/hud_frontend_d5/0x00427f00.md']),
    ]),
    ('s11', [
        ('0x00417740', ['.worktrees/batch-q-s11/re/analysis/frontend_score_getters/0x00417740.md']),
        ('0x00428140', ['.worktrees/batch-q-s11/re/analysis/hud_frontend/0x00428140.md', 're/analysis/hud_frontend/0x00428140.md']),
        ('0x004282a0', ['.worktrees/batch-q-s11/re/analysis/hud_frontend_d3/0x004282a0.md', 're/analysis/hud_frontend_d3/0x004282a0.md']),
        ('0x00428320', ['.worktrees/batch-q-s11/re/analysis/hud_frontend_d3/0x00428320.md', 're/analysis/hud_frontend_d3/0x00428320.md']),
    ]),
    ('s12', [
        ('0x004b5750', ['.worktrees/batch-q-s12/re/analysis/hud_ingame_d3/0x004b5750.md']),
        ('0x00428390', ['.worktrees/batch-q-s12/re/analysis/title_screen/0x00428390.md', 're/analysis/title_screen/0x00428390.md']),
        ('0x004288a0', ['.worktrees/batch-q-s12/re/analysis/title_screen_d2/0x004288a0.md', 're/analysis/title_screen_d2/0x004288a0.md']),
        ('0x00428a30', ['.worktrees/batch-q-s12/re/analysis/title_screen/0x00428a30.md', 're/analysis/title_screen/0x00428a30.md']),
    ]),
    ('s13', [
        ('0x004726f0', ['.worktrees/batch-q-s13/re/analysis/hud_frontend_d3/0x004726f0.md']),
        ('0x00428bf0', ['.worktrees/batch-q-s13/re/analysis/title_screen/0x00428bf0.md', 're/analysis/title_screen/0x00428bf0.md']),
        ('0x00428d30', ['.worktrees/batch-q-s13/re/analysis/title_screen/0x00428d30.md', 're/analysis/title_screen/0x00428d30.md']),
        ('0x00429240', ['.worktrees/batch-q-s13/re/analysis/title_screen/0x00429240.md', 're/analysis/title_screen/0x00429240.md']),
    ]),
    ('s14', [
        ('0x00473c20', ['.worktrees/batch-q-s14/re/analysis/hud_frontend_d3/0x00473c20.md']),
        ('0x00429290', ['.worktrees/batch-q-s14/re/analysis/title_screen/0x00429290.md', 're/analysis/title_screen/0x00429290.md']),
        ('0x00429870', ['.worktrees/batch-q-s14/re/analysis/hud_frontend_d3/0x00429870.md', 're/analysis/hud_frontend_d3/0x00429870.md']),
        ('0x004298c0', ['.worktrees/batch-q-s14/re/analysis/c0_promotion_frontend_a/0x004298c0.md', 're/analysis/c0_promotion_frontend_a/0x004298c0.md']),
    ]),
    ('s15', [
        ('0x00473ee0', ['.worktrees/batch-q-s15/re/analysis/hud_frontend_d3/0x00473ee0.md']),
        ('0x00429a30', ['.worktrees/batch-q-s15/re/analysis/hud_frontend_d3/0x00429a30.md', 're/analysis/hud_frontend_d3/0x00429a30.md']),
        ('0x00429a70', ['.worktrees/batch-q-s15/re/analysis/hud_frontend_d3/0x00429a70.md', 're/analysis/hud_frontend_d3/0x00429a70.md']),
        ('0x00429a80', ['.worktrees/batch-q-s15/re/analysis/hud_frontend_d3/0x00429a80.md', 're/analysis/hud_frontend_d3/0x00429a80.md']),
    ]),
    ('s16', [
        ('0x00474890', ['.worktrees/batch-q-s16/re/analysis/hud_frontend_d3/0x00474890.md']),
        ('0x00429a90', None),
        ('0x0042e8b0', ['.worktrees/batch-q-s16/re/analysis/frontend_unmapped_a/0x0042e8b0.md', 're/analysis/frontend_unmapped_a/0x0042e8b0.md']),
        ('0x00427e00', ['.worktrees/batch-q-s16/re/analysis/hud_frontend/0x00427e00.md', 're/analysis/hud_frontend/0x00427e00.md']),
    ]),
]

ok = 0; missing_file = 0; drift_skip = 0; no_text = 0

for session, rvalist in SESSIONS:
    for rva, paths in rvalist:
        if paths is None:
            drift_skip += 1
            print(f'SKIP|{session}|{rva}|DRIFT-SKIP|')
            continue
        fp = find_file(paths)
        if fp is None:
            missing_file += 1
            print(f'MISSING_FILE|{session}|{rva}||{paths[0]}')
            continue
        text = extract_plate(fp)
        if not text:
            no_text += 1
            print(f'NO_TEXT|{session}|{rva}||{fp}')
        else:
            ok += 1
            print(f'OK|{session}|{rva}|{text}|{fp}')

import sys
print(f'\nSUMMARY: ok={ok} missing_file={missing_file} no_text={no_text} drift_skip={drift_skip}', file=sys.stderr)
