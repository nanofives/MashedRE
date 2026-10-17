"""Move the lane's bulk evidence (arm dumps not cited in RESULT, every .msd, latbracket CSVs) to D:\\MashedWT\\d3bB2_data\\verify,
keeping the cited files in git. sha8 of every moved arm output is in the RUNLOG.txt files that stay. usage: py -3.12 move_evidence.py"""
import fnmatch, os, shutil

HERE = os.path.dirname(os.path.abspath(__file__))
DST = r'D:\MashedWT\d3bB2_data\verify'
KEEP = ['*RUNLOG.txt', 'batch*.txt', 'SCORE*.txt',
        'arms/B_DEF_K1.step.csv', 'arms/B_DEF_K4.step.csv', 'arms/rep2/B_DEF_K1.step.csv',
        'arms/knob/B_CC_K0.step.csv', 'arms/knob/B_CC_K1.step.csv', 'arms/knob/B_CC_K26.step.csv',
        'arms/knob/B_CCBL_K1.cc.txt', 'arms/knob/B_CCBDL_K1.cc.txt', 'arms/knob/B_CCBDL_K1.step.csv',
        'arms/k3/B_SL_K1.*', 'arms/k3/B_EL_K1.*', 'arms/k3/B_EL_K0.ev.txt', 'arms/k3/B_DEFL_K1.cc.txt',
        'arms/probe/B_SLP_K1.pathprog.csv',
        'arms/final/B_E_K1.step.csv', 'arms/final/B_S_K1.step.csv', 'arms/final/B_CC*_K1.step.csv',
        'arms/d2/*.motion_diag.log']
moved = kept = 0
for root, _, files in os.walk(HERE):
    for f in files:
        rel = os.path.relpath(os.path.join(root, f), HERE).replace(os.sep, '/')
        if not (rel.startswith('arms/') or rel.endswith('.msd') or rel.endswith('latbracket.csv')):
            continue
        if not rel.endswith('.msd') and any(fnmatch.fnmatch(rel, k) or fnmatch.fnmatch(f, k) for k in KEEP):
            kept += 1
            continue
        d = os.path.join(DST, rel)
        os.makedirs(os.path.dirname(d), exist_ok=True)
        shutil.move(os.path.join(HERE, rel), d)
        moved += 1
print('moved', moved, 'kept', kept)
