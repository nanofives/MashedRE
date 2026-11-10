"""HB-RUNARMS-STACKFILES: dump run_arms.py's resolved STACK (sorted keys, JSON) without running an arm.
usage: py -3.12 dump_stack.py <run_arms.py> <out.json>"""
import json, os, runpy, sys
src, out = sys.argv[1], sys.argv[2]
os.environ.pop('A4_BATCH_KNOBS', None)
sys.argv = [src]   # no arms: the arm loop does nothing
g = runpy.run_path(src, run_name='__dump__')
with open(out, 'w', newline='\n') as f:
    json.dump(g['STACK'], f, sort_keys=True, indent=1)
    f.write('\n')
print(len(g['STACK']), 'stacks ->', out)
