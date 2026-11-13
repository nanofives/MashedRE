"""RWC-PLUGIN-ATTACH: append ORIGTOP-ONLY allowlist lines for the NEW rva-lint findings of a build log.

usage: py -3.12 verify/rwccorerun_20261111/allowlist_from_log.py <build.log>
Only pairs where one body is an RwCore/ TU (the knob-path RW core closure) are appended; any other NEW finding is
printed and left alone.
"""
import re
import sys

ALLOW = 're/tools/dual_copy_allowlist.txt'
CIT = ('   # RWC-PLUGIN-ATTACH (L-124 C step 3): RW core body on the MASHED_RWC_CORE knob path (plugin attach closure); '
       'verify/rwccorerun_20261111/RESULT_RWC-PLUGIN-ATTACH.md')

log = open(sys.argv[1], encoding='utf-8', errors='replace').read()
have = open(ALLOW, encoding='utf-8').read()
new, other = [], []
for m in re.finditer(r'ERROR DUP-IN-TARGET 0x([0-9a-f]{8})  \[exe\] two bodies: (\S+):\d+ \([^)]*\), (\S+):\d+', log):
    r, a, b = m.groups()
    (new if 'RwCore/' in a + b else other).append('ORIGTOP-ONLY DUP-IN-TARGET:%s:%s' % (r, ','.join(sorted([a, b]))))
for m in re.finditer(r'ERROR CROSS-TARGET 0x([0-9a-f]{8})  exe-only (\S+)  vs  asi-only (\S+) ', log):
    r, a, b = m.groups()
    (new if 'RwCore/' in a + b else other).append('ORIGTOP-ONLY CROSS-TARGET:%s:%s' % (r, ','.join(sorted([a, b]))))
new = [k for k in dict.fromkeys(new) if k not in have]
if new:
    s = have if have.endswith('\n') else have + '\n'
    open(ALLOW, 'w', encoding='utf-8', newline='\n').write(s + '\n'.join(k + CIT for k in new) + '\n')
print('appended %d; not RwCore (left alone): %d' % (len(new), len(other)))
for o in other:
    print('  ', o)
