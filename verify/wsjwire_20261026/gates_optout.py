r"""FLIP (L-27 (a)) opt-out gate: run verify/savelink_20261023/run_arms.py unchanged except that its clean_env() also
sets MASHED_NO_AUDIO_WIRE=1 (run_arms strips every MASHED_* variable from the parent environment, so the plain runner
measures the new default, wire ON). Same in-memory patch shape as gates_knob.py.
usage: py -3.12 gates_optout.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
"""
import os, sys
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
RUNNER = os.path.join(W, 'verify', 'savelink_20261023', 'run_arms.py')
src = open(RUNNER, encoding='utf-8').read()
old = "    e['MASHED_ROOT'] = ROOT\n    return e\n"
assert src.count(old) == 1, 'run_arms.py clean_env() changed; re-check the patch'
src = src.replace(old, "    e['MASHED_ROOT'] = ROOT\n    e['MASHED_NO_AUDIO_WIRE'] = '1'   # D4-WSJ-WIRE flip opt-out\n    return e\n")
sys.argv = [RUNNER] + sys.argv[1:]
g = {'__name__': '__main__', '__file__': RUNNER}
exec(compile(src, RUNNER, 'exec'), g)
