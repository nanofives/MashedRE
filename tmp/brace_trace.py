src = open('re/frida/hooks_registry.py', encoding='utf-8').read()


def strip(s):
    out = []
    i = 0
    n = len(s)
    while i < n:
        c = s[i]
        if c == '#':
            while i < n and s[i] != '\n':
                i += 1
            continue
        if c in ('"', "'"):
            q = c
            if s[i:i+3] == q*3:
                i += 3
                while i < n and s[i:i+3] != q*3:
                    i += 1
                i += 3
                continue
            i += 1
            while i < n and s[i] != q:
                if s[i] == '\\':
                    i += 2
                else:
                    i += 1
            i += 1
            continue
        out.append(c)
        i += 1
    return ''.join(out)


depth = 0
prev = 0
# print delta for each non-trivial line
for ln, line in enumerate(src.split('\n'), 1):
    cleaned = strip(line)
    open_n = cleaned.count('{')
    close_n = cleaned.count('}')
    delta = open_n - close_n
    depth += delta
    if delta != 0:
        # report when depth goes up but the close doesn't follow soon
        marker = '+'+str(delta) if delta > 0 else str(delta)
        # only print lines where the indent is at dict-top level (1 dict open) to find anomalies
        if ln > 62 and ln < 200 or (depth > 2 and ln < 200):
            pass  # skip noise
    prev = depth

# Find runs where depth is non-monotonic — also print the location of the highest depth
max_depth = 0
max_at = 0
depth = 0
for ln, line in enumerate(src.split('\n'), 1):
    cleaned = strip(line)
    delta = cleaned.count('{') - cleaned.count('}')
    depth += delta
    if depth > max_depth:
        max_depth = depth
        max_at = ln

print(f'max depth: {max_depth} at line {max_at}')
# print the trailing 200 lines' depth running
print()
print('--- last 30 transitions ---')
depth = 0
transitions = []
for ln, line in enumerate(src.split('\n'), 1):
    cleaned = strip(line)
    delta = cleaned.count('{') - cleaned.count('}')
    new = depth + delta
    if delta != 0:
        transitions.append((ln, depth, new, line.rstrip()[:100]))
    depth = new
for ln, p, n, txt in transitions[-30:]:
    print(f'  line {ln:5d}: depth {p} -> {n}    {txt!r}')
