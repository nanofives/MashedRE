"""run 40: append the ORIGTOP-ONLY pair suggestions of a build log to re/tools/dual_copy_allowlist.txt (only lines the
lint itself marks `[ORIGTOP-ONLY pair: ...]`; any other finding is left for a person). usage: allow_origtop.py <log>"""
import re
import sys
C = ('# RWC-PLUGIN-ATTACH run 40: RW core body on the MASHED_RWC_CORE knob path (closure of the librw binding RVAs, '
     'MASHED_RWC_FRAME); verify/rwccorerun_20261111/RESULT_RWC-PLUGIN-ATTACH.md')
out = []
for ln in open(sys.argv[1], encoding='latin-1'):
    m = re.search(r'ERROR (DUP-IN-TARGET|CROSS-TARGET) 0x([0-9a-f]{8})', ln)
    if m and 'ORIGTOP-ONLY pair' in ln:
        files = sorted(set(re.findall(r'([A-Za-z0-9_]+/[A-Za-z0-9_]+\.cpp)', ln)))
        out.append('ORIGTOP-ONLY %s:%s:%s   %s' % (m.group(1), m.group(2), ','.join(files), C))
if out:
    open('re/tools/dual_copy_allowlist.txt', 'a', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
print('allowlisted %d ORIGTOP-ONLY pair(s)' % len(out))
