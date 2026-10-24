r"""U-9782: count car-car contact calls per FUN_004709a0 inner loop in a MASHED_CARCAR_LOG file.

Each log line is one VehicleCarCarContact entry (VehiclePhysicsRun.cpp, the j = i+1..15 loop):
    i=<self> j=<other> pass=<ccPass at the call> ret=<0|1> ... fr=<race frame>
ccPass is the port twin of the original's ebp at `push ebp` 0x00470bc3: it starts at the
substep's pass and is incremented after every ret-1 call (0x00470bde inc ebp).

A loop is a run of lines with the same (fr, i) and strictly increasing j. A loop is a pass-0
loop when its first call has pass=0. [UNCERTAIN] two loops of the same (fr, i) whose j sets do
not overlap would merge into one run (the log has no substep id), so `merged-run` counts are an
upper bound. The headline count (pass-0 loops with two ret-1 calls) is not affected by that
merge: a second ret-1 call would need pass=0 after an earlier ret-1, and ccPass is >= 1 there.

usage: py -3.12 count_cc.py <cc.txt> [...]
"""
import re, sys

PAT = re.compile(r'i=(-?\d+) j=(-?\d+) pass=(-?\d+) ret=(\d).* fr=(-?\d+)')


def loops(path):
    cur, key, lastj = [], None, None
    for ln in open(path, encoding='ascii', errors='replace'):
        m = PAT.search(ln)
        if not m:
            continue
        i, j, p, r, fr = (int(x) for x in m.groups())
        k = (fr, i)
        if cur and (k != key or j <= lastj):
            yield cur
            cur = []
        cur.append((i, j, p, r, fr))
        key, lastj = k, j
    if cur:
        yield cur


for path in sys.argv[1:]:
    calls = ret1 = ret1_nonzero_pass = 0
    p0 = p0_multi = p0_two_ret1 = p0_ret1_then_more = 0
    examples = []
    for lp in loops(path):
        calls += len(lp)
        ret1 += sum(1 for c in lp if c[3] == 1)
        ret1_nonzero_pass += sum(1 for c in lp if c[3] == 1 and c[2] != 0)
        if lp[0][2] != 0:
            continue
        p0 += 1
        if len(lp) >= 2:
            p0_multi += 1
        n1 = sum(1 for c in lp if c[3] == 1 and c[2] == 0)
        if n1 >= 2:
            p0_two_ret1 += 1
        first = next((x for x, c in enumerate(lp) if c[3] == 1), None)
        if first is not None and first + 1 < len(lp):
            p0_ret1_then_more += 1
            if len(examples) < 5:
                examples.append(lp)
    print(f'{path}')
    print(f'  calls={calls} ret1={ret1} ret1_with_pass!=0={ret1_nonzero_pass}')
    print(f'  pass0_loops={p0} pass0_loops_with>=2_calls={p0_multi} (merged-run upper bound)')
    print(f'  pass0_loops_with_two_ret1={p0_two_ret1}')
    print(f'  pass0_loops_with_a_call_after_the_first_ret1={p0_ret1_then_more} (merged-run upper bound)')
    for lp in examples:
        print('    e.g. ' + ' | '.join(f'fr={c[4]} i={c[0]} j={c[1]} pass={c[2]} ret={c[3]}' for c in lp))
