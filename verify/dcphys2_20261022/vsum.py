"""Verdict-only D3 (b) summary: one line per car (regime, PASS / failing items with value and envelope), per file.
Same rule as percar.py (ai_ctrl_window.check_regime on re/tools/b_envelopes.json). Prints NOT SCORED explicitly.
usage: py -3.12 vsum.py <step.csv> [...]"""
import json, os, sys, importlib.util
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
spec = importlib.util.spec_from_file_location("w", os.path.join(W, "re", "tools", "ai_ctrl_window.py"))
w = importlib.util.module_from_spec(spec); spec.loader.exec_module(w)
ENV = json.load(open(os.path.join(W, r"re\tools\b_envelopes.json")))
for p in sys.argv[1:]:
    cr = w.check_regime(p, ENV)
    parts, npass = [], 0
    for v in (1, 2, 3):
        r = cr.get(v)
        if r is None:
            parts.append('car%d unlabelled' % v); continue
        f = r['fails']
        if f is None:
            parts.append('car%d %s NOT SCORED (%s)' % (v, r['regime'], r.get('why', ''))); continue
        npass += not f
        parts.append('car%d %s %s' % (v, r['regime'], 'PASS' if not f else 'FAIL[' + '; '.join(x.replace(' not in ', ' ') for x in f) + ']'))
    print('%-22s %d/3  %s' % (os.path.basename(p).replace('.step.csv', ''), npass, ' | '.join(parts)))
