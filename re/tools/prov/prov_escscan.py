"""RO-BOOT-CRASH-3 (2026-11-12). usage: py -3.12 -I re/tools/prov/prov_escscan.py re/match
List C string literals in re/match sources whose backslash escapes are not one of the forms a match source
writes on purpose (\\n \\t \\r \\0 \\x.. \\\\ \\" \\'). A Windows path written with single backslashes compiles to
different bytes (\\p -> p, \\a -> 0x07) and match.py cannot see it (the literal is a relocated data operand)."""
import pathlib
import re
import sys

BS = chr(92)
root = pathlib.Path(sys.argv[1])
lit = re.compile('"((?:[^"' + BS * 2 + ']|' + BS * 2 + '.)*)"')
esc = re.compile(BS * 2 + '(.)')
ok = set('ntr0x' + BS + '"' + "'")
for f in sorted(root.rglob('*.c')):
    for i, line in enumerate(f.read_text(errors='replace').splitlines(), 1):
        s = line.lstrip()
        if s.startswith(('//', '/*', '*')):
            continue
        for body in lit.findall(line):
            bad = [m.group(1) for m in esc.finditer(body) if m.group(1) not in ok]
            if bad:
                print(f'{f.as_posix()}:{i}: "{body}"  escapes={bad}')
