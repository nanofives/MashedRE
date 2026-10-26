r"""D4-FLIP-L29-L30 gate wrapper: runs verify/savelink_20261023/run_arms.py unchanged, plus one extra knob stack
OPT0 = both flipped knobs opted out (MASHED_AI_VEHICLES=0, MASHED_VIDEOCFG_WRITE=0). Arms: H_DEF B_DEF_K* H_OPT0 B_OPT0_K*.
"""
import os, sys
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
P = os.path.join(W, 'verify', 'savelink_20261023', 'run_arms.py')
src = open(P, encoding='utf-8').read()
hook = "for name in sys.argv[1:]:"
assert src.count(hook) == 1
src = src.replace(hook, "STACK['OPT0'] = ['MASHED_AI_VEHICLES=0', 'MASHED_VIDEOCFG_WRITE=0']\n" + hook)
exec(compile(src, P, 'exec'), {'__file__': P, '__name__': '__main__'})
