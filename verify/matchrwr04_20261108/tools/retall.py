import csv, subprocess, sys
rows = list(csv.DictReader(open('D:/MashedWT/MATCH-RWLIB-2_data/R04.tsv'), delimiter='\t'))
void = sys.argv[1].split(',')
for r in rows:
    if r['rva'] not in void:
        continue
    src = r['source'].replace(chr(92), '/')
    out = subprocess.run([sys.executable, 'D:/MashedWT/MATCH-RWR04_data/tools/retv.py', src, '0x' + r['rva']],
                         capture_output=True, text=True)
    print('==', r['best_pct'], r['rva'], flush=True)
    print(out.stdout.strip(), out.stderr.strip()[-200:], flush=True)
