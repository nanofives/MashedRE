import csv, subprocess, json, sys
wt = sys.argv[1]
out = sys.argv[2]
rows = list(csv.DictReader(open(wt + '/hooks.csv', encoding='utf-8', newline='')))
tgt = {r['rva'] for r in rows if r['confidence'].strip() == 'C4' and 'c4=' not in (r['notes'] or '')}
print(len(tgt), flush=True)
p = subprocess.Popen(['git', '-C', wt, 'log', '--reverse', '--no-color', '-p', '-U0', '--format=@@C %h %cs', '--', 'hooks.csv'],
                     stdout=subprocess.PIPE, text=True, encoding='utf-8', errors='replace')
cur = None
state = {}   # rva -> is C4
promo = {}   # rva -> commit of latest transition to C4
n = 0
for line in p.stdout:
    if line.startswith('@@C '):
        cur = line.split()[1] + ' ' + line.split()[2]
        n += 1
        continue
    if not line or line[0] not in '+-' or line.startswith(('+++', '---')):
        continue
    rva = line[1:].split(',', 1)[0]
    if rva not in tgt:
        continue
    try:
        f = next(csv.reader([line[1:].rstrip('\n')]))
    except Exception:
        continue
    if len(f) < 4:
        continue
    isc4 = f[3].strip() == 'C4'
    if line[0] == '+':
        if isc4 and not state.get(rva):
            promo[rva] = cur
        state[rva] = isc4
    else:
        pass
print('commits', n, 'found', len(promo), flush=True)
json.dump(promo, open(out, 'w'))
