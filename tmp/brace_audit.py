import sys

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
last_zero = 0
last_pos = 0
for ln, line in enumerate(src.split('\n'), 1):
    cleaned = strip(line)
    open_n = cleaned.count('{')
    close_n = cleaned.count('}')
    delta = open_n - close_n
    depth += delta
    if depth == 0:
        last_zero = ln

print(f'final depth: {depth}')
print(f'last line where depth returned to 0: {last_zero}')
